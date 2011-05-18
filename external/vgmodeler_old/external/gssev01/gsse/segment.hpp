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

// *** BOOST includes
//
#include <boost/type_traits.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/at.hpp>
#include <boost/static_assert.hpp>
#include <boost/iterator/transform_iterator.hpp>

// *** GSSE includes
//
#include "gsse/topology.hpp"
#include "gsse/math/matrix.hpp"           
#include "gsse/quantity_sparse.hpp"
#include "gsse/quantity.hpp"
#include "gsse/property_tree.hpp"

namespace gsse
{

namespace detail_segment
{



// ##############################################################################################
//

template<typename TopologyT, typename QuantityNumericT> 
class basic_segment  
{
public:
   typedef TopologyT                 topology_type;
   typedef QuantityNumericT          numeric_t;
   typedef std::string               quan_key_type;
   typedef gsse_matrix<numeric_t>    storage_type;
   typedef gsse_matrix<numeric_t>    data_pattern_type;  // [RH]200902 ..backward compability

   typedef quan_key_type             vertex_key_type;      
   typedef quan_key_type             cell_key_type;         
   typedef quan_key_type             facet_key_type;        
   typedef quan_key_type             edge_key_type;         
   typedef quan_key_type             segment_key_type;

   typedef gsse_container            gsse_type; // type information [RH] 200801

  typedef basic_segment<TopologyT, QuantityNumericT>                          self_t;
  typedef typename topology_traits<topology_type>::vertex_type                vertex_type;
  typedef typename topology_traits<topology_type>::edge_type                  edge_type;
  typedef typename topology_traits<topology_type>::facet_type                 facet_type;
  typedef typename topology_traits<topology_type>::cell_type                  cell_type;

  typedef typename topology_traits<topology_type>::vertex_iterator            vertex_iterator;
  typedef typename topology_traits<topology_type>::cell_iterator              cell_iterator;
  typedef typename topology_traits<topology_type>::edge_iterator              edge_iterator;
  typedef typename topology_traits<topology_type>::facet_iterator             facet_iterator;

  typedef typename topology_traits<topology_type>::edge_on_cell_iterator      edge_on_cell_iterator;
  typedef typename topology_traits<topology_type>::facet_on_cell_iterator     facet_on_cell_iterator;

private:
                                                      		   
  typedef typename topology_traits<topology_type>::vertex_handle              vertex_handle;
  typedef typename topology_traits<topology_type>::edge_handle                edge_handle;
  typedef typename topology_traits<topology_type>::facet_handle               facet_handle;
  typedef typename topology_traits<topology_type>::cell_handle                cell_handle;
                                                       		   
  typedef typename topology_traits<topology_type>::dimension_tag              dimension_tag;
  typedef typename topology_traits<topology_type>::dimension_tag              dimension_topology_tag;

 
public:

   typedef gsse_quantity_sparse<vertex_handle, quan_key_type, storage_type>   segment_quantity_type;
   typedef gsse_quantity_sparse<vertex_handle, quan_key_type, std::string>    segment_squantity_type;

   typedef std::set<quan_key_type>                                            segment_vertex_quantity_names_container;
   typedef std::set<quan_key_type>                                            segment_edge_quantity_names_container;
   typedef std::set<quan_key_type>                                            segment_facet_quantity_names_container;
   typedef std::set<quan_key_type>                                            segment_cell_quantity_names_container;

   typedef typename std::set<quan_key_type>::iterator                         segment_vertex_quantity_iterator;
   typedef typename std::set<quan_key_type>::iterator                         segment_edge_quantity_iterator;
   typedef typename std::set<quan_key_type>::iterator                         segment_facet_quantity_iterator;
   typedef typename std::set<quan_key_type>::iterator                         segment_cell_quantity_iterator;


public:

  //  --------------------   segment name
  // 

