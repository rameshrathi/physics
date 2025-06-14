# Shared/CMakeLists

file(GLOB SHARED_SOURCES
        Atomic.cpp
)

add_library(Shared
	STATIC ${SHARED_SOURCES}
)

target_include_directories(
	Shared PUBLIC
        ${CMAKE_SOURCE_DIR}
)
