/* ============================================================================
   Copyright (c) 2007-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        

  based on the fusion access protocols
   Copyright (c) 2001-2006 Joel de Guzman 
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_UTIL_ACCESS_COMPLEX_HH)
#define GSSE_UTIL_ACCESS_COMPLEX_HH

// *** system includes
// *** BOOST includes
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/support/tag_of.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/spirit/home/phoenix/function.hpp>  

// *** GSSE includes
#include "gsse/util/common.hpp"
#include "gsse/util/tag_of.hpp"


// ############################################################
//
namespace gsse { namespace extension {

struct identity
{
   identity():val(0){}
   identity(long val):val(val){}

   template<typename Val>
   identity& operator=(Val val_)
   {
#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "identity operator used with value: " << val_ << std::endl;
#endif
      return (*this);
   }

private:
   long val;
};


template<typename Sequence2, typename Enable =void>
struct at_index_impl
{
   template<typename Sequence>
   struct apply
   {
      typedef identity  type;

      static type
      call(Sequence& seq)
      {
#ifdef GSSE_DEBUG_FULLOUTPUT
         std::cout << " ##### warning.. at_index is used without a compile-time sequence (hence no index space available) ###### " << std::endl;
#endif
         return identity();    
      }
   };
};

template<typename Sequence2>
struct at_index_impl<Sequence2,  typename boost::enable_if<boost::fusion::traits::is_sequence<Sequence2> >::type >
{
   template<typename Sequence>
   struct apply
   {
      typedef typename boost::fusion::result_of::at_c<Sequence,0>::type type;
      
      static type
      call(Sequence& seq)
      {
         return boost::fusion::at<boost::mpl::int_<0> >(seq);
      }
   };
};

// ===============

template<typename Sequence2, typename Enable =void>
struct at_fiber_impl
{
   template<typename Sequence>
   struct apply
   {
      typedef Sequence&  type;

      static type
      call(Sequence& seq)
      {
         return seq;
      }
   };
};

template<typename Sequence2>
struct at_fiber_impl<Sequence2,  typename boost::enable_if<boost::fusion::traits::is_sequence<Sequence2> >::type >
{
   template<typename Sequence>
   struct apply
   {
      typedef typename boost::fusion::result_of::at_c<Sequence,1>::type type;
      
      static type
      call(Sequence& seq)
      {
         return boost::fusion::at<boost::mpl::int_<1> >(seq);
      }
   };
};


template<typename Sequence2, typename Enable =void>
struct at_fiber_val_impl
{
   template<typename Sequence>
   struct apply
   {
      typedef Sequence  type;
   };
};

template<typename Sequence2>
struct at_fiber_val_impl<Sequence2,  typename boost::enable_if<boost::fusion::traits::is_sequence<Sequence2> >::type >
{
   template<typename Sequence>
   struct apply
   {
      typedef typename boost::fusion::result_of::value_at_c<Sequence,1>::type type;
   };
};


} // namespace extension 
// =========================================================================================


namespace result_of {

template <typename Sequence>
struct at_index 
   :  gsse::extension::at_index_impl< Sequence >::template apply<Sequence>  
{};

template <typename Sequence>
struct at_fiber
   :  gsse::extension::at_fiber_impl< Sequence >::template apply<Sequence>  
{ };


// value functions
template <typename Sequence>
struct at_fiber_val
   :  gsse::extension::at_fiber_val_impl< Sequence >::template apply<Sequence>  
{ };


}  //namespace result_of
// =========================================================================================




template<typename Sequence>
inline typename 
boost::lazy_disable_if<
   boost::is_const<Sequence>
   ,result_of::at_index<Sequence>
   >::type
at_index(Sequence& seq)  
{
   return result_of::at_index< Sequence >::call(seq);
}
template<typename Sequence>
typename result_of::at_index<Sequence const>::type
at_index(Sequence const& seq)  
{
   return result_of::at_index< Sequence const >::call(seq);
}

// ============

template<typename Sequence>
inline typename 
boost::lazy_disable_if<
   boost::is_const<Sequence>
   ,result_of::at_fiber<Sequence>
   >::type
at_fiber(Sequence& seq)  
{
   return result_of::at_fiber< Sequence >::call(seq);
}
template<typename Sequence>
typename result_of::at_fiber<Sequence const>::type
at_fiber(Sequence const& seq)  
{
   return result_of::at_fiber< Sequence const >::call(seq);
}




namespace functional
{
struct at_index_impl
{    
   template< typename Fiber >
   struct result
   { 
      typedef long  type; // derive it!
   };
      
   template< typename Fiber >
   typename result< Fiber >::type 
   operator()( Fiber const& fiber )  const
   {
      return gsse::at_index( fiber );
   }
};
boost::phoenix::function< gsse::functional::at_index_impl > const at_index = gsse::functional::at_index_impl();
} // end namespace: functional


} // namespace gsse


#endif



