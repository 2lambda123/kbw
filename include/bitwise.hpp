#pragma once
#include <iostream>
#include <boost/unordered_map.hpp>
#include <complex>
#include <vector>
#include "index.hpp"

using namespace std::complex_literals;
using complex = std::complex<double>;
using map = boost::unordered_map<sim::Index, complex>; 
using ctrl_list = std::vector<size_t>;

class Bitwise {
 public:
    Bitwise(size_t seed = 42);

    void x(size_t idx, const ctrl_list& ctrl = {});
    void y(size_t idx, const ctrl_list& ctrl = {});
    void z(size_t idx, const ctrl_list& ctrl = {});
    void h(size_t idx, const ctrl_list& ctrl = {});
    void s(size_t idx, const ctrl_list& ctrl = {});
    void sd(size_t idx, const ctrl_list& ctrl = {});
    void t(size_t idx, const ctrl_list& ctrl = {});
    void td(size_t idx, const ctrl_list& ctrl = {});
    void cnot(size_t ctrl, size_t target, const ctrl_list& ctrl2 = {});
    int measure(size_t idx);

 private:
    map qbits;

    friend std::ostream& operator<<(std::ostream &os, const Bitwise& q);
};
