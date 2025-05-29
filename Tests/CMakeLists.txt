enable_testing()

file(GLOB TEST_SOURCES *.cpp)

foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE FS)
    target_include_directories(${test_name}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/FS
            ${CMAKE_SOURCE_DIR}/Main
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
