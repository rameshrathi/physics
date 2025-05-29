set(MAIN_SOURCES
    main.cpp
)

add_executable(
    LAB
    ${MAIN_SOURCES}
)

target_include_directories(
    LAB
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link FS to Executable
target_link_libraries(
    LAB
    PRIVATE FS
)