  basic_segment()                             { segment_name = std::string("DefaultName"); }
  void set_name(const std::string& segname)   { segment_name = segname; }
  std::string const& get_segment_name() const { return segment_name;  }
  std::string const& get_name() const { return segment_name;  }
 


  //  --------------------   quantity part
  //
  // numerical and text quantitites 
  //
  typedef typename quantity_traits<segment_quantity_type>::key_iterator    segment_key_iterator;
  typedef typename quantity_traits<segment_squantity_type>::key_iterator    segment_skey_iterator;

  segment_key_iterator  segment_key_begin()  const {return segment_quantity.key_begin(); }
  segment_key_iterator  segment_key_end()    const {return segment_quantity.key_end();   }
  segment_skey_iterator segment_skey_begin() const {return segment_squantity.key_begin();}
  segment_skey_iterator segment_skey_end()   const {return segment_squantity.key_end();  }

   unsigned int segment_key_size()           const {return segment_quantity.key_size();  }
   unsigned int segment_skey_size()          const {return segment_squantity.key_size(); }


  //  --------------------   quantity access for the segment
  //
  typedef typename quantity_traits<segment_quantity_type>::storage_type storage_segment_type;

  void retrieve_quantity(const quan_key_type& key, storage_type & value)
  {
     value = segment_quantity.retrieve_quantity(0, key);
  }
  
  void store_quantity(const quan_key_type& key, const storage_type& value)
  {
    segment_quantity.insert(0, key, value);
  }
  
  storage_segment_type &
  operator()(const quan_key_type& key)                      
  {
     return segment_quantity(0, key);
  }

  storage_segment_type const&
  operator()(const quan_key_type& key) const
  {
     return segment_quantity(0, key);
  }

  typedef typename quantity_traits<segment_squantity_type>::storage_type storage_ssegment_type;
  void retrieve_quantity(const quan_key_type& key, std::string& value)
      {
         value = segment_squantity.retrieve_quantity(0, key);
      }
   
  void store_quantity(const quan_key_type& key, std::string value)
      {
         segment_squantity.insert(0, key, value);
      }
   

  friend std::ostream& operator<<(std::ostream& ostr,const basic_segment& sg)
  {
    ostr << "Segment .. vertex size: " << sg.vertex_size() << " cell size: " << sg.cell_size();
    return ostr;
  }


   //  --------------------   topological part of a segment
   // 
   unsigned int      cell_size()                              const { return topo_repository.get_cell_size_internal(); }
   unsigned int      vertex_size()                            const { return topo_repository.get_vertex_size_internal(); } 
   unsigned int      edge_size()                              const { return topo_repository.edge_size(); } 
   unsigned int      facet_size()                             const { return topo_repository.facet_size();}

   // topological iterators (basic iterators)
   //
   vertex_iterator   vertex_begin()                           const { return topo_repository.vertex_begin();} 
   vertex_iterator   vertex_end()                             const { return topo_repository.vertex_end();} 
   cell_iterator     cell_begin()                             const { return topo_repository.cell_begin();} 
   cell_iterator     cell_end()                               const { return topo_repository.cell_end();} 

   edge_iterator     edge_begin()                                   { topo_repository.build_up_edge_container(); return topo_repository.edge_begin();} 
   edge_iterator     edge_end()                                     { return topo_repository.edge_end();} 
   edge_iterator     edge_end()                               const { return topo_repository.edge_end();} 
   facet_iterator    facet_begin()                                  { topo_repository.build_up_facet_container(); return topo_repository.facet_begin();} 
   facet_iterator    facet_end()                                    { return topo_repository.facet_end();} 
   facet_iterator    facet_end()                              const { return topo_repository.facet_end();} 


   void set_cell_index_offset(long offset)
      {
         topo_repository.set_cell_index_offset(offset );
      }

   long get_cell_index_offset()
      {
         return topo_repository.get_cell_index_offset();
      }


