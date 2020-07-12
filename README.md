[![kbw](https://snapcraft.io//kbw/badge.svg)](https://snapcraft.io/kbw)

# Ket Bitwise Simulator

Ket Bitwise Simulator (kbw) is the quantum computer simulator of the Ket
quantum programming. The simulator executes Ket Quantum Assembly codes (.kqasm)
generated by the libket (and the Ket quantum programming language), using the
bitwise representation [[arxiv:2004.03560](https://arxiv.org/abs/2004.03560)]. 

## Installation

The kbw is available in most Linux distribution through the Snap Store.

Information on how to enable Snap on your Linux distribution is available on
https://snapcraft.io/kbw.

```bash
sudo snap install kbw --edge
```

### Install from source 

To install from source, follow the commands:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
sudo make install
```

-----------

This project is part of the Ket Quantum Programming, see the documentation for
more information http://ket.readthedocs.io.