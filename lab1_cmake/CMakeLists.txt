cmake_minimum_required(VERSION 3.10)

project(OS_Lab1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(Creator creator.cpp)

add_executable(Reporter reporter.cpp)

add_executable(Main main.cpp)