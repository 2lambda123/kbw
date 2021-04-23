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

#include "../include/assembler.hpp"
#include "../include/simulator.hpp"
#include "../include/kbw.hpp"
#include <boost/dll/import.hpp> 

inline size_t get_size_t(std::string token) {
    std::stringstream ss;
    ss << token.substr(1, token.size()-1);
    size_t index;
    ss >> index;
    return index;
}

Assembler::Assembler(block_t &blocks, next_block_t &next_block, std::string &end_block) :
    blocks{blocks},
    next_block{next_block},
    end_block{end_block} 
    {}

antlrcpp::Any Assembler::visitBlock(kqasmParser::BlockContext *ctx) {
    auto label = visit(ctx->label()).as<std::string>();
    
    std::vector<std::function<void(Simulator&)>> instructions;

    for (auto inst : ctx->instruction()) 
        instructions.push_back(visit(inst).as<std::function<void(Simulator&)>>());

    blocks[label] = [instructions](Simulator& sim) {
        for (auto inst : instructions) inst(sim);
    }; 
    
    next_block[label] = visit(ctx->end_instruction()).as<std::function<std::string(Simulator&)>>();
    
    return 0;
}

antlrcpp::Any Assembler::visitEnd_block(kqasmParser::End_blockContext *ctx) {
    auto label = visit(ctx->label()).as<std::string>();
    
    std::vector<std::function<void(Simulator&)>> instructions;

    for (auto inst : ctx->instruction()) 
        instructions.push_back(visit(inst).as<std::function<void(Simulator&)>>());

    blocks[label] = [instructions](Simulator& sim) {
        for (auto inst : instructions) inst(sim);
    }; 
    
    end_block = label;
    
    return 0;
}

antlrcpp::Any Assembler::visitCtrl(kqasmParser::CtrlContext *ctx) {
    return visit(ctx->qubits_list());
}

antlrcpp::Any Assembler::visitGate(kqasmParser::GateContext *ctx) {
    std::vector<size_t> ctrl;
    if (ctx->ctrl()) ctrl = visit(ctx->ctrl()).as<std::vector<size_t>>();

    auto qbit_idx = get_size_t(ctx->QBIT()->getText());  

    std::vector<double> args;
    if (ctx->arg_list()) args = visit(ctx->arg_list()).as<std::vector<double>>();

    auto gate = visit(ctx->gate_name()).as<std::string>();
    
    return std::function<void(Simulator&)>{[ctrl, qbit_idx, gate, args] (Simulator &simulator) {
        const boost::unordered_map<std::string, std::function<void(void)>> gate_map{
            {"X",  [&]() {simulator.x(qbit_idx, ctrl);}},
            {"Y",  [&]() {simulator.y(qbit_idx, ctrl);}},
            {"Z",  [&]() {simulator.z(qbit_idx, ctrl);}},
            {"H",  [&]() {simulator.h(qbit_idx, ctrl);}},
            {"S",  [&]() {simulator.s(qbit_idx, ctrl);}},
            {"SD", [&]() {simulator.sd(qbit_idx, ctrl);}},
            {"T",  [&]() {simulator.t(qbit_idx, ctrl);}},
            {"TD", [&]() {simulator.td(qbit_idx, ctrl);}},
            {"RX", [&]() {simulator.u3(args[0], -M_PI_2, M_PI_2, qbit_idx, ctrl);}},
            {"RY", [&]() {simulator.u3(args[0], 0, 0, qbit_idx, ctrl);}},
            {"RZ", [&]() {simulator.rz(args[0], qbit_idx, ctrl);}},
            {"U1", [&]() {simulator.u1(args[0], qbit_idx, ctrl);}},
            {"U2", [&]() {simulator.u2(args[0], args[1], qbit_idx, ctrl);}},
            {"U3", [&]() {simulator.u3(args[0], args[1], args[2], qbit_idx, ctrl);}},
        };
        
        gate_map.at(gate)();

    }};
}

antlrcpp::Any Assembler::visitAlloc(kqasmParser::AllocContext *ctx) {
    auto qubit_idx = get_size_t(ctx->QBIT()->getText());
    bool dirty = ctx->DIRTY();
    
    return std::function<void(Simulator&)>{[qubit_idx, dirty](Simulator &simulator) {
        simulator.alloc(qubit_idx, dirty);
    }};
}

antlrcpp::Any Assembler::visitFree(kqasmParser::FreeContext *ctx) {
    auto qubit_idx = get_size_t(ctx->QBIT()->getText());
    bool dirty = ctx->DIRTY();
    return std::function<void(Simulator&)>{[qubit_idx, dirty](Simulator &simulator) {
        simulator.free(qubit_idx, dirty);
    }};
}

antlrcpp::Any Assembler::visitMeasure(kqasmParser::MeasureContext *ctx) {
    auto qubit_idx = visit(ctx->qubits_list()).as<std::vector<size_t>>();
    auto int_idx = get_size_t(ctx->INT()->getText());
    return std::function<void(Simulator&)>{[qubit_idx, int_idx](Simulator &simulator) {
        for (auto i : qubit_idx)
            simulator.measure(i);
            
        std::int64_t value = 0;
        size_t i = 0;
        for (auto j = qubit_idx.rbegin(); j != qubit_idx.rend(); ++j) 
            value |= simulator.get_bit(*j) << i++;

        simulator.set_i64(int_idx, value);    
    }};
}

antlrcpp::Any Assembler::visitLabel(kqasmParser::LabelContext *ctx) {
    return ctx->LABEL()->getText();
}

