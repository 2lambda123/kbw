#pragma once
#include <iostream>
#include <boost/config.hpp>
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#include <complex>
#include <string>
#include <vector>
#include <array>
#include <mutex>

namespace ket { 
    using complex = std::complex<double>;
    using ctrl_list = std::vector<size_t>;

    class Index {
    public:
        Index operator|(const Index& other) const;

        void flip(size_t idx);
        bool is_one(size_t idx) const;
        bool is_zero(size_t idx) const;
      
        uint64_t operator[](size_t idx) const;
        uint64_t& operator[](size_t idx);
      
        bool operator<(const Index& other) const;
      
        const static size_t size = 3;
 
    private:
        std::array<uint64_t, size> data;
    };
 
    size_t hash_value(const Index& idx);
    bool operator==(const ket::Index& a, const ket::Index& b);
    
    std::ostream& operator<<(std::ostream &os, const ket::Index& idx);

    using map = boost::unordered_map<Index, complex>; 

    class Bitwise {
    public:
        Bitwise();
        Bitwise(const Bitwise& a, const Bitwise& b);
       
        void x(size_t idx, const ctrl_list& ctrl = {});
        void y(size_t idx, const ctrl_list& ctrl = {});
        void z(size_t idx, const ctrl_list& ctrl = {});
        void h(size_t idx, const ctrl_list& ctrl = {});
        void s(size_t idx, const ctrl_list& ctrl = {});
        void sd(size_t idx, const ctrl_list& ctrl = {});
        void t(size_t idx, const ctrl_list& ctrl = {});
        void td(size_t idx, const ctrl_list& ctrl = {});
        void cnot(size_t ctrl, size_t target, const ctrl_list& ctrl2 = {});
        void u1(double lambda, size_t idx, const ctrl_list& ctrl={});
        void u2(double phi, double lambda, size_t idx, const ctrl_list& ctrl={});
        void u3(double theta, double phi, double lambda, size_t idx, const ctrl_list& ctrl={});
        int measure(size_t idx);
        void swap(size_t a, size_t b);

        map& get_map();

        std::mutex m;

    private:
        map qbits;

        friend std::ostream& operator<<(std::ostream &os, const ket::Bitwise& q);
    };
    
    std::ostream& operator<<(std::ostream &os, const ket::Bitwise& q);
    
    class BOOST_SYMBOL_VISIBLE bitwise_api {
    public:

        virtual void run(map& qbits, size_t size, std::string args) = 0;

    };
    
}


#define bitwise_plugin(plugin_class) extern "C" BOOST_SYMBOL_EXPORT plugin_class plugin; plugin_class plugin
