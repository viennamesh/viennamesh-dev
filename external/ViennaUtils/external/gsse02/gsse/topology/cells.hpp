/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_TOPOLOGY_CELLS_HH)
#define GSSE_TOPOLOGY_CELLS_HH

// *** GSSE includes
#include "gsse/math/power.hpp"
#include "gsse/util/common.hpp"
#include <boost/integer/static_log2.hpp>


namespace gsse { 

template<typename CellType>
struct cell_cardinality_calculator
{
   template<long DIM>
   struct apply
   {
      static const long value = 0;
   };

   long operator()(long cardinality) const
   {
      return 0;
   }

};

template<>
struct cell_cardinality_calculator<gsse::cell_simplex>
{
   template<long DIM>
   struct apply
   {
      static const long value = DIM+1;
   };

   long operator()(long dim) const
   {
      return dim + 1;
   }

};

template<>
struct cell_cardinality_calculator<gsse::cell_cube>
{
   template<long DIM>
   struct apply
   {
      static const long value = gsse::power_ct<2,DIM>::value;
   };

   long operator()(long dim) const
   {
      // [PS] calculate 2^dim and avoid pow() as it causes problems on older compilers (ambiguous)
      // 
      return (1 << dim) ; 
   }

};

// #####################

template<typename CellType>
struct cell_reverse_cardinality_calculator
{
   template<long CARDINALITY>
   struct apply
   {
      static const long value = 0;
   };

   long operator()(long cardinality) const
   {
      return 0; // [RH][TODO] .. warning?? log??
   }
};

template<>
struct cell_reverse_cardinality_calculator<gsse::cell_simplex>
{
   template<long CARDINALITY>
   struct apply
   {
      static const long value = CARDINALITY - 1;
   };

   long operator()(long cardinality) const
   {
      return cardinality - 1;
   }
};

template<>
struct cell_reverse_cardinality_calculator<gsse::cell_cube>
{
   template<long CARDINALITY>
   struct apply
   {
      static const long value = boost::static_log2<CARDINALITY>::value;
   };

   long operator()(long cardinality) const
   {
      // this is a purely integer (long) based implementation of log2 (logarithm base 2)
      // and avoids complaints from compilers about type conversions [PS]
      // [PS TODO] find a better fix for this
      // 
      long l(0);
      while( (cardinality >> l) > 1 ) ++l;
      return l;
      // return std::log(cardinality) / std::log(2);
   }

};




// #########################

template <typename Tag>
struct cell_container
{
   template<typename DataType, long Size>
   struct apply
   {
      // error
   };
};

template <>
struct cell_container<cell_nothing>
{
   template<typename DataType, long Size>
   struct apply
   {
      typedef DataType type;
   };
};

template<>
struct cell_container<cell_simplex>
{
   template<typename DataType, long Size>
   struct apply
   {
      //typedef boost::array<DataType, Size> type;
      typedef gsse::array<DataType, Size> type;
   };
};

template<>
struct cell_container<cell_cube>
{
   template<typename DataType, long Size>
   struct apply
   {
      typedef gsse::array<DataType, Size> type;
      //typedef boost::array<DataType, Size> type;
   };
};

template<>
struct cell_container<cell_arbitrary>
{
   template<typename DataType, long Size>
   struct apply
   {
      typedef std::vector<DataType> type;
   };
};







} //namespace gsse

#endif


