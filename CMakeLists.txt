cmake_minimum_required(VERSION 3.10)

project(sbe VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the executable
add_executable(sbe main.cpp)

# Set compiler warnings
if(MSVC)
    target_compile_options(sbe PRIVATE /W4)
else()
    target_compile_options(sbe PRIVATE -Wall -Wextra -Wpedantic)
endif()
