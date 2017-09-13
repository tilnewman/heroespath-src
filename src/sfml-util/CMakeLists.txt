project("sfml-util")

if(WIN32)
	set(SFML_INCLUDE_DIR ${SFML_PATH}/include)
	set(SFML_LIBRARY_DIRS ${SFML_PATH}/lib)
	set(SFML_LIBRARIES sfml-system.lib sfml-window.lib sfml-graphics.lib sfml-audio.lib)
	set(Boost_COMPILER -vc90)
    include_directories(${include_directories} . ${SFML_INCLUDE_DIR})
else()
	set(SFML_INCLUDE_DIR "-I/usr/include/SFML")
	set(SFML_LIBRARY_DIRS "/usr/lib")
	set(SFML_LIBRARIES sfml-system sfml-window sfml-graphics sfml-audio)
endif()

file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(${PROJECT_NAME}
				${Boost_LIBRARIES}
				Game
				Game-Song
				Misc)

add_subdirectory(gui)