project("Popup")

if(WIN32)
    set(SFML_INCLUDE_DIR ${SFML_PATH}/include)
    include_directories(${include_directories} . ${SFML_INCLUDE_DIR})
else()
       add_definitions("-Wall -O3")
       set(SFML_INCLUDE_DIR "-I/usr/include/SFML")
       set(SFML_LIBRARY_DIRS "/usr/lib")
       set(SFML_LIBRARIES sfml-system sfml-window sfml-graphics sfml-audio)
endif()

file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(${PROJECT_NAME}
		      Config
              Log
              sfml-util
		      sfml-util-gui
              StringUtil
              Misc
		      Game
		      Combat
              ${SFML_LIBRARIES}
		      ${Boost_LIBRARIES})
