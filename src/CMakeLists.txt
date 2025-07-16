cmake_minimum_required(VERSION 3.10)

# BUG:
# set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

project(GLFun)

include_directories(lib)

include_directories(imgui)
include_directories(./imgui/misc/cpp)
file(GLOB imgui ./imgui/*.cpp)
list(APPEND imgui ./imgui/backends/imgui_impl_opengl3.cpp ./imgui/backends/imgui_impl_glfw.cpp ./imgui/misc/cpp/imgui_stdlib.cpp)

add_executable(glfun main.cc vertex.cc shader.cc texture.cc impl.cc ${imgui})
target_link_libraries(glfun glfw)
