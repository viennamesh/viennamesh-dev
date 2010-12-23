/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_UNSTRUCTURED_TOPOLOGY_ID
#define GSSE_UNSTRUCTURED_TOPOLOGY_ID

// *** system includes
//
#include<algorithm>

// *** BOOST includes
//
#include <boost/array.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/int.hpp>

// *** GSSE includes
//
#include "gsse/topology.hpp"
#include "gsse/topology/topology_handles.hpp"
#include "gsse/topology/derived_iterator.hpp"
#include "gsse/topology/boundary_iterator.hpp"
#include "gsse/topology/unstructured_iterator.hpp"
#include "gsse/topology/derived_objects.hpp"




namespace gsse
{

///////////////////////////////////////////////////////////////////////////
/// @brief Unstructured n-dimensional helper class
///
/// For an easier access to cell-point indices
///
///////////////////////////////////////////////////////////////////////////

namespace detail_topology 
{


template<unsigned int internal_size>
class cell_2_vertex_indices
{
  typedef long numeric_t;
  boost::array<numeric_t, internal_size> index_v;
  typedef cell_2_vertex_indices<internal_size> self_t;

public:

  // for backward compatiblity -> user defined constructors

  cell_2_vertex_indices()   {
 }


  //< user-convenient constructor for 1D
  cell_2_vertex_indices(long pp1, long pp2)   
  {
    index_v[0] = pp1;
    index_v[1] = pp2;
  }

  //< user-convenient constructor for 2D
  cell_2_vertex_indices(long pp1, long pp2, long pp3)   
  {
    index_v[0] = pp1;
    index_v[1] = pp2;
    index_v[2] = pp3;
  }

  //< user-convenient constructor for 3D
  cell_2_vertex_indices(long pp1, long pp2, long pp3, long pp4)   
  {
    index_v[0] = pp1;
    index_v[1] = pp2;
    index_v[2] = pp3;
    index_v[3] = pp4;
  }

  //<  generic constructor
  template<typename InputIterator>
  cell_2_vertex_indices(InputIterator first, InputIterator last)
  {
   long index=0;
    while (first != last)
      {
	index_v[index++] = *first++;
      }
  }

 
   unsigned int size()            const { return internal_size; }
   numeric_t operator[](int i) const 
      { 
         //std::cout << i << std::endl;
         return index_v[i];  
      }

   numeric_t& operator[](int i) 
      { 
         return index_v[i];  
      }


   // High Peformance Variant
   //unsigned int operator[](int i) const { if (i == 0) return i1; else return i2;  }
	
   friend std::ostream& operator<<(std::ostream& ostr, const cell_2_vertex_indices cpi)
      {
         // ostr << "cell_2_vertex_indices " << cpi[0] << "/" << cpi[1];
         return ostr;
      }
};



///////////////////////////////////////////////////////////////////////////
/// @brief Generic unstructured topology class
///
///////////////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////////////
  /// !brief vertex index container
  ///
  /// global container for storing vertices with corresponding cell index
  ///   vertexIndicies vector[ vertex index , set [cell indices ] ]  =   [ [ 0 ] ->  [  1, 2, 3 ..] || .. ]
  ///
  ///////////////////////////////////////////////////////////////////////////


template<unsigned int DIM>
class unstructured
{
  typedef  unstructured<DIM>       self_t;


public:
   typedef gsse_container                                                 gsse_type;     // type information [RH] 200801

   typedef cell_2_vertex_indices<DIM+1>                                   cell_2_vertex_mapping;
   typedef mesh_dim_tag<DIM>                                              dimension_tag;
   typedef mesh_dim_tag<DIM>                                              dimension_topology_tag;    

   
   typedef detail_topology::generic_vertex<self_t>                        vertex_type;
   typedef detail_topology::generic_cell<self_t>                          cell_type;
   typedef detail_topology::generic_edge<self_t>                          edge_type;			      
   typedef detail_topology::simplex_facet<self_t>                         facet_type;

