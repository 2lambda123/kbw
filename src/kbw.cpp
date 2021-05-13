/* Copyright 2020, 2021 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright 2020, 2021 Rafael de Santiago <r.santiago@ufsc.br>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../include/assembler.hpp"
#include "../include/kbw.hpp"
#include "antlr4-runtime.h"
#include "kqasmBaseVisitor.h"
#include "kqasmLexer.h"
#include "kqasmParser.h"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/complex.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <cstdio>
#include <filesystem>
#include <fstream>

void set_plugin_path(const std::string &path) {
    plugin_path = path;
}

std::string get_plugin_path() {
    return plugin_path;
}

void set_seed(int seed) {
    std::srand(seed);
}

kbw::kbw(const std::string& kqasm) {
    std::stringstream input{kqasm};
    antlr4::ANTLRInputStream file(input);
    kqasmLexer lexer(&file);
    antlr4::CommonTokenStream tokens(&lexer);
    kqasmParser parser(&tokens); 

    auto* tree = parser.start();

    Assembler assembler{blocks, next_block, end_block};
    assembler.visitStart(tree);
}

void kbw::run() {

    std::string label{"@entry"};
    while (true) {
        blocks[label](simulator);
        if (label == end_block) break;
        label = next_block[label](simulator);
    } 
}

std::string kbw::get_results() {
    return simulator.get_results();
}

std::int64_t kbw::get_result(size_t idx) {
    return simulator.get_i64(idx);
}

std::string kbw::get_dump(size_t idx) {
    std::stringstream stream;
    boost::archive::binary_oarchive oarchive{stream};

    oarchive << simulator.get_dump(idx);
    
    return stream.str();
}

std::string kbw::dump_to_fs(size_t idx) {
    std::string tmp_name = std::tmpnam(nullptr)+std::string{".ketd"};

    std::ofstream tmp_file{tmp_name, std::ofstream::binary};
    boost::archive::binary_oarchive oarchive{tmp_file};

    oarchive << simulator.get_dump(idx);

    tmp_file.close();

    return tmp_name;
}

std::int64_t kbw::dumps_len() {
    return simulator.dumps_len();
}

std::int64_t kbw::results_len(){
    return simulator.i64_len();
}