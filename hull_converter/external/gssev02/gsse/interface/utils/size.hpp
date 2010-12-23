/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at   

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GMI_UTILS_SIZE
#define GMI_UTILS_SIZE
//
// ======================================================================
//
// *** BOOST includes
//
#include <boost/spirit/home/phoenix/core.hpp>  
#include <boost/spirit/home/phoenix/function.hpp>  
//
// ======================================================================
// 
namespace gmi {
//
// ======================================================================
// 
namespace result_of {
//
// ======================================================================
// 

template < typename T, typename Active = void >
struct size;
//
// #### default 
//
template <typename CTArray, typename Active>
struct size
{
   static const long value = CTArray::value;
};
//
// #### specializations
//
// -----------------------------------------------------
template<typename T, std::size_t N>
struct size< gsse::metric_object<T,N> >
{
   static const long value = N;
};
template<typename T, long N>
struct size< gsse::metric_object<T,N> >
{
   static const long value = N;
};
// -----------------------------------------------------
template<typename T, std::size_t N>
struct size< boost::array<T,N> >
{
   static const long value = N;
};
template<typename T, long N>
struct size< boost::array<T,N> >
{
   static const long value = N;
};
// -----------------------------------------------------
template<typename T0>
struct size< boost::fusion::vector<T0> >
{
   static const long value = 1;
};
template<typename T0, typename T1>
struct size< boost::fusion::vector<T0,T1> >
{
   static const long value = 2;
};
template<typename T0, typename T1, typename T2>
struct size< boost::fusion::vector<T0,T1,T2> >
{
   static const long value = 3;
};
template<typename T0, typename T1, typename T2, typename T3>
struct size< boost::fusion::vector<T0,T1,T2,T3> >
{
   static const long value = 4;
};
// -----------------------------------------------------
//
// ======================================================================
// 
} // end namespace: result_of
//
// ======================================================================
// 
namespace detail {
//
// ======================================================================
// 
template < typename Container, typename EvaluationTag >
struct size { };

template < typename Container >
struct size < Container, gsse::tag_runtime >
{
   static long 
   compute( Container const& cont )
   {
      return cont.size();
   }
};

template < typename Container >
struct size < Container, gsse::tag_compiletime >
{
   static long 
   compute( Container const&  )
   {
      return gmi::result_of::size< Container >::value;
   }
};
//
// ======================================================================
// 
} // end namespace: detail
//
// ======================================================================
// 
template < typename Container >
long size( Container & cont )
{
   return gmi::detail::size< Container, 
      typename gsse::traits::tag_of< Container >::evaluation_type >::compute( cont );
}

template < typename Container >
long size( Container const& cont )
{
   return gmi::detail::size< Container, 
      typename gsse::traits::tag_of< Container >::evaluation_type >::compute( cont );
}
/*
struct size_functor
{ 
   typedef long         result_type;
   // ------------------------------------------------
   template < typename T >
   inline result_type 
   operator() (T const&  t)
   {  
      return gmi::size(t);
   }
   // ------------------------------------------------
   template < typename T >
   inline result_type 
   operator() (T &  t)
   {  
      return gmi::size(t);
   }   
};
//
// ===================================================================================
// 
namespace functional {
//
// ===================================================================================
// 
struct size_impl
{
   // ---------------------------------------------
   template < typename Container >
   struct result
   {
      typedef long  type;
   };
   // ---------------------------------------------
   template < typename Container >
   typename result< Container >::type       
   operator()( Container const& cont ) const
   {
      return gmi::size( cont );
   }
   // ---------------------------------------------
   
};
boost::phoenix::function< gmi::functional::size_impl > const size  = gmi::functional::size_impl();
//
// ===================================================================================
// 
} // end namespace: functional
*/
//
// ===================================================================================
// 
} // end namespace: gmi


#endif
