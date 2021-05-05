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

#include <vector>

#include "./memory_aware_console_reporter.hpp"

namespace performance_test_fixture
{

MemoryAwareConsoleReporter::MemoryAwareConsoleReporter(
  benchmark::ConsoleReporter::OutputOptions opts_)
: benchmark::ConsoleReporter(opts_)
{
}

void MemoryAwareConsoleReporter::ReportRuns(
  const std::vector<benchmark::BenchmarkReporter::Run> & reports)
{
  std::vector<benchmark::BenchmarkReporter::Run> augmented_reports(reports.begin(), reports.end());

  for (std::vector<benchmark::BenchmarkReporter::Run>::iterator it = augmented_reports.begin();
    it != augmented_reports.end();
    it++)
  {
    if (it->has_memory_result) {
      *it = benchmark::BenchmarkReporter::Run(*it);
      it->counters["allocs_per_iter"] =
        benchmark::Counter(static_cast<double>(it->allocs_per_iter));
      it->counters["max_bytes_used"] =
        benchmark::Counter(static_cast<double>(it->max_bytes_used));
    }
  }

  benchmark::ConsoleReporter::ReportRuns(augmented_reports);
}

}  // namespace performance_test_fixture