   // =========================================================================
   //
   // [RH]. be carefull with this ..
   //
   topology_type&        retrieve_topology()                        { return topo_repository;  }   
   const topology_type&  retrieve_topology()   	              const { return topo_repository;  }   


   // [RH].. experimental
   //
   vertex_type           add_user_point(vertex_handle vh )          { return topo_repository.add_user_point(vh); }


   // =========================================================================
   // [RH] 200801
   //   a segment knows it vertex/edge/facet/cell quantity names
   //  

   void add_vertex_quantity(const quan_key_type& quan_k)     	{ s_vq_container.insert(quan_k);   }
   void remove_vertex_quantity(const quan_key_type& quan_k)    { s_vq_container.erase(quan_k);    }
   segment_vertex_quantity_iterator  vertex_key_begin()  const { return s_vq_container.begin();   }
   segment_vertex_quantity_iterator  vertex_key_end()    const { return s_vq_container.end();     }
   long vertex_key_size()                                const { return s_vq_container.size();    }

   void add_edge_quantity(const quan_key_type& quan_k)         { s_eq_container.insert(quan_k);   }
   void remove_edge_quantity(const quan_key_type& quan_k)      { s_eq_container.erase(quan_k);    }
   segment_edge_quantity_iterator  edge_key_begin()      const { return s_eq_container.begin();   }
   segment_edge_quantity_iterator  edge_key_end()        const { return s_eq_container.end();     }
   long edge_key_size()                                  const { return s_eq_container.size();    }

   void add_facet_quantity(const quan_key_type& quan_k)        { s_fq_container.insert(quan_k);   } // std::cout << "added facet quantity..: " << quan_k << std::endl; std::cout << "size: " << facet_key_size() << std::endl; }
   segment_facet_quantity_iterator  facet_key_begin()    const { return s_fq_container.begin();   }
   segment_facet_quantity_iterator  facet_key_end()      const { return s_fq_container.end();     }
   long facet_key_size()                                 const { return s_fq_container.size();    }

   void add_cell_quantity(const quan_key_type& quan_k)         { s_cq_container.insert(quan_k);   }
   segment_cell_quantity_iterator  cell_key_begin()      const { return s_cq_container.begin();   }
   segment_cell_quantity_iterator  cell_key_end()        const { return s_cq_container.end();     }
   long cell_key_size()                                  const { return s_cq_container.size();    }


   template<typename ObjectT>
   void add_object_quantity(const quan_key_type& quan_k )
      {
         s_fq_container.insert(quan_k);
      }

   // =======================================================
   //

   bool is_vertex_handle_inside(const vertex_handle& vh) const
   {
      return topo_repository.is_vertex_handle_inside(vh);
   }

   bool is_cell_handle_inside(const cell_handle& ch) const
   {
      return topo_repository.is_cell_handle_inside(ch);
   }
   //
   // =======================================================


   // =========================================================================
   //
protected:

   topology_type                            topo_repository;
   std::string                              segment_name;

   // quantity section
   //
   segment_quantity_type                    segment_quantity;
   segment_squantity_type                   segment_squantity;
   
   segment_vertex_quantity_names_container  s_vq_container;
   segment_edge_quantity_names_container    s_eq_container;
   segment_facet_quantity_names_container   s_fq_container;
   segment_cell_quantity_names_container    s_cq_container;
   

 public:   // [RH][TODO] .. replace this with the boost::property tree
   gsse::property_tree ptree;
};




// ##############################################################################################
//
// *** this class provides all extensions to the structured segments. 
//
template<typename TopologyT, typename QuantityNumericT> 
struct basic_segment_structured : public basic_segment<TopologyT, QuantityNumericT>
{
  typedef TopologyT topology_type;

  void resize_dim_n(const unsigned int& newdim1, const int& dim)   
  { 
    basic_segment<TopologyT, QuantityNumericT>::topo_repository.resize_dim_n(newdim1, dim); 
  }


   

// ######################################################################
   typedef basic_segment_structured<TopologyT, QuantityNumericT>        self_t;
   typedef basic_segment<TopologyT, QuantityNumericT>                   base_segment_type;


