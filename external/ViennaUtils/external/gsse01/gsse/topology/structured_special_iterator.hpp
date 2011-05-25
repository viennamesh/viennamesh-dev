/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_STRUCTURED_SPECIAL_ITERATOR_HH_ID
#define GSSE_STRUCTURED_SPECIAL_ITERATOR_HH_ID 

// *** GSSE includes
// 
#include "gsse/topology.hpp"
#include "gsse/math/power.hpp"

// *** BOOST includes
//
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
/// @brief Structured cell  on Facet Iterator
///
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT>
class structured_cell_on_facet_iterator
  : public boost::iterator_facade<
        structured_cell_on_facet_iterator<TopologyT>     // Derived
      , typename topology_traits<TopologyT>::cell_type        // Base
      , boost::bidirectional_traversal_tag               // CategoryOrTraversal     
      , typename topology_traits<TopologyT>::cell_type        // Base
    >
{
   typedef structured_cell_on_facet_iterator<TopologyT>         self;
   typedef int                                                  mycounter;
   typedef typename topology_traits<TopologyT>::vertex_type          vertex_type;
   typedef typename topology_traits<TopologyT>::cell_type            cell_type;
   typedef typename topology_traits<TopologyT>::vertex_handle   vertex_handle;
   typedef typename topology_traits<TopologyT>::facet_type           facet_type;
   typedef typename topology_traits<TopologyT>::facet_handle    facet_handle;
   typedef typename topology_traits<TopologyT>::dimension_tag   dimension_tag;

private:
   facet_type     f;
   mycounter mc;
   std::vector<cell_type> cells;
  int size;

public:

   typedef facet_type  base_type;
   typedef cell_type value_type;

   structured_cell_on_facet_iterator  () : mc(-1) {}
   explicit
   structured_cell_on_facet_iterator  (facet_type const& f) : f(f), mc(0) 
   {
     cells.clear();

     boost::array<long, dimension_tag::dim> indices = topo_anchor().get_vertex_index(f.base_vertex());

     cells.push_back(*(topo_anchor().get_index_cell(indices)));

     if (indices[f.direction()-1] != 0)
       {
	 --indices[f.direction()-1];
	 cells.push_back(*(topo_anchor().get_index_cell(indices)));
       }
     size = cells.size();
   }

   friend class boost::iterator_core_access;

   void increment() { ++mc; }
   void decrement() { --mc; }
   bool equal(const self& other) const { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= size); }
   cell_type operator*()  const  { return  cells[mc]; }
   bool valid()      const { return (mc < size); }

   self end()  { self new_object(*this); new_object.mc = size; return new_object; }

  TopologyT const& topo_anchor() const 
  { return f.topo_anchor();}

  facet_type const& base() const 
  { return f; }

  facet_handle const& handle() const 
  { return f.handle(); }
};







