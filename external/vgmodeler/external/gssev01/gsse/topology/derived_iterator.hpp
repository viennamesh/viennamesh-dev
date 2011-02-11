/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_DERIVED_ITERATOR_HH_ID
#define GSSE_DERIVED_ITERATOR_HH_ID 

// *** GSSE includes
// 
#include "gsse/topology.hpp"
#include "gsse/math/power.hpp"

// *** BOOST includes
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_categories.hpp>
#include "boost/tuple/tuple.hpp"

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

  //
  // The following iterators make use of the base iterators of the
  // topology: cell on vertex and vertex on cell. These iterators
  // apply an archetype concept to the base iterators and provide the
  // face on cell as well as vertex on face access. 

/// @brief Common edge on cell iterator
///
/// This iterator is specialized within all topologies and dimensions
/// on the particular topology type This is only a wrapper iterator
/// for the tasks, derived from boost::iterator_facade
///
/// TODO:: check all iterator traits
/// TODO:: for structured topology a simple permutation does not help !!!!!
///        instead of this, we have to forbid some permutations
/// TODO:: Insert the archetype concept
///

template <typename TopologyT>
class edge_on_cell_iterator 
  : public boost::iterator_facade<
        edge_on_cell_iterator<TopologyT>             
      , typename topology_traits<TopologyT>::edge_type    
      , boost::random_access_traversal_tag           
      , typename topology_traits<TopologyT>::edge_type    
      , unsigned int
    >
{
   typedef edge_on_cell_iterator                                   self;
   typedef int                                                     counter_t;
   typedef typename topology_traits<TopologyT>::edge_type::edge_handle  edge_representation;
   typedef typename topology_traits<TopologyT>::vertex_handle      vertex_handle;
   typedef typename topology_traits<TopologyT>::edge_type          edge_type;
   typedef typename topology_traits<TopologyT>::cell_type          cell_type;

public:
  
  typedef cell_type base_type;
  
  edge_on_cell_iterator() : mc(-1) {}
  explicit
  edge_on_cell_iterator(cell_type const& ce) : ce(ce), mc(0) 
    {
    typename topology_traits<TopologyT>::vertex_on_cell_iterator vocit (ce);
    while (vocit.valid())
      {
	vertexhandle_v.push_back((*vocit).handle());
	++vocit;
      }
    
    // Here, the archetype concept becomes relevant. Using simplices
    // we insert all possible combinations. [MS]
    
    for (unsigned int i2 = 0; i2 < vertexhandle_v.size(); ++i2)
      {
	for (unsigned int i = i2+1; i < vertexhandle_v.size(); ++i)
	  {
	    edgerep_v.push_back(edge_representation(vertexhandle_v[i], vertexhandle_v[i2] ));
	  }
      }
    
    size = edgerep_v.size();
  }
  
  friend class boost::iterator_core_access;
  
  void increment() { ++mc; }
  void decrement() { --mc; }
  void advance(int n) {mc+=n;}    
  bool equal(const self& other) const 
      { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= size); }

  unsigned int distance_to(self const& other) const
      { return mc - other.mc; }
  
  edge_type dereference()  const 
      { 
         if (mc < size) 
	   return edge_type(topo_anchor(),  
			    boost::tuples::get<0>(edgerep_v[mc]),
			    boost::tuples::get<1>(edgerep_v[mc]) );
         else return edge_type();
      } 
  
  bool valid()      const { return (mc < size); }
  void reset() {mc = 0;}

   self end()  { self new_object(*this); new_object.mc = size; return new_object; }


  typename topology_traits<TopologyT>::topology_t     const& topo_anchor() 
    const { return ce.topo_anchor();}

  base_type const& base() const { return ce;}

private:
  cell_type     ce;
  counter_t     mc;
  int           size;
  
  std::vector<edge_representation>   edgerep_v;   
  std::vector<vertex_handle>         vertexhandle_v;
};



