#!/bin/sh

# get the operating system string
OS=`uname`

# print detected operating system
echo "operating system: '"$OS"'"

# default value for building jobs
CORES=1

# OS specific available number of cores extraction
if   [ "$OS" == "Darwin" ]; then
   CORES=`sysctl -n hw.ncpu`
elif [ "$OS" == "Linux" ]; then
   CORES=`grep -c ^processor /proc/cpuinfo`
else 
   echo "no available CPU core extraction available"
fi

# print actual number of cores
echo "number of cores: " $CORES

# build the library and the tests
mkdir -p build
cd build/
cmake ../
make -j$CORES
make install
cd ..




