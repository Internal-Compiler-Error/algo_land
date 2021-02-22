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
        include/binary_search_tree.h include/utils.h include/search.h)
set_target_properties(algo_and_data PROPERTIES LINKER_LANGUAGE CXX)
target_compile_options(algo_and_data PRIVATE
        ${common_warnings} ${common_features})
target_include_directories(algo_and_data PUBLIC include/)

add_executable(algo_land main.cpp)
target_compile_options(algo_land PRIVATE
        ${common_warnings} ${common_features})
target_link_libraries(algo_land PRIVATE algo_and_data)
