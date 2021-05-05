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

#ifndef PERFORMANCE_TEST_FIXTURE__MIMICK_MEMORY_MANAGER_HPP_
#define PERFORMANCE_TEST_FIXTURE__MIMICK_MEMORY_MANAGER_HPP_

#include <benchmark/benchmark.h>

#include <functional>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <utility>

// Defined by mimick.h
struct mmk_stub;

// Defined by mimick_utils.hpp
template<class T>
struct mmk_allocator;

namespace performance_test_fixture
{

class MimickMemoryManager : public benchmark::MemoryManager
{
public:
  MimickMemoryManager();

  ~MimickMemoryManager();

  virtual void Pause();

  virtual void Reset();

  virtual void Resume();

  void Start() override;

  void Stop(benchmark::MemoryManager::Result * result) override;

private:
  void Stop() noexcept;

  void * on_calloc(size_t nitems, size_t size);
  void on_free(void * ptr);
  void * on_malloc(size_t size);
  void * on_realloc(void * ptr, size_t size);

  std::mutex stat_lock;
  bool recording_enabled;

  decltype(benchmark::MemoryManager::Result::max_bytes_used) cur_bytes_used;
  decltype(benchmark::MemoryManager::Result::max_bytes_used) max_bytes_used;
  decltype(benchmark::MemoryManager::Result::num_allocs) num_allocs;

  template<typename T>
  using mmk_unordered_set = std::unordered_set<T, std::hash<T>, std::equal_to<T>, mmk_allocator<T>>;
  std::unique_ptr<mmk_unordered_set<void *>> ptr_set;

  struct mmk_stub * calloc_stub;
  struct mmk_stub * free_stub;
  struct mmk_stub * malloc_stub;
  struct mmk_stub * realloc_stub;
};

}  // namespace performance_test_fixture

#endif  // PERFORMANCE_TEST_FIXTURE__MIMICK_MEMORY_MANAGER_HPP_
