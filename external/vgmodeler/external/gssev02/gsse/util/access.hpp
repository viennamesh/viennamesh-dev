/* ============================================================================
   Copyright (c) 2007-2008 Rene Heinzl                             rene@gsse.at

  resize/at protocols are 
   Copyright (c) 2001-2006 Joel de Guzman 
   Copyright (c) 2006      Dan Marsden
  switch concept 
   Copyright (c) 2007      Steven Watanabe
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_UTIL_ACCESS_HH)
#define GSSE_UTIL_ACCESS_HH

// *** system includes
// *** BOOST includes
#include <boost/type_traits/is_const.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/support/tag_of.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>
#include <boost/type_traits/remove_const.hpp>


// *** GSSE includes
#include "gsse/util/at_complex.hpp"
#include "gsse/util/specifiers.hpp"
#include "gsse/util/common.hpp"
#include "gsse/util/tag_of.hpp"


namespace gsse
{
namespace extension
{

template <typename Tag>
struct at_impl
{
   template <typename Sequence, typename IndexT=long>
   struct apply;
};

      
template<>
struct at_impl<tag_compiletime>
{
   // [info] .. this can only be used for
   //           _homogeneous_ containers
   //        the data-type is extracted from the first entry
   //
   template<typename Sequence, typename IndexT>
   struct apply
   {
      typedef typename gsse::result_of::ref<Sequence>::type type;

//       // [TODO]   enable_if and const_reference qualifier
//       typedef typename Sequence::reference type;

      static
      type
      call(Sequence& seq, IndexT index)
      {
          return seq[index];
       }
   };
};

template<>
struct at_impl<tag_runtime>
{
   template<typename Sequence, typename IndexT>
   struct apply
   {
      
      typedef typename gsse::result_of::ref<Sequence>::type type;
            
      static type
      call(Sequence& seq, IndexT index)
      {
#ifndef GSSE_HIGH_PERFORMANCE_NO_CHECKS
         if (index >= static_cast<IndexT>(seq.size()))
         {
            seq.resize(index+1);
         }
#endif

         return seq[index];
      }
   };
};

template<>
struct at_impl<tag_runtime_pair>
{
   template<typename Sequence, typename IndexT>
   struct apply
   {
//      typedef typename gsse::result_of::ref<Sequence>::type type;

      // [TODO]   enable_if and const_reference qualifier
      typedef typename boost::fusion::result_of::at_c<typename Sequence::value_type,1>::type type;  // second
            
      static type
      call(Sequence& seq, IndexT index)
      {
         return boost::fusion::at_c<1>(seq[index]);
      }
   };
};


template<>
struct at_impl<tag_runtime_map>
{
   template<typename Sequence, typename IndexT>
   struct apply
   {
      typedef typename Sequence::mapped_type& type;
//       typedef typename gsse::result_of::ref<Sequence>::type value_type;
//       typedef typename value_type::second_type type;
      // [RH][new][TODO] .. this does not work .. 
          
      static type
      call(Sequence& seq, IndexT index)
      {
 /*
   std::cout << "test 1" <<std::endl;
	gsse::dump_type<Sequence>();
	gsse::dump_type<IndexT>();
      	std::cout << "test 2" <<std::endl;
*/	
//	return type();
	return seq[index];
      }
   };
};


template<>
struct at_impl<tag_runtime_list>
{
   template<typename Sequence, typename IndexT>
   struct apply
   {
      typedef typename Sequence::value_type& type;
            
      static type
      call(Sequence& seq, IndexT index)
      {
         IndexT seq_size = seq.size();
         if (index < seq_size)
         { 
            typename Sequence::iterator it;
            it = seq.begin();
            std::advance(it, index);
            return *it;
         }

         // other cases
         //
         seq.resize( index + 1); 
         return seq.back();
      }
   };
};




template<>
struct at_impl<tag_none>
{
//    template<typename Sequence, typename IndexT>
//    struct apply
//    {
//       // [TODO]   enable_if and const_reference qualifier
            
//       typedef typename Sequence::reference type;
            
//       static type
//       call(Sequence& seq, IndexT index)
//       {
//          return seq[index];
//       }
//    };
};
} // namespace extension



namespace result_of
{
template <typename Sequence, typename IndexT>
struct at_c
{
   typedef typename
   //   gsse::extension::at_impl<typename traits::tag_of<Sequence>::type>::
   gsse::extension::at_impl<typename traits::tag_of<Sequence>::evaluation_type>::
   template apply<Sequence, IndexT>::type
   type;
};
}  //namespace result_of




template<typename IndexT>
struct at_object
{
   at_object(IndexT index):index(index){}
   