   typedef vertex_handle_int<self_t>                                      vertex_handle;
   typedef cell_handle_int<self_t>                                        cell_handle;
   typedef boost::tuples::tuple<vertex_handle, vertex_handle>             edge_handle;

   
   typedef typename
   boost::mpl::if_
   <
      typename boost::is_same<boost::mpl::int_<DIM>, boost::mpl::int_<2> >, 
      boost::tuples::tuple<vertex_handle, vertex_handle, int>,     // [RH][TODO] ..use another type ..  !!! HACK !!!! for dispatching..
      boost::tuples::tuple<vertex_handle, vertex_handle, vertex_handle>
   > :: type facet_handle;  

   typedef detail_topology::unstructured_vertex_iterator<self_t>          vertex_iterator;
   typedef detail_topology::unstructured_cell_iterator<self_t>            cell_iterator;
 
   // sub-topology object .. GSSE stores only X^0 and X^n 
   //
   typedef          std::set<edge_type>                                   edge_container_t;
   typedef          std::set<facet_type>                                  facet_container_t;

   typedef typename edge_container_t::iterator                            edge_iterator;
   typedef typename facet_container_t::iterator                           facet_iterator;


   // *** base iterators
   //
   typedef detail_topology::unstructured_vertex_on_cell_iterator<self_t>   vertex_on_cell_iterator;
   typedef detail_topology::unstructured_cell_on_vertex_iterator<self_t>   cell_on_vertex_iterator;

   // *** derived iterators
   //
   typedef detail_topology::vertex_on_facet_iterator<self_t>               vertex_on_facet_iterator;
   typedef detail_topology::vertex_on_edge_iterator <self_t>               vertex_on_edge_iterator;
   
   typedef detail_topology::edge_on_cell_iterator<self_t>                  edge_on_cell_iterator;
   typedef detail_topology::edge_on_facet_iterator<self_t>                 edge_on_facet_iterator;  
   typedef detail_topology::edge_on_vertex_iterator<self_t>                edge_on_vertex_iterator;
   
   typedef detail_topology::facet_on_vertex_iterator<self_t>               facet_on_vertex_iterator;
   typedef detail_topology::facet_on_edge_iterator<self_t>                 facet_on_edge_iterator;  
   typedef detail_topology::facet_on_cell_iterator<self_t>                 facet_on_cell_iterator;
      
   typedef detail_topology::cell_on_edge_iterator<self_t>                  cell_on_edge_iterator;
   typedef detail_topology::cell_on_facet_iterator<self_t>                 cell_on_facet_iterator;



// [RH][TODO].. check the friend
//
//private:
//  friend class topology_traits<self_t>::cell_on_vertex_iterator; 

  typedef std::set<unsigned int>                                cell_reference_container;
  typedef std::pair<vertex_handle, cell_reference_container >   vertex_index_entry;
  typedef std::vector<vertex_index_entry>                       vertex_indices_container;
  typedef typename vertex_indices_container::iterator           vertex_indices_container_iterator;
  typedef typename vertex_indices_container::const_iterator     vertex_indices_container_const_iterator;
  typedef typename cell_reference_container::const_iterator     set_int_const_iterator;

     
public:

  // default constructor, copy constructor, destructor work fine here


   unstructured() {max_handle = 0; cell_indices.resize(0); set_cell_index_offset(0);}

   // for a high performance approach
   // HP_ID
   void set_topo_size(long vertex_count)
      {
         vertex_indices.resize(vertex_count);
         for (long i =0; i < vertex_count; ++i)
            vertex_indices[i]=vertex_index_entry(-1  , cell_reference_container());
      }

  // !!!  I have to optimize that [RH]   -> 1st try .. optimized 2006-01-15 [RH]
  // .. first approach with an additional map
  //    works much faster than the original one .. anyway, it is still to slow
  //    
  cell_handle add_cell_2(const cell_2_vertex_mapping& cpi)
  {
    int cell_size = cell_indices.size();
    typename std::map<vertex_handle, int>::iterator vh_it;

      for (unsigned int dim = 0; dim < DIM +1; ++dim)  // only for simplices
      {
           vh_it = vh_2_ci_m.find(cpi[dim]) ;

           //std::cout << cpi[dim] << std::endl;
           if (max_handle < cpi[dim]) max_handle = cpi[dim];

           if (vh_it == vh_2_ci_m.end())  // current cpi(dim) not found
           {    
                vh_2_ci_m[cpi[dim]]=vertex_indices.size();

                vertex_indices.push_back( vertex_index_entry(cpi[dim], cell_reference_container() ) );
                vertex_indices[vertex_indices.size()-1].second.insert(cell_size);
           }
           else // current cpi(dim) was found
           {   
                 vertex_indices[(*vh_it).second].second.insert(cell_size);
           }
       }
       cell_indices.push_back(cpi);
       return cell_indices.size()-1;
   }

