#include <iostream>

#include "viennamesh/algorithm/tetgen_tetrahedron_mesher.hpp"

#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"





int main()
{
    typedef viennagrid::triangular_3d_domain hull_domain_type;
    
    typedef viennagrid::result_of::point<hull_domain_type>::type point_type;
    
    typedef viennagrid::result_of::element<hull_domain_type, viennagrid::vertex_tag>::type hull_vertex_type;
    typedef viennagrid::result_of::handle<hull_domain_type, viennagrid::vertex_tag>::type hull_vertex_handle_type;
    
    typedef viennagrid::result_of::element<hull_domain_type, viennagrid::triangle_tag>::type hull_triangle_type;
    
    
    hull_domain_type hull_domain;
    
    
    double s = 10.0;
    hull_vertex_handle_type vtx[8];
    
    vtx[0] = viennagrid::make_vertex( hull_domain, point_type(0, 0, 0) );
    vtx[1] = viennagrid::make_vertex( hull_domain, point_type(0, s, 0) );
    vtx[2] = viennagrid::make_vertex( hull_domain, point_type(s, 0, 0) );
    vtx[3] = viennagrid::make_vertex( hull_domain, point_type(s, s, 0) );
    
    vtx[4] = viennagrid::make_vertex( hull_domain, point_type(0, 0, s) );
    vtx[5] = viennagrid::make_vertex( hull_domain, point_type(0, s, s) );
    vtx[6] = viennagrid::make_vertex( hull_domain, point_type(s, 0, s) );
    vtx[7] = viennagrid::make_vertex( hull_domain, point_type(s, s, s) );
    
    
    viennagrid::make_triangle( hull_domain, vtx[0], vtx[1], vtx[2] );
    viennagrid::make_triangle( hull_domain, vtx[2], vtx[1], vtx[3] );
    
    viennagrid::make_triangle( hull_domain, vtx[4], vtx[6], vtx[5] );
    viennagrid::make_triangle( hull_domain, vtx[6], vtx[7], vtx[5] );
    
    viennagrid::make_triangle( hull_domain, vtx[0], vtx[2], vtx[4] );
    viennagrid::make_triangle( hull_domain, vtx[2], vtx[6], vtx[4] );
    
    viennagrid::make_triangle( hull_domain, vtx[1], vtx[5], vtx[3] );
    viennagrid::make_triangle( hull_domain, vtx[3], vtx[5], vtx[7] );
    
    viennagrid::make_triangle( hull_domain, vtx[0], vtx[4], vtx[1] );
    viennagrid::make_triangle( hull_domain, vtx[1], vtx[4], vtx[5] );
    
    viennagrid::make_triangle( hull_domain, vtx[2], vtx[3], vtx[6] );
    viennagrid::make_triangle( hull_domain, vtx[3], vtx[7], vtx[6] );
    
    
    viennamesh::result_of::settings<viennamesh::tetgen_tetrahedron_tag>::type settings;
    
//     settings.cell_size = 1;
    
    
    viennagrid::tetrahedral_3d_domain tet_domain;
    viennamesh::run_algo<viennamesh::tetgen_tetrahedron_tag>(hull_domain, tet_domain, settings);


    viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_domain> vtk_writer;
    vtk_writer(tet_domain, "cube_meshed_tetgen");
    
}
