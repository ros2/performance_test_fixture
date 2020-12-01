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

#include "performance_test_fixture/performance_test_fixture.hpp"

namespace performance_test_fixture
{

/*
 * This is a lightweight MemoryManager wrapper that unregisters itself when it
 * is asked to stop recording. This is necessary so that when a benchmark
 * executable contains multiple tests and not all of them use this fixture,
 * the memory manager (which is stored in Google Benchmark as a global) isn't
 * registered despite the absence of this fixture.
 *
 * Since that behavior is specific to our use of a fixture to take care of
 * the MemoryManager registration, it isn't intrinsic to the MemoryManager
 * concept in general, which is why this class is implemented here.
 */
template<class U>
class AutoStopMemoryManager : public U
{
public:
  void Stop(benchmark::MemoryManager::Result * result) override
  {
    U::Stop(result);
    benchmark::RegisterMemoryManager(nullptr);
  }
};

PerformanceTest::PerformanceTest()
: memory_manager(new AutoStopMemoryManager<MimickMemoryManager>())
{
}

void PerformanceTest::SetUp(benchmark::State &)
{
  // Registering the MemoryManager doesn't mean that it will be used in the
  // current run. Benchmark does the timing measurements first, so this will
  // cause the MemoryManager to be registered during those runs, but it won't
  // be actually started until the registration is checked after the timing
  // runs are finished.
  benchmark::RegisterMemoryManager(memory_manager.get());
  memory_manager->Reset();
}

void PerformanceTest::TearDown(benchmark::State &)
{
  memory_manager->Pause();
}

void PerformanceTest::reset_heap_counters()
{
  memory_manager->Reset();
}

void PerformanceTest::set_are_allocation_measurements_active(bool value)
{
  if (value) {
    memory_manager->Resume();
  } else {
    memory_manager->Pause();
  }
}

}  // namespace performance_test_fixture
