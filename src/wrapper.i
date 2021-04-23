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

%include <std_string.i>
%include <std_vector.i>
%include <stdint.i>
%include <std_complex.i>
%include <std_pair.i>

%module kbw
%{
    #include "include/kbw.hpp"
%}

%exception {
  try {
    $action
  } catch(std::exception &e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}

%typemap(out) std::vector<unsigned long long> get_dump_states
%{
  $result = PyList_New($1.size());
  for (size_t i = 0; i < $1.size(); i++) {
    PyList_SetItem($result, i, PyLong_FromUnsignedLongLong($1.at(i)));
  }
%}

%typemap(out) std::vector<std::complex<double>> get_dump_amplitude
%{
  $result = PyList_New($1.size());
  for (size_t i = 0; i < $1.size(); i++) {
    PyList_SetItem($result, i, PyComplex_FromDoubles($1.at(i).real(), $1.at(i).imag()));
  }
%}

%typemap(out) std::string get_dump
%{
  $result = PyBytes_FromStringAndSize($1.c_str(), $1.size());
%}

%include "include/kbw.hpp"
