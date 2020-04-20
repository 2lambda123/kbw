#include "../include/code.hpp"

Code::Code(const std::vector<std::function<void(Simulator&, size_t&)>> &instructions, 
           const boost::unordered_map<std::string, size_t> &labels) :
    instructions{instructions},
    labels{labels}
    {}

void Code::run() {
   for (size_t pc = 0; pc < instructions.size(); pc++) {
       instructions[pc](simulator, pc);
   }
}