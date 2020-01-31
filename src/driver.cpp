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

void Driver::gate(const std::string& gate, const std::vector<size_t>& qubits) {
    if (gate == "x") {
        simulator.x(qubit_map[qubits[0]]);
    } else if (gate == "y") {
        simulator.y(qubit_map[qubits[0]]);
    } else if (gate == "z") {
        simulator.z(qubit_map[qubits[0]]);
    } else if (gate == "h") {
        simulator.h(qubit_map[qubits[0]]);
    } else if (gate == "s") {
        simulator.s(qubit_map[qubits[0]]);
    } else if (gate == "sd") {
        simulator.sd(qubit_map[qubits[0]]);
    } else if (gate == "t") {
        simulator.t(qubit_map[qubits[0]]);
    } else if (gate == "td") {
        simulator.td(qubit_map[qubits[0]]);
    } else if (gate == "cnot") {
        simulator.cnot(qubit_map[qubits[0]], qubit_map[qubits[1]]);
    } 
}

void Driver::measure(size_t qubit, size_t bit) {
    int result = simulator.measure(qubit_map[qubit]);
    std::cout << bit << " " << result << std::endl;
    measurements[bit_map[bit]] = result;
}

void Driver::dump() {
    std::cout << "<<<" << std::endl << simulator << "<<<" << std::endl ;
}