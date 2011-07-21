/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_STRUCTURED_ITERATOR_HH_ID
#define GSSE_STRUCTURED_ITERATOR_HH_ID 

// *** GSSE includes
//
#include "gsse/topology.hpp"
#include "gsse/math/power.hpp"

// *** BOOST includes
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/array.hpp>

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
/// @brief structured vertex iterator class
///
///
/// TODO:: check all iterator traits
///
/// Concepts: Bidirectional iterator
///
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class structured_vertex_iterator
   : public boost::iterator_facade<
         structured_vertex_iterator<TopologyT>          
       , typename topology_traits<TopologyT>::vertex_type    
       , boost::bidirectional_traversal_tag             
       , typename topology_traits<TopologyT>::vertex_type    
     >
{
  typedef structured_vertex_iterator<TopologyT>  self;
  typedef int                                    counter_t;

  typedef typename topology_traits<TopologyT>::vertex_type        vertex_type;
  typedef typename topology_traits<TopologyT>::vertex_handle      vertex_handle;
  
public:
   typedef TopologyT         base_type;
   typedef vertex_handle     my_handle;

	
public:
   structured_vertex_iterator() : g(0), mc(-1) {}
   explicit
   structured_vertex_iterator(TopologyT const& gg) : g(&gg), mc(0), size(g->get_vertex_size_internal()) 
      {
//          std::cout << "internal: " <<  g->get_vertex_size_internal()  << std::endl;
      }
   structured_vertex_iterator(TopologyT const& gg, int vv) : g(&gg), mc(vv), size(g->get_vertex_size_internal()) {}

   friend class boost::iterator_core_access;

   void increment() { ++mc; }
   void decrement() { --mc; }
   void advance(int n) {mc+=n;}    // random access 
  //bool equal(const self& other) const { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())) ;}// || mc >= size); }

   // warning:!! only for hp approaches
   bool equal(const self& other) const { return ( mc >= size); }

  vertex_type dereference()  const { if (mc < size) return vertex_type(*g, mc); else return vertex_type();} 
  bool valid()               const { return (mc < size); }
  void reset()                     {mc = 0;}

   self end()  { self new_object(*this); new_object.mc = size; return new_object; }


  TopologyT const&           base() const { return *g; }
  TopologyT const&           topo_anchor() const { return *g; }
  
  friend std::ostream& operator<<(std::ostream& ostr, const structured_vertex_iterator<TopologyT>& mv)
  { ostr << "iterator counter: " << mv.mc ; return ostr; }

private:
   TopologyT const*  g;
   counter_t         mc;
   int               size;

};


