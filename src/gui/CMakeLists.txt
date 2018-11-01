project("gui" VERSION 0.0.8 LANGUAGES CXX)
file(GLOB source_files *.?pp)
add_library(${PROJECT_NAME} STATIC ${source_files})
setup_target(${PROJECT_NAME})
