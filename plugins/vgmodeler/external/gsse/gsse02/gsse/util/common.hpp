/* ============================================================================
   Copyright (c) 2005-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2004 Joel de Guzman    (phoenix part)
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_COMMON_HH)
#define GSSE_COMMON_HH

// *** system includes
#include <iostream>
#include <map>
#include <vector>

// *** BOOST includes
#include <boost/fusion/support/is_sequence.hpp>
#include <boost/fusion/container.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/mpl/eval_if.hpp>

// *** GSSE includes
#include "gsse/util/size.hpp"
#include "gsse/util/tag_of.hpp"
#include "gsse/util/debug_meta.hpp"
#include "gsse/util/array.hpp"


// #########################################################
//
namespace gsse { 

namespace result_of { namespace detail{

#define MEMBER_TYPE_OF(MEMBER_TYPE)                                             \
    template <typename C>                                                       \
    struct BOOST_PP_CAT(MEMBER_TYPE, _of)                                       \
    {                                                                           \
        typedef typename C::MEMBER_TYPE type;                                   \
    }

    MEMBER_TYPE_OF(allocator_type);
    MEMBER_TYPE_OF(const_iterator);
    MEMBER_TYPE_OF(const_reference);
    MEMBER_TYPE_OF(const_reverse_iterator);
    MEMBER_TYPE_OF(container_type);
    MEMBER_TYPE_OF(data_type);
    MEMBER_TYPE_OF(iterator);
    MEMBER_TYPE_OF(key_compare);
    MEMBER_TYPE_OF(key_type);
    MEMBER_TYPE_OF(reference);
    MEMBER_TYPE_OF(reverse_iterator);
    MEMBER_TYPE_OF(size_type);
    MEMBER_TYPE_OF(value_compare);
    MEMBER_TYPE_OF(value_type);

// by [RH]
   template<typename C>
   struct const_value_type_of
   {
      typedef const typename C::value_type type;
   };

#undef MEMBER_TYPE_OF
} // namespace detail

 
//namespace --> result_of

   template <typename Sequence, typename Key>
   struct at
      : boost::fusion::extension::value_at_key_impl<typename boost::fusion::detail::tag_of<Sequence>::type>::
      template apply<Sequence, Key>
   {};

   template <typename Sequence, typename Key>
   struct at_dim
      : boost::fusion::extension::value_at_key_impl<typename boost::fusion::detail::tag_of<Sequence>::type>::
      template apply<Sequence, Key>
   {};

   template <typename Sequence, int N>
   struct at_dim_c
      : boost::fusion::result_of::value_at<Sequence, boost::mpl::int_<N> >
   {};

   template <typename Sequence>
   struct at_dim_index
      : boost::fusion::result_of::value_at<typename Sequence::value_type, boost::mpl::int_<1> >   //[RH][TODO] .. _<0>
   {};
   template <typename Sequence>
   struct at_dim_fiber
      : boost::fusion::result_of::value_at<typename Sequence::value_type, boost::mpl::int_<1> >
   {};



   template <typename Sequence, typename Key=void, long ISD=0>
   struct at_dim_isd
   {};
   template <typename Sequence, typename Key>
   struct at_dim_isd<Sequence, Key, 0>
   {
      typedef typename at_dim<Sequence, Key>::type::value_type type;
   };
   template <typename Sequence, typename Key>
   struct at_dim_isd<Sequence, Key, 1>
   {
      typedef typename at_dim<Sequence, Key>::type::value_type::value_type type;
   };



   template <typename Sequence, long ISD=0>
   struct at_isd
   {};
   template <typename Sequence>
   struct at_isd<Sequence, 0>
   {
      typedef typename Sequence::mapped_type type;
//      typedef typename at_dim<Sequence, Key>::type::value_type type;
   };
   template <typename Sequence>
   struct at_isd<Sequence, 1>
   {
      typedef typename Sequence::mapped_type::mapped_type type;
//      typedef typename at_dim<Sequence, Key>::type::value_type::value_type type;
   };



   template <typename Sequence>
   struct at_dim_isd<Sequence, void, 0>
   {
      typedef typename Sequence::value_type type;
   };


// ########################################################################################

// ==========================
// (c) 2004 Joel de Guzman (from spirit/phoenix v02 source code)

    template <typename C>
    struct const_qualified_iterator_of
    {
        typedef typename
            boost::mpl::eval_if<
                boost::is_const<C>
              , gsse::result_of::detail::const_iterator_of<C>
              , gsse::result_of::detail::iterator_of<C>
            >::type
        type;
    };
    template <typename C>
    struct const_qualified_reference_of
    {
        typedef typename
            boost::mpl::eval_if<
                boost::is_const<C>
              , gsse::result_of::detail::const_reference_of<C>
              , gsse::result_of::detail::reference_of<C>
            >::type
        type;
    };
// by [RH]
    template <typename C>
    struct const_qualified_value_of
    {
        typedef typename
            boost::mpl::eval_if<
                boost::is_const<C>
              , gsse::result_of::detail::const_value_type_of<C>
              , gsse::result_of::detail::value_type_of<C>
            >::type
        type;
    };


// ==========================

   template <typename Sequence, typename Enable=void>
   struct itr 
   {
      typedef typename gsse::result_of::const_qualified_iterator_of<Sequence>::type type;
   };
   template <typename Sequence, typename Enable=void>
   struct ref
   {
      typedef typename gsse::result_of::const_qualified_reference_of<Sequence>::type type;
   };
   template <typename Sequence, typename Enable=void>
   struct value
   {
      typedef typename gsse::result_of::const_qualified_value_of<Sequence>::type type;
   };

   // =====================

   template <typename Sequence, typename SequenceType>
   struct val_impl
   {
      typedef typename gsse::result_of::const_qualified_value_of<Sequence>::type type;
//      typedef typename Sequence::value_type type;
   };

   template <typename Sequence>
   struct val_impl<Sequence, gsse::tag_runtime>
   {
      typedef typename Sequence::value_type type;
   };
   template <typename Sequence>
   struct val_impl<Sequence, gsse::tag_container_array>
   {
      typedef typename Sequence::value_type type;
   };

   template <typename Sequence>
   struct val_impl<Sequence, gsse::tag_runtime_map>
   {
      typedef typename Sequence::mapped type;
   };

   template <typename Sequence, typename Enable=void>
   struct val
   {
      typedef typename gsse::result_of::val_impl<Sequence, typename gsse::traits::tag_container_type<Sequence>::type>::type type;
   };

}  // namespace result_of


// ################################################################


template <long DIM, typename Sequence>
inline typename 
boost::lazy_disable_if<
   boost::is_const<Sequence>
 , boost::fusion::result_of::at_c<Sequence, DIM>
   >::type
at_dim(Sequence& seq)
{
   return boost::fusion::at<boost::mpl::int_<DIM> >(seq);
}
template <typename Key, typename Sequence>
inline typename
boost::lazy_disable_if<
   boost::is_const<Sequence>
 , boost::fusion::result_of::at_key<Sequence, Key>
>::type
at_dim(Sequence& seq)
{
   return boost::fusion::at_key<Key>(seq);
}

template <long DIM, typename Sequence>
inline typename
boost::fusion::result_of::at_c<Sequence const, DIM>::type 
at_dim(Sequence const& seq)
{
   return boost::fusion::at<boost::mpl::int_<DIM> >(seq);
}
template <typename Key, typename Sequence>
inline typename
boost::fusion::result_of::at_key<Sequence const, Key>::type 
at_dim(Sequence const& seq)
{
  return boost::fusion::at_key<Key>(seq);
}


// ##########################################################
// shortcuts // [RH][TODO] .. check this file


// [RH][info] .. added this namespace to avoid a nameclash ???? 
// [RH][TODO] .. check this..

namespace debug{

template <long DIM, typename Sequence>
inline typename
boost::fusion::result_of::at_c<Sequence, DIM>::type
at(Sequence& seq)
{
   return boost::fusion::at<boost::mpl::int_<DIM> >(seq);
}

template <typename Key, typename Sequence>
inline typename
boost::fusion::result_of::at_key<Sequence, Key>::type
at(Sequence& seq)
{
   return boost::fusion::at_key<Key>(seq);
}

}


// ====================================================================================
//

template <typename ContainerInformation>
struct is_gsse_container 
{
   enum { value = false };  
};

template <>
struct is_gsse_container<gsse::gsse_container>
{
   enum { value = true };  
};



template <typename KeyType>
struct is_associative
{
   enum { value = false };  
};
template <>
struct is_associative<gsse::key_associative>
{
   enum { value = true };  
};

template <typename KeyType>
struct is_transfer
{
   enum { value = false };  
};
template <>
struct is_transfer<gsse::transfer_key>
{
   enum { value = true };  
};

template <typename ContainerInformation>
struct is_traversable 
{
   enum { value = false };  
};
template <>
struct is_traversable<gsse::gsse_container>
{
   enum { value = true };  
};




// ====================================================================================
//
// generic serialization mechanisms
//
   struct output_mechanism
   {
      friend std::ostream& operator<<(std::ostream& ostr, output_mechanism om)
      {
         return ostr;
      }
   };
   struct input_mechanism
   {
      friend std::istream& operator>>(std::istream& istr, input_mechanism im)
      {
         return istr;;
      }
   };

   // [RH][TODO] .. merge/separate this in more detail
   struct no_type{};
   template<typename T1>
   struct no_type1{};
   template<typename T1, typename T2>
   struct no_type2{};

   struct notype :  output_mechanism, input_mechanism  {  };
   struct notype0 : notype{};
   struct notype1 : notype{};
   struct notype2 : notype{};
   struct notype3 : notype{};
   struct notype4 : notype{};
   struct notype5 : notype{};
   struct notype6 : notype{};
   struct notype7 : notype{};
   struct notype8 : notype{};

   
   // ### quantity complex (fiber-bundle collection)
   //  is based on _co-chains_
   //
   template<typename StorageType, size_t DIM=0>
   struct cell_cochain   {  };

   template<typename StorageType>
   struct cell_cochain<StorageType, 1> : gsse::array<StorageType, 1>
   {
      typedef gsse::array<StorageType, 1> DerivedT;

      boost::mpl::int_<1> cardinality;
      
      friend std::istream& operator>>(std::istream &is,gsse::cell_cochain<StorageType, 1> &cochain)
      {
         is>>cochain[0];
         return is;
      }
//      friend std::ostream& operator <<(std::ostream &os,const Base &obj);
   };
   template<typename StorageType>
   struct cell_cochain<StorageType, 2>: gsse::array<StorageType, 2>
   {
      typedef gsse::array<StorageType, 2> DerivedT;

      boost::mpl::int_<2> cardinality;
      
      friend std::istream& operator>>(std::istream &is,gsse::cell_cochain<StorageType, 2> &cochain)
      {
         is>>cochain[0];
         is>>cochain[1];
         return is;
      }
//      friend ostream& operator <<(ostream &os,const Base &obj);
   };
   template<typename StorageType>
   struct cell_cochain<StorageType, 3>: gsse::array<StorageType, 3>
   {
      typedef gsse::array<StorageType, 3> DerivedT;

      boost::mpl::int_<3> cardinality;
      
      friend std::istream& operator>>(std::istream &is,gsse::cell_cochain<StorageType, 3> &cochain)
      {
         is>>cochain[0];
         is>>cochain[1];
         is>>cochain[2];
         return is;
      }
//      friend ostream& operator <<(ostream &os,const Base &obj);
   };


   template<typename StorageType>
   struct cell_cochain<StorageType, 4>: gsse::array<StorageType, 4>
   {
      typedef gsse::array<StorageType, 4> DerivedT;

      boost::mpl::int_<4> cardinality;
      
      friend std::istream& operator>>(std::istream &is,gsse::cell_cochain<StorageType, 4> &cochain)
      {
         is>>cochain[0];
         is>>cochain[1];
         is>>cochain[2];
         is>>cochain[3];
         return is;
      }
//      friend ostream& operator <<(ostream &os,const Base &obj);
   };

   
   // map declarator to circumvent the template template (partial) specialization problem
   template<typename StorageType, typename T2> class map: public std::map<long, StorageType>
   {  };


// ##############################################################################
// ###########################################





} // namespace gsse


#endif
