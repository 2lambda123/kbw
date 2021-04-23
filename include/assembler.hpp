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
