project("test" VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory("util")

enable_testing()

function(setup_test test_source_file)
    get_filename_component(test_name ${test_source_file} NAME_WE)
    add_executable(${test_name} ${test_source_file})
    setup_target(${test_name})

    set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS ${custom_compile_flags})
    target_link_libraries(${test_name} PUBLIC ${custom_link_flags})
    target_link_libraries(${test_name} PUBLIC "util")
    target_link_libraries(${test_name} PUBLIC Boost::unit_test_framework)

    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

setup_test("unit-test-util.cpp")
setup_test("unit-test-strings.cpp")
setup_test("unit-test-misc.cpp")
setup_test("unit-test-vector-map.cpp")
setup_test("unit-test-vectors.cpp")
setup_test("unit-test-enum-util.cpp")
setup_test("unit-test-sliders.cpp")
setup_test("unit-test-filesystem.cpp")
setup_test("unit-test-sfutil.cpp")
setup_test("unit-test-texture-cache.cpp")
setup_test("unit-test-combat.cpp")
setup_test("unit-test-image-loading.cpp")
setup_test("unit-test-gui-elements.cpp")
setup_test("unit-test-holders.cpp")
setup_test("unit-test-maps.cpp")
setup_test("unit-test-audio.cpp")

#copy testing files
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/filesystem-test-stuff" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/filesystem-test-stuff" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
else()
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/filesystem-test-stuff" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

