/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_TOPOLOGY_ELEMENTS_HH)
#define GSSE_TOPOLOGY_ELEMENTS_HH

namespace gsse { namespace topology { 


// 0D: vertex 
// 1D: edge / segment
// 2D: face / (D-Dt)-facet
// 3D: cell 
 
enum Element_Type { ET_SEGM = 1,
		    ET_TRIG = 10, ET_QUAD = 11, 
		    ET_TET = 20, ET_PYRAMID = 21, ET_PRISM = 22, ET_HEX = 24 };

// ###
struct segment;

struct triangle;
struct quadrilateral;

struct tetrahedron;
struct hexahedron;
struct prism;
struct pyramid;


} //namespace topology
} //namespace gsse

#endif


