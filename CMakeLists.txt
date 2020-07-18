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
find_package(ament_cmake_export_targets REQUIRED)
find_package(benchmark REQUIRED)
find_package(osrf_testing_tools_cpp REQUIRED)

add_library(${PROJECT_NAME}
  src/${PROJECT_NAME}.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME} PUBLIC
  benchmark::benchmark
  osrf_testing_tools_cpp::memory_tools)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  "PERFORMANCE_TEST_FIXTURE_BUILDING_DLL")

install(TARGETS ${PROJECT_NAME}
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

ament_package(
  CONFIG_EXTRAS_POST "${PROJECT_NAME}-extras.cmake"
)
