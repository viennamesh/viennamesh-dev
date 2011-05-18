/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at   

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GMI_UTILS_PROPERTY
#define GMI_UTILS_PROPERTY
//
// ===================================================================================
//
// *** BOOST includes
//
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/container/generation/make_list.hpp>
//
// ===================================================================================
//
namespace gmi
{
namespace property
{
//
// ===================================================================================
//
//    INSTANTIATED PROPERTY TAGS
//
// ===================================================================================
//
struct tag_mesh_property {};

struct ConformingDelaunay
{
   typedef void               container_t_type;
   typedef void               value_type;
};
ConformingDelaunay conforming_delaunay;
// ----------------------------------
struct ConformingGabriel
{
   typedef void               container_t_type;
   typedef void               value_type;
};
ConformingGabriel conforming_gabriel;
// ----------------------------------
struct ConstrainedDelaunay
{
   typedef void               container_t_type;
   typedef void               value_type;
};
ConstrainedDelaunay constrained_delaunay;
// ----------------------------------
struct Convex
{
   typedef void               container_t_type;
   typedef void               value_type;
};
Convex convex;
//
// ===================================================================================
//
//    PROPERTY TAGS WITH STATES
//
// ===================================================================================
//
struct size
{
   typedef void               container_t_type;
   typedef void               value_type;
   typedef double             Numeric;
   size(Numeric const& val) : _val(val) {}
   size(Numeric & val)      : _val(val) {}
   Numeric _val;
};
// ----------------------------------
struct angle
{
   typedef void               container_t_type;
   typedef void               value_type;
   typedef double             Numeric;
   angle(Numeric const& val) : _val(val) {}
   angle(Numeric & val)      : _val(val) {}
   Numeric _val;
};
// ----------------------------------
struct radius_edge_ratio
{
   typedef void               container_t_type;
   typedef void               value_type;
   typedef double             Numeric;
   radius_edge_ratio(Numeric const& val) : _val(val) {}
   radius_edge_ratio(Numeric & val)      : _val(val) {}
   Numeric _val;
};
// ----------------------------------
} // end namespace: property
} // end namespace: gmi
#endif
