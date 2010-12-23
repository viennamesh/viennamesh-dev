/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_TOPOLOGY_TRAITS_HH
#define GSSE_TOPOLOGY_TRAITS_HH


// *** system includes
//
#include<iostream>

// *** BOOST
//
#include <boost/logic/tribool.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/tuple/tuple_io.hpp>

// *** GSSE includes
//
#include <gsse/math/power.hpp>


   // ============================================================================================================
   //
   //                           GSSE implements the concept of a fiber bundle / sheaf
   //                      detailed information can be found at http://www.reneheinzl.at/phd/
   //
   //  The GSSE::topology represents the base space
   //          elements of the topology are simple indicies
   //          theoretically the topology models a manifold where the cw-complex properties and connectivity
   //          is stored in special fibers
   //
   // ============================================================================================================



namespace gsse
{
  enum GSSE_Property {directed, not_directed};

  enum GSSE_elem_type {cell_elem_type, facet_elem_type, edge_elem_type, vertex_elem_type};

   // compile time type information (an adapted but intrusive SFINAE principle)
   //  .. gsse data types contain a gsse_type data type
   //
   struct gsse_container{};


   // *** new gsse edge types
   struct gsse_orientation    {};
   struct gsse_orientation_directed : public gsse_orientation   {};
   struct gsse_orientation_undirected : public gsse_orientation {};


  struct orient_tri_on_tetra;
  struct orient_rect_on_cube;
  struct orient_edge_on_tri;
  struct orient_edge_on_rect;
  struct orient_vertex_on_edge;
  struct orient_false;
  template <typename F> struct commutate;



///////////////////////////////////////////////////////////////////////////
/// @brief Orientation check and change mechanisms
///
/// Require an OrientableObject and operator[] access
///
///////////////////////////////////////////////////////////////////////////

  template<typename OrientableObject>
  bool check_orientation(OrientableObject oo1, OrientableObject oo2)
  {
    long i, j;
    //long counter;
    for(i = 0; i <= 2; i++)
      {
	for(j = 0; j <= 2; j++)
	  {
	    // checks, if two out of n points are oriented consistently
	    //
	    if (oo1[(i+1)%3]  == oo2[(j+1)%3] &&    
		oo1[i]        == oo2[j])
	      {
// 		std::cout << "orient1: " << oo1[(i+1)%3] << "  orient2: " << oo2[(j+1)%3] << std::endl;
// 		std::cout << "orient1: " << oo1[i] << "  orient2: " << oo2[j] << std::endl;
// 		std::cout << "return true: "<< std::endl;
		return true;
	      }
// 	    if (oo1[i]  == oo2[j])
// 	      {
// 		++counter;
// 	      }

	  }
      }
    return false;
  }



  template<typename OrientableObject>
  bool check_oriented_neighbor(OrientableObject oo1, OrientableObject oo2)
  {
    long i, j;
    for(i = 0; i <= 2; i++)
      {
	for(j = 0; j <= 2; j++)
	  {
	    // checks, if two out of n points are oriented consistently
	    //
	    if (oo1[(i+1)%3]  == oo2[j] &&    
		oo1[i]        == oo2[(j+1)%3])
	      {
		return true;
	      }

	  }
      }
    return false;
  }

  template<typename OrientableObject>
  bool check_wrong_oriented_neighbor(OrientableObject oo1, OrientableObject oo2)
  {
    long i, j;
    for(i = 0; i <= 2; i++)
      {
	for(j = 0; j <= 2; j++)
	  {
	    // checks, if two out of n points are oriented consistently
	    //
	    if (oo1[(i+1)%3]  == oo2[(j+1)%3] &&    
		oo1[i]        == oo2[j])
	      {
		return true;
	      }
	  }
      }
    return false;
  }
  
  
  template<typename OrientableObject>
  void change_orientation(OrientableObject& oo)
  {
    std::swap(oo[0], oo[1]);
  }
  
  
namespace detail_topology 
{
  // *** forward declarations

  template <unsigned int dimension >  class unstructured;
  
  // [RH] .. new adaptive version
  template <unsigned int dimension >  class unstructured_adaptive;
  template <unsigned int dimension >  class structured;


  template <typename BasicType>   class generic_vertex;
  template <typename BasicType>   class generic_cell;
  template <typename BasicType>   class generic_edge;


// *** structured special objects
  template <typename BasicType>   class structured_vertex_iterator;
  template <typename BasicType>   class structured_cell_iterator;

