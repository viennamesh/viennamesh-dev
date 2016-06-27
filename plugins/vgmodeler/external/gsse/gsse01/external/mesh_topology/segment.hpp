/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_SEGMENT_HH_ID
#define GSSE_SEGMENT_HH_ID

// *** GSSE includes
//
#include "gsse/topology.hpp"
#include "gsse/math/matrix.hpp"           
#include "gsse/quantity_sparse.hpp"
#include "gsse/quantity.hpp"

// *** BOOST includes
//
#include <boost/type_traits.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>
#include <boost/static_assert.hpp>
#include <boost/iterator/transform_iterator.hpp>


namespace gsse
{
namespace detail_segment
{

  // the concept of a GSSE Segment:


template<typename TopologyT, typename QuantityNumericT> 
class basic_segment  
{

public:
  // preparation of the specific part: 

  typedef TopologyT              topology_type;
  typedef TopologyT              topology_t;
  typedef QuantityNumericT       numeric_t;
  
  typedef std::string                            quan_key_type;
  typedef std::string                            quan_key_t;
  typedef gsse_matrix<numeric_t>                 storage_type;
  typedef gsse_matrix<numeric_t>                 storage_type;

  typedef quan_key_type vertex_key_type;      
  typedef quan_key_type cell_key_type;         
  typedef quan_key_type facet_key_type;        
  typedef quan_key_type edge_key_type;         
  typedef quan_key_type segment_key_type;

private:

  // end of the specific part


  typedef typename topology_traits<topology_type>::vertex_type       vertex_type;
  typedef typename topology_traits<topology_type>::cell_type         cell_type;
  typedef typename topology_traits<topology_type>::facet_type        facet_type;
  typedef typename topology_traits<topology_type>::edge_type         edge_type;
  
  typedef typename topology_traits<topology_type>::vertex_iterator   vertex_iterator;
  typedef typename topology_traits<topology_type>::cell_iterator     cell_iterator;
                                                      		   
  typedef typename topology_traits<topology_type>::vertex_handle     vertex_handle;
  typedef typename topology_traits<topology_type>::edge_handle       edge_handle;
  typedef typename topology_traits<topology_type>::facet_handle      facet_handle;
  typedef typename topology_traits<topology_type>::cell_handle       cell_handle;
                                                       		   
  typedef typename topology_traits<topology_type>::dimension_tag     dimension_tag;
  typedef typename topology_traits<topology_type>::dimension_tag     dimension_topology_tag;

public:

  typedef gsse_quantity_sparse<vertex_handle, quan_key_type, storage_type>       
  vertex_quantity_type;
  typedef gsse_quantity_sparse<cell_handle, quan_key_type, storage_type>         
  cell_quantity_type;
  typedef gsse_quantity_sparse<edge_handle, quan_key_type, storage_type>  
  edge_quantity_type;
  typedef gsse_quantity_sparse<facet_handle, quan_key_type, storage_type> 
  facet_quantity_type;
  typedef gsse_quantity_sparse<vertex_handle, quan_key_type, storage_type>       
  segment_quantity_type;

public:

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //
  //  --------------------   HANDLING OF THE SEGMENT NAME  ------------------------
  // 

