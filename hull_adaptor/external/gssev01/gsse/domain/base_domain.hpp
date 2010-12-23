/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_DOMAINNEW_BASE_HH_ID
#define GSSE_DOMAINNEW_BASE_HH_ID

// *** system includes
//
// include <iostream>

// *** GSSE includes
//
#include "gsse/topology.hpp"
#include "gsse/math/matrix.hpp"           
#include "gsse/quantity_traits.hpp"
#include "gsse/quantity.hpp"
#include "gsse/quantity_sparse.hpp"

namespace gsse
{
namespace detail_domain
{


template<typename TopologyT, typename NumericTQuantity, typename NameTQuantity = std::string>
class base_domain
{
   
   typedef NumericTQuantity                                                   numeric_t;
   typedef NameTQuantity                                                      quan_key_type;

public:
   typedef quan_key_type                                                      vertex_key_type;      
   typedef quan_key_type                                                      edge_key_type;         
   typedef quan_key_type                                                      facet_key_type;        
   typedef quan_key_type                                                      cell_key_type;         

private:
   typedef typename topology_traits<TopologyT>::vertex_handle                 vertex_handle;
   typedef typename topology_traits<TopologyT>::edge_handle                   edge_handle;
   typedef typename topology_traits<TopologyT>::facet_handle                  facet_handle;
   typedef typename topology_traits<TopologyT>::cell_handle                   cell_handle;

   typedef typename topology_traits<TopologyT>::vertex_type                   vertex_type;
   typedef typename topology_traits<TopologyT>::edge_type                     edge_type;
   typedef typename topology_traits<TopologyT>::facet_type                    facet_type;
   typedef typename topology_traits<TopologyT>::cell_type                     cell_type;

   typedef typename topology_traits<TopologyT>::vertex_iterator               vertex_iterator;
   typedef typename topology_traits<TopologyT>::edge_iterator                 edge_iterator;    
   typedef typename topology_traits<TopologyT>::facet_iterator                facet_iterator;
   typedef typename topology_traits<TopologyT>::cell_iterator                 cell_iterator;


public:
   typedef gsse_matrix<numeric_t>                                             storage_type;
   typedef gsse_container                                                     gsse_type;     // type information [RH] 200801
   typedef NumericTQuantity                                                   quan_numeric_t;
//   typedef quan_key_type                                                      quan_key_type;


protected:
   typedef gsse_quantity_sparse<vertex_handle, quan_key_type, storage_type>   vertex_quantity_type;
   typedef gsse_quantity_sparse<cell_handle,   quan_key_type, storage_type>   cell_quantity_type;
   typedef gsse_quantity_sparse<edge_handle,   quan_key_type, storage_type>   edge_quantity_type;
   typedef gsse_quantity_sparse<facet_handle,  quan_key_type, storage_type>   facet_quantity_type;

//   ## object handle with dimension
//   object_handle<0>   handle_vertex
//   object_handle<1>   handle_edge
//   object_handle<2>   handle_facet
//   object_handle<3>   handle_cell
//
//
//  ... quantity container for 3D::
//
//    boost::fusion::vector3<object_handle<0> , object_handle<1> , object_handle<2> , object_handle<2> > quantity_container;
//
//   template<long DIM>
//   void retrieve_quantity(const object_handle<DIM>& objecth, const key_type& key, storage_vertex_type& value)
//   {  value = quantity_container<DIM>.retrieve_quantity(objecth.handle(), key);  }



   // ##############################################################################################
   //
   //

  
public:
//[RH][TODO] .. check this with the gsse::segment
//
//    typedef typename vertex_quantity_type::key_iterator       vertex_key_iterator;
//    typedef typename edge_quantity_type::key_iterator         edge_key_iterator;
//    typedef typename facet_quantity_type::key_iterator        facet_key_iterator;
//    typedef typename cell_quantity_type::key_iterator         cell_key_iterator;

   typedef typename quantity_traits<vertex_quantity_type>::key_iterator       vertex_key_iterator;
   typedef typename quantity_traits<edge_quantity_type>::key_iterator         edge_key_iterator;
   typedef typename quantity_traits<facet_quantity_type>::key_iterator        facet_key_iterator;
   typedef typename quantity_traits<cell_quantity_type>::key_iterator         cell_key_iterator;


