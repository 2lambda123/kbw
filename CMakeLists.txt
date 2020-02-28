cmake_minimum_required(VERSION 3.16)

project(kbw)

add_compile_options(-Wall -Wextra -pedantic -Werror)

find_package(BISON)
find_package(FLEX)

BISON_TARGET(Parser src/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
FLEX_TARGET(Scanner src/scanner.ll ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)

ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

set(CMAKE_BUILD_TYPE Release)

if (NOT TARGET CONAN_PKG::boost)
        conan_cmake_run(REQUIRES boost/1.71.0
                        BASIC_SETUP CMAKE_TARGETS
                        BUILD missing)
endif()

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(kbw
    ${SOURCES}
    ${BISON_Parser_OUTPUTS} 
    ${FLEX_Scanner_OUTPUTS}
)

target_include_directories(kbw PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src )

target_link_libraries(kbw CONAN_PKG::boost)

install(TARGETS kbw DESTINATION /usr/bin)
install(FILES desktop/kbw.desktop DESTINATION /usr/share/applications)
install(FILES desktop/kbw.png DESTINATION /usr/share/icons)
