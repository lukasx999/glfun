cmake_minimum_required(VERSION 3.3)
project(CubeGL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(cube main.cc)
add_library(shader shader.cc)

target_link_libraries(cube glfw shader)
