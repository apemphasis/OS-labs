cmake_minimum_required(VERSION 3.14)

project(OS_Lab4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(Main main.cpp "utilities.h")
add_executable(Sender sender.cpp "utilities.h")
add_executable(Tests tests.cpp "utilities.h")

enable_testing()
add_test(NAME ThreadLogicTests COMMAND Tests)