  unsigned long add_cell_2r(const cell_2_vertex_mapping& cpi)
  {
    int cell_size = cell_indices.size();
    typename std::map<vertex_handle, int>::iterator vh_it;

      for (unsigned int dim = 0; dim < DIM +1; ++dim)  // only for simplices
      {
           vh_it = vh_2_ci_m.find(cpi[dim]) ;

           //std::cout << cpi[dim] << std::endl;
           if (max_handle < cpi[dim]) max_handle = cpi[dim];

           if (vh_it == vh_2_ci_m.end())  // current cpi(dim) not found
           {
                vh_2_ci_m[cpi[dim]]=vertex_indices.size();

                vertex_indices.push_back( vertex_index_entry(cpi[dim], cell_reference_container() ) );
                vertex_indices[vertex_indices.size()-1].second.insert(cell_size);
           }
           else // current cpi(dim) was found
           {
                 vertex_indices[(*vh_it).second].second.insert(cell_size);
           }
       }
       cell_indices.push_back(cpi);
       return cell_indices.size() - 1;
   }


  //
  // !!!  -> 2nd try .. optimized 2006-01-15 [RH]
  // this works much faster but the data to be processed is still to much
  // 
  void add_cell_3(const cell_2_vertex_mapping cpi, const cell_2_vertex_mapping cpi_index)
  {
	  int cell_size = cell_indices.size();
/*	  std::cout << "cpi: " << cpi << std::endl;
	  std::cout << "cpi2: " << cpi_index << std::endl;
	  std::cout << "size: "<< vertex_indices.size() << std::endl;
	  std::cout << "cap: " << vertex_indices.capacity() << std::endl;
*/	  vertex_indices[cpi_index[0]].second.insert(cell_size);
	  vertex_indices[cpi_index[0]].first = cpi[0];
	  vertex_indices[cpi_index[1]].second.insert(cell_size);
	  vertex_indices[cpi_index[1]].first = cpi[1];
	  vertex_indices[cpi_index[2]].second.insert(cell_size);
	  vertex_indices[cpi_index[2]].first = cpi[2];
	  vertex_indices[cpi_index[3]].second.insert(cell_size);
	  vertex_indices[cpi_index[3]].first = cpi[3];

	  cell_indices.push_back(cpi);
  }

   unsigned int get_cell_size_internal()   const { return cell_indices.size(); }
   unsigned int get_vertex_size_internal() const { return vertex_indices.size();  }
   unsigned int get_max_handle()           const { return max_handle + 1;  }
   

  // --------   generic part ----------------

  unsigned int cell_size()               const { return cell_indices.size(); }
  unsigned int vertex_size()             const { return vertex_indices.size(); } 

  unsigned int edge_size()               const { return edge_container.size(); } 
  unsigned int facet_size()              const { return facet_container.size();}

  inline vertex_iterator vertex_begin () const { return vertex_iterator(*this,0); }
  inline vertex_iterator vertex_end   () const { return vertex_iterator(*this, -1); }
  inline cell_iterator   cell_begin   () const { return cell_iterator(*this);}
  inline cell_iterator   cell_end     () const { return cell_iterator(*this, -1);}



   // temporary iterators
   //
   void build_up_edge_container()
   {
	   for (cell_iterator c_it = cell_begin(); c_it != cell_end(); ++c_it)
	   {
		   for (edge_on_cell_iterator eoc_it(*c_it); eoc_it.valid(); ++eoc_it)
		   {
			   edge_container.insert( *eoc_it );
		   }
	   }

   } 
// [RH][TODO]  const and so on..
//
   edge_iterator   edge_begin()  const  {   return edge_container.begin();   }    // !! here the build_up must be called before !! 
   edge_iterator   edge_begin()     
   { 
//	   build_up_edge_container();
	   return edge_container.begin(); 
   }
   edge_iterator   edge_end()     const  { return edge_container.end(); }


