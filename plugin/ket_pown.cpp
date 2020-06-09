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
