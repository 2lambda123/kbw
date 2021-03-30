/* MIT License
 * 
 * Copyright (c) 2020 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright (c) 2020 Rafael de Santiago <r.santiago@ufsc.br>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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

Assembler::Assembler(inst_t &instructions, label_t &labels) :
    instructions{instructions},
    labels{labels}
    {}

antlrcpp::Any Assembler::visitEntry(kqasmParser::EntryContext *ctx) {
    visitChildren(ctx);
    instructions.push_back([](Simulator&, size_t&, label_t&) {return;});
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

    instructions.push_back([ctrl, qbit_idx, gate, args] (Simulator &simulator, size_t&, label_t&) {
        switch (gate[0]) {
        case 'X':
            simulator.x(qbit_idx, ctrl);
            break;
        case 'Y':
            simulator.y(qbit_idx, ctrl);
            break;
        case 'Z':
            simulator.z(qbit_idx, ctrl);
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
        case 'R':
            switch (gate[1]) {
            case 'X':
                simulator.u3(args[0], -M_PI_2, M_PI_2, qbit_idx, ctrl); 
                break;
            case 'Y':
                simulator.u3(args[0], 0, 0, qbit_idx, ctrl); 
                break;
            case 'Z':
                simulator.rz(args[0], qbit_idx, ctrl);
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
    instructions.push_back([qubit_idx, dirty](Simulator &simulator, size_t&, label_t&) {
        simulator.alloc(qubit_idx, dirty);
    });

    return 0;
}

antlrcpp::Any Assembler::visitFree(kqasmParser::FreeContext *ctx) {
    auto qubit_idx = get_size_t(ctx->QBIT()->getText());
    bool dirty = ctx->DIRTY();
    instructions.push_back([qubit_idx, dirty](Simulator &simulator, size_t&, label_t&) {
        simulator.free(qubit_idx, dirty);
    });

    return 0;
}

antlrcpp::Any Assembler::visitMeasure(kqasmParser::MeasureContext *ctx) {
    auto qubit_idx = visit(ctx->qubits_list()).as<std::vector<size_t>>();
    auto int_idx = get_size_t(ctx->INT()->getText());
    instructions.push_back([qubit_idx, int_idx](Simulator &simulator, size_t&, label_t) {
        for (auto i : qubit_idx)
            simulator.measure(i);
            
        std::int64_t value = 0;
        size_t i = 0;
        for (auto j = qubit_idx.rbegin(); j != qubit_idx.rend(); ++j) 
            value |= simulator.get_bit(*j) << i++;

        simulator.set_i64(int_idx, value);    
    });

    return 0;
}

antlrcpp::Any Assembler::visitLabel(kqasmParser::LabelContext *ctx) {
    labels[ctx->LABEL()->getText()] = instructions.size()-1;
    return 0;
}

antlrcpp::Any Assembler::visitBranch(kqasmParser::BranchContext *ctx) {
    auto then = ctx->then->getText();
    auto otherwise = ctx->otherwise->getText();
    auto i64_idx = get_size_t(ctx->INT()->getText());
    instructions.push_back([i64_idx, then, otherwise](Simulator &simulator, size_t &pc, label_t& labels) {
        if (simulator.get_i64(i64_idx))
            pc = labels.at(then);
        else 
            pc = labels.at(otherwise);
    });
    
    return 0;
}

antlrcpp::Any Assembler::visitJump(kqasmParser::JumpContext *ctx) {
    auto label = ctx->LABEL()->getText();
    instructions.push_back([label](Simulator&, size_t& pc, label_t& labels) {
        pc = labels.at(label);
    });
    
    return 0;
}

antlrcpp::Any Assembler::visitDump(kqasmParser::DumpContext *ctx) {
    auto qubit_idx = visit(ctx->qubits_list()).as<std::vector<size_t>>();

    instructions.push_back([qubit_idx](Simulator &simulator, size_t&, label_t&) {
        simulator.dump(qubit_idx);
    });
    
    return 0;
}

antlrcpp::Any Assembler::visitPlugin(kqasmParser::PluginContext *ctx) {

    auto qubit_idx = visit(ctx->qubits_list()).as<std::vector<size_t>>();

    std::vector<size_t> ctrl_idx;
    if (ctx->ctrl()) ctrl_idx = visit(ctx->ctrl()).as<std::vector<size_t>>();

    auto args = ctx->ARGS() ? ctx->ARGS()->getText().substr(1, ctx->ARGS()->getText().size()-2) : "";
    
    bool adj = ctx->ADJ();
    auto ctrl = ctrl_idx.size();
    
    auto plugin_name = ctx->STR()->getText();

    instructions.push_back([plugin_name, ctrl, adj, qubit_idx, args, ctrl_idx](Simulator &simulator, size_t&, label_t&) {
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
    });

    return 0;
}

antlrcpp::Any Assembler::visitBinary_op(kqasmParser::Binary_opContext *ctx) {
    auto result = get_size_t(ctx->result->getText());
    auto left_idx = get_size_t(ctx->left->getText());
    auto right_idx = get_size_t(ctx->right->getText());
    auto op = visit(ctx->bin_op()).as<std::string>();
    
    instructions.push_back([result, left_idx, right_idx, op](Simulator &simulator, size_t&, label_t&){
        auto left = simulator.get_i64(left_idx);
        auto right = simulator.get_i64(right_idx);
        switch (op[0]) {
            case '=':
                simulator.set_i64(result, left==right);
                break;
            case '!':
                simulator.set_i64(result, left!=right);
                break;
            case '>':
                if (op.size() == 1) {
                    simulator.set_i64(result, left>right);
                } else {
                    switch (op[1]) {
                        case '=':
                            simulator.set_i64(result, left>=right);
                            break;
                        case '>':
                            simulator.set_i64(result, left>>right);
                            break;                        
                    }
                }
                break;                        
            case '<':
                if (op.size() == 1) {
                    simulator.set_i64(result, left<right);
                } else {
                    switch (op[1]) {
                        case '=':
                            simulator.set_i64(result, left<=right);
                            break;
                        case '<':
                            simulator.set_i64(result, left<<right);
                            break;                        
                    }
                }
                break;                        
            case '+':
                simulator.set_i64(result, left+right);
                break;
            case '-':
                simulator.set_i64(result, left-right);
                break;
            case '*': 
                simulator.set_i64(result, left*right);
                break;
            case '/': 
                simulator.set_i64(result, left/right);
                break;
            case 'a':
                simulator.set_i64(result, left&right);
                break;
            case 'x':
                simulator.set_i64(result, left^right);
                break;
            case 'o':
                simulator.set_i64(result, left|right);
                break;
        }
        
    });

    return 0;
}

antlrcpp::Any Assembler::visitConst_int(kqasmParser::Const_intContext *ctx) {
    std::stringstream ss;
    ss << ctx->UINT()->getText();
    std::int64_t uval;
    ss >> uval;
    std::int64_t val = ctx->SIG()? -uval : uval;
    auto idx = get_size_t(ctx->INT()->getText());
   
    instructions.push_back([idx, val](Simulator &simulator, size_t&, label_t&){
        simulator.set_i64(idx, val);
    });
   
    return 0;
}

antlrcpp::Any Assembler::visitSet(kqasmParser::SetContext *ctx) {
    auto i64_in_idx = get_size_t(ctx->target->getText());
    auto i64_value_idx = get_size_t(ctx->from->getText());
    instructions.push_back([i64_in_idx, i64_value_idx](Simulator &simulator, size_t&, label_t&) {
        simulator.set_i64(i64_in_idx, simulator.get_i64(i64_value_idx));
    });

    return 0;   
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
