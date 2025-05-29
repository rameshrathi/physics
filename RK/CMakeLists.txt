# RK/CMakeLists

file(GLOB RK_SOURCES
        AtomicRefCount.cpp
)

add_library(RK
        STATIC ${RK_SOURCES}
)

target_include_directories(
        RK PUBLIC
        ${CMAKE_SOURCE_DIR}
)