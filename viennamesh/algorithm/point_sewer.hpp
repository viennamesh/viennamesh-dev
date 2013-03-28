//     {
//         viennagrid::config::triangular_3d_domain sewed_hull_domain;
//         std::map<vertex_hook_type, vertex_hook_type> hook_hook_map;
//         
//         {
//             typedef std::deque< std::pair<point_type, vertex_hook_type> > point_hook_map_type;    
//             point_hook_map_type point_hook_map;
//             
//             
//             vertex_range_type vertices = viennagrid::elements<viennagrid::vertex_tag>( hull_domain );
//             std::cout << "Old num of vertices: " << vertices.size() << std::endl;
//             for (vertex_range_hook_iterator it = vertices.hook_begin(); it != vertices.hook_end(); ++it)
//             {
//                 vertex_type & vertex = viennagrid::dereference_hook( hull_domain, *it );
//                 point_type & point = viennagrid::point( hull_domain, vertex );
//                 
//                 point_hook_map_type::iterator pit = point_hook_map.begin();
//                 for (; pit != point_hook_map.end(); ++pit)
//                 {
//                     if ( viennagrid::norm_2( pit->first - point ) < 1e-6)
//                     {
//                         break;
//                         hook_hook_map.insert( std::make_pair( *it, pit->second ) );
//                     }
//                 }
//                 
//                 if ( pit == point_hook_map.end() )
//                 {
//                     vertex_hook_type new_vertex = viennagrid::create_vertex( sewed_hull_domain, point );
//                     
//                     point_hook_map.push_back( std::make_pair(point, new_vertex) );
//                     hook_hook_map.insert( std::make_pair(*it, new_vertex) );
//                 }
//             }
//         }
//         
//         std::cout << "Old num of vertices: " << viennagrid::elements<viennagrid::vertex_tag>( hull_domain ).size() << std::endl;
//         
//         triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( sewed_hull_domain );
//         for (triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
//         {
//             triangle_type & triangle = *it;
//             
//             vertex_hook_type vtx[3];
//             vtx[0] = hook_hook_map[viennagrid::elements<viennagrid::vertex_tag>(triangle).hook_at(0)];
//             vtx[1] = hook_hook_map[viennagrid::elements<viennagrid::vertex_tag>(triangle).hook_at(1)];
//             vtx[2] = hook_hook_map[viennagrid::elements<viennagrid::vertex_tag>(triangle).hook_at(2)];
//             
//             viennagrid::create_element<triangle_type>( sewed_hull_domain, vtx, vtx+3 );
//         }
//     }