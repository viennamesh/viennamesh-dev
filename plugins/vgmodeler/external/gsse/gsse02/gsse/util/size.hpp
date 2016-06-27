/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_UTIL_SIZE_HH)
#define GSSE_UTIL_SIZE_HH

// *** system includes
// *** BOOST includes
#include <boost/spirit/home/phoenix/core.hpp>  
#include <boost/spirit/home/phoenix/function.hpp>  
// *** GSSE includes
#include "gsse/util/tag_of.hpp"
#include "gsse/util/debug_meta.hpp"

// ############################################################
//
namespace gsse { 

// template<typename Container>
// size_t size(Container const& container)
// {
//    return container.size();
// }




// ############################################################
// ############################################################


namespace result_of {

template < typename Sequence, typename Enable=void>
struct size_impl
{ 
   static const size_t value = Sequence::value;
};

// -----------------------------------
template<typename T1, long T2>
struct size_impl <boost::array<T1, T2> >
{ 
   static const size_t value = T2;
};

template<typename T1, unsigned long T2>
struct size_impl <boost::array<T1, T2> >
{ 
   static const size_t value = T2;
};

#ifndef __x86_64__ // [JW] we need this only on 32 bit archs
template<typename T1, std::size_t T2>
struct size_impl <boost::array<T1, T2> >
{ 
   static const size_t value = T2;
};
#endif
// -----------------------------------
template<typename T1, long T2>
struct size_impl <gsse::array<T1, T2> >
{ 
   static const size_t value = T2;
};

template<typename T1, unsigned long T2>
struct size_impl <gsse::array<T1, T2> >
{ 
   static const size_t value = T2;
};

#ifndef __x86_64__ // [JW] we need this only on 32 bit archs
template<typename T1, std::size_t T2>
struct size_impl <gsse::array<T1, T2> >
{ 
   static const size_t value = T2;
};
#endif
// -----------------------------------
template<typename T1, long T2>
struct size_impl <gsse::metric_object<T1, T2> >
{ 
   static const size_t value = T2;
};

template<typename T1, unsigned long T2>
struct size_impl <gsse::metric_object<T1, T2> >
{ 
   static const size_t value = T2;
};

#ifndef __x86_64__ // [JW] we need this only on 32 bit archs
template<typename T1, std::size_t T2>
struct size_impl <gsse::metric_object<T1, T2> >
{ 
   static const size_t value = T2;
};
#endif
// -----------------------------------
template<int T1, typename T2 >
struct size_impl <ads::FixedArray<T1, T2> >
{ 
   static const size_t value = T1;
};

template < typename CTArray >
struct size
{
   static const size_t value = gsse::result_of::size_impl< CTArray >::value;
};

} // end namespace: result_of


//
// ===================================================================================
// 
namespace detail {

template < typename Container, typename EvaluationTag >
struct size { };

template < typename Container >
struct size < Container, gsse::tag_runtime >
{
   static size_t 
   apply( Container const& cont )
   {
      return cont.size();
   }
};

template < typename Container >
struct size < Container, gsse::tag_runtime_map >
{
   static size_t 
   apply( Container const& cont )
   {
      return cont.size();
   }
};


template < typename Container >
struct size < Container, gsse::tag_compiletime >
{
   static size_t 
   apply( Container const&  )
   {
      return gsse::result_of::size< Container >::value;
   }
};
} // end namespace: detail


//
// ===================================================================================
//   MAIN SIZE implementation
// ===================================================================================
//
template < typename Container >
size_t size( Container & cont )
{
   return gsse::detail::size< Container, 
      typename gsse::traits::tag_of< Container >::evaluation_type >::apply( cont );
}

template < typename Container >
size_t size( Container const& cont )
{
   return gsse::detail::size< Container, 
      typename gsse::traits::tag_of< Container >::evaluation_type >::apply( cont );
}





//
// ===================================================================================
// 

struct size_functor
{ 
   typedef size_t         result_type;
   // ------------------------------------------------
   template < typename T >
   inline result_type 
   operator() (T const&  t)
   {  
      return gsse::size(t);
   }
   // ------------------------------------------------
   template < typename T >
   inline result_type 
   operator() (T &  t)
   {  
      return gsse::size(t);
   }   
};


//
// ===================================================================================
// 
namespace functional {

struct size_impl
{
   // ---------------------------------------------
   template < typename Container >
   struct result
   {
      typedef size_t  type;
   };
   // ---------------------------------------------
   template < typename Container >
   typename result< Container >::type       
   operator()( Container const& cont ) const
   {
      return gsse::size( cont );
   }
   // ---------------------------------------------
   
};
boost::phoenix::function< gsse::functional::size_impl > const size  = gsse::functional::size_impl();

} // end namespace: functional

} // namespace gsse
#endif
