# build the doxygen documentation
cd doc/
doxygen
cd .. 

# extract number of cores on the system
CORES=`grep -c ^processor /proc/cpuinfo`
echo "cores: " $CORES

#CORES=1

# build the library and the tests
mkdir -p build
cd build/
cmake -D CMAKE_BUILD_TYPE=Debug\
      -D CGAL_INC_DIR=/mnt/data/libraries/cgal/cgal-install/include\
      -D CGAL_LIB_DIR=/mnt/data/libraries/cgal/cgal-install/lib\
      ../
make -j$CORES
cd ..
