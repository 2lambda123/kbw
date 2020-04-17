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
      
        bool operator<(const Index& other) const;
      
        const static size_t size = 2;
 
    private:
        std::array<uint64_t, size> data;
        
        friend Index operator|(const Index& l, const Index& r);
    };
 
    size_t hash_value(const Index& idx);
    bool operator==(const sim::Index& a, const sim::Index& b);
}

std::ostream& operator<<(std::ostream &os, const sim::Index& idx);