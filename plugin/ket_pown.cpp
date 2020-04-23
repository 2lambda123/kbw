#include "ket_bitwise.hpp"
#include <sstream>

using namespace ket;

size_t pown(size_t a, size_t x, size_t n) {
    size_t result = 1 ;

    while (x--) {
       result *= a;
       result %= n;
    }

    return result;
}

class ket_pown : public bitwise_api {
public:
    void run(map &qbits, size_t size, std::string args) {
        size_t n, a;
        std::stringstream ss{args};
        ss >> n >> a;

        map new_map;

        for (auto &i : qbits) {
            auto val = i.first[0] & ((1ul << size)-1);
            auto x = val >> (size/2);
            auto y = pown(a, x, n);
            val |= y;
            auto j = i.first;
            j[0] = val;

            new_map[j] = i.second;                        
        }

        qbits.swap(new_map);
        
    }
};

bitwise_plugin(ket_pown);
