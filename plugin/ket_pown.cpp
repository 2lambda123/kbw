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
 
inline size_t pown(size_t x, size_t reg1, size_t N) { 
    if (reg1 == 0) return 1;

    size_t y = 1;
    for (; reg1 > 1; reg1 >>=  1) {
        if (reg1 & 1) y = (y*x)%N;
        x = (x*x)%N;
    }
    
    return (x*y)%N;
}

class ket_pown : public bitwise_api {
public:
    void run(map &qbits, size_t size, std::string args, bool adj, size_t ctrl) const {
        size_t L, x, N;
        std::stringstream ss{args};
        ss >> L // #n bits of N
           >> x >> N;

        map new_map;

        for (auto &i : qbits) {
            auto reg1_reg2 = i.first[0] & ((1ul << size)-1);
            auto reg1 = reg1_reg2 >> L;
            auto reg2 = pown(x, reg1, N);
            reg1_reg2 |= reg2;
            auto j = i.first;
            j[0] = reg1_reg2;

            new_map[j] = i.second;                        
        }

        qbits.swap(new_map);
        
    }
};

bitwise_plugin(ket_pown);
