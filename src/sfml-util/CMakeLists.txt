project("sfml-util")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(${PROJECT_NAME}
				${Boost_LIBRARIES}
				Game
				Song
				Misc
                Map
				Popup)
add_subdirectory(gui)