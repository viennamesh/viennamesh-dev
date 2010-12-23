/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_DATASTRUCTURE_METRICOBJECTRT
#define GML_DATASTRUCTURE_METRICOBJECTRT
//
// ===================================================================================
//
// *** SYSTEM include
//
#include <vector>
//
// *** GML include
//
#include "../util/tags.hpp"
//
// *** GSSE include
//
#include <gsse/util/resize.hpp>
//
// ===================================================================================
//

namespace gml
{

template< typename NumericType >
struct metric_object_rt 
{
   // -------------------------------------------------
   typedef std::vector< NumericType >              Container;
   // -------------------------------------------------
   // this runtime metric object models the 
   // gsse::metric-space concept
   //
   typedef gml::metric_object_rt< NumericType >    self;
   typedef typename Container::value_type          value_type;
   typedef typename Container::iterator            iterator;
   typedef typename Container::const_iterator      const_iterator;
   typedef NumericType                             NumericT;
   typedef gml::tag_runtime                        type;
   typedef gml::tag_runtime                        evaluation_type;
   typedef gml::tag_container_array                container_t_type;   
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




} // end namespace: gml


//
// meet the gsse::concept requirements for
// tag dispatching ..
//
namespace gsse { namespace traits {
template<typename T>
struct tag_of< gml::metric_object_rt<T> >
{
   typedef gsse::tag_runtime     type;
   typedef gsse::tag_runtime     evaluation_type;
   typedef gsse::gsse_container  container_type;
};
}}

















#endif
