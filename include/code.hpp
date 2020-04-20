#pragma once 
#include "simulator.hpp"

class Code {
public:
    Code (const std::vector<std::function<void(Simulator&, size_t&)>> &instructions, 
          const boost::unordered_map<std::string, size_t> &labels);

    void run();

private:
    const std::vector<std::function<void(Simulator&, size_t&)>> &instructions; 
    const boost::unordered_map<std::string, size_t> &labels;
    Simulator simulator;
};
