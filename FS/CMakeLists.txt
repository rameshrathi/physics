file(GLOB FS_SOURCES
    FileSystem.cpp
    FileSystem.h
)

add_library(
    FS
    STATIC
    ${FS_SOURCES}
)

target_include_directories(
    FS PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)