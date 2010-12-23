#!/bin/bash

# configure waf
#
./waf configure 

# build
#   note: waf uses automatically all available cores
./waf --progress 

# strip executable and copy to bin folder
#
strip build/mesh_classifier
cp build/mesh_classifier ../bin/