   // ===========================

   void build_up_facet_container()
   {
	   for (cell_iterator c_it = cell_begin(); c_it != cell_end(); ++c_it)
	   {
		   for (facet_on_cell_iterator foc_it(*c_it); foc_it.valid(); ++foc_it)
		   {
			   facet_container.insert( *foc_it );
		   }
	   }

   }
// [RH][TODO]
   facet_iterator   facet_begin() const {   return facet_container.begin();    }  // !! here the build_up must be called before.. !! 
   facet_iterator   facet_begin()     
   { 
//	   build_up_facet_container();
	   return facet_container.begin(); 
   }
   facet_iterator   facet_end()     const  { return facet_container.end(); }







  // ----------------------------------------

  //
  //  convenient methods
  //
   
   vertex_handle get_vertex_index(unsigned int index) const    
      { return vertex_indices[index].first; }
   
   vertex_handle get_vertex_index_from_cell(cell_handle ch, unsigned int index) const
      { 
	//std::cout << "offset:"  << cell_index_offset << std::endl;
  	//std::cout << "ch:"  << ch << std::endl;
	//ch -= cell_index_offset;
//  	std::cout << "my adress:"  << this << std::endl;
/*	if (cell_indices[ch].size() <= index ) std::cout << "ERROR with topology access: vertex on cell: index:" 
	<< index << " size: "<< cell_indices[ch].size()  << std::endl;
        if (cell_indices.size() <= unsigned(ch))        std::cout << "ERROR with topology access: ch:" << ch << " size: " << cell_indices.size()<< std::endl;
*/
//  	std::cout << "cell indices size: " << cell_indices.size() << std::endl;
//  	std::cout << "cell indices [] size: " << cell_indices[ch].size() << std::endl;


        //std::cout << "Index: " << index << std::endl;
        //std::cout << "Cell Handle " << ch << std::endl;

         return cell_indices[ch][index]; 
      }
   
   bool operator==(const self_t& other) const { return this == &other; } 
   
   // only for now.. maybe we can eliminate this
   vertex_type add_user_point(const vertex_handle& vh)
      { 
         vertex_indices.push_back( vertex_index_entry(vh, cell_reference_container() ) );
         return vertex_type(*this, vh);
      }
  
  cell_2_vertex_mapping& get_cell(cell_handle ch)
  {
	ch -= cell_index_offset;
    return cell_indices[ch];
  }

   void set_cell_index_offset(long offset)
      {
         cell_index_offset = offset;
      }

   long get_cell_index_offset() const
      {
        return cell_index_offset;
      }



   // =======================================================
   //
   template<typename VHandleType>
   struct vh_functor
   {
      vh_functor(VHandleType vh):vh(vh) {}

      template <typename Arg>
      bool operator()(Arg arg1)
         {
            if (vh == arg1.first)
               return true;
            return false;
         }

      VHandleType vh;

   };

   bool is_vertex_handle_inside(const vertex_handle& vh) const
      {
         vertex_indices_container_const_iterator vh_inside_it =   
            std::find_if (vertex_indices.begin(), 
                          vertex_indices.end(), 
                          vh_functor<vertex_handle>(vh) );
         if (vh_inside_it != vertex_indices.end())
            return true;

         return false;
      }

   bool is_cell_handle_inside(const cell_handle& ch) const
      {
//         ch -= cell_index_offset;
         unsigned long ch_long(ch);
         if (ch_long < cell_indices.size())
            return true;
         
         return false;
      }

   //
   // =======================================================


