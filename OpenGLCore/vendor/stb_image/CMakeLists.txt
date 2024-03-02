cmake_minimum_required(VERSION 3.20)
project(StbImage VERSION 0.1.0 LANGUAGES C CXX)

add_library(stb_image src/stb_image.cpp)
target_include_directories(stb_image PUBLIC include)
