cmake_minimum_required(VERSION 3.16)

project(kbw)

add_compile_options(-Wall -Wextra -pedantic -Werror)

find_package(BISON)
find_package(FLEX)

BISON_TARGET(Parser src/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
FLEX_TARGET(Scanner src/scanner.ll  ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)

ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(kbw
    ${SOURCES}
    ${BISON_Parser_OUTPUTS} 
    ${FLEX_Scanner_OUTPUTS}
)

target_link_libraries(kbw ${CONAN_LIBS})

install(TARGETS kbw DESTINATION /usr/bin)
install(FILES desktop/kbw.desktop DESTINATION /usr/share/applications)
install(FILES desktop/kbw.png DESTINATION /usr/share/icons)
