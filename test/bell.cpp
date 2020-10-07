#include "../include/ket_bitwise.hpp"

using namespace ket;

int main() {
    Bitwise bw{};

    bw.h(0);
    bw.x(1, {0});

    std::cout << bw << std::endl;

    bw.measure(0);

    std::cout << bw << std::endl;
    
    return 0;
}
