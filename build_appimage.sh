#! /bin/bash
mkdir -p build
cd build; conan install ..; cmake ..; make install DESTDIR=AppDir; linuxdeploy --appdir AppDir --output appimage