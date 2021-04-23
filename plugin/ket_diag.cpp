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
using namespace std::complex_literals;

class ket_diag : public bitwise_api {
public:
    void run(map &qbits, size_t size, std::string args, bool adj, size_t ctrl) const {
        double diag[size_t(pow(2, size))];
        std::stringstream ss{args};
        for (int i = 0; i < pow(2, size); i++) 
            ss >> diag[i];

        if (adj) for (auto &i : diag) i *= -1;

        for (auto &i : qbits) {
            bool exec = true;
            for (auto j = size; j < size+ctrl; j++) exec &= i.first.is_one(j);
            if (exec) {
                auto val = i.first[0] & ((1ul << size)-1);
                qbits[i.first] *= exp(1i*diag[val]);
            }
        }
        
    }
};

bitwise_plugin(ket_diag);
