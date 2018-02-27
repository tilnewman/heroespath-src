project("Stage")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        Game
                        Stats
                        State
                        Item
                        Creature
                        Player
                        NonPlayer
                        Spell
                        ${Boost_LIBRARIES} )
