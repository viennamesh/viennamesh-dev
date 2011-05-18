/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_DERIVEDOBJECTS_HH_ID
#define GSSE_DERIVEDOBJECTS_HH_ID 

// *** system includes
//
#include <iostream>
#include <set>
#include <vector>

// *** BOOST includes
// 
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/operators.hpp>
#include <boost/array.hpp>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/tuple/tuple_io.hpp>

// *** GSSE includes
// 
#include "gsse/topology.hpp"
#include "gsse/topology/topology_handles.hpp"
#include "gsse/topology/structured_iterator.hpp"
#include "gsse/topology/structured_special_iterator.hpp"



namespace gsse
{

///////////////////////////////////////////////////////////////////////////
/// @brief Generic unstructured vertex object
///
/// It is a leightweight object, which provides the topological information 
/// related to a vertex
///////////////////////////////////////////////////////////////////////////
namespace detail_topology
{
template<typename BaseType>
class generic_vertex : 
    public boost::equality_comparable< generic_vertex<BaseType>  >
  , boost::less_than_comparable< generic_vertex<BaseType>  >

{
  typedef generic_vertex<BaseType>       self_t;
  typedef vertex_handle_int<BaseType>    vertex_handle;

   BaseType const*                       g;
   vertex_handle                         vh;

public:
   typedef self_t                        value_type;
   typedef vertex_handle                 handle_type;
   typedef BaseType                      topology_type;
   typedef BaseType                      base_type;

   generic_vertex() : g(0), vh(-1) {}
   generic_vertex(BaseType const& gg) : g(&gg), vh(-1) {}
   explicit
   generic_vertex(BaseType const& gg, vertex_handle vh) : g(&gg), vh(vh) {}

   bool operator==(const self_t& other)  const {return (vh == other.vh && topo_anchor()==other.topo_anchor());}

   bool operator<(const self_t& other)  const {return handle() < other.handle();}


   BaseType const&  topo_anchor() const {  return *g;}
   vertex_handle    handle()      const {  return vh;}

   friend std::ostream& operator<<(std::ostream& ostr, const generic_vertex<BaseType>& mv) 
      {
         ostr << "vertex: .. vertex_handle: " << mv.vh;
         return ostr;
      }
};


///////////////////////////////////////////////////////////////////////////
/// @brief Generic unstructured cell class
///
/// This object should be eliminated by a current compiler (so called "mayfly" objects)
/// It is only used to wrap up the view onto the topology container
///
/// Concepts: equality comparable
///           
///////////////////////////////////////////////////////////////////////////
template<typename BaseType>
class generic_cell :
    public boost::equality_comparable< generic_cell<BaseType>  >
  , boost::less_than_comparable< generic_cell<BaseType>  >

{
  typedef generic_cell<BaseType>                              self_t;
  typedef typename topology_traits<BaseType>::cell_handle     cell_handle;
  typedef typename topology_traits<BaseType>::dimension_tag   dimension_tag;


public:
  typedef cell_handle      handle_type;
  typedef BaseType         topology_type;
  typedef BaseType         base_type;

   generic_cell() : g(0), ch(-1) {}
   generic_cell(BaseType const& gg) : g(&gg), ch(-1) {}
   explicit
   generic_cell(BaseType const& gg, cell_handle ch) : g(&gg), ch(ch) 
  { 
    //std::cout << "!!# cell constructor here with adress of ust: " << gg << std::endl;
  }

   bool operator==(const self_t& other)  const {return (ch == other.ch);}
   bool operator<(const self_t& other)  const {return handle() < other.handle();}
  
   BaseType const& topo_anchor() const {  return *g;}
   cell_handle     handle()      const {  return ch;}

   friend std::ostream& operator<<(std::ostream& ostr, const generic_cell<BaseType>& mv)
      {
         ostr << "Generic cell: .. cell_handle: " << mv.ch;
         return ostr;
      }
private:
   BaseType const*                g;
   cell_handle                    ch;

};



///////////////////////////////////////////////////////////////////////////
/// @brief Generic edge object
///
/// It is a leightweight object, which provides the topological information 
/// related to an edge
//
///   the main parts are always directed (without an order relation)
///   but there are also handles with an order relation 
/// 
///  the unique handle is always calculated with the order-relation
///
///////////////////////////////////////////////////////////////////////////
template <typename BaseType>
class generic_edge:
    public boost::equality_comparable< generic_edge<BaseType>  >
  , boost::less_than_comparable< generic_edge<BaseType>  >
{
  typedef generic_edge<BaseType>                                 self_t;
  typedef typename topology_traits<BaseType>::vertex_type        vertex_type;
  typedef typename topology_traits<BaseType>::vertex_handle      vertex_handle;
  
 public:
  typedef typename topology_traits<BaseType>::edge_handle        edge_handle;
 private:
  typedef edge_handle                                    handle_notordered_t;
  typedef edge_handle                                    handle_ordered_t;
  
  typedef vertex_handle                                  handle_1;
  typedef vertex_handle                                  handle_2;
  typedef int                                            mycounter;
  
public:
   typedef self_t                                        value_type;
   typedef edge_handle                                   handle_type;
   typedef BaseType                                      topology_t;
   typedef BaseType                                      topology_type;
   typedef GSSE_Property                                 edge_direction_trait;
   typedef BaseType                                      base_type;

private:
  topology_type const* g;
  vertex_handle     h1, ho1;
  vertex_handle     h2, ho2;

  handle_notordered_t  ht_no;
  handle_ordered_t     ht_o;
  

public:
  generic_edge() : g(0) 
  {
    boost::tuples::get<0>(ht_no) =vertex_handle(-1);
    boost::tuples::get<1>(ht_no) =vertex_handle(-1);

    h1=-1; h2=-1;
  }
  explicit
  generic_edge(topology_type const& gg) : g(&gg) 
  {
    boost::tuples::get<0>(ht_no) =vertex_handle(0);
    boost::tuples::get<1>(ht_no) =vertex_handle(0);

    h1=0; h2=0;
  }
  
  generic_edge(topology_type const& gg, vertex_handle h1_, vertex_handle h2_) 
    : g(&gg) , h1(h1_), h2(h2_) 
  {
    boost::tuples::get<0>(ht_no) = h1_;
    boost::tuples::get<1>(ht_no) = h2_;

     //std::cout << "CALLING CONSTRUCTOR1" << std::endl;

    if (boost::tuples::get<0>(ht_no) > boost::tuples::get<1>(ht_no))
      //if (h1_ > h2_) 
      {
	ho1 = h2_; 
	ho2 = h1_; 
	boost::tuples::get<0>(ht_o) = h2_;
	boost::tuples::get<1>(ht_o) = h1_;

      }
    else  
      {
	ho1 = h1_; 
	ho2 = h2_;

	boost::tuples::get<0>(ht_o) = h1_;
	boost::tuples::get<1>(ht_o) = h2_;

      } // guarantee ordering 

    //std::cout << "DONE CONSTRUCTOR1" << std::endl;

  }
  
  generic_edge(topology_type const& gg, edge_handle eh) : g(&gg)
  {
// std::cout << "CALLING CONSTRUCTOR2" << std::endl;

    // check, if the given handle is valid
    //
//      if (eh < 0) 
//      {
//         h1=-1; h2=-1; ho1 = -1; ho2 = -1; 

// 	boost::tuples::get<0>(ht_no) = -1;
// 	boost::tuples::get<1>(ht_no) = -1;

// 	boost::tuples::get<0>(ht_o) = -1;
// 	boost::tuples::get<1>(ht_o) = -1;

// std::cout << "DONE CONSTRUCTOR2" << std::endl;
//         return;
//      }

//      h1 = eh % topo_anchor().get_max_handle();
//      h2 = eh / topo_anchor().get_max_handle();

     h1 = boost::tuples::get<0>(eh);
     h2 = boost::tuples::get<1>(eh);
     
     boost::tuples::get<0>(ht_no) = h1;
     boost::tuples::get<1>(ht_no) = h2;
     
    if (boost::tuples::get<0>(ht_no) > boost::tuples::get<1>(ht_no))
      {
	ho1 = h2; 
	ho2 = h1; 
	boost::tuples::get<0>(ht_o) = h2;
	boost::tuples::get<1>(ht_o) = h1;

      }
    else  
      {
	ho1 = h1; 
	ho2 = h2;

	boost::tuples::get<0>(ht_o) = h1;
	boost::tuples::get<1>(ht_o) = h2;

      } // guarantee ordering 

    
//      if (h1 > h2) 
//      {ho1 = h2; ho2 = h1;} 
//      else  
//      {ho1 = h1; ho2 = h2;} // guarantee ordering 

     //std::cout << "DONE CONSTRUCTOR2" << std::endl;    
  }
    
  topology_type   const& topo_anchor() const {  return *g;}
//   vertex_handle   handle1()     const {  return h1;}
//   vertex_handle   handle2()     const {  return h2;}
  vertex_handle   handle1()     const {  return boost::tuples::get<0>(ht_no);}
  vertex_handle   handle2()     const {  return boost::tuples::get<1>(ht_no);}

  vertex_type     vertex1()     const {  return vertex_type(topo_anchor(), handle1());}
  vertex_type     vertex2()     const {  return vertex_type(topo_anchor(), handle2());}
  //  edge_handle     handle()      const {  return ho1 + ho2 * topo_anchor().get_max_handle(); } 
   edge_handle  handle()     const {  return ht_o;}
   edge_handle  handle_n()   const {  return ht_no;}

//   vertex_handle   handleo1()     const {  return ho1;}
//   vertex_handle   handleo2()     const {  return ho2;}
  vertex_handle   handleo1()     const {  return boost::tuples::get<0>(ht_o);}
  vertex_handle   handleo2()     const {  return boost::tuples::get<1>(ht_o);}
  vertex_type     vertexo1()     const {  return vertex_type(topo_anchor(), handleo1());}
  vertex_type     vertexo2()     const {  return vertex_type(topo_anchor(), handleo2());}
  
  bool operator<(const self_t& other) const { return !(handle() <= other.handle() && g==other.g);}
  bool operator==(const self_t& other) const { return (handle()==other.handle() && g==other.g);}


  friend std::ostream& operator<<(std::ostream& ostr, const generic_edge& mv)
  {
    ostr << "generic_edge: .. edge_handles: " 
	 << mv.handle1() << "/" << mv.handle2() 
	 << "  handle: " << mv.handle();
    return ostr;
  }
};

///////////////////////////////////////////////////////////////////////////
/// @brief Generic simplex facet
///
/// this object stores all contained vertices' information in a container. 
///
///////////////////////////////////////////////////////////////////////////
 template <typename BaseType>
 class simplex_facet :
    public boost::equality_comparable< simplex_facet<BaseType>  >
  , boost::less_than_comparable< simplex_facet<BaseType>  >
 {
    typedef simplex_facet<BaseType>                               self_t;
    typedef typename topology_traits<BaseType>::vertex_type       vertex_type;
    typedef typename topology_traits<BaseType>::cell_type         cell_type;
    typedef typename topology_traits<BaseType>::vertex_handle     vertex_handle;

    typedef int                                                   mycounter;

 public:

    typedef typename topology_traits<BaseType>::facet_handle      facet_handle;
    typedef facet_handle                                          handle_type;
    typedef BaseType                                              topology_type;
    typedef BaseType                                              base_type;

   friend class vertex_on_facet_iterator<BaseType>;

      
 public:
   simplex_facet() : base_topo(0) {}
   explicit
   simplex_facet(topology_type const& gg) : base_topo(&gg) {}

   simplex_facet(topology_type const& gg, facet_handle fh) : base_topo(&gg), thehandle(fh) 
   {
      std::cout << "simplex facet.. ctr 1.. " << std::endl;

     handles.clear();

// [RH][TODO] reimplement this as a meta container 

     handles.insert( boost::tuples::get<0>(fh) );
     handles.insert( boost::tuples::get<1>(fh) );

     if (BaseType::dimension_tag::dim == 3)
        handles.insert( boost::tuples::get<2>(fh) );
     
   }
   

// [RH] information:: this is used in derived_iterator :: 
//
    simplex_facet(topology_type const& gg, const cell_type& c, const vertex_type& v) : base_topo(&gg) 
    {
       typename topology_traits<topology_type>::vertex_on_cell_iterator vocit(c);
       bool valid = false;
       
       while(vocit.valid())
       {
          if (v != *vocit)
          {
             handles.insert((*vocit).handle());
          }
          else 
          {
             valid = true;
          }
          ++vocit;
       }
       if (!valid) handles.clear();
       
       typename std::set<vertex_handle>::iterator iter;
       
       // [RH][TODO] .. create a meta program // fusion
       //
       iter = handles.begin();
       boost::tuples::get<0>(thehandle) = *iter; ++iter;
       boost::tuples::get<1>(thehandle) = *iter;
       if (BaseType::dimension_tag::dim == 3)
       {
          ++iter;
          boost::tuples::get<2>(thehandle) = *iter;
       }
    
       //std::cout << "handle: " << thehandle << std::endl;
    }
    
    facet_handle handle() const  {   return thehandle;  }
    
    bool operator<  (self_t const& other) const {return handle() < other.handle();  }
    bool operator== (self_t const& other) const {return handle() == other.handle(); }
    
    topology_type const& topo_anchor() const { return *base_topo;}
    
    friend std::ostream& operator<<(std::ostream& ostr, const simplex_facet& facet)
    {
       ostr << "simplex_facet: " << facet.handle();
          return ostr;
    }
    
 private:
    topology_type const*     base_topo;
    std::set<vertex_handle>  handles;
    facet_handle             thehandle;
 };


///////////////////////////////////////////////////////////////////////////
///
/// @brief structured facet for arbitrary dimension
///
///////////////////////////////////////////////////////////////////////////

template <typename BaseType>
class structured_facet :
    public boost::equality_comparable< structured_facet<BaseType>  >
  , boost::less_than_comparable< structured_facet<BaseType>  >

{
  typedef structured_facet<BaseType>                            self_t;
  typedef typename topology_traits<BaseType>::vertex_type       vertex_type;
  typedef typename topology_traits<BaseType>::cell_type         cell_type;
  typedef typename topology_traits<BaseType>::vertex_handle     vertex_handle;
  typedef typename topology_traits<BaseType>::facet_handle      facet_handle;
  typedef int                                                   mycounter;  
  
public:
  typedef facet_handle                                          handle_type;
  typedef typename topology_traits<BaseType>::facet_handle      handle_t;
  typedef BaseType                                              topology_type;
  typedef BaseType                                              base_type;

private:

  static const int dimension = topology_traits<topology_type>::dimension_tag::dim;

public:

   //
   // .. implicit/structured facets are always connected to the lowerleft vertex
   //    and are created by the dimensional direction as normal direction
   //

   structured_facet() : base_topo(0) {}
   explicit structured_facet(topology_type const& gg) : base_topo(&gg) {}
   
   explicit structured_facet(topology_type const& gg, facet_handle handle) : base_topo(&gg), thehandle(handle) 
   {
         
//          internal_vertex = vertex_type(gg, thehandle / dimension);
//          internal_dir = thehandle % dimension + 1;
      
      // [RH][TODO] .. implement compile time algorithmus
      //  -> structured topology:: get_index_vertex
      //
      internal_vertex = vertex_type(gg, boost::tuples::get<0>(thehandle));
      internal_dir = 1;   // [RH][TODO] .. recheck this
   }
   
   explicit structured_facet(cell_type const& cl, int dir)  
   {
      // negative faces are mapped to other cells
      // 
      //
      
      base_topo = &(cl.topo_anchor());
      
      boost::array<long, dimension> indices;
      indices = (*base_topo).get_cell_index(cl);
      internal_vertex = (*base_topo).get_index_vertex(indices);  // get the least-handeled vertex on the cell
      
//       std::cout << "cell: "<< cl << std::endl;
//       std::cout << "indices: "<< indices[0] << "/" << indices[1]  << std::endl;
//       std::cout << "internal vertex: "<< internal_vertex << std::endl;
      
      if (dir < 0)
      {
         // step into the direction of the 
         //
         internal_dir = -dir;
         structured_direction_n_iterator<dimension> iter(internal_vertex, internal_dir);
         ++iter;
         internal_vertex = *iter;
         std::cout << "internal vertex: "<< internal_vertex << std::endl;
         
      } 
      else
      {
         internal_dir = dir;
      }
      
// [RH][TODO]... implement here..
//

//          if (internal_dir == 1)  .. vertex_direction<0> iterator
//          vertex_handle v1 = indices[0]+1 ,  indices[1];
//          vertex_handle v2 = indices[0]+1 ,  indices[1]+1;

//          if (internal_dir == 2)
//          vertex_handle v1 = indices[0]+1 ,  indices[1];
//          vertex_handle v2 = indices[0]+1 ,  indices[1]+1;


      thehandle = facet_handle();
//          thehandle = facet_handle(0,0,0);
//          thehandle = internal_vertex.handle() * dimension + (internal_dir-1);
      }
   
  int direction() const { return internal_dir; }
  vertex_type const& base_vertex() const { return internal_vertex; }
  
  bool operator<  (self_t const& other) const {return handle() <  other.handle();}
  bool operator== (self_t const& other) const {return handle() == other.handle();}
  
  topology_type const& topo_anchor() const {return *base_topo;}
  
  facet_handle handle() const {  return thehandle; }
   
  friend std::ostream& operator<<(std::ostream& ostr, const structured_facet& structured_facet)
  {
     ostr << "structured_facet, handle = " << structured_facet.thehandle;
    return ostr;
  }

private:
  topology_type const* base_topo;
  vertex_type       internal_vertex;
  int               internal_dir;
  facet_handle      thehandle;
};


///////////////////////////////////////////////////////////////////////////
///
/// @brief structured facet for arbitrary dimension
///
///////////////////////////////////////////////////////////////////////////

template <typename BaseType>
class structured_edge :     
    public boost::equality_comparable< structured_edge<BaseType>  >
  , boost::less_than_comparable< structured_edge<BaseType>  >

{
   typedef structured_edge<BaseType>                             self_t;
   typedef typename topology_traits<BaseType>::vertex_type       vertex_type;
   typedef typename topology_traits<BaseType>::cell_type         cell_type;
   typedef typename topology_traits<BaseType>::vertex_handle     vertex_handle;
   typedef typename topology_traits<BaseType>::edge_handle       edge_handle;
   typedef int                                                   mycounter;  

public:

   typedef BaseType                                              topology_type;
   typedef typename topology_traits<BaseType>::edge_handle       handle_type;
   typedef typename topology_traits<BaseType>::edge_handle       handle_t;
   typedef BaseType                                              base_type;

private:
  static const int dimension = topology_traits<topology_type>::dimension;


   // requierements for call
   // 1) base_vertex
   // 2) direction
   //
   void make_both_vertices()
      {
         typename topology_traits<BaseType>::direction_n_iterator iter(internal_vertex, internal_dir);
         //++iter;
         //
         //// check validity
         ////
         //if (iter.valid())
         //{
         //   internal_vertex_2 = *iter;
         //}
      }

public:

  vertex_type          vertex1()     const {  return internal_vertex;}
  vertex_type          vertex2()     const {  return internal_vertex_2;}

  structured_edge() : g(0) {
     my_handle = -1;
     internal_handle = handle_t(-1,-1);

     internal_vertex=vertex_type();
     internal_vertex_2=vertex_type();
     internal_dir=-1;
     my_handle=-1;
     inv=false;                  // the internal direction of the edge
  }
   
   explicit
   structured_edge(topology_type const& gg) : g(&gg)
      {
         internal_vertex=vertex_type(gg);
         internal_vertex_2=vertex_type(gg);
         internal_dir=-1;
         my_handle=-1;
         internal_handle = handle_t(-1,-1);

         inv=false;                  // the internal direction of the edge
      }
   
   // the only creation method from "outside"
   //
   structured_edge(topology_type const& gg, handle_t const handle) : g(&gg)
   {
//      std::cout << "structured edge iterator.. constructor " << std::endl;
//      std::cout << "handle: " << handle << std::endl;

     my_handle =(boost::tuples::get<0>(handle));
     internal_handle = handle;

     if ((boost::tuples::get<0>(handle) != -1)  &&
	 (boost::tuples::get<1>(handle) != -1))
       //if (handle < 0)
     {
        internal_vertex  = vertex_type(gg, boost::tuples::get<0>(handle));
        internal_vertex_2  = vertex_type(gg, boost::tuples::get<1>(handle));

        internal_handle = handle_t(internal_vertex.handle(), internal_vertex_2.handle()  ) ;
//         make_sorted_handle();

        return;
     }

     // [RH] new handle data type .. from long to boost::tuple
     //
     // vertex_handle vh = (my_handle / dimension);
     vertex_handle vh = boost::tuples::get<0>(handle);
     internal_vertex  = vertex_type(gg, vh);
     

     // [RH] new handle data type .. from long to boost::tuple
     //   check this again
     //
     // internal_dir=(my_handle % dimension)+1;
     internal_dir=boost::tuples::get<0>(handle)+1;
     
     inv=false;                  // the internal direction of the edge
     internal_vertex_2 = vertex_type(gg, vh + gg.pointsdim_cumm[internal_dir-1]);
     

     internal_handle = handle_t(internal_vertex.handle(), internal_vertex_2.handle()  ) ;

//      std::cout << "structured edge iterator.. end " << std::endl;
//      make_sorted_handle();
     
  }
   

  // get interior information and handle from a vertex / direction 
  //
   structured_edge(vertex_type const& v, int dir, bool inv = true)  : inv(inv)
  {
    // negative faces are mapped to other vertices
    //
    g = &(v.topo_anchor());

    boost::array<long, dimension> indices;

    indices = (*g).get_vertex_index(v);

    internal_vertex = v;                 // the first poitn of the edge

    if (dir < 0)
      {
         
	// step into the direction of the 
	internal_dir = -dir;
	if (indices[internal_dir-1] == 0)
        {
           my_handle = -1;
           internal_handle = handle_t(-1,-1);
           return;
        }
	
	structured_direction_n_iterator<dimension> iter(internal_vertex, internal_dir);
	--iter;
	internal_vertex = *iter;
      } 
    else
    {
       internal_dir = dir;
    }

    // [RH][TODO] .. depricated.. remove the my_handle
    //
    my_handle = internal_vertex.handle() * dimension + (internal_dir-1);



    //  use the start point and the number of points within a dimension  
    //   for the second vertex
    //
    internal_vertex_2 = vertex_type((*g), internal_vertex.handle() + (*g).pointsdim_cumm[internal_dir-1]);


     internal_handle = handle_t(internal_vertex.handle(), internal_vertex_2.handle()  ) ;

//      make_sorted_handle();
 }


   void make_sorted_handle()
      {
         if (boost::tuples::get<0>(internal_handle) > boost::tuples::get<1>(internal_handle))
         {
//             std::cout << "make sorted.. true .. " << std::endl;

            internal_handle = handle_t( boost::tuples::get<1>(internal_handle) , boost::tuples::get<0>(internal_handle) ) ;
         }
         else
         {
//             std::cout << "make sorted.. false .. " << std::endl;

            internal_handle = handle_t( boost::tuples::get<0>(internal_handle) , boost::tuples::get<1>(internal_handle) ) ;
         }
      }


  bool inverted() const {return inv;}

   vertex_handle const handle1() const {return internal_vertex.handle();}
   vertex_handle const handle2() const {return internal_vertex_2.handle();}

  vertex_handle const handleo1() const {if (inv) return internal_vertex_2.handle(); else return internal_vertex.handle(); }
  vertex_handle const handleo2() const {if (inv) return internal_vertex.handle(); else return internal_vertex_2.handle(); }
  
  vertex_type     vertexo1()     const {  return vertex_type(topo_anchor(), handleo1());}
  vertex_type     vertexo2()     const {  return vertex_type(topo_anchor(), handleo2());}


  int direction()             const { return internal_dir; }
  vertex_type const& base_vertex() const { return internal_vertex; }
    
  bool operator<  (self_t const& other) const {return handle() <  other.handle();}
  bool operator== (self_t const& other) const {return handle() == other.handle();}
  
  topology_type const& topo_anchor()  const { return *g; }
  edge_handle handle()                const { return internal_handle; }

  friend std::ostream& operator<<(std::ostream& ostr, const structured_edge& edge)
  {
    ostr << "generic_structured_edge:" << "handle = "  << edge.internal_handle;
    return ostr;
  }

private:
   topology_type const*   g;
   vertex_type            internal_vertex;
   vertex_type            internal_vertex_2;
   int                    internal_dir;
   long                   my_handle;
   handle_t               internal_handle;
   bool                   inv;                  // the internal direction of the edge
   
};


}  // end of namespace::detail_topology

}  // end of namespace::gsse

#endif
