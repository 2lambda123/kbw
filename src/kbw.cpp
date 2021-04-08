/* MIT License
 * 
 * Copyright (c) 2020 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright (c) 2020 Rafael de Santiago <r.santiago@ufsc.br>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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