#include <iostream>
#include <boost/iterator/iterator_concepts.hpp>

#include "viennagrid/domain/config.hpp"
#include "viennagrid/algorithm/intersect.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "viennamesh/algorithm/viennasap_layer_mesher.hpp"
#include "viennamesh/algorithm/vgmodeler_hull_adaption.hpp"
#include "viennamesh/algorithm/netgen_tetrahedron_mesher.hpp"






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
                
//                 std::cout << det << " " << det2 << std::endl;

            
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
    
    
    viennasap_layer_domain_type viennasap_layer_domain;
    std::vector<double> layer_heights;
    
    unsigned int object_index = 0;

    
    {
        layer_type tmp;
        tmp.add_rectangle( point_type(0,0), point_type(6.5, 14.0), 1 );
        viennasap_layer_domain.layers.push_back(tmp);
        layer_heights.push_back( 0.5 );
    }
    


    {
        layer_type tmp;
        tmp.add_rectangle( point_type(0,0), point_type(6.5, 14.0), 2 );
        tmp.add_rectangle( point_type(2,0), point_type(2.5, 14.0), 3 );
        tmp.add_rectangle( point_type(3,0), point_type(3.5, 14.0), 4 );
        tmp.add_rectangle( point_type(4,0), point_type(4.5, 14.0), 5 );
        viennasap_layer_domain.layers.push_back(tmp);
        layer_heights.push_back( 0.5 );
    }
    
    {
        layer_type tmp;
        tmp.add_rectangle( point_type(0,0), point_type(6.5, 14.0), 6 );
        viennasap_layer_domain.layers.push_back(tmp);
        layer_heights.push_back( 0.5 );
    }

    
    {
        layer_type tmp;
        tmp.add_rectangle( point_type(0,0), point_type(6.5, 14.0), 7 );
        
        {
            std::vector<point_type> poly;
            poly.push_back( point_type(0, 2.0) );
            poly.push_back( point_type(6.5, 8.5) );
            poly.push_back( point_type(6.5, 9.2) );
            poly.push_back( point_type(0, 2.7) );
            tmp.add_polygon( poly.begin(), poly.end(), 8 );
        }
        
        {
            std::vector<point_type> poly;
            poly.push_back( point_type(0, 3.4) );
            poly.push_back( point_type(6.5, 9.9) );
            poly.push_back( point_type(6.5, 10.6) );
            poly.push_back( point_type(0, 4.1) );
            tmp.add_polygon( poly.begin(), poly.end(), 9 );
        }
        
        {
            std::vector<point_type> poly;
            poly.push_back( point_type(0, 4.8) );
            poly.push_back( point_type(6.5, 11.3) );
            poly.push_back( point_type(6.5, 12.0) );
            poly.push_back( point_type(0, 5.5) );
            tmp.add_polygon( poly.begin(), poly.end(), 10 );
        }
        

        viennasap_layer_domain.layers.push_back(tmp);
        layer_heights.push_back( 0.5 );
    }

    
    {
        layer_type tmp;
        tmp.add_rectangle( point_type(0,0), point_type(6.5, 14.0), 11 );
        viennasap_layer_domain.layers.push_back(tmp);
        layer_heights.push_back( 3.0 );
    }
    
    
    viennasap_layer_domain.calc_interface_layers();

    {
        double cur_z = 0.0;
        {
            interface_type tmp;
            tmp.planar_from_layer( viennasap_layer_domain.interface_layers.front(), cur_z );
            viennasap_layer_domain.interfaces.push_back( tmp );
        }
        for (unsigned int i = 0; i < layer_heights.size(); ++i)
        {
            cur_z += layer_heights[i];
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
    
    
    
    std::map< viennamesh::segment_id_type, std::string > material_map;
    material_map[1] = "SIO2";
    material_map[2] = "SIO2";
    material_map[3] = "AL";
    material_map[4] = "AL";
    material_map[5] = "AL";
    material_map[6] = "SIO2";
    material_map[7] = "SIO2";
    material_map[8] = "AL";
    material_map[9] = "AL";
    material_map[10] = "AL";
    material_map[11] = "SIO2";
    
    
    std::map< viennamesh::segment_id_type, viennamesh::segment_id_type > segment_merge_map;
    for (viennamesh::segment_id_type id = 1; id != 12; ++id)
        segment_merge_map[id] = id;
    
    
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
                    if (segment_merge_map[first] != segment_merge_map[second])
                    {
                        viennamesh::segment_id_type new_segment_id = segment_merge_map[first];
                        viennamesh::segment_id_type old_segment_id = segment_merge_map[second];
                        
                        for (std::map< viennamesh::segment_id_type, viennamesh::segment_id_type >::iterator jt = segment_merge_map.begin(); jt != segment_merge_map.end(); ++jt)
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
    for (std::map< viennamesh::segment_id_type, viennamesh::segment_id_type >::iterator it = segment_merge_map.begin(); it != segment_merge_map.end(); ++it)
    {
        std::map<viennamesh::segment_id_type, viennamesh::segment_id_type>::iterator jt = new_segment_merge_map.find( it->second );
        if (jt == new_segment_merge_map.end())
            jt = new_segment_merge_map.insert( std::make_pair( it->second, current_seg++) ).first;
        
        it->second = jt->second;
        std::cout << "Mapping segment " << it->first << " to " << it->second << std::endl;
    }
//         std::cout << it->first << " " << it->second << std::endl;
    
    
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
                
                if (segment_merge_map[first] != segment_merge_map[second])
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
                    viennamesh::add_face_to_segment( triangle_domain_3d_simplified, triangle, segment_merge_map[jt->first], jt->second );
                }
            }
        }
    }
    