///////////////////////////////////////////////////////////////////////////
/// @brief Structured cell iterator class
///
///
/// TODO:: check all iterator traits
///
/// Concepts: Bidirectional iterator
///
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class structured_cell_iterator
   : public boost::iterator_facade<
         structured_cell_iterator<TopologyT>           
       , typename topology_traits<TopologyT>::cell_type
       , boost::bidirectional_traversal_tag            
       , typename topology_traits<TopologyT>::cell_type     
     >
{
   typedef structured_cell_iterator<TopologyT>                   self;
   typedef int                                                   counter_t;
   typedef typename topology_traits<TopologyT>::cell_type        cell_type;
   typedef typename topology_traits<TopologyT>::cell_handle      cell_handle;

public:
   typedef TopologyT         base_type;

	
public:
   structured_cell_iterator() : g(0), mc(-1) {}
   explicit
   structured_cell_iterator(TopologyT const& gg) : g(&gg), mc(0), size(g->get_cell_size_internal()) {}
   structured_cell_iterator(TopologyT const& gg, int vv) : g(&gg), mc(vv),size(g->get_cell_size_internal()) {}
  friend class boost::iterator_core_access;
  
  void increment() { ++mc; }
  void decrement() { --mc; }
  void advance(int n) {mc+=n;}   
  bool equal(const self& other) const 
      { return ( (mc == other.mc && topo_anchor() == other.topo_anchor())  
                 ||  mc >= size  ); }
  
  cell_type dereference() const 
      { if (mc < size) return cell_type(*g, mc); else return cell_type(*g,-1);} 

  bool valid()            const { return (mc < size); }
  void reset() {mc = 0;}
  
   self end()  { self new_object(*this); new_object.mc = size; return new_object; }


  TopologyT const&        topo_anchor() const { return *g; }
  base_type const&        base() const { return *g; }
  
  
  friend std::ostream& operator<<(std::ostream& ostr, const structured_cell_iterator<TopologyT>& mv)
  { ostr << "iterator counter: " << mv.mc ; return ostr; }
  
private:
   TopologyT const*  g;
   counter_t        mc;
   int              size;
};




  ///////////////////////////////////////////////////////////////////////////
   //
   //  part two from base iterators
   //
   ///////////////////////////////////////////////////////////////////////////

   template <unsigned int dimension>
   class structured_vertex_on_cell_iterator 
   : public boost::iterator_facade<
      structured_vertex_on_cell_iterator<dimension>
      , typename topology_traits<structured<dimension> >::vertex_type
      , boost::bidirectional_traversal_tag            
      , typename topology_traits<structured<dimension> >::vertex_type     
     >

   {
      typedef structured_vertex_on_cell_iterator<dimension>       self;
      typedef int                                                 mycounter;

      typedef structured<dimension>                               topology_t;
      typedef typename topology_traits<topology_t>::cell_type     cell_type;
      typedef typename topology_traits<topology_t>::vertex_type   vertex_type;
      typedef typename topology_traits<topology_t>::vertex_handle vertex_handle;


      structured_vertex_on_cell_iterator(cell_type const& cc, mycounter count) 
         : c(cc), mc(count) {}       

   public:
      typedef cell_type   base_type;

   public:
      structured_vertex_on_cell_iterator() : mc(-1) {}
      explicit
      structured_vertex_on_cell_iterator(cell_type const& cc) : c(cc), mc(0) 
         {
            vertex_handle_vector.clear();

            vertex_handle_vector.reserve(power<2, dimension>::value);

            boost::array<long, dimension> indices;

            indices = c.topo_anchor().get_cell_index(cc);
            vertex_type v = c.topo_anchor().get_index_vertex(indices);

            vertex_handle_vector.push_back(v.handle());

            typename std::vector<vertex_handle>::iterator begin;
            typename std::vector<vertex_handle>::iterator end;

            for (unsigned int i=0; i<dimension; ++i)
            {
               begin = vertex_handle_vector.begin();
               end   = vertex_handle_vector.end();

               for (; begin != end; ++begin)
               {
                  vertex_handle_vector.push_back((*begin) + cc.topo_anchor().pointsdim_cumm[i]);
               }
            }
            number_of_vertices = vertex_handle_vector.size();
         }

      bool equal(const self& S) const { return S.c.handle() == c.handle() && S.mc == mc; }
      void reset()     {mc = 0;}
      void increment() {++mc;}
      void decrement() {--mc;}
      void advance(unsigned int i) { mc += i; }
      unsigned int distance_to(self const& other) const {return mc - other.mc;}
      
      vertex_type dereference() const     
         { return vertex_type(topo_anchor(), vertex_handle_vector[mc]);}
      bool valid()              const     { return (mc < number_of_vertices);}
      
      self end()  { self new_object(*this); new_object.mc = number_of_vertices; return new_object; }


      topology_t const& topo_anchor() const { return c.topo_anchor();}
      base_type const& base()         const { return c;}

  friend class boost::iterator_core_access;

   private:
      cell_type       c;
      mycounter  mc;

      std::vector<vertex_handle> vertex_handle_vector;
      long number_of_vertices;
   };




