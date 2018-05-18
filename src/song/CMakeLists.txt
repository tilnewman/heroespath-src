project("Song")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        Game
                        Stage
                        Stats
                        State
                        Item
                        Creature
                        NonPlayer
				        Spell
                        Misc
                        ${Boost_LIBRARIES} )
