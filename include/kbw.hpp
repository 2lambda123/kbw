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
#include "simulator.hpp"

#ifndef SWIG
inline std::string plugin_path;

using block_t = boost::unordered_map<std::string, std::function<void(Simulator&)>>;
using next_block_t = boost::unordered_map<std::string, std::function<std::string(Simulator&)>>;
#endif

void set_plugin_path(const std::string &path);
std::string get_plugin_path();

void set_seed(int seed);

class kbw {
public:
    kbw (const std::string& kqasm);

    void run();

    std::string get_results();
    std::int64_t get_result(size_t idx);
    std::int64_t results_len();

    std::string get_dump(size_t idx);
    std::string dump_to_fs(size_t idx);
    std::int64_t dumps_len();

private:
    block_t blocks; 
    next_block_t next_block;
    std::string end_block;
    Simulator simulator;
};
