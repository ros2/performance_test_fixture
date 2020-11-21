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

#ifndef MIMICK_UTILS_HPP_
#define MIMICK_UTILS_HPP_

#include <limits>
#include <memory>

#include "mimick/mimick.h"

template<typename Fn, Fn fn, class C, typename ... A>
typename std::result_of<Fn(C, A...)>::type
mmk_wrapper(A... a)
{
  struct mmk_stub * stub = mmk_ctx();
  assert(nullptr != stub);
  C * instance = static_cast<C *>(mmk_stub_context(stub));
  assert(nullptr != instance);
  return (instance->*fn)(a ...);
}

#define mmk_stub_create_wrapped(name, func, inst, ...) \
  mmk_stub_create( \
    name, \
    (mmk_fn) & mmk_wrapper< \
      decltype(&std::remove_reference<decltype(*inst)>::type::func), \
      &std::remove_reference<decltype(*inst)>::type::func, \
      std::remove_reference<decltype(*inst)>::type, \
      __VA_ARGS__>, \
    inst);

template<class T>
struct mmk_allocator
{
  typedef T value_type;

  mmk_allocator()
  {
    // TODO(cottsay): This is just a hack to get Mimick to initialize the "vital" functions.
    mmk_stub * stub = mmk_stub_create("free", (mmk_fn)dummy_free, nullptr);
    if (MMK_STUB_INVALID == stub) {
      abort();
    }
    mmk_stub_destroy(stub);
  }

  template<class U>
  constexpr mmk_allocator(const mmk_allocator<U> &) noexcept {}

  T * allocate(std::size_t n)
  {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      throw std::bad_alloc();
    }

    if (T * p = static_cast<T *>(mmk_malloc(n * sizeof(T)))) {
      return p;
    }

    throw std::bad_alloc();
  }

  void deallocate(T * p, std::size_t) noexcept
  {
    mmk_free(p);
  }

private:
  static void dummy_free(void * unused)
  {
    (void)unused;
  }
};

#endif  // MIMICK_UTILS_HPP_
