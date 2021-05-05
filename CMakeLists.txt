cmake_minimum_required(VERSION 3.5)
project(performance_test_fixture CXX)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(BUILD_SHARED_LIBS TRUE CACHE BOOL "Build shared libraries")

# find dependencies
find_package(ament_cmake_core REQUIRED)
find_package(ament_cmake_export_dependencies REQUIRED)
find_package(ament_cmake_export_targets REQUIRED)
find_package(ament_cmake_test REQUIRED)
find_package(benchmark REQUIRED)
find_package(mimick_vendor REQUIRED)

# TODO(cottsay): It would be great if this didn't need to be STATIC, but
# libmimick is specifically compiled static and without PIC.
add_library(${PROJECT_NAME} STATIC
  src/mimick_memory_manager.cpp
  src/${PROJECT_NAME}.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME} PUBLIC
  benchmark::benchmark
  mimick)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  "PERFORMANCE_TEST_FIXTURE_BUILDING_DLL")

add_library(memory_aware_benchmark_main
  src/memory_aware_benchmark_main.cpp
  src/memory_aware_console_reporter.cpp)

target_include_directories(memory_aware_benchmark_main PRIVATE
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(memory_aware_benchmark_main PUBLIC
  benchmark::benchmark)

target_compile_definitions(memory_aware_benchmark_main PRIVATE
  "PERFORMANCE_TEST_FIXTURE_BUILDING_DLL")

install(TARGETS ${PROJECT_NAME} memory_aware_benchmark_main
  EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(
  DIRECTORY include/
  DESTINATION include)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME})

ament_export_targets(${PROJECT_NAME})
ament_export_dependencies(
  ament_cmake_google_benchmark
  benchmark
  mimick_vendor)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  set(performance_test_fixture_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
  add_library(${PROJECT_NAME}::memory_aware_benchmark_main ALIAS memory_aware_benchmark_main)
  include(performance_test_fixture-extras.cmake)

  add_performance_test(
    benchmark_malloc_realloc
    test/benchmark/benchmark_malloc_realloc.cpp
    TIMEOUT 120)

  add_performance_test(
    benchmark_pause_resume
    test/benchmark/benchmark_pause_resume.cpp)
endif()

ament_package(
  CONFIG_EXTRAS_POST "${PROJECT_NAME}-extras.cmake"
)
