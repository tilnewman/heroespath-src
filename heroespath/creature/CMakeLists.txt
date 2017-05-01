project("HeroesPath-Creature")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        HeroesPath
                        HeroesPath-Stats
                        HeroesPath-Item
                        HeroesPath-Spell
                        ${Boost_LIBRARIES} )
