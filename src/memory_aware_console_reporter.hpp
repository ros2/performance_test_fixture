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

#ifndef MEMORY_AWARE_CONSOLE_REPORTER_HPP_
#define MEMORY_AWARE_CONSOLE_REPORTER_HPP_

#include <benchmark/benchmark.h>

#include <vector>

namespace performance_test_fixture
{

class MemoryAwareConsoleReporter : public benchmark::ConsoleReporter
{
public:
  explicit MemoryAwareConsoleReporter(
    benchmark::ConsoleReporter::OutputOptions opts_ = benchmark::ConsoleReporter::OO_Defaults);
  void ReportRuns(const std::vector<benchmark::BenchmarkReporter::Run> & reports) override;
};

}  // namespace performance_test_fixture

#endif  // MEMORY_AWARE_CONSOLE_REPORTER_HPP_
