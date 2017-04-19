cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project("HeroesPath-Stage")

file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        HeroesPath
                        HeroesPath-Stats
                        HeroesPath-State
                        HeroesPath-Item
                        HeroesPath-Creature
                        HeroesPath-Player
                        HeroesPath-NonPlayer
                        HeroesPath-Location
                        HeroesPath-Spell
                        ${Boost_LIBRARIES} )
