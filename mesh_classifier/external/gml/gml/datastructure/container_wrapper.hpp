/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GML_DATASTRUCTURE_CONTAINERWRAPPER
#define GML_DATASTRUCTURE_CONTAINERWRAPPER
//
// ===================================================================================
//
// *** GML include
//
#include "../util/common.hpp"
//#include "../util/traits.hpp"
//
// *** BOOST include
//
# include <boost/iterator/iterator_facade.hpp>
//
// ===================================================================================
//
namespace gml
{

namespace detail
{
//
// ===================================================================================
//
// CONTAINER WRAPPER - DEFAULT 
//
// ===================================================================================
//
template < typename Container >
struct container_wrapper_impl 
{  
   typedef container_wrapper_impl< Container >         self;

   typedef typename Container::iterator      iterator;
   typedef typename Container::value_type    value_type;
   typedef gml::tag_runtime                  type;
   typedef gml::tag_runtime                  evaluation_type;
   typedef gsse::gsse_container              container_type;

   container_wrapper_impl( Container& cont ) : _cont( cont ) { }

   typename gsse::result_of::val< Container >::type &
   operator[] ( long index )
   {
      return _cont[ index ];   
   }
   typename gsse::result_of::val< Container >::type &
   operator[] ( long index ) const
   {
      return _cont[ index ];   
   }   
   inline typename gsse::result_of::val< Container >::type &
   operator() ( long index )
   {
      return (*this)[index];
   }   
   inline typename gsse::result_of::val< Container >::type &
   operator() ( long index ) const
   {
      return (*this)[index];
   }      
   typename Container::iterator
   begin()
   {
      return _cont.begin();
   }
   typename Container::iterator
   begin() const
   {
      return _cont.begin();
   }
   typename Container::iterator
   end()
   {
      return _cont.end();
   }
   typename Container::iterator
   end() const
   {
      return _cont.end();
   }   
   long size()
   {
      return _cont.size();
   }   
   long size() const
   {
      return _cont.size();
   }

   self 
   operator-( self const& other ) 
   {
      Container temp;
      gsse::resize( gsse::size(other) )(temp);
      iterator i = (*this).begin(), i_end = (*this).end();      
      iterator j = other.begin();
      iterator k = temp.begin();
      for ( ; i != i_end; ++i, ++j, ++k ) {
         *k = *i - *j;
      }
      return temp;
   }      

   self 
   operator-( self const& other ) const
   {
      Container temp;
      gsse::resize( gsse::size(other) )(temp);
      iterator i = (*this).begin(), i_end = (*this).end();      
      iterator j = other.begin();
      iterator k = temp.begin();
      for ( ; i != i_end; ++i, ++j, ++k ) {
         *k = *i - *j;
      }
      return temp;
   }    

   friend std::ostream& operator<<(std::ostream& ostr, self const& gp)
   {
      long gp_size = gsse::size(gp);

      if (gp_size == 0) return ostr;

      for (long i=0; i < gp_size;i++)
      {
         if (i == gp_size-1)
            ostr << gp[i];
         else
            ostr << gp[i] << "  ";
      }

      return ostr;
   }
   Container & _cont;   
};
//
// ===================================================================================
//
// CONTAINER WRAPPER - STD::MAP 
//
// ===================================================================================
//
template < typename T0, typename T1 >
struct container_wrapper_impl < std::map< T0, T1 > >
{ 
   typedef std::map< T0, T1 >                         Container;
   typedef container_wrapper_impl< Container >        self;
   
   container_wrapper_impl( Container& cont ) : _cont( cont ) { }   
   
   class Iterator
     : public boost::iterator_facade<
           Iterator
         , T1
         , boost::forward_traversal_tag
       >
   {
   public: 
      Iterator() : _pnt(0) { }
      explicit Iterator( typename Container::iterator pnt ) : _pnt( pnt ) { }
   private:
      friend class boost::iterator_core_access;
      
      void increment() { _pnt++; }
      
      void advance( long n ) { std::advance( _pnt, n ); }
      
      bool equal( Iterator const& other ) const
      {
         return this->_pnt == other._pnt;
      }

      T1& dereference() const { return _pnt->second; }

      typename Container::iterator _pnt;
   };
   
   typedef Iterator                 iterator;
   typedef T1                       value_type;
   typedef gml::tag_runtime         type;
   typedef gml::tag_runtime         evaluation_type;
   typedef gsse::gsse_container     container_type;   

   T1& operator[] ( long index )
   {
      return _cont[ index ];   
   }
   T1& operator[] ( long index ) const
   {
      return _cont[ index ];   
   }   
   T1& operator() ( long index )
   {
      return (*this)[index];
   }   
   T1& operator() ( long index ) const
   {
      return (*this)[index];
   }    

   iterator begin()
   {
      return iterator( _cont.begin() );
   }   

   iterator begin() const
   {
      return iterator( _cont.begin() );
   }      

   iterator end()
   {
      return iterator( _cont.end() );
   }      
   
