/* ============================================================================
   Copyright (c) 2010 Philipp Schwaha                           philipp@gsse.at        
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef DETERMINANT_COMPILE_TIME_HPP
#define DETERMINANT_COMPILE_TIME_HPP

// boost::mpl includes
//
#include <boost/mpl/fold.hpp>
#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/push_back.hpp>

// gsse includes
//
#include "gsse/util_meta/omit_view.hpp"

namespace gsse
{

struct signed_inserter
{
   template<typename SignedSequenceType, typename ItemType>
   struct apply
   {
      typedef typename SignedSequenceType::first initial_sign_type;
      typedef typename SignedSequenceType::second coefficients_type;
      
      typedef typename ItemType::first additional_sign_type;
      typedef typename ItemType::second item_type;

      typedef typename boost::mpl::times<initial_sign_type, additional_sign_type>::type sign_type;

      typedef typename boost::mpl::push_back<coefficients_type, item_type>::type extended_sequence_type;
      typedef typename boost::mpl::pair<sign_type, extended_sequence_type>::type type;
   };
};

template<typename SequenceType, typename ItemType>
struct append_to_subsequences
{
   typedef typename boost::mpl::fold<
      SequenceType,
      boost::mpl::vector<>,
      boost::mpl::push_back<boost::mpl::_1, signed_inserter::apply<boost::mpl::_2, ItemType> >
      >::type type;
};

template<long exponent>
struct sign
{
   static const long value = (exponent % 2) ? -1 : 1;
};


template<typename MappingTypeX, typename MappingTypeY, long matrix_size>
struct recursive_determinant;

template<typename MappingTypeX, typename MappingTypeY, typename LostTyoe>
struct recurse
{
   template<typename StateType, typename c>
   struct apply
   {
      static const size_t length = boost::mpl::size<MappingTypeX>::type::value;

      typedef typename omit_view<MappingTypeX, c>::type reduced_mapping;

      typedef typename recursive_determinant<reduced_mapping, MappingTypeY, 
                                             length-1>::type sub_determinant_type;

      typedef typename boost::mpl::at<MappingTypeX, c>::type current_index;
      
      typedef typename boost::mpl::pair<current_index,
                                        LostTyoe>::type local_element_type;
      
      typedef typename boost::mpl::pair<typename boost::mpl::long_<sign<c::value>::value>::type,  local_element_type>::type signed_local_element_type;

      typedef typename append_to_subsequences<sub_determinant_type, 
                                              signed_local_element_type>::type sub_multiplied_type;
      
      typedef typename boost::mpl::joint_view<StateType, sub_multiplied_type>::type type;
   };
};

template<typename MappingTypeX, typename MappingTypeY, long matrix_size>
struct recursive_determinant
{
   typedef typename omit_view_c<MappingTypeY, 0>::type remappedY;
   typedef typename boost::mpl::range_c<long, 0, matrix_size>::type index_stepping;

   typedef typename boost::mpl::fold<
      index_stepping,
      boost::mpl::vector<>,
      recurse<MappingTypeX, remappedY, typename boost::mpl::at_c<MappingTypeY,0>::type >
      >::type type;
};


// to end recursion 
// the 2x2 determinant has a simple formula and is hardcoded here
//
template<typename MappingTypeX, typename MappingTypeY>
struct recursive_determinant<MappingTypeX, MappingTypeY, 2>
{
   typedef typename boost::mpl::at_c<MappingTypeX, 0>::type x0;
   typedef typename boost::mpl::at_c<MappingTypeX, 1>::type x1;
   typedef typename boost::mpl::at_c<MappingTypeY, 0>::type y0;
   typedef typename boost::mpl::at_c<MappingTypeY, 1>::type y1;

   typedef typename boost::mpl::vector<typename boost::mpl::pair<x0, y0>::type, 
                                       typename boost::mpl::pair<x1, y1>::type >::type first;
   typedef typename boost::mpl::vector<typename boost::mpl::pair<x1, y0>::type, 
                                       typename boost::mpl::pair<x0, y1>::type >::type second;

   static const long correction = 1;
   typedef typename boost::mpl::vector<
      typename boost::mpl::pair<typename boost::mpl::long_<correction>::type, first>::type,
      typename boost::mpl::pair<typename boost::mpl::long_<-1 * correction>::type, second>::type >::type type;
};

template<long size>
struct determinant_structure_sequence
{
   typedef typename boost::mpl::range_c<long, 0, size>::type initial_mappings;
   typedef typename recursive_determinant<initial_mappings, 
                                          initial_mappings, 
                                          size>::type type;
};

} // namespace gsse

#endif // DETERMINANT_COMPILE_TIME_HPP
