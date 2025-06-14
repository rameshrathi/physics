cmake_minimum_required(VERSION 3.21)
project(Lab CXX)

# Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Folders
add_subdirectory(Shared)
add_subdirectory(Physics)
add_subdirectory(Run)
add_subdirectory(Tests)
