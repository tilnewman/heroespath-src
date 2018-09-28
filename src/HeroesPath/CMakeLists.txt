project("heroespath" VERSION 0.1.0 LANGUAGES CXX)
file(GLOB source_files *.?pp)
add_executable(${PROJECT_NAME} ${source_files})
setup_target(${PROJECT_NAME})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "\$(SolutionDir)\$(Configuration)\\.")

endif()