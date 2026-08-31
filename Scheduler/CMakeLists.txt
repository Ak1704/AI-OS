cmake_minimum_required(VERSION 3.16)

project(aios_scheduler
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(aios-scheduler
    src/main.cpp
    src/scheduler.cpp
    src/policy.cpp
)

target_include_directories(aios-scheduler
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)


target_link_libraries(aios-scheduler
    PRIVATE
        Threads::Threads
)