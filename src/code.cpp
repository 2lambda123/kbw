#include "../include/code.hpp"

Code::Code(const std::vector<std::function<void(Simulator&, size_t&)>> &instructions, 
           const boost::unordered_map<std::string, size_t> &labels,
           boost::asio::thread_pool * t_pool) :
    instructions{instructions},
    labels{labels},
    simulator{t_pool}
    {}

void Code::run() {
   for (size_t pc = 0; pc < instructions.size(); pc++) {
       instructions[pc](simulator, pc);
   }
}

std::string Code::get_results() {
    return simulator.get_results();
}