  void erase_cell(cell_handle ch)
  {
    long index_array[3];
    index_array[0] = cell_indices[ch][0];
    index_array[1] = cell_indices[ch][1];
    index_array[2] = cell_indices[ch][2];
    std::cout << "01: " << index_array[0] << std::endl;
    std::cout << "02: " << index_array[1] << std::endl;
    std::cout << "03: " << index_array[2] << std::endl;
    // only correct for one segment
    //
    std::cout << "erasing01.. " << std::endl;
    if ( vertex_indices[index_array[0]].second.find(ch) != vertex_indices[index_array[0]].second.end())
      vertex_indices[index_array[0]].second.erase(ch);
    if ( vertex_indices[index_array[1]].second.find(ch) != vertex_indices[index_array[1]].second.end())
      vertex_indices[index_array[1]].second.erase(ch);
    if ( vertex_indices[index_array[2]].second.find(ch) != vertex_indices[index_array[2]].second.end())
      vertex_indices[index_array[2]].second.erase(ch);


    std::cout << "erasing.. 2" << std::endl;
    cell_indices[ch] = cell_2_vertex_mapping();  // erase the information from the cell


  }

  std::ostream& output(std::ostream& ostr)  const
  {
    ostr << "ust: " << this << std::endl;
    return ostr;
  }

   friend std::ostream& operator<<(std::ostream& ostr, const unstructured<DIM>& tri2d)
      {
	ostr  << " single ust operator<< : " << tri2d.output(ostr) << std::endl;
         return ostr;
      }





   // ######################################################################
   //
   //  topological traversal functors
   // 
   template<typename TopologyT>
   struct traverse_vertex
   {
      typedef typename TopologyT::vertex_iterator iterator;
      typedef typename TopologyT::vertex_type     value_type;
      
      traverse_vertex(TopologyT& topology) : topology(topology) {}
      
      iterator begin() { return topology.vertex_begin();}
      iterator end()   { return topology.vertex_end(); }
   protected:
      TopologyT& topology;
   };

   template<typename TopologyT>
   struct traverse_cell
   {
      typedef typename TopologyT::cell_iterator iterator;
      typedef typename TopologyT::cell_type     value_type;
      
      traverse_cell(TopologyT& topology) : topology(topology) {}
      
      iterator begin() { return topology.cell_begin();}
      iterator end()   { return topology.cell_end(); }
   protected:
      TopologyT& topology;
   };


   // ----
   template<typename TopologyT>
   struct traverse_edge
   {
      typedef typename TopologyT::edge_iterator iterator;
      typedef typename TopologyT::edge_type     value_type;
      
      traverse_edge(TopologyT& topology) : topology(topology) {   }
      
      iterator begin() { return topology.edge_begin();}
      iterator end()   { return topology.edge_end(); }
   protected:
      TopologyT& topology;
   };

   template<typename TopologyT>
   struct traverse_facet
   {
      typedef typename TopologyT::facet_iterator iterator;
      typedef typename TopologyT::facet_type     value_type;
      
      traverse_facet(TopologyT& topology) : topology(topology) {  }
      
      iterator begin() { return topology.facet_begin();}
      iterator end()   { return topology.facet_end(); }
   protected:
      TopologyT& topology;
   };


   typedef boost::mpl::map<
      boost::mpl::pair<boost::mpl::pair<self_t, vertex_type>,  traverse_vertex<self_t> >,
      boost::mpl::pair<boost::mpl::pair<self_t, cell_type>,    traverse_cell  <self_t> >,
      boost::mpl::pair<boost::mpl::pair<self_t, edge_type>,    traverse_edge  <self_t> >,
      boost::mpl::pair<boost::mpl::pair<self_t, facet_type>,   traverse_facet <self_t> >
      > traversal_repository;



// ############################################
//
// [RH][TODO] .. friends data type problem
//private:
   
   std::vector<cell_2_vertex_mapping>      cell_indices;
   vertex_indices_container                vertex_indices;
   
   // a new vector is used to translate the constant index entries from
   // the iterators from one segment to the real index.. only for
   // convenience and speed up of -> dd_cell .. vertex handle 2 current
   // index map ..  HP_ID
   //
   std::map<vertex_handle, int>   vh_2_ci_m;
   

   long cell_index_offset;   //200712 .. new for global domain quantities / cell indices
   long max_handle;


   // object iterators
   //
   edge_container_t                       edge_container;
   facet_container_t                      facet_container;


};

} // end of namespace::detail_topology

} // end of namespace::gsse

#endif
