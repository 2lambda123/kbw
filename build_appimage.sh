#! /bin/bash
mkdir -p build
cd build; cmake ..; make install DESTDIR=AppDir; linuxdeploy --appdir AppDir --output appimage