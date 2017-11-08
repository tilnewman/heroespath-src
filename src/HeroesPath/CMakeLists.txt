project("HeroesPath")

set(TARGET_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
    set(TARGET_BINARY_DIR ${TARGET_BINARY_DIR}/Release)

    set(SFML_BIN_DIR ${SFML_PATH}/bin)
    set(SFML_INCLUDE_DIR ${SFML_PATH}/include)
    set(SFML_LIBRARY_DIRS ${SFML_PATH}/lib)
    set(SFML_LIBRARIES sfml-system.lib sfml-window.lib sfml-graphics.lib sfml-audio.lib)
    set(Boost_COMPILER -vc90)
    include_directories(${include_directories} . ${SFML_INCLUDE_DIR})

    file(GLOB files ${SFML_BIN_DIR}/*)
    foreach(file ${files})
        file(COPY ${file} DESTINATION ${TARGET_BINARY_DIR})
    endforeach()
else()
    set(SFML_INCLUDE_DIR "-I/usr/include/SFML")
    set(SFML_LIBRARY_DIRS "/usr/lib")
    set(SFML_LIBRARIES sfml-system sfml-window sfml-graphics sfml-audio)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../game-data.txt DESTINATION ${TARGET_BINARY_DIR})

include_directories(${include_directories}
                    ${SFML_INCLUDE_DIR})

link_directories(   ${link_directories}
                    ${SFML_LIBRARY_DIRS} )

file(GLOB source_files *.?pp)
add_executable(${PROJECT_NAME} ${source_files})

target_link_libraries(  ${PROJECT_NAME}
                        Misc
                        Config
                        Log
                        sfml-util
                        sfml-util-gui
                        Game
                        Stage
                        Stats
                        Creature
                        Player
                        NonPlayer
                        Location
                        State
                        Combat
			Popup
                        ${Boost_LIBRARIES}
                        ${SFML_LIBRARIES})
