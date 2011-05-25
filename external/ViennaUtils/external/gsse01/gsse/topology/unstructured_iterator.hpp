/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_UNSTRUCTURED_ITERATOR_HH_ID
#define GSSE_UNSTRUCTURED_ITERATOR_HH_ID 

//  *** GSSE includes
//
#include "gsse/topology.hpp"
#include "gsse/math/power.hpp"

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

template<typename TopologyT> struct topology_traits;  // forward declaration

namespace detail_topology
{
  template <typename TopologyT>
  class unstructured_vertex_iterator
    : public boost::iterator_facade<
    unstructured_vertex_iterator<TopologyT>                    
    , typename topology_traits<TopologyT>::vertex_type              
    , boost::bidirectional_traversal_tag                       
    , typename topology_traits<TopologyT>::vertex_type              
    >
  {
    typedef unstructured_vertex_iterator<TopologyT>            self;
    typedef int                                                counter_t;
    typedef typename topology_traits<TopologyT>::vertex_type   vertex_type;
    typedef typename topology_traits<TopologyT>::vertex_handle vertex_handle;

  public:
    typedef TopologyT         base_type;
    typedef vertex_handle     my_handle;

  private:
    TopologyT const*  g;
    counter_t        mc;
    int              size;
     
  public:
    unstructured_vertex_iterator() : g(0), mc(-1) {}
    explicit
    unstructured_vertex_iterator(TopologyT const& gg) : 
      g(&gg), mc(0), size(g->vertex_size()) {}
    unstructured_vertex_iterator(TopologyT const& gg, int vv) : 
      g(&gg), mc(vv), size(g->vertex_size()) {}

    friend class boost::iterator_core_access;

    void increment() { ++mc; }
    void decrement() { --mc; }
    void advance(int n) {mc+=n;}  
    bool equal(const self& other) const 
    { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= size); }

    vertex_type dereference()  const 
    { if (mc < size) return vertex_type(*g, g->get_vertex_index(mc)); else return vertex_type(); } 

    bool valid() const 
    { return (mc < size); }
    void reset() {mc = 0;}

     self end()  { self new_object(*this); new_object.mc = size; return new_object; }


