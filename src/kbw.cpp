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
#include <boost/serialization/boost_unordered_map.hpp>
#include <boost/serialization/complex.hpp>
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

    auto* tree = parser.entry();

    Assembler assembler{instructions, labels};
    assembler.visitEntry(tree);
}

void kbw::run() {
   for (size_t pc = 0; pc < instructions.size(); pc++) {
       instructions[pc](simulator, pc, labels);
   }
}

std::string kbw::get_results() {
    return simulator.get_results();
}

std::int64_t kbw::get_result(size_t idx) {
    return simulator.get_i64(idx);
}

std::vector<unsigned long long> kbw::get_dump_states(size_t idx) {
    std::vector<unsigned long long> states;
    for (auto &i : simulator.get_dump(idx)) 
        states.push_back(i.first);
    return states;
}

std::vector<std::complex<double>> kbw::get_dump_amplitude(size_t idx, std::uint64_t state) {
    return simulator.get_dump(idx)[state];
}

PyObject* kbw::get_dump_base64(size_t idx) {
    std::stringstream stream;
    boost::archive::binary_oarchive oarchive{stream};

    oarchive << simulator.get_dump(idx);
    
    return PyBytes_FromStringAndSize(stream.str().c_str(), stream.str().size());
}