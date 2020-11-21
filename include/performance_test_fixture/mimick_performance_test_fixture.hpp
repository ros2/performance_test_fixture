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

#ifndef PERFORMANCE_TEST_FIXTURE__MIMICK_PERFORMANCE_TEST_FIXTURE_HPP_
#define PERFORMANCE_TEST_FIXTURE__MIMICK_PERFORMANCE_TEST_FIXTURE_HPP_

#include <benchmark/benchmark.h>

#include <memory>

#include "performance_test_fixture/common.hpp"
#include "performance_test_fixture/mimick_memory_manager.hpp"

namespace performance_test_fixture
{

class MimickPerformanceTest : public benchmark::Fixture
{
public:
  MimickPerformanceTest();

  void SetUp(benchmark::State & state) override;

  void TearDown(benchmark::State & state) override;

protected:
  void reset_heap_counters();

  void set_are_allocation_measurements_active(bool value);

  std::unique_ptr<MimickMemoryManager> memory_manager;
};

}  // namespace performance_test_fixture

#endif  // PERFORMANCE_TEST_FIXTURE__MIMICK_PERFORMANCE_TEST_FIXTURE_HPP_