   iterator end() const
   {
      return iterator( _cont.end() );
   }       

   long size()
   {
      return _cont.size();
   }   
   long size() const
   {
      return _cont.size();
   }

   self 
   operator-( self const& other ) 
   {
      Container temp;
      typename Container::iterator i = _cont.begin(), i_end = _cont.end();      
      iterator j = other.begin();
      for ( ; i != i_end; ++i, ++j ) {
         temp[i->first] = (i->second) - *j;
      }
      return temp;
   }      


   friend std::ostream& operator<<(std::ostream& ostr, self const& other)
   {
      if (gsse::size(other) == 0) return ostr;

      for( iterator iter = other.begin(); iter != other.end(); iter++ )
      {
         if ( iter == (other.end()-1) )
            ostr << *iter;
         else
            ostr << *iter << "  ";         
      }
      return ostr;
   }

   Container & _cont;      
};
//
// ===================================================================================
//
// CONTAINER WRAPPER - FUSION::VECTOR
//
// ===================================================================================
//
template < typename T0, typename T1 >
struct container_wrapper_impl < boost::fusion::vector< T0, T1 > >
{ 
   typedef boost::fusion::vector< T0, T1 >                  Container;
   typedef boost::fusion::vector_iterator< Container, 0 >   ContainerIterator;
   typedef container_wrapper_impl< Container >              self;
  
   container_wrapper_impl( Container& cont ) : _cont( cont ) { }   

   class Iterator
     : public boost::iterator_facade<
           Iterator
         , T0
         , boost::forward_traversal_tag
       >
   {
   public: 
      Iterator() : _wrapper(Container(0,0)), _index(0) { }
      explicit Iterator( self& wrapper, unsigned int index ) 
         : _wrapper(wrapper), _index( index ) { }
   private:
      friend class boost::iterator_core_access;
      
      void increment() 
      { 
         _index++;
      }
      
      void advance( long n ) 
      { 
         _index += n;
      }
      
      bool equal( Iterator const& other ) const
      {
         return this->_index == other._index;
      }

      T0& dereference() const 
      { 
         return _wrapper[_index];
      }

      self&          _wrapper;
      unsigned int   _index;
      
   };

   typedef Iterator                 iterator;
   typedef T0                       value_type;
   typedef gml::tag_runtime         type;
   typedef gml::tag_runtime         evaluation_type;
   typedef gsse::gsse_container     container_type;      
   
   T0& operator[] ( long index )
   {
      switch( index )
      {
         case 0: return boost::fusion::at_c< 0 >( _cont );
         case 1: return boost::fusion::at_c< 1 >( _cont );
      }
      std::cout << "ERROR::accessing fusion::sequence at index: " << 
         index << " is not possible .. shutting down." << std::endl;
      exit(-1);
      // stop compiler from complaining ..
      return boost::fusion::at_c< 0 >( _cont );      
   }      
   
   iterator begin()
   {
      return iterator( *this, 0 );
   }

   iterator begin() const 
   {
      return iterator( *this, 0 );
   }

   iterator end()
   {
      return iterator( *this, 2 );
   }

   iterator end() const 
   {
      return iterator( *this, 2 );
   }
   
/*
   iterator begin()
   {
      return iterator( boost::fusion::begin( _cont ) );
   }   
   
   iterator begin() const
   {
      return iterator( boost::fusion::begin( _cont ) );
   }      

   iterator end()
   {
      return iterator( boost::fusion::end( _cont ) );
   }   

   iterator end() const
   {
      return iterator( boost::fusion::end( _cont ) );
   }     

   long size()
   {
      return _cont.size();
   }   
   long size() const
   {
      return _cont.size();
   }
*/
   Container & _cont;         
};
/*
template < typename T0, typename T1 >
struct wrapper_impl < boost::fusion::vector< T0, T1 > >
{  

   typedef boost::fusion::vector< T0, T1 >      Container;
   typedef sequence_iterator< Container >       SequIter;
   typedef long value_type;
   
   typedef gml::tag_runtime                  type;
   typedef gml::tag_runtime                  evaluation_type;
   typedef gsse::gsse_container              container_type;   

   wrapper_impl( Container& cont ) : _cont( cont ), _iter( SequIter( cont ) ), _error_value(-1)
   { }
   
*/

   
   /*
   long &
   operator[] ( long index )
   {
      if( index == 0 ) return boost::fusion::at_c< 0 >( _cont );   
      else
      if( index == 1 ) return boost::fusion::at_c< 1 >( _cont );   
      std::cout << "ERROR::WRAPPER: accessing element out of range .. " << std::endl;
      return _error_value;
   }
   
   inline long &
   operator() ( long index )
   {
      return (*this)[index];
   }

   Container & _cont;
   SequIter   _iter;
   long _error_value;
  
};
   */ 

} // end namespace: detail

template < typename Container >
struct container_wrapper
{
   typedef gml::detail::container_wrapper_impl< Container > type;
};

} // end namespace: gml
#endif
