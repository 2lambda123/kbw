#include "../include/simulator.hpp"

Simulator::Simulator(size_t seed) {
    std::srand(seed);
    bitwise[0] = std::make_shared<Bitwise>();
}

void Simulator::join(size_t idx, const ctrl_list& ctrl) {
    if (ctrl.empty()) return;

    auto ptr = bitwise[ctrl[0]];

    for (auto i: ctrl) if (ptr != bitwise[i]) 
        ptr = std::make_shared<Bitwise>(*ptr, *bitwise[i]);

    ptr = std::make_shared<Bitwise>(*ptr, *bitwise[idx]);

    for (auto i: ctrl) 
        bitwise[i] = ptr;
    
    bitwise[idx] = ptr;
}

void Simulator::x(size_t idx, const ctrl_list& ctrl) {
    join(idx, ctrl);
    bitwise[idx]->x(idx, ctrl);
}

void Simulator::y(size_t idx, const ctrl_list& ctrl) {
    join(idx, ctrl);
    bitwise[idx]->y(idx, ctrl);
}

void Simulator::z(size_t idx, const ctrl_list& ctrl) {
    join(idx, ctrl);
    bitwise[idx]->z(idx, ctrl);
}

void Simulator::h(size_t idx, const ctrl_list& ctrl) {
    join(idx, ctrl);
    bitwise[idx]->h(idx, ctrl);
}

void Simulator::s(size_t idx, const ctrl_list& ctrl) {
    join(idx, ctrl);
    bitwise[idx]->s(idx, ctrl);
}

void Simulator::sd(size_t idx, const ctrl_list& ctrl) {
    join(idx, ctrl);
    bitwise[idx]->sd(idx, ctrl);
}

void Simulator::t(size_t idx, const ctrl_list& ctrl) {
    join(idx, ctrl);
    bitwise[idx]->t(idx, ctrl);
}

void Simulator::td(size_t idx, const ctrl_list& ctrl) {
    join(idx, ctrl);
    bitwise[idx]->td(idx, ctrl);
}

void Simulator::u1(double lambda, size_t idx, const ctrl_list& ctrl) {
    join(idx, ctrl);
    bitwise[idx]->u1(lambda, idx, ctrl);
}

void Simulator::u2(double phi, double lambda, size_t idx, const ctrl_list& ctrl) {
    join(idx, ctrl);
    bitwise[idx]->u2(phi, lambda, idx, ctrl);
}

void Simulator::u3(double theta, double phi, double lambda, size_t idx, const ctrl_list& ctrl) {
    join(idx, ctrl);
    bitwise[idx]->u3(theta, phi, lambda, idx, ctrl);
}

int Simulator::measure(size_t idx) {
    bitwise[idx]->measure(idx);
}