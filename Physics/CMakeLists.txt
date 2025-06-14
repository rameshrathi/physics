# Physics/CMakeLists.txt

file(GLOB PHY_SOURCES
	World.cpp
	Shape.cpp
	Force.cpp
)

add_library(
    Physics
    STATIC
    ${PHY_SOURCES}
)

target_include_directories(
    Physics PUBLIC 
    ${CMAKE_SOURCE_DIR}
)
