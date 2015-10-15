#!/bin/sh
sudo apt-get install cmake g++-4.9 build-essential libboost-all-dev
mkdir build
cd build
cmake ..
make