   template <typename Sequence>
   typename 
   result_of::at_c<Sequence, IndexT>::type
   operator()(Sequence& seq)
   {
      //   typedef result_of::at_c<Sequence, IndexT> at_meta;   // [RH][TODO][remove this]
     //      return gsse::extension::at_impl<typename traits::tag_of<Sequence>::type>::
      return gsse::extension::at_impl<typename traits::tag_of<Sequence>::evaluation_type>::
         template apply<Sequence, IndexT>::call(seq, index);
   }


   template <typename Sequence>
   const typename Sequence::value_type&    // [RH][TODO]
   operator()(const Sequence& seq)
   {
      return seq[index];
   }

private:
   IndexT index;
};


template<typename Index>
at_object<Index>
at(Index index=0)  
{
    return at_object<Index>(index);
}


// ##################

template<typename Accessor>
struct at_gsse_ds
{
   at_gsse_ds(long index):index(index){}
   
   template <typename Sequence>
   typename gsse::result_of::at_dim< typename gsse::result_of::at_fiber_val<Sequence>::type, Accessor>::type::value_type& // [RH][TODO]  .. use "at" protocol
   operator()(Sequence& seq)
   {
      return gsse::at(index) (gsse::at_dim< Accessor >(  gsse::at_fiber(seq)  ) );
   }

private:
   long index;
};


template<typename Index>
at_gsse_ds<gsse::access_specifier::AT_vx>
at_vx(Index index=0)  
{
   return at_gsse_ds<gsse::access_specifier::AT_vx>(index);
}
template<typename Index>
at_gsse_ds<gsse::access_specifier::AT_ee>
at_ee(Index index=0)  
{
   return at_gsse_ds<gsse::access_specifier::AT_ee>(index);
}
template<typename Index>
at_gsse_ds<gsse::access_specifier::AT_f0>
at_f0(Index index=0)  
{
   return at_gsse_ds<gsse::access_specifier::AT_f0>(index);
}
template<typename Index>
at_gsse_ds<gsse::access_specifier::AT_cl>
at_cl(Index index=0)  
{
   return at_gsse_ds<gsse::access_specifier::AT_cl>(index);
}



// ==========================

// [RH][TODO]
//
// struct at_vx
// {
//    at_vx(long index):index(index){};

//    template<typename Container>
//    typename gsse::result_of::at_c<Container, long>::type
//    operator()(Container const& container)
//    {
//       return gsse::at(index)(gsse::at_dim<gsse::access_specifier::AT_vx>( gsse::at_fiber(container) ) );
//    }

// private:
//    long index;
// };
// struct at_cl
// {
//    at_cl(long index):index(index){};

//    template<typename Container>
//    typename gsse::result_of::at_c< 
//       typename gsse::result_of::at_dim<
//          typename gsse::result_of::at_fiber<Container>::type 
//        , gsse::access_specifier::AT_cl >::type  
//       , long>::type 
//    operator()(Container const& container)
//    {
//       return gsse::at_object<long>(index)  (gsse::at_dim<gsse::access_specifier::AT_cl>( gsse::at_fiber(container) ) );
//    }

// private:
//    long index;
// };




// ################################################################################################
// ################################################################################################


template<typename Tag>
struct pushback_impl
{
   template<typename Sequence>
   struct apply;
};

template<>
struct pushback_impl<tag_runtime>
{
   template<typename Sequence>
   struct apply
   {
      typedef typename Sequence::value_type type;
            
      static type&
      call(Sequence& seq)
      {
         seq.push_back(type());
         return seq.back();
      }
   };
};

template<>
struct pushback_impl<tag_runtime_list>
{
   template<typename Sequence>
   struct apply
   {
      typedef typename Sequence::value_type type;
            
      static type&
      call(Sequence& seq)
      {
         seq.push_back(type());
         return seq.back();
      }
   };
};


namespace result_of
{
template <typename Sequence>
struct pushback_c
{
   typedef typename Sequence::value_type type;
//    gsse::pushback_impl<typename traits::tag_of<Sequence>::type>::
//    template apply<Sequence>::type
//    type;
};
}  //namespace result_of


template<typename Sequence>
//typename result_of::pushback_c<Sequence>&
typename Sequence::value_type&
pushback(Sequence& seq)  
{
       return gsse::pushback_impl<typename traits::tag_of<Sequence>::type>::
          template apply<Sequence>::call(seq);
}


// template<typename Sequence>
// typename result_of::pushback_c<Sequence>
// pushback(Sequence& seq)  
// {
//       return gsse::pushback_impl<typename traits::tag_of<Sequence>::type>::
//          template apply<Sequence>::call(seq);
// }


} // namespace gsse


#endif



