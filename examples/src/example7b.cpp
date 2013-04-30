#include <iostream>

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/algorithm/intersect.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "viennamesh/algorithm/viennasap_layer_mesher.hpp"
#include "viennamesh/algorithm/vgmodeler_hull_adaption.hpp"
#include "viennamesh/algorithm/netgen_tetrahedron_mesher.hpp"



#include "viennamesh/io/viennasap_common.hpp"


template<typename domain_type, typename vertex_handle_map_type, typename stream_type, typename interface_type>
void write_flat_contact( domain_type & domain, vertex_handle_map_type & vertex_handle_map, stream_type & stream, interface_type const & interface)
{
    typedef typename viennagrid::result_of::point_type< domain_type >::type point_type;
    typedef typename viennagrid::result_of::handle< domain_type, viennagrid::vertex_tag >::type vertex_handle_type;
    
    typedef typename viennagrid::result_of::element_range< domain_type, viennagrid::triangle_tag >::type triangle_range_type;
    typedef typename viennagrid::result_of::iterator< triangle_range_type >::type triangle_range_iterator;
    

    
    {
        typedef boost::tuple<vertex_handle_type, vertex_handle_type, vertex_handle_type> tuple_triangle;
        std::deque< tuple_triangle > boundary_condition_triangles;
        
        triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( domain );
        for (triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
        {
            point_type const & p0 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[0] );
            point_type const & p1 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[1] );
            point_type const & p2 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[1] );
            
//             typedef viennasap_layer_domain_type::point_type point_type_2d;
            
            if ( (std::abs( interface(p0[0], p0[1]) - p0[2] ) < 1e-6) &&
                (std::abs( interface(p1[0], p1[1]) - p1[2] ) < 1e-6) &&
                (std::abs( interface(p2[0], p2[1]) - p2[2] ) < 1e-6) )
            {
                boundary_condition_triangles.push_back( tuple_triangle( viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(0),
                                                                        viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(1),
                                                                        viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(2) ) );
            }
        }
        
        stream << "    objdesc { struct { label{int 1} type{string 1} nodeRef{ int 1}* } numobj{" << boundary_condition_triangles.size() << "} }\n";
        
        unsigned int index = 0;
        for (typename std::deque< tuple_triangle >::iterator it = boundary_condition_triangles.begin(); it != boundary_condition_triangles.end(); ++it, ++index)
        {
            stream << "       {    " << index << " TRI3LIN " << vertex_handle_map[boost::get<0>(*it)] << " " <<
                                                                vertex_handle_map[boost::get<1>(*it)] << " " <<
                                                                vertex_handle_map[boost::get<2>(*it)] << " }\n";
        }
    }
}