  template <unsigned int dim>     class structured_vertex_on_cell_iterator;
  template <unsigned int dim>     class structured_cell_on_vertex_iterator; 
  template <unsigned int dim>     class structured_direction_n_iterator;
  template <unsigned int dim>     class structured_cell_direction_n_iterator;
  template <typename BasicType>   class structured_facet;
  template <typename BasicType>   class structured_edge;


// *** unstructured special objects
  template <typename BasicType>   class unstructured_vertex_iterator;
  template <typename BasicType>   class unstructured_cell_iterator;

  // [RH] .. new adaptive version
  //
  template <typename BasicType>   class unstructured_cell_iterator_adaptive;

  template <typename BasicType>   class unstructured_vertex_on_cell_iterator;
  template <typename BasicType>   class unstructured_cell_on_vertex_iterator;
  template <typename BasicType>   class simplex_facet;

// *** common iterators
  template <typename BasicType>   class vertex_on_edge_iterator;
  template <typename BasicType>   class cell_on_edge_iterator;
  template <typename BasicType>   class edge_on_vertex_iterator;
  
  template <typename BasicType>   class edge_on_cell_iterator;
  template <typename BasicType>   class structured_edge_on_cell_iterator;
  template <typename BasicType>   class facet_on_edge_iterator;
  template <typename BasicType>   class edge_on_facet_iterator;
  template <typename BasicType>   class facet_on_vertex_iterator;
  template <typename BasicType>   class facet_on_cell_iterator;
  template <typename BasicType>   class cell_on_facet_iterator;
  template <typename BasicType>   class vertex_on_facet_iterator;
  template <typename BasicType>   class vertex_on_vertex_iterator;
  template <typename BasicType>   class cell_on_boundary_iterator;
  template <typename BasicType>   class facet_on_boundary_iterator;
  template <typename BasicType>   class vertex_on_boundary_iterator;

  template <typename BasicType>   class structured_vertex_on_facet_iterator;
  template <typename BasicType>   class structured_cell_on_facet_iterator;
  template <typename BasicType>   class structured_facet_on_cell_iterator;
  template <typename BasicType>   class structured_vertex_on_edge_iterator;
  template <typename BasicType>   class structured_edge_on_cell_iterator;
  template <typename BasicType>   class structured_vertex_on_boundary_iterator;

  template <typename BasicType>   class structured_facet_on_edge_iterator;
  template <typename BasicType>   class structured_edge_on_facet_iterator;

  template <typename TopologyT, unsigned int Dimension> class structured_edge_on_vertex_direction_iterator;
  template <typename TopologyT, unsigned int Dimension> class structured_edge_on_vertex_direction_left_iterator;
  template <typename TopologyT, bool ordered>  class structured_edge_on_vertex_iterator;

  // *** the object handles
  template<class Topology>  struct vertex_handle_int;
  template<class Topology>  struct facet_handle_int;
  template<class Topology>  struct cell_handle_int;

  template<unsigned int dimension> class cell_2_vertex_indices;

} // end of namespace::detail_topology


/// ============================================================================================
/// *** Tag classes for the exterior interface
/// 
///
// ############

  // *** topology basic type tags: structured and unstructured
  struct topology_basic_tag {};
  struct structured_topology_tag   : public topology_basic_tag {};
  struct unstructured_topology_tag : public topology_basic_tag {};
                                               
   // [RH][V0.1] new tags
   //
   // topology_implicit:: models container with implicit topology (structured)
   // topology_explicit:: models container with explicit topology (unsttructured)
   //
   struct topology_tag {};
   struct topology_tag_implicit : public topology_tag {};
   struct topology_tag_explicit : public topology_tag {};


  // *** topology type tags: cuboid and simplex
  struct topology_type_tag {};
  struct simplex_topology_tag : public topology_type_tag {};     
  struct cuboid_topology_tag  : public topology_type_tag {};      

   // [RH][V0.1] new tags
   //
   struct cell_tag {};
   struct cell_tag_simplex : public cell_tag {};     
   struct cell_tag_ncube   : public cell_tag {};      


  // *** topology type tags: dimension
  template<unsigned int N>
  struct mesh_dim_tag  {enum {dim = N}; };
 

/// ============================================================================================
/// *** common declaration of the topology_traits
/// 
///

