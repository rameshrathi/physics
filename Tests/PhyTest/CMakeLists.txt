file(GLOB TEST_FILES *.cpp)

foreach(test_src ${TEST_FILES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} Shared)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