///////////////////////////////////////////////////////////////////////////
/// @brief Structured Edge on Vertex Iterator
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT, bool ordered>
class structured_edge_on_vertex_iterator
  : public boost::iterator_facade<
  structured_edge_on_vertex_iterator<TopologyT, ordered>       // Derived
      , typename topology_traits<TopologyT>::edge_type        // Base
      , boost::bidirectional_traversal_tag                    // CategoryOrTraversal     
      , typename topology_traits<TopologyT>::edge_type        // Base
    >
{
  typedef structured_edge_on_vertex_iterator<TopologyT, ordered>          self;
  typedef int                                                             mycounter;
  typedef typename topology_traits<TopologyT>::vertex_type                vertex_type;
  typedef typename topology_traits<TopologyT>::cell_type                  cell_type;
  typedef typename topology_traits<TopologyT>::vertex_handle              vertex_handle;
  typedef typename topology_traits<TopologyT>::vertex_handle              cell_handle;
  typedef typename topology_traits<TopologyT>::facet_type                 facet_type;
  typedef typename topology_traits<TopologyT>::edge_type                  edge_type;
  typedef typename topology_traits<TopologyT>::edge_handle                edge_handle;
  typedef typename topology_traits<TopologyT>::facet_handle               facet_handle;
  typedef typename topology_traits<TopologyT>::dimension_tag              dimension_tag;
  typedef typename topology_traits<TopologyT>::vertex_on_facet_iterator   vertex_on_facet_iterator;
  typedef typename topology_traits<TopologyT>::direction_n_iterator       direction_n_iterator;

public:

  typedef vertex_type  base_type;

  structured_edge_on_vertex_iterator  () : mc(-1) {}
  explicit
  structured_edge_on_vertex_iterator  (vertex_type const& v) : v(v), mc(0) 
  {

    for(int dimension = 1; dimension <= dimension_tag::dim; ++dimension)
      {
	edge_type e = edge_type(v, dimension);

	direction_n_iterator iter(v, dimension);
	++iter;

	if (iter.valid())
	  edges.push_back(e);

	if (!ordered)
	  {
             // switch the direction if ordered is off
             e = edge_type(v, -dimension, true);  
          }
	else
	  e = edge_type(v, -dimension);
    
        if ((boost::tuples::get<0>(e.handle()) != -1)  &&
            (boost::tuples::get<1>(e.handle()) != -1))
        {
           edges.push_back(e);
        }
      }
    size = edges.size();
  }
  
  friend class boost::iterator_core_access;

  void increment() { ++mc; }
  void decrement() { --mc; }
  bool equal(const self& other)  const 
  { return ( (mc == other.mc && topo_anchor() == other.topo_anchor()) || mc >= size); }

  edge_type dereference()             const { return edges[mc]; }
  bool valid()                   const { return (mc < size); }
  void reset()                         {mc = 0;}

   self end()  { self new_object(*this); new_object.mc = size; return new_object; }


  TopologyT const& topo_anchor() const { return v.topo_anchor();}
  vertex_type const& base()           const { return v; }
  vertex_handle const& handle()  const { return v.handle(); }

private:

  vertex_type v;
  mycounter mc;
  std::vector<edge_type> edges;
  int size;

};

