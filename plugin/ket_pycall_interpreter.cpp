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

#include <Python.h>
#include <iostream>
#include <vector>

int main() {
    
    // Parse args
    size_t argc;
    std::vector<size_t> argv;
    std::string func_name;
    std::string func_b64;
    
    std::cin >> argc;
    for (auto i = 0ul; i < argc; i++) {
        size_t tmp;
        std::cin >> tmp;
        argv.push_back(tmp);
    }
    
    std::cin >> func_name;
    std::cin >> func_b64;

    // Init py
    Py_Initialize();

    PyRun_SimpleString("from base64 import b64decode\n\
def exec64(code64):\n\
    code = b64decode(code64).decode()\n\
    exec(code, globals())\n\
    ");
    

    auto __main__ = PyImport_AddModule("__main__");
    auto exec64 = PyObject_GetAttrString(__main__, "exec64");

    auto args_exec64 = PyTuple_New(1);
    auto input_exec64 = PyUnicode_FromString(func_b64.c_str());
    PyTuple_SetItem(args_exec64, 0, input_exec64);

    PyObject_CallObject(exec64, args_exec64);

    auto func = PyObject_GetAttrString(__main__, func_name.c_str());
    

    // Call

    size_t n;
    std::cin >> n;

    for (auto i = 0ul; i < n; i++) {

        auto args_func = PyTuple_New(argc);

        size_t i0;
        std::cin >> i0;

        for (int i = argc; i > 0; i--) {
            auto value = i0 & ((1ul << argv[i-1])-1);
            i0 >>= argv[i-1];
            PyTuple_SetItem(args_func, i-1, PyLong_FromSize_t(value));
        }
        
        // call
        auto outputs = PyObject_CallObject(func, args_func);
        Py_DecRef(args_func);


        // get outputs
        size_t out = 0;
        for (int i = 0; i < argc; i++) {
            auto item = PyTuple_GetItem(outputs, i);
            out <<= argv[i];
            out |= PyLong_AsSize_t(item);
        }

        Py_DecRef(outputs);

        std::cout << out << " ";
        
    }

    Py_Finalize();

    return 0;
}