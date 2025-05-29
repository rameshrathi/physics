set(MAIN_SOURCES
    main.cpp
)

add_executable(
    RUN
    ${MAIN_SOURCES}
)

target_include_directories(
        RUN
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link FS to Executable
target_link_libraries(
        RUN
        PRIVATE RK
        PRIVATE FS
)