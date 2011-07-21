/* =============================================================================
   Copyright (c) 2010, 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                ViennaUtils - The Vienna Utilities Library
                             -----------------

   authors:    Josef Weinbub                      weinbub@iue.tuwien.ac.at

   license:    see file LICENSE in the base directory
============================================================================= */

#include <iostream>
#include <vector>
#include <cassert>

#include "boost/array.hpp"

#include "viennautils/geometry/barycenter.hpp"
#include "viennautils/contio.hpp"
#include "viennautils/numeric.hpp"


// testing .) std::vector container
//         .) triangle embedded in 2d geometrical space
void test_stdvector_triangle_2d()
{
   typedef std::vector<double>   vector_type;
   vector_type  p1, p2, p3;
   p1.resize(2);
   p2.resize(2);
   p3.resize(2);
   
   p1[0] = 0;
   p1[1] = 0;
   p2[0] = 0.5;
   p2[1] = 0;
   p3[0] = 0.25;
   p3[1] = 0.3;
   
   vector_type bc = viennautils::barycenter(p1, p2, p3);
   assert( viennautils::equals(bc[0], 0.25) );   
   assert( viennautils::equals(bc[1], 0.1) );   
   //std::cout << "bc@stdvector-triangle-2d: " << bc;
}

// testing .) boost::array container
//         .) triangle embedded in 2d geometrical space
void test_boostarray_triangle_2d()
{
   typedef boost::array<double, 2>  vector_type;
   vector_type p1, p2, p3;
   
   p1[0] = 0;
   p1[1] = 0;
   p2[0] = 0.5;
   p2[1] = 0;
   p3[0] = 0.25;
   p3[1] = 0.3;

   vector_type bc = viennautils::barycenter(p1, p2, p3);
   assert( viennautils::equals(bc[0], 0.25) );   
   assert( viennautils::equals(bc[1], 0.1) );   
   //std::cout << "bc@boostarray-triangle-2d: " << bc;
}

// testing .) std::vector container
//         .) triangle embedded in 3d geometrical space
void test_stdvector_triangle_3d()
{
   typedef std::vector<double>   vector_type;
   vector_type  p1, p2, p3;
   p1.resize(3);
   p2.resize(3);
   p3.resize(3);
   
   p1[0] = 0;
   p1[1] = 0;
   p1[2] = 0;
   p2[0] = 0.5;
   p2[1] = 0;
   p2[2] = 0;   
   p3[0] = 0.25;
   p3[1] = 0.3;
   p3[2] = 0;   
   
   vector_type bc = viennautils::barycenter(p1, p2, p3);   
   assert( viennautils::equals(bc[0], 0.25) );   
   assert( viennautils::equals(bc[1], 0.1) );   
   assert( viennautils::equals(bc[2], 0.0) );   
   //std::cout << "bc@stdvector-triangle-3d: " << bc;
}

// testing .) std::vector container
//         .) triangle embedded in 3d geometrical space
void test_boostarray_triangle_3d()
{
   typedef boost::array<double, 3>  vector_type;
   vector_type p1, p2, p3;
   
   p1[0] = 0;
   p1[1] = 0;
   p1[2] = 0;
   p2[0] = 0.5;
   p2[1] = 0;
   p2[2] = 0;   
   p3[0] = 0.25;
   p3[1] = 0.3;
   p3[2] = 0;   
   
   vector_type bc = viennautils::barycenter(p1, p2, p3);   
   assert( viennautils::equals(bc[0], 0.25) );   
   assert( viennautils::equals(bc[1], 0.1) );   
   assert( viennautils::equals(bc[2], 0.0) );   
   
   //std::cout << "bc@boostarray-triangle-3d: " << bc;
}

// testing .) std::vector container
//         .) tetrahedron
void test_stdvector_tetrahedron_3d()
{
   typedef std::vector<double>   vector_type;
   vector_type  p1, p2, p3, p4;
   p1.resize(3);
   p2.resize(3);
   p3.resize(3);
   p4.resize(4);
   
   p1[0] = 0;
   p1[1] = 0;
   p1[2] = 0;
   p2[0] = 0.5;
   p2[1] = 0;
   p2[2] = 0;   
   p3[0] = 0.25;
   p3[1] = 0.3;
   p3[2] = 0;   
   p4[0] = 0.25;
   p4[1] = 0.3;
   p4[2] = 1;      
   
   vector_type bc = viennautils::barycenter(p1, p2, p3, p4);   
   assert( viennautils::equals(bc[0], 0.25) );   
   assert( viennautils::equals(bc[1], 0.15) );   
   assert( viennautils::equals(bc[2], 0.25) );   
   //std::cout << "bc@stdvector-tetrahedron-3d: " << bc;
}

// testing .) std::vector container
//         .) tetrahedron
void test_boostarray_tetrahedron_3d()
{
   typedef boost::array<double, 3>  vector_type;
   vector_type p1, p2, p3, p4;
   
   p1[0] = 0;
   p1[1] = 0;
   p1[2] = 0;
   p2[0] = 0.5;
   p2[1] = 0;
   p2[2] = 0;   
   p3[0] = 0.25;
   p3[1] = 0.3;
   p3[2] = 0;   
   p4[0] = 0.25;
   p4[1] = 0.3;
   p4[2] = 1;      
   
   vector_type bc = viennautils::barycenter(p1, p2, p3, p4);   
   assert( viennautils::equals(bc[0], 0.25) );   
   assert( viennautils::equals(bc[1], 0.15) );   
   assert( viennautils::equals(bc[2], 0.25) );      
   //std::cout << "bc@boostarray-tetrahedron-3d: " << bc;
}

int main(int argc, char * argv[])
{
   test_stdvector_triangle_2d();
   test_boostarray_triangle_2d();
   
   test_stdvector_triangle_3d();
   test_boostarray_triangle_3d();   
   
   test_stdvector_tetrahedron_3d();
   test_boostarray_tetrahedron_3d();      
   return 0;
}



