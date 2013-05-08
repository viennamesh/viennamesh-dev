#include <iostream>


#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennagrid/io/poly_reader.hpp"
#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/neighbour_iteration.hpp"

#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/centroid.hpp"

#include "viennamesh/algorithm/cgal_plc_mesher.hpp"
#include "viennamesh/algorithm/vgmodeler_hull_adaption.hpp"
#include "viennamesh/algorithm/netgen_tetrahedron_mesher.hpp"


template<typename hull_domain_type, typename element_handle_type, typename point_type, typename hull_view_type, typename visited_container_type>
void recursively_add_neighbours( hull_domain_type & hull_domain, element_handle_type const & element_handle, point_type const & normal_vector, visited_container_type & visited_container, hull_view_type & view )
{
    typedef typename viennagrid::storage::handle::value_type<element_handle_type>::type element_type;
//     typedef typename viennagrid::result_of::cell_type<hull_domain_type>::type element_type;
    element_type & element = viennagrid::dereference_handle( hull_domain, element_handle );
    
    if (viennagrid::look_up(visited_container, element))
        return;
    
//     typedef typename viennagrid::result_of::point_type<hull_domain_type>::type point_type;
    
    point_type current_normal_vector = viennagrid::normal_vector( hull_domain, element );
    current_normal_vector /= viennagrid::norm_2(current_normal_vector);
    
    if ( std::abs(viennagrid::inner_prod(normal_vector, current_normal_vector)) >= 1.0-1e-6)
    {
        viennagrid::look_up(visited_container, element) = true;
        viennagrid::add_handle( view, hull_domain, element_handle );
        viennagrid::add_handle( view, hull_domain, viennagrid::lines(element).handle_at(0) );
        viennagrid::add_handle( view, hull_domain, viennagrid::lines(element).handle_at(1) );
        viennagrid::add_handle( view, hull_domain, viennagrid::lines(element).handle_at(2) );
        
        typedef typename viennagrid::result_of::neighbour_range<hull_domain_type, element_type>::type neighbour_range_type;
        typedef typename viennagrid::result_of::iterator<neighbour_range_type>::type neighbour_range_iterator;
        
        neighbour_range_type neighgbours = viennagrid::neighbour_elements< viennagrid::line_tag >( hull_domain, element_handle );
        for (neighbour_range_iterator it = neighgbours.begin(); it != neighgbours.end(); ++it)
            recursively_add_neighbours( hull_domain, it.handle(), normal_vector, visited_container, view );
    }
}




template<typename line_domain_type, typename vertex_handle_type, typename line_handle_type, typename point_type, typename visited_container_type>
vertex_handle_type get_endpoint( line_domain_type & line_domain, vertex_handle_type const & vertex_handle, line_handle_type const & line_handle, point_type direction, visited_container_type & visited_container )
{
    typedef typename viennagrid::result_of::coboundary_range<line_domain_type, viennagrid::line_tag>::type coboundary_range_type;
    typedef typename viennagrid::result_of::iterator<coboundary_range_type>::type coboundary_range_iterator;
    typedef typename viennagrid::storage::handle::value_type<line_handle_type>::type line_type;
    
    coboundary_range_type lines = viennagrid::coboundary_elements<viennagrid::line_tag>(line_domain, vertex_handle);
    if (lines.size() != 2)
    {
//         std::cout << "Vertex does not have 2 lines -> finishing" << std::endl;
        return vertex_handle;
    }
    
//     if (viennagrid::look_up( visited_container, viennagrid::dereference_handle(line_domain, line_handle) ))
//         std::cout << "Something went wrong..." << std::endl;
    
    line_type & line = viennagrid::dereference_handle( line_domain, line_handle );
    
    line_handle_type other_line_handle;
    if (lines.handle_at(0) == line_handle)
        other_line_handle = lines.handle_at(1);
    else
        other_line_handle = lines.handle_at(0);
    
    
    line_type & other_line = viennagrid::dereference_handle( line_domain, other_line_handle );
    
    point_type other_direction = viennagrid::point( line_domain, viennagrid::vertices(other_line)[0] ) - viennagrid::point( line_domain, viennagrid::vertices(other_line)[1] );
    other_direction /= viennagrid::norm_2(other_direction);
    
    if ( std::abs(viennagrid::inner_prod(direction, other_direction)) >= 1.0-1e-6)
    {
//         std::cout << "      merging lines" << std::endl;
//         std::cout << "         "  << viennagrid::point( line_domain, viennagrid::vertices(line)[0] ) << " - " << viennagrid::point( line_domain, viennagrid::vertices(line)[1] ) << std::endl;
//         std::cout << "         "  << viennagrid::point( line_domain, viennagrid::vertices(other_line)[0] ) << " - " << viennagrid::point( line_domain, viennagrid::vertices(other_line)[1] ) << std::endl;
        
        viennagrid::look_up( visited_container, viennagrid::dereference_handle(line_domain, other_line_handle) ) = true;
        
        vertex_handle_type other_vertex_handle;
        if ( viennagrid::vertices(other_line).handle_at(0) == vertex_handle )
            other_vertex_handle = viennagrid::vertices(other_line).handle_at(1);
        else
            other_vertex_handle = viennagrid::vertices(other_line).handle_at(0);
        
        return get_endpoint( line_domain, other_vertex_handle, other_line_handle, direction, visited_container );
    }
    
    return vertex_handle;
}




