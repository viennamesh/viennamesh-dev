/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Karl Rupp                                rupp@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */


#ifndef VIENNAMESH_GENERATE_HPP
#define VIENNAMESH_GENERATE_HPP

namespace viennamesh {

template<typename MesherT>
void generate(MesherT& mesher)
{
   mesher(); // TODO use traits to decouple calling mechanisms!
}

} // end namespace viennamesh   
#endif