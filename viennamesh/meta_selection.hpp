/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Tobias Schwinger

   license:    see file LICENSE in the base directory
============================================================================= */


// technique extracted and adapted from boost.fusion developer Tobias Schwinger
// as depicted in libs/fusion/example/cookbook/do_the_bind.cpp


#ifndef VIENNAMESH_METASELECTION_HPP
#define VIENNAMESH_METASELECTION_HPP

#include <boost/mpl/filter_view.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/include/find.hpp>
#include <boost/fusion/include/find_if.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/value_of.hpp>
#include <boost/fusion/include/value_at.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/include/has_key.hpp>

#include "viennamesh/tags.hpp"
#include "viennamesh/generation.hpp"

namespace viennamesh {

namespace error {
   struct meta_selection_not_conclusive {};
} // end namespace error

namespace detail {
//
// -----------------------------------------------------------------------------
//
template<typename PairT>
struct check 
{ 
   template<typename EleT>
   struct apply
   {
      typedef typename boost::fusion::result_of::value_of<  // dereference the meta-iterator ..
         typename boost::fusion::result_of::find_if<           // .. which is a result of the find algorithm
            typename EleT::properties_map_type, 
            boost::is_same<boost::mpl::_,PairT>
         >::type 
      >::type  find_result_type;
      typedef typename boost::is_same<PairT, find_result_type>::type type;
   };
};   
//
// -----------------------------------------------------------------------------
//
struct dothefold
{
   struct fold_op
   {
      template <typename Sig> struct result;
      
      template <class S, class State, class Element> 
      struct result< S(State &,Element &) > 
      {
         // compute a view on the sequence of mesh generators 
         // based on the predicate provided by Element
         // --> the result sequence contains only generators which 
         // support the required prerequesite 
         //
         typedef typename boost::mpl::filter_view< 
            State, 
            viennamesh::detail::check<Element> 
         >::type  type;
      };
   };

   template <typename Seq, typename State>
   struct apply : boost::fusion::result_of::fold<Seq, State, fold_op>::type { };
};


template<typename Result, int ResultSize>
struct process_result
{
   // the preliminary result may contain several valid generators
   // to specificaly result to a single generator, we use, for now, the first one
   //
   typedef typename boost::fusion::result_of::value_at_c<Result,0>::type type;
};

template<typename Result>
struct process_result <Result, 0>
{
   typedef viennamesh::error::meta_selection_not_conclusive type;
};

} // end namespace detail
//
// -----------------------------------------------------------------------------
//

namespace result_of  {
template<typename PropertiesMapT>
struct compute_mesh_generator
{
   typedef boost::mpl::vector< 
      typename viennamesh::result_of::mesh_generator<viennamesh::tag::netgen>::type
      //,viennamesh::tag::tetgen
      //,viennamesh::tag::cervpt
   >                                                        AvailableMeshGeneratorsT;

   // the result is not evaluated yet, only when an element is actually 
   // accessed of the result, it is being computed - > lazy concept
   //
   typedef typename viennamesh::detail::dothefold::apply< 
      PropertiesMapT, AvailableMeshGeneratorsT >::type      PrelimResulT;
      
      
////   // it may happen that the fold approach is not able to find a 
////   // a sequence of generators which fulfill the required prerequesits
////   // to support this case without producing tons of inconclusive compiler error
////   // messages, this additional metafunction not only access the first element
////   // but it does this only in case a valid result has been computed.
////   // if this is not the case, the result type is mapped to a 
////   // error structure which gives a conclusive information 
////   //
////   typedef typename viennamesh::detail::process_result<
////      PrelimResulT, 
////      boost::fusion::result_of::size< PrelimResulT >::value
////   >::type                                                  type;
   
   
   typedef typename boost::mpl::if_c<
      boost::is_same< boost::mpl::int_<0>, boost::mpl::int_<boost::fusion::result_of::size<PrelimResulT>::value> >::value,
      boost::integral_constant<bool,false>,
      boost::integral_constant<bool,true>
   >::type                                                  CheckResult;

    BOOST_MPL_ASSERT_MSG(
          CheckResult::value
        , VIENNAMESH_META_SELECTION_WAS_NOT_CONCLUSIVE
        , (CheckResult)
        );

//   // the preliminary result may contain several valid generators
//   // to specificaly result to a single generator, we use, for now, the first one
//   //
//   typedef typename boost::fusion::result_of::value_at_c<PrelimResulT,0>::type type;
   typedef typename viennamesh::detail::process_result<
      PrelimResulT, 
      boost::fusion::result_of::size< PrelimResulT >::value
   >::type                                                  type;

};
} // end namespace result_of
//
// -----------------------------------------------------------------------------
//
} // end namespace viennamesh

#endif

