/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GML_UTIL_TAGS
#define GML_UTIL_TAGS

namespace gml
{
// ...
struct tag_gsse_array {};
struct tag_gsse_metric_object {};
struct tag_gsse_map{};
struct tag_boost_array {};
struct tag_metric_object_rt {};
struct tag_metric_object {};
struct tag_fusion_vector {};
struct tag_fusion_list {};
struct tag_std_vector {};
struct tag_std_map {};
struct tag_std_pair {};
struct tag_std_set {};
struct tag_std_tr1_unordered_map {};
struct tag_boost_tuples_tuple {};
// ...
struct tag_compiletime{};
struct tag_runtime{};
// ...
struct tag_compiletime_container {};
struct tag_runtime_container {};
struct tag_sequence_container {};
struct tag_container_array{};
// ...
struct tag_none {};
// ...
struct cell_simplex{};
struct cell_cube{};
struct cell_sphere{};
// ...
struct gsse_container {};
// ...
}
#endif
