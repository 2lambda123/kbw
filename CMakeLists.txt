cmake_minimum_required(VERSION 3.15)

project(kbw)

add_compile_options(-Wall -Wextra -pedantic)

################ CONAN - BOOST ################
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                  "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

if (NOT TARGET CONAN_PKG::boost)
    conan_cmake_run(REQUIRES boost/1.71.0
                    BASIC_SETUP CMAKE_TARGETS
                    BUILD missing)
endif()
#################################################

################ ANTLR4 ################
execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/antlr4_download.sh
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tools/)
                
add_subdirectory(${CMAKE_SOURCE_DIR}/tools/cpp_runtime/)
include_directories(${CMAKE_SOURCE_DIR}/tools/cpp_runtime/runtime/src)
#################################################

add_compile_options(-Wall -Wextra -pedantic)

add_custom_target(parser 
                  COMMAND java -jar ${CMAKE_SOURCE_DIR}/tools/antlr.jar -Dlanguage=Cpp -visitor -no-listener ${CMAKE_SOURCE_DIR}/src/kqasm.g4 -o ${CMAKE_CURRENT_BINARY_DIR}
                  SOURCES src/kqasm.g4)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


file(GLOB_RECURSE SOURCES *.cpp)

add_executable(kbw
               ${SOURCES})

target_link_libraries(kbw CONAN_PKG::boost antlr4_static)

install(TARGETS kbw DESTINATION /usr/bin)
install(FILES desktop/kbw.desktop DESTINATION /usr/share/applications)
install(FILES desktop/kbw.png DESTINATION /usr/share/icons)

