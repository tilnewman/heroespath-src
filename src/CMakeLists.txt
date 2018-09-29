cmake_minimum_required(VERSION 3.8)
project("Heroes' Path" VERSION 0.0.8 LANGUAGES CXX)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})


#compiler specific tasks
set(custom_compile_flags "")
set(custom_link_flags "")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    file (READ ../build-flags/msvc.txt custom_compile_flags)

    #On Windows, Boost has some trouble linking dynamically, these defines help
    set(custom_compile_flags "${custom_compile_flags} -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK")

    #copy pvs-studio suppression files for Visual Studio
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/game.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/game.suppress DESTINATION ${CMAKE_BINARY_DIR}/game)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/combat.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/combat.suppress DESTINATION ${CMAKE_BINARY_DIR}/combat)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/creature.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/creature.suppress DESTINATION ${CMAKE_BINARY_DIR}/creature)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/item.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/item.suppress DESTINATION ${CMAKE_BINARY_DIR}/item)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/location.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/location.suppress DESTINATION ${CMAKE_BINARY_DIR}/location)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/spell.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/spell.suppress DESTINATION ${CMAKE_BINARY_DIR}/spell)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/stage.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/stage.suppress DESTINATION ${CMAKE_BINARY_DIR}/stage)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/stats.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/stats.suppress DESTINATION ${CMAKE_BINARY_DIR}/stats)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/heroespath.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/heroespath.suppress DESTINATION ${CMAKE_BINARY_DIR}/heroespath)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/logbase.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/logbase.suppress DESTINATION ${CMAKE_BINARY_DIR}/logbase)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/misc.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/misc.suppress DESTINATION ${CMAKE_BINARY_DIR}/misc)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/sfml-util.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/sfml-util.suppress DESTINATION ${CMAKE_BINARY_DIR}/sfml-util)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/stringutil.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/stringutil.suppress DESTINATION ${CMAKE_BINARY_DIR}/stringutil)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/popup.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/popup.suppress DESTINATION ${CMAKE_BINARY_DIR}/popup)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/map.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/map.suppress DESTINATION ${CMAKE_BINARY_DIR}/map)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/avatar.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/avatar.suppress DESTINATION ${CMAKE_BINARY_DIR}/avatar)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/interact.suppress)
        file(COPY ${CMAKE_SOURCE_DIR}/../pvs-studio-suppressions/interact.suppress DESTINATION ${CMAKE_BINARY_DIR}/interact)
    endif()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL GNU)

    set(custom_link_flags "${custom_link_flags} -pthread")

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
    option(ADDRESS_SANITIZER "Using Clang's Address Sanitizer" OFF)

    if(ADDRESS_SANITIZER)
	    message("Using Clang's Address Sanitizer")
	    set(custom_compile_flags ${custom_compile_flags} -fsanitize=address -fno-omit-frame-pointer)
	    set(custom_link_flags ${custom_link_flags} -fsanitize=address -fno-omit-frame-pointer)
    endif()

else()

    message(FATAL_ERROR " Unknwon Compiler: ${CMAKE_CXX_COMPILER_ID}")

endif()


#print/echo custom compile/link flags that were set above
message( ${CMAKE_CXX_COMPILER_ID} " Compile Flags: " ${custom_compile_flags})
message( ${CMAKE_CXX_COMPILER_ID} " Link Flags: " ${custom_link_flags})


#SFML
find_package(SFML 2.5 COMPONENTS window graphics audio REQUIRED)


#BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost COMPONENTS system date_time filesystem serialization unit_test_framework REQUIRED)


#list of library names (also the directory name)
set(library_names
    misc
    log
    sfml-util
    sfutil
    popup
    game
    combat
    item
    song
    spell
    stage
    creature
    map
    avatar
    interact)


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
set(executable_directory_names heroespath unit-tests)
foreach(executable_directory_name ${executable_directory_names})
    add_subdirectory(${executable_directory_name})
endforeach()


#copy the game configuration file alongside the executables for windows
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  
    foreach(executable_directory_name ${executable_directory_names})
        
        file(COPY game-data.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${executable_directory_name}/Release)
        
        file(COPY game-data.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${executable_directory_name}/Debug)
        
    endforeach()
    
else()

    file(COPY game-data.txt DESTINATION heroespath)

endif()
