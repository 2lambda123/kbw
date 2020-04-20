#pragma once
#include "bitwise.hpp"
#include <stack>
#include <boost/unordered_set.hpp>
#include <functional>

class Simulator {
public:
    Simulator();
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
    void measure(size_t idx, size_t bit);
    void alloc(size_t idx, bool dirty = false);
    void free(size_t idx, bool dirty = false);
    int get_bit(size_t idx);
    std::int64_t get_i64(size_t idx);
    void set_i64(size_t idx, std::int64_t value);
    void dump(size_t idx);
    std::string get_results();

private:
    ctrl_list map_ctrl(const ctrl_list& ctrl);
    void join(size_t idx, const ctrl_list& ctrl = {});
    
    boost::unordered_map<size_t, std::shared_ptr<Bitwise>> bitwise;
    boost::unordered_map<size_t, std::shared_ptr<boost::unordered_set<size_t>>> entangled;

    boost::unordered_map<size_t, size_t> allocated_qubits;
    std::stack<size_t> free_qubits;
    std::stack<size_t> dirty_qubits;

    boost::unordered_map<size_t, int> measurement;
    boost::unordered_map<size_t, std::int64_t> i64s;
};