#include <iostream>

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"


#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Projection_traits_xy_3.h>


int main()
{
    typedef viennagrid::config::plc_3d_domain domain_type;
    domain_type domain;
    
    typedef viennagrid::result_of::point_type<domain_type>::type point_type;
     
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::vertex_tag>::type vertex_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::line_tag>::type line_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::plc_tag>::type plc_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::plc_tag>::type plc_handle_type;
    
    plc_handle_type plc_handle;
    
    {
        std::vector<vertex_handle_type> v;
        
        v.push_back( viennagrid::create_vertex( domain, point_type(0, 0) ) );
        v.push_back( viennagrid::create_vertex( domain, point_type(10, 0) ) );
        v.push_back( viennagrid::create_vertex( domain, point_type(20, 10) ) );
        v.push_back( viennagrid::create_vertex( domain, point_type(20, 20) ) );
        v.push_back( viennagrid::create_vertex( domain, point_type(10, 20) ) );
        v.push_back( viennagrid::create_vertex( domain, point_type(0, 10) ) );
        v.push_back( viennagrid::create_vertex( domain, point_type(5, 5) ) );
        
        v.push_back( viennagrid::create_vertex( domain, point_type(10, 10) ) );
        v.push_back( viennagrid::create_vertex( domain, point_type(12, 10) ) );
        v.push_back( viennagrid::create_vertex( domain, point_type(10, 12) ) );
        
        v.push_back( viennagrid::create_vertex( domain, point_type(8, 10) ) );
        
        v.push_back( viennagrid::create_vertex( domain, point_type(15, 15) ) );
        
        
        std::vector<line_handle_type> lines;
        
        {
            std::vector<vertex_handle_type>::iterator start = v.begin();
            std::vector<vertex_handle_type>::iterator end = v.begin() + 7;
            
            std::vector<vertex_handle_type>::iterator it1 = start;
            std::vector<vertex_handle_type>::iterator it2 = it1; ++it2;
            for (; it2 != end; ++it1, ++it2)
                lines.push_back( viennagrid::create_line(domain, *it1, *it2) );
            lines.push_back( viennagrid::create_line(domain, *it1, *start) );
        }
        
        
        {
            std::vector<vertex_handle_type>::iterator start = v.begin() + 7;
            std::vector<vertex_handle_type>::iterator end = v.begin() + 10;
            
            std::vector<vertex_handle_type>::iterator it1 = start;
            std::vector<vertex_handle_type>::iterator it2 = it1; ++it2;
            for (; it2 != end; ++it1, ++it2)
                lines.push_back( viennagrid::create_line(domain, *it1, *it2) );
            lines.push_back( viennagrid::create_line(domain, *it1, *start) );
        }
        
        lines.push_back( viennagrid::create_element<line_type>( domain, v.begin() + 9, v.begin() + 11 ) );
        
        vertex_handle_type point = v[11];

        
        std::vector<point_type> hole_points;
        hole_points.push_back( point_type(10.5, 10.5) );

        plc_handle  = viennagrid::create_plc(  domain, 
                                                                        lines.begin(), lines.end(),
                                                                        &point, &point + 1,
                                                                        hole_points.begin(), hole_points.end()
                                                                    );
    }
    
    plc_type & plc = viennagrid::dereference_handle(domain, plc_handle);
    
    
    
    
    
    typedef viennagrid::result_of::element_range<plc_type, viennagrid::vertex_tag>::type vertex_range_type;
    typedef viennagrid::result_of::handle_iterator<vertex_range_type>::type vertex_range_handle_iterator;
    
    typedef viennagrid::result_of::element_range<plc_type, viennagrid::line_tag>::type line_range_type;
    typedef viennagrid::result_of::iterator<line_range_type>::type line_range_iterator;
    
    
    typedef viennagrid::config::point_type_2d point_type_2d;
    
    std::vector<point_type> plc_points_3d;
    std::vector<point_type_2d> plc_points_2d;
    
    std::map<vertex_handle_type, std::size_t> vertex_to_index_map;
    
    vertex_range_type vertices = viennagrid::elements<viennagrid::vertex_tag>(plc);
    for (vertex_range_handle_iterator it = vertices.handle_begin(); it != vertices.handle_end(); ++it)
    {
        vertex_handle_type const & vtx_handle = *it;
        vertex_type const & vtx = viennagrid::dereference_handle(domain, *it);
        point_type const & vgrid_point = viennagrid::point( domain, vtx );
        
        vertex_to_index_map[vtx_handle] = plc_points_3d.size();
        plc_points_3d.push_back(vgrid_point);
    }
    
    point_type center;
    point_type projection_matrix[2];
    
    viennagrid::geometry::projection_matrix( plc_points_3d.begin(), plc_points_3d.end(), 1e-6, center, projection_matrix );
    
    plc_points_2d.resize( plc_points_3d.size() );
    viennagrid::geometry::project( plc_points_3d.begin(), plc_points_3d.end(), plc_points_2d.begin(), center, (point_type*)projection_matrix, projection_matrix + 2 );
    
    
    for (size_t i = 0; i < plc_points_3d.size(); ++i)
    {
        std::cout << plc_points_3d[i] << "  - " << plc_points_2d[i] << std::endl;
    }
    
    
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
    
//     vertex_range_type vertices = viennagrid::elements<viennagrid::vertex_tag>(plc);
    for (vertex_range_handle_iterator it = vertices.handle_begin(); it != vertices.handle_end(); ++it)
    {
        vertex_handle_type const & vtx_handle = *it;
        vertex_type const & vtx = viennagrid::dereference_handle(domain, *it);
        Point cgal_point;
        point_type_2d const & vgrid_point = plc_points_2d[ vertex_to_index_map[vtx_handle] ];
        
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
    std::vector<point_type_2d> vgrid_list_of_holes_2d(vgrid_list_of_holes.size());
    
    viennagrid::geometry::project( vgrid_list_of_holes.begin(), vgrid_list_of_holes.end(), vgrid_list_of_holes_2d.begin(), center, (point_type*)projection_matrix, projection_matrix + 2 );
    
    std::list<Point> cgal_list_of_holes;
    
    for (std::vector<point_type_2d>::iterator it = vgrid_list_of_holes_2d.begin(); it != vgrid_list_of_holes_2d.end(); ++it)
        cgal_list_of_holes.push_back( Point( (*it)[0], (*it)[1] ) );
    
    CGAL::refine_Delaunay_mesh_2(cdt, cgal_list_of_holes.begin(), cgal_list_of_holes.end(), Criteria());
    
    std::cout << "Number of vertices: " << cdt.number_of_vertices() << std::endl;
    std::cout << "Number of finite faces: " << cdt.number_of_faces() << std::endl;
    
    
    
    
    
    typedef viennagrid::config::triangular_3d_domain triangle_domain_type;
    triangle_domain_type triangle_domain;
    
    typedef viennagrid::result_of::point_type<triangle_domain_type>::type triangle_point_type;
     
    typedef viennagrid::result_of::element<triangle_domain_type, viennagrid::vertex_tag>::type triangle_vertex_type;
    typedef viennagrid::result_of::handle<triangle_domain_type, viennagrid::vertex_tag>::type triangle_vertex_handle_type;
    
    typedef viennagrid::result_of::element<triangle_domain_type, viennagrid::line_tag>::type triangle_line_type;
    typedef viennagrid::result_of::handle<triangle_domain_type, viennagrid::line_tag>::type triangle_line_handle_type;
    
    typedef viennagrid::result_of::element<triangle_domain_type, viennagrid::triangle_tag>::type triangle_triangle_type;
    typedef viennagrid::result_of::handle<triangle_domain_type, viennagrid::triangle_tag>::type triangle_triangle__handle_type;
    
    
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
                    triangle_point_type tmp = projection_matrix[0] * tri[i].x() + projection_matrix[1] * tri[i].y() + center;
                    
                    std::cout << tmp << std::endl;
                    
                    vgrid_vtx[i] = viennagrid::create_vertex( triangle_domain, tmp );
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
    vtk_writer(triangle_domain, "test_3d.vtu");
    
}
