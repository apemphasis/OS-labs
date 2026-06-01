cmake_minimum_required(VERSION 3.10)
project(lab5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(Server server.cpp)
add_executable(Client client.cpp)

add_executable(TestRWMutex tests.cpp)

enable_testing()

add_test(NAME RWMutexTest COMMAND TestRWMutex)