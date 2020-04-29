cmake_minimum_required(VERSION 3.15)

project(kbw)

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
################################################

################ ANTLR4 ########################
execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/antlr4_download.sh
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tools/)
                
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools/cpp_runtime/ EXCLUDE_FROM_ALL)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tools/cpp_runtime/runtime/src)
#################################################

add_custom_target(parser 
                  COMMAND java -jar ${CMAKE_CURRENT_SOURCE_DIR}/tools/antlr.jar -Dlanguage=Cpp -visitor -no-listener ${CMAKE_CURRENT_SOURCE_DIR}/src/kqasm.g4 -o ${CMAKE_CURRENT_BINARY_DIR}/src
                  SOURCES src/kqasm.g4)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

set_source_files_properties(
              ${CMAKE_CURRENT_BINARY_DIR}/src/kqasmBaseVisitor.cpp
              ${CMAKE_CURRENT_BINARY_DIR}/src/kqasmLexer.cpp
              ${CMAKE_CURRENT_BINARY_DIR}/src/kqasmParser.cpp
              ${CMAKE_CURRENT_BINARY_DIR}/src/kqasmVisitor.cpp
              PROPERTIES GENERATED TRUE)
              
file(GLOB_RECURSE 
     SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(kbw 
              ${SOURCES}
              ${CMAKE_CURRENT_BINARY_DIR}/src/kqasmBaseVisitor.cpp
              ${CMAKE_CURRENT_BINARY_DIR}/src/kqasmLexer.cpp
              ${CMAKE_CURRENT_BINARY_DIR}/src/kqasmParser.cpp
              ${CMAKE_CURRENT_BINARY_DIR}/src/kqasmVisitor.cpp)

target_compile_options(kbw PUBLIC -Wall -Wextra -pedantic)

add_dependencies(kbw parser)
               
target_link_libraries(kbw CONAN_PKG::boost antlr4_static dl)

add_library(ket_bitwise STATIC 
            ${CMAKE_CURRENT_SOURCE_DIR}/src/bitwise.cpp 
            ${CMAKE_CURRENT_SOURCE_DIR}/src/index.cpp)
            
target_link_libraries(ket_bitwise CONAN_PKG::boost)

set_target_properties(ket_bitwise PROPERTIES 
                      PUBLIC_HEADER "include/ket_bitwise.hpp"
                      POSITION_INDEPENDENT_CODE ON)
                      
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/plugin)

install(TARGETS ket_bitwise
        LIBRARY DESTINATION /usr/lib
        PUBLIC_HEADER DESTINATION /usr/include)

install(TARGETS kbw DESTINATION /usr/bin)
install(FILES desktop/kbw.desktop DESTINATION /usr/share/applications)
install(FILES desktop/kbw.png DESTINATION /usr/share/icons)

