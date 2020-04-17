#pragma once
#include "kqasmBaseVisitor.h"
#include "../include/simulator.hpp"
#include <boost/unordered_map.hpp>
#include <vector>
#include <functional>

class Assembler : public kqasmBaseVisitor {
public:
    virtual antlrcpp::Any visitGate(kqasmParser::GateContext *ctx) override;

private:
    std::vector<std::function<void(Simulator&)>> instructions; 
    boost::unordered_map<std::string, size_t> labels;
};