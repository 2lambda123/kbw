#include "../include/simulator.hpp"

Simulator::Simulator(size_t seed) {
    std::srand(seed);
    for (size_t i = 0; i < 128; i++)
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

    auto ptr = bitwise[allocated_qubits[ctrl[0]]];

    for (auto i: ctrl) if (ptr != bitwise[allocated_qubits[i]]) 
        ptr = std::make_shared<Bitwise>(*ptr, *bitwise[allocated_qubits[i]]);

    ptr = std::make_shared<Bitwise>(*ptr, *bitwise[allocated_qubits[idx]]);

    for (auto i: ctrl) 
        bitwise[allocated_qubits[i]] = ptr;
    
    bitwise[allocated_qubits[idx]] = ptr;
}

void Simulator::x(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[idx]->x(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::y(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[idx]->y(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::z(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[idx]->z(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::h(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[idx]->h(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::s(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[idx]->s(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::sd(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[idx]->sd(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::t(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[idx]->t(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::td(size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[idx]->td(allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::u1(double lambda, size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[idx]->u1(lambda, allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::u2(double phi, double lambda, size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[idx]->u2(phi, lambda, allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::u3(double theta, double phi, double lambda, size_t idx, const ctrl_list& ctrl) {
    join(allocated_qubits[idx], map_ctrl(ctrl));
    bitwise[idx]->u3(theta, phi, lambda, allocated_qubits[idx], map_ctrl(ctrl));
}

void Simulator::measure(size_t idx, size_t bit) {
    measurement[bit] = bitwise[allocated_qubits[idx]]->measure(allocated_qubits[idx]);
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
}

void Simulator::free(size_t idx, bool dirty) {
    if (dirty) {
        dirty_qubits.push(allocated_qubits[idx]);
    } else {
        free_qubits.push(allocated_qubits[idx]);
        bitwise.erase(allocated_qubits[idx]);
    }
    allocated_qubits.erase(idx);
}
