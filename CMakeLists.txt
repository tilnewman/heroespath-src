cmake_minimum_required(VERSION 3.5)
project("Heroes Path")

message(STATUS "System Detected: ${CMAKE_SYSTEM}")

set(CUSTOM_BUILD_FLAGS "")
SET(CUSTOM_LINK_FLAGS "")

option(VALGRIND_BUILD "Build flags customized for Valgrind" OFF)

if(MSVC)
    set(SFML_PATH "C:/Program Files/SFML-2.4.1")
    message(STATUS "${PROJECT_NAME} will look for SFML here: \"" ${SFML_PATH} "\"")

    file (READ build-flags-msvc.txt CUSTOM_BUILD_FLAGS)

    set(BOOST_INCLUDEDIR "C:/local/boost_1_61_0")
    set(BOOST_LIBRARYDIR "C:/local/boost_1_61_0/lib32-msvc-14.0")
    set(BOOST_ROOT "C:/local/boost_1_61_0/boost")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)

    SET(CUSTOM_LINK_FLAGS "-pthread")

    if(VALGRIND_BUILD)
        file (READ build-flags-gnu-valgrind.txt CUSTOM_BUILD_FLAGS)
        message("Compiler flags customized for Valgrind.")
    else()
        file (READ build-flags-gnu.txt CUSTOM_BUILD_FLAGS)
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL AppleClang)
    file (READ build-flags-clang.txt CUSTOM_BUILD_FLAGS)
endif()

add_definitions( ${CUSTOM_BUILD_FLAGS} )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${CUSTOM_LINK_FLAGS}" )

message(STATUS "Compiler Detected: ${CMAKE_CXX_COMPILER_ID} with compile flags: ${CUSTOM_BUILD_FLAGS} and linker flags: ${CMAKE_EXE_LINKER_FLAGS}")

set(CUSTOM_PROJECT_BOOST_VER "1.58.0")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON) #default is ON, but the consequences...
set(Boost_ADDITIONAL_VERSIONS ${Boost_ADDITIONAL_VERSIONS} ${CUSTOM_PROJECT_BOOST_VER})
FIND_PACKAGE(Boost ${CUSTOM_PROJECT_BOOST_VER} COMPONENTS system thread filesystem random serialization)
message(STATUS "Using boost version: " ${Boost_VERSION})

include_directories(${include_directories} . ${Boost_INCLUDE_DIRS})
link_directories(${link_directories} ${Boost_LIBRARY_DIRS})
add_subdirectory(common)
add_subdirectory(configbase)
add_subdirectory(logbase)
add_subdirectory(stringutil)
add_subdirectory(sfml-util)
add_subdirectory(HeroesPath)
add_subdirectory(game)
add_subdirectory(utilz)
