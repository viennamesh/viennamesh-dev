#include <iostream>

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"

#include "viennamesh/algorithm/cgal_plc_mesher.hpp"


#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Projection_traits_xy_3.h>


int main()
{
    typedef viennagrid::plc_3d_domain domain_type;
    domain_type domain;
    
    typedef viennagrid::result_of::point<domain_type>::type point_type;
     
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::vertex_tag>::type vertex_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::line_tag>::type line_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::plc_tag>::type plc_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::plc_tag>::type plc_handle_type;
    
    plc_handle_type plc_handle;
    
    {
        std::vector<vertex_handle_type> v;
        
        v.push_back( viennagrid::make_vertex( domain, point_type(0, 0) ) );
        v.push_back( viennagrid::make_vertex( domain, point_type(10, 0) ) );
        v.push_back( viennagrid::make_vertex( domain, point_type(20, 10) ) );
        v.push_back( viennagrid::make_vertex( domain, point_type(20, 20) ) );
        v.push_back( viennagrid::make_vertex( domain, point_type(10, 20) ) );
        v.push_back( viennagrid::make_vertex( domain, point_type(0, 10) ) );
        v.push_back( viennagrid::make_vertex( domain, point_type(5, 5) ) );
        
        v.push_back( viennagrid::make_vertex( domain, point_type(10, 10) ) );
        v.push_back( viennagrid::make_vertex( domain, point_type(12, 10) ) );
        v.push_back( viennagrid::make_vertex( domain, point_type(10, 12) ) );
        
        v.push_back( viennagrid::make_vertex( domain, point_type(8, 10) ) );
        
        v.push_back( viennagrid::make_vertex( domain, point_type(15, 15) ) );
        
        
        std::vector<line_handle_type> lines;
        
        {
            std::vector<vertex_handle_type>::iterator start = v.begin();
            std::vector<vertex_handle_type>::iterator end = v.begin() + 7;
            
            std::vector<vertex_handle_type>::iterator it1 = start;
            std::vector<vertex_handle_type>::iterator it2 = it1; ++it2;
            for (; it2 != end; ++it1, ++it2)
                lines.push_back( viennagrid::make_line(domain, *it1, *it2) );
            lines.push_back( viennagrid::make_line(domain, *it1, *start) );
        }
        
        
        {
            std::vector<vertex_handle_type>::iterator start = v.begin() + 7;
            std::vector<vertex_handle_type>::iterator end = v.begin() + 10;
            
            std::vector<vertex_handle_type>::iterator it1 = start;
            std::vector<vertex_handle_type>::iterator it2 = it1; ++it2;
            for (; it2 != end; ++it1, ++it2)
                lines.push_back( viennagrid::make_line(domain, *it1, *it2) );
            lines.push_back( viennagrid::make_line(domain, *it1, *start) );
        }
        
        lines.push_back( viennagrid::make_element<line_type>( domain, v.begin() + 9, v.begin() + 11 ) );
        
        vertex_handle_type point = v[11];

        
        std::vector<point_type> hole_points;
        hole_points.push_back( point_type(10.5, 10.5) );

        plc_handle  = viennagrid::make_plc(  domain,
                                                                        lines.begin(), lines.end(),
                                                                        &point, &point + 1,
                                                                        hole_points.begin(), hole_points.end()
                                                                    );
    }
    
    
    plc_type & plc = viennagrid::dereference_handle(domain, plc_handle);
    
    
    
    
    
    viennamesh::result_of::settings<viennamesh::cgal_plc_3d_mesher_tag>::type settings(0.3, 0.0);
    
    viennagrid::triangular_3d_domain triangle_domain;
    viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( domain, triangle_domain, settings );
    
    viennagrid::io::vtk_writer<viennagrid::triangular_3d_domain> vtk_writer;
    vtk_writer(triangle_domain, "test_plc_3d");

}