///////////////////////////////////////////////////////////////////////////
//
// Special structured iterators
//
///////////////////////////////////////////////////////////////////////////

   template <unsigned int dimension>
   class structured_direction_n_iterator 
   {
      typedef structured_direction_n_iterator<dimension>           self;
      typedef structured<dimension>                                topology_t;

      typedef typename topology_traits<topology_t>::vertex_type    vertex_type;
      typedef typename topology_traits<topology_t>::vertex_handle  vertex_handle;
      typedef typename topology_traits<topology_t>::dimension_tag  dimension_tag;

   private:
     vertex_type v;
     long dim;
     long iter_pos;

   public:

     typedef vertex_type value_type;

     explicit
     structured_direction_n_iterator(vertex_type const& vv, const unsigned int& dim) : v(vv), dim(dim) 
     {
       iter_pos = topo_anchor().get_vertex_dim_n_index(v,dim);
     }
      ~structured_direction_n_iterator() {}

     vertex_type const& operator*() const  { return v;}
     
     bool valid()
     {
       int pos = topo_anchor().get_vertex_dim_n_index(v,dim);
       int max = topo_anchor().get_dim_n_size(dim);

       if (pos >= max) return false;
       if (pos != iter_pos) return false;
       return true;
     }

      self end()  
         { 
                int max = topo_anchor().get_dim_n_size(dim);
                self new_object(*this); new_object.pos = max; return new_object; 
      }


      // 	vertex const& operator->() const { return v; }

     // introduced by [MS]

     void change_direction(const unsigned int d)
     {
       dim = d;
       iter_pos = topo_anchor().get_vertex_dim_n_index(v,dim);
     }

      vertex_type& operator++()       
         {  
            boost::array<long, dimension_tag::dim> index_v;
            for (unsigned int i=1; i <= dimension; i++)
            {
               index_v[i-1] = topo_anchor().get_vertex_dim_n_index(v,i);
            }
            index_v[dim-1]++;
            v = vertex_type(topo_anchor().get_index_vertex(index_v)); 
	    ++iter_pos;
            return v; 
         }
      vertex_type  operator++(int)    
         {  
            vertex_type oldObj = v;  

            boost::array<long, dimension_tag::dim> index_v;
            for (unsigned int i=1; i <= dimension; i++)
            {
               index_v[i-1]  = topo_anchor().get_vertex_dim_n_index(v,i);
            }
            index_v[dim-1]++;
            v = vertex_type(topo_anchor().get_index_vertex(index_v)); 
	    ++iter_pos;
            return oldObj; 
         }

      vertex_type& operator+=(int addcount)  
         {  
            boost::array<long, dimension_tag::dim> index_v;
            for (unsigned int i=1; i <= dimension; i++)
            {
               index_v[i-1] = topo_anchor().get_vertex_dim_n_index(v,i);
            }
            index_v[dim-1]+=addcount;
            v = vertex_type(topo_anchor().get_index_vertex(index_v)); 
	    iter_pos+=addcount;
            return v; 
         }



      vertex_type& operator--()       
         {  
            boost::array<long, dimension_tag::dim> index_v;
            for (unsigned int i=1; i <= dimension; i++)
            {
	      index_v[i-1] =topo_anchor().get_vertex_dim_n_index(v,i);
            }
            --index_v[dim-1];
            v = vertex_type(topo_anchor().get_index_vertex(index_v)); 
	    --iter_pos;
            return v; 
         }
      vertex_type  operator--(int)    
      {  
            vertex_type oldObj = v;  

            boost::array<long, dimension_tag::dim> index_v;
            for (unsigned int i=1; i <= dimension; i++)
            {
               index_v[i-1] = topo_anchor().get_vertex_dim_n_index(v,i);
            }
	    
            --index_v[dim-1];
            v = vertex_type(topo_anchor().get_index_vertex(index_v)); 
	    --iter_pos;
            return oldObj; 
         }

      vertex_type& operator-=(int addcount)  
         {  
            boost::array<long, dimension_tag::dim> index_v;
            for (unsigned int i=1; i <= dimension; i++)
            {
               index_v[i-1] = topo_anchor().get_vertex_dim_n_index(v,i);
            }
            index_v[dim-1]-=addcount;
            v = vertex_type(topo_anchor().get_index_vertex(index_v)); 
	    iter_pos -= addcount;
            return v; 
         }

         
      topology_t const& topo_anchor() const {  return v.topo_anchor();}
      vertex_handle    handle()  const    {  return v.handle();}
   };



