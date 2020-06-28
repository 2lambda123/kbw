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
#include "code.hpp"
#include <boost/unordered_map.hpp>
#include <vector>
#include <functional>

class Assembler : public kqasmBaseVisitor {
public:
    Assembler(const std::string& plugin_path);
    
    virtual antlrcpp::Any visitEntry(kqasmParser::EntryContext *ctx) override;
    virtual antlrcpp::Any visitGate(kqasmParser::GateContext *ctx) override;
    virtual antlrcpp::Any visitAlloc(kqasmParser::AllocContext *ctx) override;
    virtual antlrcpp::Any visitFree(kqasmParser::FreeContext *ctx) override;
    virtual antlrcpp::Any visitMeasure(kqasmParser::MeasureContext *ctx) override;
    virtual antlrcpp::Any visitLabel(kqasmParser::LabelContext *ctx) override;
    virtual antlrcpp::Any visitBranch(kqasmParser::BranchContext *ctx) override;
    virtual antlrcpp::Any visitJump(kqasmParser::JumpContext *ctx) override;
    virtual antlrcpp::Any visitInt_ex(kqasmParser::Int_exContext *ctx) override;
    virtual antlrcpp::Any visitDump(kqasmParser::DumpContext *ctx) override;
    virtual antlrcpp::Any visitPlugin(kqasmParser::PluginContext *ctx) override;
    virtual antlrcpp::Any visitInt_infix(kqasmParser::Int_infixContext *ctx) override;
    virtual antlrcpp::Any visitInt_const(kqasmParser::Int_constContext *ctx) override;
    virtual antlrcpp::Any visitSet(kqasmParser::SetContext *ctx) override;

    std::vector<std::function<void(Simulator&, size_t&)>> instructions; 
    boost::unordered_map<std::string, size_t> labels;

private:
    size_t get_size_t(std::string s) const;
    std::string plugin_path;
};
