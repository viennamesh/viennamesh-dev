License

ViennaMesh (everything in viennamesh directory) is distributed under GNU LGPL license
Netgen (everything in external/netgen) is distributed under GNU LGPLv2 license (see external/netgen/LICENSE file for more information)
Tetgen (everything in external/tetgen) is distributed under GNU Affero General Public License (see external/tetgen/LICENSE file for more information)
Triangle (everything in external/triangle) is freely distributed for non-commercial purposes (see external/triangle/LICENSE file for more information)



System requirements for the developer version:

* Boost libraries >= 1.45
* CMake 2.8 or higher
* A not-too-ancient C++ compiler
* ViennaGrid (most current checkout of master branch from github: https://github.com/viennagrid/viennagrid-dev )

Requrements for using Netgen:
* zlib
* pthreads


How to build ViennaMesh:
<pre>
$> cd viennamesh-dev
$> cd build
$> cmake ..
$> make
</pre>


How to build external programs with ViennaMesh: first build ViennaMesh like above using "make viennamesh". Then use a cmake file like:

<pre>
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(ViennaMesh_DIR "/path/to/viennamesh-dev/build/" )
set(ViennaGrid_DIR "/path/to/viennagrid-dev/build/" )
find_package(ViennaMesh)

include_directories(${VIENNAMESH_INCLUDE_DIRS})

add_executable(main main.cpp)
target_link_libraries(main ${VIENNAMESH_LIBRARIES})
</pre>
