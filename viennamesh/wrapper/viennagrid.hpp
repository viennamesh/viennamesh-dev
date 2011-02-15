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

#ifndef VIENNAMESH_WRAPPER_VIENNAGRID_HPP
#define VIENNAMESH_WRAPPER_VIENNAGRID_HPP

#include "viennamesh/wrapper/wrapper_base.hpp"
#include "viennamesh/tags.hpp"
#include "viennautils/value_type.hpp"

#include "viennagrid/celltags.hpp"

namespace viennamesh {

namespace tag {
struct viennagrid {};
} // end namespace tag   

// namespace detail {
// } // end namespace detail

// namespace result_of {
// 
// template<typename ViennaGridCellTag>
// struct extract_cell_topology 
// {
//    typedef viennamesh::tag::none type;
// };
// template<>
// struct extract_cell_topology <viennagrid::line_tag >
// {
//    typedef viennamesh::tag::simplex type;
// };
// template<>
// struct extract_cell_topology <viennagrid::triangle_tag >
// {
//    typedef viennamesh::tag::simplex type;
// };
// template<>
// struct extract_cell_topology <viennagrid::tetrahedron_tag >
// {
//    typedef viennamesh::tag::simplex type;
// };
// template<>
// struct extract_cell_topology <viennagrid::quadrilateral_tag >
// {
//    typedef viennamesh::tag::cuboid type;
// };
// 
// } // end namespace result_of

template<typename ArrayT>
struct viennagrid_point_wrapper
{
   // -------------------------------------------------------------------------------------------
   typedef viennagrid_point_wrapper<ArrayT>        self_type;
   typedef typename ArrayT::numeric_type           result_type;
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

template<typename ArrayT>
struct viennagrid_cell_wrapper
{
   // -------------------------------------------------------------------------------------------
   typedef viennagrid_cell_wrapper<ArrayT>         self_type;
//   typedef typename ArrayT::numeric_type           result_type;
   // -------------------------------------------------------------------------------------------   
   viennagrid_cell_wrapper(ArrayT& array) : array_(array) {}
   // -------------------------------------------------------------------------------------------   
   //result_type& operator[](size_t i) const
   void operator[](size_t i) const
   { 
      
   }      
 
/*   result_type& operator[](size_t i) 
   { 
      return array_[i];      
   }    */  
   // -------------------------------------------------------------------------------------------
//    size_t size() const
//    {
//       return ArrayT::dimension_tag::value;
//    }
//    size_t size() 
//    {
//       return ArrayT::dimension_tag::value;
//    }
   // -------------------------------------------------------------------------------------------
   friend std::ostream& operator<<(std::ostream& ostr, self_type const& val) 
   {
//      size_t size = val.size();
//       if (size == 0) return ostr;
//       for (size_t i = 0; i < size; i++)
//       {
//          if (i == size-1) ostr << val[i];
//          else             ostr << val[i] << "  ";
//       }
      return ostr;
   }      
   // -------------------------------------------------------------------------------------------   
   ArrayT & array_;
};


template<typename DomainT>
struct cell_complex_wrapper
{
   typedef typename DomainT::config_type                                                  domain_config_type;
   static const int DIMT = domain_config_type::cell_tag::topology_level;
   typedef viennagrid::segment_t<domain_config_type>                                      vgrid_segment_type;
   typedef typename viennagrid::result_of::ncell_type<domain_config_type, DIMT>::type     vgrid_cell_type;         
   
   typedef viennamesh::viennagrid_cell_wrapper<vgrid_cell_type>  cell_type;
   
   cell_complex_wrapper(DomainT& domain, size_t segment_id) : domain_(domain), segment_id_(segment_id) {}
   
   struct cell_iterator : viennamesh::iterator_base< cell_complex_wrapper<DomainT> > 
   {
      cell_iterator(cell_complex_wrapper<DomainT>& obj)                : viennamesh::iterator_base< cell_complex_wrapper<DomainT> > (obj)         {};
      cell_iterator(cell_complex_wrapper<DomainT>& obj, size_t newpos) : viennamesh::iterator_base< cell_complex_wrapper<DomainT> > (obj, newpos) {};      

      cell_type 
      operator*() const
      {
         vgrid_segment_type const & segment = (*this).obj().domain().segment((*this).obj().segment_id());
         vgrid_cell_type cell = segment.cells( (*this).pos() );
         
         std::cout << viennamesh::viennagrid_cell_wrapper<vgrid_cell_type>(cell) << std::endl;
         return viennamesh::viennagrid_cell_wrapper<vgrid_cell_type>(cell);
      }      
   };
   
