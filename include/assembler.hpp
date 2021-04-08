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

#pragma once
#include "kqasmBaseVisitor.h"
#include <boost/unordered_map.hpp>
#include <vector>
#include <functional>
#include "kbw.hpp"

class Assembler : public kqasmBaseVisitor {
public:
    Assembler(block_t &blocks, next_block_t &next_block, std::string &end_block);
    
    virtual antlrcpp::Any visitBlock(kqasmParser::BlockContext *ctx) override;
    virtual antlrcpp::Any visitEnd_block(kqasmParser::End_blockContext *ctx) override;

    virtual antlrcpp::Any visitGate(kqasmParser::GateContext *ctx) override;
    virtual antlrcpp::Any visitPlugin(kqasmParser::PluginContext *ctx) override;
    virtual antlrcpp::Any visitAlloc(kqasmParser::AllocContext *ctx) override;
    virtual antlrcpp::Any visitBranch(kqasmParser::BranchContext *ctx) override;
    virtual antlrcpp::Any visitDump(kqasmParser::DumpContext *ctx) override;
    virtual antlrcpp::Any visitFree(kqasmParser::FreeContext *ctx) override;
    virtual antlrcpp::Any visitJump(kqasmParser::JumpContext *ctx) override;
    virtual antlrcpp::Any visitLabel(kqasmParser::LabelContext *ctx) override;
    virtual antlrcpp::Any visitMeasure(kqasmParser::MeasureContext *ctx) override;
    virtual antlrcpp::Any visitSet(kqasmParser::SetContext *ctx) override;
    virtual antlrcpp::Any visitBinary_op(kqasmParser::Binary_opContext *ctx) override;
    virtual antlrcpp::Any visitConst_int(kqasmParser::Const_intContext *ctx) override;
    virtual antlrcpp::Any visitCtrl(kqasmParser::CtrlContext *ctx) override;
    virtual antlrcpp::Any visitQubits_list(kqasmParser::Qubits_listContext *ctx) override;
    virtual antlrcpp::Any visitGate_name(kqasmParser::Gate_nameContext *ctx) override;
    virtual antlrcpp::Any visitArg_list(kqasmParser::Arg_listContext *ctx) override;
    virtual antlrcpp::Any visitBin_op(kqasmParser::Bin_opContext *ctx) override;

private:
    block_t &blocks; 
    next_block_t &next_block;
    std::string &end_block;
};