  template <typename DIM>
  struct topology_traits{};

  

/// ============================================================================================
/// *** partial template specialization for structured topologies
/// 
///
  template <unsigned int DIM>
  struct topology_traits< detail_topology::structured < DIM > >
  { 
     static const unsigned int dimension           = DIM;
     static const unsigned int dimension_topology  = DIM;

     static const unsigned int vertices = power<2, dimension_topology>::value;

    // *** general declarations, tag forwarding
     
     typedef structured_topology_tag                                           topology_tag;
     typedef cuboid_topology_tag                                               topology_type_tag;
     typedef detail_topology::structured<DIM>                                  topology_t;     // ### main type
     
     typedef typename topology_t::dimension_tag                                dimension_tag;
     typedef typename topology_t::dimension_tag                                dimension_topology_tag;
     
     // *** required dimensionensional objects		      
     //
     typedef typename topology_t::vertex_type                                  vertex_type;
     typedef typename topology_t::cell_type                                    cell_type;
     typedef typename topology_t::edge_type                                    edge_type;
     typedef typename topology_t::facet_type                                   facet_type;
     
     // *** handles
     //
     typedef typename topology_t::vertex_handle                                vertex_handle;
     typedef typename topology_t::cell_handle                                  cell_handle;
     typedef typename topology_t::edge_handle                                  edge_handle;
     typedef typename topology_t::facet_handle                                 facet_handle;

    
     // *** base iterators 				      
     //
     typedef typename topology_t::vertex_iterator                              vertex_iterator;
     typedef typename topology_t::cell_iterator                                cell_iterator;
     typedef typename topology_t::edge_iterator                                edge_iterator;
     typedef typename topology_t::facet_iterator                               facet_iterator;


     // *** derived iterators				      
     //
     typedef typename topology_t::vertex_on_cell_iterator                      vertex_on_cell_iterator;
     typedef typename topology_t::vertex_on_facet_iterator                     vertex_on_facet_iterator;
     typedef typename topology_t::vertex_on_edge_iterator                      vertex_on_edge_iterator;
     typedef typename topology_t::vertex_on_vertex_iterator                    vertex_on_vertex_iterator;

     typedef typename topology_t::edge_on_vertex_iterator                      edge_on_vertex_iterator;
     typedef typename topology_t::edge_on_facet_iterator                       edge_on_facet_iterator;
     typedef typename topology_t::edge_on_cell_iterator                        edge_on_cell_iterator;

     typedef typename topology_t::cell_on_vertex_iterator                      cell_on_vertex_iterator;
     typedef typename topology_t::cell_on_edge_iterator                        cell_on_edge_iterator;
     typedef typename topology_t::cell_on_facet_iterator                       cell_on_facet_iterator;
     
     typedef typename topology_t::facet_on_vertex_iterator                     facet_on_vertex_iterator;
     typedef typename topology_t::facet_on_edge_iterator                       facet_on_edge_iterator;
     typedef typename topology_t::facet_on_cell_iterator                       facet_on_cell_iterator;


     // *** special iterators
     //
     template <unsigned int Direction>
     struct edge_on_vertex_direction_iterator
     {
        typedef detail_topology::structured_edge_on_vertex_direction_iterator<topology_t, Direction> type;
     };
     
     template <unsigned int Direction>
     struct edge_on_vertex_direction_left_iterator
     {
        typedef detail_topology::structured_edge_on_vertex_direction_left_iterator<topology_t, Direction> type;
     };
     
     
     typedef detail_topology::structured_vertex_on_boundary_iterator<topology_t>  vertex_on_boundary_iterator;
     
     
     template <typename Element>
     struct vertex_on_element_iterator
     {
        static const bool is_vertex = boost::is_same<Element, vertex_type>::value;
        static const bool is_edge   = boost::is_same<Element, edge_type>::value;
        static const bool is_facet  = boost::is_same<Element, facet_type>::value;
        static const bool is_cell   = boost::is_same<Element, cell_type>::value;
        
        typedef typename boost::mpl::if_c<is_vertex, vertex_on_vertex_iterator,
              typename boost::mpl::if_c<is_edge, vertex_on_edge_iterator, 
              typename boost::mpl::if_c<is_facet, vertex_on_facet_iterator, 
              typename boost::mpl::if_c<is_cell, vertex_on_cell_iterator, void
					>::type >::type >::type >::type
      type;
    };