///////////////////////////////////////////////////////////////////////////
//
// Special structured iterators
//
///////////////////////////////////////////////////////////////////////////

   template <unsigned int  dimension>
   class structured_cell_direction_n_iterator 
   {
      typedef structured_cell_direction_n_iterator              self;
      typedef structured<dimension>                             topology_t;

      typedef typename topology_traits<topology_t>::cell_type        cell_type;
      typedef typename topology_traits<topology_t>::cell_handle cell_handle;
      typedef typename topology_traits<topology_t>::dimension_tag dimension_tag;

   private:
      cell_type c;
      unsigned int dim;
   public:

      typedef cell_type value_type;

      explicit
      structured_cell_direction_n_iterator(cell_type const& cc, const unsigned int dim) : c(cc), dim(dim) {}
      ~structured_cell_direction_n_iterator() {}
	

      bool valid()   const   {  return c.valid();  }

// [RH][TODO] .. ??? 

//      self end()  { self new_object(*this); new_object.c. = max; return new_object; }
   

      cell_type const& operator*() const  { return c;}
      // 	cell_type const& operator->() const { return c;}


     // introduced by [MS], firstly only for testing ...

     void change_direction(const unsigned int d)
     {
       dim = d;
     }

     
     cell_type& operator++()      
     {  
       boost::array<long, dimension_tag::dim> index_v;
       for (unsigned int i=1; i <= dimension; i++)
	 {
	   index_v[i-1] =topo_anchor().get_cell_dim_n_index(c, i);
	 }
       index_v[dim-1]++;
       c = *(topo_anchor().get_index_cell(index_v)); 
       return  c;
     }

     cell_type  operator++(int)   
         {  
            cell_type oldObj = c; 

            boost::array<long, dimension_tag::dim> index_v;
            for (unsigned int i=1; i <= dimension; i++)
            {
               index_v[i-1] =topo_anchor().get_cell_dim_n_index(c, i);
            }
            index_v[dim-1]++;
            c = topo_anchor().get_index_cell(index_v); 
            return oldObj;
         }
      cell_type& operator+=(int addcount) 
         {  
            boost::array<long, dimension_tag::dim> index_v;
            for (unsigned int i=1; i <= dimension; i++)
            {
               index_v[i-1] =topo_anchor().get_cell_dim_n_index(c, i);
            }
            index_v[dim-1]+=addcount;
            c = topo_anchor().get_index_cell(index_v); 
            return c; 
         }

      cell_type& operator--()      
         {  
            boost::array<long, dimension_tag::dim> index_v;
            for (unsigned int i=1; i <= dimension; i++)
            {
               index_v[i-1] =topo_anchor().get_cell_dim_n_index(c, i);
            }
            index_v[dim-1]--;
            c = topo_anchor().get_index_cell(index_v); 
            return  c;
         }
      cell_type  operator--(int)   
         {  
            cell_type oldObj = c; 

            boost::array<long, dimension_tag::dim> index_v;
            for (unsigned int i=1; i <= dimension; i++)
            {
               index_v[i-1] =topo_anchor().get_cell_dim_n_index(c, i);
            }
            index_v[dim-1]--;
            c = topo_anchor().get_index_cell(index_v); 
            return oldObj;
         }
      cell_type& operator-=(int addcount) 
         {  
            boost::array<long, dimension_tag::dim> index_v;
            for (unsigned int i=1; i <= dimension; i++)
            {
               index_v[i-1] =topo_anchor().get_cell_dim_n_index(c, i);
            }
            index_v[dim-1]-=addcount;
            c = topo_anchor().get_index_cell(index_v); 
            return c; 
         }



      topology_t const& topo_anchor() const {  return c.topo_anchor();}
     cell_handle      handle()     const {  return c.handle();}
   };


///////////////////////////////////////////////////////////////////////////
/// @brief Structured Edge on Vertex Iterator for one direction
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT, unsigned int Dimension>
class structured_edge_on_vertex_direction_iterator
  : public boost::iterator_facade<
  structured_edge_on_vertex_direction_iterator<TopologyT, Dimension> 
      , typename topology_traits<TopologyT>::edge_type   
      , boost::bidirectional_traversal_tag               
      , typename topology_traits<TopologyT>::edge_type >
{

  typedef structured_edge_on_vertex_direction_iterator<TopologyT, Dimension>        self;
  typedef int                                                  mycounter;
  typedef typename topology_traits<TopologyT>::vertex_type     vertex_type;
  typedef typename topology_traits<TopologyT>::cell_type       cell_type;
  typedef typename topology_traits<TopologyT>::vertex_handle   vertex_handle;
  typedef typename topology_traits<TopologyT>::vertex_handle   cell_handle;
  typedef typename topology_traits<TopologyT>::facet_type      facet_type;
  typedef typename topology_traits<TopologyT>::edge_type       edge_type;
  typedef typename topology_traits<TopologyT>::facet_handle    facet_handle;
  typedef typename topology_traits<TopologyT>::dimension_tag   dimension_tag;
  typedef typename topology_traits<TopologyT>::vertex_on_facet_iterator vertex_on_facet_iterator;
  typedef typename topology_traits<TopologyT>::direction_n_iterator direction_n_iterator;
  typedef typename topology_traits<TopologyT>::edge_on_vertex_iterator edge_on_vertex_iterator;

  BOOST_STATIC_ASSERT(Dimension <= dimension_tag::dim);
  BOOST_STATIC_ASSERT(Dimension > 0);
  

public:

  typedef vertex_type  base_type;
  typedef edge_type    value_type;

  structured_edge_on_vertex_direction_iterator  () : mc(-1) {}
  explicit
  structured_edge_on_vertex_direction_iterator  (vertex_type const& v) : v(v), mc(0) 
  {
     edge_on_vertex_iterator eovit(v);

     while (eovit.valid())
     {
        if ((*eovit).direction() == Dimension) edges.push_back(*eovit);
        ++eovit;
     }
  
    size = edges.size();
  }
  
  friend class boost::iterator_core_access;

  void increment() { ++mc; }
  void decrement() { --mc; }
  bool equal(const self& other)  const 
  { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= size); }
  edge_type dereference()        const { return edges[mc]; }
  bool valid()                   const { return (mc < size); }

   self end()  { self new_object(*this); new_object.mc = size; return new_object; }

  TopologyT const& topo_anchor() const { return v.topo_anchor();}
  base_type const& base()        const { return v; }
  //  vertex_handle const& handle()   const { return v.handle(); }

