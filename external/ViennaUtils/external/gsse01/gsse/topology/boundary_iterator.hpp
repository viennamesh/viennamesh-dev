/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_BOUNDARY_ITERATOR_HH_ID
#define GSSE_BOUNDARY_ITERATOR_HH_ID 

// *** GSSE includes
// 
#include "gsse/topology.hpp"

// *** BOOST includes
//
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/array.hpp>

// *** system includes
//
#include<vector>
#include<set>
#include<map>
#include<algorithm>
#include<iostream>


namespace gsse
{
template<typename TopologyT> struct topology_traits;  // let the compiler know that there are some

namespace detail_topology
{

///////////////////////////////////////////////////////////////////////////
/// @brief vertex cell on boundary iterator
///
/// This iterator is specialized within all topologies and dimensions
/// on the particular topology type. This is only a wrapper iterator
/// for the tasks, derived from boost::iterator_facade
///
/// TODO:: check all iterator traits
///
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class vertex_on_boundary_iterator 
  : public boost::iterator_facade<
        vertex_on_boundary_iterator<TopologyT>       
      , typename topology_traits<TopologyT>::vertex_type
      , boost::bidirectional_traversal_tag         
      , typename topology_traits<TopologyT>::vertex_type
    >
{
  typedef vertex_on_boundary_iterator<TopologyT> self;

  typedef typename topology_traits<TopologyT>::cell_iterator            cell_iterator;
  typedef typename topology_traits<TopologyT>::cell_handle              cell_handle;
  typedef typename topology_traits<TopologyT>::cell_type                cell_type;      
  typedef typename topology_traits<TopologyT>::vertex_type              vertex_type;      
  typedef typename topology_traits<TopologyT>::cell_on_facet_iterator   cell_on_facet_iterator;
  typedef typename topology_traits<TopologyT>::vertex_on_facet_iterator vertex_on_facet_iterator;
  typedef typename topology_traits<TopologyT>::facet_on_cell_iterator   facet_on_cell_iterator;

public:

  typedef TopologyT   base_type;
  
  vertex_on_boundary_iterator() : mc(-1) {}


  explicit
  vertex_on_boundary_iterator(TopologyT const& top) : top(top)
  {
     vertices.clear();

     for (cell_iterator cit = top.cell_begin(); cit.valid(); ++cit)
     {
        //std::cout << *cit << std::endl;
        
	for(facet_on_cell_iterator focit(*cit); focit.valid(); ++focit)
        {
	    int count = 0;
            //std::cout << (*focit).handle() << " :";
            
            cell_on_facet_iterator cofit(*focit);

            for (; cofit.valid(); ++cofit, ++count);
            //std::cout << "# gsseinternal.. vob it: " << count << std::endl;

            if (count == 1)
	      {
                 //std::cout << "#### count: " << count << std::endl;
                 for (vertex_on_facet_iterator vofit(*focit); vofit.valid(); ++vofit)
                 {
		    vertices.insert(*vofit);
                 }
	      }
	  }
      }

    mc = vertices.begin();
    //std::cout << "size of segx: " << vertices.size() << std::endl;
  }

//    template<typename SegmentT>
//    vertex_on_boundary_iterator(SegmentT& segment) : top(segment.retrieve_topology())
//    {
//      vertices.clear();

//      for (cell_iterator cit = top.cell_begin(); cit.valid(); ++cit)
//      {
//         //std::cout << *cit << std::endl;
        
// 	for(facet_on_cell_iterator focit(*cit); focit.valid(); ++focit)
//         {
// 	    int count = 0;
//             //std::cout << (*focit).handle() << " :";
            
//             cell_on_facet_iterator cofit(*focit);

//             for (; cofit.valid(); ++cofit, ++count);
//             //std::cout << "# gsseinternal.. vob it: " << count << std::endl;

//             if (count == 1)
// 	      {
//                  //std::cout << "#### count: " << count << std::endl;
//                  for (vertex_on_facet_iterator vofit(*focit); vofit.valid(); ++vofit)
//                  {
// 		    vertices.insert(*vofit);
//                  }
// 	      }
// 	  }
//       }

//     mc = vertices.begin();
//     //std::cout << "size of segx: " << vertices.size() << std::endl;
//   }



  friend class boost::iterator_core_access;
  
   void increment() { ++mc; }
   void decrement() { --mc; }
   void advance(int n) {mc+=n;}    
   bool equal(const self& other) const 
      { return mc == other.mc && topo_anchor() == other.topo_anchor(); }
   vertex_type dereference()   const { return *mc; }
   
   bool valid()       const { return mc != vertices.end(); }
   void reset() {mc = vertices.begin();}
   TopologyT  const& topo_anchor() const { return top;}
   base_type  const& base()        const { return top;}
   
private:
  const TopologyT&                           top;
  std::set<vertex_type>                      vertices;
  typename std::set<vertex_type>::iterator   mc;
};


template <typename TopologyT>
class facet_on_boundary_iterator 
  : public boost::iterator_facade<
        facet_on_boundary_iterator<TopologyT>       
      , typename topology_traits<TopologyT>::facet_type
      , boost::bidirectional_traversal_tag         
      , typename topology_traits<TopologyT>::facet_type
    >
{
  typedef facet_on_boundary_iterator<TopologyT> self;
  typedef typename topology_traits<TopologyT>::cell_iterator            cell_iterator;
  typedef typename topology_traits<TopologyT>::cell_handle              cell_handle;
  typedef typename topology_traits<TopologyT>::cell_type                cell_type;      
  typedef typename topology_traits<TopologyT>::vertex_type              vertex_type;      
  typedef typename topology_traits<TopologyT>::facet_type               facet_type;      
  typedef typename topology_traits<TopologyT>::cell_on_facet_iterator   cell_on_facet_iterator;
  typedef typename topology_traits<TopologyT>::vertex_on_facet_iterator vertex_on_facet_iterator;
  typedef typename topology_traits<TopologyT>::facet_on_cell_iterator   facet_on_cell_iterator;

public:

  typedef TopologyT   base_type;
  
  facet_on_boundary_iterator() : mc(-1) {}

  explicit
  facet_on_boundary_iterator(TopologyT const& top) : top(top)
  {
    for (cell_iterator cit = top.cell_begin(); cit.valid(); ++cit)
      {
	for(facet_on_cell_iterator focit(*cit); focit.valid(); ++focit)
	  {
	    int count = 0;
	    for (cell_on_facet_iterator cofit(*focit); cofit.valid(); ++cofit, ++count);
	    
	    if (count != 2)  {facets.insert(*focit); }
	  }
      }
    mc = facets.begin();
  }

  friend class boost::iterator_core_access;
  
  void increment() { ++mc; }
  void decrement() { --mc; }
  void advance(int n) {mc+=n;}    
  bool equal(const self& other) const 
  { return ( (mc == other.mc && topo_anchor() == other.topo_anchor())); }
  facet_type dereference()   const { return *mc; }
  bool valid()       const { return mc != facets.end(); }
  void reset() {mc = facets.begin();}
  
  TopologyT  const& topo_anchor() const { return top;}
  base_type  const& base()        const { return top;}
  
private:
  const TopologyT&                           top;
  std::set<facet_type>                       facets;
  typename std::set<facet_type>::iterator    mc;
};


///////////////////////////////////////////////////////////////////////////
/// @brief Common vertex on boundary iterator
///
/// This iterator is specialized within all topologies and dimensions on the particular topology type
/// This is only a wrapper iterator for the tasks, derived from boost::iterator_facade
///
/// TODO:: check all iterator traits
///
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class structured_vertex_on_boundary_iterator 
  : public boost::iterator_facade<
        structured_vertex_on_boundary_iterator<TopologyT>    
      , typename topology_traits<TopologyT>::vertex_type     
      , boost::bidirectional_traversal_tag                   
      , typename topology_traits<TopologyT>::vertex_type     
    >
{
   typedef structured_vertex_on_boundary_iterator<TopologyT> self;

   typedef typename topology_traits<TopologyT>::vertex_type                vertex_type;      
   typedef typename topology_traits<TopologyT>::vertex_handle              vertex_handle;      
   typedef typename topology_traits<TopologyT>::dimension_tag              dimension_tag;      
   typedef typename std::set<vertex_handle>                                result_container;

   static unsigned int const neighbor_cells = topology_traits<TopologyT>::dimension_tag::dim + 1;
   

public:

  typedef TopologyT   base_type;
  
  structured_vertex_on_boundary_iterator() : iter(vertices.end()) {}

  explicit
  structured_vertex_on_boundary_iterator(TopologyT const& topo) : top(&topo)
  {
    vertex_type v = *((*top).vertex_begin());

    boost::array<long, dimension_tag::dim> pos = (*top).get_vertex_index(v);

    int dim;

    for (dim = 1; dim <= dimension_tag::dim; ++dim)
      {
	while(true)
	  {
	    int subdim;

	    if (dim == 1)
	      subdim = 2;
	    else 
	      subdim = 1;

	    while(subdim <= dimension_tag::dim)
	      {
		++pos[subdim-1];
		if (pos[subdim-1] == int((*top).get_dim_n_size(subdim) ))
		  {
		    pos[subdim-1] = 0;
		  }
		else 
		  break;

		++subdim;
		if (subdim == dim) 
		  ++subdim;
	      }

	    // ###
	    // std::for_each(pos.begin(), pos.end(), std::cout << phoenix::arg1 << "  ");
	    // std::cout << (*top).get_index_vertex(pos).handle() << std::endl;
	    // ###


	    vertices.insert((*top).get_index_vertex(pos));
	    pos[dim-1] = (*top).get_dim_n_size(dim)-1;

	    // ###
	    // std::cout << "INSERT VERTEX" << (*top).get_index_vertex(pos) << std::endl;
	    //std::for_each(pos.begin(), pos.end(), std::cout << phoenix::arg1 << "  ");
	    //std::cout << (*top).get_index_vertex(pos).handle() << std::endl;
	    // ###

	    vertices.insert((*top).get_index_vertex(pos));
	    pos[dim-1] = 0;

	    // ###
	    // std::cout << "INSERT VERTEX" << (*top).get_index_vertex(pos) << std::endl;
	    // ###

	    if (subdim > dimension_tag::dim) break;

	  }
      }
    iter = vertices.begin();
  }

  friend class boost::iterator_core_access;
  
  void increment() { ++iter; }
  void decrement() { --iter; }
  bool equal(const self& other) const 
  { return ( (iter == other.iter && topo_anchor() == other.topo_anchor())); }

  vertex_type dereference() const { return *iter; }
  bool   valid()     const { return iter != vertices.end(); }
  void reset() {iter = vertices.begin();}
  
  TopologyT    const& topo_anchor() const { return top;}  
  TopologyT    const& base() const { return top;}  

private:
  const TopologyT* top;
  std::set<vertex_type> vertices;
  typename std::set<vertex_type>::iterator iter;

};

}  // end of namespace::detail_topology

}  // end of namespace::gsse

#endif
