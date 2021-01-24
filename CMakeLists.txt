cmake_minimum_required(VERSION 3.15)
project(c_json C)

set(CMAKE_C_STANDARD 99)

add_library(json SHARED json.h init.c parse.c populate.c)

add_executable(json_test test.c)
target_link_libraries(json_test json)