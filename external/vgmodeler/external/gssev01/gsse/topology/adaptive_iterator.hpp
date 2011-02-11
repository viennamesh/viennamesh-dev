/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_ADAPTIVE_ITERATOR_HH_ID
#define GSSE_ADAPTIVE_ITERATOR_HH_ID 

// *** GSSE includes
// 
#include "gsse/topology.hpp"

// *** BOOST includes
// 
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_categories.hpp>


// *** system includes
// 
#include<vector>
#include<set>
#include<bitset>
#include<map>
#include<algorithm>
#include<iostream>

namespace gsse
{

template<typename TopologyT> struct topology_traits;  // let the compiler know that there are some

namespace detail_topology
{

///////////////////////////////////////////////////////////////////////////
// @brief Unstructured cell iterator class (for adaptive us topology)
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class unstructured_cell_iterator_adaptive
   : public boost::iterator_facade<
         unstructured_cell_iterator_adaptive<TopologyT>              // Derived
       , typename topology_traits<TopologyT>::cell          // Base
       , boost::bidirectional_traversal_tag                 // CategoryOrTraversal     
     >
{
  typedef unstructured_cell_iterator_adaptive<TopologyT>     self;
  typedef typename topology_traits<TopologyT>::cell          cell;
  typedef typename topology_traits<TopologyT>::cell_handle   cell_handle;
  typedef typename TopologyT::cell_indices_const_iterator    cell_indices_const_iterator;
  
public:
  typedef cell_handle       my_handle;
  typedef TopologyT         base_type;
  typedef cell              value_type;
  
private:
  TopologyT const*  g;
  int               size;
  
  cell_indices_const_iterator  cit_it_begin, cit_it_end;
  
public:
  unstructured_cell_iterator_adaptive() : g(0) {}
  explicit
  unstructured_cell_iterator_adaptive(TopologyT const& gg) : 
    g(&gg),  cit_it_begin( gg.get_ci_iterator_begin() ), 
    cit_it_end( gg.get_ci_iterator_end() )  {}

  unstructured_cell_iterator_adaptive(TopologyT const& gg, cell_indices_const_iterator ci_it) : 
    g(&gg), cit_it_begin( ci_it ), cit_it_end( gg.get_ci_iterator_end() )  {}
  
  friend class boost::iterator_core_access;
  
  void increment() { ++cit_it_begin;}
  void decrement() { --cit_it_begin;}
  bool equal(const self& other) const { return (cit_it_begin == other.cit_it_begin);}
  cell operator*()    const { return  cell(*g, (*cit_it_begin).first); }
  bool valid()        const { return ( cit_it_begin != cit_it_end); }

  TopologyT           const& topo_anchor() const { return *g; }
  base_type           const& base()        const { return *g; }
   
  friend std::ostream& operator<<(std::ostream& ostr, const unstructured_cell_iterator_adaptive<TopologyT>& mv)
  {
    ostr << "iterator: " ;
    return ostr;
  }
};

}  // end of namespace::detail_topology

}  // end of namespace::gsse

#endif
