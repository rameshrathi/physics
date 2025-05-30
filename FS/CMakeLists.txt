file(GLOB FS_SOURCES
    FileSystem.cpp
)

add_library(
    FS
    STATIC
    ${FS_SOURCES}
)

target_include_directories(
    FS PUBLIC 
    ${CMAKE_SOURCE_DIR}
)
