#!/bin/bash

# configure waf
#
./waf configure 

# build
#   note: waf uses automatically all available cores
./waf --progress

# strip executable and copy to bin folder
#
strip build/hull_adaptor
cp build/hull_adaptor ../bin/

