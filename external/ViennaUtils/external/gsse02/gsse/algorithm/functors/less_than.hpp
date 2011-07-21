/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_FUNCTORS_LESSTHAN
#define GML_FUNCTORS_LESSTHAN
//
// ===================================================================================
//
 #include  <boost/type_traits/is_scalar.hpp>

namespace gml 
{
namespace detail
{

template < typename NumericT >
struct less_than 
{ 
   typedef bool         result_type;
   // ------------------------------------------------
   less_than()                    : _ref(0)   {}
   less_than(NumericT ref)        : _ref(ref) {}
   // ------------------------------------------------

   template < typename T >
   result_type 
   operator() (T const&  t)
   {  
      return process( t, boost::is_scalar< T >() );
   }
   template < typename T >
   result_type
   process(T const& t, boost::true_type const& )
   {
      if (t < _ref) return true;
      else return false;
   }
   template < typename T >
   result_type
   process(T const& t, boost::false_type const& )
   {
      namespace phoenix = boost::phoenix;    
      long cnt = 0;
      gml::for_each(t, 
      (
         phoenix::if_( gml::_1 > _ref )
         [
            gml::ref( cnt )++
         ]
      ));
      if( cnt == 0 ) return true;
      else return false;
   }   
   // ------------------------------------------------   
   NumericT    _ref;
   // ------------------------------------------------
};

} // end namespace: detail

template < typename T >
struct less_than
{
   typedef gml::detail::less_than< T >  type;
};


} // end namespace: gml
#endif















