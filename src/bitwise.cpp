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

#include "../include/ket_bitwise.hpp"
#include <boost/container/map.hpp>
#include <random>
#include <algorithm>

using namespace ket;
using namespace std::complex_literals;

Bitwise::Bitwise() {
    qbits[Index()] = 1;
}

void Bitwise::x(size_t idx, const ctrl_list& ctrl) {
    map qbits_tmp{};
    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl) exec &= i.first.is_one(j);
        if (exec) {
            auto j = i.first;
            j.flip(idx);
            qbits_tmp[j] = i.second; 
        } else {
            qbits_tmp[i.first] = i.second; 
        }
    }
    qbits.swap(qbits_tmp);
}

void Bitwise::y(size_t idx, const ctrl_list& ctrl) {
    map qbits_tmp{};
    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl) exec &= i.first.is_one(j);
        if (exec) {
            auto j = i.first;
            j.flip(idx);
            if (i.first.is_one(idx)) {
                qbits_tmp[j] = i.second*-1i;
            } else {
                qbits_tmp[j] = i.second*1i;
            }
        } else {
            qbits_tmp[i.first] = i.second; 
        }
    }
    qbits.swap(qbits_tmp);
}

void Bitwise::z(size_t idx, const ctrl_list& ctrl) {
    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl) exec &= i.first.is_one(j);
        if (exec and i.first.is_one(idx)) {
            qbits[i.first] *= -1;
        }
    }
}

void Bitwise::h(size_t idx, const ctrl_list& ctrl) {
    map qbits_tmp{};
    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl) exec &= i.first.is_one(j);
        if (exec) {
            if (i.first.is_one(idx)) {
                qbits_tmp[i.first] -= i.second/std::sqrt(2);
            } else {
                qbits_tmp[i.first] += i.second/std::sqrt(2);
            }
            if (std::abs(qbits_tmp[i.first]) < 1e-10) {
                qbits_tmp.erase(i.first);
            }
            auto j = i.first;
            j.flip(idx);
            qbits_tmp[j] += i.second/std::sqrt(2);
            if (std::abs(qbits_tmp[j]) < 1e-10) {
                qbits_tmp.erase(j);
            }
        } else {
            qbits_tmp[i.first] = i.second; 
        }

    }
    qbits.swap(qbits_tmp);
}

void Bitwise::s(size_t idx, const ctrl_list& ctrl) {
    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl) exec &= i.first.is_one(j);
        if (exec and i.first.is_one(idx)) {
            qbits[i.first] *= 1i;
        }
    }
}

void Bitwise::sd(size_t idx, const ctrl_list& ctrl) {
    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl) exec &= i.first.is_one(j);
        if (exec and i.first.is_one(idx)) {
            qbits[i.first] *= -1i;
        }
    }
}

void Bitwise::t(size_t idx, const ctrl_list& ctrl) {
    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl) exec &= i.first.is_one(j);
        if (exec and i.first.is_one(idx)) {
            qbits[i.first] *= std::exp(1i*M_PI/4.0);
        }
    }
}

void Bitwise::td(size_t idx, const ctrl_list& ctrl) {
    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl) exec &= i.first.is_one(j);
        if (exec and i.first.is_one(idx)) {
            qbits[i.first] *= std::exp(-1i*M_PI/4.0);
        }
    }
}

void Bitwise::cnot(size_t ctrl, size_t target,  const ctrl_list& ctrl2) {
    map qbits_tmp{};

    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl2) exec &= i.first.is_one(j);
        if (exec and i.first.is_one(ctrl)) {
            auto j = i.first;
            j.flip(target);
            qbits_tmp[j] = i.second; 
        } else {
            qbits_tmp[i.first] = i.second; 
        }
    }

    qbits.swap(qbits_tmp);
}

void Bitwise::u1(double lambda, size_t idx, const ctrl_list& ctrl) {
    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl) exec &= i.first.is_one(j);
        if (exec and i.first.is_one(idx)) {
            qbits[i.first] *= std::exp(1i*lambda);
        }
    }
}

void Bitwise::u2(double phi, double lambda, size_t idx, const ctrl_list& ctrl) {
    map qbits_tmp{};
    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl) exec &= i.first.is_one(j);
        if (exec) {
            auto j = i.first;
            j.flip(idx);
            if (i.first.is_one(idx)) {
                qbits_tmp[i.first] += i.second*std::exp(1i*(lambda+phi))/std::sqrt(2);
                if (std::abs(qbits_tmp[i.first]) < 1e-10)
                    qbits_tmp.erase(i.first);
                
                qbits_tmp[j] -= i.second*std::exp(1i*lambda)/std::sqrt(2);
                if (std::abs(qbits_tmp[j]) < 1e-10)
                    qbits_tmp.erase(j);
            } else {
                qbits_tmp[i.first] += i.second/sqrt(2);
                if (std::abs(qbits_tmp[i.first]) < 1e-10)
                    qbits_tmp.erase(i.first);
                
                qbits_tmp[j] += i.second*std::exp(1i*phi)/std::sqrt(2);
                if (std::abs(qbits_tmp[j]) < 1e-10)
                    qbits_tmp.erase(j);
            }

        } else {
            qbits_tmp[i.first] = i.second;
        }
    }

    qbits.swap(qbits_tmp);
} 

