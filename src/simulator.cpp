/* Copyright 2020, 2021 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright 2020, 2021 Rafael de Santiago <r.santiago@ufsc.br>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../include/simulator.hpp"
#include <boost/smart_ptr.hpp>

using namespace ket;

Simulator::Simulator() {
    for (size_t i = 0; i < 64*Index::size; i++)
        free_qubits.push(i);
}

#define KET_GATE(x) void Simulator::x(size_t idx, const ctrl_list& ctrl) {\
    auto qubit_idx = allocated_qubits[idx];\
    auto mapped_ctrl = map_ctrl(ctrl);\
    if (merge(qubit_idx, mapped_ctrl)) {\
        auto &bw = bitwise[qubit_idx];\
        bw->x(qubit_idx, mapped_ctrl);\
    }\
}

void Simulator::x(size_t idx, const ctrl_list& ctrl) {
    auto qubit_idx = allocated_qubits[idx];
    auto mapped_ctrl = map_ctrl(ctrl);
    if (merge(qubit_idx, mapped_ctrl)) {
        auto &bw = bitwise[qubit_idx];
        bw->x(qubit_idx, mapped_ctrl);
    }
}

KET_GATE(y)
KET_GATE(z)
KET_GATE(h)
KET_GATE(s)
KET_GATE(sd)
KET_GATE(t)
KET_GATE(td)

void Simulator::p(double lambda, size_t idx, const ctrl_list& ctrl) {
    auto qubit_idx = allocated_qubits[idx];
    auto mapped_ctrl = map_ctrl(ctrl);
    if (merge(qubit_idx, mapped_ctrl)) {
        auto &bw = bitwise[qubit_idx];
        bw->p(lambda, qubit_idx, mapped_ctrl);
    }
}

void Simulator::u3(double theta, double phi, double lambda, size_t idx, const ctrl_list& ctrl) {
    auto qubit_idx = allocated_qubits[idx];
    auto mapped_ctrl = map_ctrl(ctrl);
    if(merge(qubit_idx, mapped_ctrl)) {
        auto &bw = bitwise[qubit_idx];
        bw->u3(theta, phi, lambda, qubit_idx, mapped_ctrl);
    }
}

void Simulator::rz(double lambda, size_t idx, const ctrl_list& ctrl) {
    auto qubit_idx = allocated_qubits[idx];
    auto mapped_ctrl = map_ctrl(ctrl);
    if (merge(qubit_idx, mapped_ctrl)) {
        auto &bw = bitwise[qubit_idx];
        bw->rz(lambda, qubit_idx, mapped_ctrl);
    }
}

void Simulator::apply_plugin(const boost::shared_ptr<ket::bitwise_api>& plugin, std::vector<size_t> idx, const std::string& args, bool adj, const ctrl_list &ctrl) {
    auto mapped_idx = map_ctrl(idx);

    auto mapped_ctrl = map_ctrl(ctrl);
    if (not map_ctrl_for_plugin(mapped_ctrl)) return;
    
    mapped_idx.insert(mapped_idx.end(), mapped_ctrl.begin(), mapped_ctrl.end());

    merge_for_plugin(mapped_idx); 
    auto &bw = bitwise[mapped_idx[0]];

    for (size_t i = 0; i < mapped_idx.size(); i++) bw->swap(i, mapped_idx[mapped_idx.size()-i-1]);
    plugin->run(bw->get_map(), idx.size(), args, adj, mapped_ctrl.size());
    for (size_t i = 0; i < mapped_idx.size(); i++) bw->swap(i, mapped_idx[mapped_idx.size()-i-1]);
}

void Simulator::measure(size_t idx) {
    measurement[idx] = bitwise[allocated_qubits[idx]]->measure(allocated_qubits[idx]);
}

void Simulator::alloc(size_t idx, bool dirty) {
    size_t allocated;
    if (dirty and not dirty_qubits.empty()) {
        allocated = dirty_qubits.top();
        dirty_qubits.pop();
    } else {
        allocated = free_qubits.top();
        free_qubits.pop();
        bitwise[allocated] = std::make_shared<Bitwise>();
        entangled[allocated] = std::make_shared<boost::unordered_set<size_t>>();
        entangled[allocated]->insert(allocated); 
    }

    allocated_qubits[idx] = allocated;    
}

void Simulator::free(size_t idx, bool dirty) {
    if (dirty) {
        dirty_qubits.push(allocated_qubits[idx]);
    } else {
        bitwise[allocated_qubits[idx]]->measure_zero(allocated_qubits[idx]);
        free_qubits.push(allocated_qubits[idx]);
        bitwise.erase(allocated_qubits[idx]);
        entangled[allocated_qubits[idx]]->erase(allocated_qubits[idx]);
        entangled.erase(allocated_qubits[idx]);
    }
    allocated_qubits.erase(idx);
}

size_t Simulator::get_bit(size_t idx) {
    return measurement[idx];
}

std::int64_t Simulator::get_i64(size_t idx) {
    return i64s[idx];
}

void Simulator::set_i64(size_t idx, std::int64_t value) {
    i64s[idx] = value;
}

void Simulator::print(size_t idx) {
    std::cerr << "/--------/ q" << idx << " allocated in "
              << allocated_qubits[idx] <<" /--------/" << std::endl
              << *bitwise[allocated_qubits[idx]]
              << "/---------------------------------------/" << std::endl;
}

std::string Simulator::get_results() {
    std::stringstream ss;
    for (auto &i: i64s) 
        ss << i.first << " " << i.second << std::endl;
    return ss.str();
} 

void Simulator::dump(const std::vector<size_t>& idx) {
    auto mapped_idx = map_ctrl(idx);
    boost::unordered_set<std::shared_ptr<Bitwise>> maps;
    for (auto i : mapped_idx) maps.insert(bitwise.at(i));

    auto bw = *(*maps.begin());
    maps.erase(maps.begin());

    for (auto i : maps) bw = Bitwise(bw, *i);
    
    for (size_t i = 0; i < mapped_idx.size(); i++) bw.swap(i, mapped_idx[mapped_idx.size()-i-1]);
    
    dumps.push_back(bw.dump(mapped_idx.size()));
}

dump_t Simulator::get_dump(size_t idx) const {
    return dumps.at(idx);
}

std::int64_t Simulator::dumps_len() const {
    return dumps.size();
}

std::int64_t Simulator::i64_len() const {
    return i64s.size();
}