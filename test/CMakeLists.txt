add_executable(u3_test 
               u3_test.cpp)

target_link_libraries(u3_test ket_bitwise)

add_executable(bell 
               bell.cpp)

target_link_libraries(bell ket_bitwise)



set_source_files_properties(
              ${CMAKE_BINARY_DIR}/src/kqasmBaseVisitor.cpp
              ${CMAKE_BINARY_DIR}/src/kqasmLexer.cpp
              ${CMAKE_BINARY_DIR}/src/kqasmParser.cpp
              ${CMAKE_BINARY_DIR}/src/kqasmVisitor.cpp
              PROPERTIES GENERATED TRUE)

add_executable(exec_kqasm 
               ${SOURCES}
               ${CMAKE_BINARY_DIR}/src/kqasmBaseVisitor.cpp
               ${CMAKE_BINARY_DIR}/src/kqasmLexer.cpp
               ${CMAKE_BINARY_DIR}/src/kqasmParser.cpp
               ${CMAKE_BINARY_DIR}/src/kqasmVisitor.cpp
               exec_kqasm.cpp)

set_target_properties(exec_kqasm PROPERTIES 
                      CXX_STANDARD 17)

target_link_libraries(exec_kqasm CONAN_PKG::boost antlr4_static dl Threads::Threads)