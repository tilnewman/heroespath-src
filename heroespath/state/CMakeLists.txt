project("HeroesPath-State")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        HeroesPath
                        HeroesPath-Stats
                        HeroesPath-Stage
                        HeroesPath-Item
                        HeroesPath-Creature
                        HeroesPath-Player
                        HeroesPath-NonPlayer
                        HeroesPath-Location
                        ${Boost_LIBRARIES} )
