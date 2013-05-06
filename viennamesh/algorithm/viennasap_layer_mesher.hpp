#ifndef VIENNAMESH_ALGORITHM_VIENNASAP_LAYER_MESHER_HPP
#define VIENNAMESH_ALGORITHM_VIENNASAP_LAYER_MESHER_HPP

#include "viennagrid/config/default_configs.hpp"

#include "viennamesh/domain/viennasap_layer.hpp"
#include "viennamesh/algorithm/cgal_plc_mesher.hpp"

namespace viennamesh
{
    struct viennasap_layer_mesher_tag {};
    
    struct viennasap_layer_settings {};
    
    namespace result_of
    {
        template<>
        struct works_in_place<viennasap_layer_mesher_tag>
        {
            static const bool value = false;
        };

        
        template<typename domain_type_or_tag>
        struct best_matching_native_input_domain<viennasap_layer_mesher_tag, domain_type_or_tag>
        {
            typedef viennasap_layer_domain type;
        };

        template<typename domain_type_or_tag>
        struct best_matching_native_output_domain<viennasap_layer_mesher_tag, domain_type_or_tag>
        {
            typedef viennagrid::config::plc_3d_domain type;
        };

        
        template<>
        struct settings<viennasap_layer_mesher_tag>
        {
            typedef viennasap_layer_settings type;
        };

    }
    
    
    
    template<>
    struct native_algorithm_impl<viennasap_layer_mesher_tag>
    {
        