//     {
//         viennamesh::segment_id_container_type const & segments = viennamesh::segments(triangle_domain_3d_simplified);
//         for (viennamesh::segment_id_container_type::iterator it = segments.begin(); it != segments.end(); ++it)
//             std::cout << *it << std::endl;
//     }


    std::cout << "Hull simplified - face segment check" << std::endl << std::endl;
    viennamesh::check_face_segments<viennagrid::triangle_tag>(triangle_domain_3d_simplified);
    
    {        
        viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
        vtk_writer(triangle_domain_3d_simplified, "hull_simplified.vtu");
    }

    

    
    
        viennagrid::config::triangular_3d_domain hull_domain;
        viennamesh::result_of::settings<viennamesh::vgmodeler_hull_adaption_tag>::type vgm_settings;
        
        vgm_settings.cell_size = 0.4;
        
        viennamesh::run_algo< viennamesh::vgmodeler_hull_adaption_tag >( triangle_domain_3d_simplified, hull_domain, vgm_settings );
        
        
        {        
            viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
            vtk_writer(hull_domain, "layer_adapted_hull_3d.vtu");
        }
        
//     {
//         viennamesh::segment_id_container_type const & segments = viennamesh::segments(hull_domain);
//         for (viennamesh::segment_id_container_type::iterator it = segments.begin(); it != segments.end(); ++it)
//             std::cout << *it << std::endl;
//     }
        
        
        viennagrid::config::tetrahedral_3d_domain tetrahedron_domain;
        viennamesh::result_of::settings<viennamesh::netgen_tetrahedron_tag>::type netgen_settings;
        
        netgen_settings.cell_size = 0.4;
        
        viennamesh::run_algo< viennamesh::netgen_tetrahedron_tag >( hull_domain, tetrahedron_domain, netgen_settings );
        
        
        {        
            viennagrid::io::vtk_writer<viennagrid::config::tetrahedral_3d_domain, viennagrid::config::tetrahedral_3d_cell> vtk_writer;
            viennagrid::io::add_scalar_data_on_cells<viennamesh::unique_segment_tag, viennamesh::segment_id_type>(vtk_writer, "segment_id");
            vtk_writer(tetrahedron_domain, "netgen_volume.vtu");
        }
        
