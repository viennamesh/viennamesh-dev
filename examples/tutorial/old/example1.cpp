#include <iostream>

#include "viennamesh/base/triangle_datastructure.hpp"
#include "viennamesh/convert/triangle_convert.hpp"
// #include "viennamesh/generation/triangle.hpp"

#include "viennagrid/mesh/element_creation.hpp"

int main()
{
    typedef viennagrid::config::plc_2d_mesh mesh_type;
    mesh_type mesh;
    
    typedef viennagrid::result_of::point_type<mesh_type>::type point_type;
     
    typedef viennagrid::result_of::element<mesh_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::handle<mesh_type, viennagrid::vertex_tag>::type vertex_handle_type;
    
    typedef viennagrid::result_of::element<mesh_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::handle<mesh_type, viennagrid::line_tag>::type line_handle_type;
    
     typedef viennagrid::result_of::element<mesh_type, viennagrid::plc_tag>::type plc_type;
    typedef viennagrid::result_of::handle<mesh_type, viennagrid::plc_tag>::type plc_handle_type;
    
    plc_handle_type plc_handle;
    
    {
        std::vector<vertex_handle_type> v;
        
        v.push_back( viennagrid::create_element<vertex_type>( mesh, point_type(0, 0) ) );
        v.push_back( viennagrid::create_element<vertex_type>( mesh, point_type(10, 0) ) );
        v.push_back( viennagrid::create_element<vertex_type>( mesh, point_type(20, 10) ) );
        v.push_back( viennagrid::create_element<vertex_type>( mesh, point_type(20, 20) ) );
        v.push_back( viennagrid::create_element<vertex_type>( mesh, point_type(10, 20) ) );
        v.push_back( viennagrid::create_element<vertex_type>( mesh, point_type(0, 10) ) );
        v.push_back( viennagrid::create_element<vertex_type>( mesh, point_type(5, 5) ) );
        
        v.push_back( viennagrid::create_element<vertex_type>( mesh, point_type(10, 10) ) );
        v.push_back( viennagrid::create_element<vertex_type>( mesh, point_type(12, 10) ) );
        v.push_back( viennagrid::create_element<vertex_type>( mesh, point_type(10, 12) ) );
        
        v.push_back( viennagrid::create_element<vertex_type>( mesh, point_type(8, 10) ) );
        
        v.push_back( viennagrid::create_element<vertex_type>( mesh, point_type(15, 15) ) );
        
        
        std::vector<line_handle_type> lines;
        
        {
            std::vector<vertex_handle_type>::iterator start = v.begin();
            std::vector<vertex_handle_type>::iterator end = v.begin() + 7;
            
            std::vector<vertex_handle_type>::iterator it1 = start;
            std::vector<vertex_handle_type>::iterator it2 = it1; ++it2;
            for (; it2 != end; ++it1, ++it2)
                lines.push_back( viennagrid::create_line(mesh, *it1, *it2) );
            lines.push_back( viennagrid::create_line(mesh, *it1, *start) );
        }
        
        
        {
            std::vector<vertex_handle_type>::iterator start = v.begin() + 7;
            std::vector<vertex_handle_type>::iterator end = v.begin() + 10;
            
            std::vector<vertex_handle_type>::iterator it1 = start;
            std::vector<vertex_handle_type>::iterator it2 = it1; ++it2;
            for (; it2 != end; ++it1, ++it2)
                lines.push_back( viennagrid::create_line(mesh, *it1, *it2) );
            lines.push_back( viennagrid::create_line(mesh, *it1, *start) );
        }
        
        lines.push_back( viennagrid::create_element<line_type>( mesh, v.begin() + 9, v.begin() + 11 ) );
        
        vertex_handle_type point = v[11];

        
        std::vector<point_type> hole_points;
        hole_points.push_back( point_type(10.5, 10.5) );

        plc_handle  = viennagrid::create_element<plc_type>(  mesh, 
                                                                        lines.begin(), lines.end(),
                                                                        &point, &point + 1,
                                                                        hole_points.begin(), hole_points.end()
                                                                    );
    }
    
    
    plc_type & plc = viennagrid::dereference_handle(mesh, plc_handle);
    
    
    
    
    
    viennamesh::viennamesh_triangle::mesh triangle_mesh;
    viennamesh::viennamesh_triangle::cell_element cell;
    
    viennamesh::convert::element_convert(plc, mesh, cell, triangle_mesh);
    
    
    
    

    
}
