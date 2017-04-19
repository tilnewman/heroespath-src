cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project("AppBase")


set(CUSTOM_BUILD_FLAGS "")

set(SFML_PATH "C:/Program Files/SFML-2.4.1")
message(STATUS "${PROJECT_NAME} will look for SFML here: \"" ${SFML_PATH} "\"")

if(WIN32)    
    set(CUSTOM_BUILD_FLAGS "/W4 /MP")
    set(BOOST_INCLUDEDIR "C:/local/boost_1_61_0")
    set(BOOST_LIBRARYDIR "C:/local/boost_1_61_0/lib32-msvc-14.0")
    set(BOOST_ROOT "C:/local/boost_1_61_0/boost")
else()
    file (READ common/gcc-build-flags.txt CUSTOM_BUILD_FLAGS)
endif()

add_definitions( ${CUSTOM_BUILD_FLAGS} )
message(STATUS "${PROJECT_NAME} will use build flags: \"" ${CUSTOM_BUILD_FLAGS} "\"")


set(CUSTOM_PROJECT_BOOST_VER "1.61.0")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON) #default is ON, but the consequences...
set(Boost_ADDITIONAL_VERSIONS ${Boost_ADDITIONAL_VERSIONS} ${CUSTOM_PROJECT_BOOST_VER})
FIND_PACKAGE(Boost ${CUSTOM_PROJECT_BOOST_VER} COMPONENTS system thread filesystem random serialization)
message(STATUS "${PROJECT_NAME} will use boost version: " ${Boost_VERSION})


include_directories(${include_directories} . ${Boost_INCLUDE_DIRS})
link_directories(${link_directories} ${Boost_LIBRARY_DIRS})
add_subdirectory(common)
add_subdirectory(configbase)
add_subdirectory(logbase)
add_subdirectory(stringutil)
add_subdirectory(sfml-util)
add_subdirectory(main)
add_subdirectory(heroespath)