template<typename hull_domain_type, typename plc_domain_type>
void extract_plcs( hull_domain_type & hull_domain, plc_domain_type & plc_domain )
{
    typedef typename viennagrid::result_of::point_type<hull_domain_type>::type hull_point_type;
    typedef typename viennagrid::result_of::cell_type<hull_domain_type>::type hull_cell_type;
    typedef typename viennagrid::result_of::handle<hull_domain_type, hull_cell_type>::type hull_cell_handle_type;
    typedef typename viennagrid::result_of::cell_range<hull_domain_type>::type cell_range_type;
    typedef typename viennagrid::result_of::iterator<cell_range_type>::type cell_range_iterator;
    
    cell_range_type cells = viennagrid::elements( hull_domain );
    
    std::deque<bool> visited( cells.size(), false );
    
    typedef typename viennagrid::result_of::domain_view<hull_domain_type>::type hull_view_type;
    typedef typename viennagrid::result_of::vertex_handle<hull_view_type>::type hull_view_vertex_handle_type;
    typedef typename viennagrid::result_of::element_range<hull_view_type, viennagrid::line_tag>::type view_line_range_type;
    typedef typename viennagrid::result_of::iterator<view_line_range_type>::type view_line_range_iterator;

    std::deque<hull_view_type> planes;
    
    hull_view_type all_lines = viennagrid::create_view<hull_view_type>( hull_domain );
    
    for (cell_range_iterator it = cells.begin(); it != cells.end(); ++it)
    {
        hull_cell_type const & cell = *it;
        
        if (viennagrid::look_up(visited, cell))
            continue;
        
        hull_point_type normal_vector = viennagrid::normal_vector( hull_domain, cell );
        normal_vector /= viennagrid::norm_2(normal_vector);
        
        
        planes.resize( planes.size()+1 );
        hull_view_type & current_plane = planes.back();
        current_plane = viennagrid::create_view<hull_view_type>( hull_domain );        
        
        recursively_add_neighbours( hull_domain, it.handle(), normal_vector, visited, current_plane );
        
        typedef typename viennagrid::result_of::cell_range<hull_view_type>::type view_cell_range_type;
        typedef typename viennagrid::result_of::iterator<view_cell_range_type>::type view_cell_range_iterator;
        
//         std::cout << "Plane with normal_vector=" << normal_vector << std::endl;
//         
//         view_cell_range_type plane_elements = viennagrid::elements( current_plane );
//         for (view_cell_range_iterator it = plane_elements.begin(); it != plane_elements.end(); ++it)
//         {
//             std::cout << "  Triangle" << std::endl;
//             std::cout << "    " << viennagrid::point( current_plane, viennagrid::vertices(*it)[0] ) << std::endl;
//             std::cout << "    " << viennagrid::point( current_plane, viennagrid::vertices(*it)[1] ) << std::endl;
//             std::cout << "    " << viennagrid::point( current_plane, viennagrid::vertices(*it)[2] ) << std::endl;
//         }
        
        
        view_line_range_type lines = viennagrid::elements( current_plane );
//         viennadata::access<viennagrid::boundary_key<hull_view_type>, bool>(viennagrid::boundary_key<hull_view_type>(current_plane))(current_plane) = false;
//         viennadata::erase<viennadata::all, viennadata::all>()(current_plane);
//         for (view_line_range_iterator it = lines.begin(); it != lines.end(); ++it)
//             viennadata::erase<viennadata::all, viennadata::all>()(*it);
        
        for (view_line_range_iterator it = lines.begin(); it != lines.end(); ++it)
        {
//             viennagrid::detect_boundary<viennagrid::triangle_tag>( current_plane,  );
            
            if (viennagrid::is_boundary<viennagrid::triangle_tag>( *it, current_plane ))
            {
//                 std::cout <<  "  Line" << std::endl;
//                 std::cout << "    " << viennagrid::point( current_plane, viennagrid::vertices(*it)[0] ) << std::endl;
//                 std::cout << "    " << viennagrid::point( current_plane, viennagrid::vertices(*it)[1] ) << std::endl;
                
                viennagrid::add_handle( all_lines, hull_domain, it.handle() );
            }
        }
        
        
//         std::cout << std::endl << std::endl;
    }
    
    
    viennagrid::config::line_3d_domain line_domain;
    
    typedef typename viennagrid::result_of::line_handle<plc_domain_type>::type plc_line_handle_type;
    typedef typename viennagrid::result_of::vertex_handle<plc_domain_type>::type plc_vertex_handle_type;
    typedef typename viennagrid::result_of::point_type<plc_domain_type>::type plc_point_type;
    
    
    for (std::size_t i = 0; i != planes.size(); ++i)
    {
        hull_view_type & current_plane = planes[i];
        
        view_line_range_type current_plane_lines = viennagrid::elements(current_plane);
        std::deque<bool> lines_visited( current_plane_lines.size(), false );
        
//         viennagrid::config::line_3d_domain tmp_domain;
        
//         std::cout << "Plane with " << lines_visited.size() << " lines " << std::endl;
        std::deque<plc_line_handle_type> plc_lines;
        
        for (view_line_range_iterator it = current_plane_lines.begin(); it != current_plane_lines.end(); ++it)
        {
            if (!viennagrid::is_boundary<viennagrid::triangle_tag>( *it, current_plane ))
                continue;
            
            if (viennagrid::look_up(lines_visited, *it) == true)
                continue;
            
            
            
            hull_point_type direction = viennagrid::point( all_lines, viennagrid::vertices(*it)[0] ) - viennagrid::point( all_lines, viennagrid::vertices(*it)[1] );
            direction /= viennagrid::norm_2(direction);
            
            hull_view_vertex_handle_type first = get_endpoint( all_lines, viennagrid::vertices(*it).handle_at(0), it.handle(), direction, lines_visited );
            hull_view_vertex_handle_type second = get_endpoint( all_lines, viennagrid::vertices(*it).handle_at(1), it.handle(), direction, lines_visited );
            
            viennagrid::look_up( lines_visited, *it ) = true;
            
//             std::cout << "  Line" << std::endl;
//             std::cout << "    " << viennagrid::point( all_lines, viennagrid::vertices(*it)[0] ) << std::endl;
//             std::cout << "    " << viennagrid::point( all_lines, viennagrid::vertices(*it)[1] ) << std::endl;
//             
//             std::cout << "    " << viennagrid::point( all_lines, first ) << std::endl;
//             std::cout << "    " << viennagrid::point( all_lines, second ) << std::endl;
            
            viennagrid::create_line( line_domain,
                                    viennagrid::create_unique_vertex(line_domain, viennagrid::point( all_lines, first )),
                                    viennagrid::create_unique_vertex(line_domain, viennagrid::point( all_lines, second ))    
                                    );
            
//             viennagrid::create_line( tmp_domain,
//                                     viennagrid::create_unique_vertex(tmp_domain, viennagrid::point( all_lines, first )),
//                                     viennagrid::create_unique_vertex(tmp_domain, viennagrid::point( all_lines, second ))    
//                                     );
            
            plc_lines.push_back(viennagrid::create_line(plc_domain,
                                        viennagrid::create_unique_vertex(plc_domain, viennagrid::point( all_lines, first )),
                                        viennagrid::create_unique_vertex(plc_domain, viennagrid::point( all_lines, second ))));
        }
        
        plc_vertex_handle_type tmp_vtx_handle;
        plc_point_type tmp_pt;
        
        
        viennagrid::create_plc(plc_domain, plc_lines.begin(), plc_lines.end(), &tmp_vtx_handle, &tmp_vtx_handle, &tmp_pt, &tmp_pt);
        
//         {
//             char buffer[100];
//             sprintf( buffer, "planes_%d.vtu", i );
//             viennagrid::io::vtk_writer<viennagrid::config::line_3d_domain, viennagrid::config::line_3d_cell> vtk_writer;
//             vtk_writer(tmp_domain, buffer);
//         }
        
//         std::cout << std::endl << std::endl;
    }
    
    
    {
        viennagrid::io::vtk_writer<viennagrid::config::line_3d_domain, viennagrid::config::line_3d_cell> vtk_writer;
        vtk_writer(line_domain, "extracted_lines.vtu");
    }
    
    
    
}






