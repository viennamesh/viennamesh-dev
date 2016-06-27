/* ============================================================================
   Copyright (c) 2007-2010 Rene Heinzl                             rene@gsse.at

   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_ADAPTED_TAGOF_HH)
#define GSSE_ADAPTED_TAGOF_HH

// *** system includes
#include <vector>
#include <set>
#include <list>
#include <map>
#include <valarray>
#include <tr1/unordered_map>
// *** BOOST includes
#include <boost/array.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/adapted/array.hpp>
#include <boost/rational.hpp>  // only for tag dispatching

// *** GSSE includes
#include <boost/numeric/mtl/mtl.hpp>   // for now in /gsse/external/boost
#include "gsse/adapted/orq.hpp"
//#include "gsse/algorithm/fem/matrix.hpp"


// [RH][TODO] find a way to not include all the header files
//   just use the files which are defined elsewhere
//  SO -> the facades should just be inserted if is necessary


// ############################################################
//

// forward declarations
namespace boost  { 

   template<typename T, std::size_t N>
   class array;

namespace fusion { 

   struct array_tag;
   struct random_access_traversal_tag;

}  // namespace fusion
} // namespace boost

namespace gsse {
  template<long T1, typename T2>
  struct vec_ct;
}

namespace std { 
// #ifndef _VECTOR_H
// //    template<typename T,  typename _Alloc = std::allocator<T> >
// //    class vector;

//    template<typename T,  typename _Alloc  >
//    class vector;
// #endif

// #ifndef _LIST_H
// //   template<typename T,  typename _Alloc = std::allocator<T> >

//    template<typename T,  typename _Alloc  >
//    class list;
// #endif


//    // stl map:
//    template<typename _Key>
//    class less;

//    template<typename T>
//    class allocator;

//    template<typename T1, typename T2>
//    class pair;
   
//    template<typename T>
//    class equal_to;


// #ifndef _MAP_H   // GCC's map declaration
// //    template <typename _Key, typename _Tp, typename Compare = std::less<_Key>,
// //              typename _Alloc = std::allocator<std::pair<const _Key, _Tp> > >

//    template <typename _Key, typename _Tp, typename Compare , typename _Alloc >
//    class map;
// #endif
   

//    namespace tr1
//    {
// #ifndef GNU_LIBSTDCXX_TR1_UNORDERED_MAP_ // GCC's unordered map declaration
//       template<typename T>
//       class hash;

// //       template<class Key, class T,
// //                class Hash = hash<Key>,
// //                class Pred = std::equal_to<Key>,
// //                class Alloc = std::allocator<std::pair<const Key, T> >,
// //                bool cache_hash_code = false>
      
//       template<class Key, class T,
//                class Hash ,
//                class Pred ,
//                class Alloc  >
//                class unordered_map;
// #endif
//    }
}

namespace gsse  { namespace facade    {

   template<typename T, typename T2>
   class access_facade;
   
   template<typename T, typename T2, long IndexSpaceDepth=0> // set initialy to zero, no separate index
   class index_facade;
   
   template<typename T>
   class map_facade;

}    //namespace facade

namespace traversal {

   template<typename T>
   class traversal_facade;
   
}  //namespace traversal 


   template<typename DataType, std::size_t N>
   struct array;

   template<typename TA, typename TB>
   class map;

namespace fiberbundle     {

      template<long DIM, typename CellType, typename FB0, typename FB1, typename FB2, typename FB3, typename FB4, typename FB5, typename FB6, typename FB7, typename FB8 >
      struct collection;
}  // namespace fiberbundle


   template<typename StorageType, size_t DIM>
   struct cell_cochain;

   template<typename NumericType, long N>
   class metric_object;

// =============================================================================

   struct tag_none{};
   struct tag_compiletime{};
   struct tag_runtime {};
   struct tag_runtime_pair : public tag_runtime{};   // actually not a pair, but a fusion::vector
   struct tag_runtime_map  : public tag_runtime{};
   struct tag_runtime_set  : public tag_runtime{};
   struct tag_runtime_list : public tag_runtime{};
   
   struct gsse_container{};
   struct tag_container{};
   struct tag_container_array{};

   struct key_associative{};
   struct transfer_key{};

   struct tag_metric_object {};       // [RH][TODO] check with other types
   struct tag_gsse_metric_object {};  // [RH][TODO] check with other types
   struct tag_gsse_array {};
   struct tag_boost_array {};
   struct tag_fusion_vector {};
   struct tag_boost_tuples_tuple {};
   struct array_tag {};

   struct cell_simplex {};
   struct cell_cube{};
   struct cell_arbitrary{};
   struct cell_nothing{};


   struct tag_rational{};
   struct tag_mpfr{};
   struct rational_farey{};
   struct rational_cgal{};

namespace traits    {

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

   // ################## COMPILE-TIME ##########################
   
   template<typename T, std::size_t N>
   struct tag_of<boost::array<T,N> >
   {
      typedef array_tag       type;
     //      typedef tag_compiletime type;
      typedef tag_compiletime evaluation_type;
      typedef gsse_container  container_type;
   };
   template<typename T, std::size_t N>
   struct tag_of<const boost::array<T,N> >
   {
      typedef array_tag       type;
     //      typedef tag_compiletime type;
      typedef tag_compiletime evaluation_type;
      typedef gsse_container  container_type;
   };
   // ---------------------------------------
   template<int N, typename T>
   struct tag_of<ads::FixedArray<N,T> >
   {
      typedef array_tag       type;
      typedef tag_compiletime evaluation_type;
      typedef gsse_container  container_type;
   };   
   template<int N, typename T>
   struct tag_of<const ads::FixedArray<N,T> >
   {
      typedef array_tag       type;
      typedef tag_compiletime evaluation_type;
      typedef gsse_container  container_type;
   };      
   // ---------------------------------------
   template<typename T0>
   struct tag_of<boost::fusion::vector<T0> >
   {
      typedef tag_compiletime type;
      typedef tag_compiletime evaluation_type;
   };
   template<typename T0, typename T1>
   struct tag_of<boost::fusion::vector<T0, T1> >
   {
      typedef tag_compiletime type;
      typedef tag_compiletime evaluation_type;
   };
   template<typename T0, typename T1, typename T2>
   struct tag_of<boost::fusion::vector<T0, T1, T2> >
   {
      typedef tag_compiletime type;
      typedef tag_compiletime evaluation_type;
   };
   
   
//    template<typename T, std::size_t N, typename T2>
//    struct tag_of<gsse::facade::index_facade<boost::array<T,N>, T2 > >
//    {
//       typedef tag_compiletime type;
//       typedef tag_compiletime evaluation_type;
//       typedef gsse_container  container_type;
//    };
//    template<typename T, std::size_t N, typename T2, typename T3>
//    struct tag_of<gsse::facade::access_facade<gsse::facade::index_facade<boost::array<T,N>, T2 >, T3> >
//    {
//       typedef tag_compiletime type;
//       typedef tag_compiletime evaluation_type;
//       typedef gsse_container  container_type;
//    };
   
//    template<typename T, std::size_t N, typename T2>
//    struct tag_of<gsse::facade::index_facade<boost::array<T,N>, T2,1 > >
//    {
//       typedef tag_compiletime type;
//       typedef tag_compiletime evaluation_type;
//       typedef gsse_container  container_type;
//    };
//    template<typename T, std::size_t N, typename T2, typename T3>
//    struct tag_of<gsse::facade::access_facade<gsse::facade::index_facade<boost::array<T,N>, T2,1 >, T3> >
//    {
//       typedef tag_compiletime type;
//       typedef tag_compiletime evaluation_type;
//       typedef gsse_container  container_type;
//    };
   

   template<typename T, std::size_t N>
   struct tag_of<gsse::array<T,N> >
   {
      typedef array_tag       type;

     //      typedef tag_compiletime type;
      typedef tag_compiletime evaluation_type;
      typedef gsse_container  container_type;
   };
   template<typename T, std::size_t N>
   struct tag_of<const gsse::array<T,N> >
   {
      typedef array_tag       type;

     //      typedef tag_compiletime type;
      typedef tag_compiletime evaluation_type;
      typedef gsse_container  container_type;
   };
   
   template<typename T, std::size_t N>
   struct tag_of<gsse::cell_cochain<T,N> >
   {
      typedef tag_compiletime type;
      typedef tag_compiletime evaluation_type;
      typedef gsse_container  container_type;
   };
   
   
   template<typename NT, long SizeN>
   struct tag_of<gsse::metric_object<NT,SizeN> >
   {
      typedef array_tag       type;

     //      typedef tag_compiletime type;
      typedef tag_compiletime evaluation_type;
      typedef gsse_container  container_type;
   };
   
   template<typename NT, long SizeN>
   struct tag_of<const gsse::metric_object<NT,SizeN> >
   {
      typedef array_tag       type;

     //      typedef tag_compiletime type;
      typedef tag_compiletime evaluation_type;
      typedef gsse_container  container_type;
   };
   
   // ################## RUN-TIME ##########################
   template<typename T>
   struct tag_of<std::list<T> >
   {
      typedef tag_runtime_list type;
      typedef tag_runtime_list evaluation_type;
      typedef gsse_container   container_type;
   };

   template<typename T1>
   struct tag_of<std::vector<T1> >
   {
      typedef tag_runtime     type;
      typedef tag_runtime     evaluation_type;
      typedef gsse_container  container_type;
   };
   
   template<typename T1, typename T2>
   struct tag_of<std::vector<T1, T2> >
   {
      typedef tag_runtime     type;
      typedef tag_runtime     evaluation_type;
      typedef gsse_container  container_type;
   };
   template<typename T1>
   struct tag_of<const std::vector<T1> >
   {
      typedef tag_runtime     type;
      typedef tag_runtime     evaluation_type;
      typedef gsse_container  container_type;
   };
   template<typename T1, typename T2>
   struct tag_of<const std::vector<T1, T2> >
   {
      typedef tag_runtime     type;
      typedef tag_runtime     evaluation_type;
      typedef gsse_container  container_type;
   };
   
   
//    template<typename T, typename T2>
//    struct tag_of<gsse::facade::index_facade<std::vector<T>, T2> >
//    {
//       typedef tag_runtime type;
//       typedef tag_runtime     evaluation_type;
//       typedef gsse_container  container_type;
//    };
   
//    template<typename T, typename T2, typename T3>
//    struct tag_of<gsse::facade::access_facade<gsse::facade::index_facade<std::vector<T>, T2>, T3> >
//    {
//       typedef tag_runtime type;
//       typedef tag_runtime     evaluation_type;
//       typedef gsse_container  container_type;
//    };
//    template<typename T, typename T2>
//    struct tag_of<gsse::facade::index_facade<std::vector<T>, T2,1> >
//    {
//       typedef tag_runtime_pair  type;
//       typedef tag_runtime_pair  evaluation_type;
//       typedef gsse_container    container_type;
//    };
   
//    template<typename T, typename T2, typename T3>
//    struct tag_of<gsse::facade::access_facade<gsse::facade::index_facade<std::vector<T>, T2,1>, T3> >
//    {
//       typedef tag_runtime_pair  type;
//       typedef tag_runtime_pair  evaluation_type;
//       typedef gsse_container    container_type;
//    };
   
//    template<typename T>
//    struct tag_of<gsse::traversal::traversal_facade<std::vector<T> > >
//    {
//       typedef tag_runtime type;
//       typedef tag_runtime     evaluation_type;
//       typedef gsse_container  container_type;
//    };
   
   template<typename TA, typename TB>
   struct tag_of<std::map<TA, TB> >
   {
      typedef tag_runtime_map  type;
      typedef tag_runtime_map  evaluation_type;
      typedef gsse_container   container_type;
   };
   template<typename TA, typename TB>
   struct tag_of<gsse::map<TA, TB> >
   {
      typedef tag_runtime_map  type;
      typedef tag_runtime_map  evaluation_type;
      typedef gsse_container   container_type;
   };
   template<typename TA, typename TB>
   struct tag_of<std::set<TA, TB> >
   {
      typedef tag_runtime_set  type;
      typedef tag_runtime_set  evaluation_type;
      typedef gsse_container   container_type;
   };


//     template<typename TA>
//     struct tag_of<mtl::dense2D<TA> >
//     {
//        typedef tag_runtime     evaluation_type;
//        typedef gsse_container  container_type;
//     };

    template<typename TA>
    struct tag_of<mtl::vector::dense_vector<TA> >
    {
       typedef tag_runtime     evaluation_type;
       typedef gsse_container  container_type;
    };


  
//    template<typename TA, typename TB, typename T3>
//    struct tag_of<gsse::facade::access_facade<gsse::facade::map_facade<std::map<TA, TB> >, T3> >
//    {
//       typedef tag_runtime_map type;
//       typedef tag_runtime_map  evaluation_type;
//       typedef gsse_container   container_type;
//    };

//    template<typename TA, typename TB>
//    struct tag_of<gsse::traversal::traversal_facade<gsse::facade::map_facade<std::map<TA, TB> > > >
//    {
//       typedef tag_runtime_map type;
//       typedef tag_runtime_map  evaluation_type;
//       typedef gsse_container   container_type;
//    };


   template<typename TA, typename TB>
   struct tag_of<std::tr1::unordered_map<TA, TB> >  
   {
      typedef tag_runtime_map type;
      typedef tag_runtime_map  evaluation_type;
      typedef gsse_container   container_type;
   };

//    template<typename TA, typename TB>
//    struct tag_of<gsse::facade::map_facade<std::tr1::unordered_map<TA, TB> > > 
//    {
//       typedef tag_runtime_map type;
//       typedef tag_runtime_map  evaluation_type;
//       typedef gsse_container   container_type;
//    };
//    template<typename TA, typename TB, typename T3>
//    struct tag_of<gsse::facade::access_facade<gsse::facade::map_facade<std::tr1::unordered_map<TA, TB> >, T3> >
//    {
//       typedef tag_runtime_map type;
//       typedef tag_runtime_map  evaluation_type;
//       typedef gsse_container   container_type;
//    };



   template<typename Sequence, typename Enable=void>
   struct tag_container_type
   {
      typedef typename Sequence::container_t_type type;
   };



   template<typename T1, typename T2>
   struct tag_container_type<std::vector<T1, T2> >
   {
      typedef tag_container_array type;
   };
   template<typename T1, unsigned long T2>
   struct tag_container_type<boost::array<T1, T2> >
   {
      typedef tag_container_array type;
   };
   template<typename T1, unsigned long T2>
   struct tag_container_type<gsse::array<T1, T2> >
   {
      typedef tag_container_array type;
   };
   template<long T1, typename T2>
   struct tag_container_type<gsse::vec_ct<T1, T2> >
   {
      typedef tag_container_array type;
   };
   template<int T1, typename T2 >
   struct tag_container_type<ads::FixedArray<T1, T2> >
   {
      typedef tag_container_array type;
   };
   template<int T1, typename T2 >
   struct tag_container_type<const ads::FixedArray<T1, T2> >
   {
      typedef tag_container_array type;
   };   


} // namespace traits
} // namespace gsse
#endif