   template<typename SegmentT>
   struct traverse_vertex
   {
      typedef typename SegmentT::vertex_iterator iterator;
      typedef typename SegmentT::vertex_type     value_type;
      
      traverse_vertex(SegmentT& segment) : segment(segment) {}
      
      iterator begin() { return segment.vertex_begin();}
      iterator end()   { return segment.vertex_end(); }
   protected:
      SegmentT& segment;
   };

   template<typename SegmentT>
   struct traverse_cell
   {
      typedef typename SegmentT::cell_iterator iterator;
      typedef typename SegmentT::cell_type     value_type;
      
      traverse_cell(SegmentT& segment) : segment(segment) {}
      
      iterator begin() { return segment.cell_begin();}
      iterator end()   { return segment.cell_end(); }
   protected:
      SegmentT& segment;
   };


   // ----
   template<typename SegmentT>
   struct traverse_edge
   {
      typedef typename SegmentT::edge_iterator iterator;
      typedef typename SegmentT::edge_type     value_type;
      
      traverse_edge(SegmentT& segment) : segment(segment) {   }
      
      iterator begin() { return segment.edge_begin();}
      iterator end()   { return segment.edge_end(); }
   protected:
      SegmentT& segment;
   };

   template<typename SegmentT>
   struct traverse_facet
   {
      typedef typename SegmentT::facet_iterator iterator;
      typedef typename SegmentT::facet_type     value_type;
      
      traverse_facet(SegmentT& segment) : segment(segment) {  }
      
      iterator begin() { return segment.facet_begin();}
      iterator end()   { return segment.facet_end(); }
   protected:
      SegmentT& segment;
   };


   typedef boost::mpl::map<
      boost::mpl::pair<boost::mpl::pair<self_t, typename base_segment_type::vertex_type>,  traverse_vertex<self_t> >,
      boost::mpl::pair<boost::mpl::pair<self_t, typename base_segment_type::cell_type>,    traverse_cell  <self_t> >,
      boost::mpl::pair<boost::mpl::pair<self_t, typename base_segment_type::edge_type>,    traverse_edge  <self_t> >,
      boost::mpl::pair<boost::mpl::pair<self_t, typename base_segment_type::facet_type>,   traverse_facet <self_t> >
      > traversal_repository;

};


//
// *** this class provides all extensions to the unstructured segments. 
//

template<typename TopologyT, typename QuantityNumericT> 
struct basic_segment_unstructured : public basic_segment<TopologyT, QuantityNumericT>
{
  typedef TopologyT topology_type;