///////////////////////////////////////////////////////////////////////////
/// @brief Common facet on cell iterator
///
/// This iterator is specialized within all topologies and dimensions on the particular topology type
/// This is only a wrapper iterator for the tasks, derived from boost::iterator_facade
///
/// TODO:: check all iterator traits, archetype concept
///
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class facet_on_cell_iterator 
  : public boost::iterator_facade<
        facet_on_cell_iterator<TopologyT>            
      , typename topology_traits<TopologyT>::facet_type   
      , boost::bidirectional_traversal_tag           
      , typename topology_traits<TopologyT>::facet_type   
      >
{
   typedef facet_on_cell_iterator<TopologyT>                       self;
   typedef int                                                     counter_t;

   typedef typename topology_traits<TopologyT>::vertex_handle      vertex_handle;
   typedef typename topology_traits<TopologyT>::cell_handle        cell_handle;
   typedef typename topology_traits<TopologyT>::facet_handle       facet_handle;

   typedef typename topology_traits<TopologyT>::facet_type         facet_type;
   typedef typename topology_traits<TopologyT>::cell_type          cell_type;
   typedef typename topology_traits<TopologyT>::vertex_type        vertex_type;
  
public:

  typedef cell_type  base_type;

  facet_on_cell_iterator() : mc(-1) {}
  explicit
  facet_on_cell_iterator(cell_type const& ce) : ce(ce), mc(0) 
  {
//      std::cout << "facet on cell iterator.. 1.. " << std::endl;
    size = 0;
    typename topology_traits<TopologyT>::vertex_on_cell_iterator vocit(ce);
    while (vocit.valid())
    {

       facet_type my_facet(ce.topo_anchor(), ce, *vocit);
       facets.push_back(my_facet);
       ++vocit;
       ++size;
    }  
    std::unique(facets.begin(), facets.end());
  }

  friend class boost::iterator_core_access;
  
  void increment() {  ++mc; }
  void decrement() { --mc; }
  void advance(int n) {mc+=n;} 

   bool               valid()       const { return (mc < size); }
   TopologyT   const& topo_anchor() const { return ce.topo_anchor();}
   base_type   const& base()        const { return ce;}
   
   bool equal(const self& other) const 
      { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= size); }
   facet_type dereference()  const 
      {
//          std::cout << "VALID" << (mc < size) << std::endl;
//          std::cout <<  "facets[mc]" << facets[mc] << std::endl;

         if (mc < size) return facets[mc]; else return facet_type();
      } 
   void reset() {mc = 0;}

   self end()  { self new_object(*this); new_object.mc = size; return new_object; }


private:
   cell_type               ce;
   counter_t               mc;
   int                     size;
   std::vector<facet_type> facets;
};


///////////////////////////////////////////////////////////////////////////
/// @brief Common vertex on edge iterator
///
/// This iterator is specialized within all topologies and dimensions
/// on the particular topology type This is only a wrapper iterator
/// for the tasks, derived from boost::iterator_facade
///
/// TODO:: check all iterator traits, handle mapping, archetype
///
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class vertex_on_edge_iterator 
  : public boost::iterator_facade<
  vertex_on_edge_iterator<TopologyT>            
  , typename topology_traits<TopologyT>::vertex_type   
  , boost::random_access_traversal_tag          
  , typename topology_traits<TopologyT>::vertex_type   
  >
{
   typedef vertex_on_edge_iterator                             self;
   typedef int                                                 counter_t;
   typedef typename topology_traits<TopologyT>::vertex_handle  vertex_handle;
   typedef typename topology_traits<TopologyT>::edge_type      edge_type;
   typedef typename topology_traits<TopologyT>::vertex_type    vertex_type;
   
public:

  typedef edge_type   base_type;

  vertex_on_edge_iterator() : mc(-1) {}
  explicit
  vertex_on_edge_iterator(edge_type const& ed) : ed(ed), mc(0) 
  {
    vertexhandle_v.push_back(ed.handle1());
    vertexhandle_v.push_back(ed.handle2());
    size = vertexhandle_v.size();
  }

  friend class boost::iterator_core_access;

  void increment() { ++mc; }
  void decrement() { --mc; }
  void advance(int n) {mc+=n;}  

  unsigned int distance_to(self const& other) const {return mc - other.mc;}

  bool equal(const self& other) const 
      { return ( mc >= size || mc == other.mc) ;} //&&   topo_anchor() == other.topo_anchor())  ); }

  vertex_type dereference() const
  {
    if (mc < size) 
      return vertex_type(topo_anchor(), vertexhandle_v[mc]); 
    else return vertex_type();
  } 

  bool valid() const 
  { return (mc < size); }
     
  TopologyT  const& topo_anchor() const { return ed.topo_anchor();}
  base_type  const& base()        const { return ed;}
    void reset() {mc = 0;}
   self end()  { mc = size; return (*this); }


private:
   edge_type                  ed;
   counter_t                  mc;
   int                        size;
   std::vector<vertex_handle> vertexhandle_v;

};




