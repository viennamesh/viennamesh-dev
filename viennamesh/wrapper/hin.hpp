/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_WRAPPER_HIN_HPP
#define VIENNAMESH_WRAPPER_HIN_HPP

#include "viennamesh/wrapper/wrapper_base.hpp"
#include "viennamesh/tags.hpp"
#include "viennautils/value_type.hpp"

namespace viennamesh {

namespace tag {
struct hin {};
} // end namespace tag   

template<typename ArrayT>
struct hin_point_wrapper
{
   // -------------------------------------------------------------------------------------------
   typedef hin_point_wrapper<ArrayT>            self_type;
   typedef typename ArrayT::value_type          result_type;
   // -------------------------------------------------------------------------------------------   
   hin_point_wrapper(ArrayT& array) : array_(array) {}
   // -------------------------------------------------------------------------------------------   
   result_type& operator[](size_t i) const
   { 
      return array_[i];      
   }      
   result_type& operator[](size_t i) 
   { 
      return array_[i];      
   }      
   // -------------------------------------------------------------------------------------------
   size_t size() const
   {
      return array_.size();
   }
   size_t size() 
   {
      return array_.size();
   }
   // -------------------------------------------------------------------------------------------
   friend std::ostream& operator<<(std::ostream& ostr, self_type const& val) 
   {
      size_t size = val.size();
      if (size == 0) return ostr;
      for (size_t i = 0; i < size; i++)
      {
         if (i == size-1) ostr << val[i];
         else             ostr << val[i] << "  ";
      }
      return ostr;
   }      
   // -------------------------------------------------------------------------------------------   
   ArrayT & array_;
};

template<typename DomainT>
struct hin_cell_complex_wrapper
{
   typedef typename DomainT::polygon_type                           cell_type;
   
   hin_cell_complex_wrapper(DomainT& domain, size_t segment_id) : domain_(domain), segment_id_(segment_id) {}
   
   struct cell_iterator : viennamesh::iterator_base< hin_cell_complex_wrapper<DomainT> > 
   {
      typedef  viennamesh::iterator_base< hin_cell_complex_wrapper<DomainT> > iterator_base_type;

      cell_iterator(hin_cell_complex_wrapper<DomainT>& obj)                : iterator_base_type (obj)         {};
      cell_iterator(hin_cell_complex_wrapper<DomainT>& obj, size_t newpos) : iterator_base_type (obj, newpos) {};      

      cell_type 
      operator*() const
      {
         return (*this).obj().domain().segment( (*this).obj().segment_id() )[ (*this).pos() ];
      }                   
   };
   cell_iterator cell_begin() const
   {
      return cell_iterator(*this);
   }     
   cell_iterator cell_begin()
   {
      return cell_iterator(*this);
   }  
   cell_iterator cell_end() const
   {
      return cell_iterator(*this, (*this).domain().segment( (*this).segment_id() ).size());  
   }        
   cell_iterator cell_end()
   {
      return cell_iterator(*this, (*this).domain().segment( (*this).segment_id() ).size());     
   }        
   
   inline DomainT& domain()      { return domain_;     }   
   inline size_t   segment_id()  { return segment_id_; }

private:
   DomainT& domain_;
   size_t   segment_id_;
};

template<typename Datastructure>
struct wrapper <viennamesh::tag::hin, Datastructure>
{   
   // -------------------------------------------------------------------------------------------
   // extract meta information from datastructure
   //
   typedef typename Datastructure::point_type      hin_point_type;

   // -------------------------------------------------------------------------------------------   
   // provide wrappers for the datastructures
   //
   typedef typename viennamesh::hin_point_wrapper<hin_point_type>                      point_type;
   typedef typename viennamesh::hin_cell_complex_wrapper<Datastructure>                cell_complex_wrapper_type;
   typedef typename viennamesh::hin_cell_complex_wrapper<Datastructure>::cell_iterator cell_iterator;
   typedef typename viennamesh::hin_cell_complex_wrapper<Datastructure>::cell_type     cell_type;   
   // -------------------------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------------------------   
   wrapper(Datastructure& domain) : domain_(domain) {}
   // -------------------------------------------------------------------------------------------   
   
   inline Datastructure& domain() { return domain_; }
   
   // -------------------------------------------------------------------------------------------
   struct geometry_iterator : viennamesh::iterator_base< viennamesh::wrapper< viennamesh::tag::hin, Datastructure > >
   {
      typedef viennamesh::wrapper< viennamesh::tag::hin, Datastructure >     wrapper_type;
      typedef viennamesh::iterator_base< wrapper_type >                      iterator_base_type;
      
      geometry_iterator(wrapper_type& obj)                : iterator_base_type (obj)         {};
      geometry_iterator(wrapper_type& obj, size_t newpos) : iterator_base_type (obj, newpos) {};      

      point_type
      operator*() const
      {
         return point_type( (*this).obj().domain().point( (*this).pos() ) );
      }            

   };   
   geometry_iterator geometry_begin()
   {
      return geometry_iterator(*this);
   }  
   geometry_iterator geometry_end()
   {
      return geometry_iterator(*this, domain().geometry_size());
   }   
   // -------------------------------------------------------------------------------------------

   // -------------------------------------------------------------------------------------------
   struct segment_iterator : viennamesh::iterator_base< viennamesh::wrapper< viennamesh::tag::hin, Datastructure > >
   {
      typedef viennamesh::wrapper< viennamesh::tag::hin, Datastructure >         wrapper_type;
      typedef viennamesh::iterator_base< wrapper_type >                          iterator_base_type;
      
      segment_iterator(wrapper_type& obj)                : iterator_base_type (obj)         {};
      segment_iterator(wrapper_type& obj, size_t newpos) : iterator_base_type (obj, newpos) {};      


      cell_complex_wrapper_type
      operator*() const
      {  
         return cell_complex_wrapper_type((*this).obj().domain(), (*this).pos());
      }            
   };   
   segment_iterator segment_begin()
   {
      return segment_iterator(*this);
   }  
   segment_iterator segment_end()
   {
      return segment_iterator(*this, domain().segment_size());
   }      
   std::size_t segment_size()  { return domain().segment_size(); }
   // -------------------------------------------------------------------------------------------   


   // -------------------------------------------------------------------------------------------   
   Datastructure & domain_;
   // -------------------------------------------------------------------------------------------
};
   
} // end namespace viennamesh

#endif


