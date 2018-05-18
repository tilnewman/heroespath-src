project("Spell")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        Game
                        State
                        Stage
                        Item
                        Creature
                        ${Boost_LIBRARIES} )
