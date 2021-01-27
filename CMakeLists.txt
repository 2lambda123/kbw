cmake_minimum_required(VERSION 3.16)

project(KBW)

if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Release)
endif()

if(DEFINED SNAP)
    add_definitions(-DSNAP)
endif()

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

find_package(Threads REQUIRED)

################ ANTLR4 ########################
execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/antlr4_download.sh
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tools/)
                
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools/cpp_runtime/ EXCLUDE_FROM_ALL)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tools/cpp_runtime/runtime/src)

set_target_properties(antlr4_static PROPERTIES 
                      POSITION_INDEPENDENT_CODE ON)

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
#################################################

################ SWIG - Python ##################
find_package(SWIG REQUIRED)
include(UseSWIG)

SET_PROPERTY(SOURCE src/wrapper.i
             PROPERTY CPLUSPLUS ON)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
#################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE 
     SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

swig_add_library(kbw
                 LANGUAGE python
                 SOURCES src/wrapper.i 
                 ${SOURCES}
                 ${CMAKE_CURRENT_BINARY_DIR}/src/kqasmBaseVisitor.cpp
                 ${CMAKE_CURRENT_BINARY_DIR}/src/kqasmLexer.cpp
                 ${CMAKE_CURRENT_BINARY_DIR}/src/kqasmParser.cpp
                 ${CMAKE_CURRENT_BINARY_DIR}/src/kqasmVisitor.cpp)

set_target_properties(kbw PROPERTIES 
                      CXX_STANDARD 17
                      POSITION_INDEPENDENT_CODE ON)

add_dependencies(kbw parser)

target_compile_options(kbw PUBLIC -Wall -Wextra -pedantic)

target_link_libraries(kbw CONAN_PKG::boost antlr4_static dl ${PYTHON_LIBRARIES} Threads::Threads)

add_library(ket_bitwise STATIC 
            EXCLUDE_FROM_ALL
            ${CMAKE_CURRENT_SOURCE_DIR}/src/bitwise.cpp 
            ${CMAKE_CURRENT_SOURCE_DIR}/src/index.cpp)
            
target_link_libraries(ket_bitwise CONAN_PKG::boost)

set_target_properties(ket_bitwise PROPERTIES 
                      PUBLIC_HEADER "include/ket_bitwise.hpp"
                      POSITION_INDEPENDENT_CODE ON)
                      
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/plugin)

#install(TARGETS ket_bitwise
#        ARCHIVE DESTINATION /usr/lib
#        PUBLIC_HEADER DESTINATION /usr/include)

install(TARGETS kbw DESTINATION kbw)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kbw.py DESTINATION kbw)

if(DEFINED KBW_TEST)
    add_subdirectory(test/)
endif()