void Bitwise::u3(double theta, double phi, double lambda, size_t idx, const ctrl_list& ctrl) {
    map qbits_tmp{};
    std::complex<double> amp;
    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl) exec &= i.first.is_one(j);
        if (exec) {
            auto j = i.first;
            j.flip(idx);
            if (i.first.is_one(idx)) {
                amp = std::exp(1i*(lambda+phi))*std::cos(theta/2);
                qbits_tmp[i.first] += i.second*amp;
                if (std::abs(qbits_tmp[i.first]) < 1e-10)
                    qbits_tmp.erase(i.first);
                
                amp = std::exp(1i*lambda)*std::sin(theta/2);
                qbits_tmp[j] -= i.second*amp;
                if (std::abs(qbits_tmp[j]) < 1e-10)
                    qbits_tmp.erase(j);
            } else {
                amp = std::cos(theta/2);
                qbits_tmp[i.first] += i.second*amp;
                if (std::abs(qbits_tmp[i.first]) < 1e-10)
                    qbits_tmp.erase(i.first);

                amp = std::exp(1i*phi)*std::sin(theta/2);
                qbits_tmp[j] += i.second*amp;
                if (std::abs(qbits_tmp[j]) < 1e-10)
                    qbits_tmp.erase(j);
            }
        } else {
            qbits_tmp[i.first] = i.second;
        }
    }

    qbits.swap(qbits_tmp);
}

void Bitwise::rz(double lambda, size_t idx, const ctrl_list& ctrl) {
    for (auto &i : qbits) {
        bool exec = true;
        for (auto j : ctrl) exec &= i.first.is_one(j);
        if (exec) {
            if (i.first.is_one(idx))
                qbits[i.first] *= std::exp(1i*lambda/2.0);
            else 
                qbits[i.first] *= std::exp(-1i*lambda/2.0);
        } 
    }
}

int Bitwise::measure(size_t idx) {
    double p = 0;

    for (auto &i : qbits) {
        if (i.first.is_zero(idx)) {
            p += std::pow(std::abs(i.second), 2);
        }
    }
    
    auto result = p != 0 and 
                  (double(std::rand()) / double(RAND_MAX) <= p)?
                  0 : 1;
    
    p = result == 0? std::sqrt(p) : std::sqrt(1.0-p);

    map qbits_tmp{};

    for (auto &i : qbits)
        if (i.first.is_zero(idx) xor result) 
            qbits_tmp[i.first] = i.second/p;

    qbits.swap(qbits_tmp);
    return result;
}

void Bitwise::measure_zero(size_t idx) {
    double p = 0;

    for (auto &i : qbits) 
        if (i.first.is_zero(idx)) 
            p += std::pow(std::abs(i.second), 2);
            
    auto result = p != 0 and 
                  (double(std::rand()) / double(RAND_MAX) <= p)?
                  0 : 1;
    
    p = result == 0? std::sqrt(p) : std::sqrt(1.0-p);

    map qbits_tmp{};

    for (auto &i : qbits) {
        if (i.first.is_zero(idx) xor result) {
            if (result == 0) {
                qbits_tmp[i.first] = i.second/p;
            } else {
                auto j = i.first;
                j.flip(idx);
                qbits_tmp[j] = i.second/p;
            }
        }
    }
    
    qbits.swap(qbits_tmp);
}

std::ostream& ket::operator<<(std::ostream &os, const Bitwise& q) {
    boost::container::map<Index, complex> sorted;
    sorted.insert(q.qbits.begin(), q.qbits.end());
    for (auto &i : sorted) {
        os << i.first << ' ' << i.second << std::endl;
    }
    return os;
}

Bitwise::Bitwise(const Bitwise& a, const Bitwise& b) {
    for (const auto &i: a.qbits) for (const auto &j: b.qbits) 
        qbits[i.first|j.first] = i.second*j.second; 
}

void Bitwise::swap(size_t a, size_t b) {
    map qbits_tmp{};
    for (auto &i : qbits) {
        if (i.first.is_one(a) != i.first.is_one(b)) {
            auto j = i.first;
            j.flip(a);
            j.flip(b);
            qbits_tmp[j] = i.second;
        } else {
            qbits_tmp[i.first] = i.second;
        }
    }
    qbits.swap(qbits_tmp);
}

map& Bitwise::get_map() {
    return qbits;
}

dump_t Bitwise::dump(size_t size) const {
    dump_t state;

    for (auto &i : qbits) {
        std::vector<uint64_t> tmp_state;
        for (auto j = 0; j < size/64; j++)
            tmp_state.push_back(i.first[j]);
        tmp_state.push_back(i.first[size/64] & ((1ul << size%64) -1));
        state[tmp_state].push_back(i.second);
    }
    for (auto &i : state) {
        std::sort(i.second.begin(), i.second.end(), [](std::complex<double> a, std::complex<double> b) {
            if (a.real() == b.real()) return a.imag() < b.imag();
            else return a.real() < b.real();
        });
    }
    
    return state;
}