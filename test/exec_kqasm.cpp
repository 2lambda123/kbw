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

#include "antlr4-runtime.h"
#include "kqasmLexer.h"
#include "kqasmParser.h"
#include "kqasmBaseVisitor.h"
#include "../include/assembler.hpp"
#include "../include/kbw.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

int main(int argc, const char* argv[]) {
    size_t seed = 42; 
    std::string input_path;
    std::string output_path;

    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
            ("help,h", "Show this information")
            ("seed,s", boost::program_options::value<size_t>(), "Pseudo random number generator seed")
            ("kqasm,i", boost::program_options::value<std::string>()->default_value(""), "kqasm input file")
            ("plugin,p", boost::program_options::value<std::string>(), "plugin directory path");

        boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options(desc).run();
        boost::program_options::variables_map vm;
    
        boost::program_options::store(parsed, vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            exit(0);
        } 

        if (vm.count("seed")) set_seed(vm["seed"].as<size_t>());

        input_path = vm["kqasm"].as<std::string>();
        
        if (vm.count("plugin")) set_plugin_path(vm["plugin"].as<std::string>());

    } catch (boost::program_options::error &e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
    
    std::cout << "Input KQASM: " << input_path << std::endl; 
    std::ifstream in_file(input_path);
    std::stringstream kqasm_buffer;
    kqasm_buffer << in_file.rdbuf();
    
    kbw qc{kqasm_buffer.str()};
    
    std::cout << "Simulating" << std::endl;
    qc.run();

    std::cout << "Results:" << std::endl;
    std::cout << "========" << std::endl;
    std::cout << qc.get_results();

    return 0;    
}
