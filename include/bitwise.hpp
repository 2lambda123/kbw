#pragma once
#include <iostream>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <complex>
#include <vector>
#include "index.hpp"

using namespace std::complex_literals;
using complex = std::complex<double>;
using map = boost::unordered_map<sim::Index, complex>; 
using ctrl_list = std::vector<size_t>;
using gate_map = boost::unordered_map<size_t, boost::unordered_set<std::pair<std::complex<double>, size_t>>>;

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
    void u1(double lambda, size_t idx, const ctrl_list& ctrl={});
    void u2(double phi, double lambda, size_t idx, const ctrl_list& ctrl={});
    void u3(double theta, double phi, double lambda, size_t idx, const ctrl_list& ctrl={});
    int measure(size_t idx);
    void oracle(gate_map& gate, size_t size);
    void swap(size_t a, size_t b);

 private:
    map qbits;

    friend std::ostream& operator<<(std::ostream &os, const Bitwise& q);
};