antlrcpp::Any Assembler::visitBranch(kqasmParser::BranchContext *ctx) {
    auto then = ctx->then->getText();
    auto otherwise = ctx->otherwise->getText();
    auto i64_idx = get_size_t(ctx->INT()->getText());

    return std::function<std::string(Simulator&)>{[i64_idx, then, otherwise](Simulator &simulator) {
        return simulator.get_i64(i64_idx)? then : otherwise;    
    }};
}

antlrcpp::Any Assembler::visitJump(kqasmParser::JumpContext *ctx) {
    auto label = ctx->LABEL()->getText();

    return std::function<std::string(Simulator&)>{[label](Simulator &simulator) {
        return label;
    }};
}

antlrcpp::Any Assembler::visitDump(kqasmParser::DumpContext *ctx) {
    auto qubit_idx = visit(ctx->qubits_list()).as<std::vector<size_t>>();

    return std::function<void(Simulator&)>{[qubit_idx](Simulator &simulator) {
        simulator.dump(qubit_idx);
    }};
}

antlrcpp::Any Assembler::visitPlugin(kqasmParser::PluginContext *ctx) {

    auto qubit_idx = visit(ctx->qubits_list()).as<std::vector<size_t>>();

    std::vector<size_t> ctrl_idx;
    if (ctx->ctrl()) ctrl_idx = visit(ctx->ctrl()).as<std::vector<size_t>>();

    auto args = ctx->ARGS() ? ctx->ARGS()->getText().substr(1, ctx->ARGS()->getText().size()-2) : "";
    
    bool adj = ctx->ADJ();
    auto ctrl = ctrl_idx.size();
    
    auto plugin_name = ctx->STR()->getText();

    return std::function<void(Simulator&)>{[plugin_name, ctrl, adj, qubit_idx, args, ctrl_idx](Simulator &simulator) {
        std::stringstream path_ss{plugin_path};
        std::string path;
        boost::shared_ptr<ket::bitwise_api> plugin;
        while (std::getline(path_ss, path, ':')) {
            try {
                boost::dll::fs::path lib_path(path);       
                plugin = boost::dll::import<ket::bitwise_api>(lib_path / plugin_name,             
                                                              "plugin",                                     
                                                              boost::dll::load_mode::append_decorations);
                break;
            } catch (boost::system::system_error &e) {
                continue;
            }
        }
        
        simulator.apply_plugin(plugin, qubit_idx, args, adj, ctrl_idx);
    }};
}

antlrcpp::Any Assembler::visitBinary_op(kqasmParser::Binary_opContext *ctx) {
    auto result = get_size_t(ctx->result->getText());
    auto left_idx = get_size_t(ctx->left->getText());
    auto right_idx = get_size_t(ctx->right->getText());
    auto op = visit(ctx->bin_op()).as<std::string>();

    return std::function<void(Simulator&)>{[result, left_idx, right_idx, op](Simulator &simulator) {
        auto left = simulator.get_i64(left_idx);
        auto right = simulator.get_i64(right_idx);
        
        const boost::unordered_map<std::string, std::function<void(void)>> op_map{
            { "==",  [&](){simulator.set_i64(result, left == right);}},
            { "!=",  [&](){simulator.set_i64(result, left != right);}},
            { ">",   [&](){simulator.set_i64(result, left >  right);}},
            { ">=",  [&](){simulator.set_i64(result, left >= right);}},
            { "<",   [&](){simulator.set_i64(result, left <  right);}},
            { "<=",  [&](){simulator.set_i64(result, left <= right);}},
            { "+",   [&](){simulator.set_i64(result, left +  right);}},
            { "-",   [&](){simulator.set_i64(result, left -  right);}},
            { "*",   [&](){simulator.set_i64(result, left *  right);}},
            { "/",   [&](){simulator.set_i64(result, left /  right);}},
            { "<<",  [&](){simulator.set_i64(result, left << right);}},
            { ">>",  [&](){simulator.set_i64(result, left >> right);}},
            { "and", [&](){simulator.set_i64(result, left &  right);}},
            { "or",  [&](){simulator.set_i64(result, left |  right);}},
            { "xor", [&](){simulator.set_i64(result, left ^  right);}},
        };
        
        op_map.at(op)();
    }};
}

antlrcpp::Any Assembler::visitConst_int(kqasmParser::Const_intContext *ctx) {
    std::stringstream ss;
    ss << ctx->UINT()->getText();
    std::int64_t uval;
    ss >> uval;
    std::int64_t val = ctx->SIG()? -uval : uval;
    auto idx = get_size_t(ctx->INT()->getText());
    
    return std::function<void(Simulator&)>{[idx, val](Simulator &simulator) {
        simulator.set_i64(idx, val);
    }};
}

antlrcpp::Any Assembler::visitSet(kqasmParser::SetContext *ctx) {
    auto i64_in_idx = get_size_t(ctx->target->getText());
    auto i64_value_idx = get_size_t(ctx->from->getText());
    
    return std::function<void(Simulator&)>{[i64_in_idx, i64_value_idx](Simulator &simulator) {
        simulator.set_i64(i64_in_idx, simulator.get_i64(i64_value_idx));
    }};
}

antlrcpp::Any Assembler::visitBin_op(kqasmParser::Bin_opContext *ctx) {
    return ctx->getText();
}

antlrcpp::Any Assembler::visitArg_list(kqasmParser::Arg_listContext *ctx) {
    std::vector<double> args;
    for (auto i : ctx->DOUBLE())
        args.push_back(atof(i->getText().c_str()));
    return args;
}

antlrcpp::Any Assembler::visitGate_name(kqasmParser::Gate_nameContext *ctx) {
    return ctx->getText();
}

antlrcpp::Any Assembler::visitQubits_list(kqasmParser::Qubits_listContext *ctx) {
    std::vector<size_t> qubits;
    for (auto i : ctx->QBIT())
        qubits.push_back(get_size_t(i->getText()));
    return qubits;
}