template<typename domain_type, typename vertex_handle_map_type, typename stream_type>
void write_volume_contact( domain_type & domain, vertex_handle_map_type & vertex_handle_map, stream_type & stream, viennamesh::segment_id_type segment_id)
{
    typedef typename viennagrid::result_of::point_type< domain_type >::type point_type;
    typedef typename viennagrid::result_of::handle< domain_type, viennagrid::vertex_tag >::type vertex_handle_type;
    
    typedef typename viennagrid::result_of::element_range< domain_type, viennagrid::tetrahedron_tag >::type tetrahedron_range_type;
    typedef typename viennagrid::result_of::iterator< tetrahedron_range_type >::type tetrahedron_range_iterator;
    
    {
        typedef boost::tuple<vertex_handle_type, vertex_handle_type, vertex_handle_type, vertex_handle_type> tuple_tetrahedron;
        std::deque< tuple_tetrahedron > boundary_condition_tetrahedrons;
        
        tetrahedron_range_type tets = viennagrid::elements<viennagrid::tetrahedron_tag>( domain );
        for (tetrahedron_range_iterator it = tets.begin(); it != tets.end(); ++it)
        {
            if (viennamesh::segment(*it) == segment_id)
            {
                
                point_type const & p0 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[0] );
                point_type const & p1 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[1] );
                point_type const & p2 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[2] );
                point_type const & p3 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[3] );
                
                double det = viennamesh::utils::determinant(p1-p0, p2-p0, p3-p0);
                
            
                if (det > 0)
                {
                    boundary_condition_tetrahedrons.push_back( tuple_tetrahedron( viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(0),
                                                                            viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(1),
                                                                            viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(2),
                                                                            viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(3)) );
                }
                else
                {
                    boundary_condition_tetrahedrons.push_back( tuple_tetrahedron( viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(0),
                                                                            viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(2),
                                                                            viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(1),
                                                                            viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(3)) );
                }
            }
        }
        
        stream << "    objdesc { struct { label{int 1} type{string 1} nodeRef{ int 1}* } numobj{" << boundary_condition_tetrahedrons.size() << "} }\n";
                    
        unsigned int index = 0;
        for (typename std::deque< tuple_tetrahedron >::iterator it = boundary_condition_tetrahedrons.begin(); it != boundary_condition_tetrahedrons.end(); ++it, ++index)
        {
            stream << "       {    " << index << " TET4LIN " << vertex_handle_map[boost::get<0>(*it)] << " " <<
                                                                vertex_handle_map[boost::get<1>(*it)] << " " <<
                                                                vertex_handle_map[boost::get<2>(*it)] << " " <<
                                                                vertex_handle_map[boost::get<3>(*it)] << " }\n";
        }
    }
}












