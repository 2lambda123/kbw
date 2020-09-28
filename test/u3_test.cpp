#include "../include/ket_bitwise.hpp"

using namespace ket;

int main() {
    Bitwise bw{};

    bw.h(0);
    
    std::cout << bw << std::endl;

    bw.u3(2.0*M_PI, 2.0*M_PI, 2.0*M_PI, 0);
    std::cout << bw << std::endl;

    bw.u3(2.0*M_PI, 2.0*M_PI, 2.0*M_PI, 0);
    std::cout << bw << std::endl;
    

    return 0;
}