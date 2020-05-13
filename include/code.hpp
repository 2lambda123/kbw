#pragma once 
#include "simulator.hpp"
#include <boost/asio/thread_pool.hpp>

class Code {
public:
    Code (const std::vector<std::function<void(Simulator&, size_t&)>> &instructions, 
          const boost::unordered_map<std::string, size_t> &labels,
          boost::asio::thread_pool * t_pool);

    void run();

    std::string get_results();

private:
    const std::vector<std::function<void(Simulator&, size_t&)>> &instructions; 
    const boost::unordered_map<std::string, size_t> &labels;
    Simulator simulator;
};
