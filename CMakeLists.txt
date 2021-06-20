cmake_minimum_required(VERSION 3.16)
project(Timers VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(remove_heap_test src/remove_heap_test.cpp)
add_executable(timerheap_with_test src/Timer.cpp)
