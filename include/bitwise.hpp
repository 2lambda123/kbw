#pragma once
#include <iostream>
#include <boost/unordered_map.hpp>
#include <complex>
#include "index.hpp"

using namespace std::complex_literals;
using complex = std::complex<double>;
using map = boost::unordered_map<sim::Index, complex>; 

class Bitwise {
 public:
    Bitwise(size_t seed = 42);

    void x(size_t idx);
    void y(size_t idx);
    void z(size_t idx);
    void h(size_t idx);
    void s(size_t idx);
    void sd(size_t idx);
    void t(size_t idx);
    void td(size_t idx);
    void cnot(size_t ctrl, size_t target);
    int measure(size_t idx);

 private:
    map qbits;

    friend std::ostream& operator<<(std::ostream &os, const Bitwise& q);
};
