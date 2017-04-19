cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project("HeroesPath-Stats")

file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(  ${PROJECT_NAME}
                        HeroesPath
                        ${Boost_LIBRARIES} )