    template <typename Elem1, typename Elem2>
    struct orientation_func
    {
       // This metafunction creates the correct orientation function
       // for two different topological elements.
       //
      static const bool is_vertex1  = boost::is_same<Elem1, vertex_type>::value;
      static const bool is_edge1    = boost::is_same<Elem1, edge_type>::value;
      static const bool is_facet1   = boost::is_same<Elem1, facet_type>::value;
      static const bool is_cell1    = boost::is_same<Elem1, cell_type>::value;

      static const bool is_vertex2  = boost::is_same<Elem2, vertex_type>::value;
      static const bool is_edge2    = boost::is_same<Elem2, edge_type>::value;
      static const bool is_facet2   = boost::is_same<Elem2, facet_type>::value;
      static const bool is_cell2    = boost::is_same<Elem2, cell_type>::value;

      // get the cell facet orientation function

      typedef typename boost::mpl::if_c<dimension_topology == 1, orient_vertex_on_edge,
	      typename boost::mpl::if_c<dimension_topology == 2, orient_edge_on_rect,
              typename boost::mpl::if_c<dimension_topology == 3, orient_rect_on_cube,
					void
					>::type >::type >::type orient_facet_on_cell;

      // get the facet edge orientation function (if dim == 3)

      typedef typename boost::mpl::if_c<dimension_topology == 3, orient_edge_on_rect, orient_false
					>::type orient_edge_on_facet;


      typedef typename boost::mpl::if_c<is_vertex2 & is_edge1,  orient_vertex_on_edge,
	      typename boost::mpl::if_c<is_vertex1 & is_edge2,  commutate<orient_vertex_on_edge>,
	      typename boost::mpl::if_c<is_edge2   & is_facet1, orient_edge_on_facet,
	      typename boost::mpl::if_c<is_edge1   & is_facet2, commutate<orient_edge_on_facet>,
	      typename boost::mpl::if_c<is_facet2  & is_cell1,  orient_facet_on_cell,
	      typename boost::mpl::if_c<is_facet1  & is_cell2,  commutate<orient_facet_on_cell>,
					orient_false>::type >::type >::type >::type >::type >::type
      type;
            
    };


    // *** special iterators

    typedef detail_topology::structured_direction_n_iterator<DIM>         direction_n_iterator;
    typedef detail_topology::structured_cell_direction_n_iterator<DIM>    cell_direction_n_iterator;

    
  };

  
/// ============================================================================================
/// *** partial template specialization for unstructured topologies
/// 
///
// ############
   template<unsigned int DIM>
   struct topology_traits<detail_topology::unstructured<DIM> >  
   {

      static const unsigned int dimension           = DIM;
      static const unsigned int dimension_topology  = DIM;
      static const unsigned int vertices            = dimension_topology + 1;
      
      typedef detail_topology::cell_2_vertex_indices<DIM+1>    cell_2_vertex_mapping;       

     // *** general declarations, tag forwarding
      //
     typedef unstructured_topology_tag                         topology_tag;
     typedef simplex_topology_tag                              topology_type_tag;
     typedef detail_topology::unstructured<DIM>                topology_t;
  
     typedef typename topology_t::dimension_tag                                dimension_tag;
     typedef typename topology_t::dimension_tag                                dimension_topology_tag;
     
     // *** required dimensionensional objects		      
     //
     typedef typename topology_t::vertex_type                                  vertex_type;
     typedef typename topology_t::cell_type                                    cell_type;
     typedef typename topology_t::edge_type                                    edge_type;
     typedef typename topology_t::facet_type                                   facet_type;
     
     // *** handles
     //
     typedef typename topology_t::vertex_handle                                vertex_handle;
     typedef typename topology_t::cell_handle                                  cell_handle;
     typedef typename topology_t::edge_handle                                  edge_handle;
     typedef typename topology_t::facet_handle                                 facet_handle;

    
     // *** base iterators 				      
     //
     typedef typename topology_t::vertex_iterator                              vertex_iterator;
     typedef typename topology_t::cell_iterator                                cell_iterator;
     typedef typename topology_t::edge_iterator                                edge_iterator;
     typedef typename topology_t::facet_iterator                               facet_iterator;