// -------------------------------------------------------------------------------------
// n-dimension formula
//  vertex dimensional indicies::   V [a1, a2, .. , an]
//  cell dimensional indicies::     C [c1, c2, .. , cn]
//
//   ci = ai   OR    ci = ai - 1 
//
// -------------------------------------------------------------------------------------

   template <unsigned int dimension>
   class structured_cell_on_vertex_iterator 
   : public boost::iterator_facade<
      structured_cell_on_vertex_iterator<dimension>           
       , typename topology_traits<structured<dimension> >::cell_type
       , boost::bidirectional_traversal_tag            
       , typename topology_traits<structured<dimension> >::cell_type     
     >
   
   {
      typedef structured_cell_on_vertex_iterator<dimension>     self;
      typedef int   mycounter;
  
      typedef structured<dimension>                             topology_t;
  
      typedef typename topology_traits<topology_t>::vertex_type vertex_type;
      typedef typename topology_traits<topology_t>::cell_type   cell_type;
      typedef typename topology_traits<topology_t>::cell_handle cell_handle;

   public:
      typedef vertex_type base_type;


      structured_cell_on_vertex_iterator() : mc(-1) {}
      explicit
      structured_cell_on_vertex_iterator(vertex_type const& vv) : v(vv), mc(0) 
         {
            cell_handle_vector.clear();
            cell_handle_vector.reserve(power<2, dimension>::value);

            boost::array<long, dimension> indices;

            indices = v.topo_anchor().get_vertex_index(v);

            // check on which dimensions we are on the boundary
            // and get the base cell


            boost::array<long, dimension> boundary;
            for (unsigned d = 0; d < dimension; ++d)
            {
               boundary[d] = 1;
               if (indices[d] == (v.topo_anchor().get_dim_n_size(d+1)-1))
               {
                  --indices[d];
                  boundary[d] = 2;
               }
               else if (indices[d] == 0)
               {
                  boundary[d] = 0;
               }
            }

            cell_type c = v.topo_anchor().get_index_cell(indices);

            
            cell_handle_vector.push_back(c.handle());

            typename std::vector<cell_handle>::iterator begin;
            typename std::vector<cell_handle>::iterator end;

            for (unsigned int i=0; i<dimension; ++i)
            {
               begin = cell_handle_vector.begin();
               end   = cell_handle_vector.end();


               if (boundary[i] == 1)
               for (; begin != end; ++begin)
               {
                  cell_handle_vector.push_back((*begin) - v.topo_anchor().pointsdim_cumm_cells[i]);

               }
            }

         }

     void increment()              { ++mc;}
     void decrement()              { --mc;}
     cell_type dereference() const  { return cell_type(topo_anchor(), cell_handle_vector[mc]);} 
     bool equal(self const& other) const
     {
       return (mc == other.mc) && (base() == other.base());
     }
     bool valid() const            { return (mc < static_cast<int>(cell_handle_vector.size())); }
     void reset()                          {mc = 0;}
     vertex_type const& base()       const { return v;}
     topology_t const& topo_anchor() const { return v.topo_anchor();}

      self end()  { self new_object(*this); new_object.mc = static_cast<int>(cell_handle_vector.size()); return new_object; }


  friend class boost::iterator_core_access;

   private:

      vertex_type               v;
      mycounter                 mc;
      std::vector<cell_handle>  cell_handle_vector;
   };





///////////////////////////////////////////////////////////////////////////
/// @brief Structured Vertex on Facet Iterator
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class structured_vertex_on_facet_iterator
  : public boost::iterator_facade<
        structured_vertex_on_facet_iterator<TopologyT>  
      , typename topology_traits<TopologyT>::vertex_type       
      , boost::random_access_traversal_tag              
      , typename topology_traits<TopologyT>::vertex_type 
      , unsigned int
    >
{
   typedef structured_vertex_on_facet_iterator<TopologyT>       self;
   typedef int                                                  mycounter;
   typedef typename topology_traits<TopologyT>::vertex_type     vertex_type;
   typedef typename topology_traits<TopologyT>::vertex_handle   vertex_handle;
   typedef typename topology_traits<TopologyT>::facet_type      facet_type;
   typedef typename topology_traits<TopologyT>::facet_handle    facet_handle;
   typedef typename topology_traits<TopologyT>::dimension_tag   dimension_tag;

   static const int size = power<2, dimension_tag::dim - 1>::value;
    
public:
  
  typedef facet_type  base_type;
   
  structured_vertex_on_facet_iterator  () : mc() {}
   
   explicit
   structured_vertex_on_facet_iterator  (facet_type const& f) : f(f), mc(0)
      {
         handles.clear();
         handles.reserve(size);

         handles.push_back(f.base_vertex().handle());

         int prod(1);

         for (int i=1; i<=dimension_tag::dim; ++i)
         {
            typename std::vector<vertex_handle>::iterator beg = handles.begin();
            typename std::vector<vertex_handle>::iterator end = handles.end();

            if (i != f.direction())
               for (;beg != end; ++beg)
               {
                  handles.push_back(*beg + prod);
               }
            
            prod *= f.topo_anchor().get_dim_n_size(i);
         }

         
         mc = handles.begin();
      }
   
   friend class boost::iterator_core_access;
   
   void increment() { ++mc; }
   void decrement() { --mc; }
   void advance(unsigned int i) { mc += i; }
   unsigned int distance_to(self const& other) const {return mc - other.mc;}
   
   bool equal(const self& other) const { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())); }
   
   vertex_type dereference()  const 
      { 
         return vertex_type(f.topo_anchor(), *mc);
      } 
   
   bool valid()  const { return mc != handles.end(); }
   void reset()                     {mc = handles.begin();}

   self end()  { self new_object(*this); new_object.mc = handles.end(); return new_object; }

   
   typename topology_traits<TopologyT>::topology_t const& topo_anchor() const 
      { return f.topo_anchor();}
   
  base_type const& base() const  { return f; }
   
