/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#include <iostream>

#include "viennamesh/mesh.hpp"
#include "viennamesh/interfaces/triangle.hpp"   // a specific mesher

#include <boost/array.hpp>


int main(int argc, char * argv[])
{
   typedef boost::array< double, 2 >     point_type;
   point_type pnt1 = {{0,0}};
   point_type pnt2 = {{1,0}};   
   point_type pnt3 = {{1,1}};
   point_type pnt4 = {{0,1}};
   point_type pnt5 = {{2,0}};      
   point_type pnt6 = {{2,1}};         
   
   point_type pntreg1 = {{0.5,0.5}};
   point_type pntreg2 = {{1.5,0.5}};   
   
   typedef boost::array< std::size_t, 2 >    constraint_type;
   constraint_type c1 = {{0,1}};
   constraint_type c2 = {{1,2}};
   constraint_type c3 = {{2,3}};
   constraint_type c4 = {{3,0}};   
   constraint_type c5 = {{1,4}};
   constraint_type c6 = {{4,5}};
   constraint_type c7 = {{5,2}};
   constraint_type c8 = {{2,1}};      

   
   typedef viennamesh::triangle<double>  mesher_type;
   mesher_type mesher;
   typedef viennamesh::add<mesher_type>  add_type;
   add_type add(mesher);

   add(viennamesh::object<viennamesh::key::point>(pnt1));
   add(viennamesh::object<viennamesh::key::point>(pnt2));
   add(viennamesh::object<viennamesh::key::point>(pnt3));
   add(viennamesh::object<viennamesh::key::point>(pnt4));
   add(viennamesh::object<viennamesh::key::point>(pnt5));
   add(viennamesh::object<viennamesh::key::point>(pnt6));   
   
   add(viennamesh::object<viennamesh::key::constraint>(c1));   
   add(viennamesh::object<viennamesh::key::constraint>(c2));   
   add(viennamesh::object<viennamesh::key::constraint>(c3));   
   add(viennamesh::object<viennamesh::key::constraint>(c4));   
   add(viennamesh::object<viennamesh::key::constraint>(c5));   
   add(viennamesh::object<viennamesh::key::constraint>(c6));      
   add(viennamesh::object<viennamesh::key::constraint>(c7));   
   add(viennamesh::object<viennamesh::key::constraint>(c8));      
   
   add(viennamesh::object<viennamesh::key::region>(pntreg1));
   add(viennamesh::object<viennamesh::key::region>(pntreg2));   
   
   add(viennamesh::object<viennamesh::key::method>(viennamesh::method::constrained_delaunay()));
   //add(viennamesh::object<viennamesh::key::size>(1.0));   
   
   viennamesh::generate(mesher);
   
   return 0;
}

