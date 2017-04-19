cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project("HeroesPath-Player")

file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        HeroesPath
                        HeroesPath-Stats
                        HeroesPath-Item
                        HeroesPath-Creature
                        HeroesPath-Spell
                        ${Boost_LIBRARIES} )
