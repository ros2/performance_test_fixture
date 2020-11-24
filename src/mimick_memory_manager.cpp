// Copyright 2020 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "performance_test_fixture/mimick_memory_manager.hpp"

#include <cstring>
#include <functional>
#include <iostream>
#include <unordered_set>
#include <utility>

#include "mimick/mimick.h"
#include "./mimick_utils.hpp"

namespace performance_test_fixture
{

MimickMemoryManager::MimickMemoryManager()
: recording_enabled(true),
  cur_bytes_used(0),
  max_bytes_used(0),
  num_allocs(0),
  ptr_set(new std::unordered_set<void *, std::hash<void *>, std::equal_to<void *>,
    mmk_allocator<void *>>()),
  calloc_stub(MMK_STUB_INVALID),
  free_stub(MMK_STUB_INVALID),
  malloc_stub(MMK_STUB_INVALID),
  realloc_stub(MMK_STUB_INVALID)
{
}

MimickMemoryManager::~MimickMemoryManager()
{
  Stop();
}

void MimickMemoryManager::Pause()
{
  stat_lock.lock();

  recording_enabled = false;

  stat_lock.unlock();
}

void MimickMemoryManager::Reset()
{
  stat_lock.lock();

  cur_bytes_used = 0;
  max_bytes_used = 0;
  num_allocs = 0;
  ptr_set->clear();

  stat_lock.unlock();
}

void MimickMemoryManager::Resume()
{
  stat_lock.lock();

  recording_enabled = true;

  stat_lock.unlock();
}

void MimickMemoryManager::Start()
{
  stat_lock.lock();

  Stop();

  recording_enabled = true;

  cur_bytes_used = 0;
  max_bytes_used = 0;
  num_allocs = 0;
  ptr_set->clear();

  calloc_stub = mmk_stub_create_wrapped("calloc", on_calloc, this, size_t, size_t);
  if (MMK_STUB_INVALID == calloc_stub) {
    std::cerr << "Failed to create 'calloc' stub!" << std::endl;
    exit(1);
  }

  free_stub = mmk_stub_create_wrapped("free", on_free, this, void *);
  if (MMK_STUB_INVALID == free_stub) {
    std::cerr << "Failed to create 'free' stub!" << std::endl;
    exit(1);
  }

  malloc_stub = mmk_stub_create_wrapped("malloc", on_malloc, this, size_t);
  if (MMK_STUB_INVALID == malloc_stub) {
    std::cerr << "Failed to create 'malloc' stub!" << std::endl;
    exit(1);
  }

  realloc_stub = mmk_stub_create_wrapped("realloc", on_realloc, this, void *, size_t);
  if (MMK_STUB_INVALID == realloc_stub) {
    std::cerr << "Failed to create 'realloc' stub!" << std::endl;
    exit(1);
  }

  stat_lock.unlock();
}

void MimickMemoryManager::Stop(benchmark::MemoryManager::Result * result)
{
  stat_lock.lock();

  Stop();

  if (nullptr != result) {
    result->max_bytes_used = max_bytes_used;
    result->num_allocs = num_allocs;
  }

  stat_lock.unlock();
}

void MimickMemoryManager::Stop() noexcept
{
  if (MMK_STUB_INVALID != calloc_stub) {
    mmk_stub_destroy(calloc_stub);
    calloc_stub = MMK_STUB_INVALID;
  }

  if (MMK_STUB_INVALID != free_stub) {
    mmk_stub_destroy(free_stub);
    free_stub = MMK_STUB_INVALID;
  }

  if (MMK_STUB_INVALID != malloc_stub) {
    mmk_stub_destroy(malloc_stub);
    malloc_stub = MMK_STUB_INVALID;
  }

  if (MMK_STUB_INVALID != realloc_stub) {
    mmk_stub_destroy(realloc_stub);
    realloc_stub = MMK_STUB_INVALID;
  }
}

void * MimickMemoryManager::on_calloc(size_t nitems, size_t size)
{
  stat_lock.lock();
  void * new_ptr = mmk_malloc(sizeof(size_t) + (nitems * size));

  if (nullptr != new_ptr) {
    std::memset(new_ptr, 0x0, nitems * size);
  }

  if (recording_enabled) {
    num_allocs++;
    if (nullptr != new_ptr) {
      cur_bytes_used += nitems * size;
      *static_cast<size_t *>(new_ptr) = nitems * size;
      new_ptr = static_cast<size_t *>(new_ptr) + 1;
      ptr_set->emplace(new_ptr);
      if (cur_bytes_used > max_bytes_used) {
        max_bytes_used = cur_bytes_used;
      }
    }
  }

  stat_lock.unlock();

  return new_ptr;
}

void MimickMemoryManager::on_free(void * ptr)
{
  stat_lock.lock();

  auto ph = ptr_set->find(ptr);
  if (ph != ptr_set->end()) {
    ptr = static_cast<size_t *>(ptr) - 1;
    cur_bytes_used -= *static_cast<size_t *>(ptr);
    ptr_set->erase(ph);
  }

  mmk_free(ptr);

  stat_lock.unlock();
}

void * MimickMemoryManager::on_malloc(size_t size)
{
  stat_lock.lock();
  void * new_ptr = mmk_malloc(sizeof(size_t) + size);

  if (recording_enabled) {
    num_allocs++;
    if (nullptr != new_ptr) {
      cur_bytes_used += size;
      *static_cast<size_t *>(new_ptr) = size;
      new_ptr = static_cast<size_t *>(new_ptr) + 1;
      ptr_set->emplace(new_ptr);
      if (cur_bytes_used > max_bytes_used) {
        max_bytes_used = cur_bytes_used;
      }
    }
  }

  stat_lock.unlock();

  return new_ptr;
}

void * MimickMemoryManager::on_realloc(void * ptr, size_t size)
{
  stat_lock.lock();

  auto ph = ptr_set->find(ptr);
  if (ph != ptr_set->end()) {
    // Assume that the realloc will succeed
    ptr = static_cast<size_t *>(ptr) - 1;
    cur_bytes_used -= *static_cast<size_t *>(ptr);
    ptr_set->erase(ph);
  }

  void * new_ptr = mmk_realloc(ptr, size);

  if ((nullptr == new_ptr && 0 != size)) {
    // Realloc failed - undo the removal
    cur_bytes_used += *static_cast<size_t *>(ptr);
    ptr = static_cast<size_t *>(ptr) + 1;
    ptr_set->emplace(ptr);
  }

  if (recording_enabled) {
    num_allocs++;
    if (nullptr != new_ptr) {
      cur_bytes_used += size;
      *static_cast<size_t *>(new_ptr) = size;
      new_ptr = static_cast<size_t *>(new_ptr) + 1;
      ptr_set->emplace(new_ptr);
      if (cur_bytes_used > max_bytes_used) {
        max_bytes_used = cur_bytes_used;
      }
    }
  }

  stat_lock.unlock();

  return new_ptr;
}

}  // namespace performance_test_fixture
