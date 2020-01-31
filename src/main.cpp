#include "../include/driver.hpp"
#include <boost/program_options.hpp>

int main(int argc, char* argv[]) {

    size_t seed = 42;

    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
            ("help,h", "Show this")
            ("seed,s", boost::program_options::value<size_t>()->default_value(42), "Pseudo random number generator seed");
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        } 
        
        if (vm.count("seed")) {
            seed = vm["seed"].as<size_t>();
        }
    } catch (const boost::program_options::error& e) {
        std::cerr << e.what() << std::endl;
    }

    Driver dvr{seed};
    return dvr.parse();
}