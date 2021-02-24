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

#pragma once
#include "ket_bitwise.hpp"
#include <boost/unordered_set.hpp>
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
    void rz(double lambda, size_t idx, const ket::ctrl_list& ctrl = {});
    void measure(size_t idx);
    void alloc(size_t idx, bool dirty = false);
    void free(size_t idx, bool dirty = false);
    void apply_plugin(const boost::shared_ptr<ket::bitwise_api>& plugin, std::vector<size_t> idx, const std::string& args, bool adj, const ket::ctrl_list& ctrl);
    size_t get_bit(size_t idx);
    std::int64_t get_i64(size_t idx);
    void set_i64(size_t idx, std::int64_t value);
    void print(size_t idx);
    std::string get_results();
    void dump(const std::vector<size_t>& idx);
    ket::dump_t get_dump(size_t idx) const;

private:
    inline ket::ctrl_list map_ctrl(const ket::ctrl_list& ctrl) const {
        ket::ctrl_list mapped_ctrl;
        for (auto i : ctrl) mapped_ctrl.push_back(allocated_qubits.at(i));
        return mapped_ctrl;
    }

    inline bool map_ctrl_for_plugin(ket::ctrl_list& ctrl) {
        if (ctrl.empty()) return true;
        
        ket::ctrl_list new_ctrl{};

        for (auto i: ctrl) {
            if (bitwise[i]->get_map().size() > 1) new_ctrl.push_back(i);
            else if (bitwise[i]->get_map().begin()->first.is_one(i)) continue;
            else return false;
        }

        ctrl.swap(new_ctrl);
        
        return true;        
    }

    inline void merge_for_plugin(const ket::ctrl_list& ctrl) {
        if (ctrl.size() < 2) return;
        
        auto &ptr = bitwise[ctrl[0]];
        for (size_t i = 1; i < ctrl.size(); i++) if (ptr != bitwise[ctrl[i]]) {
            ptr = std::make_shared<ket::Bitwise>(*ptr, *bitwise[ctrl[i]]);

            auto &entangle_set = entangled[ctrl[0]];
            entangle_set->insert(entangled[ctrl[i]]->begin(), entangled[ctrl[i]]->end());

            auto entangle_backup = *entangled[ctrl[i]];
            for (auto j : entangle_backup) 
                entangled[j] = entangle_set;

            for (auto j : *entangle_set)
                bitwise[j] = ptr;
        }

    }

    inline bool merge(size_t idx, ket::ctrl_list& ctrl) {
        if (ctrl.empty()) return true;
        
        ket::ctrl_list new_ctrl{};

        for (auto i: ctrl) {
            if (bitwise[i]->get_map().size() > 1) new_ctrl.push_back(i);
            else if (bitwise[i]->get_map().begin()->first.is_one(i)) continue;
            else return false;
        }

        ctrl.swap(new_ctrl);

        auto &ptr = bitwise[idx];
        for (auto i: ctrl) if (ptr != bitwise[i]) {
            ptr = std::make_shared<ket::Bitwise>(*ptr, *bitwise[i]);

            auto &entangle_set = entangled[idx];
            entangle_set->insert(entangled[i]->begin(), entangled[i]->end());

            auto entangle_backup = *entangled[i];
            for (auto j : entangle_backup) 
                entangled[j] = entangle_set;

            for (auto j : *entangle_set)
                bitwise[j] = ptr;
        }
        
        return true;
    }

    boost::unordered_map<size_t, std::shared_ptr<ket::Bitwise>> bitwise;
    boost::unordered_map<size_t, std::shared_ptr<boost::unordered_set<size_t>>> entangled;

    boost::unordered_map<size_t, size_t> allocated_qubits;
    std::stack<size_t> free_qubits;
    std::stack<size_t> dirty_qubits;

    boost::unordered_map<size_t, int> measurement;
    boost::unordered_map<size_t, std::int64_t> i64s;
    
    std::vector<ket::dump_t> dumps;
};
