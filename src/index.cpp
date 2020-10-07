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

#include "../include/ket_bitwise.hpp"
#include <iostream>
#include <bitset>

using namespace ket;

void Index::flip(size_t idx) {
    data[idx/64] ^= 1ull << (idx%64); 
}

bool Index::is_one(size_t idx) const {
    return data[idx/64] & (1ull << (idx%64));
}

bool Index::is_zero(size_t idx) const {
    return not (data[idx/64] & (1ull << (idx%64)));
}


uint64_t Index::operator[](size_t idx) const {
    return data[idx];
}

uint64_t& Index::operator[](size_t idx) {
    return data[idx];
}

Index Index::operator|(const Index& other) const {
    Index result;
    for (size_t i = 0; i < Index::size; i++) 
        result.data[i] = data[i] | other.data[i];
    return result;
}

bool Index::operator<(const Index& other) const {
    for (size_t i = 0; i < size; i++) {
        if (data[i] < other.data[i]) return true;
        else if (data[i] > other.data[i]) return false;
    }
    return false;
}

size_t ket::hash_value(const Index& idx) {
    size_t aux = 0;
    boost::hash<uint64_t> ui64_hash;
    for (size_t i = 0; i < idx.size; i++) {
        aux ^= ui64_hash(idx[i]);
    }
    return aux;
}

bool ket::operator==(const Index& a, const Index& b) {
    for (size_t i = 0; i < a.size; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

std::ostream& ket::operator<<(std::ostream &os, const Index& idx) {
    for (size_t i = idx.size; i > 0; --i) {
        std::bitset<64> bits(idx[i-1]);
        os << bits;
    }
    return os;
}
