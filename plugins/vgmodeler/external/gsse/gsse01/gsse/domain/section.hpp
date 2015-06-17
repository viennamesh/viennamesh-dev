/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_SECTION_HH_ID
#define GSSE_SECTION_HH_ID

// *** BOOST includes
//
#include <boost/test/minimal.hpp>

// *** GSSE includes
//
#include "gsse/math/matrix.hpp"           
#include "gsse/quantity_sparse.hpp"
#include "gsse/quantity.hpp"
#include "gsse/domain.hpp"

namespace gsse
{

// the GSSE::section (aka fiber bundle section)
//  has to automatically handle the segment information
//     *) the   theoretical section   concept is always defined over the whole base space
//     *) the   practical   section   concept has to manage the memory efficiently 
//

template<typename DomainT, typename QuantityNumericT, typename QuantityNameT>
class section_type
{

public:
   typedef QuantityNumericT                                                   numeric_t;
   typedef QuantityNameT                                                      quan_key_type;

   typedef quan_key_type                                                      vertex_key_type;      
   typedef quan_key_type                                                      edge_key_type;         
   typedef quan_key_type                                                      facet_key_type;        
   typedef quan_key_type                                                      cell_key_type;         

   typedef typename domain_traits<DomainT>::vertex_handle                     vertex_handle;
   typedef typename domain_traits<DomainT>::edge_handle                       edge_handle;
   typedef typename domain_traits<DomainT>::facet_handle                      facet_handle;
   typedef typename domain_traits<DomainT>::cell_handle                       cell_handle;
   
   typedef gsse_matrix<numeric_t>                                             storage_type;

   typedef gsse_quantity_sparse<vertex_handle, quan_key_type, storage_type>   vertex_quantity_type;
   typedef gsse_quantity_sparse<cell_handle,   quan_key_type, storage_type>   cell_quantity_type;
   typedef gsse_quantity_sparse<edge_handle,   quan_key_type, storage_type>   edge_quantity_type;
   typedef gsse_quantity_sparse<facet_handle,  quan_key_type, storage_type>   facet_quantity_type;

   typedef typename quantity_traits<vertex_quantity_type>::key_iterator       vertex_key_iterator;
   typedef typename quantity_traits<edge_quantity_type>::key_iterator         edge_key_iterator;
   typedef typename quantity_traits<facet_quantity_type>::key_iterator        facet_key_iterator;
   typedef typename quantity_traits<cell_quantity_type>::key_iterator         cell_key_iterator;

   // ##############################################################################################
   //
   //
public:

   section() 
      { }


  
public:
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
   // *** quantity access interface
   //
   typedef typename quantity_traits<vertex_quantity_type>::storage_type storage_vertex_type;
   void retrieve_quantity(const vertex_handle v_h, const vertex_key_type& key, storage_vertex_type & value)
   {  value = vertex_quantity.retrieve_quantity(v_h, key);   }
   void store_quantity(const vertex_handle& v_h, const vertex_key_type& key, const storage_vertex_type& value)
   {   vertex_quantity.insert(v_h, key, value);    }


   typedef typename quantity_traits<edge_quantity_type>::storage_type storage_edge_type;
   void retrieve_quantity(const edge_handle e_h, const edge_key_type& key, storage_edge_type & value)
   {  value = edge_quantity.retrieve_quantity(e_h, key);   }
   void store_quantity(const edge_handle& e_h, const edge_key_type& key, const storage_edge_type& value)
   {   edge_quantity.insert(e_h, key, value);    }


   //
   // ##############################################################################################


protected:



};

}



#endif