  basic_segment()                             { segment_name = std::string("DefaultName"); }
  void set_name(const std::string& segname)   { segment_name = segname; }
  std::string const& get_segment_name() const { return segment_name;  }
  std::string const& get_name() const { return segment_name;  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////////
  //
  //  --------------------   THE QUANTITY PART OF THE SEGMENT   ------------------------
  // 

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //
  // *** Key iterators and iterator pairs
  //
  
  typedef typename quantity_traits<segment_quantity_type>::key_iterator segment_key_iterator;
  typedef typename quantity_traits<vertex_quantity_type>::key_iterator  vertex_key_iterator;
  typedef typename quantity_traits<edge_quantity_type>::key_iterator    edge_key_iterator;
  typedef typename quantity_traits<facet_quantity_type>::key_iterator   facet_key_iterator;
  typedef typename quantity_traits<cell_quantity_type>::key_iterator    cell_key_iterator;

  segment_key_iterator segment_key_begin() const {return segment_quantity.key_begin();}
  segment_key_iterator segment_key_end()   const {return segment_quantity.key_end();}
  vertex_key_iterator  vertex_key_begin()  const {return vertex_quantity.key_begin();}
  vertex_key_iterator  vertex_key_end()    const {return vertex_quantity.key_end();}
  edge_key_iterator    edge_key_begin()    const {return edge_quantity.key_begin();}
  edge_key_iterator    edge_key_end()      const {return edge_quantity.key_end();}
  facet_key_iterator   facet_key_begin()   const {return facet_quantity.key_begin();}
  facet_key_iterator   facet_key_end()     const {return facet_quantity.key_end();}
  cell_key_iterator    cell_key_begin()    const {return cell_quantity.key_begin();}
  cell_key_iterator    cell_key_end()      const {return cell_quantity.key_end();}

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //
  // *** Handle iterators and iterator pairs for private use
  //
  
private:

  typedef typename quantity_traits<vertex_quantity_type>::handle_iterator  vertex_handle_iterator;
  typedef typename quantity_traits<edge_quantity_type>::handle_iterator    edge_handle_iterator;
  typedef typename quantity_traits<facet_quantity_type>::handle_iterator   facet_handle_iterator;
  typedef typename quantity_traits<cell_quantity_type>::handle_iterator    cell_handle_iterator;
  
  vertex_handle_iterator  vertex_handle_begin()  const {return vertex_quantity.handle_begin();}
  vertex_handle_iterator  vertex_handle_end()    const {return vertex_quantity.handle_end();}
  edge_handle_iterator    edge_handle_begin()    const {return edge_quantity.handle_begin();}
  edge_handle_iterator    edge_handle_end()      const {return edge_quantity.handle_end();}
  facet_handle_iterator   facet_handle_begin()   const {return facet_quantity.handle_begin();}
  facet_handle_iterator   facet_handle_end()     const {return facet_quantity.handle_end();}
  cell_handle_iterator    cell_handle_begin()    const {return cell_quantity.handle_begin();}
  cell_handle_iterator    cell_handle_end()      const {return cell_quantity.handle_end();}

  //
  // *** quantity sizes for keys and handles
  //
  // xxx_object size --> xxx_size

   unsigned int vertex_object_size()  const {return vertex_quantity.object_size();}
   unsigned int edge_object_size()    const {return edge_quantity.object_size();}
   unsigned int facet_object_size()   const {return facet_quantity.object_size();}
   unsigned int cell_object_size()    const {return cell_quantity.object_size();}

public:   

   unsigned int vertex_key_size()     const {return vertex_quantity.key_size();}
   unsigned int edge_key_size()       const {return edge_quantity.key_size();}
   unsigned int facet_key_size()      const {return facet_quantity.key_size();}
   unsigned int cell_key_size()       const {return cell_quantity.key_size();}
   unsigned int segment_key_size()    const {return segment_quantity.key_size();}


  ///////////////////////////////////////////////////////////////////////////////////////////////
  //
  // *** quantity access for the segment
  //
  
  typedef typename quantity_traits<segment_quantity_type>::storage_type mapped_segment_type;

  void retrieve_quantity(const quan_key_type& key, storage_type & value)
  {
     value = segment_quantity.retrieve_quantity(0, key);
  }
  
  void store_quantity(const quan_key_type& key, const storage_type& value)
  {
    segment_quantity.insert(0, key, value);
  }
  
  mapped_segment_type &
  operator()(const quan_key_type& key)                      
  {
     return segment_quantity(0, key);
  }

  mapped_segment_type const&
  operator()(const quan_key_type& key) const
  {
     return segment_quantity(0, key);
  }


  ///////////////////////////////////////////////////////////////////////////////////////////////
  //
  // *** quantity access for the vertex
  //
  
  typedef typename quantity_traits<vertex_quantity_type>::storage_type mapped_vertex_type;

  
  void retrieve_quantity(const vertex_type& v, const quan_key_type& key, storage_type & value)
  {
    value = vertex_quantity.retrieve_quantity(v.handle(), key);
  }


  // not for general usage
  //
//   void store_quantity_rh(const vertex_handle& vh, const quan_key_type& key, const storage_type& value)
//   {
//     vertex_quantity.insert(vh, key, value);
//   }

  
  void store_quantity(const vertex_type& v, const quan_key_type& key, const storage_type& value)
  {
    vertex_quantity.insert(v.handle(), key, value);
  }

  // for readers only
   //
  void retrieve_quantity(const vertex_handle vh, const quan_key_type& key, storage_type & value)
  {
    value = vertex_quantity.retrieve_quantity(vh, key);
  }

  void store_quantity(const vertex_handle vh, const quan_key_type& key, const storage_type& value)
  {
    vertex_quantity.insert(vh, key, value);
  }


  mapped_vertex_type &
  operator()(const vertex_type& v, const quan_key_type& key)                      
  {
    return vertex_quantity(v.handle(), key);
  }

  mapped_vertex_type const&
  operator()(const vertex_type& v, const quan_key_type& key) const
  {
    return vertex_quantity(v.handle(), key);
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //
  // *** quantity access for the edge
  //
  
  typedef typename quantity_traits<edge_quantity_type>::storage_type mapped_edge_type; 
  
  void retrieve_quantity(const edge_type& e, const quan_key_type& key, storage_type & value)
  {
     value = edge_quantity.retrieve_quantity(e.handle(), key);
  }
  
  void store_quantity(const edge_type& e, const quan_key_type& key, const storage_type& value)
  {
    edge_quantity.insert(e.handle(), key, value);
  }

  // for readers only
  //
  void retrieve_quantity(const edge_handle eh, const quan_key_type& key, storage_type & value)
  {
     value = edge_quantity.retrieve_quantity(eh, key);
  }

  void store_quantity(const edge_handle eh, const quan_key_type& key, const storage_type& value)
  {
    edge_quantity.insert(eh, key, value);
  }
  
  mapped_edge_type &
  operator()(const edge_type& e, const quan_key_type& key)                      
  {
    return edge_quantity(e.handle(), key);
  }


  mapped_edge_type const&
  operator()(const edge_type& e, const quan_key_type& key) const
  {
    return edge_quantity(e.handle(), key);
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //
  // *** quantity access for the facet
  //
  
  typedef typename quantity_traits<facet_quantity_type>::storage_type mapped_facet_type;

  
  void retrieve_quantity(const facet_type& f, const quan_key_type& key, storage_type & value)
  {
    value = facet_quantity.retrieve_quantity(f.handle(), key);
  }

  void retrieve_quantity(const facet_type& f, const quan_key_type& key, storage_type const& value) const
  {
     value = facet_quantity.retrieve_quantity(f.handle(), key);
  }
   
  void store_quantity(const facet_type& f, const quan_key_type& key, const storage_type& value)
  {
    facet_quantity.insert(f.handle(), key, value);
  }
  
  // for readers only
   //
  void retrieve_quantity(const facet_handle fh, const quan_key_type& key, storage_type & value)
  {
     value = facet_quantity.retrieve_quantity(fh, key);
  }
  void store_quantity(const facet_handle fh, const quan_key_type& key, const storage_type& value)
  {
    facet_quantity.insert(fh, key, value);
  }

  mapped_facet_type &
  operator()(const facet_type& f, const quan_key_type& key)                      
  {
    return facet_quantity(f.handle(), key);
  }

  mapped_facet_type const&
  operator()(const facet_type& f, const quan_key_type& key) const
  {
    return facet_quantity(f.handle(), key);
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //
  // *** quantity access for cells
  //
  
  typedef typename quantity_traits<cell_quantity_type>::storage_type mapped_cell_type;

  
  void retrieve_quantity(const cell_type& c, const quan_key_type& key, storage_type & value)
  {
    value = cell_quantity.retrieve_quantity(c.handle(), key);
  }

  
  void store_quantity(const cell_type& c, const quan_key_type& key, const storage_type& value)
  {
    cell_quantity.insert(c.handle(), key, value);
  }
  
  // for readers only
   //
  void retrieve_quantity(const cell_handle ch, const quan_key_type& key, storage_type & value)
  {
     value = cell_quantity.retrieve_quantity(ch, key);
  }
  void store_quantity(const cell_handle ch, const quan_key_type& key, const storage_type& value)
  {
     cell_quantity.insert(ch, key, value);
  }

  mapped_cell_type &
  operator()(const cell_type& c, const quan_key_type& key)                      
  {
    return cell_quantity(c.handle(), key);
  }

  mapped_cell_type const&
  operator()(const cell_type& c, const quan_key_type& key) const
  {
     return cell_quantity(c.handle(), key);
  }

  // ////////////////////////////////////////////////////////////////////////////////////////////
  //
  // DEBUGGING 

  void dump_quantities_on_vertex()
  {   
    std::cout << vertex_quantity << std::endl;
  }

  void dump_quantities_on_edge()
  {   
    std::cout << edge_quantity << std::endl;
  }

  void dump_quantities_on_facet()
  {   
    std::cout << facet_quantity << std::endl;
  }

  void dump_quantities_on_cell()
  {   
    std::cout << cell_quantity << std::endl;
  }

  friend std::ostream& operator<<(std::ostream& ostr,const basic_segment& sg)
  {
    ostr << "Segment .. vertex size: " << sg.vertex_size() << " cell size: " << sg.cell_size();
    return ostr;
  }





   //
   //  --------------------   GENERAL (TOPOLOGICAL) PART OF THE SEGMENT   ------------------------
   // 
   
   unsigned int      cell_size()                              const { return topo_repository.get_cell_size_internal(); }
   unsigned int      vertex_size()                            const { return topo_repository.get_vertex_size_internal(); } 
   unsigned int      facet_size()                             const { return facet_object_size();}
   unsigned int      edge_size()                              const { return edge_object_size(); } 
  
   vertex_iterator   vertex_begin()                           const { return topo_repository.vertex_begin();} 
   vertex_iterator   vertex_end()                             const { return topo_repository.vertex_end();} 
   cell_iterator     cell_begin()                             const { return topo_repository.cell_begin();} 
   cell_iterator     cell_end()                               const { return topo_repository.cell_end();} 

   template <typename ElementT>
   struct elem_functor
   {
      typedef ElementT result_type;
      typedef typename ElementT::handle_type handle_type;
      typedef typename ElementT::topology_type topology_type;
      
      topology_type const* topo;
      
      elem_functor(topology_type const* topo) : topo(topo) {}
      elem_functor() : topo(0x0) {}
      
      ElementT operator()(handle_type const handle) const
      {
         return result_type(*topo, handle);
      }
   };

   typedef boost::transform_iterator <elem_functor<edge_type>, 
                                      edge_handle_iterator, 
                                      typename elem_functor<edge_type>::result_type > 
   internal_edge_iterator;
   
   typedef boost::transform_iterator <elem_functor<facet_type>, 
                                      facet_handle_iterator, 
                                      typename elem_functor<facet_type>::result_type > 
   internal_facet_iterator;
   

  internal_edge_iterator internal_edge_begin()  const 
  {return internal_edge_iterator(edge_handle_begin(), 
				 elem_functor<edge_type>(&topo_repository));} 
  
  internal_edge_iterator internal_edge_end() const 
  { return internal_edge_iterator(edge_handle_end(), 
				  elem_functor<edge_type>(&topo_repository));} 

  internal_facet_iterator internal_facet_begin() const
  { return internal_facet_iterator(facet_handle_begin(), 
			  elem_functor<facet_type>(&topo_repository));} 

  internal_facet_iterator internal_facet_end() const
  { return internal_facet_iterator(facet_handle_end(),   
			  elem_functor<facet_type>(&topo_repository));} 

  typedef gsse_iterator<internal_edge_iterator> edge_iterator;
  typedef gsse_iterator<internal_facet_iterator> facet_iterator;

  edge_iterator edge_begin() const 
  {return edge_iterator(internal_edge_begin(), internal_edge_begin(), internal_edge_end());}

  edge_iterator edge_end() const 
  {return edge_iterator(internal_edge_end(), internal_edge_begin(), internal_edge_end());}


  facet_iterator facet_begin() const 
  {return facet_iterator(internal_facet_begin(), internal_facet_begin(), internal_facet_end());}

  facet_iterator facet_end() const 
  {return facet_iterator(internal_facet_end(), internal_facet_begin(), internal_facet_end());}
  
   
  topology_type&        retrieve_topology()                        { return topo_repository;  }   
  const topology_type&  retrieve_topology()   	             const { return topo_repository;  }   
  vertex_type           add_user_point(vertex_handle vh )          { return topo_repository.add_user_point(vh); }


  ///////////////////////////////////////////////////////////////////////////////////////////////
  //
  // *** private member variables
  //

protected:
  topology_type       topo_repository;

private:
  segment_quantity_type  segment_quantity;

  vertex_quantity_type   vertex_quantity;
  edge_quantity_type     edge_quantity;
  facet_quantity_type    facet_quantity;
  cell_quantity_type     cell_quantity;

  std::string         segment_name;


};



// -------------- end of class  --------------------

//
// *** this class provides all extensions to the structured segments. 
//

template<typename TopologyT, typename QuantityNumericT> 
struct basic_segment_structured : public basic_segment<TopologyT, QuantityNumericT>
{
  typedef TopologyT topology_t;

  void resize_dim_n(const unsigned int& newdim1, const int& dim)   
  { 
    basic_segment<TopologyT, QuantityNumericT>::topo_repository.resize_dim_n(newdim1, dim); 
  }
};


//
// *** this class provides all extensions to the unstructured segments. 
//

template<typename TopologyT, typename QuantityNumericT> 
struct basic_segment_unstructured : public basic_segment<TopologyT, QuantityNumericT>
{
  typedef TopologyT topology_t;

  typedef typename topology_traits<TopologyT>::cell_2_vertex_mapping cell_2_vertex_mapping;
  typedef typename topology_traits<TopologyT>::cell_type  cell_type;
  
   void set_topo_size(long vertex_count)
   {
       basic_segment<TopologyT, QuantityNumericT>::topo_repository.set_topo_size(vertex_count);
   }

   void add_cell(const cell_2_vertex_mapping cpi)  
   { 
      basic_segment<TopologyT, QuantityNumericT>::topo_repository.add_cell(cpi);
   }
   // HP_ID
   //  .. tests with other input strategies
   void add_cell_2(const cell_2_vertex_mapping cpi)  
   { 
      basic_segment<TopologyT, QuantityNumericT>::topo_repository.add_cell_2(cpi);
   }
   // HP_ID
   //  .. tests with additional information
   //
   void add_cell_3(const cell_2_vertex_mapping cpi, const cell_2_vertex_mapping& cpi2)  
   { 
      basic_segment<TopologyT, QuantityNumericT>::topo_repository.add_cell_3(cpi,cpi2);
   }
   
   void remove_cell(cell_type c)
   {
      basic_segment<TopologyT, QuantityNumericT>::topo_repository.remove_cell(c);
   }
};
}    //namespace gsse_detail





/// ============================================================================================
/// *** this class is a get - metafunction to obtain the correct
/// *** segment from a topology and a numeric type the result can be
/// *** taken from the internal typedef type
///
// ############
template<typename TopologyT, typename QuantityNumericT> 
struct get_segment 
{
   
   typedef typename
   boost::mpl::if_
   <
      typename boost::is_same<typename topology_traits<TopologyT>::topology_tag, structured_topology_tag>, 
      detail_segment::basic_segment_structured<TopologyT, QuantityNumericT>, 
      detail_segment::basic_segment_unstructured<TopologyT, QuantityNumericT>
   > :: type
   
   type;  
};



/// ============================================================================================
/// *** The segment_traits class as well as the get_segment
/// *** metafunction are the only connections to the exterior.
///
// ############
template<typename SegmentT> 
struct segment_traits : public topology_traits<typename SegmentT::topology_type>
{
  typedef std::string                                        quan_key_type;      // OLD
  typedef std::string                                        quan_key_t;         // OLD

  typedef typename SegmentT::storage_type               storage_type; 
  typedef typename SegmentT::storage_type                     storage_type;        

  typedef typename SegmentT::topology_type                   topology_type;  
  typedef typename SegmentT::numeric_t			      quantity_type;  


  typedef typename SegmentT::facet_iterator                  facet_iterator;    
  typedef typename SegmentT::edge_iterator                   edge_iterator;
  
  typedef typename SegmentT::vertex_key_iterator             vertex_key_iterator;   
  typedef typename SegmentT::edge_key_iterator               edge_key_iterator;   
  typedef typename SegmentT::facet_key_iterator              facet_key_iterator;   
  typedef typename SegmentT::cell_key_iterator               cell_key_iterator;   
  typedef typename SegmentT::segment_key_iterator            segment_key_iterator;   
  
  typedef typename SegmentT::vertex_key_type                 vertex_key_type;   
  typedef typename SegmentT::edge_key_type                   edge_key_type;   
  typedef typename SegmentT::facet_key_type                  facet_key_type;   
  typedef typename SegmentT::cell_key_type                   cell_key_type;   
  typedef typename SegmentT::segment_key_type                segment_key_type;   
  
  typedef typename SegmentT::vertex_quantity_type            vertex_quantity_type;
  typedef typename SegmentT::edge_quantity_type              edge_quantity_type;
  typedef typename SegmentT::facet_quantity_type             facet_quantity_type;
  typedef typename SegmentT::cell_quantity_type              cell_quantity_type;
  typedef typename SegmentT::segment_quantity_type           segment_quantity_type;  
};
   
} // namespace gsse


#endif

