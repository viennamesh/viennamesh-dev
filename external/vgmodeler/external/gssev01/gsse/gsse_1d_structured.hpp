/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_CVL_1D_S_HH
#define GSSE_CVL_1D_S_HH

/// *** GSSE includes
// 
#include "gsse/gsse_xd_basic_part.hpp"
#include "gsse/topology/structured_topology.hpp"




namespace gsse
{

// *** construct the domain type

typedef get_topology<1, cuboid_topology_tag, structured_topology_tag>::type     topology_t;
typedef get_domain<topology_t, double, double>::type                            domain_t;
typedef domain_traits<domain_t>::segment_t                                      segment_t;

// *** derived file definitions

typedef domain_traits<domain_t>::segment_t                      segment_t;
typedef domain_traits<domain_t>::segment_iterator               segment_iterator;

typedef segment_traits<segment_t>::vertex_type                  vertex_type;
typedef segment_traits<segment_t>::cell_type                    cell_type;
typedef segment_traits<segment_t>::edge_type                    edge_type;

typedef topology_traits<topology_t>::vertex_iterator            vertex_iterator;
typedef topology_traits<topology_t>::cell_iterator              cell_iterator;

typedef topology_traits<topology_t>::vertex_on_edge_iterator    vertex_on_edge_iterator;
typedef topology_traits<topology_t>::vertex_on_facet_iterator   vertex_on_facet_iterator;
typedef topology_traits<topology_t>::vertex_on_cell_iterator    vertex_on_cell_iterator;
                                                                                    
typedef topology_traits<topology_t>::edge_on_vertex_iterator    edge_on_vertex_iterator;
typedef topology_traits<topology_t>::edge_on_facet_iterator     edge_on_facet_iterator;
typedef topology_traits<topology_t>::edge_on_cell_iterator      edge_on_cell_iterator;
                                                                                  
typedef topology_traits<topology_t>::facet_on_edge_iterator     facet_on_edge_iterator;
typedef topology_traits<topology_t>::facet_on_vertex_iterator   facet_on_vertex_iterator;
typedef topology_traits<topology_t>::facet_on_cell_iterator     facet_on_cell_iterator;
                                                                                   
typedef topology_traits<topology_t>::cell_on_edge_iterator      cell_on_edge_iterator;
typedef topology_traits<topology_t>::cell_on_facet_iterator     cell_on_facet_iterator;
typedef topology_traits<topology_t>::cell_on_vertex_iterator    cell_on_vertex_iterator;

//typedef topology_traits<segment_t>::vertex_on_boundary_iterator  vertex_on_boundary_iterator;
//typedef topology_traits<segment_t>::facet_on_boundary_iterator   facet_on_boundary_iterator;

//typedef segment_traits<segment_t>::edge_on_vertex_direction_left_iterator<1>::type vertex_edge_left;
typedef segment_traits<segment_t>::storage_type                               storage_type;

}  // end namespace gsse

#endif
