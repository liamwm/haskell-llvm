# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(CompilerProject VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(pegtl CONFIG REQUIRED)

# Add the executable target with the required source files
add_executable(compiler main.cpp HaskellNode.hpp)

target_link_libraries(compiler PRIVATE taocpp::pegtl)

# Add compiler flags (equivalent to -Wall)
target_compile_options(compiler PRIVATE -Wall)