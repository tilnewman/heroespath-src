project("Game-Stage")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        Game
                        Game-Stats
                        Game-State
                        Game-Item
                        Game-Creature
                        Game-Player
                        Game-NonPlayer
                        Game-Location
                        Game-Spell
                        ${Boost_LIBRARIES} )
