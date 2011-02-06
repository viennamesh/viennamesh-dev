#!/bin/bash

echo ""
echo "-------------------------------------------------------------------" 
echo "-- ViennaMesh 1.0  2010                                          --" 
echo "-------------------------------------------------------------------" 
echo "--  developed by:                                                --" 
echo "--    Franz Stimpfl, Rene Heinzl, Philipp Schwaha                --" 
echo "--  maintained by:                                               --" 
echo "--    Josef Weinbub, Johann Cervenka                             --" 
echo "--  Institute for Microelectronics, TU Wien, Austria             --"
echo "--  http://www.iue.tuwien.ac.at                                  --"  
echo "-------------------------------------------------------------------" 

echo ""
echo "-------------------------------------------------------------------"
echo "## creating bin/ folder .. "
echo "-------------------------------------------------------------------"
rm -rf bin/
mkdir bin/

echo ""
echo "-------------------------------------------------------------------"
echo "## building hull converter .. "
echo "-------------------------------------------------------------------"
cd hull_converter/
./build.sh
cd ..

echo ""
echo "-------------------------------------------------------------------"
echo "## building hull adaptor .. "
echo "-------------------------------------------------------------------"
cd hull_adaptor/
./build.sh
cd ..

echo ""
echo "-------------------------------------------------------------------"
echo "## building volume mesher .. "
echo "-------------------------------------------------------------------"
cd volume_mesher/
./build.sh
cd ..

echo ""
echo "-------------------------------------------------------------------"
echo "## building mesh classifier .. "
echo "-------------------------------------------------------------------"
cd mesh_classifier/
./build.sh
cd ..

echo ""
echo "-------------------------------------------------------------------"
echo "## building hull orienter .. "
echo "-------------------------------------------------------------------"
cd hull_orienter/
./build.sh
cd ..

echo ""
echo "-------------------------------------------------------------------"
echo "## done .. "
echo "## executables are in bin/"
echo "-------------------------------------------------------------------"