///////////////////////////////////////////////////////////////////////////
/// @brief Common vertex on facet iterator
///
/// This iterator is specialized within all topologies and dimensions on the particular topology type
/// This is only a wrapper iterator for the tasks, derived from boost::iterator_facade
///
/// TODO:: check all iterator traits, handle mapping
///
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class vertex_on_facet_iterator 
  : public boost::iterator_facade<
        vertex_on_facet_iterator<TopologyT>        
      , typename topology_traits<TopologyT>::vertex_type
      , boost::random_access_traversal_tag         
      , typename topology_traits<TopologyT>::vertex_type
      , unsigned int 
    >
{
   typedef vertex_on_facet_iterator<TopologyT>    self;
 

   typedef typename topology_traits<TopologyT>::vertex_handle      vertex_handle;
   typedef typename topology_traits<TopologyT>::facet_handle       facet_handle;
   typedef typename topology_traits<TopologyT>::facet_type         facet_type;      
   typedef typename topology_traits<TopologyT>::vertex_type        vertex_type;
   typedef typename std::vector<vertex_handle>::iterator           counter_t;

public:
  typedef facet_type   base_type;

  vertex_on_facet_iterator() : mc() {}

  explicit
  vertex_on_facet_iterator(facet_type const& fa) : fa(fa)
  {
    std::copy(fa.handles.begin(), fa.handles.end(), std::back_inserter(interior_copy));
    mc = interior_copy.begin();  
  }

  friend class boost::iterator_core_access;

  void increment() { ++mc; }
  void decrement() { --mc; }
  void advance(unsigned int n) {mc+=n;}    
  unsigned int distance_to(self const& other) const
  { return mc - other.mc; }

  bool equal(const self& other) const 
  { return ( (mc == other.mc && topo_anchor() == other.topo_anchor())); }

   vertex_type       dereference() const { return vertex_type(fa.topo_anchor(), *mc); } 
  bool              valid()       const { return mc != interior_copy.end(); }
  TopologyT  const& topo_anchor() const { return fa.topo_anchor();}
  base_type const&  base()   const { return fa;}
  void reset() {mc = interior_copy.begin();}

   self end()  { self new_object(*this); new_object.mc = interior_copy.end(); return new_object; }


private:

   facet_type                  fa;
   counter_t                   mc;
   int                         size;
   std::vector<vertex_handle>  interior_copy;
  
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


    // The next iterators are completely derived from other iterators
    // and may not use any internal structure of the topology. If we
    // only use the interface this is very orthogonal. 

template <typename TopologyT>
class edge_on_vertex_iterator
  : public boost::iterator_facade<
  edge_on_vertex_iterator<TopologyT>        
  , typename topology_traits<TopologyT>::edge_type
  , boost::bidirectional_traversal_tag         
  , typename topology_traits<TopologyT>::edge_type
  >
{
  typedef edge_on_vertex_iterator<TopologyT>                      self;
  typedef int                                                     counter_t;
  typedef typename topology_traits<TopologyT>::vertex_handle      vertex_handle;
  typedef typename topology_traits<TopologyT>::edge_type          edge_type;
  typedef typename topology_traits<TopologyT>::vertex_type        vertex_type;


  edge_type make_the_edge(edge_type const& edge_, vertex_type const& ve) 
  {
    return edge_type
      (edge_.topo_anchor(), 
       ve.handle(), 
       ve.handle() == edge_.handle1() ? edge_.handle2() : edge_.handle1() );
  }
	
public:

  typedef vertex_type base_type;

   edge_on_vertex_iterator() : mc(-1) {}

   explicit
   edge_on_vertex_iterator(vertex_type const& ve) : ve(ve), mc(0)
      {         
//	std::cout << "######### edge on vertex iterator.. " << std::endl;

         //  -- 1) all cells on a point
         typename topology_traits<TopologyT>::cell_on_vertex_iterator covit(ve); 
         while ( covit.valid() )
         {
            //  -- 2) all edges on cells
            typename topology_traits<TopologyT>::edge_on_cell_iterator eocit(*covit);   
            while ( eocit.valid() )
            {
               edge_s.insert( *eocit );
               ++eocit;
            }
            ++covit;
         }

	 // -------------------------------------------------
	 // [MS] DO NOT MAKE USE OF TYPE SPECIFIC FEATURES!?!

         // -- 3) delete all edges except edges, with the vertex ve

         for (sit = edge_s.begin(); sit != edge_s.end(); sit++)
         {
//            std::cout << ".....  constructor: edge: " << (*sit) << std::endl;
            if (ve.handle() == (*sit).handle1() || ve.handle() == (*sit).handle2())
	      edge_v.push_back(make_the_edge(*sit,ve));
         }

	 // -------------------------------------------------

         // -- 4) store size
         size = edge_v.size();
//         std::cout << ".....  constructor: edge size: " << size  << std::endl;
      }

  friend class boost::iterator_core_access;

  edge_type dereference()  const { return edge_v[mc];}

   void increment()    { ++mc; }
  void decrement()     { --mc; }
  void advance(unsigned int i)    { mc += i;}

  unsigned int distance_to(self const& other) const
  { return mc - other.mc; }

  bool equal(self const& other) const 
      {return mc == other.mc;} // && topo_anchor() == other.topo_anchor();}

  bool valid()  const {  return (mc < static_cast<int>(size) ); }

   self end()  { mc = static_cast<int>(size); return (*this); }


  TopologyT    const& topo_anchor() const { return ve.topo_anchor();}
  base_type    const& base()        const { return ve;}
  void reset() {mc = 0;}

private:
  vertex_type                            ve;
  counter_t                              mc;
  unsigned int                           size;
  std::set<edge_type>                    edge_s;   
  typename std::set<edge_type>::iterator sit;
  std::vector<edge_type>                 edge_v;   

};




///////////////////////////////////////////////////////////////////////////
/// @brief Common cell on edge iterator
///
/// This iterator is specialized within all topologies and dimensions
/// on the particular topology type This is only a wrapper iterator
/// for the tasks, derived from boost::iterator_facade
///
/// TODO:: check all iterator traits
///
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class cell_on_edge_iterator 
  : public boost::iterator_facade<
        cell_on_edge_iterator<TopologyT>              
      , typename topology_traits<TopologyT>::cell_type     
      , boost::bidirectional_traversal_tag            
      , typename topology_traits<TopologyT>::cell_type     
    >
{
  typedef cell_on_edge_iterator                                   self;
  typedef int                                                     counter_t;
  typedef typename topology_traits<TopologyT>::vertex_handle      vertex_handle;
  typedef typename topology_traits<TopologyT>::cell_handle        cell_handle;
  typedef typename topology_traits<TopologyT>::edge_type          edge_type;
  typedef typename topology_traits<TopologyT>::cell_type          cell_type;
  typedef typename topology_traits<TopologyT>::vertex_type        vertex_type;
  
public:

  typedef edge_type base_type;
  
  cell_on_edge_iterator() : mc(-1) {}
  explicit
  cell_on_edge_iterator(edge_type const& ed) : ed(ed), mc(0) 
  {
    
    // Change this!!!
    vertex_type V1 = ed.vertex1();
    vertex_type V2 = ed.vertex2();
    
    typename topology_traits<TopologyT>::cell_on_vertex_iterator CoV(V1);
    
    while(CoV.valid())
      {
	bool take = false;
	typename topology_traits<TopologyT>::vertex_on_cell_iterator VoC(*CoV);
	
	while(VoC.valid())
            {
	      if ((*VoC).handle() == V2.handle())
		{
                  take = true;
		}
	      VoC++;
            }
	if (take) 
	  {
	    cellhandle_v.push_back((*CoV).handle());
	  }
	++CoV;
      }
    
    size = cellhandle_v.size();
  }
  
  friend class boost::iterator_core_access;

  void increment() { ++mc; }
  void decrement() { --mc; }
  void advance(int n) {mc+=n;}     

  bool equal(const self& other) const 
  { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= size); }
  
  cell_type dereference()  const 
  { if (mc < size) return cell_type(topo_anchor(), cellhandle_v[mc]); else return cell_type();} 

  bool valid()   const { return (mc < size); }
  TopologyT const& topo_anchor() const { return ed.topo_anchor();}
  base_type const& base()        const { return ed;}
  void reset() {mc = 0;}

   self end()  { self new_object(*this); new_object.mc = size; return new_object; }


private:
  edge_type                  ed;
  counter_t                  mc;
  int                        size;
  std::vector<cell_handle>   cellhandle_v;
  
};


