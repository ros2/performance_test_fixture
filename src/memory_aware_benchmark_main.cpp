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

#include <benchmark/benchmark.h>

#include "performance_test_fixture/visibility_control.hpp"
#include "./memory_aware_console_reporter.hpp"

constexpr benchmark::ConsoleReporter::OutputOptions output_options =
  benchmark::ConsoleReporter::OO_None;

PERFORMANCE_TEST_FIXTURE_PUBLIC
int main(int argc, char * argv[])
{
  benchmark::Initialize(&argc, argv);

  if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
    return 1;
  }

  performance_test_fixture::MemoryAwareConsoleReporter display_reporter(output_options);
  benchmark::RunSpecifiedBenchmarks(&display_reporter);

  return 0;
}
