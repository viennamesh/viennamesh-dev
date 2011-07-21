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
#include <cassert>

#include "viennagrid/point.hpp"
#include "viennautils/geometry/inclusion.hpp"

template<typename VectorT>
void test_point_in_triangle()
{
   { // is inside
   std::vector<VectorT> pntvec;
   VectorT p(0.5,0.5,1.0);
   pntvec.push_back(VectorT(0.0,0.0,1.0));
   pntvec.push_back(VectorT(1.0,0.0,1.0));
   pntvec.push_back(VectorT(0.5,1.0,1.0));      
   assert( (viennautils::inclusion<0,2>()(p, pntvec)) );
   }   
   { // is outside
   std::vector<VectorT> pntvec;   
   VectorT p(0.0,0.5,1.0);
   pntvec.push_back(VectorT(0.0,0.0,1.0));
   pntvec.push_back(VectorT(1.0,0.0,1.0));
   pntvec.push_back(VectorT(0.5,1.0,1.0));    
   assert( !(viennautils::inclusion<0,2>()(p, pntvec)) );
   }   
   { // is on a corner -> is inside
   std::vector<VectorT> pntvec;   
   VectorT p(0.0,0.0,1.0);
   pntvec.push_back(VectorT(0.0,0.0,1.0));
   pntvec.push_back(VectorT(1.0,0.0,1.0));
   pntvec.push_back(VectorT(0.5,1.0,1.0));     
   assert( (viennautils::inclusion<0,2>()(p, pntvec)) );
   }
   { // is outside
   std::vector<VectorT> pntvec;   
   VectorT p(-1.0,0.0,1.0);
   pntvec.push_back(VectorT(0.0,0.0,1.0));
   pntvec.push_back(VectorT(1.0,0.0,1.0));
   pntvec.push_back(VectorT(0.5,1.0,1.0));     
   assert( !(viennautils::inclusion<0,2>()(p, pntvec)) );
   }
   { // is outside (on a different plane - different z coo)
   std::vector<VectorT> pntvec;   
   VectorT p(0.0,0.0,-1.0);
   pntvec.push_back(VectorT(0.0,0.0,1.0));
   pntvec.push_back(VectorT(1.0,0.0,1.0));
   pntvec.push_back(VectorT(0.5,1.0,1.0));     
   assert( !(viennautils::inclusion<0,2>()(p, pntvec)) );
   }   
}

template<typename VectorT>
void test_point_on_line()
{
   { // above the line segment
   std::vector<VectorT> pntvec;      
   VectorT p(1,2,0);
   pntvec.push_back(VectorT(1,1,0));
   pntvec.push_back(VectorT(2,1,0));
   assert( !(viennautils::inclusion<0,1>()(p, pntvec)) );
   }
   { // on the line segment
   std::vector<VectorT> pntvec;      
   VectorT p(1.5,1,0);
   pntvec.push_back(VectorT(1,1,0));
   pntvec.push_back(VectorT(2,1,0));
   assert( (viennautils::inclusion<0,1>()(p, pntvec)) );
   }
   { // on the infinite line but _not_ on the line segment
   std::vector<VectorT> pntvec;      
   VectorT p(0.5,1,0);
   pntvec.push_back(VectorT(1,1,0));
   pntvec.push_back(VectorT(2,1,0));
   assert( !(viennautils::inclusion<0,1>()(p, pntvec)) );
   }
}


int main(int argc, char * argv[])
{
   typedef viennagrid::point<double, 3>   vector_type;

   test_point_in_triangle<vector_type>();
   test_point_on_line<vector_type>();

   return 0;
}