  typedef typename topology_traits<TopologyT>::cell_2_vertex_mapping cell_2_vertex_mapping;
  typedef typename topology_traits<TopologyT>::cell_type    cell_type;
  typedef typename topology_traits<TopologyT>::cell_handle  cell_handle;
  
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
   cell_handle add_cell_2(const cell_2_vertex_mapping cpi)  
   { 
      return basic_segment<TopologyT, QuantityNumericT>::topo_repository.add_cell_2(cpi);
   }
   unsigned long add_cell_2r(const cell_2_vertex_mapping cpi)  
   { 
     return basic_segment<TopologyT, QuantityNumericT>::topo_repository.add_cell_2r(cpi);
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


// ######################################################################
   typedef basic_segment_unstructured<TopologyT, QuantityNumericT>      self_t;
   typedef basic_segment<TopologyT, QuantityNumericT>                   base_segment_type;


   template<typename SegmentT>
   struct traverse_vertex
   {
      typedef typename SegmentT::vertex_iterator iterator;
      typedef typename SegmentT::vertex_type     value_type;
      
      traverse_vertex(SegmentT& segment) : segment(segment) {}
      
      iterator begin() { return segment.vertex_begin();}
      iterator end()   { return segment.vertex_end(); }
   protected:
      SegmentT& segment;
   };

   template<typename SegmentT>
   struct traverse_cell
   {
      typedef typename SegmentT::cell_iterator iterator;
      typedef typename SegmentT::cell_type     value_type;
      
      traverse_cell(SegmentT& segment) : segment(segment) {}
      
      iterator begin() { return segment.cell_begin();}
      iterator end()   { return segment.cell_end(); }
   protected:
      SegmentT& segment;
   };


   // ----
   template<typename SegmentT>
   struct traverse_edge
   {
      typedef typename SegmentT::edge_iterator iterator;
      typedef typename SegmentT::edge_type     value_type;
      
      traverse_edge(SegmentT& segment) : segment(segment) {   }
      
      iterator begin() { return segment.edge_begin();}
      iterator end()   { return segment.edge_end(); }
   protected:
      SegmentT& segment;
   };

   template<typename SegmentT>
   struct traverse_facet
   {
      typedef typename SegmentT::facet_iterator iterator;
      typedef typename SegmentT::facet_type     value_type;
      
      traverse_facet(SegmentT& segment) : segment(segment) {  }
      
      iterator begin() { return segment.facet_begin();}
      iterator end()   { return segment.facet_end(); }
   protected:
      SegmentT& segment;
   };


   typedef boost::mpl::map<
      boost::mpl::pair<boost::mpl::pair<self_t, typename base_segment_type::vertex_type>,  traverse_vertex<self_t> >,
      boost::mpl::pair<boost::mpl::pair<self_t, typename base_segment_type::cell_type>,    traverse_cell  <self_t> >,
      boost::mpl::pair<boost::mpl::pair<self_t, typename base_segment_type::edge_type>,    traverse_edge  <self_t> >,
      boost::mpl::pair<boost::mpl::pair<self_t, typename base_segment_type::facet_type>,   traverse_facet <self_t> >
      > traversal_repository;


};
}    //namespace gsse_detail





// ##############################################################################################
//
// *** this class is a get - metafunction to obtain the correct
// *** segment from a topology and a numeric type the result can be
// *** taken from the internal typedef type
//
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


// ##############################################################################################
//
// *** The segment_traits class as well as the get_segment
// *** metafunction are the only connections to the exterior.
//
// ############
template<typename SegmentT> 
struct segment_traits : public topology_traits<typename SegmentT::topology_type>
{
  typedef std::string                                        quan_key_type;      // OLD
  typedef std::string                                        quan_key_t;         // OLD

  typedef typename SegmentT::storage_type                    storage_type;        
  typedef typename SegmentT::data_pattern_type               data_pattern_type;        


  typedef typename SegmentT::topology_type                   topology_type;  
  typedef typename SegmentT::numeric_t			     quantity_type;  

   typedef typename SegmentT::facet_iterator                  facet_iterator;    
   typedef typename SegmentT::edge_iterator                   edge_iterator;
   
   typedef typename SegmentT::segment_key_iterator            segment_key_iterator;   
   typedef typename SegmentT::segment_skey_iterator           segment_skey_iterator;   
   typedef typename SegmentT::segment_key_type                segment_key_type;   
   typedef typename SegmentT::segment_quantity_type           segment_quantity_type;  
   typedef typename SegmentT::segment_squantity_type          segment_squantity_type;  
   
   typedef typename SegmentT::segment_vertex_quantity_iterator   segment_vertex_quantity_iterator  ;
   typedef typename SegmentT::segment_edge_quantity_iterator     segment_edge_quantity_iterator  ;
   typedef typename SegmentT::segment_facet_quantity_iterator    segment_facet_quantity_iterator  ;
   typedef typename SegmentT::segment_cell_quantity_iterator     segment_cell_quantity_iterator  ;
};
   

} // namespace gsse


#endif

