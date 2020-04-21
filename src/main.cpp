#include "antlr4-runtime.h"
#include "kqasmLexer.h"
#include "kqasmParser.h"
#include "kqasmBaseVisitor.h"
#include "../include/assembler.hpp"
#include <boost/program_options.hpp>
#include <iostream>

int main(int argc, const char* argv[]) {
    size_t seed = 42; 
    std::string input_path;
    std::string output_path;
    
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
            ("help,h", "Show this informations")
            ("seed,s", boost::program_options::value<size_t>()->default_value(42), "Pseudo random number generator seed")
            ("kqasm,i", boost::program_options::value<std::string>()->default_value(""), "kqasm input file")
            ("out,o", boost::program_options::value<std::string>()->default_value(""), "output file");

        boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options(desc).run();
        boost::program_options::variables_map vm;
    
        boost::program_options::store(parsed, vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            exit(0);
        } 

        seed = vm["seed"].as<size_t>();
        input_path = vm["kqasm"].as<std::string>();
        output_path = vm["out"].as<std::string>();

    } catch (boost::program_options::error &e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
 
    std::srand(seed);

    std::ifstream in_file;
    if (input_path != "") in_file.open(input_path);
    std::istream &input = input_path == "" ? std::cin : in_file; 

    antlr4::ANTLRInputStream file(input);
    kqasmLexer lexer(&file);
    antlr4::CommonTokenStream tokens(&lexer);
    kqasmParser parser(&tokens); 

    auto* tree = parser.entry();

    Assembler assembler;
    Code code = assembler.visitEntry(tree);
    code.run();

    std::ofstream out_file;
    if (output_path != "") out_file.open(output_path);
    std::ostream &output = output_path == "" ? std::cout : out_file;
    output << code.get_results();
}