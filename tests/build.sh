
CURRENT=$PWD

# extract number of cores on the system
CORES=`grep -c ^processor /proc/cpuinfo`
echo "cores: " $CORES

#CORES=1

mkdir -p build/
cd build/
cmake ..
make -j$CORES



