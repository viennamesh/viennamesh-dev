/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_POINT_HH_ID
#define GSSE_POINT_HH_ID 

// *** system includes
//
#include <iterator>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <numeric>      /// inner product
#include <cmath>

// *** BOOST includes
//
#include <boost/array.hpp>
#include <boost/operators.hpp>
#include <boost/static_assert.hpp>

// *** GSSE includes
//
#include "gsse/topology.hpp"


namespace gsse
{

///////////////////////////////////////////////////////////////////////////
//
// Generic nD point 
//  with BOOST operator facade
//
///////////////////////////////////////////////////////////////////////////
// requires: NumericType 
// function: create coordinate values 

template<typename NumericType, long N>
class generic_point
   : boost::addable< generic_point<NumericType, N> >,
     boost::subtractable< generic_point<NumericType, N> >,
     boost::multipliable< generic_point<NumericType, N>, NumericType>,
     boost::dividable< generic_point<NumericType, N>, NumericType >,
     boost::equality_comparable< generic_point<NumericType, N>  >
{
   typedef generic_point<NumericType,N>  self;
   typedef boost::array<NumericType,N>   coordinates_t; 

  coordinates_t   coordinates;
//   bool local_valid;

public:
   typedef NumericType                                   value_type;
   typedef NumericType                                   numeric_t;
   typedef typename coordinates_t::iterator              iterator;
   typedef typename coordinates_t::const_iterator        const_iterator;

   static const long dimension = N;
	
   generic_point() 
   {
      for (int i=0; i<N; i++)
      {
         coordinates[i]=NumericType(0.0);
      }
   }

   ///////////////////////////////////////////////////////////////////////////
   //
   // Generic n-dimensional constructor 
   //
   ///////////////////////////////////////////////////////////////////////////
   // requires: NumericType convertible
   // function: create coordinate values 
	
   template<typename InputIterator>
   generic_point(InputIterator first, InputIterator last)
      {
         for (int i=0; i<N; i++)
         {
            coordinates[i] = *first;
            ++first;
         }
      }
   
   ///////////////////////////////////////////////////////////////////////////
   //
   // 1D constructor
   //
   ///////////////////////////////////////////////////////////////////////////
   // requires: 
   // function: create 1D coordinate values 

   generic_point(NumericType a1)
      {
	//BOOST_STATIC_ASSERT(N == 1);

         coordinates[0] = a1;
      }

         
   ///////////////////////////////////////////////////////////////////////////
   //
   // 2D constructor
   //
   ///////////////////////////////////////////////////////////////////////////
   // requires: 
   // function: create 2D coordinate values 

   generic_point(NumericType a1, NumericType a2)
      {
         //BOOST_STATIC_ASSERT(N == 2);

         coordinates[0] = a1;
         coordinates[1] = a2;
      }
         
   ///////////////////////////////////////////////////////////////////////////
   //
   // 3D constructor
   //
   ///////////////////////////////////////////////////////////////////////////
   // requires: 
   // function: create 3D coordinate values 

   generic_point(NumericType a1, NumericType a2, NumericType a3)
   {
      //BOOST_STATIC_ASSERT(N == 3);   
      coordinates[0] = a1;
      coordinates[1] = a2;
      coordinates[2] = a3;
   }

   ///////////////////////////////////////////////////////////////////////////
   //
   // Point methods
   //
   ///////////////////////////////////////////////////////////////////////////

   //
   // operators with FixedArray operand
   //

   long size() const {return N;}

   self& operator+=( const self& x )
   {
      iterator i = begin(), i_end = end();
      const_iterator j = x.begin();
      for ( ; i != i_end; ++i, ++j ) {
         *i += *j;
      }
      return *this;
   }
   
   self& operator-=( const self& x )
   {
      iterator i = begin(), i_end = end();
      const_iterator j = x.begin();
      for ( ; i != i_end; ++i, ++j ) {
         *i -= *j;
      }
      return *this;
   }


   self& operator*=( const double x )
   {
      iterator i = begin(), i_end = end();
      for ( ; i != i_end; ++i ) 
      { *i *= x; }
      return *this;
   }

   self& operator/=( const double x )
   {
      iterator i = begin(), i_end = end();
      for ( ; i != i_end; ++i ) 
      { *i /= x; }
      return *this;
   }

  bool operator==( self const& other) const  {  return std::equal( begin(), end(), other.begin() );  }
  bool operator< ( self const& other) const  
  {  
    if (coordinates[0] < other[0])
      {
        return (true);
      }
    if (dimension == 1) return false;
    else if (coordinates[0] == other[0])
      {
        if (coordinates[1] < other[1])
	  {
            return (true);
	  }
	if (dimension == 2) return false;
        else if (coordinates[1] == other[1])
	  {
            if (coordinates[2] < other[2])
	      {
                return (true);
	      }
	  }
      }
    return (false);
  }

   NumericType& operator[](int i) 
   { 
      return coordinates[i];
   }

   // alternative declaration
  //    compatible with gsse_matrix
  //  !! NOTE !! .. the second index is omitted
  //
   NumericType& operator()(int i, int j) 
   { 
      return coordinates[i];
   }


   const NumericType& operator[](int i) const
   { 
      return coordinates[i];
   }

   typename coordinates_t::iterator begin()
   {
      return coordinates.begin();
   }

   typename coordinates_t::iterator end()
   {
      return coordinates.end();
   }

   typename coordinates_t::const_iterator begin() const
   {
      return coordinates.begin();
   }

   typename coordinates_t::const_iterator end() const
   {
      return coordinates.end();
   }

   double len()
   {
      double length(0.);
      for (unsigned long i=0; i < N;i++)
      {
         length += coordinates[i] * coordinates[i];
      }
      
      return std::sqrt(length);
   }

   double square_len()
   {
      double length(0.);
      for (unsigned long i=0; i < N;i++)
      {
         length += coordinates[i] * coordinates[i];
      }
      
      return (length);
   }


  void normalize()
  {
    double length = len();
    //std::cout << "len: " << len() << std::endl;
    if (  length != 0.0)
      {
      for (unsigned long i=0; i < N;i++)
	{
	  //std::cout <<"##"<<  coordinates[i] << std::endl;
	coordinates[i] /= length;
	//std::cout << "#" << coordinates[i] << std::endl;
      }
      }
    
  }

  double in(const self& other) const
  { 
    double ip = std::inner_product(begin(),end(), other.begin(),0.0);
    return ip;
  }



  self ex(const self& other) const
  {       
    // only defined for 3 dimensions
    //
    BOOST_STATIC_ASSERT(N == 3);   
    self ex_point;
    ex_point = self(operator[](1) * other[2] - operator[](2) * other[1],
  		    operator[](2) * other[0] - operator[](0) * other[2],
  		    operator[](0) * other[1] - operator[](1) * other[0]);
    return ex_point;
  }

//   bool& valid() 
//   {
//     return  local_valid;
//   }


   friend std::ostream& operator<<(std::ostream& ostr, const generic_point<NumericType,N>& gp) 
   {
      if (gp.coordinates.size() == 0)
         return ostr;

      for (unsigned long i=0; i < N;i++)
      {
         if (i == N-1)
            ostr << gp.coordinates[i];
         else
            ostr << gp.coordinates[i] << "  ";

      }
      return ostr;
   }

};

template<typename MetricalSpace>
typename MetricalSpace::value_type length(MetricalSpace metrical_object)
{
   typedef typename MetricalSpace::value_type  numerical_type;
   typedef typename MetricalSpace::iterator    it_t;

   numerical_type length(0);
  
   for (it_t it = metrical_object.begin();
             it != metrical_object.end(); 
             ++it)
   {
      length += (*it) * (*it);
   }
   return std::sqrt(length);
}

template<typename MetricalSpace>
typename MetricalSpace::value_type square_length(MetricalSpace metrical_object)
{
   typedef typename MetricalSpace::value_type  numerical_type;
   typedef typename MetricalSpace::iterator    it_t;

   numerical_type length(0);
  
   for (it_t it = metrical_object.begin();
             it != metrical_object.end(); 
             ++it)
   {
      length += (*it) * (*it);
   }
   return (length);
}


template<typename MetricalSpace>
void normalize(MetricalSpace& metrical_object)
{
   typedef typename MetricalSpace::value_type  numerical_type;
   typedef typename MetricalSpace::iterator    it_t;

   numerical_type local_len = length(metrical_object);
      
   //std::cout << "len: " << len() << std::endl;
   if (  local_len != 0.0)
   {
      for (long i=0; i < metrical_object.size();i++)
	{
	metrical_object[i] /= local_len;
        }
   }
}




template<typename MetricalSpace>
MetricalSpace ex(const MetricalSpace& metrical_object1, 
                 const MetricalSpace& metrical_object2 ) 
{
   MetricalSpace ex_point;
   ex_point = MetricalSpace(metrical_object1[1] * metrical_object2[2] - metrical_object1[2] * metrical_object2[1],
                            metrical_object1[2] * metrical_object2[0] - metrical_object1[0] * metrical_object2[2],
                            metrical_object1[0] * metrical_object2[1] - metrical_object1[1] * metrical_object2[0]);
   return ex_point;
}

template<typename MetricalSpace>
MetricalSpace ex2D(MetricalSpace& metrical_object1, MetricalSpace& metrical_object2 )
{
   // [RH] .. temp debug version with temp 3D point for ex product
   //
   typedef  generic_point<typename MetricalSpace::value_type, 3>  temp3dpoint_t;
   temp3dpoint_t mo1, mo2, final_p;

   mo1[0] = metrical_object1[0];
   mo1[1] = metrical_object1[1];
   mo1[2] = 0.0;
   mo2[0] = metrical_object2[0];
   mo2[1] = metrical_object2[1];
   mo2[2] = 0.0;
//    std::cout << "GSSE:: EX:: mo1: "<< mo1 << std::endl;
//    std::cout << "GSSE:: EX:: mo2: "<< mo2 << std::endl;

   final_p = temp3dpoint_t(mo1[1] * mo2[2] - mo1[2] * mo2[1],
                           mo1[2] * mo2[0] - mo1[0] * mo2[2],
                           mo1[0] * mo2[1] - mo1[1] * mo2[0]);
   
   typename MetricalSpace::value_type val1, val2, val3;
   val1 = final_p[0];
   val2 = final_p[1];
    std::cout << "GSSE:: EX::  final p: " << final_p << std::endl;
//    std::cout << "GSSE:: EX::  final p: " << MetricalSpace(val1, val2) << std::endl;
//    std::cout << std::endl;

   return MetricalSpace(val1, val2);
}


template<typename MetricalSpace>
typename MetricalSpace::value_type in(const MetricalSpace& metrical_object1, const MetricalSpace& metrical_object2 )
{
   typename MetricalSpace::value_type ip = std::inner_product(metrical_object1.begin(),
                                                              metrical_object1.end(), 
                                                              metrical_object2.begin(),0.0);
    return ip;
}


}
#endif
