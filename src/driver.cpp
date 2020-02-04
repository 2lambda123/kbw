#include "../include/driver.hpp"
#include <iostream>

Driver::Driver(size_t seed) 
    : simulator{seed}, quantum_counter{0}, classic_counter{0} {}

void Driver::add_qubit(size_t qubit) {
    qubit_map[qubit] = quantum_counter;
    quantum_counter++;
}

size_t Driver::get_qubit(size_t qubit) {
    return qubit_map[qubit];
}

void Driver::add_bit(size_t bit) {
    bit_map[bit] = classic_counter;
    classic_counter++;
}

size_t Driver::get_bit(size_t bit) {
    return bit_map[bit];
}

void Driver::gate(const std::string& gate, const std::vector<size_t>& qubits, bool adj, const std::vector<size_t>& ctrl) {
    std::vector<size_t> mapped_ctrl;
    for (auto i : ctrl) mapped_ctrl.push_back(qubit_map[i]);
    if (gate == "x") {
        simulator.x(qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "y") {
        simulator.y(qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "z") {
        simulator.z(qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "h") {
        simulator.h(qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "s" and not adj) {
        simulator.s(qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "s" and adj) {
        simulator.sd(qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "sd" and not adj) {
        simulator.sd(qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "sd" and adj) {
        simulator.s(qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "t" and not adj) {
        simulator.t(qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "t" and adj) {
        simulator.td(qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "td" and not adj) {
        simulator.td(qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "td" and adj) {
        simulator.t(qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "cnot") {
        simulator.cnot(qubit_map[qubits[0]], qubit_map[qubits[1]], mapped_ctrl);
    } 
}

void Driver::measure(size_t qubit, size_t bit) {
    int result = simulator.measure(qubit_map[qubit]);
    std::cout << bit << " " << result << std::endl;
    measurements[bit_map[bit]] = result;
}

bool Driver::get_measure(size_t bit) {
    return measurements[bit_map[bit]];
}

void Driver::dump() {
    std::cerr << ">>>" << std::endl << simulator << "<<<" << std::endl ;
}