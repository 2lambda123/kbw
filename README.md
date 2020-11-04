[![kbw](https://snapcraft.io//kbw/badge.svg)](https://snapcraft.io/kbw)

# Ket Bitwise Simulator

Ket Bitwise Simulator (KBW) server is the quantum computer simulator of the Ket
Quantum Programming. The simulator executes Ket Quantum Assembly (.kqasm)
generated by the libket (and the Ket quantum programming language), using the
bitwise representation [[arxiv:2004.03560](https://arxiv.org/abs/2004.03560)]. 

### Table of contents:

* [Usage](##usage)  
* [Installation](##installation)

## Usage

```shell
$ kbw -h
Ket Biswise Simulator server
============================

usage: kbw [-h] [-b 127.0.1.1] [-p 4242] [-s random] [-l]

Ket Biswise Simulator server

optional arguments:
  -h, --help     show this help message and exit
  -b 127.0.1.1   Server bind
  -p 4242        Server port
  -s random      Seed for the PRNG
  -l             Extra plugin path
```

## Installation

Available installation methods:

* [Snap](###install-using-snap) (recommended)
* [pip](###install-using-pip)
* [Source](###install-from-source)

### Install using Snap

The kbw is available in most Linux distribution through the Snap Store.

> Information on how to enable Snap on your Linux distribution is available on
> https://snapcraft.io/kbw.

To install using snap runs:
```shell
$ sudo snap install kbw --edge
```

### Install using pip

Install requirements:

* C/C++ compiler
* CMake 
* Java
* Ninja or GNU Make
* SWIG
* cURL
* unzip

To install using pip runs:
```shell
$ pip install kbw
```

> **Usage:** `python -m kbw`

### Install from source 

> Recommended if you want to be up to date without using Snap.

This method has the same install requirements as installing using pip.

To install from source runs:
```shell
$ git clone https://gitlab.com/quantum-ket/kbw.git
$ cd kbw
$ python setup.py install
```

## Ket Bitwise Plugins

To get start developing you own plugin for KBW we recommend modify the example
plugin: [example.cpp](plugin/example.cpp).  See
https://quantum-ket.gitlab.io/kbw.html for the Ket Bitwise API documentation.

To use your plugin you can re[install KBW from source](###install-from-source)
or add the compiled plugin to the extra plugin path.

### Compile example plugin

```shell
$ git clone https://gitlab.com/quantum-ket/kbw.git
$ cd kbw
$ mkdir build
$ cd build 
$ cmake -GNinja ..
$ ninja example
```

### Add plugin to path

```shell
$ kbw -l kbw/build/lib
```

> Do not execute KBW (Snap or Python) inside project directory or subdirectories.  

-----------

This project is part of the Ket Quantum Programming, see the documentation for
more information https://quantum-ket.gitlab.io.