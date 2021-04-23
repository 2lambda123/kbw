#include "ket_bitwise.hpp"
#include <sstream>

using namespace ket;
using namespace std::complex_literals;

class example : public bitwise_api {
public:
    void run(map &qbits, size_t size, std::string args, bool adj, size_t ctrl) const {
        /*   
         *  Put your plugin here.
         */
        std::cout << "++++ PLUGIN ++++" << std::endl;
        std::cout << " Hellow word!!! " << std::endl;
        std::cout << "================" << std::endl;
        std::cout << " N of qubits: " << size << std::endl;
        std::cout << " Args: " << args << std::endl;
        std::cout << " Inverse: " << (adj? "yes" : "no") << std::endl;
        std::cout << " N of ctrl: " << ctrl << std::endl;
        std::cout << "++++++++++++++++" << std::endl;
    }
};

bitwise_plugin(example); // expose the plugin class
