#pragma once
#include <boost/functional/hash.hpp>

namespace sim { 
   class Index {
   public:
      Index(size_t size = 2); // num qubits = size*64
      ~Index();

      void flip(size_t idx);
      bool is_one(size_t idx) const;
      bool is_zero(size_t idx) const;

      uint64_t operator[](size_t idx) const;
      size_t size() const;

   private:
      
      size_t _size;
      uint64_t* data;
   };

   size_t hash_value(const Index& idx);
   bool operator==(const sim::Index& a, const sim::Index& b);
}

std::ostream& operator<<(std::ostream &os, const sim::Index& idx);
