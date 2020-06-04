#include "../include/simulator.hpp"
#include <boost/asio/post.hpp>
#include <iostream>
#include <thread>

using namespace ket;

Simulator::Simulator() {
    for (size_t i = 0; i < 64*Index::size; i++)
        free_qubits.push(i);
}

#define KET_GATE(x) void Simulator::x(size_t idx, const ctrl_list& ctrl) {\
    auto qubit_idx = allocated_qubits[idx];\
    auto mapped_ctrl = map_ctrl(ctrl);\
    merge(qubit_idx, mapped_ctrl);\
    auto &bw = bitwise[qubit_idx];\
    bw->x(qubit_idx, mapped_ctrl);\
}

KET_GATE(x)
KET_GATE(y)
KET_GATE(z)
KET_GATE(h)
KET_GATE(s)
KET_GATE(sd)
KET_GATE(t)
KET_GATE(td)

void Simulator::u1(double lambda, size_t idx, const ctrl_list& ctrl) {
    auto qubit_idx = allocated_qubits[idx];
    auto mapped_ctrl = map_ctrl(ctrl);
    merge(qubit_idx, mapped_ctrl);
    auto &bw = bitwise[qubit_idx];
    bw->u1(lambda, qubit_idx, mapped_ctrl);
}

void Simulator::u2(double phi, double lambda, size_t idx, const ctrl_list& ctrl) {
    auto qubit_idx = allocated_qubits[idx];
    auto mapped_ctrl = map_ctrl(ctrl);
    merge(qubit_idx, mapped_ctrl);
    auto &bw = bitwise[qubit_idx];
    bw->u2(phi, lambda, qubit_idx, mapped_ctrl);
}

void Simulator::u3(double theta, double phi, double lambda, size_t idx, const ctrl_list& ctrl) {
    auto qubit_idx = allocated_qubits[idx];
    auto mapped_ctrl = map_ctrl(ctrl);
    merge(qubit_idx, mapped_ctrl);
    auto &bw = bitwise[qubit_idx];
    bw->u3(theta, phi, lambda, qubit_idx, mapped_ctrl);
}

void Simulator::apply_plugin(const boost::shared_ptr<bitwise_api>& plugin, std::vector<size_t> idx, const std::string& args) {
    auto mapped_idx = map_ctrl(idx);
    merge(mapped_idx); 
    auto &bw = bitwise[mapped_idx[0]];
    for (size_t i = 0; i < mapped_idx.size(); i++) bw->swap(i, mapped_idx[mapped_idx.size()-i-1]);
    plugin->run(bw->get_map(), mapped_idx.size(), args);
    for (size_t i = 0; i < mapped_idx.size(); i++) bw->swap(i, mapped_idx[mapped_idx.size()-i-1]);
}

void Simulator::measure(size_t idx, size_t bit) {
    measurement[bit] = bitwise[allocated_qubits[idx]]->measure(allocated_qubits[idx]);
    free(idx, false);
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

void Simulator::dump(size_t idx) {
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
