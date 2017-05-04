project("Game-Item")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        Game
                        Game-Stats
                        Game-Creature
                        Game-Spell
                        ${Boost_LIBRARIES} )
