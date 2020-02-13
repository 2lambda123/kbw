#include "../include/driver.hpp"
#include <iostream>
#include <boost/serialization/boost_unordered_map.hpp>
#include <boost/serialization/boost_unordered_set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/complex.hpp>

Driver::Driver(size_t seed) 
    : simulator{seed}, classic_counter{0}
{
    for (int i = 128; i > 0; i--) clean_qubits.push_back(i-1);
}

void Driver::add_qubit(size_t qubit) {
    qubit_map[qubit] = clean_qubits.back();
    clean_qubits.pop_back();
}

void Driver::add_dirty(size_t qubit) {
    if (not dirty_qubits.empty()) {
        qubit_map[qubit] = dirty_qubits.back();
        dirty_qubits.pop_back();
    } else {
        qubit_map[qubit] = clean_qubits.back();
        clean_qubits.pop_back();
    }
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

void Driver::gate(const std::string& gate, const std::vector<size_t>& qubits, const std::vector<double>& args, bool adj, const std::vector<size_t>& ctrl) {
    if (gate[0] == '\"') {
        std::stringstream file{gate.substr(1, gate.size()-2)};
        boost::archive::text_iarchive iarch(file);
        gate_map gm;
        iarch >> gm; 
        std::vector<size_t> mapped_qubits;
        for (auto i : qubits) mapped_qubits.push_back(qubit_map[i]);
        for (size_t i = 0; i < qubits.size(); i++) 
            simulator.swap(i, mapped_qubits[i]);
        
        simulator.oracle(gm, mapped_qubits.size());

        for (size_t i = 0; i < qubits.size(); i++) 
            simulator.swap(i, mapped_qubits[i]);

        return;
    }
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
    } else if (gate == "u1") {
        simulator.u1(args[0], qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "u2" and not adj) {
        simulator.u2(args[0], args[1], qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "u2" and adj) {
        simulator.u2(-args[1]-M_PI, -args[0]+M_PI, qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "u3" and not adj) {
        simulator.u3(args[0], args[1], args[2], qubit_map[qubits[0]], mapped_ctrl);
    } else if (gate == "u3" and adj) {
        simulator.u3(-args[0], args[2], args[1], qubit_map[qubits[0]], mapped_ctrl);
    }
}

void Driver::measure(size_t qubit, size_t bit) {
    int result = simulator.measure(qubit_map[qubit]);
    std::cout << bit << " " << result << std::endl;
    measurements[bit_map[bit]] = result;
    clean_qubits.push_back(qubit_map[qubit]);
    qubit_map.erase(qubit);
}

bool Driver::get_measure(size_t bit) {
    return measurements[bit_map[bit]];
}

void Driver::free(size_t qubit) {
    clean_qubits.push_back(qubit_map[qubit]);
    qubit_map.erase(qubit);
}

void Driver::freedirty(size_t qubit) {
    dirty_qubits.push_back(qubit_map[qubit]);
    qubit_map.erase(qubit);
}

void Driver::dump() {
    std::cerr << ">>>" << std::endl << simulator << "<<<" << std::endl ;
}