//     {
//         viennamesh::segment_id_container_type const & segments = viennamesh::segments(tetrahedron_domain);
//         for (viennamesh::segment_id_container_type::iterator it = segments.begin(); it != segments.end(); ++it)
//             std::cout << *it << std::endl;
//     }
        
        
//     typedef viennagrid::result_of::element_range< viennagrid::config::tetrahedral_3d_domain, viennagrid::tetrahedron_tag >::type tetrahedron_range_type;
//     typedef viennagrid::result_of::iterator< tetrahedron_range_type >::type tetrahedron_range_iterator;
//     
//     tetrahedron_range_type tets = viennagrid::elements<viennagrid::tetrahedron_tag>( tetrahedron_domain );
//     for (tetrahedron_range_iterator it = tets.begin(); it != tets.end(); ++it)
//         std::cout << viennamesh::segment(*it) << " ";
    
        
    
        
        
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
            file << "    scalingfactor {1.000000e-06}\n";
            
            
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
            
            file << "boundarycondition {\n";
            file << "    objdesc { { identifier } numobj{9} }\n";
    
            
            file << "    dirichlet{ \n";
            file << "    name { C_GND } \n";
            write_flat_contact( domain, vertex_handle_map, file, viennasap_layer_domain.interfaces.front() );
            file << "    }\n";
            
            
            file << "    dirichlet{ \n";
            file << "    name { CONT1 } \n";
            write_volume_contact( domain, vertex_handle_map, file, 1);
            file << "    }\n";
    
            file << "    dirichlet{ \n";
            file << "    name { CONT2 } \n";
            write_volume_contact( domain, vertex_handle_map, file, 2);
            file << "    }\n";
            
            file << "    dirichlet{ \n";
            file << "    name { CONT3 } \n";
            write_volume_contact( domain, vertex_handle_map, file, 3);
            file << "    }\n";
            
            
            file << "    dirichlet{ \n";
            file << "    name { CONT4 } \n";
            write_volume_contact( domain, vertex_handle_map, file, 4);
            file << "    }\n";
            
            file << "    dirichlet{ \n";
            file << "    name { CONT5 } \n";
            write_volume_contact( domain, vertex_handle_map, file, 5);
            file << "    }\n";
    
            file << "    dirichlet{ \n";
            file << "    name { CONT6 } \n";
            write_volume_contact( domain, vertex_handle_map, file, 6);
            file << "    }\n";
            
            
            file << "    dirichlettherm{ \n";
            file << "    name { C_THA } \n";
            file << "    value { 3.000000e+02 K } \n";
            write_flat_contact( domain, vertex_handle_map, file, viennasap_layer_domain.interfaces.front() );
            file << "    }\n";
    
            file << "    dirichlettherm{ \n";
            file << "    name { C_THB } \n";
            file << "    value { 3.500000e+02 K } \n";
            write_flat_contact( domain, vertex_handle_map, file, viennasap_layer_domain.interfaces.back() );
            file << "    }\n";
    
            file << "} \n";
            
            file << "material {\n";
            file << "    objdesc{struct{ label{int 1} name{string 1}\n";
            file << "       RelativePermittivity {double 1} Permittivity_TemperatureCoefficient {double 1 1/K} Permittivity_TemperatureCoefficient1 {double 1 1/K}\n";
            file << "        Conductivity {double 1 1/(Ohm*m)} Conductivity_TemperatureCoefficient {double 1 1/K} Conductivity_TemperatureCoefficient1 {double 1 1/K}\n";
            file << "        ThermalConductivity {double 1 W/(K*m)} ThermalConductivity_TemperatureCoefficient {double 1 1/K} ThermalConductivity_TemperatureCoefficient1 {double 1 1/K}\n";
            file << "        SpecificHeat {double 1 K/(W*kg)} SpecificHeat_TemperatureCoefficient_B {double 1 1/(W*kg)} SpecificHeat_TemperatureCoefficient_C {double 1 1/(K*W*kg)} SpecificHeat_TemperatureCoefficient_D {double 1 1/(K^2*W*kg)} SpecificHeat_TemperatureCoefficient_E {double 1 K^3/(W*kg)}\n";
            file << "        Density {double 1 kg/m^3}\n";
            file << "        } numobj{7}}\n";
            file << "     {0   SiO2 3.9 0.1 0 0 0 0 1.4 0 0 0 0 0 0 0 0 }\n";
            file << "     {1     Al 1 0 0 3.33333e+07 0 0 380 0 0 0 0 0 0 0 0 }\n";
            file << "     {2     Al 1 0 0 3.33333e+07 0 0 380 0 0 0 0 0 0 0 0 }\n";
            file << "     {3     Al 1 0 0 3.33333e+07 0 0 380 0 0 0 0 0 0 0 0 }\n";
            file << "     {4     Al 1 0 0 3.33333e+07 0 0 380 0 0 0 0 0 0 0 0 }\n";
            file << "     {5     Al 1 0 0 3.33333e+07 0 0 380 0 0 0 0 0 0 0 0 }\n";
            file << "     {6     Al 1 0 0 3.33333e+07 0 0 380 0 0 0 0 0 0 0 0 }\n";
            file << "} \n";
        }
    
    
    
    
    
    
    
    
    
    
    
    return 0;
}
