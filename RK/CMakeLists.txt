# RK/CMakeLists

file(GLOB RK_SOURCES
        AtomicRefCount.cpp
        Atomic.cpp
        Helpers.cpp
        String.cpp
)

add_library(RK
        STATIC ${RK_SOURCES}
)

target_include_directories(
        RK PUBLIC
        ${CMAKE_SOURCE_DIR}
)