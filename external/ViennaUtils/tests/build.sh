
# extract number of cores on the system
CORES=`grep -c ^processor /proc/cpuinfo`
echo "cores: " $CORES

mkdir -p build/
cd build/
cmake -D CMAKE_BUILD_TYPE=Release\
      ../
make -j$CORES
cd ..



