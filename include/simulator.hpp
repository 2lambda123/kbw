#pragma once
#include "ket_bitwise.hpp"
#include <boost/asio/thread_pool.hpp>
#include <boost/unordered_set.hpp>
#include <boost/smart_ptr.hpp>
#include <functional>
#include <stack>

class Simulator {
public:
    Simulator();
    void x(size_t idx, const ket::ctrl_list& ctrl = {});
    void y(size_t idx, const ket::ctrl_list& ctrl = {});
    void z(size_t idx, const ket::ctrl_list& ctrl = {});
    void h(size_t idx, const ket::ctrl_list& ctrl = {});
    void s(size_t idx, const ket::ctrl_list& ctrl = {});
    void sd(size_t idx, const ket::ctrl_list& ctrl = {});
    void t(size_t idx, const ket::ctrl_list& ctrl = {});
    void td(size_t idx, const ket::ctrl_list& ctrl = {});
 // void cnot(size_t ctrl, size_t target, const ket::ctrl_list& ctrl2 = {});
    void u1(double lambda, size_t idx, const ket::ctrl_list& ctrl = {});
    void u2(double phi, double lambda, size_t idx, const ket::ctrl_list& ctrl = {});
    void u3(double theta, double phi, double lambda, size_t idx, const ket::ctrl_list& ctrl = {});
    void measure(size_t idx, size_t bit);
    void alloc(size_t idx, bool dirty = false);
    void free(size_t idx, bool dirty = false);
    void apply_plugin(const boost::shared_ptr<ket::bitwise_api>& plugin, std::vector<size_t> idx, const std::string& args);
    size_t get_bit(size_t idx);
    std::int64_t get_i64(size_t idx);
    void set_i64(size_t idx, std::int64_t value);
    void dump(size_t idx);
    std::string get_results();

private:
    inline ket::ctrl_list map_ctrl(const ket::ctrl_list& ctrl) {
        ket::ctrl_list mapped_ctrl;
        for (auto i : ctrl) mapped_ctrl.push_back(allocated_qubits[i]);
        return mapped_ctrl;
    }

    inline void merge(const ket::ctrl_list& ctrl) {
        if (ctrl.size() < 2) return;
        
        auto &ptr = bitwise[ctrl[0]];
        for (size_t i = 1; i < ctrl.size(); i++)
            ptr = std::make_shared<ket::Bitwise>(*ptr, *bitwise[ctrl[i]]);

        auto &entangle_set = entangled[ctrl[0]];
        for (size_t i = 1; i < ctrl.size(); i++) {
            entangle_set->insert(entangled[ctrl[i]]->begin(), entangled[ctrl[i]]->end());
            entangled[ctrl[i]] = entangle_set;
        }

        for (auto i : *entangle_set) 
            bitwise[i] = ptr;
    }

    inline void merge(size_t idx, const ket::ctrl_list& ctrl = {}) {
        if (ctrl.empty()) return;
        
        auto &ptr = bitwise[idx];
        for (auto i: ctrl) ptr = std::make_shared<ket::Bitwise>(*ptr, *bitwise[i]);

        auto &entangle_set = entangled[idx];
        for (auto i: ctrl) {
            entangle_set->insert(entangled[i]->begin(), entangled[i]->end());
            entangled[i] = entangle_set;
        }

        for (auto i : *entangle_set) 
            bitwise[i] = ptr;
    }

    boost::unordered_map<size_t, std::shared_ptr<ket::Bitwise>> bitwise;
    boost::unordered_map<size_t, std::shared_ptr<boost::unordered_set<size_t>>> entangled;

    boost::unordered_map<size_t, size_t> allocated_qubits;
    std::stack<size_t> free_qubits;
    std::stack<size_t> dirty_qubits;

    boost::unordered_map<size_t, int> measurement;
    boost::unordered_map<size_t, std::int64_t> i64s;
};