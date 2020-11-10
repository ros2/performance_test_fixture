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

#ifndef PERFORMANCE_TEST_FIXTURE__PERFORMANCE_TEST_FIXTURE_HPP_
#define PERFORMANCE_TEST_FIXTURE__PERFORMANCE_TEST_FIXTURE_HPP_

#include <benchmark/benchmark.h>
#include <osrf_testing_tools_cpp/memory_tools/memory_tools.hpp>

#include "performance_test_fixture/visibility_control.hpp"

namespace performance_test_fixture
{

class PerformanceTest : public ::benchmark::Fixture
{
public:
  PERFORMANCE_TEST_FIXTURE_PUBLIC
  PerformanceTest();

  PERFORMANCE_TEST_FIXTURE_PUBLIC
  void SetUp(::benchmark::State & state);

  PERFORMANCE_TEST_FIXTURE_PUBLIC
  void TearDown(::benchmark::State & state);

protected:
  PERFORMANCE_TEST_FIXTURE_PUBLIC
  void on_malloc(osrf_testing_tools_cpp::memory_tools::MemoryToolsService & service);

  PERFORMANCE_TEST_FIXTURE_PUBLIC
  void on_realloc(osrf_testing_tools_cpp::memory_tools::MemoryToolsService & service);

  PERFORMANCE_TEST_FIXTURE_PUBLIC
  void reset_heap_counters();

  PERFORMANCE_TEST_FIXTURE_PUBLIC
  void pause_performance_measurements(::benchmark::State & state);

  PERFORMANCE_TEST_FIXTURE_PUBLIC
  void resume_performance_measurements(::benchmark::State & state);

private:
  size_t allocation_count;
  bool suppress_memory_tools_logging;
  bool are_performance_measurements_active;
};

}  // namespace performance_test_fixture

#endif  // PERFORMANCE_TEST_FIXTURE__PERFORMANCE_TEST_FIXTURE_HPP_