template <typename TopologyT>
class cell_on_facet_iterator 
  : public boost::iterator_facade<
  cell_on_facet_iterator<TopologyT>            
  , typename topology_traits<TopologyT>::cell_type  
  , boost::bidirectional_traversal_tag         
  , typename topology_traits<TopologyT>::cell_type  
  >
{
  typedef cell_on_facet_iterator                                        self;
  typedef int                                                           counter_t;
  typedef typename topology_traits<TopologyT>::vertex_handle            vertex_handle;
  typedef typename topology_traits<TopologyT>::cell_type                cell_type;
  typedef typename topology_traits<TopologyT>::facet_type               facet_type;
  typedef typename topology_traits<TopologyT>::cell_on_vertex_iterator  cell_on_vertex_iterator;
  typedef typename topology_traits<TopologyT>::vertex_on_facet_iterator vertex_on_facet_iterator;
  typedef typename topology_traits<TopologyT>::facet_on_cell_iterator   facet_on_cell_iterator;


public:
  typedef facet_type base_type;

public:

  cell_on_facet_iterator()  {}

  explicit
  cell_on_facet_iterator(facet_type const& fa) : fa(fa) 
  {
    vertex_on_facet_iterator vofit(fa);

    while(vofit.valid())
      {
	cell_on_vertex_iterator covit(*vofit);
	
	while(covit.valid())
	  {
	    facet_on_cell_iterator focit(*covit);
	    while(focit.valid())
	      {
		if ((*focit).handle() == fa.handle()) 
		  cells.insert(*covit);

                //	std::cout << *vofit << "  " << *covit << "  " << *focit << std::endl;

		++focit;
	      }
	    ++covit;
	  }
	++vofit;
      }

    iter = cells.begin();
//    std::cout << "\t ctr:: size: "<< cells.size() << std::endl;
  }
   friend class boost::iterator_core_access;
   
   void increment() { ++iter;    } // std::cout << "  cof inc. op. " << std::endl; }
   void decrement() { --iter; }
   bool equal(const self& other) const 
      { 
         return ( (iter == other.iter) ) ; } //  && ( topo_anchor() == other.topo_anchor()) ); }
   
   cell_type dereference()  const { return *iter;}
   bool      valid()        const { return (iter !=cells.end()); }
   
   TopologyT const& topo_anchor() const { return fa.topo_anchor();}
   base_type const& base()        const { return fa;}
   void reset() { iter = cells.begin();}
   self end()   { self new_object(*this); new_object.iter = cells.end(); return new_object; }
   
   
private:
  facet_type    fa;

   std::set<cell_type> cells;
   typename std::set<cell_type>::iterator iter;
};




