cmake_minimum_required(VERSION 3.0...3.20 FATAL_ERROR)

project(Glad VERSION 3.3.10 LANGUAGES C)

add_library(glad src/glad.c)

target_include_directories(glad PUBLIC include)