    TopologyT const&         topo_anchor() const { return *g; }
    base_type const&         base()        const { return *g; }
  };


  template <typename TopologyT>
  class unstructured_cell_iterator
    : public boost::iterator_facade<
    unstructured_cell_iterator<TopologyT>                     // Derived
    , typename topology_traits<TopologyT>::cell_type          // Base
    , boost::bidirectional_traversal_tag                      // CategoryOrTraversal     
    , typename topology_traits<TopologyT>::cell_type          // Base
    >
  {
    typedef unstructured_cell_iterator<TopologyT>                 self;
    typedef int                                                   counter_t;
    typedef typename topology_traits<TopologyT>::cell_type             cell_type;
    typedef typename topology_traits<TopologyT>::cell_handle      cell_handle;
    
  public:
    typedef TopologyT         base_type;
    
  public:
     unstructured_cell_iterator() : g(0), mc(-1) {}
     explicit
     unstructured_cell_iterator(TopologyT const& gg) : 
        g(&gg), mc(0), size(g->get_cell_size_internal()) {}
     
     unstructured_cell_iterator(TopologyT const& gg, int vv) : 
        g(&gg), mc(vv),size(g->get_cell_size_internal()) {}
     
     friend class boost::iterator_core_access;
     
     void increment() { ++mc; }
     void decrement() { --mc; }
     void advance(int n) {mc+=n;}    // random access 
     bool equal(const self& other) const 
        { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= size); }
     cell_type dereference()   const { if (mc < size) return cell_type(*g, mc+g->get_cell_index_offset()); else return cell_type();} 
     bool valid()         const { return (mc < size); }
     void reset() {mc = 0;}

     self end()  { self new_object(*this); new_object.mc = size; return new_object; }

     
     TopologyT const&  topo_anchor() const { return *g; }
     base_type const&  base()        const { return *g; }
     
  private:
     TopologyT const*  g;
     counter_t        mc;
     int              size;
  };
  
  

  template <typename TopologyT>
  class unstructured_vertex_on_cell_iterator
    : public boost::iterator_facade<
    unstructured_vertex_on_cell_iterator<TopologyT>     // Derived
    , typename topology_traits<TopologyT>::vertex_type         // Base
    , boost::random_access_traversal_tag                  // CategoryOrTraversal     
    , typename topology_traits<TopologyT>::vertex_type         // Base
    , unsigned int
    >
  {
    typedef unstructured_vertex_on_cell_iterator<TopologyT>       self;
    typedef int                                                   counter_t;
    typedef typename topology_traits<TopologyT>::cell_type        cell_type;
    typedef typename topology_traits<TopologyT>::vertex_type      vertex_type;
    typedef typename topology_traits<TopologyT>::vertex_handle    vertex_handle;
    typedef typename topology_traits<TopologyT>::dimension_tag    dimension_tag;

    
  public:
     typedef cell_type    base_type;
     typedef vertex_type  traverse_type;

     unstructured_vertex_on_cell_iterator() : mc(-1) {}
     explicit
     unstructured_vertex_on_cell_iterator(cell_type const& cc) : c(cc), mc(0) 
        { }
     unstructured_vertex_on_cell_iterator(cell_type const& cc, counter_t count) : c(cc), mc(count) {}
     
     friend class boost::iterator_core_access;
     
     void increment() { ++mc; }
     void decrement() { --mc; }
     void advance(int n) {mc+=n;}    
     
     unsigned int distance_to(self const& other) const {return mc - other.mc;}
     
     bool equal(const self& other) const 
        { return ( (other.c.handle() == c.handle() && mc == other.mc && 
                    topo_anchor() == other.topo_anchor())  || mc >= dimension_tag::dim+1); }

     vertex_type dereference()    const 
        { 
// 	  std::cout << " DEREF " << mc << "   " << c.handle() << "  " 
// 	              << topo_anchor().get_vertex_index_from_cell(c.handle(),mc) << std::endl;
           
           if (mc < dimension_tag::dim+1) 
	     {
//   	       std::cout << "## normal" << std::endl;
//   	       std::cout << "## c.handle() " << c.handle() << std::endl;
//   	       std::cout << "## mc " << mc << std::endl;
//  	       std::cout << " topo: " << topo_anchor().get_vertex_index_from_cell(c.handle(),mc) << std::endl;
                return vertex_type(topo_anchor(), topo_anchor().get_vertex_index_from_cell(c.handle() - topo_anchor().get_cell_index_offset(),mc)); 
	     }
           else 
	     {
 	       //std::cout << "## else" << std::endl;
	       return vertex_type();
	     }
        } 
    
     bool valid()          const { return (mc < dimension_tag::dim+1); }
     TopologyT const&      topo_anchor()  const { return c.topo_anchor(); }
     base_type const&      base()         const { return c;}
     void reset() {mc = 0;}
     self end()  { self new_object(*this); new_object.mc = dimension_tag::dim+1; return new_object; }

//    friend std::ostream& operator<<(std::ostream& ostr, const self& vocit)
//       {
//          ostr << "uvocit: " << vocit.mc;
//          return ostr;
//       }

     
  private:
    cell_type    c;
    counter_t    mc;
  };
  
  template <typename TopologyT>
  class unstructured_cell_on_vertex_iterator
    : public boost::iterator_facade<
    unstructured_cell_on_vertex_iterator<TopologyT>  
    , typename topology_traits<TopologyT>::cell_type      
    , boost::bidirectional_traversal_tag             
    , typename topology_traits<TopologyT>::cell_type      
    >
  {
    typedef unstructured_cell_on_vertex_iterator<TopologyT>       self;
    typedef int                                                   counter_t;
    typedef typename topology_traits<TopologyT>::cell_type        cell_type;
    typedef typename topology_traits<TopologyT>::vertex_type      vertex_type;
    typedef typename topology_traits<TopologyT>::cell_handle      cell_handle;
    typedef typename topology_traits<TopologyT>::dimension_tag    dimension_tag;


    
  public:
     typedef typename topology_traits<TopologyT>::cell_type        value_type;    // STL iterator concepts
    typedef vertex_type  base_type;
    typedef cell_type    traverse_type;

  public:
    unstructured_cell_on_vertex_iterator() {}
    explicit
    unstructured_cell_on_vertex_iterator(vertex_type const& vv) : v(vv)
    {
    // [RH].. is this really a ch ??
    //    .. not a vh ?
    //
	       
		 cell_handle ch = (*(topo_anchor().vh_2_ci_m.find(vv.handle()))).second;

       iter = vv.topo_anchor().vertex_indices[ch].second.begin();
       iter_begin = iter;
       iter_end = vv.topo_anchor().vertex_indices[ch].second.end();
    }

    void increment() { ++iter; }
    void decrement() { --iter; }
    cell_type dereference()  const { return cell_type(topo_anchor(), *iter + topo_anchor().get_cell_index_offset()); }
    bool equal(self const& other) const 
    {return iter == other.iter;}

    bool valid()        const { return iter != iter_end; }

    TopologyT const&   topo_anchor()  const { return v.topo_anchor();}
    base_type const&   base()         const { return v;}
    void reset() { iter = iter_begin;}
     self end()  { self new_object(*this); new_object.iter = new_object.iter_end; return new_object; }

  private:
    vertex_type                   v;
    typename TopologyT::cell_reference_container::iterator iter;
    typename TopologyT::cell_reference_container::iterator iter_begin;
    typename TopologyT::cell_reference_container::iterator iter_end;
  };


}  // end of namespace::detail_topology

}  // end of namespace::gsse

#endif
