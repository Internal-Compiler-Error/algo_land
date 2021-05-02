cmake_minimum_required(VERSION 3.17)
project(algo_land)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(common_warnings
        -Wall
        -Wextra
        -Wshadow
        -Wnon-virtual-dtor
        -Wpedantic
        -Wcast-align
        -Wunused
        -Wmisleading-indentation)
set(common_features
        -fcoroutines
        -fdiagnostics-color=always)


add_library(algo_and_data
        include/sort.h
        include/priority_queue.h
        include/map.h include/utils.h include/search.h)
set_target_properties(algo_and_data PROPERTIES LINKER_LANGUAGE CXX)
target_compile_options(algo_and_data PRIVATE
        ${common_warnings} ${common_features})
target_include_directories(algo_and_data PUBLIC include/)

enable_testing()
find_package(Catch2 CONFIG REQUIRED)
include(Catch)

add_library(Catch2Main test/catch2_main.cpp)
target_link_libraries(Catch2Main PUBLIC Catch2::Catch2)

set(tests
        test/map_test.cpp
        test/priority_queue_test.cpp)

foreach (test ${tests})
    # hack of hacks, jank of janks solution to turn /test/name.cpp to name.cpp
    string(REGEX MATCH "[A-z0-9]+\\.cpp$" test_name_temp ${test})
    string(REGEX MATCH "[A-z0-9]+" test_name ${test_name_temp})
    message(STATUS "adding test ${test_name}")
    add_executable(${test_name} ${test})
    target_link_libraries(${test_name} PRIVATE Catch2Main algo_and_data)
    catch_discover_tests(${test_name})
endforeach ()
