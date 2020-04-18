#include "../include/assembler.hpp"
#include "../include/simulator.hpp"
#include <iostream>

size_t Assembler::get_size_t(std::string s) const {
    std::stringstream ss;
    auto str = s.substr(1, s.size()-1);  
    ss << str;
    size_t idx;
    ss >> idx;
    return idx;
}

antlrcpp::Any Assembler::visitGate(kqasmParser::GateContext *ctx) {
    std::vector<size_t> ctrl;
    for (auto i : ctx->ctrl) 
        ctrl.push_back(get_size_t(i->getText()));
    
    auto qbit_idx = get_size_t(ctx->QBIT()[0]->getText());  

    std::vector<double> args;
    for (auto i : ctx->DOUBLE()) 
        args.push_back(get_size_t(i->getText()));

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

antlrcpp::Any Assembler::visitAlloc(kqasmParser::AllocContext *ctx) {
    auto qubit_idx = get_size_t(ctx->QBIT()->getText());
    bool dirty = ctx->DIRTY();
    instructions.push_back([qubit_idx, dirty](Simulator &simulator) {
        simulator.alloc(qubit_idx, dirty);
    });

    return 0;
}

antlrcpp::Any Assembler::visitFree(kqasmParser::FreeContext *ctx) {
    auto qubit_idx = get_size_t(ctx->QBIT()->getText());
    bool dirty = ctx->DIRTY();
    instructions.push_back([qubit_idx, dirty](Simulator &simulator) {
        simulator.free(qubit_idx, dirty);
    });

    return 0;
}

antlrcpp::Any Assembler::visitMeasure(kqasmParser::MeasureContext *ctx) {
    auto qubit_idx = get_size_t(ctx->QBIT()->getText());
    auto bit_idx = get_size_t(ctx->BIT()->getText());
    instructions.push_back([qubit_idx, bit_idx](Simulator &simulator) {
        simulator.measure(qubit_idx, bit_idx);
    });

    return 0;
}

antlrcpp::Any Assembler::visitLabel(kqasmParser::LabelContext *ctx) {
    labels[ctx->LABEL()->getText()] = instructions.size()-1;
    return 0;
}

antlrcpp::Any Assembler::visitBranch(kqasmParser::BranchContext *ctx) {
    auto then = labels[ctx->LABEL()[0]->getText()];
    auto otherwise = labels[ctx->LABEL()[1]->getText()];
    auto i64_idx = get_size_t(ctx->I64()->getText());
    instructions.push_back([i64_idx, then, otherwise](Simulator &simulator) {
        if (simulator.get_i64(i64_idx))
            simulator.jump(then);
        else 
            simulator.jump(otherwise);
    });
    
    return 0;
}

antlrcpp::Any Assembler::visitJump(kqasmParser::JumpContext *ctx) {
    auto label = labels[ctx->LABEL()->getText()];
    instructions.push_back([label](Simulator &simulator) {
        simulator.jump(label);
    });
    
    return 0;
}


antlrcpp::Any Assembler::visitInt_ex(kqasmParser::Int_exContext *ctx) {
    bool se = ctx->ex->getText() == "SE";
    std::vector<size_t> bits;
    for (auto i : ctx->BIT())
        bits.push_back(get_size_t(i->getText()));
    auto i64_idx = get_size_t(ctx->I64()->getText());
    instructions.push_back([se, bits, i64_idx](Simulator &simulator) {
        std::int64_t value = 0;
        int i = 0;
        for (auto j = bits.rbegin(); j != bits.rend(); ++j) 
            value |= simulator.get_bit(*j) << i++;

        if (se and simulator.get_bit(bits[0])) for (auto j = i; j < 64; j++) 
            value |= std::int64_t{1} << j;

        simulator.set_i64(i64_idx, value);
    });
    
    return 0;
}