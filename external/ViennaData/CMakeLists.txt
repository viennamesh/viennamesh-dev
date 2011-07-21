
PROJECT(viennadata)

cmake_minimum_required(VERSION 2.4)

#if you don't want the full compiler output, remove or comment the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#specify include and source directory
INCLUDE_DIRECTORIES(".")


#add definitions, compiler switches, etc.
IF(DEFINED CMAKE_BUILD_TYPE)
 SET (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
ELSE()
 SET (CMAKE_BUILD_TYPE Release)
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX)
   #ADD_DEFINITIONS(-Wall -O0 -g)
   ADD_DEFINITIONS(-Wall -pedantic -O3)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_EXECUTABLE(oopbench            examples/benchmarks/oop.cpp)
ADD_EXECUTABLE(densebench          examples/benchmarks/dense.cpp)
ADD_EXECUTABLE(sparseaddressbench  examples/benchmarks/sparse-address.cpp)
ADD_EXECUTABLE(sparsecustombench   examples/benchmarks/sparse-custom.cpp)

ADD_EXECUTABLE(access              examples/tutorials/access.cpp)
ADD_EXECUTABLE(copy                examples/tutorials/copy.cpp)
ADD_EXECUTABLE(move                examples/tutorials/move.cpp)
ADD_EXECUTABLE(erase               examples/tutorials/erase.cpp)
ADD_EXECUTABLE(find                examples/tutorials/find.cpp)
ADD_EXECUTABLE(customid            examples/tutorials/custom_id.cpp)
ADD_EXECUTABLE(densedata           examples/tutorials/dense_data.cpp)
ADD_EXECUTABLE(keydispatch         examples/tutorials/key_dispatch.cpp)
ADD_EXECUTABLE(defaulttype         examples/tutorials/default_data_type.cpp)
