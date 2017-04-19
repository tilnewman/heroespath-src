cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project("HeroesPath")

if(WIN32)
    set(SFML_INCLUDE_DIR ${SFML_PATH}/include)
    include_directories(${include_directories} . ${SFML_INCLUDE_DIR})
else()
	add_definitions("-Wall -O3")
	set(SFML_INCLUDE_DIR "-I/usr/include/SFML")
	set(SFML_LIBRARY_DIRS "/usr/lib")
	set(SFML_LIBRARIES sfml-system sfml-window sfml-graphics)
endif()

file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(${PROJECT_NAME}
			          ConfigBase
                      LogBase
                      sfml-util
			          sfml-util-gui
                      StringUtil
			          ${Boost_LIBRARIES})

add_subdirectory(stats)
add_subdirectory(creature)
add_subdirectory(item)
add_subdirectory(player)
add_subdirectory(non-player)
add_subdirectory(location)
add_subdirectory(state)
add_subdirectory(stage)
add_subdirectory(combat)
add_subdirectory(spell)
