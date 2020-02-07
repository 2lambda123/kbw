#pragma once
#include "../src/parser.hpp"
#include "bitwise.hpp"
#include <vector>
#include <boost/unordered_map.hpp>

# define YY_DECL yy::parser::symbol_type yylex(Driver& drv)
YY_DECL;

class Driver {
 public:
   Driver(size_t seed = 42);

   void add_qubit(size_t qubit);   
   void add_dirty(size_t qubit);   

   size_t get_qubit(size_t qubit);   

   void add_bit(size_t bit);
   size_t get_bit(size_t bit);

   void gate(const std::string& gate, const std::vector<size_t>& qubits, bool adj=false, const std::vector<size_t>& ctrl={});

   void measure(size_t qubit, size_t bit);
   void free(size_t qubit);
   void freedirty(size_t qubit);

   bool get_measure(size_t bit);

   void dump();

   int parse();

   yy::location location;

 private:
   Bitwise simulator;
   std::vector<size_t> clean_qubits;
   std::vector<size_t> dirty_qubits;
   size_t classic_counter;
   boost::unordered_map<size_t, size_t> qubit_map;
   boost::unordered_map<size_t, size_t> bit_map;
   boost::unordered_map<size_t, int> measurements;
};