     // *** derived iterators				      
     //
     typedef typename topology_t::vertex_on_cell_iterator                      vertex_on_cell_iterator;
     typedef typename topology_t::vertex_on_facet_iterator                     vertex_on_facet_iterator;
     typedef typename topology_t::vertex_on_edge_iterator                      vertex_on_edge_iterator;

     typedef typename topology_t::edge_on_vertex_iterator                      edge_on_vertex_iterator;
     typedef typename topology_t::edge_on_facet_iterator                       edge_on_facet_iterator;
     typedef typename topology_t::edge_on_cell_iterator                        edge_on_cell_iterator;

     typedef typename topology_t::cell_on_vertex_iterator                      cell_on_vertex_iterator;
     typedef typename topology_t::cell_on_edge_iterator                        cell_on_edge_iterator;
     typedef typename topology_t::cell_on_facet_iterator                       cell_on_facet_iterator;
     
     typedef typename topology_t::facet_on_vertex_iterator                     facet_on_vertex_iterator;
     typedef typename topology_t::facet_on_edge_iterator                       facet_on_edge_iterator;
     typedef typename topology_t::facet_on_cell_iterator                       facet_on_cell_iterator;


      // *** special iterators
      //
     typedef detail_topology::vertex_on_boundary_iterator<topology_t>    vertex_on_boundary_iterator;
     typedef detail_topology::vertex_on_vertex_iterator <topology_t>     vertex_on_vertex_iterator;


    template <typename Element>
    struct vertex_on_element_iterator
    {
      static const bool is_vertex = boost::is_same<Element, vertex_type>::value;
      static const bool is_edge = boost::is_same<Element, edge_type>::value;
      static const bool is_facet = boost::is_same<Element, facet_type>::value;
      static const bool is_cell = boost::is_same<Element, cell_type>::value;

      typedef typename boost::mpl::if_c<is_vertex, vertex_on_vertex_iterator,
              typename boost::mpl::if_c<is_edge, vertex_on_edge_iterator, 
              typename boost::mpl::if_c<is_facet, vertex_on_facet_iterator, 
              typename boost::mpl::if_c<is_cell, vertex_on_cell_iterator, void
					>::type >::type >::type >::type
      type;
    };



    template <typename Elem1, typename Elem2>
    struct orientation_func
    {
       // This metafunction creates the correct orientation function
       // for two different topological elements.
       //
      static const bool is_vertex1 = boost::is_same<Elem1, vertex_type>::value;
      static const bool is_edge1   = boost::is_same<Elem1, edge_type>::value;
      static const bool is_facet1  = boost::is_same<Elem1, facet_type>::value;
      static const bool is_cell1   = boost::is_same<Elem1, cell_type>::value;

      static const bool is_vertex2 = boost::is_same<Elem2, vertex_type>::value;
      static const bool is_edge2   = boost::is_same<Elem2, edge_type>::value;
      static const bool is_facet2  = boost::is_same<Elem2, facet_type>::value;
      static const bool is_cell2   = boost::is_same<Elem2, cell_type>::value;

       // get the cell facet orientation function
       //
       typedef typename boost::mpl::if_c<dimension_topology == 1, orient_vertex_on_edge,
	      typename boost::mpl::if_c<dimension_topology == 2, orient_edge_on_tri,
      	      typename boost::mpl::if_c<dimension_topology == 3, orient_tri_on_tetra,
					orient_false
					>::type >::type >::type orient_facet_on_cell;

       // get the facet edge orientation function (if dim == 3)
       //
       typedef typename boost::mpl::if_c<dimension_topology == 3, orient_edge_on_tri, orient_false
                                         >::type orient_edge_on_facet;


