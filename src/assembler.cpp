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
#include <boost/dll/import.hpp> 
#include <iostream>

Assembler::Assembler(const std::string& plugin_path) :
    plugin_path{plugin_path}
    {}

size_t Assembler::get_size_t(std::string s) const {
    std::stringstream ss;
    auto str = s.substr(1, s.size()-1);  
    ss << str;
    size_t idx;
    ss >> idx;
    return idx;
}

antlrcpp::Any Assembler::visitEntry(kqasmParser::EntryContext *ctx) {
    visitChildren(ctx);
    instructions.push_back([](Simulator&, size_t&) {return;});
    return 0;
} 

antlrcpp::Any Assembler::visitGate(kqasmParser::GateContext *ctx) {
    std::vector<size_t> ctrl;
    for (auto i = 0u; i < ctx->QBIT().size()-1; i++)
        ctrl.push_back(get_size_t(ctx->QBIT()[i]->getText()));
    
    auto qbit_idx = get_size_t(ctx->QBIT().back()->getText());  

    std::vector<double> args;
    for (auto i : ctx->DOUBLE()) {
        std::stringstream ss;
        ss << i->getText();
        double arg;
        ss >> arg;
        args.push_back(arg);
    }

    auto gate = ctx->gate->getText();

    instructions.push_back([ctrl, qbit_idx, gate, args] (Simulator &simulator, size_t&) {
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
        }
    });

    return 0;
}

antlrcpp::Any Assembler::visitAlloc(kqasmParser::AllocContext *ctx) {
    auto qubit_idx = get_size_t(ctx->QBIT()->getText());
    bool dirty = ctx->DIRTY();
    instructions.push_back([qubit_idx, dirty](Simulator &simulator, size_t&) {
        simulator.alloc(qubit_idx, dirty);
    });

    return 0;
}

antlrcpp::Any Assembler::visitFree(kqasmParser::FreeContext *ctx) {
    auto qubit_idx = get_size_t(ctx->QBIT()->getText());
    bool dirty = ctx->DIRTY();
    instructions.push_back([qubit_idx, dirty](Simulator &simulator, size_t&) {
        simulator.free(qubit_idx, dirty);
    });

    return 0;
}

antlrcpp::Any Assembler::visitMeasure(kqasmParser::MeasureContext *ctx) {
    auto qubit_idx = get_size_t(ctx->QBIT()->getText());
    instructions.push_back([qubit_idx](Simulator &simulator, size_t&) {
        simulator.measure(qubit_idx);
    });

    return 0;
}

antlrcpp::Any Assembler::visitLabel(kqasmParser::LabelContext *ctx) {
    labels[ctx->LABEL()->getText()] = instructions.size()-1;
    return 0;
}

antlrcpp::Any Assembler::visitBranch(kqasmParser::BranchContext *ctx) {
    auto then = ctx->LABEL()[0]->getText();
    auto otherwise = ctx->LABEL()[1]->getText();
    auto i64_idx = get_size_t(ctx->I64()->getText());
    instructions.push_back([this, i64_idx, then, otherwise](Simulator &simulator, size_t &pc) {
        if (simulator.get_i64(i64_idx))
            pc = this->labels.at(then);
        else 
            pc = this->labels.at(otherwise);
    });
    
    return 0;
}

antlrcpp::Any Assembler::visitJump(kqasmParser::JumpContext *ctx) {
    auto label = ctx->LABEL()->getText();
    instructions.push_back([this, label](Simulator&, size_t& pc) {
        pc = this->labels.at(label);
    });
    
    return 0;
}


antlrcpp::Any Assembler::visitInt_ex(kqasmParser::Int_exContext *ctx) {
    bool se = ctx->ex->getText() == "SE";
    std::vector<size_t> bits;
    for (auto i : ctx->BIT())
        bits.push_back(get_size_t(i->getText()));
    auto i64_idx = get_size_t(ctx->I64()->getText());
    instructions.push_back([se, bits, i64_idx](Simulator &simulator, size_t&) {
        std::int64_t value = 0;
        size_t i = 0;
        for (auto j = bits.rbegin(); j != bits.rend(); ++j) 
            value |= simulator.get_bit(*j) << i++;

        if (se and simulator.get_bit(bits[0])) for (auto j = i; j < 64; j++) 
            value |= std::int64_t{1} << j;

        simulator.set_i64(i64_idx, value);
    });
    
    return 0;
}
antlrcpp::Any Assembler::visitDump(kqasmParser::DumpContext *ctx) {
    auto qubit_idx = get_size_t(ctx->QBIT()->getText());
    instructions.push_back([qubit_idx](Simulator &simulator, size_t&) {
        simulator.dump(qubit_idx);
    });
    
    return 0;
}

antlrcpp::Any Assembler::visitPlugin(kqasmParser::PluginContext *ctx) {

    std::stringstream path_ss{plugin_path};
    std::string path;
    boost::shared_ptr<ket::bitwise_api> plugin;
    while (std::getline(path_ss, path, ':')) {
        try {
            boost::dll::fs::path lib_path(path);       
            plugin = boost::dll::import<ket::bitwise_api>(lib_path / ctx->STR()->getText(),             
                                                          "plugin",                                     
                                                          boost::dll::load_mode::append_decorations);
            break;
        } catch (boost::system::system_error &e) {
            continue;
        }
    }
    
    std::vector<size_t> qubit_idx;
    for (auto &i : ctx->QBIT()) qubit_idx.push_back(get_size_t(i->getText())); 

    auto args = ctx->ARGS() ? ctx->ARGS()->getText().substr(1, ctx->ARGS()->getText().size()-2) : "";

    instructions.push_back([plugin, qubit_idx, args](Simulator &simulator, size_t&) {
        simulator.apply_plugin(plugin, qubit_idx, args);
    });

    return 0;
}

antlrcpp::Any Assembler::visitInt_infix(kqasmParser::Int_infixContext *ctx) {
    auto result = get_size_t(ctx->I64()[0]->getText());
    auto left_idx = get_size_t(ctx->I64()[1]->getText());
    auto right_idx = get_size_t(ctx->I64()[2]->getText());
    auto op = ctx->op->getText();
    
    instructions.push_back([result, left_idx, right_idx, op](Simulator &simulator, size_t&){
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

antlrcpp::Any Assembler::visitInt_const(kqasmParser::Int_constContext *ctx) {
    std::stringstream ss;
    ss << ctx->UINT()->getText();
    std::int64_t uval;
    ss >> uval;
    std::int64_t val = ctx->SIG()? -uval : uval;
    auto idx = get_size_t(ctx->I64()->getText());
   
    instructions.push_back([idx, val](Simulator &simulator, size_t&){
        simulator.set_i64(idx, val);
    });
   
    return 0;
}

antlrcpp::Any Assembler::visitSet(kqasmParser::SetContext *ctx) {
    auto i64_in_idx = get_size_t(ctx->I64()[0]->getText());
    auto i64_value_idx = get_size_t(ctx->I64()[1]->getText());
    instructions.push_back([i64_in_idx, i64_value_idx](Simulator &simulator, size_t&) {
        simulator.set_i64(i64_in_idx, simulator.get_i64(i64_value_idx));
    });

    return 0;   
}