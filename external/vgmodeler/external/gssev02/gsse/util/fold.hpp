/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GSSE_UTIL_FOLD
#define GSSE_UTIL_FOLD
//
// ===================================================================================
//
// *** GSSE includes
//
#include "../datastructure/sequence.hpp"
//
// *** BOOST includes
//
#include <boost/fusion/iterator/value_of.hpp>
#include <boost/fusion/iterator/next.hpp>
#include <boost/fusion/iterator/equal_to.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>
#include <boost/spirit/home/phoenix/core.hpp>  
#include <boost/spirit/home/phoenix/function.hpp>  
//
// ===================================================================================
//
namespace gsse 
{


namespace detail
{
// ========================================================
//
// AN ADAPTED IMPLEMENTATION OF THE BOOST::FUSION::FOLD ALGORITHM
//
//    note: this is the linear implementation of the boost::fusion::fold algorithm by guzman
//          however, to be able to apply the fold algorithm on functor sequences with different return types
//          the original fusion::fold algorithm had to be adapted. 
//
// ========================================================
template < typename First, typename Last, typename State, typename Functor > 
inline State  
fold_impl(First const& first, Last const& last, State const& state, Functor f, boost::mpl::true_ const&)
{
   return state;
}   

template < typename First, typename Last, typename State, typename Functor > 
inline
typename boost::fusion::result_of::value_of<
   typename boost::fusion::result_of::prior< Last >::type
>::type::result_type 
fold_impl(First const& first, Last const& last, State const& state, Functor f, boost::mpl::false_ const&)
{
   return gsse::detail::fold_impl( boost::fusion::next(first), last, f(*first, state), f, 
          boost::fusion::result_of::equal_to<typename boost::fusion::result_of::next<First>::type, Last>() );
}

template < typename First, typename Last, typename State1, typename State2, typename Functor > 
inline
typename boost::fusion::result_of::value_of<
   typename boost::fusion::result_of::prior< Last >::type
>::type::result_type 
fold_impl(First const& first, Last const& last, State1 const& state1, State2 const& state2, 
          Functor f, boost::mpl::false_ const&)
{
   return gsse::detail::fold_impl( boost::fusion::next(first), last, f(*first, state1, state2), f, 
          boost::fusion::result_of::equal_to<typename boost::fusion::result_of::next<First>::type, Last>() );
}

template < typename First, typename Last, typename State1, typename State2, typename State3, typename Functor > 
inline
typename boost::fusion::result_of::value_of<
   typename boost::fusion::result_of::prior< Last >::type
>::type::result_type 
fold_impl(First const& first, Last const& last, State1 const& state1, State2 const& state2, State3 const& state3,
          Functor f, boost::mpl::false_ const&)
{
   return gsse::detail::fold_impl( boost::fusion::next(first), last, f(*first, state1, state2, state3), f, 
          boost::fusion::result_of::equal_to<typename boost::fusion::result_of::next<First>::type, Last>() );
}

template < typename First, typename Last, typename State1, typename State2, typename State3, typename State4, typename Functor > 
inline
typename boost::fusion::result_of::value_of<
   typename boost::fusion::result_of::prior< Last >::type
>::type::result_type 
fold_impl(First const& first, Last const& last, State1 const& state1, State2 const& state2, State3 const& state3, State4 const& state4,
          Functor f, boost::mpl::false_ const&)
{
   return gsse::detail::fold_impl( boost::fusion::next(first), last, f(*first, state1, state2, state3, state4), f, 
          boost::fusion::result_of::equal_to<typename boost::fusion::result_of::next<First>::type, Last>() );
}

//
// ===================================================================================
//
} // end namespace: detail
//
// ===================================================================================
//
template < typename Sequence, typename State, typename Functor >
typename boost::fusion::result_of::value_of<
   typename boost::fusion::result_of::prior<
      typename boost::fusion::result_of::end<Sequence>::type
   >::type
>::type::result_type
fold(Sequence const& sequ, State const& state, Functor f)     
{
   return
   gsse::detail::fold_impl(  boost::fusion::begin(sequ), boost::fusion::end(sequ), state, f,  
               boost::fusion::result_of::equal_to<typename boost::fusion::result_of::begin<Sequence>::type, 
                                                  typename boost::fusion::result_of::end<Sequence>::type>() );
}

template < typename Sequence, typename State1, typename State2, typename Functor >
typename boost::fusion::result_of::value_of<
   typename boost::fusion::result_of::prior<
      typename boost::fusion::result_of::end<Sequence>::type
   >::type
>::type::result_type    
fold(Sequence const& sequ, State1 const& state1, State2 const& state2, Functor f)     
{
   return 
   gsse::detail::fold_impl(  boost::fusion::begin(sequ), boost::fusion::end(sequ), state1, state2, f,           
               boost::fusion::result_of::equal_to<typename boost::fusion::result_of::begin<Sequence>::type, 
                                                  typename boost::fusion::result_of::end<Sequence>::type>() );
}

template < typename Sequence, typename State1, typename State2, typename State3, typename Functor >
typename boost::fusion::result_of::value_of<
   typename boost::fusion::result_of::prior<
      typename boost::fusion::result_of::end<Sequence>::type
   >::type
>::type::result_type
fold(Sequence const& sequ, State1 const& state1, State2 const& state2, State3 const& state3, Functor f)     
{
   return 
   gsse::detail::fold_impl(  boost::fusion::begin(sequ), boost::fusion::end(sequ), state1, state2, state3, f,           
               boost::fusion::result_of::equal_to<typename boost::fusion::result_of::begin<Sequence>::type, 
                                                  typename boost::fusion::result_of::end<Sequence>::type>() );
}

template < typename Sequence, typename State1, typename State2, typename State3, typename State4, typename Functor >
typename boost::fusion::result_of::value_of<
   typename boost::fusion::result_of::prior<
      typename boost::fusion::result_of::end<Sequence>::type
   >::type
>::type::result_type
fold(Sequence const& sequ, State1 const& state1, State2 const& state2, State3 const& state3, State4 const& state4, Functor f)     
{
   return 
   gsse::detail::fold_impl(  boost::fusion::begin(sequ), boost::fusion::end(sequ), state1, state2, state3, state4, f,           
               boost::fusion::result_of::equal_to<typename boost::fusion::result_of::begin<Sequence>::type, 
                                                  typename boost::fusion::result_of::end<Sequence>::type>() );
}

//
// ===================================================================================
//
namespace functional
{
//
// ===================================================================================
//
struct caller 
{
   template< typename UnaryOperation, typename Element >
   typename UnaryOperation::result_type       
   operator()(UnaryOperation op, Element const& ele)
   {
      return op(ele);
   }      

