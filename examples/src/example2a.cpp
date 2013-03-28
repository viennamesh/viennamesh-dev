#include <iostream>

#include "viennamesh/algorithm/cgal_plc_mesher.hpp"

#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"





int main()
{
    typedef viennagrid::config::plc_2d_domain domain_type;
    domain_type domain;
    
    typedef viennagrid::result_of::point_type<domain_type>::type point_type;
     
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::vertex_tag>::type vertex_hook_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::line_tag>::type line_hook_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::polygon_tag>::type polygon_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::polygon_tag>::type polygon_hook_type;
    
    
    
    std::vector<vertex_hook_type> v;
    
    v.push_back( viennagrid::create_element<vertex_type>( domain, point_type(0, 0) ) );
    v.push_back( viennagrid::create_element<vertex_type>( domain, point_type(10, 0) ) );
    v.push_back( viennagrid::create_element<vertex_type>( domain, point_type(20, 10) ) );
    v.push_back( viennagrid::create_element<vertex_type>( domain, point_type(20, 20) ) );
    v.push_back( viennagrid::create_element<vertex_type>( domain, point_type(10, 20) ) );
    v.push_back( viennagrid::create_element<vertex_type>( domain, point_type(0, 10) ) );
    v.push_back( viennagrid::create_element<vertex_type>( domain, point_type(5, 5) ) );
    
    v.push_back( viennagrid::create_element<vertex_type>( domain, point_type(10, 10) ) );
    v.push_back( viennagrid::create_element<vertex_type>( domain, point_type(12, 10) ) );
    v.push_back( viennagrid::create_element<vertex_type>( domain, point_type(10, 12) ) );
    
    v.push_back( viennagrid::create_element<vertex_type>( domain, point_type(8, 10) ) );
    
    v.push_back( viennagrid::create_element<vertex_type>( domain, point_type(15, 15) ) );
    
    
    polygon_hook_type bounding_poly = viennagrid::create_element<polygon_type>( domain, v.begin(), v.begin() + 7 );
    
    polygon_hook_type hole_poly = viennagrid::create_element<polygon_type>( domain, v.begin() + 7, v.begin() + 10 );
    
    line_hook_type line = viennagrid::create_element<line_type>( domain, v.begin() + 9, v.begin() + 11 );
    
    vertex_hook_type point = v[11];    

    typedef viennagrid::result_of::element<domain_type, viennagrid::plc_tag>::type plc_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::plc_tag>::type plc_hook_type;
    
    std::vector<point_type> hole_points;
    hole_points.push_back( point_type(10.5, 10.5) );

    viennagrid::create_element<plc_type>(domain, 
                                         &bounding_poly, &bounding_poly + 1,
                                         &hole_poly, &hole_poly+1,
                                         &line, &line+1,
                                         &point, &point + 1,
                                         hole_points.begin(), hole_points.end());
   


    
    
    viennamesh::result_of::settings<viennamesh::cgal_plc_2d_mesher_tag>::type settings(0.3, 0.0);
    
    viennagrid::config::triangular_2d_domain triangle_domain;
    viennamesh::run_algo< viennamesh::cgal_plc_2d_mesher_tag >( domain, triangle_domain, settings );
    
    viennagrid::io::vtk_writer<viennagrid::config::triangular_2d_domain, viennagrid::config::triangular_2d_cell> vtk_writer;
    vtk_writer(triangle_domain, "test.vtu");

    
}
