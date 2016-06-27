/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_UTIL_META_DIMENSION_HH)
#define GSSE_UTIL_META_DIMENSION_HH

// *** system includes
// *** BOOST includes
#include <boost/mpl/int.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/container/vector/convert.hpp>

// *** GSSE includes
#include "gsse/util/specifiers.hpp"


// ############################################################
//
namespace gsse {   namespace util_meta {

template<typename Complex, typename Key>
struct key_2_dim
{
//    typedef typename Complex::template meta_at_impl<Key>::type  KeyType;
//    typedef typename Complex::template meta_find_impl<Key>::type  VectorType;

//   typedef typename Complex::template meta_find_impl<Key>::type  VectorIteratorType;

   // [FS] using find instead of meta_find_impl
   //
   typedef typename boost::fusion::result_of::find<Complex, Key>::type VectorIteratorType;
   
   static const long value = VectorIteratorType::index::value - 1;   // [RH][info] DIM+1 because of FBPosT
};


template<typename Complex, long DIM>
struct dim_2_key
{
   // [TODO]
   //  only if this is a map
   //
   typedef typename boost::fusion::result_of::as_vector<Complex>::type  ComplexV;


   // DIM+1 -> a local coordinate system is always included (FBPosT)
   // [TODO].. check if dimension of complex >= DIM+1
   //
   typedef typename boost::fusion::result_of::value_at_c<ComplexV, DIM+1>::type PairType;

   typedef typename boost::fusion::result_of::first<PairType>::type type;
};



template<typename CellComplex, long DIM_relative = 1, typename AccessorStart=gsse::AT_cl>
struct calc_bnd
{
   static const long dim_cl  = gsse::util_meta::key_2_dim<CellComplex, AccessorStart>::value;
   static const long dim_bnd = dim_cl - DIM_relative;
   typedef typename gsse::util_meta::dim_2_key<CellComplex, dim_bnd>::type type;

};

} // namespace util_meta
} // namespace gsse


#endif