      typedef typename boost::mpl::if_c<is_vertex2 & is_edge1,  orient_vertex_on_edge,
	      typename boost::mpl::if_c<is_vertex1 & is_edge2,  commutate<orient_vertex_on_edge>,
	      typename boost::mpl::if_c<is_edge2   & is_facet1, orient_edge_on_facet,
	      typename boost::mpl::if_c<is_edge1   & is_facet2, commutate<orient_edge_on_facet>,
	      typename boost::mpl::if_c<is_facet2  & is_cell1,  orient_facet_on_cell,
	      typename boost::mpl::if_c<is_facet1  & is_cell2,  commutate<orient_facet_on_cell>,
					orient_false>::type >::type >::type >::type >::type >::type
      type;
            
    };

   };


//     template <typename Element>
//     struct vertex_on_element_iterator
//     {
//       static const bool is_vertex = boost::is_same<Element, vertex_type>::value;
//       static const bool is_edge = boost::is_same<Element, edge_type>::value;
//       static const bool is_facet = boost::is_same<Element, facet_type>::value;
//       static const bool is_cell = boost::is_same<Element, cell_type>::value;

//       typedef typename boost::mpl::if_c<is_vertex, vertex_on_vertex_iterator,
//               typename boost::mpl::if_c<is_edge, vertex_on_edge_iterator, 
//               typename boost::mpl::if_c<is_facet, vertex_on_facet_iterator, 
//               typename boost::mpl::if_c<is_cell, vertex_on_cell_iterator, void
// 					>::type >::type >::type >::type
//       type;
//     };


//     template <typename Elem1, typename Elem2>
//     struct orientation_func
//     {
//       // This metafunction creates the correct orientation function
//       // for two different topological elements.

//       static const bool is_vertex1 = boost::is_same<Elem1, vertex_type>::value;
//       static const bool is_edge1 = boost::is_same<Elem1, edge_type>::value;
//       static const bool is_facet1 = boost::is_same<Elem1, facet_type>::value;
//       static const bool is_cell1 = boost::is_same<Elem1, cell_type>::value;

//       static const bool is_vertex2 = boost::is_same<Elem2, vertex_type>::value;
//       static const bool is_edge2 = boost::is_same<Elem2, edge_type>::value;
//       static const bool is_facet2 = boost::is_same<Elem2, facet_type>::value;
//       static const bool is_cell2 = boost::is_same<Elem2, cell_type>::value;

//       // get the cell facet orientation function

//       typedef typename boost::mpl::if_c<dimension_topology == 1, orient_vertex_on_edge,
// 	      typename boost::mpl::if_c<dimension_topology == 2, orient_edge_on_tri,
// 	      typename boost::mpl::if_c<dimension_topology == 3, orient_tri_on_tetra,
// 					orient_false
// 					>::type >::type >::type orient_facet_on_cell;

//       // get the facet edge orientation function (if dim == 3)

//       typedef typename boost::mpl::if_c<dimension_topology == 3, orient_edge_on_tri, orient_false
// 					>::type orient_edge_on_facet;

//       typedef typename boost::mpl::if_c<is_vertex2 & is_edge1,  orient_vertex_on_edge,
// 	      typename boost::mpl::if_c<is_vertex1 & is_edge2,  commutate<orient_vertex_on_edge>,
// 	      typename boost::mpl::if_c<is_edge2   & is_facet1, orient_edge_on_facet,
// 	      typename boost::mpl::if_c<is_edge1   & is_facet2, commutate<orient_edge_on_facet>,
// 	      typename boost::mpl::if_c<is_facet2  & is_cell1,  orient_facet_on_cell,
// 	      typename boost::mpl::if_c<is_facet1  & is_cell2,  commutate<orient_facet_on_cell>,
// 					orient_false>::type >::type >::type >::type >::type >::type
//       type;
            
//     };
//    };



/// ============================================================================================
/// *** get_topology metafunction
/// 
///

template <unsigned int DIM, typename topology_tag, typename topology_basic_tag>
  struct get_topology {};
  
  template<unsigned int DIM>
  struct get_topology <DIM, cuboid_topology_tag, structured_topology_tag>
  { typedef detail_topology::structured<DIM> type; };

  template<unsigned int DIM>
  struct get_topology <DIM, simplex_topology_tag, unstructured_topology_tag>
  { typedef detail_topology::unstructured<DIM> type; };

// [RH][V0.1] topology metafunctions
//
  template<unsigned int DIM>
  struct get_topology <DIM, topology_tag_implicit, cell_tag_ncube>
  { typedef detail_topology::structured<DIM> type; };

  template<unsigned int DIM>
  struct get_topology <DIM, topology_tag_explicit, cell_tag_simplex>
  { typedef detail_topology::unstructured<DIM> type; };





  // /////////////////////////////////////////
  // new version [RH] 2005-11-06
  // 
//   template <unsigned int dim, typename topology_tag, typename topology_basic_tag>
//   struct get_topology_adaptive {};
  
//   template<unsigned int dim>
//   struct get_topology_adaptive <dim, simplex_topology_tag, unstructured_topology_tag>
//   { typedef detail_topology::unstructured_adaptive<dim> type; };
}


#endif