template <typename TopologyT>
class vertex_on_vertex_iterator 
  : public boost::iterator_facade<
  vertex_on_vertex_iterator<TopologyT>            
  , typename topology_traits<TopologyT>::vertex_type  
  , boost::random_access_traversal_tag         
  , typename topology_traits<TopologyT>::vertex_type  
  , unsigned int
  >
{
  typedef vertex_on_vertex_iterator                                     self;
  typedef int                                                           counter_t;
  typedef typename topology_traits<TopologyT>::vertex_type              vertex_type;

public:
  typedef vertex_type base_type;

public:

   vertex_on_vertex_iterator()  {mc = 0;}

  explicit
  vertex_on_vertex_iterator(vertex_type const& v) : v(v) {mc = 0;}

  friend class boost::iterator_core_access;

  void increment() { ++mc; }
  void decrement() { --mc; }

  unsigned int distance_to (self const& other) const {return mc - other.mc;}
  void advance (unsigned int const i) {mc += i;}

  bool equal(const self& other) const 
  { return ( ( topo_anchor() == other.topo_anchor()) || mc == 0); }

  vertex_type dereference()  const { return v;}
  bool      valid()          const { return (mc == 0); }

   self end()  { self new_object(*this); new_object.mc = 0; return new_object; }    // [RH][TODO] .. check this again ?? 0 ?? 

  TopologyT const& topo_anchor() const { return v.topo_anchor();}
  base_type const& base()        const { return v;}
  void reset()                         { mc = 0;}

private:

  vertex_type v;
  unsigned int mc;
};


  // purely derived

