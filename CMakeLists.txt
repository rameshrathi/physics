cmake_minimum_required(VERSION 3.12)
project(Lab, CXX)

# Standard
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

# FS
add_subdirectory(FS)
add_subdirectory(Main)
