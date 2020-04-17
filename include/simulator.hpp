#pragma once
#include "bitwise.hpp"

class Simulator {
public:
    Simulator(size_t seed = 42);
    void x(size_t idx, const ctrl_list& ctrl = {});
    void y(size_t idx, const ctrl_list& ctrl = {});
    void z(size_t idx, const ctrl_list& ctrl = {});
    void h(size_t idx, const ctrl_list& ctrl = {});
    void s(size_t idx, const ctrl_list& ctrl = {});
    void sd(size_t idx, const ctrl_list& ctrl = {});
    void t(size_t idx, const ctrl_list& ctrl = {});
    void td(size_t idx, const ctrl_list& ctrl = {});
 // void cnot(size_t ctrl, size_t target, const ctrl_list& ctrl2 = {});
    void u1(double lambda, size_t idx, const ctrl_list& ctrl = {});
    void u2(double phi, double lambda, size_t idx, const ctrl_list& ctrl = {});
    void u3(double theta, double phi, double lambda, size_t idx, const ctrl_list& ctrl = {});
    int measure(size_t idx);

private:
    void join(size_t idx, const ctrl_list& ctrl = {});
    
    boost::unordered_map<size_t, std::shared_ptr<Bitwise>> bitwise;
};