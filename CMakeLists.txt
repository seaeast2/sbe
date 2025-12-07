cmake_minimum_required(VERSION 3.10)
project(sbe VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SBE_SOURCES
    src/ir.cpp
    src/codegen.cpp
)

# Library
add_library(sbe_lib STATIC ${SBE_SOURCES})

# Example executable
add_executable(sbe_example examples/simple_example.cpp)
target_link_libraries(sbe_example sbe_lib)

# Tests
enable_testing()
add_executable(sbe_test tests/test_main.cpp)
target_link_libraries(sbe_test sbe_lib)
add_test(NAME sbe_test COMMAND sbe_test)
