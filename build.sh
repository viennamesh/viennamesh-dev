# build the doxygen documentation
cd doc/
doxygen
cd .. 

# extract number of cores on the system
CORES=`grep -c ^processor /proc/cpuinfo`
echo "cores: " $CORES

# build the library and the tests
mkdir -p build
cd build/
cmake ..
make -j$CORES
cd ..
