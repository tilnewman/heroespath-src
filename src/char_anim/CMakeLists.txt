project("Char_Anim")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        Game
                        State
                        Location
			Map
                        ${Boost_LIBRARIES} )
