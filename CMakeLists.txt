cmake_minimum_required(VERSION 3.21)
project(Lab CXX)

# Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Folders
add_subdirectory(RK)
add_subdirectory(FS)
add_subdirectory(Run)
add_subdirectory(Tests)