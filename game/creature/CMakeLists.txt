project("Game-Creature")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        Game
                        Game-Stats
                        Game-Item
                        Game-Spell
                        ${Boost_LIBRARIES} )
