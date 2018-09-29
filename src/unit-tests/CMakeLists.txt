project("unit-tests" VERSION 0.1.0 LANGUAGES CXX)

enable_testing()

file(GLOB test_source_files *.cpp)

foreach(test_source_file ${test_source_files})

    get_filename_component(test_name ${test_source_file} NAME_WE)

    add_executable(${test_name} ${test_source_file})

    setup_target(${test_name})

    #since we are linking with boost dynamically we have to define BOOST_TEST_DYN_LINK or else the Boost Unit Test Framework won't link, I'm sure there's a really good reason, really, I'm like totally sure this is a fantastics design.
    set(custom_compile_flags_for_tests "${custom_compile_flags} -DBOOST_TEST_DYN_LINK")
    set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS ${custom_compile_flags_for_tests})

    #since we are linking with boost dynamically we ALSO have to link with something called boost_test_exec_monitor or else the Boost Unit Test Framework won't link, I'm even more sure that this is a good design than the previous note, really, great work guys.
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_link_libraries(${test_name} PUBLIC -lboost_test_exec_monitor)
    endif()

    target_link_libraries(${test_name} PUBLIC Boost::unit_test_framework)
    
    add_test(NAME ${test_name} COMMAND ${test_name})

endforeach()