int main()
{
    typedef viennamesh::viennasap_layer_domain viennasap_layer_domain_type;
    
    typedef viennasap_layer_domain_type::point_type point_type;
    typedef viennasap_layer_domain_type::layer_type layer_type;
    typedef viennasap_layer_domain_type::interface_type interface_type;
    
    

    
    std::ifstream file( "../../examples/data/cross33.lay" );
    
    std::string file_content = stringtools::read_stream(file);
    boost::shared_ptr<viennagrid::io::viennasap_common::token_t> root = viennagrid::io::viennasap_common::split_into_tokens( file_content, 0, file_content.size() );

    
    std::map<std::string, std::string> element_layer_map;
    std::map<std::string, viennamesh::segment_id_type> layer_element_id_map;
    std::map<std::string, layer_type> layers;
    std::map<viennamesh::segment_id_type, std::string> layer_element_id_material_map;
    
    
    
    typedef viennagrid::config::point_type_3d point_type_3d;
    
    point_type_3d origin_3d;
    boost::shared_ptr<viennagrid::io::viennasap_common::token_t> origin_token = root->get_child("layerstructure/origin");
    {
        std::stringstream buffer( origin_token->content );
        buffer >> origin_3d[0] >> origin_3d[1] >> origin_3d[2];
    }
    
    point_type origin;
    origin[0] = origin_3d[0];
    origin[1] = origin_3d[1];
    
    
    
    
    double scale_factor = 1.0;
    {
        boost::shared_ptr<viennagrid::io::viennasap_common::token_t> token = root->get_child("lengthunit");
        if (token)
        {
            std::stringstream buffer( token->content );
            std::string unit;
            
            buffer >> scale_factor >> unit;
            
            if (unit == "m")
                scale_factor *= 1.0;
            else if (unit == "mm")
                scale_factor *= 1e-3;
            else if (unit == "um")
                scale_factor *= 1e-6;
        }
    }
    
    
    
    
    
    

    for (viennagrid::io::viennasap_common::token_t::children_container::iterator it = root->children.begin(); it != root->children.end(); ++it)
    {
        if (it->first != "mask")
            continue;
        
        std::string mask_name;

        {
            std::stringstream buffer( it->second->content );
            buffer >> mask_name;
        }
                
        std::cout << mask_name << std::endl;
        
        
        layer_type & layer = layers[mask_name];
        
        for (viennagrid::io::viennasap_common::token_t::children_container::iterator jt = it->second->children.begin(); jt != it->second->children.end(); ++jt)
        {
            if (jt->first == "rectangle")
            {
                std::string layer_element_name;
                std::string material_name;
                
                {
                    std::stringstream buffer( jt->second->content );
                    buffer >> layer_element_name >> material_name;
                }
                
                element_layer_map[layer_element_name] = mask_name;
                
                std::map<std::string, viennamesh::segment_id_type>::iterator lei_it = layer_element_id_map.find(layer_element_name);
                if (lei_it == layer_element_id_map.end())
                    lei_it = layer_element_id_map.insert( std::make_pair(layer_element_name, static_cast<viennamesh::segment_id_type>(layer_element_id_map.size())) ).first;
                
                viennamesh::segment_id_type layer_element_id = lei_it->second;
                
                layer_element_id_material_map[layer_element_id] = material_name;
                
                point_type p0;
                
                {
                    std::stringstream buffer( jt->second->unnamed_children[0]->content );
                    buffer >> p0[0] >> p0[1];
                }
                
                point_type p1;
                
                {
                    std::stringstream buffer( jt->second->unnamed_children[1]->content );
                    buffer >> p1[0] >> p1[1];
                }
                
                layer.add_rectangle( p0+origin, p0+p1+origin, layer_element_id );
            }
            else if (jt->first == "polygon")
            {
                std::string layer_element_name;
                std::string material_name;
                
                {
                    std::stringstream buffer( jt->second->content );
                    buffer >> layer_element_name >> material_name;
                }
                
                element_layer_map[layer_element_name] = mask_name;
                
                std::map<std::string, viennamesh::segment_id_type>::iterator lei_it = layer_element_id_map.find(layer_element_name);
                if (lei_it == layer_element_id_map.end())
                    lei_it = layer_element_id_map.insert( std::make_pair(layer_element_name, static_cast<viennamesh::segment_id_type>(layer_element_id_map.size())) ).first;
                
                viennamesh::segment_id_type layer_element_id = lei_it->second;
                
                layer_element_id_material_map[layer_element_id] = material_name;
                
                point_type local_origin;
                
                {
                    std::stringstream buffer( jt->second->unnamed_children[0]->content );
                    buffer >> local_origin[0] >> local_origin[1];
                }
                
                std::deque<point_type> polygon_points;
                
                for (unsigned int i = 1; i != jt->second->unnamed_children.size(); ++i)
                {
                    point_type tmp;
                    std::stringstream buffer( jt->second->unnamed_children[i]->content );
                    buffer >> tmp[0] >> tmp[1];
                    
                    polygon_points.push_back( origin + local_origin + tmp );
                }
                
                layer.add_polygon( polygon_points.begin(), polygon_points.end(), layer_element_id );
                
            }
            
        }
    }
    
    
    
    std::deque<std::string> interface_strings;
    std::deque< std::pair<std::string, double> > layer_structure;
    std::map<std::string, int> layer_position_map;
    

    boost::shared_ptr<viennagrid::io::viennasap_common::token_t> layerstructure = root->get_child("layerstructure");
    for (viennagrid::io::viennasap_common::token_t::children_container::iterator it = layerstructure->children.begin(); it != layerstructure->children.end(); ++it)
    {
        if ( it->first == "plane" )
        {
            interface_strings.push_back( it->second->content );
        }
        else if (it->first == "layer")
        {
            std::stringstream buffer( it->second->content );
            std::string mask_name;
            double layer_height;
            buffer >> mask_name >> layer_height;
            
            layer_position_map[mask_name] = layer_structure.size();
            layer_structure.push_back( std::make_pair(mask_name, layer_height) );
        }
    }
    
    
    viennasap_layer_domain_type viennasap_layer_domain;
    for (unsigned int i = 0; i < layers.size(); ++i)
        viennasap_layer_domain.layers.push_back( layers[layer_structure[i].first] );
    
    viennasap_layer_domain.calc_interface_layers();
    
    
    
    {
        double cur_z = 0.0;
        {
            interface_type tmp;
            tmp.planar_from_layer( viennasap_layer_domain.interface_layers.front(), cur_z );
            viennasap_layer_domain.interfaces.push_back( tmp );
        }
        for (unsigned int i = 0; i < layer_structure.size(); ++i)
        {
            cur_z += layer_structure[i].second;
            interface_type tmp;
            tmp.planar_from_layer( viennasap_layer_domain.interface_layers[i], cur_z );
            viennasap_layer_domain.interfaces.push_back( tmp );
        }
    }
    
    

    viennagrid::config::plc_3d_domain plc_domain_3d;
    {
        viennamesh::result_of::settings<viennamesh::viennasap_layer_mesher_tag>::type settings;
        viennamesh::run_algo< viennamesh::viennasap_layer_mesher_tag >( viennasap_layer_domain, plc_domain_3d, settings );
    }
    
    
    viennagrid::config::triangular_3d_domain triangle_domain_3d;
    {
        viennamesh::result_of::settings<viennamesh::cgal_plc_3d_mesher_tag>::type settings;
        viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_domain_3d, triangle_domain_3d, settings );
    }
    
    viennamesh::mark_face_segments( triangle_domain_3d );
    

    std::cout << "Hull - face segment check" << std::endl << std::endl;
    viennamesh::check_face_segments<viennagrid::triangle_tag>(triangle_domain_3d);
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
        vtk_writer(triangle_domain_3d, "hull.vtu");
    }
    
    
    
    std::map< viennamesh::segment_id_type, std::string > & material_map = layer_element_id_material_map;
    
    std::map< viennamesh::segment_id_type, viennamesh::segment_id_type > layer_element_id_segment_id_map;
    for (std::map< viennamesh::segment_id_type, std::string >::iterator it = material_map.begin(); it != material_map.end(); ++it)
        layer_element_id_segment_id_map[it->first] = it->first;
    
    
    viennagrid::config::triangular_3d_domain triangle_domain_3d_simplified;
    
    typedef viennagrid::result_of::element_range<viennagrid::config::triangular_3d_domain, viennagrid::triangle_tag>::type triangle_range_type;
    typedef viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;
    
    typedef viennagrid::result_of::handle<viennagrid::config::triangular_3d_domain, viennagrid::triangle_tag>::type triangle_handle_type;
    typedef viennagrid::result_of::element<viennagrid::config::triangular_3d_domain, viennagrid::triangle_tag>::type triangle_type;
    
    {
        triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( triangle_domain_3d );
        for ( triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it )
        {
            viennamesh::face_segment_definition_type const & segment_def = viennamesh::face_segments( *it );
            
            if (segment_def.size() == 2)
            {
                viennamesh::segment_id_type first = (segment_def.begin())->first;
                viennamesh::segment_id_type second = (++segment_def.begin())->first;
                
                if (material_map[first] == material_map[second])
                {
                    if (layer_element_id_segment_id_map[first] != layer_element_id_segment_id_map[second])
                    {
                        viennamesh::segment_id_type new_segment_id = layer_element_id_segment_id_map[first];
                        viennamesh::segment_id_type old_segment_id = layer_element_id_segment_id_map[second];
                        
                        for (std::map< viennamesh::segment_id_type, viennamesh::segment_id_type >::iterator jt = layer_element_id_segment_id_map.begin(); jt != layer_element_id_segment_id_map.end(); ++jt)
                            if (jt->second == old_segment_id) jt->second = new_segment_id;
                        
                        
                        std::cout << "Merging segment " << old_segment_id << " to " << new_segment_id << std::endl;
//                         segment_merge_map[first] = segment_merge_map[second];
                    }
                }
            }
        }
    }
    
    std::map<viennamesh::segment_id_type, viennamesh::segment_id_type> new_segment_merge_map;
    
    viennamesh::segment_id_type current_seg = 0;
    for (std::map< viennamesh::segment_id_type, viennamesh::segment_id_type >::iterator it = layer_element_id_segment_id_map.begin(); it != layer_element_id_segment_id_map.end(); ++it)
    {
        std::map<viennamesh::segment_id_type, viennamesh::segment_id_type>::iterator jt = new_segment_merge_map.find( it->second );
        if (jt == new_segment_merge_map.end())
            jt = new_segment_merge_map.insert( std::make_pair( it->second, current_seg++) ).first;
        
        it->second = jt->second;
        std::cout << "Mapping segment " << it->first << " to " << it->second << std::endl;
    }