private:
   facet_type                                    f;
   typename std::vector<vertex_handle>::iterator mc;
   std::vector<vertex_handle>                    handles;
};


///////////////////////////////////////////////////////////////////////////
/// @brief Structured facet on cell  Iterator
///
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class structured_facet_on_cell_iterator
  : public boost::iterator_facade<
        structured_facet_on_cell_iterator<TopologyT>      // Derived
      , typename topology_traits<TopologyT>::facet_type   // Base
      , boost::bidirectional_traversal_tag                // CategoryOrTraversal     
      , typename topology_traits<TopologyT>::facet_type   // Base
    >
{
  typedef structured_facet_on_cell_iterator<TopologyT>         self;
  typedef int                                                  mycounter;
  typedef typename topology_traits<TopologyT>::vertex_type     vertex_type;
  typedef typename topology_traits<TopologyT>::cell_type       cell_type;
  typedef typename topology_traits<TopologyT>::vertex_handle   vertex_handle;
  typedef typename topology_traits<TopologyT>::facet_type      facet_type;
  typedef typename topology_traits<TopologyT>::facet_handle    facet_handle;
  typedef typename topology_traits<TopologyT>::dimension_tag   dimension_tag;
  

public:

  typedef cell_type base_type;

  structured_facet_on_cell_iterator  () : mc(-1) {}
  explicit
  structured_facet_on_cell_iterator  (cell_type const& c) : c(c), mc(0) 
  {
    facets.clear();
    
    for (int i = 1; i <= dimension_tag::dim; i++)
      {
	facets.push_back(facet_type(c, i));
	facets.push_back(facet_type(c, -i));
      }
    size = facets.size();
  }
  
  friend class boost::iterator_core_access;

  void increment() { ++mc; }
  void decrement() { --mc; }
  bool equal(const self& other)    const 
  { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= size); }

  facet_type dereference()         const { return facets[mc]; }
  bool valid()                     const { return (mc < size); }
  void reset()                     {mc = 0;}

   self end()  { self new_object(*this); new_object.mc = size; return new_object; }


  TopologyT const& topo_anchor()   const { return c.topo_anchor();}
  base_type const& base()          const { return c; }

private:
  facet_handle const& handle()     const { return c.handle(); }

private:

  cell_type c;
  mycounter mc;
  std::vector<facet_type> facets;
  int size;

};


///////////////////////////////////////////////////////////////////////////
/// @brief Structured Vertex on Edge Iterator
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class structured_vertex_on_edge_iterator
  : public boost::iterator_facade<
        structured_vertex_on_edge_iterator<TopologyT>       // Derived
      , typename topology_traits<TopologyT>::vertex_type    // Base
      , boost::random_access_traversal_tag                  // CategoryOrTraversal     
      , typename topology_traits<TopologyT>::vertex_type    // Base
      , unsigned int
>
{
  typedef structured_vertex_on_edge_iterator<TopologyT>         self;
  typedef int                                                  mycounter;
  typedef typename topology_traits<TopologyT>::vertex_type     vertex_type;
  typedef typename topology_traits<TopologyT>::cell_type       cell_type;
  typedef typename topology_traits<TopologyT>::vertex_handle   vertex_handle;
  typedef typename topology_traits<TopologyT>::facet_type      facet_type;
  typedef typename topology_traits<TopologyT>::edge_type       edge_type;
  typedef typename topology_traits<TopologyT>::facet_handle    facet_handle;
  typedef typename topology_traits<TopologyT>::dimension_tag   dimension_tag;
  

public:

  typedef edge_type   base_type;
  typedef vertex_type value_type;

  structured_vertex_on_edge_iterator  () : mc(-1) {}

  explicit
  structured_vertex_on_edge_iterator  (edge_type const& e) : e(e), mc(0) 
  {
     vertices.reserve(2);

     if ((boost::tuples::get<0>(e.handle()) != -1)  &&
	 (boost::tuples::get<1>(e.handle()) != -1))
      {
         vertices.push_back(vertex_type(e.topo_anchor(),e.handleo1()));
         vertices.push_back(vertex_type(e.topo_anchor(),e.handleo2()));
      }
    size = vertices.size();
    mc = 0;
  }
  
  friend class boost::iterator_core_access;

  void increment() { ++mc; }
  void decrement() { --mc; }
  void advance(unsigned int i) { mc += i; }
  unsigned int distance_to(self const& other) const {return mc - other.mc;}

  bool equal(const self& other)  const 
  { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= size); }
  vertex_type dereference()       const { return vertices[mc]; }
   bool valid()                   const { return (mc < size); }
  void reset()                     {mc = 0;}

   self end()  { self new_object(*this); new_object.mc = size; return new_object; }

  TopologyT const& topo_anchor() const { return e.topo_anchor();}
  edge_type const& base()         const { return e; }

