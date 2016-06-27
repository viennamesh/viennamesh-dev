/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_MATRIX_HH_ID
#define GSSE_MATRIX_HH_ID

// *** BOOST includes
//
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/operators.hpp>

// *** system includes
//
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>


// *** GSSE includes
//
#include "gsse/exceptions.hpp"

namespace gsse {


///////////////////////////////////////////////////////////////////////////
/// @brief GSSEMatrix Type
///
/// Just a simple matrix wrapper for our quantity generalization 
/// We want a schema for storing an arbitrary number of datatypes within the
/// quantities.
///
///////////////////////////////////////////////////////////////////////////

template<typename NumericType, int Dimension=2>
class gsse_matrix
   : boost::addable< gsse_matrix<NumericType, Dimension> >,
     boost::subtractable< gsse_matrix<NumericType, Dimension> >,
     boost::multipliable< gsse_matrix<NumericType, Dimension>, NumericType >,
     boost::dividable< gsse_matrix<NumericType, Dimension>, NumericType >,
     boost::equality_comparable< gsse_matrix<NumericType, Dimension>  >
{
   typedef gsse_matrix<NumericType>                 self;
   typedef std::vector<std::vector<NumericType> >   matrix_t;

public:
   typedef NumericType                              numeric_t;
   typedef NumericType                              numeric_type;

   gsse_matrix() :size1(1), size2(1)
   {
      matrix = matrix_t(1, std::vector<NumericType>(1,0));
   }

  // ??? the g++ compiler needs this if boost::property_maps are integrated
  // I do not know why ??? if you know the solution, please mail me
  // -> rene@gsse.at [RH]

   gsse_matrix(const unsigned int size1, const unsigned int size2) : size1(size1), size2(size2)
   {
      matrix = matrix_t(size1, std::vector<NumericType>(size2,0));
   }

   gsse_matrix(const unsigned int size1, const unsigned int size2, const NumericType& defaultvalue ) 
      : size1(size1), size2(size2)
   {
      matrix = matrix_t(size1, std::vector<NumericType>(size2,defaultvalue));
   }

   template<typename InputIterator>
   gsse_matrix(const unsigned int size1, const unsigned int size2, InputIterator start, InputIterator end) 
      : size1(size1), size2(size2)
   {
      matrix = matrix_t(size1, std::vector<NumericType>(size2));
      for (unsigned int i=0; i < matrix.size(); i++)
      {
         for (unsigned int j=0; j < matrix[i].size(); j++)
         {
            matrix[i][j]=*start++;
         }
      }         
   }

   numeric_t& operator()(const unsigned int index1=0, const unsigned int index2=0) 
      { return matrix[index1][index2];}
   numeric_t const& operator()(const unsigned int index1=0, const unsigned int index2=0) const
      { return matrix[index1][index2];}
   
   std::vector<NumericType>& operator[](const unsigned int index1) 
      { return matrix[index1];}
   std::vector<NumericType>const& operator[](const unsigned int index1) const
      { return matrix[index1];}


   bool operator==(self const& other) const
   {
      if (matrix.size() != other.matrix.size()) {return false;}

      for (unsigned int i=0; i < matrix.size(); i++)
      {
         if (matrix[i].size() != other.matrix[i].size()) {return false;}

         for (unsigned int j=0; j < matrix[i].size(); j++)
         {
            if (matrix[i][j] != other.matrix[i][j]) {return false;}
         }
      }
      return true;
   }

   self& operator+=(self const& other) 
   {
     for (unsigned int i=0; i < matrix.size() && i < other.matrix.size(); i++)
       {
         for (unsigned int j=0; j < matrix[i].size() && j < other.matrix[i].size(); j++)
	   {
              matrix[i][j] += other.matrix[i][j];
	   }
       }

     return *this;
   }

   self& operator-=(self const& other) 
   {
     for (unsigned int i=0; i < matrix.size() && i < other.matrix.size(); i++)
       {
         for (unsigned int j=0; j < matrix[i].size() && j < other.matrix[i].size(); j++)
	   {
              matrix[i][j] -= other.matrix[i][j];
	   }
       }
     return *this;
   }
 
   self& operator*=(numeric_t other) 
   {     
      for (unsigned int i=0; i < matrix.size() ; i++)
      {
         for (unsigned int j=0; j < matrix[i].size(); j++)
         {
	   matrix[i][j] *= other;
	 }
      }
      return *this;
   }

   self& operator*=(self const& other) 
   {     
      for (unsigned int i=0; i < matrix.size() ; i++)
      {
         for (unsigned int j=0; j < matrix[i].size(); j++)
         {
               matrix[i][j] *= other.matrix[0][0];
	 }
      }
      return *this;
   }
   self& operator*(self const& other) 
   {     
      for (unsigned int i=0; i < matrix.size() ; i++)
      {
         for (unsigned int j=0; j < matrix[i].size(); j++)
         {
               matrix[i][j] *= other.matrix[0][0];
	 }
      }
      return *this;
   }
   // [RH][TODO] .. 
   // for partial specialization of a scalar
   //
   self& operator/=(self const& other) 
   {
      for (unsigned int i=0; i < matrix.size() ; i++)
      {
         for (unsigned int j=0; j < matrix[i].size(); j++)
         {
            matrix[i][j] /= other(0,0);
	 }
      }
      return *this;
   }

   self& operator/=(numeric_t other) 
   {
      for (unsigned int i=0; i < matrix.size() ; i++)
      {
         for (unsigned int j=0; j < matrix[i].size(); j++)
         {
            matrix[i][j] /= other;
	 }
      }
      return *this;
   }


public:

   friend std::ostream& operator<<(std::ostream& ostr, gsse_matrix const&  matrix)
   {
      //          ostr << "matrixtype" << std::endl;
      for (unsigned int i=0; i < matrix.matrix.size(); i++)
      {
         for (unsigned int j=0; j < matrix.matrix[i].size(); j++)
         {
            ostr << matrix.matrix[i][j] << " ";
         }
         if (matrix.matrix.size() > 1 )  
	   ostr << "  \t ";
	 //ostr << std::endl;
      }
      return ostr;
   }

   unsigned int size_1() const {return size1;}
   unsigned int size_2() const {return size2;}

private:
   matrix_t matrix;
   unsigned int size1;
   unsigned int size2;
};



/*
// for high performance double calculation .. fixed size (1,1) matrix
// [RH] 2006-02-15

template<typename NumericType>
class gsse_matrix<NumericType, 0>
{
   typedef gsse_matrix<NumericType>                 self;
   typedef NumericType                              matrix_t;

public:
   typedef NumericType                              numeric_t;

   gsse_matrix() :size1(0), size2(0)
   {
      matrix = matrix_t(0.0);
   }

   gsse_matrix(const unsigned int& size1, const unsigned int& size2) : size1(1), size2(1)
   {
      matrix = matrix_t(0.0);
   }

   gsse_matrix(const NumericType& defaultvalue )
      : size1(1), size2(1)
   {
      matrix = matrix_t(defaultvalue);
   }


   gsse_matrix(const unsigned int& size1, const unsigned int& size2, const NumericType& defaultvalue )
      : size1(1), size2(1)
   {
      matrix = matrix_t(defaultvalue);
   }

   template<typename InputIterator>
   gsse_matrix(const unsigned int& size1, const unsigned int& size2, InputIterator start, InputIterator end)
      : size1(1), size2(1)
   {
         matrix = matrix_t(0.0);
   }

   numeric_t& operator()(const unsigned int& index1=0, const unsigned int& index2=0) { return matrix;}

   bool operator==(self const& other)
   {
        return (other.matrix == matrix);
   }


   self operator*(NumericType other) const
   {
        return matrix * other;
   }


   self operator+(self other)  const
   {
        return matrix+other.matrix;
   }



   bool operator!=(self const& other) {return !(*this != other);}

public:

   friend std::ostream& operator<<(std::ostream& ostr, gsse_matrix const&  matrix)
   {
      ostr << "  " << matrix;
      return ostr;
   }

   unsigned int size_1() {return size1;}
   unsigned int size_2() {return size2;}

private:
   matrix_t matrix;
   unsigned int size1;
   unsigned int size2;

};

*/


/// ============================================================================================
/// *** the matrix traits class
///
// ############

  template <typename MatrixType>
  struct matrix_traits { };

  
//   template <typename NumericT, int Dimension>
//   struct matrix_traits<blitz::Array<NumericT, Dimension> >
//   {
//      typedef NumericT numeric_t;
//      static const int dimension = Dimension;
//   };

  template <typename NumericT, int Dimension>
  struct matrix_traits<gsse::gsse_matrix<NumericT, Dimension> >
  {
     typedef NumericT numeric_t;
     static const int dimension = Dimension;
  };

}

#endif
