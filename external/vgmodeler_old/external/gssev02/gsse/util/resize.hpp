/* ============================================================================
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2004-2008 Philipp Schwaha                      philipp@gsse.at
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        

  resize/at protocols are 
   Copyright (c) 2001-2006 Joel de Guzman 
   Copyright (c) 2006      Dan Marsden
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_UTIL_RESIZE_HH)
#define GSSE_UTIL_RESIZE_HH

// *** system includes
// *** BOOST includes
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/spirit/home/phoenix/core.hpp>  
#include <boost/spirit/home/phoenix/function.hpp>  


// *** GSSE includes
#include "gsse/util/common.hpp"
#include "gsse/util/tag_of.hpp"

// ############################################################
//
namespace gsse { namespace extension {

template <typename Tag>
struct resize_impl
{
   template <typename Sequence>
   struct apply;
};
   
template<>
struct resize_impl<tag_none>
{
   template<typename Sequence>
   struct apply
   {
      static void
      call(Sequence& seq, size_t size)
      {
#ifdef GSSETEST               
         std::cout << "## no_type:: no operation .. " << std::endl;
#endif
      }
   };
};
   
template<>
struct resize_impl<tag_compiletime>
{
   template<typename Sequence>
   struct apply
   {
      static void
      call(Sequence& seq, size_t size)
      {
#ifdef GSSETEST               
         std::cout << "## CT:: no resize possible .. check for size.. " << std::endl;
#endif
         if (size > (seq.size()))
         {
            // [TODO] ..exception
#ifdef GSSETEST               
            std::cout << "CT resize :: container too small " << std::endl;
#endif
         }
      }
   };
};
   
template<>
struct resize_impl<tag_runtime>
{
   template<typename Sequence>
   struct apply
   {
         
      static void
      call(Sequence& seq, size_t size)
      {
//         std::cout << "resize impl:: vector.. " << std::endl;
#ifdef GSSETEST               
         std::cout << "## RT:: (tag_runtime) resize possible .. " << std::endl;
#endif
         seq.resize( size );
      }
   };
};
   
   
template<>
struct resize_impl<tag_runtime_map>
{
   template<typename Sequence>
   struct apply
   {
         
      static void
      call(Sequence& seq, size_t size)
      {
#ifdef GSSETEST               
         std::cout << "## RT:: (tag_runtime_map)  .. resize NOT necessary .. " << std::endl;
#endif
      }
   };
};


template<>
struct resize_impl<tag_runtime_list>
{
   template<typename Sequence>
   struct apply
   {
         
      static void
      call(Sequence& seq, size_t size)
      {
#ifdef GSSETEST               
         std::cout << "## RT:: (tag_runtime_list) .. resize necessary .. " << std::endl;
#endif
         seq.resize(size);
      }
   };
};

} //namespace extension   
// =========================================================================================


struct resize_object
{
   resize_object(size_t index):index(index){}
   
   template <typename Sequence>
   void 
   operator()(Sequence& seq)
   {
      extension::resize_impl<typename traits::tag_of<Sequence>::evaluation_type>::
         template apply<Sequence>::call(seq, index);
   }
   
private:
   size_t index;
};


namespace
{
resize_object
resize(size_t size)  
{
   return resize_object(size);
}
}
 



namespace functional
{
struct resize_impl
{
   // ---------------------------------------------
   template < typename Container, typename Size >
   struct result
   {
      typedef void  type;
   };
   // ---------------------------------------------
   template < typename Container, typename Size >
   typename result< Container, Size >::type       
   operator()( Container & cont, Size size ) const
   {
      gsse::resize( size )( cont );
   }
   // ---------------------------------------------
   
};
boost::phoenix::function< gsse::functional::resize_impl > const resize  = gsse::functional::resize_impl();

} // end namespace: functional


  
} // namespace gsse
#endif


