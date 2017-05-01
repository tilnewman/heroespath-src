project("HeroesPath-Item")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        HeroesPath
                        HeroesPath-Stats
                        HeroesPath-Creature
                        HeroesPath-Spell
                        ${Boost_LIBRARIES} )
