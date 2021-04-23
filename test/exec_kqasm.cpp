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
