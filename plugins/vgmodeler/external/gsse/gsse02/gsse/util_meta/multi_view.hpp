/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_UTIL_META_MULTIVIEW_HH)
#define GSSE_UTIL_META_MULTIVIEW_HH

// *** system includes
// *** BOOST includes
#include <boost/mpl/filter_view.hpp>
#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/include/find.hpp>
#include <boost/fusion/iterator/value_of.hpp>
// *** GSSE includes

namespace gsse {

namespace traits {

template< typename T >
struct properties
{
   typedef typename T::property_sequence type;
};

} // end namespace traits


template<typename T>
struct check 
{ 
   template<typename AlgoT>
   struct apply
   {
      typedef typename boost::fusion::result_of::value_of<
         typename boost::fusion::result_of::find< typename gsse::traits::properties<AlgoT>::type, T >::type 
      >::type  find_result_type;
      typedef typename boost::is_same<T, find_result_type>::type type;
   };
};   


// technique extracted and adapted from boost.fusion developer Tobias Schwinger
// as depicted in libs/fusion/example/cookbook/do_the_bind.cpp
struct dothefold
{
   struct fold_op
   {
      template <typename Sig> struct result;
      template <class S, class State, class Element> 
      struct result< S(State &,Element &) > 
      {
         typedef typename boost::mpl::filter_view< State, Element >::type  type;
      };
   };

   template <typename Seq, typename State>
   struct apply : boost::fusion::result_of::fold<Seq, State, fold_op>::type { };
};


namespace result_of {
template< typename SequenceSeq, typename PredicatesSeq >
struct multi_view
{
   typedef typename gsse::dothefold::apply< PredicatesSeq, SequenceSeq >::type   type;

//   typedef typename boost::fusion::result_of::begin<fold_result>::type                        first_fold_result;
//   typedef typename boost::fusion::result_of::deref<first_fold_result>::type                  type; 
};
} // end namespace result_of
} // end namespace gsse

#endif

