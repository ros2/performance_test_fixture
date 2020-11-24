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

#include <cstdlib>
#include <string>

#include "./macros.h"
#include "performance_test_fixture/mimick_performance_test_fixture.hpp"

namespace
{

constexpr int kForcePerformanceTracking = 2;
constexpr int kEnablePerformanceTracking = 1;
constexpr int kDisablePerformanceTracking = 0;

}  // namespace

class PerformanceTestFixture : public performance_test_fixture::MimickPerformanceTest
{
public:
  void SetUp(benchmark::State & state)
  {
    switch (state.range(0)) {
      case kDisablePerformanceTracking:
        break;
      case kEnablePerformanceTracking:
        performance_test_fixture::MimickPerformanceTest::SetUp(state);
        break;
      case kForcePerformanceTracking:
        performance_test_fixture::MimickPerformanceTest::SetUp(state);
        memory_manager->Start();
        break;
      default:
        std::string message =
          std::string("Invalid enable/disable value: ") + std::to_string(state.range(0));
        state.SkipWithError(message.c_str());
        break;
    }
  }

  void TearDown(benchmark::State & state)
  {
    if (kEnablePerformanceTracking == state.range(0)) {
      performance_test_fixture::MimickPerformanceTest::TearDown(state);
    } else if (kForcePerformanceTracking == state.range(0)) {
      memory_manager->Pause();
    }
  }
};

// This does not make use of PauseTiming or ResumeTiming because timing is very short for these
// benchmarks. However, they should be enough to measure the overhead of turning on performance
// measurements.
BENCHMARK_DEFINE_F(PerformanceTestFixture, benchmark_on_malloc)(
  benchmark::State & state)
{
  const size_t malloc_size = state.range(1);
  if (malloc_size < 1) {
    state.SkipWithError("Size for allocation is too small for this test");
  }
  for (auto _ : state) {
    void * ptr = std::malloc(malloc_size);
    if (PERFORMANCE_TEST_FIXTURE_UNLIKELY(nullptr == ptr)) {
      state.SkipWithError("Malloc failed to malloc");
    }
    std::free(ptr);
    benchmark::DoNotOptimize(ptr);
    benchmark::ClobberMemory();
  }
}

BENCHMARK_DEFINE_F(PerformanceTestFixture, benchmark_on_malloc_many)(
  benchmark::State & state)
{
  const size_t malloc_size = state.range(1);
  if (malloc_size < 1) {
    state.SkipWithError("Size for allocation is too small for this test");
  }
  void * ptrs[4096];
  for (auto _ : state) {
    for (size_t i = 0; i < 4096; i++) {
      ptrs[i] = std::malloc(malloc_size);
      if (nullptr == ptrs[i]) {
        state.SkipWithError("Malloc failed to malloc");
      }
    }
    for (size_t i = 0; i < 4096; i++) {
      std::free(ptrs[i]);
    }
    benchmark::DoNotOptimize(ptrs);
    benchmark::ClobberMemory();
  }
}

BENCHMARK_DEFINE_F(PerformanceTestFixture, benchmark_on_realloc)(
  benchmark::State & state)
{
  const int64_t malloc_size = state.range(1);
  if (malloc_size < 1) {
    state.SkipWithError("Size for allocation is too small for this test");
  }
  const size_t realloc_size = state.range(2);
  if (realloc_size < 1) {
    state.SkipWithError("Size for reallocation is too small for this test");
  }
  for (auto _ : state) {
    void * ptr = std::malloc(malloc_size);
    if (PERFORMANCE_TEST_FIXTURE_UNLIKELY(nullptr == ptr)) {
      state.SkipWithError("Malloc failed to malloc");
    }
    void * realloc_ptr = std::realloc(ptr, realloc_size);
    if (PERFORMANCE_TEST_FIXTURE_UNLIKELY(nullptr == realloc_ptr)) {
      std::free(ptr);
      state.SkipWithError("Malloc failed to realloc");
    }
    std::free(realloc_ptr);
    benchmark::DoNotOptimize(realloc_ptr);
    benchmark::ClobberMemory();
  }
}

// Malloc sizes Range from 1 to 2^27 each time multiplying by 16. Each value tested
// with/without performance metrics
static void malloc_args(benchmark::internal::Benchmark * b)
{
  for (int64_t shift_left = 0; shift_left < 32; shift_left += 4) {
    b->Args({kDisablePerformanceTracking, 1ll << shift_left});
    b->Args({kEnablePerformanceTracking, 1ll << shift_left});
    b->Args({kForcePerformanceTracking, 1ll << shift_left});
  }
}

BENCHMARK_REGISTER_F(PerformanceTestFixture, benchmark_on_malloc)
->ArgNames({"Enable Performance Tracking", "Malloc Size"})->Apply(malloc_args);

BENCHMARK_REGISTER_F(PerformanceTestFixture, benchmark_on_malloc_many)
->ArgNames({"Enable Performance Tracking", "Malloc Size"})->Apply(malloc_args);

// Three types of realloc tests, one where malloc is smaller than realloc, one where they are
// the same, and one where malloc is larger than realloc. Realloc size ranges from 1 to 2^27
// each time multiplying by 32. Each stop is tested with/without performance metrics
static void realloc_args(benchmark::internal::Benchmark * b)
{
  for (int64_t malloc_adjustment = -1; malloc_adjustment <= 1; ++malloc_adjustment) {
    for (int64_t realloc_shift = 0; realloc_shift < 32; realloc_shift += 8) {
      const int64_t malloc_shift = realloc_shift + malloc_adjustment;
      if (malloc_shift < 0) {
        continue;
      }
      b->Args({kDisablePerformanceTracking, 1ll << malloc_shift, 1ll << realloc_shift});
      b->Args({kEnablePerformanceTracking, 1ll << malloc_shift, 1ll << realloc_shift});
      b->Args({kForcePerformanceTracking, 1ll << malloc_shift, 1ll << realloc_shift});
    }
  }
}

BENCHMARK_REGISTER_F(PerformanceTestFixture, benchmark_on_realloc)
->ArgNames({"Enable Performance Tracking", "Malloc Size", "Realloc Size"})
->Apply(realloc_args);