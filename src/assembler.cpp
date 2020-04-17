#include "../include/assembler.hpp"
#include "../include/simulator.hpp"
#include <iostream>

antlrcpp::Any Assembler::visitGate(kqasmParser::GateContext *ctx) {
    std::stringstream ss;
    for (auto i : ctx->ctrl) 
            ss << i->getText().substr(1, i->getText().size()-1) << " ";

    std::vector<size_t> ctrl;
    size_t c;
    while (ss >> c) ctrl.push_back(c);

    ss.clear();

    auto qbit_str = ctx->QBIT()[0]->getText();  
    qbit_str = qbit_str.substr(1, qbit_str.size()-1);  
    ss << qbit_str;

    size_t qbit_idx; 
    ss >> qbit_idx;

    ss.clear();

    for (auto i : ctx->DOUBLE()) 
        ss << i->getText();

    std::vector<double> args;
    double arg;
    while (ss >> arg) args.push_back(arg);

    auto gate = ctx->gate->getText();

    instructions.push_back([ctrl, qbit_idx, gate, args] (Simulator &simulator) {
        switch (gate[0]) {
        case 'X':
            simulator.x(qbit_idx, ctrl);
            break;
        case 'Y':
            simulator.y(qbit_idx, ctrl);
            break;
        case 'Z':
            simulator.y(qbit_idx, ctrl);
            break;
        case 'H':
            simulator.h(qbit_idx, ctrl);
            break;
        case 'S':
            if (gate.size() == 1) 
                simulator.s(qbit_idx, ctrl);
            else
                simulator.sd(qbit_idx, ctrl);
            break;
        case 'T':
            if (gate.size() == 1) 
                simulator.t(qbit_idx, ctrl);
            else
                simulator.td(qbit_idx, ctrl);
            break;
        case 'U':
            switch (gate[1]) {
            case '1':
                simulator.u1(args[0], qbit_idx, ctrl); 
                break;
            case '2':
                simulator.u2(args[0], args[1], qbit_idx, ctrl); 
                break;
            case '3':
                simulator.u3(args[0], args[1], args[2], qbit_idx, ctrl); 
                break;
            }
            break;
        }
    });

    return 0;
}