template <typename BaseT>
class facet_on_edge_iterator : 
   public boost::iterator_facade<
   facet_on_edge_iterator<BaseT>                
   , typename topology_traits<BaseT>::facet_type
   , boost::bidirectional_traversal_tag         
   , typename topology_traits<BaseT>::facet_type     
   >
{
   typedef facet_on_edge_iterator<BaseT> self;

   typedef typename topology_traits<BaseT>::vertex_on_facet_iterator   vertex_on_facet_iterator;
   typedef typename topology_traits<BaseT>::vertex_on_edge_iterator    vertex_on_edge_iterator;
   typedef typename topology_traits<BaseT>::cell_on_vertex_iterator    cell_on_vertex_iterator;
   typedef typename topology_traits<BaseT>::facet_on_cell_iterator     facet_on_cell_iterator;

   typedef typename topology_traits<BaseT>::edge_type   edge_type;
   typedef typename topology_traits<BaseT>::facet_type  facet_type;
   typedef typename topology_traits<BaseT>::vertex_type vertex_type;
   typedef typename topology_traits<BaseT>::cell_type   cell_type;

public:

  typedef edge_type base_type;

   facet_on_edge_iterator()
      {
         iter = results.begin();        
      }

   facet_on_edge_iterator(edge_type const& e)
      {
	//std::cout << "EDGE_FACET(" << e.handle() << ") : " << std::endl;

         ed = e;
         
         vertex_on_edge_iterator voeit(e);
         
         std::set<vertex_type> vertices;
         std::set<facet_type>  facets;

         // get all vertices of the edge

	 //std::cout << "VERTICES ON THE EDGE" << std::endl;

         while(voeit.valid())
         {
	   vertices.insert(*voeit);
	   ++voeit;
         }

         // get all possible facets from the patch

         voeit = vertex_on_edge_iterator(e);

         while(voeit.valid())
         {
            cell_on_vertex_iterator covit(*voeit);
            while(covit.valid())
            {
               facet_on_cell_iterator focit(*covit);
               while(focit.valid())
               {
                  facets.insert(*focit);
                  ++focit;
               }
               ++covit;
            }
            ++voeit;
         }

         typename std::set<facet_type>::iterator iter1;     // all facets
         typename std::set<vertex_type>::iterator iter2;   // the edge vertices

         for (iter1 = facets.begin(); iter1 != facets.end(); ++iter1)
         {
            bool found = true;

            for (iter2 = vertices.begin(); iter2 != vertices.end(); ++iter2)
            {
               bool found_vertex = false;
               
               vertex_on_facet_iterator vofit(*iter1);
               while(vofit.valid())
               {
		 // the vertex of the edge was found on the facet

		 if ((*vofit) == *iter2) found_vertex = true;  
                  ++vofit;
               }
               if (!found_vertex) 
               {
                  found = false;
                  break;
               }
            }
            if (found) 
            {
               results.insert(*iter1);
	       //std::cout << *iter1 << std::endl;
            }
         }
         iter = results.begin();
	 //std::cout << "FACETS ON EDGE: " << results.size() << std::endl;
	 mc = 0;
      }

  friend class boost::iterator_core_access;
  
  void increment() { ++iter; ++mc;}
  void decrement() { --iter; --mc;}
  
  bool equal(const self& other)  const   { return iter == other.iter ; }
  
  facet_type dereference()       const   { return *iter; }
  bool valid()                   const   
  {
    //    std::cout << (mc < results.size()) << std::endl;
    return mc < results.size(); 
  }
  void reset()                           { iter = results.begin();mc = 0; }
  
   self end()  { self new_object(*this); new_object.mc = results.size(); return new_object; }   


  BaseT     const& topo_anchor() const   { return ed.topo_anchor();}
  base_type const& base()        const   { return ed; }

private:

  std::set<facet_type> results;
  edge_type ed;
  typename std::set<facet_type>::iterator iter;
  unsigned int mc;

};

  // purely derived

