/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_TRAITS_HPP
#define VIENNAMESH_TRAITS_HPP

namespace viennamesh {

namespace traits {   
   
template<typename T>
struct datastructure
{
   typedef typename T::datastructure_type type;
};

} // end namespace traits   
} // end namespace viennamesh

#endif
