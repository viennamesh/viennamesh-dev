/* ============================================================================
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_MATRIX_HH_ID
#define GSSE_MATRIX_HH_ID

// *** system includes
#include <iostream>
#include <string>
#include <vector>
// *** BOOST includes
#include <boost/operators.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/remove_pointer.hpp>
// *** GSSE includes
#include "gsse/geometry/metric_operations.hpp"

namespace gsse {


template<typename NumericType=double, int Dimension=2>
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
   typedef NumericType NumericT;

   gsse_matrix() :sz_row(1), sz_clm(1)
   {
      matrix = matrix_t(1, std::vector<NumericType>(1,0));
   }

   gsse_matrix(const unsigned int sz_row) : sz_row(sz_row), sz_clm(1)
   {
      matrix = matrix_t(sz_row, std::vector<NumericType>(sz_clm,0));
   }

   gsse_matrix(const unsigned int sz_row, const unsigned int sz_clm) : sz_row(sz_row), sz_clm(sz_clm)
   {
      matrix = matrix_t(sz_row, std::vector<NumericType>(sz_clm,0));
   }

   gsse_matrix(const unsigned int sz_row, const unsigned int sz_clm, const NumericType& defaultvalue ) 
      : sz_row(sz_row), sz_clm(sz_clm)
   {
      matrix = matrix_t(sz_row, std::vector<NumericType>(sz_clm,defaultvalue));
   }

   template<typename InputIterator>
   gsse_matrix(const unsigned int sz_row, const unsigned int sz_clm, InputIterator start, InputIterator end) 
      : sz_row(sz_row), sz_clm(sz_clm)
   {
      matrix = matrix_t(sz_row, std::vector<NumericType>(sz_clm));
      for (unsigned int i=0; i < matrix.size(); i++)
      {
         for (unsigned int j=0; j < matrix[i].size(); j++)
         {
            matrix[i][j]=*start++;
         }
      }         
   }

   numeric_t& operator()(const unsigned int row=0, const unsigned int column=0) 
      { return matrix[row][column];}
   numeric_t const& operator()(const unsigned int index1=0, const unsigned int index2=0) const
      { return matrix[index1][index2];}
   
   std::vector<NumericType>& operator[](const unsigned int index1) 
      { return matrix[index1];}
   std::vector<NumericType>const& operator[](const unsigned int index1) const
      { return matrix[index1];}


//    template<typename OtherMatrix>
//    void operator=(OtherMatrix const& other) 
//    {
//       matrix = matrix_t(other.sz_row, std::vector<NumericType>(other.sz_clm,0));
//       for (unsigned int i=0; i < other.size_1(); i++)
//       {
//          for (unsigned int j=0; j < other.size_2(); j++)
//          {
//             matrix[i][j]=other[i][j];
//          }
//       }         
//    }

   bool operator==(self const& other) const
   {
      if (size_1() != other.size_1()) 
      {
         std::cout << "size 1 does not match!! " << std::endl;
         return false;
      }

      for (unsigned int i=0; i < matrix.size(); i++)
      {
         if (size_2() != other.size_2()) 
         {
            std::cout << "size 2 does not match!! " << std::endl;
            return false;
         }

         for (unsigned int j=0; j < matrix[i].size(); j++)
         {
// [RH][info]
// exact::
//            if (matrix[i][j] != other.matrix[i][j]) {return false;}
            double eps = 1e-10;
            if (fabs(matrix[i][j] - other[i][j]) > eps) 
            {
               std::cout << " ## diff: " << matrix[i][j] - other[i][j] << std::endl;
               return false;
            }

         }
      }
      return true;
   }

   self& operator+=(self const& other) 
   {
     for (unsigned int i=0; i < size_1() && i < other.size_1(); i++)
     {
        for (unsigned int j=0; j < size_2() && j < other.size_2(); j++)
        {
           matrix[i][j] += other[i][j];
        }
     }
     
     return *this;
   }

   self& operator-=(self const& other) 
   {
     for (unsigned int i=0; i < size_1() && i < other.size_1(); i++)
     {
        for (unsigned int j=0; j < size_2 && j < other.size_2(); j++)
        {
           matrix[i][j] -= other[i][j];
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

   template<typename OtherMatrix>
   self operator*(const OtherMatrix& other)  const
   {     
      self new_matrix(sz_row, other.size_2());
      
      for (unsigned int i=0; i < size_1() ; i++)
      {
         for (unsigned int j=0; j < other.size_2(); j++)
         {
            
            for (unsigned int k=0; k < size_2(); k++)
            {
               new_matrix[i][j] += matrix[i][k] * other[k][j];
            }
 	 }
      }
      return new_matrix;
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
      ostr <<  std::endl;
      for (unsigned int i=0; i < matrix.size_1(); i++)
      {
         for (unsigned int j=0; j < matrix.size_2(); j++)
         {
            ostr << matrix[i][j] << "\t";
         }
         if (matrix.size_1() > 1 )  
            //ostr << "  \t ";
            ostr << std::endl;
      }
      return ostr;
   }

   unsigned int size_1() const {return sz_row;}
   unsigned int size_2() const {return sz_clm;}

   unsigned int size_r() const {return sz_row;}
   unsigned int size_c() const {return sz_clm;}

private:
   matrix_t matrix;
   unsigned int sz_row;
   unsigned int sz_clm;
};




template<typename NumericType, int Dimension1, int Dimension2>
class gsse_matrix_ct
   : boost::addable<             gsse_matrix_ct<NumericType, Dimension1, Dimension2> >,
     boost::subtractable<        gsse_matrix_ct<NumericType, Dimension1, Dimension2> >,
     boost::multipliable<        gsse_matrix_ct<NumericType, Dimension1, Dimension2>, NumericType >,
     boost::dividable<           gsse_matrix_ct<NumericType, Dimension1, Dimension2>, NumericType >,
     boost::equality_comparable< gsse_matrix_ct<NumericType, Dimension1, Dimension2>  >
{
   typedef gsse_matrix_ct<NumericType, Dimension1, Dimension2>                self;
   typedef boost::array<NumericType,  Dimension2>   MatrixInside;
   typedef boost::array<MatrixInside, Dimension1 >  matrix_t;

public:
   typedef NumericType                              numeric_t;
   typedef NumericType                              numeric_type;

   gsse_matrix_ct()
   {}

   gsse_matrix_ct(const unsigned int sz_row, const unsigned int sz_clm)
   {
      // [RH][TODO] check if possible
   }

   gsse_matrix_ct(const unsigned int sz_row, const unsigned int sz_clm, const NumericType& defaultvalue ) 
   {
      // [RH][TODO] check if possible
      for (unsigned int i=0; i < matrix.size(); i++)
      {
         for (unsigned int j=0; j < matrix[i].size(); j++)
         {
            matrix[i][j]= defaultvalue;
         }
      }         
   }

   template<typename InputIterator>
   gsse_matrix_ct(const unsigned int sz_row, const unsigned int sz_clm, InputIterator start, InputIterator end) 
   {
      // [RH][TODO] check if possible
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
   
   MatrixInside& operator[](const unsigned int index1) 
      { return matrix[index1];}
   MatrixInside const& operator[](const unsigned int index1) const
      { return matrix[index1];}


   template<typename OtherMatrix>
   //   self operator*(const OtherMatrix& other)  const
   void operator=(OtherMatrix const& other) 
   {
      // [RH][TODO]
      // check sizes

//      matrix = matrix_t(other.sz_row, std::vector<NumericType>(other.sz_clm,0));
      for (unsigned int i=0; i < matrix.size(); i++)
      {
         for (unsigned int j=0; j < matrix[i].size(); j++)
         {
            matrix[i][j]=other[i][j];
         }
      }         
   
   }

   template<typename OtherMatrix>
   bool operator==(OtherMatrix const& other) const
   {
      if (size_1() != other.size_1()) 
      {
         std::cout << "size 1 does not match!! " << std::endl;
         return false;
      }

      for (unsigned int i=0; i < matrix.size(); i++)
      {
         if (size_2() != other.size_2()) 
         {
            std::cout << "size 2 does not match!! " << std::endl;
            return false;
         }

         for (unsigned int j=0; j < size_2(); j++)
         {
// [RH][info]
// exact::
//            if (matrix[i][j] != other.matrix[i][j]) {return false;}
            double eps = 1e-10;
            if (fabs(matrix[i][j] - other[i][j]) > eps) 
            {
               std::cout << " ## diff: " << matrix[i][j] - other[i][j] << std::endl;
               return false;
            }

         }
      }
      return true;
   }


   self& operator+=(self const& other) 
   {
     for (unsigned int i=0; i < size_1() && i < other.size_1(); i++)
     {
        for (unsigned int j=0; j < size_2 && j < other.size_2(); j++)
        {
           matrix[i][j] += other[i][j];
        }
     }
     
     return *this;
   }

   self& operator-=(self const& other) 
   {
     for (unsigned int i=0; i < size_1() && i < other.size_1(); i++)
     {
        for (unsigned int j=0; j < size_2 && j < other.size_2(); j++)
        {
           matrix[i][j] -= other[i][j];
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

   template<typename OtherMatrix>
   self operator*(const OtherMatrix& other)  const
   {     
      self new_matrix;
      
      for (unsigned int i=0; i < size_1() ; i++)
      {
         for (unsigned int j=0; j < other.size_2(); j++)
         {
            
            for (unsigned int k=0; k < size_2(); k++)
            {
               new_matrix[i][j] += matrix[i][k] * other[k][j];
            }
 	 }
      }
      return new_matrix;
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

   friend std::ostream& operator<<(std::ostream& ostr, self const&  matrix)
   {
      ostr <<  std::endl;
      for (unsigned int i=0; i < matrix.size_1(); i++)
      {
         for (unsigned int j=0; j < matrix.size_2(); j++)
         {
            ostr << matrix[i][j] << "\t";
         }
         if (matrix.size_1() > 1 )  
            //ostr << "  \t ";
            ostr << std::endl;
      }
      return ostr;
   }

   unsigned int size_1() const {return Dimension1;}
   unsigned int size_2() const {return Dimension2;}

private:
   matrix_t matrix;
};


template< typename Matrix >
struct MatrixWrapper
{
   typedef typename Matrix::numeric_type  matrix_element;

   MatrixWrapper( Matrix & matrix ) : _matrix(matrix) {}
   
   inline 
   typename boost::remove_pointer< matrix_element >::type 
   operator()(int index1, int index2) const
   {
      return access( index1, index2, boost::is_pointer<matrix_element>() );
   }   
   inline 
   typename boost::remove_pointer< matrix_element >::type 
   access(int index1, int index2, boost::true_type const&)  const
   {
      return *_matrix(index1, index2);
   }
   inline 
   typename boost::remove_pointer< matrix_element >::type 
   access(int index1, int index2, boost::false_type const&)  const
   {
      return _matrix(index1, index2);   
   }   

   unsigned int row_size()
   {
      return _matrix.size_1();  // [JW]TODO remove .. gsse::matrix dependency .. !
   }   
   unsigned int col_size()
   {
      return _matrix.size_2();
   }
   
   Matrix & _matrix;
};



// ###########################################
// free functions
//



long
num_rows(const gsse_matrix<double,2>& mx)
{
    return mx.size_1();
}

long
num_cols(const gsse_matrix<double,2>& mx)
{
    return mx.size_2();
}

void 
set_to_zero(const gsse_matrix<double,2>& mx)
{
   // do nothing..are already set to zero
}


template <typename NumericT, long DIM>
long
inline num_rows(const gsse::metric_object<NumericT,DIM>& mx)
{
   return mx.size();
}

template <typename T1>
long
inline num_rows(const std::vector<T1>& vec)
{
   return vec.size();
}


template <typename NumericT, long DIM>
long
inline size(const gsse::metric_object<NumericT,DIM>& vec)
{
   if (gsse::num_cols(vec) != 1)
   {
      //[TODO] .. warning..is not really a vector
      std::cout << " vector size:: is not really a vector.. cols size: "<< gsse::num_cols(vec) << std::endl;
   }
   return gsse::num_rows(vec);
}



// ################################## vectors




}  //namespace gsse

#endif
