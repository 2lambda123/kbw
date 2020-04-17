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

sim::Index sim::operator|(const Index& l, const Index& r) {
    Index result;
    for (size_t i = 0; i < Index::size; i++) {
        result.data[i] = l.data[i] | r.data[i];
    }
    return result;
}

bool sim::Index::operator<(const Index& other) const {
    for (size_t i = 0; i < size; i++) {
        if (data[i] < other.data[i]) return true;
        else if (data[i] > other.data[i]) return false;
    }
    return false;
}

size_t sim::hash_value(const Index& idx) {
    size_t aux = 0;
    boost::hash<uint64_t> ui64_hash;
    for (size_t i = 0; i < idx.size; i++) {
        aux ^= ui64_hash(idx[i]);
    }
    return aux;
}

bool sim::operator==(const sim::Index& a, const sim::Index& b) {
    for (size_t i = 0; i < a.size; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

std::ostream& operator<<(std::ostream &os, const sim::Index& idx) {
    for (size_t i = idx.size; i > 0; --i) {
        std::bitset<64> bits(idx[i-1]);
        os << bits;
    }
    return os;
}
