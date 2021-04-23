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

#include "ket_bitwise.hpp"
#include <sstream>
#include <boost/process.hpp>
#include <boost/process/async.hpp>
#include <boost/asio.hpp>
#include <cstdlib>

using namespace ket;

inline std::string call(const std::string& command, const std::string& in) {
    boost::asio::io_service ios;

    std::future<std::string> outdata;

    boost::process::async_pipe pipe(ios);
    boost::process::child c(command+std::string{},
                            boost::process::std_out > outdata,
                            boost::process::std_in < pipe, ios);

    boost::asio::async_write(pipe, boost::process::buffer(in),
                            [&](boost::system::error_code, size_t) { pipe.close(); });

    ios.run();

    return outdata.get();
}


class ket_pycall : public bitwise_api {
public:
    void run(map &qbits, size_t size, std::string args, bool adj, size_t ctrl) const {
        
        std::stringstream in;
        
        in << args << ' ' << qbits.size() << ' ';

        std::vector<map::iterator> it_list;
        for (auto i = qbits.begin(); i != qbits.end(); ++i) {
            it_list.push_back(i);
            in << i->first[0] << ' ';
        }
        
        std::stringstream out{call(getenv("KET_PYCALL"), in.str())};
        
        map new_map;
 
        for (auto i = 0; i < qbits.size(); i++) {
            auto j = it_list[i]->first;
            out >> j[0];
            new_map[j] = it_list[i]->second; 
        }
        
        qbits.swap(new_map);

    }
            

};

bitwise_plugin(ket_pycall);
