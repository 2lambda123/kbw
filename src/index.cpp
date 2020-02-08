#include "../include/index.hpp"
#include <iostream>
#include <bitset>

void sim::Index::flip(size_t idx) {
    data[idx/64] ^= 1ull << (idx%64); 
}

bool sim::Index::is_one(size_t idx) const {
    return data[idx/64] & (1ull << (idx%64));
}

bool sim::Index::is_zero(size_t idx) const {
    return not (data[idx/64] & (1ull << (idx%64)));
}


uint64_t sim::Index::operator[](size_t idx) const {
    return data[idx];
}

size_t sim::Index::size() const {
    return _size;
}

size_t sim::hash_value(const Index& idx) {
    size_t aux = 0;
    boost::hash<uint64_t> ui64_hash;
    for (size_t i = 0; i < idx.size(); i++) {
        aux ^= ui64_hash(idx[i]);
    }
    return aux;
}

bool sim::operator==(const sim::Index& a, const sim::Index& b) {
    if (a.size() == b.size()) {
        for (size_t i = 0; i < a.size(); i++) {
            if (a[i] != b[i]) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

std::ostream& operator<<(std::ostream &os, const sim::Index& idx) {
    for (size_t i = idx.size(); i > 0; --i) {
        std::bitset<64> bits(idx[i-1]);
        os << bits;
    }
    return os;
}