   unsigned int vertex_key_size()     const {return vertex_quantity.key_size();}
   unsigned int edge_key_size()       const {return edge_quantity.key_size();}
   unsigned int facet_key_size()      const {return facet_quantity.key_size();}
   unsigned int cell_key_size()       const {return cell_quantity.key_size();}

   vertex_key_iterator   vertex_key_begin()   const {return vertex_quantity.key_begin();}
   vertex_key_iterator   vertex_key_end()     const {return vertex_quantity.key_end();}
   edge_key_iterator     edge_key_begin()     const {return edge_quantity.key_begin();}
   edge_key_iterator     edge_key_end()       const {return edge_quantity.key_end();}
   facet_key_iterator    facet_key_begin()    const {return facet_quantity.key_begin();}
   facet_key_iterator    facet_key_end()      const {return facet_quantity.key_end();}
   cell_key_iterator     cell_key_begin()     const {return cell_quantity.key_begin();}
   cell_key_iterator     cell_key_end()       const {return cell_quantity.key_end();}



   // ##############################################################################################
   //
   // *** vertex quantity access interface
   //
   typedef typename quantity_traits<vertex_quantity_type>::storage_type storage_vertex_type;

   // normal access
   //
   void retrieve_quantity(const vertex_type& object, const vertex_key_type& key, storage_vertex_type & value)
   {  value = vertex_quantity.retrieve_quantity(object.handle(), key);  }
   void store_quantity(const vertex_type& object, const vertex_key_type& key, const storage_vertex_type& value)
   {  vertex_quantity.insert(object.handle(), key, value);              }


   // functional access
   //
   storage_vertex_type&         operator()(const vertex_type& object, const quan_key_type& key)                      
   {  
//      std::cout << " domain:: quan access 1..: " << object.handle() << " / " << key << std::endl;
      return vertex_quantity(object.handle(), key);  }

   storage_vertex_type const&   operator()(const vertex_type& object, const quan_key_type& key) const
   {    return vertex_quantity(object.handle(), key);  }

   // direct handle access
   //
   void retrieve_quantity_direct(const vertex_handle& object_h, const vertex_key_type& key, storage_vertex_type & value)
   {  value = vertex_quantity.retrieve_quantity(object_h, key);  }
   void store_quantity_direct(const vertex_handle& object_h, const vertex_key_type& key, const storage_vertex_type& value)
   {  vertex_quantity.insert(object_h, key, value);              }


   // ##############################################################################################
   //
   // *** edge quantity access interface
   //
   typedef typename quantity_traits<edge_quantity_type>::storage_type storage_edge_type;
   void retrieve_quantity(const edge_type& object, const edge_key_type& key, storage_edge_type & value)
   {  value = edge_quantity.retrieve_quantity(object.handle(), key);   }
   void store_quantity(const edge_type& object, const edge_key_type& key, const storage_edge_type& value)
   {   edge_quantity.insert(object.handle(), key, value);              }

   storage_edge_type &        operator()(const edge_type& object, const quan_key_type& key)                      
   {      return edge_quantity(object.handle(), key);   }
   storage_edge_type const&   operator()(const edge_type& object, const quan_key_type& key) const
   {  return edge_quantity(object.handle(), key);  }
   
   void retrieve_quantity_direct(const edge_handle& object_h, const edge_key_type& key, storage_edge_type & value)
   {  value = edge_quantity.retrieve_quantity(object_h, key);   }
   void store_quantity_direct(const edge_handle& object_h, const edge_key_type& key, const storage_edge_type& value)
   {   edge_quantity.insert(object_h, key, value);              }


   // ##############################################################################################
   //
   // *** facet quantity access interface
   //
   typedef typename quantity_traits<facet_quantity_type>::storage_type storage_facet_type;
   void retrieve_quantity(const facet_type& object, const facet_key_type& key, storage_facet_type & value)
   {  value = facet_quantity.retrieve_quantity(object.handle(), key);   }
   void store_quantity(const facet_type& object, const facet_key_type& key, const storage_facet_type& value)
   {   facet_quantity.insert(object.handle(), key, value);              }

