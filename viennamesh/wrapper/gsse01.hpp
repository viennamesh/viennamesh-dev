/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Karl Rupp                                rupp@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_WRAPPER_GSSEO1_HPP
#define VIENNAMESH_WRAPPER_GSSEO1_HPP

#include "viennamesh/wrapper/wrapper_base.hpp"
#include "viennamesh/tags.hpp"
#include "viennautils/value_type.hpp"

namespace viennamesh {

namespace tag {
struct gsse01 {};
} // end namespace tag   

template<typename CellT, typename CellIteratorT>
struct gsse01_cell_wrapper
{
   // -------------------------------------------------------------------------------------------
   typedef gsse01_cell_wrapper<CellT, CellIteratorT>        self_type;
   typedef size_t                                           result_type;
   // -------------------------------------------------------------------------------------------   
   
   gsse01_cell_wrapper(CellT const& cell) : cell_(cell) 
   { 
   }
   // -------------------------------------------------------------------------------------------   
   result_type operator[](size_t i) const
   { 
      CellIteratorT vocit(cell_);
      std::advance(vocit, i);
      return vocit->handle();      
   }      
 
   result_type operator[](size_t i) 
   { 
      CellIteratorT vocit(cell_);
      std::advance(vocit, i);
      return vocit->handle();      
   }     
   // -------------------------------------------------------------------------------------------   
   result_type size() const
   {
      CellIteratorT vocit(cell_);
      std::size_t cnt = 0;
      while (vocit.valid())
      {
         cnt++; vocit++;
      }      
      return cnt;
   }
   result_type size() 
   {
      CellIteratorT vocit(cell_);
      std::size_t cnt = 0;
      while (vocit.valid())
      {
         cnt++;vocit++;
      }      
      return cnt;
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
   
   CellT  cell_;
  
};


template<typename DomainT>
struct gsse01_cell_complex_wrapper
{
   
   typedef typename DomainT::segment_type::cell_type                          gsse_cell_type;
   typedef typename gsse::domain_traits<DomainT>::vertex_on_cell_iterator     gsse_vertex_on_cell_iterator;
   
   typedef gsse01_cell_wrapper<gsse_cell_type, gsse_vertex_on_cell_iterator>  cell_type;   
   
   gsse01_cell_complex_wrapper(DomainT& domain, size_t segment_id) : domain_(domain), segment_id_(segment_id) {}
   
   struct cell_iterator : viennamesh::iterator_base< gsse01_cell_complex_wrapper<DomainT> > 
   {
      typedef  viennamesh::iterator_base< gsse01_cell_complex_wrapper<DomainT> > iterator_base_type;

      cell_iterator(gsse01_cell_complex_wrapper<DomainT>& obj)                : iterator_base_type (obj)         {};
      cell_iterator(gsse01_cell_complex_wrapper<DomainT>& obj, size_t newpos) : iterator_base_type (obj, newpos) {};      

      cell_type
      operator*() const
      {  
         typename gsse::domain_traits<DomainT>::cell_iterator cit = (*this).obj().domain_.segment_container[(*this).obj().segment_id_].cell_begin();
         std::advance(cit, this->pos());
         return cell_type(*cit);
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
      return cell_iterator(*this, domain_.segment_container[this->segment_id()].cell_size());
   }        
   cell_iterator cell_end()
   {
      return cell_iterator(*this, domain_.segment_container[this->segment_id()].cell_size());      
   }        
   
  
   inline size_t   segment_id()  { return segment_id_; }
   
   DomainT&  domain_;
   size_t   segment_id_;

};


template<typename Datastructure>
struct wrapper <viennamesh::tag::gsse01, Datastructure>
{ 
   // -------------------------------------------------------------------------------------------   
   // provide wrappers for the datastructures
   //
   typedef typename viennamesh::gsse01_cell_complex_wrapper<Datastructure>          cell_complex_wrapper_type;
   typedef typename cell_complex_wrapper_type::cell_type                            cell_type;
   typedef typename cell_complex_wrapper_type::cell_iterator                        cell_iterator;
   // -------------------------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------------------------   
   wrapper(Datastructure& domain) : domain_(domain) {}
   // -------------------------------------------------------------------------------------------      

   inline Datastructure& domain() { return domain_; }
   
   
   // -------------------------------------------------------------------------------------------
   struct geometry_iterator : viennamesh::iterator_base< viennamesh::wrapper< viennamesh::tag::gsse01, Datastructure > >
   {
      typedef viennamesh::wrapper< viennamesh::tag::gsse01, Datastructure >         wrapper_type;
      typedef viennamesh::iterator_base< wrapper_type >                             iterator_base_type;
      
      geometry_iterator(wrapper_type& obj)                : iterator_base_type (obj)         {};
      geometry_iterator(wrapper_type& obj, std::size_t newpos) : iterator_base_type (obj, newpos) {};      

      typename Datastructure::point_t 
      operator*() const
      {
         return (*this).obj().domain().get_point_fromhandle(this->pos());
      }            
   };   
   geometry_iterator geometry_begin()
   {
      return geometry_iterator(*this);
   }  
   geometry_iterator geometry_end()
   {
      return geometry_iterator(*this, domain().point_size());
   }   
   // -------------------------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------------------------
   struct segment_iterator : viennamesh::iterator_base< viennamesh::wrapper< viennamesh::tag::gsse01, Datastructure > >
   {
      typedef viennamesh::wrapper< viennamesh::tag::gsse01, Datastructure >      wrapper_type;
      typedef viennamesh::iterator_base< wrapper_type >                          iterator_base_type;
      
      segment_iterator(wrapper_type& obj)                : iterator_base_type (obj)         {};
      segment_iterator(wrapper_type& obj, size_t newpos) : iterator_base_type (obj, newpos) {};      

      gsse01_cell_complex_wrapper<Datastructure> 
      operator*() const
      {  
         return gsse01_cell_complex_wrapper<Datastructure>((*this).obj().domain(), (*this).pos());
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
   Datastructure& domain_;
   // -------------------------------------------------------------------------------------------      
};



} // end namespace viennamesh

#endif