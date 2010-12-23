// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 2006-2007 Sean Mauch. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// This software is provided "as is" and any express or implied warranties
// are disclaimed.  In no event shall the author be liable for any damages
// arising from the use of this software.
// ----------------------------------------------------------------------------

#if !defined(__geom_h__)
#define __geom_h__

#include "geom/grid.h"
#include "geom/kernel.h"
#include "geom/mesh.h"
#include "geom/orq.h"
// Deprecated.
//#include "geom/orq3d.h"
#include "geom/polytope.h"
#include "geom/spatialIndexing.h"
#include "geom/tree.h"

BEGIN_NAMESPACE_GEOM

/*!
\mainpage Computational Geometry Package

\author Sean Mauch, http://www.its.caltech.edu/~sean/ , at(sean, dot(caltech, edu))

This package provides computational geometry algorithms and data structures.
This is a templated class library.  Thus
there is no library to compile or link with.  Just include the
appropriate header files in your application code when you compile.
All classes and functions are in the \c geom namespace.

The geom package is composed of five sub-packages:
- The \ref geom_kernel contains geometric primitives like line segments
  and planes.
- The \ref geom_mesh has simplicial mesh data structures and algorithms for
generating and optimizing meshes.
- The \ref geom_polytope has polygons and polyhedra.
- The \ref geom_grid has grids which store objects and support mathematical 
operations.
- The \ref geom_orq has data structures for doing orthogonal range queries.
- The \ref geom_spatialIndexing has an orthtree (quadtree, octree, etc.) data structure.
- The \ref geom_tree has a bounding box tree.

Note that I don't have a point or vector class.  For this
functionality I use the ads::FixedArray class in my 
Algorithms and Data Structures package.  Here I just define mathematical 
operations on them.
*/

END_NAMESPACE_GEOM

#endif

// End of file.
