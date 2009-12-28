#!/bin/sh
echo ">> Generating translations..."
lrelease znotes.pro
echo ">> Configuring..."
qmake
echo ">> Compiling..."
make
echo ">> Installing..."
sudo make install
