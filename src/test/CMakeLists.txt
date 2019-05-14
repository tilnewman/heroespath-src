project("test" VERSION 0.1.0 LANGUAGES CXX)

enable_testing()

file(GLOB test_header_files *.hpp)
file(GLOB test_source_files *.cpp)

foreach(test_source_file ${test_source_files})

    get_filename_component(test_name ${test_source_file} NAME_WE)
  
    add_executable(${test_name} ${test_source_file} ${test_header_files})

    setup_target(${test_name})

    set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS ${custom_compile_flags})

    target_link_libraries(${test_name} PUBLIC ${custom_link_flags})
    
    target_link_libraries(${test_name} PUBLIC Boost::system Boost::date_time Boost::filesystem Boost::serialization Boost::unit_test_framework)
    
    add_test(NAME ${test_name} COMMAND ${test_name})

endforeach()


#copy testing files
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/filesystem-test-stuff" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/filesystem-test-stuff" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
else()
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/filesystem-test-stuff" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