template <typename BaseT>
class edge_on_facet_iterator : 
   public boost::iterator_facade<
   edge_on_facet_iterator<BaseT>          
   , typename topology_traits<BaseT>::edge_type
   , boost::bidirectional_traversal_tag   
   , typename topology_traits<BaseT>::edge_type
   >
{
   typedef edge_on_facet_iterator<BaseT> self;

   typedef typename topology_traits<BaseT>::vertex_on_facet_iterator   vertex_on_facet_iterator;
   typedef typename topology_traits<BaseT>::vertex_on_edge_iterator    vertex_on_edge_iterator;
   typedef typename topology_traits<BaseT>::cell_on_vertex_iterator    cell_on_vertex_iterator;
   typedef typename topology_traits<BaseT>::facet_on_cell_iterator     facet_on_cell_iterator;
   typedef typename topology_traits<BaseT>::edge_on_cell_iterator      edge_on_cell_iterator;

   typedef typename topology_traits<BaseT>::edge_type   edge_type;
   typedef typename topology_traits<BaseT>::facet_type  facet_type;
   typedef typename topology_traits<BaseT>::vertex_type vertex_type;
   typedef typename topology_traits<BaseT>::cell_type   cell_type;



public:

  typedef facet_type base_type;

  edge_on_facet_iterator() {}

  edge_on_facet_iterator(facet_type const& f)
  {
    //std::cout << "FACET_EDGE(" << f.handle() << ") : " << std::endl;

         iter = results.begin();

         fa = f;
         
         vertex_on_facet_iterator vofit(f);

         std::set<vertex_type> vertices;
         std::set<edge_type>  edges;

         // get all vertices of the edge

         while(vofit.valid())
         {
            //std::cout << *vofit << std::endl;
            vertices.insert(*vofit);
            ++vofit;
         }

         // get all possible edges from the patch

	 // better would be: vofit.reset(), but at the moment this is
	 // not available in the concept def. [MS]

         vertex_on_facet_iterator vofit2(f);

	 int i = 0;

         while(vofit2.valid())
         {
            cell_on_vertex_iterator covit(*vofit2);
            while(covit.valid())
            {
               //std::cout << *vofit << std::endl;

               edge_on_cell_iterator eocit(*covit);
               while(eocit.valid())
               {
                  edges.insert(*eocit);
		  //std::cout << *eocit << std::endl;
                  ++eocit;
               }
               ++covit;
            }
	    ++i;
            ++vofit2;
         }

         typename std::set<edge_type>::iterator eit;       // all edges
         typename std::set<vertex_type>::iterator iter2;   // the facet vertices

         for (eit = edges.begin(); eit != edges.end(); ++eit)
         {
            bool found = true;

            vertex_on_edge_iterator vofit2(*eit);
            while(vofit2.valid())
            {
               bool found_vertex = false;
	       
               for (iter2 = vertices.begin(); iter2 != vertices.end(); ++iter2)
		 {
		   if (*vofit2 == *iter2) found_vertex = true; 
		 }
	       
               if (!found_vertex) 
		 {
		   found = false;
		   break;
		 }
               ++vofit2;
	    }
            if (found) 
	      {
		results.push_back(*eit);
		//std::cout << (*eit).handle() << std::endl;
	      }

         }

         std::unique(results.begin(), results.end());
	 
         //std::cout << "EDGES ON FACET: " << results.size() << std::endl;

         iter = results.begin();
         mc = 0;
      }

   friend class boost::iterator_core_access;
      
   void increment() { ++iter;++mc;}
   void decrement() { --iter;++mc;}
   
   bool equal(const self& other)  const 
      { return ( (mc == other.mc && topo_anchor() == other.topo_anchor())); }
   
  edge_type dereference()    const 
      { 
         //std::cout << "DEREF " << *iter << std::endl;
         //std::cout << "VALID " << valid() << std::endl;
         //std::cout << "SIZE "  << results.size() << std::endl;
         //std::cout << "POS "   << mc << std::endl;
         //std::cout << "RES "   << results[mc] << std::endl;
         return results[mc]; 
      }

//   bool valid()               const { return iter != results.end(); }
//   void reset()                     { iter = results.begin();} 

   bool valid()               const { return mc < results.size(); }
   void reset()                     { mc = 0;} 
   self end()  { self new_object(*this); new_object.mc = results.size(); return new_object; }   
   

  BaseT const& topo_anchor() const { return fa.topo_anchor();}
  base_type const& base()    const { return fa; }

private:
   std::vector<edge_type> results;
   facet_type fa;
   typename std::vector<edge_type>::iterator iter;
   unsigned int mc;
};

