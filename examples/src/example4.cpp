#include <iostream>

// #include "viennamesh/base/cgal.hpp"
// #include "viennamesh/convert/cgal.hpp"
// #include "viennamesh/generation/triangle.hpp"


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

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Mesh_polyhedron_3.h>

#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/Polyhedron_copy_3.h>
#include <CGAL/Polyhedral_mesh_domain_with_features_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/refine_mesh_3.h>

using namespace CGAL::parameters;

#define CGAL_MESH_3_VERBOSE 1


int main()
{
    // Domain
    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//     typedef CGAL::Polyhedron_3<K> Polyhedron;
    typedef CGAL::Mesh_polyhedron_3<K>::type MeshPolyhedron;
    typedef CGAL::Polyhedral_mesh_domain_with_features_3<K, MeshPolyhedron> Mesh_domain;

    // Triangulation
    typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
    typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr, Mesh_domain::Corner_index,Mesh_domain::Curve_segment_index> C3t3;

    // Criteria
    typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
    
    
    typedef MeshPolyhedron::Point_3 Point;
    
    
    double s = 10.0;
    
    MeshPolyhedron polyhedron;
    
    polyhedron.make_triangle( Point(0, 0, 0), Point(s, 0, 0), Point(0, s, 0) );
    polyhedron.make_triangle( Point(s, 0, 0), Point(0, s, 0), Point(s, s, 0) );
    
    polyhedron.make_triangle( Point(0, 0, s), Point(s, 0, s), Point(0, s, s) );
    polyhedron.make_triangle( Point(s, 0, s), Point(0, s, s), Point(s, s, s) );
    
    polyhedron.make_triangle( Point(0, 0, 0), Point(s, 0, 0), Point(0, 0, s) );
    polyhedron.make_triangle( Point(s, 0, 0), Point(0, 0, s), Point(s, 0, s) );

    polyhedron.make_triangle( Point(0, s, 0), Point(s, s, 0), Point(0, s, s) );
    polyhedron.make_triangle( Point(s, s, 0), Point(0, s, s), Point(s, s, s) );
    
    polyhedron.make_triangle( Point(0, 0, 0), Point(0, s, 0), Point(0, 0, s) );
    polyhedron.make_triangle( Point(0, s, 0), Point(0, 0, s), Point(0, s, s) );
    
    polyhedron.make_triangle( Point(s, 0, 0), Point(s, s, 0), Point(s, 0, s) );
    polyhedron.make_triangle( Point(s, s, 0), Point(s, 0, s), Point(s, s, s) );
    
//     MeshPolyhedron mesh_polyhedron;
//     
//     typedef CGAL::Polyhedron_copy_3<Polyhedron, MeshPolyhedron::HDS> Poly_copy;
//     Poly_copy polyhedron_copy_modifier(polyhedron);
//     
//     mesh_polyhedron.delegate(polyhedron_copy_modifier);

    
    
    std::vector< std::vector<Point> > lines;
    
    lines.resize(8);
    lines[0].resize(4);
    lines[0][0] = Point(0, 0, 0);
    lines[0][1] = Point(s, 0, 0);
    lines[0][2] = Point(s, s, 0);
    lines[0][3] = Point(0, s, 0);
    lines[1].resize(4);
    lines[1][0] = Point(0, 0, s);
    lines[1][1] = Point(s, 0, s);
    lines[1][2] = Point(s, s, s);
    lines[1][3] = Point(0, s, s);
    lines[2].resize(2);
    lines[2][0] = Point(0, 0, 0);
    lines[2][1] = Point(0, s, 0);
    lines[3].resize(2);
    lines[3][0] = Point(0, 0, s);
    lines[3][1] = Point(0, s, s);
    lines[4].resize(2);
    lines[4][0] = Point(0, 0, 0);
    lines[4][1] = Point(0, 0, s);
    lines[5].resize(2);
    lines[5][0] = Point(s, 0, 0);
    lines[5][1] = Point(s, 0, s);
    lines[6].resize(2);
    lines[6][0] = Point(s, s, 0);
    lines[6][1] = Point(s, s, s);
    lines[7].resize(2);
    lines[7][0] = Point(0, s, 0);
    lines[7][1] = Point(0, s, s);
    
    
//     lines[1].resize(2);
//     lines[1][0] = Point(s, 0, 0);
//     lines[1][1] = Point(s, s, 0);
//     lines[2].resize(2);
//     lines[2][0] = Point(s, s, 0);
//     lines[2][1] = Point(0, s, 0);
//     lines[3].resize(2);
//     lines[3][0] = Point(0, s, 0);
//     lines[3][1] = Point(0, 0, 0);
    
    
    
    Mesh_domain domain(polyhedron);
    
    
    domain.add_features(lines.begin(), lines.end());
//     
//     
//     
//     
//     
//     
//     domain.detect_features();
    

  // Mesh criteria
//   Mesh_criteria criteria;
  
  // Mesh generation
//   C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, features(domain));

    
  // Output
//   std::ofstream medit_file("out.mesh");
//   c3t3.output_to_medit(medit_file);
  
    

    Mesh_criteria criteria;//(facet_angle=25, facet_size=0.15, facet_distance=0.008, cell_radius_edge_ratio=3);
    
//   Mesh_criteria criteria(edge_size = 0.025,
//                          facet_angle = 25, facet_size = 0.05, facet_distance = 0.005,
//                          cell_radius_edge_ratio = 3, cell_size = 0.05);

//     domain.detect_features();
    C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria);
    
    
