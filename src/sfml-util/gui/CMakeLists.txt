project("sfml-util-gui")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(${PROJECT_NAME}
				sfml-util
				Game
				Song
				${Boost_LIBRARIES})
