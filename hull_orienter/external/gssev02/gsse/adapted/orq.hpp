/* ============================================================================
   Copyright (c) 2008-2010 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_ADAPTED_ORQ_INCLUDE_HH)
#define GSSE_ADAPTED_ORQ_INCLUDE_HH

// *** system includes

// *** BOOST includes
// *** GSSE extern includes
#include "mauch/geom/orq/CellArray.h"


namespace gsse { 
namespace adapted {

template<long DIMFinal, typename GeomNumericT=double, typename VertexHandleT=long>
struct orq_container
{
  typedef VertexHandleT                               vertex_handle;
  typedef ads::FixedArray<DIMFinal, GeomNumericT>     point_t;
  typedef std::map<vertex_handle, point_t>            new_global_point_list_t;
  typedef typename new_global_point_list_t::const_iterator     Record_t; 
  typedef point_t                                     Multikey_t;  
  
  struct gp_accessor : 
    public std::unary_function<Record_t,Multikey_t>  // The functor to access the multikey.
  {   
    const Multikey_t& operator()(const Record_t& l_record) const 
    {
      return (*l_record).second;
    }
  };

  // [RH][TODO] .. maybe DIMFinal has to be used as -> dim for cell AND dim for cellarray
  //
  typedef geom::CellArray<DIMFinal,Record_t, Multikey_t,  typename Multikey_t::value_type, gp_accessor> CellArray;
  typedef typename CellArray::BBox BBox;
  typedef typename CellArray::SemiOpenInterval SemiOpenInterval;

};




} // namespace adapted
} // namespace gsse

#endif


