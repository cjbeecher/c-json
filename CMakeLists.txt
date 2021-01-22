cmake_minimum_required(VERSION 3.15)
project(c_json C)

set(CMAKE_C_STANDARD 99)

add_library(json SHARED json.c json.h)