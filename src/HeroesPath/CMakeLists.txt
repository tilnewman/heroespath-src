project("HeroesPath")

set(TARGET_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
    set(TARGET_BINARY_DIR_RELEASE ${TARGET_BINARY_DIR}/Release)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../game-data.txt DESTINATION ${TARGET_BINARY_DIR_RELEASE})

    set(TARGET_BINARY_DIR_DEBUG ${TARGET_BINARY_DIR}/Debug)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../game-data.txt DESTINATION ${TARGET_BINARY_DIR_DEBUG})

    file(GLOB files ${SFML_BIN_DIR}/*)
    
    foreach(file ${files})
        file(COPY ${file} DESTINATION ${TARGET_BINARY_DIR_RELEASE})
        file(COPY ${file} DESTINATION ${TARGET_BINARY_DIR_DEBUG})
    endforeach()
else()
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../game-data.txt DESTINATION ${TARGET_BINARY_DIR})
endif()



include_directories(${include_directories}
                    ${SFML_INCLUDE_DIR})

link_directories(   ${link_directories}
                    ${SFML_LIBRARY_DIRS} )

file(GLOB source_files *.?pp)
add_executable(${PROJECT_NAME} ${source_files})

target_link_libraries(  ${PROJECT_NAME}
                        Misc
                        Log
                        sfml-util
                        sfml-util-gui
                        Game
                        Stage
                        Creature
                        Combat
			Popup
			Avatar
            ${SFML_LIBRARIES}
                        ${Boost_LIBRARIES})
