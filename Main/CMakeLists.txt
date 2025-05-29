set(LAB_SOURCES
    main.cpp
)

add_executable(
    LAB
    ${LAB_SOURCES}
)

target_include_directories(
    LAB
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)