template <typename BaseT>
class facet_on_vertex_iterator : 
    public boost::iterator_facade<
  facet_on_vertex_iterator<BaseT>                
  , typename topology_traits<BaseT>::facet_type       
  , boost::random_access_traversal_tag           
  , typename topology_traits<BaseT>::facet_type       
  , unsigned int
   >
{
   typedef facet_on_vertex_iterator<BaseT> self;

   typedef typename topology_traits<BaseT>::vertex_on_facet_iterator   vertex_on_facet_iterator;
   typedef typename topology_traits<BaseT>::vertex_on_edge_iterator    vertex_on_edge_iterator;
   typedef typename topology_traits<BaseT>::cell_on_vertex_iterator    cell_on_vertex_iterator;
   typedef typename topology_traits<BaseT>::facet_on_cell_iterator     facet_on_cell_iterator;

   typedef typename topology_traits<BaseT>::edge_type   edge_type;
   typedef typename topology_traits<BaseT>::facet_type  facet_type;
   typedef typename topology_traits<BaseT>::vertex_type vertex_type;
   typedef typename topology_traits<BaseT>::cell_type   cell_type;


public:

  typedef vertex_type base_type;
  
  facet_on_vertex_iterator() {}

  facet_on_vertex_iterator(vertex_type const& v)
  {
    ve = v;
    
    std::set<facet_type>  facets;
    
    cell_on_vertex_iterator covit(v);
    while(covit.valid())
      {
	facet_on_cell_iterator focit(*covit);
	while(focit.valid())
	  {
	    facets.insert(*focit);
	    ++focit;
	  }
	++covit;
      }
    
    typename std::set<facet_type>::iterator iter1;     // all facets
    
    for (iter1 = facets.begin(); iter1 != facets.end(); ++iter1)
      {
	bool found = false;
	
	vertex_on_facet_iterator vofit(*iter1);
	while(vofit.valid())
	  {
	    if ((*vofit) == ve) found = true; 
	    ++vofit;
	  }
	
	if (found) results.insert(*iter1);
      }
    iter = results.begin();
  }
  
  friend class boost::iterator_core_access;
  
  void increment() { ++iter; }
  void decrement() { --iter; }
  
  void advance(unsigned int i)    { iter += i;}

  unsigned int distance_to(self const& other) const
  { return iter - other.iter; }

  bool equal(const self& other)  const 
  { return ( (iter == other.iter && topo_anchor() == other.topo_anchor())); }
  
   facet_type dereference()        const 
      {
         return *iter; 
      }
  bool valid()                    const { return iter != results.end(); }
  BaseT const& topo_anchor()      const { return ve.topo_anchor();}
  base_type const& base()         const { return ve; }
  void reset()                          { iter = results.begin(); }
  
   self end()  { self new_object(*this); new_object.iter = results.end(); return new_object; }   

private:
  std::set<facet_type> results;
  vertex_type ve;
  typename std::set<facet_type>::iterator iter;
};

}  // end of namespace::detail_topology

}  // end of namespace::gsse

#endif
