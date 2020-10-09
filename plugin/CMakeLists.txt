cmake_minimum_required(VERSION 3.10)

project(ket_plugin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_library(ket_pown SHARED ket_pown.cpp)
target_link_libraries(ket_pown ket_bitwise)

install(TARGETS ket_pown
        LIBRARY DESTINATION kbw)

add_library(ket_diag SHARED ket_diag.cpp)
target_link_libraries(ket_diag ket_bitwise)

install(TARGETS ket_diag
        LIBRARY DESTINATION kbw)