        template<typename native_input_domain_type, typename native_output_domain_type, typename settings_type>
        static bool run( native_input_domain_type const & native_input_domain, native_output_domain_type & native_output_domain, settings_type const & settings )
        {
            typedef typename native_input_domain_type::layer_type layer_type;
            typedef typename native_input_domain_type::interface_type interface_type;
            
            typedef typename viennagrid::result_of::point_type<native_output_domain_type>::type point_type;
            typedef typename viennagrid::result_of::handle<native_output_domain_type, viennagrid::vertex_tag>::type vertex_handle_type;
            typedef typename viennagrid::result_of::handle<native_output_domain_type, viennagrid::line_tag>::type line_handle_type;
            typedef typename viennagrid::result_of::element<native_output_domain_type, viennagrid::plc_tag>::type plc_type;
            
            std::deque< layer_type > const & layers = native_input_domain.layers;
            std::deque< interface_type > const & interfaces = native_input_domain.interfaces;
            std::deque< layer_type > const & interface_layers = native_input_domain.interface_layers;
            
            
            std::vector< std::map<typename layer_type::index_type, vertex_handle_type> > interface_vertex_handle_map( interfaces.size() );
            std::vector< std::map<typename layer_type::line_type, line_handle_type> > interface_line_handle_map( interfaces.size() );

            {
                
                for (unsigned int interface_index = 0; interface_index != interfaces.size(); ++interface_index)
                {
        //             double height = iit->second;
                    interface_type const & interface = interfaces[interface_index];
                    layer_type const & current_layer = interface_layers[interface_index];
                    
//                     viennagrid::config::line_2d_domain line_domain;
//                     current_layer.to_viennagrid_domain( line_domain );            
//                     viennamesh::result_of::settings<viennamesh::cgal_plc_2d_mesher_tag>::type settings;
//                     viennamesh::run_algo< viennamesh::cgal_plc_2d_mesher_tag >( line_domain, triangulated_interface_domains[interface_index], settings );
                    
                    
//                     {
//                         std::stringstream tmp;
//                         tmp << "interface_lines_" << interface_index << std::endl;
//                         viennagrid::io::vtk_writer<viennagrid::config::triangular_2d_domain, viennagrid::triangle_tag> vtk_writer;
//                         vtk_writer(triangulated_interface_domains[interface_index], tmp.str());
//                     }
                    
        //             current_layer.check();
        //             current_layer.print_lines();
                    
                    std::map<typename layer_type::index_type, vertex_handle_type> & vertex_handle_map = interface_vertex_handle_map[interface_index];
                    std::map<typename layer_type::line_type, line_handle_type> & line_handle_map = interface_line_handle_map[interface_index];
                    
                    for (typename layer_type::index_type pit = 0; pit != current_layer.points.size(); ++pit)
                        vertex_handle_map[pit] = viennagrid::create_vertex( native_output_domain, point_type( current_layer.points[pit][0], current_layer.points[pit][1], interface(current_layer.points[pit]) ) );
                    
                    std::vector<line_handle_type> plc_lines;
                    
                    for (typename layer_type::line_container_type::const_iterator lit = current_layer.lines.begin(); lit != current_layer.lines.end(); ++lit)
                    {
                        line_handle_type line_handle = viennagrid::create_line( native_output_domain, vertex_handle_map[lit->first], vertex_handle_map[lit->second] );
                        line_handle_map[*lit] = line_handle;
                        plc_lines.push_back( line_handle );
                    }
                    
                    vertex_handle_type vtx_handle;
                    point_type pt;
                    
                    viennagrid::create_plc( native_output_domain, plc_lines.begin(), plc_lines.end(), &vtx_handle, &vtx_handle, &pt, &pt );
                }
            }
            
            
    {
//         std::vector< std::pair<layer_type, layer_interface_type> >::iterator iit1 = interfaces.begin();
//         std::vector< std::pair<layer_type, layer_interface_type> >::iterator iit2 = iit1; ++iit2;
        
        for (unsigned int layer_index = 0; layer_index != layers.size(); ++layer_index)
        {
            layer_type const & current_layer = layers[layer_index];
            layer_type const & lower_interface_layer = interface_layers[layer_index];
            layer_type const & upper_interface_layer = interface_layers[layer_index+1];
            
//             lower_interface.check();
//             upper_interface.check();
            
            std::map<typename layer_type::index_type, line_handle_type> z_line_handle_map;
            
            std::map<typename layer_type::index_type, vertex_handle_type> & lower_vertex_handle_map = interface_vertex_handle_map[layer_index];
            std::map<typename layer_type::line_type, line_handle_type> & lower_line_handle_map = interface_line_handle_map[layer_index];

            std::map<typename layer_type::index_type, vertex_handle_type> & upper_vertex_handle_map = interface_vertex_handle_map[layer_index+1];
            std::map<typename layer_type::line_type, line_handle_type> & upper_line_handle_map = interface_line_handle_map[layer_index+1];

            
            for (typename layer_type::index_type pit = 0; pit != current_layer.points.size(); ++pit)
                z_line_handle_map[pit] = viennagrid::create_line( native_output_domain,
                                                                lower_vertex_handle_map[lower_interface_layer.get_point(current_layer.get_point(pit))],
                                                                upper_vertex_handle_map[upper_interface_layer.get_point(current_layer.get_point(pit))] );
            
            
            typename layer_type::line_container_type tmp_lines = current_layer.lines;
            for (typename layer_type::line_container_type::iterator it = tmp_lines.begin(); it != tmp_lines.end();)
            {
                std::vector<line_handle_type> plc_lines;
                
                typename layer_type::line_type const & line = *it;
                
                typename layer_type::point_type const & p0 = current_layer.get_point(line.first);
                typename layer_type::point_type const & p1 = current_layer.get_point(line.second);
                
                for (typename layer_type::line_container_type::const_iterator jt = lower_interface_layer.lines.begin() ; jt != lower_interface_layer.lines.end(); ++jt)
                {
                    if (viennagrid::geometry::point_ray_intersect( lower_interface_layer.get_point(jt->first), p0, p1, 1e-6 ) &&
                        viennagrid::geometry::point_ray_intersect( lower_interface_layer.get_point(jt->second), p0, p1, 1e-6 ))
                        plc_lines.push_back( lower_line_handle_map[*jt] );
                }
                
                for (typename layer_type::line_container_type::const_iterator jt = upper_interface_layer.lines.begin() ; jt != upper_interface_layer.lines.end(); ++jt)
                {
                    if (viennagrid::geometry::point_ray_intersect( upper_interface_layer.get_point(jt->first), p0, p1, 1e-6 ) &&
                        viennagrid::geometry::point_ray_intersect( upper_interface_layer.get_point(jt->second), p0, p1, 1e-6 ))
                        plc_lines.push_back( upper_line_handle_map[*jt] );
                }
                
                
                for (typename layer_type::index_type jt = 0; jt != current_layer.points.size(); ++jt)
                {
                    if (viennagrid::geometry::point_ray_intersect( current_layer.get_point(jt), current_layer.get_point(line.first), current_layer.get_point(line.second), 1e-6 ))
                        plc_lines.push_back( z_line_handle_map[jt] );
                }
                
                
//                 polygon_handle_type poly_handle;
                vertex_handle_type vtx_handle;
                point_type pt;
                
                viennagrid::create_plc( native_output_domain, plc_lines.begin(), plc_lines.end(), &vtx_handle, &vtx_handle, &pt, &pt );
                
                typename layer_type::line_container_type::iterator jt = it; ++jt;
                for (; jt != tmp_lines.end();)
                {
                    if (viennagrid::geometry::point_ray_intersect( current_layer.get_point(jt->first), p0, p1, 1e-6 ) &&
                        viennagrid::geometry::point_ray_intersect( current_layer.get_point(jt->second), p0, p1, 1e-6 ))
                        jt = tmp_lines.erase(jt);
                    else
                        ++jt;
                }
                
                it = tmp_lines.erase(it);
            }
        }
    }
    

    
    {
        unsigned int tmp_index = 0;
        std::map<unsigned int, typename layer_type::element_id_type> segment_index_map;
        
        double cur_z = 0.0;
        unsigned int layer_index;
        for (unsigned int layer_index; layer_index != layers.size(); ++layer_index)
        {
            layer_type const & current_laver = layers[layer_index];
            
            interface_type const & lower_interface = interfaces[layer_index];
            interface_type const & upper_interface = interfaces[layer_index+1];
            
//             double height = lit->second;
//             double middle = cur_z + height / 2.0;
            
            viennagrid::config::line_2d_domain line_domain_2d;
            typedef typename viennagrid::result_of::handle<viennagrid::config::line_2d_domain, viennagrid::vertex_tag>::type vertex_handle_type;
            
            std::map<typename layer_type::index_type, vertex_handle_type> vertex_handle_map;
            
            for (typename layer_type::index_type it = 0; it != current_laver.points.size(); ++it)
                vertex_handle_map[it] = viennagrid::create_vertex( line_domain_2d, current_laver.points[it] );
            
            for (typename layer_type::line_container_type::const_iterator it = current_laver.lines.begin(); it != current_laver.lines.end(); ++it)
                viennagrid::create_line( line_domain_2d, vertex_handle_map[it->first], vertex_handle_map[it->second] );
            
            viennagrid::config::triangular_2d_domain triangular_domain_2d;
            viennamesh::result_of::settings<viennamesh::cgal_plc_2d_mesher_tag>::type settings;
    
            viennamesh::run_algo< viennamesh::cgal_plc_2d_mesher_tag >( line_domain_2d, triangular_domain_2d, settings );
            
            typedef typename viennagrid::result_of::point_type<viennagrid::config::triangular_2d_domain>::type point_type_2d;
            typedef typename viennagrid::result_of::element_range<viennagrid::config::triangular_2d_domain, viennagrid::triangle_tag>::type triangle_range_type;
            typedef typename viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;
            
            triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( triangular_domain_2d );
            for (triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
            {
                point_type_2d const & p0 = viennagrid::point( triangular_domain_2d, viennagrid::elements<viennagrid::vertex_tag>(*it)[0] );
                point_type_2d const & p1 = viennagrid::point( triangular_domain_2d, viennagrid::elements<viennagrid::vertex_tag>(*it)[1] );
                point_type_2d const & p2 = viennagrid::point( triangular_domain_2d, viennagrid::elements<viennagrid::vertex_tag>(*it)[2] );
                
                point_type_2d center = (p0+p1+p2) / 3.0;
                double z = (lower_interface( point_type_2d(center[0], center[1]) ) + upper_interface( point_type_2d(center[0], center[1]) )) / 2.0;
                point_type center_3d( center[0], center[1], z );
                
                typename layer_type::element_id_type segment_id = current_laver.get_element_id( center );
                
//                 segment_seed_points[segment_id] = center_3d;
                
                
                segment_index_map[tmp_index] = segment_id;
                
                viennamesh::add_segment_seed_point( native_output_domain, segment_id, center_3d );
//                 viennamesh::add_segment_seed_point( triangle_domain_3d, tmp_index, center_3d );
                
                tmp_index++;
            }
            
            
//             cur_z += height;
        }
        
        
//         for (std::map<layer_type::element_id_type, point_type_3d>::iterator it = segment_seed_points.begin(); it != segment_seed_points.end(); ++it)
//         {
//             std::cout << "Adding seed point: " << it->second << " to segment " << it->first << std::endl;
//             viennamesh::add_segment_seed_point( triangle_domain_3d, it->first, it->second );
//         }
        
//         viennamesh::mark_face_segments( triangle_domain_3d );
    }
            
            
            
            return true;
        }
        
    };
    
    
}

#endif
