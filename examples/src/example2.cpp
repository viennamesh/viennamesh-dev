#include <iostream>

// #include "viennamesh/base/cgal.hpp"
// #include "viennamesh/convert/cgal.hpp"
// #include "viennamesh/generation/triangle.hpp"

#include "viennagrid/domain/config.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"



#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>


int main()
{
    typedef viennagrid::config::plc_2d_domain domain_type;
    domain_type domain;
    
    typedef viennagrid::result_of::point_type<domain_type>::type point_type;
     
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::element_handle<domain_type, viennagrid::vertex_tag>::type vertex_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::element_handle<domain_type, viennagrid::line_tag>::type line_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::polygon_tag>::type polygon_type;
    typedef viennagrid::result_of::element_handle<domain_type, viennagrid::polygon_tag>::type polygon_handle_type;
    
    
    
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
    typedef viennagrid::result_of::element_handle<domain_type, viennagrid::plc_tag>::type plc_handle_type;
    
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
    
    
    
    
    
    typedef viennagrid::result_of::element_range<plc_type, viennagrid::vertex_tag>::type vertex_range_type;
    typedef viennagrid::result_of::handle_iterator<vertex_range_type>::type vertex_range_handle_iterator;
    
    typedef viennagrid::result_of::element_range<plc_type, viennagrid::line_tag>::type line_range_type;
    typedef viennagrid::result_of::iterator<line_range_type>::type line_range_iterator;
    
    
    
    typedef CGAL::Exact_predicates_inexact_constructions_kernel     Kernel;
    
    typedef CGAL::Triangulation_vertex_base_2<Kernel>               VertexBase;
    typedef CGAL::Delaunay_mesh_face_base_2<Kernel>                 FaceBase;
    typedef CGAL::Triangulation_data_structure_2<VertexBase, FaceBase> Triangulation_structure;
    
    typedef CGAL::Constrained_Delaunay_triangulation_2<Kernel, Triangulation_structure> CDT;
    
    typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
    
    typedef CDT::Vertex_handle Vertex_handle;
    typedef CDT::Point Point;
    
    CDT cdt;
    
    std::map<vertex_handle_type, Vertex_handle> vertex_handle_map;
    
    vertex_range_type vertices = viennagrid::elements<viennagrid::vertex_tag>(plc);
    for (vertex_range_handle_iterator it = vertices.handle_begin(); it != vertices.handle_end(); ++it)
    {
        vertex_handle_type const & vtx_handle = *it;
        vertex_type const & vtx = viennagrid::dereference_handle(domain, *it);
        point_type const & vgrid_point = viennagrid::point( domain, vtx );
        
        Vertex_handle handle = cdt.insert( Point(vgrid_point[0], vgrid_point[1]) );
        
        vertex_handle_map[vtx_handle] = handle;
    }

    
    line_range_type lines = viennagrid::elements<viennagrid::line_tag>(plc);
    for (line_range_iterator it = lines.begin(); it != lines.end(); ++it)
    {
        line_type & line = *it;
        
        vertex_handle_type vgrid_v0 = viennagrid::elements<viennagrid::vertex_tag>(line).handle_at(0);
        vertex_handle_type vgrid_v1 = viennagrid::elements<viennagrid::vertex_tag>(line).handle_at(1);
        
        Vertex_handle cgal_v0 = vertex_handle_map[vgrid_v0];
        Vertex_handle cgal_v1 = vertex_handle_map[vgrid_v1];
        
        cdt.insert_constraint(cgal_v0, cgal_v1);
    }
    
    std::vector<point_type> & vgrid_list_of_holes = viennagrid::hole_points<domain_type>(plc);
    std::list<Point> cgal_list_of_holes;
    
    for (std::vector<point_type>::iterator it = vgrid_list_of_holes.begin(); it != vgrid_list_of_holes.end(); ++it)
        cgal_list_of_holes.push_back( Point( (*it)[0], (*it)[1] ) );
    
    CGAL::refine_Delaunay_mesh_2(cdt, cgal_list_of_holes.begin(), cgal_list_of_holes.end(), Criteria());
    
    std::cout << "Number of vertices: " << cdt.number_of_vertices() << std::endl;
    std::cout << "Number of finite faces: " << cdt.number_of_faces() << std::endl;
    
    
    
    
    
    typedef viennagrid::config::triangular_2d_domain triangle_domain_type;
    triangle_domain_type triangle_domain;
    
    typedef viennagrid::result_of::point_type<triangle_domain_type>::type triangle_point_type;
     
    typedef viennagrid::result_of::element<triangle_domain_type, viennagrid::vertex_tag>::type triangle_vertex_type;
    typedef viennagrid::result_of::element_handle<triangle_domain_type, viennagrid::vertex_tag>::type triangle_vertex_handle_type;
    
    typedef viennagrid::result_of::element<triangle_domain_type, viennagrid::line_tag>::type triangle_line_type;
    typedef viennagrid::result_of::element_handle<triangle_domain_type, viennagrid::line_tag>::type triangle_line_handle_type;
    
    typedef viennagrid::result_of::element<triangle_domain_type, viennagrid::triangle_tag>::type triangle_triangle_type;
    typedef viennagrid::result_of::element_handle<triangle_domain_type, viennagrid::triangle_tag>::type triangle_triangle__handle_type;
    
    
    std::map<Point, triangle_vertex_handle_type> points;
    
    int mesh_faces_counter = 0;
    for(CDT::Finite_faces_iterator fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) 
    {
        if(fit->is_in_domain())
        {
            typedef CDT::Triangle Triangle;
            Triangle tri = cdt.triangle(fit);
            
            triangle_vertex_handle_type vgrid_vtx[3];
            
            for (int i = 0; i < 3; ++i)
            {
                std::map<Point, triangle_vertex_handle_type>::iterator pit = points.find( tri[i] );
                if (pit == points.end())
                {
                    vgrid_vtx[i] = viennagrid::create_element<triangle_vertex_type>( triangle_domain, triangle_point_type(tri[i].x(), tri[i].y()) );
                    points[ tri[i] ] = vgrid_vtx[i];
                }
                else
                    vgrid_vtx[i] = pit->second;
            }
            
            viennagrid::create_element<triangle_triangle_type>( triangle_domain, vgrid_vtx, vgrid_vtx+3 );
                
            
            std::cout << tri << std::endl;
            ++mesh_faces_counter;
        }
    }
    std::cout << "Number of faces in the mesh domain: " << mesh_faces_counter << std::endl;
    

    
    std::copy( viennagrid::elements<triangle_triangle_type>(triangle_domain).begin(), viennagrid::elements<triangle_triangle_type>(triangle_domain).end(), std::ostream_iterator<triangle_triangle_type>(std::cout, "\n") );
    

    viennagrid::io::vtk_writer<triangle_domain_type, triangle_triangle_type> vtk_writer;
    vtk_writer(triangle_domain, "test.vtu");
    
}
