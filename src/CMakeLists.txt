cmake_minimum_required(VERSION 3.10)
project(GLFun)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

include_directories(lib)

add_executable(glfun main.cc)
add_library(vertex vertex.cc)
add_library(shader shader.cc)
add_library(texture texture.cc)
add_library(impl impl.cc)

target_link_libraries(glfun shader vertex texture impl glfw)
