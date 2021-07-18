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

add_library(ket_mat SHARED ket_mat.cpp)
target_link_libraries(ket_mat ket_bitwise)

install(TARGETS ket_mat
        LIBRARY DESTINATION kbw)


if (NOT DEFINED NOPYCALL)
        add_library(ket_pycall SHARED ket_pycall.cpp)

        target_link_libraries(ket_pycall ket_bitwise CONAN_PKG::boost)

        install(TARGETS ket_pycall
                LIBRARY DESTINATION kbw)

        add_executable(ket_pycall_interpreter ket_pycall_interpreter.cpp)

        find_package (Python3 COMPONENTS Development.Embed)
        target_link_libraries(ket_pycall_interpreter ${Python3_LIBRARIES})

        install(TARGETS ket_pycall_interpreter
                RUNTIME DESTINATION kbw)
endif()

#### Begin example #######################
add_library(example SHARED example.cpp)
target_link_libraries(example ket_bitwise)

install(TARGETS example
        LIBRARY DESTINATION kbw)
#### End example #######################