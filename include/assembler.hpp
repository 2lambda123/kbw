#pragma once
#include "kqasmBaseVisitor.h"
#include "code.hpp"
#include <boost/unordered_map.hpp>
#include <vector>
#include <functional>

class Assembler : public kqasmBaseVisitor {
public:
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

private:
    size_t get_size_t(std::string s) const;

    std::vector<std::function<void(Simulator&, size_t&)>> instructions; 
    boost::unordered_map<std::string, size_t> labels;
};