   cell_iterator cell_begin()
   {
      return cell_iterator(*this);
   }  
   cell_iterator cell_end()
   {
      return cell_iterator(*this, 12);  //TODO DERIVCE CELL SIZE FROM DOMAIN
   }        
   
   inline DomainT& domain()      { return domain_;     }   
   inline size_t   segment_id()  { return segment_id_; }
   
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
   typedef typename viennagrid::result_of::ncell_container<Datastructure, 0>::type                 vgrid_vertex_container_type;
   typedef typename viennagrid::result_of::iterator<vgrid_vertex_container_type>::type             vgrid_vertex_container_iterator_type;

   // -------------------------------------------------------------------------------------------   
   // provide wrappers for the datastructures
   //
   typedef typename viennamesh::viennagrid_point_wrapper<vgrid_point_type>       point_wrapper_type;
   typedef typename viennamesh::cell_complex_wrapper<Datastructure>              cell_complex_wrapper_type;
   // -------------------------------------------------------------------------------------------
   
   // -------------------------------------------------------------------------------------------
   typedef typename cell_complex_wrapper<Datastructure>::cell_iterator     cell_iterator;
   typedef typename cell_complex_wrapper<Datastructure>::cell_type         cell_type;   
   // -------------------------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------------------------   
   wrapper(Datastructure& domain) : domain_(domain) {}
   // -------------------------------------------------------------------------------------------   
   
   inline Datastructure& domain() { return domain_; }
   
   // -------------------------------------------------------------------------------------------
   struct geometry_iterator : viennamesh::iterator_base< viennamesh::wrapper< viennamesh::tag::viennagrid, Datastructure > >
   {
      typedef viennamesh::wrapper< viennamesh::tag::viennagrid, Datastructure >  wrapper_type;
      
      geometry_iterator(wrapper_type& obj)                : viennamesh::iterator_base< wrapper_type > (obj)         {};
      geometry_iterator(wrapper_type& obj, size_t newpos) : viennamesh::iterator_base< wrapper_type > (obj, newpos) {};      

      // when the iterator is dereferenced, wrap the point object with the 
      // viennagrid specific wrapper. this way arbitrary datastructures can 
      // be accessed in a unified way
      //
      point_wrapper_type 
      operator*() const
      {
         return point_wrapper_type( (*this).obj().domain().vertex( (*this).pos() ) .getPoint() );
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
   struct segment_iterator : viennamesh::iterator_base< viennamesh::wrapper< viennamesh::tag::viennagrid, Datastructure > >
   {
      typedef viennamesh::wrapper< viennamesh::tag::viennagrid, Datastructure >  wrapper_type;
      
      segment_iterator(wrapper_type& obj)                : viennamesh::iterator_base< wrapper_type > (obj)         {};
      segment_iterator(wrapper_type& obj, size_t newpos) : viennamesh::iterator_base< wrapper_type > (obj, newpos) {};      

      // when the iterator is dereferenced, wrap the point object with the 
      // viennagrid specific wrapper. this way arbitrary datastructures can 
      // be accessed in a unified way
      //
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
   // -------------------------------------------------------------------------------------------   

   // -------------------------------------------------------------------------------------------
//    struct cell_iterator : viennamesh::iterator_base< viennamesh::wrapper< viennamesh::tag::viennagrid, Datastructure > >
//    {
//       typedef viennamesh::wrapper< viennamesh::tag::viennagrid, Datastructure >  wrapper_type;
//       
//       cell_iterator(wrapper_type& obj)                : viennamesh::iterator_base< wrapper_type > (obj)         {};
//       cell_iterator(wrapper_type& obj, size_t newpos) : viennamesh::iterator_base< wrapper_type > (obj, newpos) {};      
// 
//       // when the iterator is dereferenced, wrap the point object with the 
//       // viennagrid specific wrapper. this way arbitrary datastructures can 
//       // be accessed in a unified way
//       //
//       point_wrapper_type 
//       operator*() const
//       {
//          //return point_wrapper_type( (*this).obj().domain().vertex( (*this).pos() ) .getPoint());
//       }
//    };   
//    cell_iterator cell_begin()
//    {
//       return cell_iterator(*this);
//    }  
//    cell_iterator cell_end()
//    {
//       return cell_iterator(*this, domain().segment_container()->size());
//    }      
   // -------------------------------------------------------------------------------------------   

   // -------------------------------------------------------------------------------------------   
   Datastructure & domain_;
   // -------------------------------------------------------------------------------------------
};
   
} // end namespace viennamesh

#endif


