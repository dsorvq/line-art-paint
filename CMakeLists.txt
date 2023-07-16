cmake_minimum_required (VERSION 3.0)
project (line-art-paint)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-std=c++17)

include_directories(ext/stb include)
add_executable(
    ${CMAKE_PROJECT_NAME}
    src/main.cpp 
    src/stb.cpp
)

