#include <iostream>

#include "viennamesh/algorithm/cgal_plc_mesher.hpp"

#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"





int main()
{
    typedef viennagrid::plc_2d_mesh mesh_type;
    mesh_type mesh;
    
    typedef viennagrid::result_of::point<mesh_type>::type point_type;
     
    typedef viennagrid::result_of::element<mesh_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::handle<mesh_type, viennagrid::vertex_tag>::type vertex_handle_type;
    
    typedef viennagrid::result_of::element<mesh_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::handle<mesh_type, viennagrid::line_tag>::type line_handle_type;
    
    typedef viennagrid::result_of::element<mesh_type, viennagrid::plc_tag>::type plc_type;
    typedef viennagrid::result_of::handle<mesh_type, viennagrid::plc_tag>::type plc_handle_type;
    
    plc_handle_type plc_handle;
    
    {
        std::vector<vertex_handle_type> v;
        
        v.push_back( viennagrid::make_vertex( mesh, point_type(0, 0) ) );
        v.push_back( viennagrid::make_vertex( mesh, point_type(10, 0) ) );
        v.push_back( viennagrid::make_vertex( mesh, point_type(20, 10) ) );
        v.push_back( viennagrid::make_vertex( mesh, point_type(20, 20) ) );
        v.push_back( viennagrid::make_vertex( mesh, point_type(10, 20) ) );
        v.push_back( viennagrid::make_vertex( mesh, point_type(0, 10) ) );
        v.push_back( viennagrid::make_vertex( mesh, point_type(5, 5) ) );
        
        v.push_back( viennagrid::make_vertex( mesh, point_type(10, 10) ) );
        v.push_back( viennagrid::make_vertex( mesh, point_type(12, 10) ) );
        v.push_back( viennagrid::make_vertex( mesh, point_type(10, 12) ) );
        
        v.push_back( viennagrid::make_vertex( mesh, point_type(8, 10) ) );
        
        v.push_back( viennagrid::make_vertex( mesh, point_type(15, 15) ) );
        
        
        std::vector<line_handle_type> lines;
        
        {
            std::vector<vertex_handle_type>::iterator start = v.begin();
            std::vector<vertex_handle_type>::iterator end = v.begin() + 7;
            
            std::vector<vertex_handle_type>::iterator it1 = start;
            std::vector<vertex_handle_type>::iterator it2 = it1; ++it2;
            for (; it2 != end; ++it1, ++it2)
                lines.push_back( viennagrid::make_line(mesh, *it1, *it2) );
            lines.push_back( viennagrid::make_line(mesh, *it1, *start) );
        }
        
        
        {
            std::vector<vertex_handle_type>::iterator start = v.begin() + 7;
            std::vector<vertex_handle_type>::iterator end = v.begin() + 10;
            
            std::vector<vertex_handle_type>::iterator it1 = start;
            std::vector<vertex_handle_type>::iterator it2 = it1; ++it2;
            for (; it2 != end; ++it1, ++it2)
                lines.push_back( viennagrid::make_line(mesh, *it1, *it2) );
            lines.push_back( viennagrid::make_line(mesh, *it1, *start) );
        }
        
        lines.push_back( viennagrid::make_element<line_type>( mesh, v.begin() + 9, v.begin() + 11 ) );
        
        vertex_handle_type point = v[11];

        
        std::vector<point_type> hole_points;
        hole_points.push_back( point_type(10.5, 10.5) );

        plc_handle  = viennagrid::make_plc(  mesh,
                                                                        lines.begin(), lines.end(),
                                                                        &point, &point + 1,
                                                                        hole_points.begin(), hole_points.end()
                                                                    );
    }

    
    
    viennamesh::result_of::settings<viennamesh::cgal_plc_2d_mesher_tag>::type settings(0.3, 0.0);
    
    viennagrid::triangular_2d_mesh triangle_mesh;
    viennamesh::run_algo< viennamesh::cgal_plc_2d_mesher_tag >( mesh, triangle_mesh, settings );
    
    viennagrid::io::vtk_writer<viennagrid::triangular_2d_mesh> vtk_writer;
    vtk_writer(triangle_mesh, "test");

    
}