   template< typename BinaryOperation, typename Element1, typename Element2 >
   typename BinaryOperation::result_type       
   operator()(BinaryOperation op, Element1 const& ele1, Element2 const& ele2)
   {
      return op(ele1, ele2);
   }           
   
   template< typename TrinaryOperation, 
             typename Element1, typename Element2, typename Element3 >
   typename TrinaryOperation::result_type       
   operator()(TrinaryOperation op, 
              Element1 const& ele1, Element2 const& ele2, Element3 const& ele3)
   {
      return op(ele1, ele2, ele3);
   }        
   
   template< typename QuadraryOperation,    
             typename Element1, typename Element2, typename Element3, typename Element4 >
   typename QuadraryOperation::result_type       
   operator()(QuadraryOperation op, 
              Element1 const& ele1, Element2 const& ele2, Element3 const& ele3,
              Element4 const& ele4)
   {
      return op(ele1, ele2, ele3, ele4);
   }       

};
//
// ===================================================================================
//
template < int N >
struct fold_impl {};

template < >
struct fold_impl < 1 >
{
   // ---------------------------------------------
   template < typename Sequence, typename Element >
   struct result
   {
      typedef typename boost::fusion::result_of::value_of<
         typename boost::fusion::result_of::prior< 
            typename boost::fusion::result_of::end< Sequence >::type 
         >::type
      >::type::result_type       type; 
   };
   // ---------------------------------------------
   template < typename Sequence, typename Element >
   typename result< Sequence, Element >::type       
   operator()( Sequence const& sequ, Element const& ele ) const
   {
      BOOST_MPL_ASSERT(( boost::fusion::traits::is_sequence< Sequence > ));  
      return gsse::fold(sequ, ele, gsse::functional::caller());
   }
   // ---------------------------------------------
   
};

template < >
struct fold_impl < 2 >
{
   // ---------------------------------------------
   template <typename Sequence, typename Element1, typename Element2>
   struct result
   {
      typedef typename boost::fusion::result_of::value_of<
         typename boost::fusion::result_of::prior< 
            typename boost::fusion::result_of::end< Sequence >::type 
         >::type
      >::type::result_type       type; 

   };
   // ---------------------------------------------
   template <typename Sequence, typename Element1, typename Element2>
   typename result< Sequence, Element1, Element2 >::type       
   operator()( Sequence const& sequ, Element1 const& ele1, Element2 const& ele2 ) const
   {
      BOOST_MPL_ASSERT(( boost::fusion::traits::is_sequence< Sequence > ));  
      return gsse::fold( sequ, ele1, ele2, gsse::functional::caller() );
   }
   // ---------------------------------------------
   
};

template < >
struct fold_impl < 3 >
{
   // ---------------------------------------------
   template <typename Sequence, 
             typename Element1, 
             typename Element2,
             typename Element3>
   struct result
   {
      typedef typename boost::fusion::result_of::value_of<
         typename boost::fusion::result_of::prior< 
            typename boost::fusion::result_of::end< Sequence >::type 
         >::type
      >::type::result_type       type; 

   };
   // ---------------------------------------------
   template <typename Sequence, 
             typename Element1, 
             typename Element2,
             typename Element3>
   typename result< Sequence, Element1, Element2, Element3 >::type       
   operator()( Sequence const& sequ, Element1 const& ele1, 
               Element2 const& ele2, Element3 const& ele3 ) const
   {
      BOOST_MPL_ASSERT(( boost::fusion::traits::is_sequence< Sequence > ));  
      return gsse::fold( sequ, ele1, ele2, ele3, gsse::functional::caller() );
   }
   // ---------------------------------------------
   
};

template < >
struct fold_impl < 4 >
{
   // ---------------------------------------------
   template <typename Sequence, 
             typename Element1, 
             typename Element2,
             typename Element3,
             typename Element4>
   struct result
   {
      typedef typename boost::fusion::result_of::value_of<
         typename boost::fusion::result_of::prior< 
            typename boost::fusion::result_of::end< Sequence >::type 
         >::type
      >::type::result_type       type; 

   };
   // ---------------------------------------------
   template <typename Sequence, 
             typename Element1, 
             typename Element2,
             typename Element3,
             typename Element4>
   typename result< Sequence, Element1, Element2, Element3, Element4 >::type       
   operator()( Sequence const& sequ, Element1 const& ele1, 
               Element2 const& ele2, Element3 const& ele3,
               Element4 const& ele4 ) const
   {
      BOOST_MPL_ASSERT(( boost::fusion::traits::is_sequence< Sequence > ));  
      return gsse::fold( sequ, ele1, ele2, ele3, ele4, gsse::functional::caller() );
   }
   // ---------------------------------------------
   
};
//
// ===================================================================================
//
boost::phoenix::function< gsse::functional::fold_impl< 1 > > const fold  = gsse::functional::fold_impl< 1 >();
boost::phoenix::function< gsse::functional::fold_impl< 1 > > const fold1 = gsse::functional::fold_impl< 1 >();
boost::phoenix::function< gsse::functional::fold_impl< 2 > > const fold2 = gsse::functional::fold_impl< 2 >();
boost::phoenix::function< gsse::functional::fold_impl< 3 > > const fold3 = gsse::functional::fold_impl< 3 >();
boost::phoenix::function< gsse::functional::fold_impl< 4 > > const fold4 = gsse::functional::fold_impl< 4 >();
//
// ===================================================================================
//
} // end namespace: functional

} // end namespace: gsse

#endif
