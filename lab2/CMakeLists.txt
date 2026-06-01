cmake_minimum_required(VERSION 3.14)

project(OS_Lab2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(Main main.cpp)
add_executable(Tests tests.cpp)

enable_testing()
add_test(NAME ThreadLogicTests COMMAND Tests)