#include "../include/simulator.hpp"
#include <iostream>

using namespace ket;

Simulator::Simulator() {
    for (size_t i = 0; i < 64*Index::size; i++)
        free_qubits.push(i);
}

ctrl_list Simulator::map_ctrl(const ctrl_list& ctrl) {
    ctrl_list mapped_ctrl;

    for (auto i : ctrl) 
        mapped_ctrl.push_back(allocated_qubits[i]);

    return mapped_ctrl;
}

void Simulator::join(size_t idx, const ctrl_list& ctrl) {
    if (ctrl.empty()) return;

    auto ptr = bitwise[idx];

    for (auto i: ctrl) if (ptr != bitwise[i]) 
        ptr = std::make_shared<Bitwise>(*ptr, *bitwise[i]);

    auto &entangle_set = entangled[idx];

    for (auto i: ctrl) {
        entangle_set->insert(entangled[i]->begin(), entangled[i]->end());
        entangled[i] = entangle_set;
    }

    for (auto i : *entangle_set)
        bitwise[i] = ptr;
}

void Simulator::x(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[allocated_qubits[idx]]->x(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::y(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[allocated_qubits[idx]]->y(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::z(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[allocated_qubits[idx]]->z(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::h(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[allocated_qubits[idx]]->h(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::s(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[allocated_qubits[idx]]->s(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::sd(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[allocated_qubits[idx]]->sd(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::t(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[allocated_qubits[idx]]->t(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::td(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[allocated_qubits[idx]]->td(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::u1(double lambda, size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[allocated_qubits[idx]]->u1(lambda, allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::u2(double phi, double lambda, size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[allocated_qubits[idx]]->u2(phi, lambda, allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::u3(double theta, double phi, double lambda, size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[allocated_qubits[idx]]->u3(theta, phi, lambda, allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::apply_plugin(const boost::shared_ptr<bitwise_api>& plugin, std::vector<size_t> idx, const std::string& args) {
    auto mapped_idx = map_ctrl(idx);
    join(allocated_qubits[idx[0]], mapped_idx); 
    for (size_t i = 0; i < idx.size(); i++)
        bitwise[mapped_idx[0]]->swap(i, mapped_idx[mapped_idx.size()-i-1]);

    plugin->run(bitwise[mapped_idx[0]]->get_map(), idx.size(), args);
    
    for (size_t i = 0; i < idx.size(); i++)
        bitwise[mapped_idx[0]]->swap(i, mapped_idx[mapped_idx.size()-i-1]);
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
    }

    allocated_qubits[idx] = allocated;    
    entangled[allocated] = std::make_shared<boost::unordered_set<size_t>>();
    entangled[allocated]->insert(allocated); 
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
    std::cerr << "/--------/ q" << idx << " /--------/" << std::endl
              << *bitwise[allocated_qubits[idx]]
              << "/----------------------/" << std::endl;
}

std::string Simulator::get_results() {
    std::stringstream ss;
    for (auto &i: i64s) 
        ss << i.first << " " << i.second << std::endl;
    return ss.str();
} 