cmake_minimum_required(VERSION 3.8)
project("Heroes' Path" VERSION 0.0.8 LANGUAGES CXX)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})


#list of library names (also the directory name)
set(library_names
    avatar
    combat
    creature
    game
    gui
    interact
    item
    map
    misc
    popup
    sfutil
    song
    spell
    stage
    testutil)


#list of directories with executables
set(executable_directory_names heroespath unittest)


#list of all sub-directories
set(sub_directory_names ${library_names} ${executable_directory_names})


#compiler specific tasks
set(custom_compile_flags "")
set(custom_link_flags "")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    file (READ ../build-flags/msvc.txt custom_compile_flags)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    set(custom_link_flags ${custom_link_flags} -pthread)

    #Valgrind stuff here
    option(VALGRIND_BUILD "Build flags customized for Valgrind" OFF)

    if(VALGRIND_BUILD)
        message("Compiler flags customized for Valgrind.")
        file (READ ../build-flags/gnu-valgrind.txt custom_compile_flags)
    else()
        file (READ ../build-flags/gnu.txt custom_compile_flags)
    endif()

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    file (READ ../build-flags/clang.txt custom_compile_flags)

    #Clang's Address Sanitizer stuff here
    option(ADDRESS_SANITIZER "Clang's Address Sanitizer" OFF)

    if(ADDRESS_SANITIZER)
	    message("Clang's Address Sanitizer ENABLED")
	    set(custom_compile_flags ${custom_compile_flags} -fsanitize=address -fno-omit-frame-pointer)
	    set(custom_link_flags ${custom_link_flags} -fsanitize=address -fno-omit-frame-pointer)
    endif()

else()
    message(FATAL_ERROR " Unknown Compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()


#print/echo custom compile/link flags that were set above
message( ${CMAKE_CXX_COMPILER_ID} " Compile Flags: " ${custom_compile_flags})
message( ${CMAKE_CXX_COMPILER_ID} " Link Flags: " ${custom_link_flags})


#SFML
find_package(SFML 2.5 COMPONENTS window graphics audio REQUIRED)


#BOOST
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS system date_time filesystem serialization unit_test_framework REQUIRED)


#sets compiler flags, linker flags, and dependencies common to all targets
function(setup_target name)
    
    #why the fuck doesn't target_compile_options line work?! Using set_target_properties instead...
    #target_compile_options(${name} PUBLIC ${custom_compile_flags})
    set_target_properties(${name} PROPERTIES COMPILE_FLAGS ${custom_compile_flags})

    target_link_libraries(${name} PUBLIC ${custom_link_flags})
    target_link_libraries(${name} PUBLIC Boost::system Boost::date_time Boost::filesystem Boost::serialization)
    target_link_libraries(${name} PUBLIC sfml-window sfml-graphics sfml-audio)
    
    set(library_names_excluding_current ${library_names})
    list(REMOVE_ITEM library_names_excluding_current ${name})
    target_link_libraries(${name} PUBLIC ${library_names_excluding_current})
        
endfunction()


#add libraries
foreach(library_name ${library_names})
    add_subdirectory(${library_name})
endforeach()


#add executables
foreach(executable_directory_name ${executable_directory_names})
    add_subdirectory(${executable_directory_name})
endforeach()


#copy the game configuration file alongside the executables for windows
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  
    foreach(executable_directory_name ${executable_directory_names})
        file(COPY config.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${executable_directory_name}/Release)
        file(COPY config.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${executable_directory_name}/Debug)
    endforeach()
    
else()
    file(COPY config.txt DESTINATION heroespath)
    file(COPY config.txt DESTINATION unittest)
endif()


#copy pvs-studio suppression files for Visual Studio
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    set(suppress_files_directory "${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions")

    file(GLOB suppress_file_names ${suppress_files_directory}/*.suppress)

    set(suppress_names "")

    foreach(suppress_file_name ${suppress_file_names})
        set(name_without_extension "")
        get_filename_component(name_without_extension ${suppress_file_name} NAME_WE)
	    set(suppress_names ${suppress_names} ${name_without_extension})
    endforeach()

    foreach(suppress_name ${suppress_names})

	    if(EXISTS ${CMAKE_BINARY_DIR}/${suppress_name})
            file(COPY ${suppress_files_directory}/${suppress_name}.suppress DESTINATION ${CMAKE_BINARY_DIR}/${suppress_name})
        else()
            file(COPY ${suppress_files_directory}/${suppress_name}.suppress DESTINATION ${CMAKE_BINARY_DIR}/unittest)
        endif()

    endforeach()

endif()
