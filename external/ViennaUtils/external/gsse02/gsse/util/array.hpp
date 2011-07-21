/* ============================================================================
   Copyright (c) 20082010 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_ARRAY_HH)
#define GSSE_ARRAY_HH


// *** BOOST includes
#include "boost/array.hpp"



namespace gsse {


template<typename DataType, std::size_t N>
struct array : public boost::array<DataType, N> 
{ 
   typedef boost::fusion::random_access_traversal_tag category;  
   static const long Size = N;

   // [RH][TODO]
   //   merge this with space/metric_object.hpp -> metric_object
   //
   array()
   {
      for (size_t ci = 0; ci < N; ++ci)
      {
         (*this)[ci] = DataType(0);
      }
   }

   array(DataType a1)
   {
      //BOOST_STATIC_ASSERT(SizeN == 1);
   
      (*this)[0] = a1;
   }


   array(DataType a1, DataType a2)
   {
      //BOOST_STATIC_ASSERT(SizeN == 2);
   
      (*this)[0] = a1;
      (*this)[1] = a2;
   }
      
   array(DataType a1, DataType a2, DataType a3)
   {
      //BOOST_STATIC_ASSERT(SizeN == 3);   
      (*this)[0] = a1;
      (*this)[1] = a2;
      (*this)[2] = a3;
   }

   array(DataType a1, DataType a2, DataType a3, DataType a4)
   {
      //BOOST_STATIC_ASSERT(SizeN == 3);   
      (*this)[0] = a1;
      (*this)[1] = a2;
      (*this)[2] = a3;
      (*this)[3] = a4;
   }

};

} //namespace gsse



// [RH][newly inserted to introduce gsse::array as fully functional boost::array
//

/*
namespace boost { namespace fusion 
{
	struct boost_array_tag;
	struct fusion_sequence_tag;

	namespace traits
	{
		template<typename T, std::size_t N>
		struct tag_of<gsse::array<T,N> >
		{
			typedef boost_array_tag type;
		};
	}
}}
namespace boost { namespace mpl
{
	template<typename>
	struct sequence_tag;

	template<typename T, std::size_t N>
	struct sequence_tag<gsse::array<T,N> >
	{
		typedef fusion::fusion_sequence_tag type;
	};

	template<typename T, std::size_t N>
	struct sequence_tag<gsse::array<T,N> const>
	{
		typedef fusion::fusion_sequence_tag type;
	};
}}
*/



#endif
