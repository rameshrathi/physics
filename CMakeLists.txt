cmake_minimum_required(VERSION 3.10)
project(Lab, CXX)

# Standard
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

# Folders
add_subdirectory(FS)
add_subdirectory(Main)
add_subdirectory(Tests)