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

//! \file ket_bitwise.hpp

#pragma once
#include <iostream>
#include <boost/config.hpp>
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#include <complex>
#include <string>
#include <vector>
#include <array>

namespace ket { 
    using complex = std::complex<double>; //!< Complex number.

    using ctrl_list = std::vector<size_t>; //!< List of qubits of control.

    /*! \brief Key for the quantum state hashmap.
     *  \class Index 
     */ 
    class Index {
    public:

        //! Bitwise-or.
        Index operator|(const Index& other) const;

        //! Flip the bit in the idx possition. 
        void flip(size_t idx);
        
        //! True if the bit in the idx possition is 1. 
        bool is_one(size_t idx) const;

        //! True if the bit in the idx possition is 0. 
        bool is_zero(size_t idx) const;
      
        //! Get a 64 bits slice: [64*idx : 64*(idx+1)] (const).
        uint64_t operator[](size_t idx) const;

        //! Get a 64 bits slice: [64*idx : 64*(idx+1)].
        uint64_t& operator[](size_t idx);
      
        //! Less than comparison for the hashmap indexation
        bool operator<(const Index& other) const;
      
        const static size_t size = 3; //!< Size of the Index: size*64.
 
    private:
        std::array<uint64_t, size> data;
    };
 
    //! Hash of an Index.
    size_t hash_value(const Index& idx);

    //! True if Index a and b are equal.
    bool operator==(const ket::Index& a, const ket::Index& b);
    
    //! Print the Index.
    std::ostream& operator<<(std::ostream &os, const ket::Index& idx);

    using map = boost::unordered_map<Index, complex>; //<! Quantum state hashmap.

    /*! \brief Store and manipulate the quantum state hashmap.
     *  \class Bitwise
     */
    class Bitwise {
    public:
    
        //! Initialize a hashmap in the state \f$\left|0\right>\f$.
        Bitwise();

        //! Tensor product between hashmaps.
        Bitwise(const Bitwise& a, const Bitwise& b);
       
        //! Apply a Pauli X gate with optional quibits of control. 
        void x(size_t idx, const ctrl_list& ctrl = {});
        
        //! Apply a Pauli Y gate with optional quibits of control. 
        void y(size_t idx, const ctrl_list& ctrl = {});

        //! Apply a Pauli Z gate with optional quibits of control. 
        void z(size_t idx, const ctrl_list& ctrl = {});

        //! Apply a Hadamard gate with optional quibits of control. 
        void h(size_t idx, const ctrl_list& ctrl = {});

        //! Apply a Phase gate with optional quibits of control. 
        void s(size_t idx, const ctrl_list& ctrl = {});

        //! Apply a Phase Dagger gate with optional quibits of control. 
        void sd(size_t idx, const ctrl_list& ctrl = {});

        //! Apply a T gate with optional quibits of control. 
        void t(size_t idx, const ctrl_list& ctrl = {});

        //! Apply a T Dagger gate with optional quibits of control. 
        void td(size_t idx, const ctrl_list& ctrl = {});

        //! Apply a CNOT gate with optional quibits of control. 
        void cnot(size_t ctrl, size_t target, const ctrl_list& ctrl2 = {});

        //! Apply a U1 gate with optional quibits of control. 
        void u1(double lambda, size_t idx, const ctrl_list& ctrl={});
        
        //! Apply a U2 gate with optional quibits of control. 
        void u2(double phi, double lambda, size_t idx, const ctrl_list& ctrl={});

        //! Apply a U3 gate with optional quibits of control. 
        void u3(double theta, double phi, double lambda, size_t idx, const ctrl_list& ctrl={});

        //! Measure a qubit.
        int measure(size_t idx);

        //! Apply a SWAP gate.
        void swap(size_t a, size_t b);

        //! Get the quantum state hashmap. 
        map& get_map();

    private:
        map qbits; //<! Quantum state hashmap.

        friend std::ostream& operator<<(std::ostream &os, const ket::Bitwise& q);
    };
    
    //! Print the quantum state hashmap. 
    std::ostream& operator<<(std::ostream &os, const ket::Bitwise& q);
    
    /*! \brief Abstract class for plugin interface.
     *  \class bitwise_api
     */
    class BOOST_SYMBOL_VISIBLE bitwise_api {
    public:
        
        /* \brief Execute plugin.
         *  
         *  The plugin must modify the qbits param.  We suggest to populate a
         *  new map and perfor a swap at the end. 
         * 
         *  The input qubits are ordend from size-1 to 0 in the hashmap.
         *  Operate with qubits >= size is undefined behavior.
         * 
         *  \param qbits Quantum state hashmap
         *  \param size  Number of qubits affected.
         *  \param args  Argument string.
         */
        virtual void run(map& qbits, size_t size, std::string args) = 0;

    };
    
}

/*! \def bitwise_plugin(plugin_class)
 *  \brief Expose the class to be load as a plugin.
 */
#define bitwise_plugin(plugin_class) extern "C" BOOST_SYMBOL_EXPORT plugin_class plugin; plugin_class plugin