int main()
{
//     viennagrid::config::plc_3d_domain plc_domain;
//     
//     
//     viennagrid::io::poly_reader reader;
//     reader(plc_domain, "../../examples/data/big_and_small_cube.poly");
//     
//     
//     viennagrid::config::triangular_3d_domain triangulated_plc_domain;
//     viennamesh::result_of::settings<viennamesh::cgal_plc_3d_mesher_tag>::type plc_settings(0.0, 0.0);
//     
//     viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_domain, triangulated_plc_domain, plc_settings );
//     
//     {
//         viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
//         vtk_writer(triangulated_plc_domain, "meshed_plc_hull.vtu");
//     }
//     
//     
//     typedef viennagrid::result_of::point_type<viennagrid::config::triangular_3d_domain>::type point_type;
//     
//     std::cout << "Num triangles after PLC meshing: " << viennagrid::elements<viennagrid::triangle_tag>( triangulated_plc_domain ).size() << std::endl;
//     
//     
// //     typedef viennagrid::result_of::segmentation<viennagrid::config::triangular_3d_domain, viennagrid::triangle_tag>::type segmentation_type;
//     viennagrid::config::triangular_3d_segmentation triangulated_plc_segmentation;
//     
//     
//     std::vector< std::pair< int, point_type > > seed_points;
//     seed_points.push_back( std::make_pair(0, point_type(0.0, 0.0, 0.0)) );
//     seed_points.push_back( std::make_pair(1, point_type(0.0, 0.0, 20.0)) );
//     
//     viennagrid::mark_face_segments( triangulated_plc_domain, triangulated_plc_segmentation, seed_points.begin(), seed_points.end() );
// 
//     
//     viennagrid::config::triangular_3d_domain oriented_adapted_hull_domain;
//     viennagrid::config::triangular_3d_segmentation oriented_adapted_hull_segmentation;
//     viennamesh::result_of::settings<viennamesh::vgmodeler_hull_adaption_tag>::type vgm_settings;
//     
//     vgm_settings.cell_size = 3.0;
//     
//     viennamesh::run_algo< viennamesh::vgmodeler_hull_adaption_tag >( triangulated_plc_domain, triangulated_plc_segmentation,
//                                                                      oriented_adapted_hull_domain, oriented_adapted_hull_segmentation,
//                                                                      vgm_settings );
//     
//     
//     {        
//         viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
//         vtk_writer(oriented_adapted_hull_domain, "netgen_adapt_hull.vtu");
//     }
    
    
    
    
    
    
    
    typedef viennagrid::config::tetrahedral_3d_domain volume_domain_type;
    typedef viennagrid::result_of::domain_view<volume_domain_type>::type volume_view_type;
    typedef viennagrid::result_of::point_type<volume_domain_type>::type volume_point_type;
    typedef viennagrid::result_of::cell_type<volume_domain_type>::type volume_cell_type;
    
    volume_domain_type tet_domain;
    std::deque<volume_view_type> tet_segments;
    std::deque< std::pair<int, volume_point_type> > segment_seed_points;
    
    viennagrid::io::netgen_reader<volume_cell_type> reader;
    reader(tet_domain, tet_segments, "../../examples/data/half-trigate.mesh");
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::config::tetrahedral_3d_domain, viennagrid::config::tetrahedral_3d_cell> vtk_writer;
        vtk_writer(tet_domain, tet_segments, "output");
    }
    
    
    typedef viennagrid::config::triangular_3d_domain hull_domain_type;
    typedef viennagrid::result_of::domain_view<hull_domain_type>::type hull_view_type;
    
    hull_domain_type hull_domain;
    std::deque<hull_view_type> hull_segments( tet_segments.size() );
    
    for (int seg = 0; seg < tet_segments.size(); ++seg)
    {
        volume_view_type const & tet_segment = tet_segments[seg];
        
        volume_cell_type const & cell = viennagrid::cells(tet_segment)[0];
        segment_seed_points.push_back( std::make_pair(seg, viennagrid::centroid(tet_segment, cell) ) );
        
//         hull_view_type & hull_segment = hull_segments[seg];
        
//         hull_segment = viennagrid::create_view<hull_view_type>( hull_domain );
        
        typedef viennagrid::result_of::const_triangle_range<volume_view_type>::type triangle_range_type;
        typedef viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;
        typedef viennagrid::result_of::triangle<volume_view_type>::type triangle_type;
        
        triangle_range_type triangles = viennagrid::elements( tet_segment );
        for (triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
        {
            triangle_type const & triangle = *it;
            
            if ( viennagrid::is_boundary<volume_cell_type>( triangle, tet_segment ) )
            {
                viennagrid::create_triangle(
                    hull_domain,
                    viennagrid::create_unique_vertex( hull_domain, viennagrid::point(tet_segment, viennagrid::vertices(triangle)[0]) ),
                    viennagrid::create_unique_vertex( hull_domain, viennagrid::point(tet_segment, viennagrid::vertices(triangle)[1]) ),
                    viennagrid::create_unique_vertex( hull_domain, viennagrid::point(tet_segment, viennagrid::vertices(triangle)[2]) )
                );
            }
        }
    }
    
    {        
        viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
        vtk_writer(hull_domain, "output_hull.vtu");
    }
    
    
    viennagrid::config::plc_3d_domain plc_domain;
    
    extract_plcs(hull_domain, plc_domain);
    
    viennagrid::config::triangular_3d_domain triangulated_plc_domain;
    viennamesh::result_of::settings<viennamesh::cgal_plc_3d_mesher_tag>::type plc_settings(0.0, 0.0);
    
    viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_domain, triangulated_plc_domain, plc_settings );
    

    
    viennagrid::config::triangular_3d_segmentation triangulated_plc_segmentation;
    viennagrid::mark_face_segments( triangulated_plc_domain, triangulated_plc_segmentation, segment_seed_points.begin(), segment_seed_points.end() );
    
    viennagrid::result_of::cell_range<viennagrid::config::triangular_3d_domain>::type range = viennagrid::elements(triangulated_plc_domain);
    for (viennagrid::result_of::cell_range<viennagrid::config::triangular_3d_domain>::type::iterator it = range.begin(); it != range.end(); ++it)
    {
        if ( !triangulated_plc_segmentation.segment_info(*it).positive_orientation_segment_id.is_valid() &&
            !triangulated_plc_segmentation.segment_info(*it).negative_orientation_segment_id.is_valid() )
        {
            std::cout << "Element is NOT in any segment!" << std::endl;
            std::cout << "  " << viennagrid::point( triangulated_plc_domain, viennagrid::vertices(*it)[0] ) << std::endl;
            std::cout << "  " << viennagrid::point( triangulated_plc_domain, viennagrid::vertices(*it)[1] ) << std::endl;
            std::cout << "  " << viennagrid::point( triangulated_plc_domain, viennagrid::vertices(*it)[2] ) << std::endl;
            
            viennadata::access<std::string, double>("no_segment")(*it) = 1.0;
        }
        else
            viennadata::access<std::string, double>("no_segment")(*it) = 0.0;
    }
    
    
    {
        viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
        viennagrid::io::add_scalar_data_on_cells<std::string, double>(vtk_writer, "no_segment", "no_segment");
        vtk_writer(triangulated_plc_domain, "meshed_plc_hull.vtu");
    }
    
    
    viennagrid::config::triangular_3d_domain oriented_adapted_hull_domain;
    viennagrid::config::triangular_3d_segmentation oriented_adapted_hull_segmentation;
    viennamesh::result_of::settings<viennamesh::vgmodeler_hull_adaption_tag>::type vgm_settings;
    
    vgm_settings.cell_size = 3.0;
    
    viennamesh::run_algo< viennamesh::vgmodeler_hull_adaption_tag >( triangulated_plc_domain, triangulated_plc_segmentation,
                                                                     oriented_adapted_hull_domain, oriented_adapted_hull_segmentation,
                                                                     vgm_settings );
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_writer;
        vtk_writer(oriented_adapted_hull_domain, "netgen_adapt_hull.vtu");
    }

    
    viennagrid::config::tetrahedral_3d_domain tetrahedron_domain;
    viennagrid::config::tetrahedral_3d_segmentation tetrahedron_segmentation;
    viennamesh::result_of::settings<viennamesh::netgen_tetrahedron_tag>::type netgen_settings;
    
    netgen_settings.cell_size = 3.0;
    
    viennamesh::run_algo< viennamesh::netgen_tetrahedron_tag >( oriented_adapted_hull_domain, oriented_adapted_hull_segmentation,
                                                                tetrahedron_domain, tetrahedron_segmentation,
                                                                netgen_settings );
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::config::tetrahedral_3d_domain, viennagrid::config::tetrahedral_3d_cell> vtk_writer;
        vtk_writer(tetrahedron_domain, "netgen_volume.vtu");
    }

    
    
}