private:
  facet_handle const& handle()   const { return e.handle(); }

private:

  edge_type e;
  mycounter mc;
  std::vector<vertex_type> vertices;
  int size;

};

///////////////////////////////////////////////////////////////////////////
/// @brief Structured Edge on Cell Iterator
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class structured_edge_on_cell_iterator
  : public boost::iterator_facade<
        structured_edge_on_cell_iterator<TopologyT>      // Derived
      , typename topology_traits<TopologyT>::edge_type   // Base
      , boost::bidirectional_traversal_tag               // CategoryOrTraversal     
      , typename topology_traits<TopologyT>::edge_type   // Base
    >
{
  typedef structured_edge_on_cell_iterator<TopologyT>          self;
  typedef int                                                  mycounter;
  typedef typename topology_traits<TopologyT>::vertex_type     vertex_type;
  typedef typename topology_traits<TopologyT>::cell_type       cell_type;
  typedef typename topology_traits<TopologyT>::vertex_handle   vertex_handle;
  typedef typename topology_traits<TopologyT>::vertex_handle   cell_handle;
  typedef typename topology_traits<TopologyT>::facet_type      facet_type;
  typedef typename topology_traits<TopologyT>::edge_type       edge_type;
  typedef typename topology_traits<TopologyT>::facet_handle    facet_handle;
  typedef typename topology_traits<TopologyT>::dimension_tag   dimension_tag;
  typedef typename topology_traits<TopologyT>::vertex_on_facet_iterator    vertex_on_facet_iterator;
  

public:

  typedef cell_type    base_type;

  structured_edge_on_cell_iterator  () : mc(-1) {}
  explicit

  structured_edge_on_cell_iterator  (cell_type const& c) : c(c), mc(0) 
  {
    // (n * 2^(n-1)) edges have to be found
    
    for (int i=1; i<=dimension_tag::dim; i++)
      {
	facet_type f(c, i);

	vertex_on_facet_iterator vofit(f);
	while(vofit.valid())
	  {
	    edges.push_back(edge_type(*vofit, i));
	    ++vofit;
	  }
      }
     mc = 0;
  }
  
  friend class boost::iterator_core_access;
   
   void increment() { ++mc; }
   void decrement() { --mc; }
   bool equal(const self& other)  const
      { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= edges.size()); }
   
   edge_type dereference()          const { return edges[mc]; }
   bool valid()                     const { return (mc < edges.size()); }
   void reset()                     {mc = 0;}

   self end()  { self new_object(*this); new_object.mc = edges.size(); return new_object; }
   

   TopologyT const& topo_anchor()   const { return c.topo_anchor();}
   cell_type const& base()          const { return c; }
   
private:

   cell_type c;
   unsigned mc;
   std::vector<edge_type> edges;
};




