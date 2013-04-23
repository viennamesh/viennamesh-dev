//     {
//         viennagrid::config::triangular_3d_domain sewed_hull_domain;
//         std::map<vertex_handle_type, vertex_handle_type> handle_handle_map;
//         
//         {
//             typedef std::deque< std::pair<point_type, vertex_handle_type> > point_handle_map_type;    
//             point_handle_map_type point_handle_map;
//             
//             
//             vertex_range_type vertices = viennagrid::elements<viennagrid::vertex_tag>( hull_domain );
//             std::cout << "Old num of vertices: " << vertices.size() << std::endl;
//             for (vertex_range_handle_iterator it = vertices.handle_begin(); it != vertices.handle_end(); ++it)
//             {
//                 vertex_type & vertex = viennagrid::dereference_handle( hull_domain, *it );
//                 point_type & point = viennagrid::point( hull_domain, vertex );
//                 
//                 point_handle_map_type::iterator pit = point_handle_map.begin();
//                 for (; pit != point_handle_map.end(); ++pit)
//                 {
//                     if ( viennagrid::norm_2( pit->first - point ) < 1e-6)
//                     {
//                         break;
//                         handle_handle_map.insert( std::make_pair( *it, pit->second ) );
//                     }
//                 }
//                 
//                 if ( pit == point_handle_map.end() )
//                 {
//                     vertex_handle_type new_vertex = viennagrid::create_vertex( sewed_hull_domain, point );
//                     
//                     point_handle_map.push_back( std::make_pair(point, new_vertex) );
//                     handle_handle_map.insert( std::make_pair(*it, new_vertex) );
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
//             vertex_handle_type vtx[3];
//             vtx[0] = handle_handle_map[viennagrid::elements<viennagrid::vertex_tag>(triangle).handle_at(0)];
//             vtx[1] = handle_handle_map[viennagrid::elements<viennagrid::vertex_tag>(triangle).handle_at(1)];
//             vtx[2] = handle_handle_map[viennagrid::elements<viennagrid::vertex_tag>(triangle).handle_at(2)];
//             
//             viennagrid::create_element<triangle_type>( sewed_hull_domain, vtx, vtx+3 );
//         }
//     }