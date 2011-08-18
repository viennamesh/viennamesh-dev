/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_WRAPPER_VIENNAGRID_HPP
#define VIENNAMESH_WRAPPER_VIENNAGRID_HPP

#include "viennamesh/wrapper/wrapper_base.hpp"
#include "viennamesh/tags.hpp"
#include "viennautils/value_type.hpp"

#include "viennagrid/forwards.h"
#include "viennagrid/domain.hpp"
//#include "viennagrid/celltags.hpp"

namespace viennamesh {

#ifndef VIENNAMESH_VIENNAGRID_SUPPORT
#define VIENNAMESH_VIENNAGRID_SUPPORT
namespace tag {
struct viennagrid {};
} // end namespace tag   
#endif

template<typename ArrayT>
struct viennagrid_point_wrapper
{
   // -------------------------------------------------------------------------------------------
   typedef viennagrid_point_wrapper<ArrayT>        self_type;
   typedef typename ArrayT::value_type           result_type;
   // -------------------------------------------------------------------------------------------   
   viennagrid_point_wrapper(ArrayT& array) : array_(array) {}
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
      return ArrayT::dimension_tag::value;
   }
   size_t size() 
   {
      return ArrayT::dimension_tag::value;
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

template<typename VgridCellT>
struct viennagrid_cell_wrapper
{
   // -------------------------------------------------------------------------------------------
   typedef viennagrid_cell_wrapper<VgridCellT>         self_type;
   typedef size_t                                      result_type;
   // -------------------------------------------------------------------------------------------   
   typedef typename viennagrid::result_of::ncell_range<VgridCellT, 0>::type              vertex_on_cell_container_type;
   typedef typename viennagrid::result_of::iterator<vertex_on_cell_container_type>::type     vertex_on_cell_iterator_type;   
   
   viennagrid_cell_wrapper(VgridCellT& vgrid_cell) : vgrid_cell_(vgrid_cell) 
   { 
      vertex_on_cell_cont_ = viennagrid::ncells<0>(vgrid_cell_); 
   }
   // -------------------------------------------------------------------------------------------   
   result_type operator[](size_t i) const
   { 
      vertex_on_cell_iterator_type vocit = vertex_on_cell_cont_.begin();
      std::advance(vocit,i);
      return (vocit)->id();
   }      
 
   result_type operator[](size_t i) 
   { 
      vertex_on_cell_iterator_type vocit = vertex_on_cell_cont_.begin();
      std::advance(vocit,i);
      return (vocit)->id();
   }     
   // -------------------------------------------------------------------------------------------   
   result_type size() const
   {
      //return viennagrid::traits::subcell_desc<VgridCellT, 0>::num_elements;
      result_type i = 0;
      for(vertex_on_cell_iterator_type vocit = vertex_on_cell_cont_.begin();
          vocit != vertex_on_cell_cont_.end(); vocit++)
      {
         i++;
      }
      return i;
   }
   result_type size() 
   {
      //return viennagrid::traits::subcell_desc<VgridCellT, 0>::num_elements;   
      result_type i = 0;
      for(vertex_on_cell_iterator_type vocit = vertex_on_cell_cont_.begin();
          vocit != vertex_on_cell_cont_.end(); vocit++)
      {
         i++;
      }
      return i;
   }
   // -------------------------------------------------------------------------------------------
   friend std::ostream& operator<<(std::ostream& ostr, self_type const& val) 
   {
      for(vertex_on_cell_iterator_type vocit = val.begin();
          vocit != val.end(); vocit++)
      {
         ostr << vocit->id() << " ";
      }     
      return ostr;
    }      
   // -------------------------------------------------------------------------------------------   
   
   vertex_on_cell_iterator_type begin()
   {
      return vertex_on_cell_cont_.begin();
   }
   vertex_on_cell_iterator_type begin() const
   {
      return vertex_on_cell_cont_.begin();
   }   
   vertex_on_cell_iterator_type end()
   {
      return vertex_on_cell_cont_.end();
   }
   vertex_on_cell_iterator_type end() const
   {
      return vertex_on_cell_cont_.end();
   }   
   
   
   VgridCellT  vgrid_cell_;
   vertex_on_cell_container_type vertex_on_cell_cont_;
   
};


template<typename DomainT>
struct viennagrid_cell_complex_wrapper
{
private:
   typedef typename DomainT::config_type                                                  domain_config_type;
   static const int DIMT = domain_config_type::cell_tag::topology_level;
   typedef viennagrid::segment_t<domain_config_type>                                      vgrid_segment_type;
   typedef typename viennagrid::result_of::ncell_type<domain_config_type, DIMT>::type     vgrid_cell_type;         
   
public:   
   typedef viennamesh::viennagrid_cell_wrapper<vgrid_cell_type>                           cell_type;
   
   viennagrid_cell_complex_wrapper(DomainT& domain, size_t segment_id) : domain_(domain), segment_id_(segment_id) {}
   
   //struct cell_iterator : viennamesh::iterator_base_2< cell_complex_wrapper<DomainT>, cell_type >
   struct cell_iterator : viennamesh::iterator_base< viennagrid_cell_complex_wrapper<DomainT> > 
   {
      //typedef  viennamesh::iterator_base_2< cell_complex_wrapper<DomainT>, cell_type > iterator_base_type;
      typedef  viennamesh::iterator_base< viennagrid_cell_complex_wrapper<DomainT> > iterator_base_type;

      cell_iterator(viennagrid_cell_complex_wrapper<DomainT>& obj)                : iterator_base_type (obj)         {};
      cell_iterator(viennagrid_cell_complex_wrapper<DomainT>& obj, size_t newpos) : iterator_base_type (obj, newpos) {};      

//       cell_type 
//       dereference() const
//       {
//          vgrid_segment_type const & segment = (*this).obj().domain().segment((*this).obj().segment_id());
//          vgrid_cell_type cell = segment.cells( (*this).pos() );
//          //std::cout << "cellcomplexwrapper: " << viennamesh::viennagrid_cell_wrapper<vgrid_cell_type>(cell) << std::endl;
//          return viennamesh::viennagrid_cell_wrapper<vgrid_cell_type>(cell);
//       }             
      cell_type 
      operator*() const
      {
         vgrid_segment_type const & segment = (*this).obj().domain().segment((*this).obj().segment_id());
         vgrid_cell_type cell = segment.cells( (*this).pos() );
         //std::cout << "cellcomplexwrapper: " << viennamesh::viennagrid_cell_wrapper<vgrid_cell_type>(cell) << std::endl;
         return viennamesh::viennagrid_cell_wrapper<vgrid_cell_type>(cell);
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
      vgrid_segment_type const & segment = (*this).domain().segment((*this).segment_id());
      return cell_iterator(*this, segment.template size<DIMT>());  
   }        
   cell_iterator cell_end()
   {
      vgrid_segment_type const & segment = (*this).domain().segment((*this).segment_id());
      return cell_iterator(*this, segment.template size<DIMT>());  
   }        
   
   inline DomainT& domain()      { return domain_;     }   
   inline size_t   segment_id()  { return segment_id_; }

private:
   DomainT& domain_;
   size_t   segment_id_;
};


template<typename Datastructure>
struct wrapper <viennamesh::tag::viennagrid, Datastructure>
{   
   // -------------------------------------------------------------------------------------------
   // extract meta information from datastructure
   //
   typedef typename Datastructure::config_type              domain_config_type;
   typedef typename domain_config_type::numeric_type        numeric_type;
   typedef typename domain_config_type::cell_tag            vgrid_cell_type;   
   static const int DIMT = vgrid_cell_type::topology_level;
   static const int DIMG = domain_config_type::dimension_tag::value;  
   typedef typename viennagrid::result_of::point_type<domain_config_type>::type                    vgrid_point_type;      
   typedef typename viennagrid::result_of::ncell_range<Datastructure, 0>::type                 vgrid_vertex_container_type;
   typedef typename viennagrid::result_of::iterator<vgrid_vertex_container_type>::type             vgrid_vertex_container_iterator_type;

   // -------------------------------------------------------------------------------------------   
   // provide wrappers for the datastructures
   //
   typedef typename viennamesh::viennagrid_point_wrapper<vgrid_point_type>             point_type;
   typedef typename viennamesh::viennagrid_cell_complex_wrapper<Datastructure>         cell_complex_wrapper_type;
   typedef typename viennagrid_cell_complex_wrapper<Datastructure>::cell_iterator      cell_iterator;
   typedef typename viennagrid_cell_complex_wrapper<Datastructure>::cell_type          cell_type;   
   // -------------------------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------------------------   
   wrapper(Datastructure& domain) : domain_(domain) {}
   // -------------------------------------------------------------------------------------------   
   
   inline Datastructure& domain() { return domain_; }
   
   // -------------------------------------------------------------------------------------------
   //struct geometry_iterator : viennamesh::iterator_base_2< viennamesh::wrapper< viennamesh::tag::viennagrid, Datastructure >, point_wrapper_type >
   struct geometry_iterator : viennamesh::iterator_base< viennamesh::wrapper< viennamesh::tag::viennagrid, Datastructure > >
   {
      typedef viennamesh::wrapper< viennamesh::tag::viennagrid, Datastructure >     wrapper_type;
      //typedef viennamesh::iterator_base_2< wrapper_type, point_type >       iterator_base_type;
      typedef viennamesh::iterator_base< wrapper_type >                             iterator_base_type;
      
      geometry_iterator(wrapper_type& obj)                : iterator_base_type (obj)         {};
      geometry_iterator(wrapper_type& obj, size_t newpos) : iterator_base_type (obj, newpos) {};      

      // when the iterator is dereferenced, wrap the point object with the 
      // viennagrid specific wrapper. this way arbitrary datastructures can 
      // be accessed in a unified way
      //
/*      point_type
      dereference() const
      {
         return point_type( (*this).obj().domain().vertex( (*this).pos() ) .getPoint() );
      }      */
      point_type
      operator*() const
      {
         return point_type( (*this).obj().domain().vertex( (*this).pos() ) .getPoint() );
      }            

   };   
   geometry_iterator geometry_begin()
   {
      return geometry_iterator(*this);
   }  
   geometry_iterator geometry_end()
   {
      return geometry_iterator(*this, domain().template size<0>());
   }   
   // -------------------------------------------------------------------------------------------

   // -------------------------------------------------------------------------------------------
   //struct segment_iterator : viennamesh::iterator_base_2< viennamesh::wrapper< viennamesh::tag::viennagrid, Datastructure >, cell_complex_wrapper_type >
   struct segment_iterator : viennamesh::iterator_base< viennamesh::wrapper< viennamesh::tag::viennagrid, Datastructure > >
   {
      typedef viennamesh::wrapper< viennamesh::tag::viennagrid, Datastructure >  wrapper_type;
      typedef viennamesh::iterator_base< wrapper_type >                          iterator_base_type;
      
      segment_iterator(wrapper_type& obj)                : iterator_base_type (obj)         {};
      segment_iterator(wrapper_type& obj, size_t newpos) : iterator_base_type (obj, newpos) {};      

      // when the iterator is dereferenced, wrap the point object with the 
      // viennagrid specific wrapper. this way arbitrary datastructures can 
      // be accessed in a unified way
      //
//       cell_complex_wrapper_type 
//       dereference() const
//       {  
//          return cell_complex_wrapper_type((*this).obj().domain(), (*this).pos());
//       }      
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
      return segment_iterator(*this, domain().segment_container()->size());
   }      
   std::size_t segment_size()  { return domain().segment_container()->size(); }
   // -------------------------------------------------------------------------------------------   


   // -------------------------------------------------------------------------------------------   
   Datastructure & domain_;
   // -------------------------------------------------------------------------------------------
};
   
} // end namespace viennamesh

#endif


