/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at     
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at                     

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#if !defined(GSSE_UTIL_TIMER_HH)
#define GSSE_UTIL_TIMER_HH   
   
// *** system includes
//#include "gsse/util/tag_of.hpp"  // [TODO] .. extract tag_of meta function
// *** BOOST includes
#include "boost/timer.hpp"
// *** EXTERNAL includes
#include "blitz-0.9/blitz/timer.h"
// *** GSSE includes

namespace gsse {

  // if [TODO] is done, this can be deleted
  //  and replaced by an include
  //
namespace traits    {

   struct tag_none{};
   // essential line 
   //   to give the 2nd template parameter default argument
   template<typename T, typename Active = void>
   struct tag_of;
   
   
   template <typename Sequence, typename Active>
   struct tag_of
   {
      typedef tag_none     type;
      typedef tag_none     container_type;
      typedef tag_none     numeric_type;      
   };
}

// --------------------------------
//
// tags ..
//
struct boost_timer{};
struct blitz_timer{};

// --------------------------------
// 
// tag_of mechanism
//
namespace traits    {

template <>   
struct tag_of< boost::timer >
{
   typedef gsse::boost_timer  type;
};

template <>   
struct tag_of< blitz::Timer >
{
   typedef gsse::blitz_timer  type;
};

} // end namespace traits

// --------------------------------
// 
// result_of mechanism
//
namespace result_of    {

template< typename Tag >
struct return_type_impl { };

template< >
struct return_type_impl< gsse::boost_timer >
{ 
   // [JW] as for boost 1.42, 
   // boost::timer.hpp use hardcoded double 
   // value type .. 
   //
   typedef double type;
};

template< >
struct return_type_impl< gsse::blitz_timer >
{ 
   typedef double type;
};

template< typename T >
struct return_type 
{ 
   typedef typename gsse::result_of::return_type_impl< 
      typename gsse::traits::tag_of< T >::type >::type   type;      
};

} // end namespace result_of

// --------------------------------
//
// tag dispatched routing of the actual timer objects
//
template< typename Timer >
struct timer {};

template< >
struct timer < gsse::boost_timer >
{
   typedef boost::timer type;
};

template< >
struct timer < gsse::blitz_timer >
{
   typedef blitz::Timer type;
};

// --------------------------------
// 
// restart()
//

//
// tag dispatched mechanisms
//
template< typename TimerTag >
struct restart_impl { };

template< >
struct restart_impl < gsse::boost_timer >
{
   template< typename Timer >
   void operator()(Timer& t)
   {
      t.restart();
   }
};

template< >
struct restart_impl < gsse::blitz_timer >
{
   template< typename Timer >
   void operator()(Timer& t)
   {
      t.start();
   }
};

//
// generic restart() function ..
//
template< typename Timer >
void restart(Timer & t)
{
   gsse::restart_impl< typename gsse::traits::tag_of< Timer >::type >()(t);
}


// --------------------------------
// 
// elapsed()
//

//
// tag dispatched mechanisms
//
template< typename TimerTag >
struct elapsed_impl { };

template< >
struct elapsed_impl < gsse::boost_timer >
{
   template< typename Timer >
   typename gsse::result_of::return_type< Timer >::type 
   operator()(Timer& t)
   {
      return t.elapsed();
   }
};

template< >
struct elapsed_impl < gsse::blitz_timer >
{
   template< typename Timer >
   typename gsse::result_of::return_type< Timer >::type 
   operator()(Timer& t)
   {
      t.stop();
      return t.elapsedSeconds();
   }
};

//
// generic restart() function ..
//
template< typename Timer >
typename gsse::result_of::return_type< Timer >::type 
elapsed(Timer & t)
{
   return gsse::elapsed_impl< 
      typename gsse::traits::tag_of< Timer >::type >()(t);
}


} // end namespace gsse

#endif