private:

  vertex_type v;
  mycounter mc;
  std::vector<edge_type> edges;
  int size;

};


///////////////////////////////////////////////////////////////////////////
/// @brief Structured Edge on Vertex Iterator for one direction
///////////////////////////////////////////////////////////////////////////

template <typename TopologyT, unsigned int Dimension>
class structured_edge_on_vertex_direction_left_iterator
  : public boost::iterator_facade<
  structured_edge_on_vertex_direction_left_iterator<TopologyT, Dimension> 
      , typename topology_traits<TopologyT>::edge_type   
      , boost::bidirectional_traversal_tag               
      , typename topology_traits<TopologyT>::edge_type >
{

  typedef structured_edge_on_vertex_direction_left_iterator<TopologyT, Dimension>        self;
  typedef int                                                  mycounter;
  typedef typename topology_traits<TopologyT>::vertex_type     vertex_type;
  typedef typename topology_traits<TopologyT>::cell_type       cell_type;
  typedef typename topology_traits<TopologyT>::vertex_handle   vertex_handle;
  typedef typename topology_traits<TopologyT>::vertex_handle   cell_handle;
  typedef typename topology_traits<TopologyT>::facet_type      facet_type;
  typedef typename topology_traits<TopologyT>::edge_type       edge_type;
  typedef typename topology_traits<TopologyT>::facet_handle    facet_handle;
  typedef typename topology_traits<TopologyT>::dimension_tag   dimension_tag;
  typedef typename topology_traits<TopologyT>::vertex_on_facet_iterator vertex_on_facet_iterator;
  typedef typename topology_traits<TopologyT>::direction_n_iterator direction_n_iterator;
  typedef typename topology_traits<TopologyT>::edge_on_vertex_iterator edge_on_vertex_iterator;

   BOOST_STATIC_ASSERT(Dimension <= dimension_tag::dim);
   BOOST_STATIC_ASSERT(Dimension > 0);
  

public:

  typedef vertex_type  base_type;
  typedef edge_type    value_type;

  structured_edge_on_vertex_direction_left_iterator  () : mc(-1) {}
  explicit
  structured_edge_on_vertex_direction_left_iterator  (vertex_type const& v) : v(v), mc(0) 
  {
     edge_on_vertex_iterator eovit(v);

     while (eovit.valid())
     {
       if ((*eovit).direction() == Dimension && (*eovit).inverted() == false) 
	 edges.push_back(*eovit);
        ++eovit;
     }
  
    size = edges.size();
  }
  
  friend class boost::iterator_core_access;

  void increment() { ++mc; }
  void decrement() { --mc; }
  bool equal(const self& other)  const 
  { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= size); }
  edge_type dereference()        const { return edges[mc]; }
  bool valid()                   const { return (mc < size); }

   self end()  { self new_object(*this); new_object.mc = size; return new_object; }

  TopologyT const& topo_anchor() const { return v.topo_anchor();}
  base_type const& base()        const { return v; }
  //  vertex_handle const& handle()   const { return v.handle(); }

private:

  vertex_type v;
  mycounter mc;
  std::vector<edge_type> edges;
  int size;

};



}  // end of namespace::detail_topology

}  // end of namespace::gsse

#endif

