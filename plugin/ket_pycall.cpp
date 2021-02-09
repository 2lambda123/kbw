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