template <typename TopologyT>
class structured_facet_on_edge_iterator
  : public boost::iterator_facade<
        structured_facet_on_edge_iterator<TopologyT>      // Derived
      , typename topology_traits<TopologyT>::facet_type   // Base
      , boost::bidirectional_traversal_tag               // CategoryOrTraversal     
      , typename topology_traits<TopologyT>::facet_type   // Base
    >
{
  typedef structured_facet_on_edge_iterator<TopologyT>          self;
  typedef int                                                  mycounter;
  typedef typename topology_traits<TopologyT>::vertex_type     vertex_type;
  typedef typename topology_traits<TopologyT>::cell_type       cell_type;
  typedef typename topology_traits<TopologyT>::vertex_handle   vertex_handle;
  typedef typename topology_traits<TopologyT>::vertex_handle   cell_handle;
  typedef typename topology_traits<TopologyT>::facet_type      facet_type;
  typedef typename topology_traits<TopologyT>::edge_type       edge_type;
  typedef typename topology_traits<TopologyT>::facet_handle    facet_handle;
  typedef typename topology_traits<TopologyT>::dimension_tag   dimension_tag;
  typedef typename topology_traits<TopologyT>::vertex_on_facet_iterator    vertex_on_facet_iterator;
   typedef typename detail_topology::facet_on_edge_iterator<TopologyT>           standard_iterator;
  
public:

  typedef edge_type    base_type;

  structured_facet_on_edge_iterator  () : mc(-1) {}
  explicit

  structured_facet_on_edge_iterator  (edge_type const& e) : e(e), mc(0) 
  {
     if(e.topo_anchor().edge_facet_map[e].empty())
     for(standard_iterator stdit(e); stdit.valid(); ++stdit)
     {
        e.topo_anchor().edge_facet_map[e].push_back(*stdit);
     }
     mc = 0;
  }
  
  friend class boost::iterator_core_access;
   
   void increment() { ++mc; }
   void decrement() { --mc; }
   bool equal(const self& other)  const
      { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >=  topo_anchor().edge_facet_map[e].size()); }
   
   facet_type dereference()         const { return (*(topo_anchor().edge_facet_map.find(e))).second[mc]; }
   bool valid()                     const { return (mc <   (*(topo_anchor().edge_facet_map.find(e))).second.size()); }
   void reset()                     {mc = 0;}

   self end()  { self new_object(*this); new_object.mc = (*(topo_anchor().edge_facet_map.find(e))).second.size(); return new_object; }

   
   TopologyT const& topo_anchor()   const { return e.topo_anchor();}
   edge_type const& base()          const { return e; }
   
private:

   base_type e;
   unsigned mc;
};




template <typename TopologyT>
class structured_edge_on_facet_iterator
  : public boost::iterator_facade<
        structured_edge_on_facet_iterator<TopologyT>      // Derived
      , typename topology_traits<TopologyT>::edge_type   // Base
      , boost::bidirectional_traversal_tag               // CategoryOrTraversal     
      , typename topology_traits<TopologyT>::edge_type   // Base
    >
{
  typedef structured_edge_on_facet_iterator<TopologyT>          self;
  typedef int                                                  mycounter;
  typedef typename topology_traits<TopologyT>::vertex_type     vertex_type;
  typedef typename topology_traits<TopologyT>::cell_type       cell_type;
  typedef typename topology_traits<TopologyT>::vertex_handle   vertex_handle;
  typedef typename topology_traits<TopologyT>::vertex_handle   cell_handle;
  typedef typename topology_traits<TopologyT>::facet_type      facet_type;
  typedef typename topology_traits<TopologyT>::edge_type       edge_type;
  typedef typename topology_traits<TopologyT>::facet_handle    facet_handle;
  typedef typename topology_traits<TopologyT>::dimension_tag   dimension_tag;
  typedef typename topology_traits<TopologyT>::vertex_on_facet_iterator    vertex_on_facet_iterator;
   typedef typename detail_topology::edge_on_facet_iterator<TopologyT>           standard_iterator;
  
public:

   typedef facet_type    base_type;

  structured_edge_on_facet_iterator  () : mc(-1) {}
  explicit

	  //[RH] .. generates consitent information for --dereference()-- 
  structured_edge_on_facet_iterator  (facet_type const& f) : f(f), mc(0) 
  {
     if(f.topo_anchor().facet_edge_map[f].empty())
     for(standard_iterator stdit(f); stdit.valid(); ++stdit)
     {
        f.topo_anchor().facet_edge_map[f].push_back(*stdit);
     }
     mc = 0;
  }
  
  friend class boost::iterator_core_access;
   
   void increment() { ++mc; }
   void decrement() { --mc; }
   bool equal(const self& other)  const
      { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >=   topo_anchor().facet_edge_map[f].size()); }
   
  //[RH] .. check with end iterator.. 
  //  important
  //  
   edge_type dereference()          const { return (*(topo_anchor().facet_edge_map.find(f))).second[mc]; }
   bool valid()                     const { return (mc < (*(topo_anchor().facet_edge_map.find(f))).second.size()); }
   void reset()                     {mc = 0;}
   
   self end()  { self new_object(*this); new_object.mc = (*(topo_anchor().facet_edge_map.find(f))).second.size(); return new_object; }
   


   TopologyT const& topo_anchor()   const { return f.topo_anchor();}
   base_type const& base()          const { return f; }
   
private:

   facet_type f;
   unsigned mc;
};



}  // end of namespace::detail_topology

}  // end of namespace::gsse

#endif

