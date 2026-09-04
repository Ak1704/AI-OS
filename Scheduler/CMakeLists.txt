cmake_minimum_required(VERSION 3.16)

project(aios_scheduler
    VERSION 0.2
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(scheduler_core
    src/scheduler.cpp
    src/policy.cpp
    src/ready_queue.cpp
    src/resource_state.cpp
    src/scheduler_state.cpp
    src/resource_manager.cpp
    src/cpu_manager.cpp
    src/linux_resource_manager.cpp
)

target_include_directories(
    scheduler_core
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(
    aios-scheduler
    src/main.cpp
    src/scheduler.cpp
    src/policy.cpp
    src/ready_queue.cpp
    src/resource_state.cpp
    src/scheduler_state.cpp
    src/resource_manager.cpp
    src/cpu_manager.cpp
    src/linux_resource_manager.cpp
)

target_link_libraries(
    aios-scheduler
    PRIVATE
    scheduler_core
)

enable_testing()

find_package(GTest REQUIRED)

add_executable(
    scheduler_tests
    tests/test_ready_queue.cpp
    tests/test_resource_state.cpp
    tests/test_scheduler.cpp
    tests/test_resource_manager.cpp
    tests/test_cpu_manager.cpp
    tests/test_linux_resource_manager.cpp
)

target_link_libraries(
    scheduler_tests
    PRIVATE
    scheduler_core
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(
    scheduler_tests
)