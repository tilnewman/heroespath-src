project("Game")
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
target_link_libraries(${PROJECT_NAME}
		      Config
              Log
              sfml-util
		      sfml-util-gui
              StringUtil
              Misc
		      Popup
		      ${Boost_LIBRARIES})