   storage_facet_type &        operator()(const facet_type& object, const quan_key_type& key)                      
   {  return facet_quantity(object.handle(), key);  }
   storage_facet_type const&   operator()(const facet_type& object, const quan_key_type& key) const
   {  return facet_quantity(object.handle(), key);  }
   
   void retrieve_quantity_direct(const facet_handle& object_h, const facet_key_type& key, storage_facet_type & value)
   {  value = facet_quantity.retrieve_quantity(object_h, key);   }
   void store_quantity_direct(const facet_handle& object_h, const facet_key_type& key, const storage_facet_type& value)
   {   facet_quantity.insert(object_h, key, value);              }


   // ##############################################################################################
   //
   // *** cell quantity access interface
   //
   typedef typename quantity_traits<cell_quantity_type>::storage_type storage_cell_type;
   void retrieve_quantity(const cell_type& object, const cell_key_type& key, storage_cell_type & value)
   {  value = cell_quantity.retrieve_quantity(object.handle(), key);   }
   void store_quantity(const cell_type& object, const cell_key_type& key, const storage_cell_type& value)
   {   cell_quantity.insert(object.handle(), key, value);              }

   storage_cell_type &        operator()(const cell_type& object, const quan_key_type& key)                      
   {  return cell_quantity(object.handle(), key);  }
   storage_cell_type const&   operator()(const cell_type& object, const quan_key_type& key) const
   {  return cell_quantity(object.handle(), key);  }
   
   void retrieve_quantity_direct(const cell_handle& object_h, const cell_key_type& key, storage_cell_type & value)
   {  value = cell_quantity.retrieve_quantity(object_h, key);   }
  // [RH][TODO] .. remove this problem with const & 
  //    for temporaries..
  //
//    void store_quantity_direct(const cell_handle& object_h, const cell_key_type& key, const storage_cell_type& value)
//    {   cell_quantity.insert(object_h, key, value);              }
   void store_quantity_direct(const cell_handle object_h, const cell_key_type key, const storage_cell_type value)
   {   cell_quantity.insert(object_h, key, value);              }



   // ##############################################################################################
   //
   // *** debug quantities
   //
  void dump_quantities_on_vertex()
  {    std::cout << vertex_quantity << std::endl; }

  void dump_quantities_on_edge()
  {    std::cout << edge_quantity << std::endl;   }

  void dump_quantities_on_facet()
  {    std::cout << facet_quantity << std::endl;  }

  void dump_quantities_on_cell()
  {   std::cout << cell_quantity << std::endl;    }




   // ##############################################################################################
   //
   // *** quan init facility
   //
   template<typename SegmentT>
   void init_vertex_quantity(SegmentT& segment, std::string quan_key, storage_type storage)
      {
         segment.add_vertex_quantity(quan_key);
         for (vertex_iterator ob_it = segment.vertex_begin(); ob_it != segment.vertex_end(); ++ob_it)
         {
            store_quantity(*ob_it, quan_key, storage);
         }
      }
   template<typename SegmentT>
   void init_edge_quantity(SegmentT& segment, std::string quan_key, storage_type storage)
      {
         segment.add_edge_quantity(quan_key);
         for (edge_iterator ob_it = segment.edge_begin(); ob_it != segment.edge_end(); ++ob_it)
         {
            store_quantity(*ob_it, quan_key, storage);
         }
      }
   template<typename SegmentT>
   void init_facet_quantity(SegmentT& segment, std::string quan_key, storage_type storage)
      {
         segment.add_facet_quantity(quan_key);
         for (facet_iterator ob_it = segment.facet_begin(); ob_it != segment.facet_end(); ++ob_it)
         {
            store_quantity(*ob_it, quan_key, storage);
         }
      }
   template<typename SegmentT>
   void init_cell_quantity(SegmentT& segment, std::string quan_key, storage_type storage)
      {
         segment.add_cell_quantity(quan_key);
         for (cell_iterator ob_it = segment.cell_begin(); ob_it != segment.cell_end(); ++ob_it)
         {
            store_quantity(*ob_it, quan_key, storage);
         }
      }
   



protected:
  vertex_quantity_type    vertex_quantity;
  edge_quantity_type      edge_quantity;
  facet_quantity_type     facet_quantity;
  cell_quantity_type      cell_quantity;

};

}  // namespace detail_domain
}  // namespace gsse
#endif
