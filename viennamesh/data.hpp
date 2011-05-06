/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_DATA_HPP
#define VIENNAMESH_DATA_HPP

#include <map>

namespace viennamesh {

namespace data {
struct orient        {};
struct seg_orient    {};

struct seg_orient_map
{
   // mapping of a segment to a specific orientation flag
   //
   typedef std::map<std::size_t, int>  type;
};

}
}

// provide viennadata with configs for the meta-keys
//
namespace viennadata
{
  namespace config
  {
    
    template <>
    struct key_dispatch<viennamesh::data::orient>
    {
      typedef type_key_dispatch_tag    tag;
    };
    template <>
    struct key_dispatch<viennamesh::data::seg_orient>
    {
      typedef type_key_dispatch_tag    tag;
    };    
  }
}


#endif

