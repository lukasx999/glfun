cmake_minimum_required(VERSION 3.10)
project(CubeGL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_C_COMPILER "cc")
set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-c++") # Windows

include_directories("./glfw_win/include")
link_directories("./glfw_win/lib-mingw-w64")

add_executable(cube main.cc)
add_library(shader shader.cc)

target_link_libraries(cube shader glfw3 gdi32 stdc++exp) # Windows
# target_link_libraries(cube shader glfw3) # Linux
