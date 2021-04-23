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
