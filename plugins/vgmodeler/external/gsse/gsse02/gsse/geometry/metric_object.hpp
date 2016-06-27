/* ============================================================================
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_SPACE_METRICOBJECT_HH_ID
#define GSSE_SPACE_METRICOBJECT_HH_ID 

// *** system includes

// *** BOOST includes
#include <boost/array.hpp>
#include <boost/operators.hpp>
#include <boost/static_assert.hpp>

// *** GSSE includes
#include "gsse/util/common.hpp"
#include <gsse/util/resize.hpp>

// ============================================================================================

namespace gsse
{

template<typename NumericType, long SizeN>
class metric_object 
   : public boost::array<NumericType, SizeN>,
     boost::addable< metric_object<NumericType, SizeN> >,
     boost::subtractable< metric_object<NumericType, SizeN> >,
     boost::multipliable< metric_object<NumericType, SizeN>, NumericType>,
     boost::dividable< metric_object<NumericType, SizeN>, NumericType >,
     boost::equality_comparable< metric_object<NumericType, SizeN>  >
{
   typedef metric_object<NumericType,SizeN>    self;

public:
  typedef NumericType value_type;

   typedef typename self::iterator         iterator;
   typedef typename self::const_iterator   const_iterator;

   typedef NumericType          NumericT;
   typedef tag_compiletime      type;
   typedef tag_compiletime      evaluation_type;
   typedef tag_container_array  container_t_type;   // [RH][TODO] replace and merge all tags consistently
   
   static const long dimension = SizeN;
	

   metric_object()
   {
      for (size_t ci = 0; ci < SizeN; ++ci)
      {
         (*this)[ci] = NumericType(); // [FS][INFO] changed from NumericType(0)
      }
      //BOOST_STATIC_ASSERT(SizeN == 1);
   }

   metric_object(NumericType a1)
   {
      //BOOST_STATIC_ASSERT(SizeN == 1);
   
      (*this)[0] = a1;
   }


   metric_object(NumericType a1, NumericType a2)
   {
      //BOOST_STATIC_ASSERT(SizeN == 2);
   
      (*this)[0] = a1;
      (*this)[1] = a2;
   }
      
   metric_object(NumericType a1, NumericType a2, NumericType a3)
   {
      //BOOST_STATIC_ASSERT(SizeN == 3);   
      (*this)[0] = a1;
      (*this)[1] = a2;
      (*this)[2] = a3;
   }

   metric_object(NumericType a1, NumericType a2, NumericType a3, NumericType a4)
   {
      //BOOST_STATIC_ASSERT(SizeN == 3);   
      (*this)[0] = a1;
      (*this)[1] = a2;
      (*this)[2] = a3;
      (*this)[3] = a4;
   }
   // [RH][TODO] .. extend it to MAX_ARG constructor arguments
   //

   // geometrical properties (requires metric space)
   //
   self& operator+=( const self& x )
   {
      iterator i = (*this).begin(), i_end = (*this).end();
      const_iterator j = x.begin();
      for ( ; i != i_end; ++i, ++j ) {
         *i += *j;
      }
      return *this;
   }

   self& operator+=( double x )
   {
      iterator i = (*this).begin(), i_end = (*this).end();

      for ( ; i != i_end; ++i ) 
      {
         *i += x;
      }
      return *this;
   }

   
   self& operator-=( const self& x )
   {
      iterator i = (*this).begin(), i_end = (*this).end();
      const_iterator j = x.begin();
      for ( ; i != i_end; ++i, ++j ) {
         *i -= *j;
      }
      return *this;
   }


   self& operator*=( const double x )
   {
      iterator i = (*this).begin(), i_end = (*this).end();
      for ( ; i != i_end; ++i ) 
      { *i *= x; }
      return *this;
   }

   self& operator/=( const double x )
   {
      iterator i = (*this).begin(), i_end = (*this).end();
      for ( ; i != i_end; ++i ) 
      { *i /= x; }
      return *this;
   }

   NumericType& operator()(int i) 
   { 
      return (*this)[i];
   }

   friend std::ostream& operator<<(std::ostream& ostr, const metric_object<NumericType,SizeN>& gp) 
   {
      if (gp.size() == 0)
         return ostr;

      for (unsigned long i=0; i < SizeN;i++)
      {
         if (i == SizeN-1)
            ostr << gp[i];
         else
            ostr << gp[i] << "  ";

      }
      return ostr;
   }
};


template< typename NumericType = double>
struct metric_object_rt 
{
   // -------------------------------------------------
   typedef std::vector< NumericType >              Container;
   // -------------------------------------------------
   // this runtime metric object models the 
   // gsse::metric-space concept
   //
   typedef gsse::metric_object_rt< NumericType >    self;
   typedef typename Container::value_type          value_type;
   typedef typename Container::iterator            iterator;
   typedef typename Container::const_iterator      const_iterator;
   typedef NumericType                             NumericT;
   typedef gsse::tag_runtime                        type;
   typedef gsse::tag_runtime                        evaluation_type;
   typedef gsse::tag_container_array                container_t_type;   
   typedef unsigned long                           index_type;
   // -------------------------------------------------
   //
   // CONSTRUCTORS 
   //
   metric_object_rt()   {}
   
   metric_object_rt(int){}
   
   metric_object_rt(NumericType a1)    
   {
      cont.push_back( a1 );
   }         
   metric_object_rt(NumericType a1, NumericType a2)    
   {
      cont.push_back( a1 );
      cont.push_back( a2 );
   }      
   metric_object_rt(NumericType a1, NumericType a2, NumericType a3)    
   {
      cont.push_back( a1 );
      cont.push_back( a2 );
      cont.push_back( a3 );
   }   
   metric_object_rt(NumericType a1, NumericType a2, NumericType a3, NumericType a4)    
   {
      cont.push_back( a1 );
      cont.push_back( a2 );
      cont.push_back( a3 );
      cont.push_back( a4 );
   }
   // -------------------------------------------------
   //
   // ACCESS 
   //
   NumericType const& operator()(index_type i) const
   { 
      return cont[i];
   }      
   NumericType& operator()(index_type i) 
   { 
      return cont[i];
   }   
   NumericType const& operator[](index_type i) const
   { 
      return cont[i];
   }        
   NumericType& operator[](index_type i) 
   { 
      return cont[i];
   }      
   // -------------------------------------------------
   const_iterator begin() const
   {
      return cont.begin();
   }
   iterator begin()
   {
      return cont.begin();
   }
   const_iterator end() const
   {
      return cont.end();
   }
   iterator end()
   {
      return cont.end();
   }
   index_type size() const
   {
      return cont.size();
   }
   index_type size()
   {
      return cont.size();
   }
   void resize(index_type newsize)
   {
      cont.resize(newsize);
   }   
   // -------------------------------------------------
   //
   // OPERATIONS
   //   
   self 
   operator+( const self& other ) const
   {
      self temp;
      gsse::resize( gsse::size(other) )(temp);
      const_iterator i = cont.begin(), i_end = cont.end();      
      const_iterator j = other.begin();
      iterator k       = temp.begin();
      for ( ; i != i_end; ++i, ++j, ++k ) {
         *k = *i + *j;
      }
      return temp;
   }  
   
   self 
   operator+( const self& other )
   {
      self temp;
      gsse::resize( gsse::size(other) )(temp);
      const_iterator i = cont.begin(), i_end = cont.end();      
      const_iterator j = other.begin();
      iterator k       = temp.begin();
      for ( ; i != i_end; ++i, ++j, ++k ) {
         *k = *i + *j;
      }
      return temp;
   }  

   self 
   operator-( const self& other ) const
   {
      self temp;
      gsse::resize( gsse::size(other) )(temp);
      const_iterator i = cont.begin(), i_end = cont.end();      
      const_iterator j = other.begin();
      iterator k       = temp.begin();
      for ( ; i != i_end; ++i, ++j, ++k ) {
         *k = *i - *j;
      }
      return temp;
   }   
   
   self 
   operator-( const self& other ) 
   {
      self temp;
      gsse::resize( gsse::size(other) )(temp);
      const_iterator i = cont.begin(), i_end = cont.end();      
      const_iterator j = other.begin();
      iterator k       = temp.begin();
      for ( ; i != i_end; ++i, ++j, ++k ) {
         *k = *i - *j;
      }
      return temp;
   }      

   self 
   operator*( const self& other ) const
   {
      self temp;
      gsse::resize( gsse::size(other) )(temp);
      const_iterator i = cont.begin(), i_end = cont.end();      
      const_iterator j = other.begin();
      iterator k       = temp.begin();
      for ( ; i != i_end; ++i, ++j, ++k ) {
         *k = *i * *j;
      }
      return temp;
   }  

   self 
   operator*( const self& other )
   {
      self temp;
      gsse::resize( gsse::size(other) )(temp);
      const_iterator i = cont.begin(), i_end = cont.end();      
      const_iterator j = other.begin();
      iterator k       = temp.begin();
      for ( ; i != i_end; ++i, ++j, ++k ) {
         *k = *i * *j;
      }
      return temp;
   }  

   self 
   operator/( const self& other ) const
   {
      self temp;
      gsse::resize( gsse::size(other) )(temp);
      const_iterator i = cont.begin(), i_end = cont.end();      
      const_iterator j = other.begin();
      iterator k       = temp.begin();
      for ( ; i != i_end; ++i, ++j, ++k ) {
         *k = *i / *j;
      }
      return temp;
   }  

   self 
   operator/( const self& other )
   {
      self temp;
      gsse::resize( gsse::size(other) )(temp);
      const_iterator i = cont.begin(), i_end = cont.end();      
      const_iterator j = other.begin();
      iterator k       = temp.begin();
      for ( ; i != i_end; ++i, ++j, ++k ) {
         *k = *i / *j;
      }
      return temp;
   }  
   
   self& operator+=( const self& x )
   {
      iterator i = cont.begin(), i_end = cont.end();
      const_iterator j = x.begin();
      for ( ; i != i_end; ++i, ++j ) {
         *i += *j;
      }
      return *this;
   }
  
   self& operator-=( const self& x )
   {
      iterator i = cont.begin(), i_end = cont.end();
      const_iterator j = x.begin();
      for ( ; i != i_end; ++i, ++j ) {
         *i -= *j;
      }
      return *this;
   }

   self& operator*=( const self& x )
   {
      iterator i = cont.begin(), i_end = cont.end();
      for ( ; i != i_end; ++i ) 
      { *i *= x; }
      return *this;
   }

   self& operator/=( const self& x )
   {
      iterator i = cont.begin(), i_end = cont.end();
      for ( ; i != i_end; ++i ) 
      { *i /= x; }
      return *this;
   }   
   // -------------------------------------------------
   friend std::ostream& operator<<(std::ostream& ostr, const self& gp) 
   {
      long gp_size = gsse::size(gp);
   
      if (gp_size == 0)
         return ostr;

      for (long i=0; i < gp_size;i++)
      {
         if (i == gp_size-1)
            ostr << gp[i];
         else
            ostr << gp[i] << "  ";
      }
      return ostr;
   }
   
   // -------------------------------------------------   
   Container   cont;
   // -------------------------------------------------
};



//
// meet the gsse::concept requirements for
// tag dispatching ..
//
namespace traits {
template<typename T>
struct tag_of< gsse::metric_object_rt<T> >
{
   typedef gsse::tag_runtime     type;
   typedef gsse::tag_runtime     evaluation_type;
   typedef gsse::gsse_container  container_type;
};
}


typedef gsse::metric_object_rt<double> vector_rt;


}  //namespace gsse
#endif
