PROJECT(ViennaMesh)

cmake_minimum_required(VERSION 2.6)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

# set the include paths
INCLUDE_DIRECTORIES("netgen/")
INCLUDE_DIRECTORIES("netgen/nglib/")
INCLUDE_DIRECTORIES("netgen/libsrc/include")
INCLUDE_DIRECTORIES("netgen/libsrc/interface")

IF(CMAKE_COMPILER_IS_GNUCXX)
   ADD_DEFINITIONS(-Wall -pedantic -O2 -DOPENGL)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SET(PROGS read_fnf_mesh.cpp readtetmesh.cpp readuser.cpp writeabaqus.cpp writediffpack.cpp \ 
	writedolfin.cpp writeelmer.cpp writefeap.cpp writefluent.cpp writegmsh.cpp writejcm.cpp \ 
	writepermas.cpp writetecplot.cpp writetet.cpp writetochnog.cpp writeuser.cpp \ 
	wuchemnitz.cpp writegmsh2.cpp writeOpenFOAM15x.cpp )

FOREACH(PROG ${PROGS})
   LIST(APPEND NEWPROGS netgen/libsrc/interface/${PROG})
ENDFOREACH(PROG)

message("${NEWPROGS}")

ADD_LIBRARY(nglibsrc STATIC ${NEWPROGS})


