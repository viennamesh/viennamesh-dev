#include <iostream>

#include "viennamesh/base/triangle_datastructure.hpp"
#include "viennamesh/convert/triangle_convert.hpp"
// #include "viennamesh/generation/triangle.hpp"

#include "viennagrid/domain/element_creation.hpp"

int main()
{
    typedef viennagrid::config::plc_2d_domain domain_type;
    domain_type domain;
    
    typedef viennagrid::result_of::point_type<domain_type>::type point_type;
     
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::vertex_tag>::type vertex_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::line_tag>::type line_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::polygon_tag>::type polygon_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::polygon_tag>::type polygon_handle_type;
    
    
    
    std::vector<vertex_handle_type> v;
    
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
    
    
    polygon_handle_type bounding_poly = viennagrid::create_element<polygon_type>( domain, v.begin(), v.begin() + 7 );
    
    polygon_handle_type hole_poly = viennagrid::create_element<polygon_type>( domain, v.begin() + 7, v.begin() + 10 );
    
    line_handle_type line = viennagrid::create_element<line_type>( domain, v.begin() + 9, v.begin() + 11 );
    
    vertex_handle_type point = v[11];    

    typedef viennagrid::result_of::element<domain_type, viennagrid::plc_tag>::type plc_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::plc_tag>::type plc_handle_type;
    
    std::vector<point_type> hole_points;
    hole_points.push_back( point_type(10.5, 10.5) );

    plc_handle_type plc_handle = viennagrid::create_element<plc_type>(  domain, 
                                                                    &bounding_poly, &bounding_poly + 1,
                                                                    &hole_poly, &hole_poly+1,
                                                                    &line, &line+1,
                                                                    &point, &point + 1,
                                                                    hole_points.begin(), hole_points.end()
                                                                 );
    
    
    plc_type & plc = viennagrid::dereference_handle(domain, plc_handle);
    
    
    
    
    
    viennamesh::viennamesh_triangle::domain triangle_domain;
    viennamesh::viennamesh_triangle::cell_element cell;
    
    viennamesh::convert::element_convert(plc, domain, cell, triangle_domain);
    
    
    
    

    
}
