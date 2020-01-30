#include "../include/index.hpp"
#include <iostream>
#include <bitset>

Index::Index(size_t size) 
    : _size{size}, data{new uint64_t[size]} {} 

Index::~Index() {
    delete[] data;
}

void Index::flip(size_t idx) {
    data[idx/64] ^= 1ull << (idx%64); 
}

bool Index::is_one(size_t idx) const {
    return data[idx/64] & (1ull << (idx%64));
}

bool Index::is_zero(size_t idx) const {
    return not (data[idx/64] & (1ull << (idx%64)));
}

size_t hash_value(const Index& idx) {
    size_t aux = 0;
    boost::hash<uint64_t> ui64_hash;
    #pragma omp parallel for reduction(^:aux)
    for (size_t i = 0; i < idx.size(); i++) {
        aux ^= ui64_hash(idx[i]);
    }
    return aux;
}

bool operator==(const Index& a, const Index& b) {
    if (a.size() == b.size()) {
        for (auto i = 0; i < a.size(); i++) {
            if (a[i] != b[i]) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

std::ostream& operator<<(std::ostream &os, const Index& idx) {
    for (auto i = 0; i < idx.size(); i++) {
        std::bitset<64> bits(idx[i]);
        os << bits;
    }
    return os;
}
