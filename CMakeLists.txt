cmake_minimum_required(VERSION 4.0)
project(CubeGL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(cube main.cc)
add_library(shader shader.cc)

target_link_libraries(cube glfw shader)