//         std::cout << it->first << " " << it->second << std::endl;
    
    std::map<viennamesh::segment_id_type, std::string> segment_id_material_name_map;
    for (std::map< viennamesh::segment_id_type, viennamesh::segment_id_type >::iterator it = layer_element_id_segment_id_map.begin(); it != layer_element_id_segment_id_map.end(); ++it)
    {
        segment_id_material_name_map[ it->second ] = layer_element_id_material_map[it->first];
    }
    
    
    
    
    {
        triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( triangle_domain_3d );
        for ( triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it )
        {
            viennamesh::face_segment_definition_type const & segment_def = viennamesh::face_segments( *it );
            
            
            bool use = false;
            if (segment_def.size() == 1)
                use = true;
            else
            {
                viennamesh::segment_id_type first = (segment_def.begin())->first;
                viennamesh::segment_id_type second = (++segment_def.begin())->first;
                
                if (layer_element_id_segment_id_map[first] != layer_element_id_segment_id_map[second])
                    use = true;
            }
            
            if (use)
            {
                triangle_handle_type triangle_handle = viennagrid::create_triangle(triangle_domain_3d_simplified,
                    viennagrid::create_unique_vertex( triangle_domain_3d_simplified, viennagrid::point( triangle_domain_3d, viennagrid::elements<viennagrid::vertex_tag>(*it)[0] ) ),
                    viennagrid::create_unique_vertex( triangle_domain_3d_simplified, viennagrid::point( triangle_domain_3d, viennagrid::elements<viennagrid::vertex_tag>(*it)[1] ) ),
                    viennagrid::create_unique_vertex( triangle_domain_3d_simplified, viennagrid::point( triangle_domain_3d, viennagrid::elements<viennagrid::vertex_tag>(*it)[2] ) )
                );
                
                triangle_type & triangle = viennagrid::dereference_handle( triangle_domain_3d_simplified, triangle_handle );
                
                for (viennamesh::face_segment_definition_type::const_iterator jt = segment_def.begin(); jt != segment_def.end(); ++jt)
                {
//                     std::cout << segment_merge_map[jt->first] << std::endl;
                    viennamesh::add_face_to_segment( triangle_domain_3d_simplified, triangle, layer_element_id_segment_id_map[jt->first], jt->second );
                }
            }
        }
    }


    std::cout << "Hull simplified - face segment check" << std::endl << std::endl;
    viennamesh::check_face_segments<viennagrid::triangle_tag>(triangle_domain_3d_simplified);
    
    {        
        viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
        vtk_writer(triangle_domain_3d_simplified, "hull_simplified.vtu");
    }

    

    
    
        viennagrid::config::triangular_3d_domain hull_domain;
        viennamesh::result_of::settings<viennamesh::vgmodeler_hull_adaption_tag>::type vgm_settings;
        
        vgm_settings.cell_size = 1.0;
        
        viennamesh::run_algo< viennamesh::vgmodeler_hull_adaption_tag >( triangle_domain_3d_simplified, hull_domain, vgm_settings );
        
        
        {        
            viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
            vtk_writer(hull_domain, "layer_adapted_hull_3d.vtu");
        }

        
        
        viennagrid::config::tetrahedral_3d_domain tetrahedron_domain;
        viennamesh::result_of::settings<viennamesh::netgen_tetrahedron_tag>::type netgen_settings;
        
        netgen_settings.cell_size = 1.0;
        
        viennamesh::run_algo< viennamesh::netgen_tetrahedron_tag >( hull_domain, tetrahedron_domain, netgen_settings );
        
        
        {        
            viennagrid::io::vtk_writer<viennagrid::config::tetrahedral_3d_domain, viennagrid::config::tetrahedral_3d_cell> vtk_writer;
            viennagrid::io::add_scalar_data_on_cells<viennamesh::unique_segment_tag, viennamesh::segment_id_type>(vtk_writer, "segment_id");
            vtk_writer(tetrahedron_domain, "netgen_volume.vtu");
        }

        

    
        
        
        {
        
            viennagrid::config::tetrahedral_3d_domain & domain = tetrahedron_domain;
            
            std::ofstream file("test.fem");
            
            file << "header {\n";
            file << "    formatversion{ 3.0 } \n";
            file << "    title{   } \n";
            file << "    problemtype { generic }\n";
            file << "    dimension{ 3 } \n";
            file << "} \n";
            file << "node {\n";
            file << "    scalingfactor {" << scale_factor << "}\n";
            
            
            typedef viennagrid::result_of::point_type< viennagrid::config::tetrahedral_3d_domain >::type point_type;
            
            typedef viennagrid::result_of::element_range< viennagrid::config::tetrahedral_3d_domain, viennagrid::vertex_tag >::type vertex_range_type;
            typedef viennagrid::result_of::handle_iterator< vertex_range_type >::type vertex_handle_range_iterator;
    
            typedef viennagrid::result_of::element_range< viennagrid::config::tetrahedral_3d_domain, viennagrid::triangle_tag >::type triangle_range_type;
            typedef viennagrid::result_of::iterator< triangle_range_type >::type triangle_range_iterator;
            
            typedef viennagrid::result_of::element_range< viennagrid::config::tetrahedral_3d_domain, viennagrid::tetrahedron_tag >::type tetrahedron_range_type;
            typedef viennagrid::result_of::iterator< tetrahedron_range_type >::type tetrahedron_range_iterator;
    
            
            typedef viennagrid::result_of::handle< viennagrid::config::tetrahedral_3d_domain, viennagrid::vertex_tag >::type vertex_handle_type;
            
            std::map<vertex_handle_type, int> vertex_handle_map;
            
            
            int vertex_index = 0;
            vertex_range_type vertices = viennagrid::elements<viennagrid::vertex_tag>( domain );
            
            file << "    objdesc { struct { label{int 1} x{double m} y{double m} z{double m}} numobj {" << vertices.size() << "} }\n";
            
            for (vertex_handle_range_iterator it = vertices.handle_begin(); it != vertices.handle_end(); ++it, vertex_index++)
            {
                vertex_handle_map[*it] = vertex_index;
                point_type point = viennagrid::point( domain, *it );
                file << "    { " << vertex_index << " " << point << " }\n";
            }
        
            file << "} \n";
            file << "element {\n";
            
        
            tetrahedron_range_type tets = viennagrid::elements<viennagrid::tetrahedron_tag>( domain );
            
            file << "    objdesc { struct { label{int 1} type{string 1} regionRef{int 1} conn{int 1}* } numobj{" << tets.size() << "} }\n";
            
            int tet_index = 0;
            for (tetrahedron_range_iterator it = tets.begin(); it != tets.end(); ++it, tet_index++)
            {
                viennamesh::segment_id_type segment_id = viennamesh::segment(*it);
                
                    point_type const & p0 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[0] );
                    point_type const & p1 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[1] );
                    point_type const & p2 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[2] );
                    point_type const & p3 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(*it)[3] );
                    
                    double det = viennamesh::utils::determinant(p1-p0, p2-p0, p3-p0);
                    
    //                 std::cout << det << " " << det2 << std::endl;
    
                
                    if (det > 0)
                    {
                        file << "    { " << tet_index << " TET4LIN " << segment_id << " " <<
                            vertex_handle_map[viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(0)] << " " <<
                            vertex_handle_map[viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(1)] << " " <<
                            vertex_handle_map[viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(2)] << " " <<
                            vertex_handle_map[viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(3)] << " }\n";
                    }
                    else
                    {
                        file << "    { " << tet_index << " TET4LIN " << segment_id << " " <<
                            vertex_handle_map[viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(0)] << " " <<
                            vertex_handle_map[viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(2)] << " " <<
                            vertex_handle_map[viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(1)] << " " <<
                            vertex_handle_map[viennagrid::elements<viennagrid::vertex_tag>(*it).handle_at(3)] << " }\n";
                    }
            }
            
            file << "} \n";
            
            
            
            unsigned int num_contacts = 0;
            for (viennagrid::io::viennasap_common::token_t::children_container::iterator it = root->children.begin(); it != root->children.end(); ++it)
            {
                if (it->first == "contact")
                    ++num_contacts;
            }
            
            
            file << "boundarycondition {\n";
            file << "    objdesc { { identifier } numobj{" << num_contacts << "} }\n";
    
            

            for (viennagrid::io::viennasap_common::token_t::children_container::iterator it = root->children.begin(); it != root->children.end(); ++it)
            {
                if (it->first != "contact")
                    continue;
                
                
                std::stringstream buffer( it->second->content );
                
                std::string contact_name;
                std::string contact_type;
                
                buffer >> contact_name >> contact_type;
                
                if ( contact_type == "cap" )
                    file << "    dirichlet{ \n";
                
                if ( contact_type == "therm" )
                    file << "    dirichlettherm{ \n";
                
                
                file << "    name { " << contact_name << " } \n";
                
                if ( contact_type == "therm" )
                {
                    double value;
                    std::string unit;
                    
                    buffer >> value >> unit;
                    
                    file << "    value { " << value << " " << unit << " } \n";
                }
                    
                    
                std::string contact_geometry;
                buffer >> contact_geometry;
                
                
                if ( contact_geometry == "flatbottom" )
                {
                    std::string layer_element_name = it->second->unnamed_children[0]->content;
                    int layer_pos = layer_position_map[layer_element_name];
                
                    write_flat_contact( domain, vertex_handle_map, file, viennasap_layer_domain.interfaces[layer_pos] );
                }
                if ( contact_geometry == "flattop" )
                {
                    std::string layer_element_name = it->second->unnamed_children[0]->content;
                    int layer_pos = layer_position_map[layer_element_name];
                
                    write_flat_contact( domain, vertex_handle_map, file, viennasap_layer_domain.interfaces[layer_pos+1] );
                }
                if ( contact_geometry == "vol" )
                {
                    std::string layer_element_name = it->second->unnamed_children[0]->content;
                    viennamesh::segment_id_type segment_id = layer_element_id_segment_id_map[ layer_element_id_map[layer_element_name] ];
                
                    write_volume_contact( domain, vertex_handle_map, file, segment_id );
                }

                
                file << "    }\n";
            }
            
            
            file << "} \n";
            
            file << "material {\n";
            file << "    objdesc{struct{ label{int 1} name{string 1}\n";
            file << "       RelativePermittivity {double 1} Permittivity_TemperatureCoefficient {double 1 1/K} Permittivity_TemperatureCoefficient1 {double 1 1/K}\n";
            file << "        Conductivity {double 1 1/(Ohm*m)} Conductivity_TemperatureCoefficient {double 1 1/K} Conductivity_TemperatureCoefficient1 {double 1 1/K}\n";
            file << "        ThermalConductivity {double 1 W/(K*m)} ThermalConductivity_TemperatureCoefficient {double 1 1/K} ThermalConductivity_TemperatureCoefficient1 {double 1 1/K}\n";
            file << "        SpecificHeat {double 1 K/(W*kg)} SpecificHeat_TemperatureCoefficient_B {double 1 1/(W*kg)} SpecificHeat_TemperatureCoefficient_C {double 1 1/(K*W*kg)} SpecificHeat_TemperatureCoefficient_D {double 1 1/(K^2*W*kg)} SpecificHeat_TemperatureCoefficient_E {double 1 K^3/(W*kg)}\n";
            file << "        Density {double 1 kg/m^3}\n";
            
            
            
            std::map<std::string, boost::shared_ptr<viennagrid::io::viennasap_common::token_t> > material_information;
            
            for (viennagrid::io::viennasap_common::token_t::children_container::iterator it = root->children.begin(); it != root->children.end(); ++it)
            {
                if (it->first != "material")
                    continue;
                
                std::stringstream buffer( it->second->content );
                
                std::string material_id;
                buffer >> material_id;
                
                material_information[material_id] = it->second;
            }
            
            
            
            
            
            viennamesh::segment_id_container_type const & segments = viennamesh::segments(domain);
            
            file << "        } numobj{" << segments.size() << "}}\n";
            
            for ( viennamesh::segment_id_container_type::iterator it = segments.begin(); it != segments.end(); ++it)
            {
                viennamesh::segment_id_type segment_id = *it;
                
                boost::shared_ptr<viennagrid::io::viennasap_common::token_t> material = material_information[ segment_id_material_name_map[ *it ] ];
                
                std::stringstream buffer( material->content );
                
                std::string material_id;
                std::string material_name;
                
                buffer >> material_id >> material_name;
                
                double RelativePermittivity = 1.0;
                double Permittivity_TemperatureCoefficient = 0.0;
                double Permittivity_TemperatureCoefficient1 = 0.0;
                double Conductivity = 0.0;
                double Conductivity_TemperatureCoefficient = 0.0;
                double Conductivity_TemperatureCoefficient1 = 0.0;
                double ThermalConductivity = 0.0;
                double ThermalConductivity_TemperatureCoefficient = 0.0;
                double ThermalConductivity_TemperatureCoefficient1 = 0.0;
                double SpecificHeat = 0.0;
                double SpecificHeat_TemperatureCoefficient_B = 0.0;
                double SpecificHeat_TemperatureCoefficient_C = 0.0;
                double SpecificHeat_TemperatureCoefficient_D = 0.0;
                double SpecificHeat_TemperatureCoefficient_E = 0.0;
                double Density = 0.0;
                
                
                {
                    boost::shared_ptr<viennagrid::io::viennasap_common::token_t> conductivity = material->get_child("conductivity");
                    if (conductivity)
                    {
                        std::stringstream buffer( conductivity->content );
                        
                        std::string unit;
                        buffer >> Conductivity >> unit;
                        
                        if ( unit == "uOhmcm" )
                            Conductivity = 1.0 / Conductivity * 1e+8;
                        else
                            std::cout << "Error: Conductivity has not known unit: " << unit << std::endl;
                    }
                }
                
                {
                    boost::shared_ptr<viennagrid::io::viennasap_common::token_t> thermal_conductivity = material->get_child("thermal_conductivity");
                    if (thermal_conductivity)
                    {
                        std::stringstream buffer( thermal_conductivity->content );
                        
                        std::string unit;
                        buffer >> ThermalConductivity >> unit;
                        
                        if ( unit == "W/(K*m)" )
                            ThermalConductivity = ThermalConductivity;
                        else
                            std::cout << "Error: ThermalConductivity has not known unit: " << unit << std::endl;
                    }
                }
                
                
                {
                    boost::shared_ptr<viennagrid::io::viennasap_common::token_t> permittivity = material->get_child("permittivity");
                    if (permittivity)
                    {
                        std::stringstream buffer( permittivity->content );
                        
                        std::string unit;
                        buffer >> RelativePermittivity >> Permittivity_TemperatureCoefficient;
                    }
                }

                
                file << "     {" << segment_id << "   " << material_name << " " << RelativePermittivity << " " << Permittivity_TemperatureCoefficient << " 0 "  <<
                    Conductivity << " 0 0 " << ThermalConductivity << " 0 0 0 0 0 0 0 0 }\n";
            }
            
            file << "} \n";
        }
    
    
    
    
    
    
    
    
    
    
    
    return 0;
}