//     typedef C3t3::Cell_handle Cell_handle;
    typedef C3t3::Cell_iterator  Cell_iterator;
    
//     for (Cell_iterator it = c3t3.cells_begin(); it != c3t3.cells_end(); ++it)
//     {
//         Tr::Cell t = *it;
//         std::cout << t.vertex(0)->point() << std::endl;
//         std::cout << t.vertex(1)->point() << std::endl;
//         std::cout << t.vertex(2)->point() << std::endl;
//         std::cout << t.vertex(3)->point() << std::endl;
//         
//         std::cout << std::endl;
//     }
    
    
    typedef Tr::Vertex_handle Vertex_handle;
    
    
    typedef viennagrid::config::tetrahedral_3d_domain tet_domain_type;
    tet_domain_type tet_domain;
    
    typedef viennagrid::result_of::point_type<tet_domain_type>::type tet_point_type;
     
    typedef viennagrid::result_of::element<tet_domain_type, viennagrid::vertex_tag>::type tet_vertex_type;
    typedef viennagrid::result_of::handle<tet_domain_type, viennagrid::vertex_tag>::type tet_vertex_handle_type;
    
    typedef viennagrid::result_of::element<tet_domain_type, viennagrid::line_tag>::type tet_line_type;
    typedef viennagrid::result_of::handle<tet_domain_type, viennagrid::line_tag>::type tet_line_handle_type;
    
    typedef viennagrid::result_of::element<tet_domain_type, viennagrid::triangle_tag>::type tet_triangle_type;
    typedef viennagrid::result_of::handle<tet_domain_type, viennagrid::triangle_tag>::type tet_triangle__handle_type;

    typedef viennagrid::result_of::element<tet_domain_type, viennagrid::tetrahedron_tag>::type tet_tetrahedron_type;
    typedef viennagrid::result_of::handle<tet_domain_type, viennagrid::tetrahedron_tag>::type tet_tetrahedron_handle_type;
    
    
    std::map<Vertex_handle, tet_vertex_handle_type> points;
    
    int mesh_faces_counter = 0;
    for (Cell_iterator it = c3t3.cells_in_complex_begin(); it != c3t3.cells_in_complex_end(); ++it)
    {
        Tr::Cell t = *it;
        
        tet_vertex_handle_type vgrid_vtx[4];
        
        for (int i = 0; i < 4; ++i)
        {
            std::map<Vertex_handle, tet_vertex_handle_type>::iterator pit = points.find( t.vertex(i) );
            if (pit == points.end())
            {
                tet_point_type tmp;
                tmp[0] = t.vertex(i)->point().x();
                tmp[2] = t.vertex(i)->point().y();
                tmp[1] = t.vertex(i)->point().z();
                
//                 double hw = t.vertex(i)->point().hw();
//                 if ( !(std::abs(hw) < 1e-6) )
//                 {
//                     tmp[0] /= hw;
//                     tmp[1] /= hw;
//                     tmp[2] /= hw;
//                 }
                
                vgrid_vtx[i] = viennagrid::create_vertex( tet_domain, tmp );
                points[ t.vertex(i) ] = vgrid_vtx[i];
            }
            else
                vgrid_vtx[i] = pit->second;
        }
        
        viennagrid::create_element<tet_tetrahedron_type>( tet_domain, vgrid_vtx, vgrid_vtx+4 );
            
        
//         std::cout << tri << std::endl;
        ++mesh_faces_counter;
    }
    std::cout << "Number of faces in the mesh domain: " << mesh_faces_counter << std::endl;
    

    
//     std::copy( viennagrid::elements<tet_tetrahedron_type>(tet_domain).begin(), viennagrid::elements<tet_tetrahedron_type>(tet_domain).end(), std::ostream_iterator<tet_tetrahedron_type>(std::cout, "\n") );
    

    viennagrid::io::vtk_writer<tet_domain_type, tet_tetrahedron_type> vtk_writer;
    vtk_writer(tet_domain, "test_tet_3d.vtu");
    
}
