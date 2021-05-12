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

class example : public bitwise_api {
public:
    void run(map &qbits, size_t size, std::string args, bool adj, size_t ctrl) const {
        double u00r, u01r, u10r, u11r;
        double u00i, u01i, u10i, u11i;
        std::stringstream ss{args};
        ss >> u00r >> u01r >> u10r >> u11r;
        ss >> u00i >> u01i >> u10i >> u11i;
        std::complex<double> u00{u00r, u00i}, u01{u01r, u01i}, u10{u10r, u10i}, u11{u11r, u11i};

        if (adj) {
            auto tmp = u01;
            u01 = u10;
            u10 = tmp;
            u00 = std::conj(u00);
            u01 = std::conj(u01);
            u10 = std::conj(u10);
            u11 = std::conj(u11);
        }

        map tmp_qbits;

        for (auto &i : qbits) {
            bool exec = true;
            for (auto j = size; j < size+ctrl; j++) exec &= i.first.is_one(j);
            if (exec) {
                for (int idx = 0; idx < size; idx++) {
                    auto j = i.first;
                    j.flip(idx);
                    if (i.first.is_one(idx)) {
                        // 00 01|0  01 
                        // 10 11|1  11 <-
                        tmp_qbits[i.first] += i.second*u11;
                        tmp_qbits[j] += i.second*u01; 
                    } else {
                        // 00 01|1  00 <-
                        // 10 11|0  10 
                        tmp_qbits[i.first] += i.second*u00;
                        tmp_qbits[j] += i.second*u10;                
                    }
                    if (std::abs(tmp_qbits[i.first]) < 1e-10)
                            tmp_qbits.erase(i.first);
                    if (std::abs(tmp_qbits[j]) < 1e-10)
                            tmp_qbits.erase(j);
                }
            }
        }
        qbits.swap(tmp_qbits);
 
    }
};

bitwise_plugin(example); // expose the plugin class
