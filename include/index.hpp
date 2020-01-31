#pragma once
#include <boost/functional/hash.hpp>
#include <array>

namespace sim { 
   class Index {
   public:
      void flip(size_t idx);
      bool is_one(size_t idx) const;
      bool is_zero(size_t idx) const;

      uint64_t operator[](size_t idx) const;
      size_t size() const;

   private:
      
      const static size_t _size = 2;
      std::array<uint64_t, _size> data;
   };

   size_t hash_value(const Index& idx);
   bool operator==(const sim::Index& a, const sim::Index& b);
}

std::ostream& operator<<(std::ostream &os, const sim::Index& idx);
