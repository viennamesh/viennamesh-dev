#include <iostream>

#include "viennamesh/algorithm/cgal_delaunay_tetrahedron_mesher.hpp"


#include "viennagrid/domain/config.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"






struct functor
{
    template<typename point_type>
    double operator() (point_type const & p) const
    {
        return p[0] / 10.0 * 2.0 + 0.2;
    }
};




int main()
{
    typedef viennagrid::config::triangular_3d_domain hull_domain_type;
    
    typedef viennagrid::result_of::point_type<hull_domain_type>::type point_type;
    
    typedef viennagrid::result_of::element<hull_domain_type, viennagrid::vertex_tag>::type hull_vertex_type;
    typedef viennagrid::result_of::element_hook<hull_domain_type, viennagrid::vertex_tag>::type hull_vertex_hook_type;
    
    typedef viennagrid::result_of::element<hull_domain_type, viennagrid::triangle_tag>::type hull_triangle_type;
    
    
    hull_domain_type hull_domain;
    
    
    double s = 10.0;
    hull_vertex_hook_type vtx[8];
    
    vtx[0] = viennagrid::create_element<hull_vertex_type>( hull_domain, point_type(0, 0, 0) );
    vtx[1] = viennagrid::create_element<hull_vertex_type>( hull_domain, point_type(0, s, 0) );
    vtx[2] = viennagrid::create_element<hull_vertex_type>( hull_domain, point_type(s, 0, 0) );
    vtx[3] = viennagrid::create_element<hull_vertex_type>( hull_domain, point_type(s, s, 0) );
    
    vtx[4] = viennagrid::create_element<hull_vertex_type>( hull_domain, point_type(0, 0, s) );
    vtx[5] = viennagrid::create_element<hull_vertex_type>( hull_domain, point_type(0, s, s) );
    vtx[6] = viennagrid::create_element<hull_vertex_type>( hull_domain, point_type(s, 0, s) );
    vtx[7] = viennagrid::create_element<hull_vertex_type>( hull_domain, point_type(s, s, s) );
    
    
    viennagrid::create_triangle( hull_domain, vtx[0], vtx[1], vtx[2] );
    viennagrid::create_triangle( hull_domain, vtx[2], vtx[1], vtx[3] );
    
    viennagrid::create_triangle( hull_domain, vtx[4], vtx[6], vtx[5] );
    viennagrid::create_triangle( hull_domain, vtx[6], vtx[7], vtx[5] );
    
    viennagrid::create_triangle( hull_domain, vtx[0], vtx[2], vtx[4] );
    viennagrid::create_triangle( hull_domain, vtx[2], vtx[6], vtx[4] );
    
    viennagrid::create_triangle( hull_domain, vtx[1], vtx[5], vtx[3] );
    viennagrid::create_triangle( hull_domain, vtx[3], vtx[5], vtx[7] );
    
    viennagrid::create_triangle( hull_domain, vtx[0], vtx[4], vtx[1] );
    viennagrid::create_triangle( hull_domain, vtx[1], vtx[4], vtx[5] );
    
    viennagrid::create_triangle( hull_domain, vtx[2], vtx[3], vtx[6] );
    viennagrid::create_triangle( hull_domain, vtx[3], vtx[7], vtx[6] );
    
    
    functor f;
    
    
    viennamesh::result_of::settings<viennamesh::cgal_delaunay_tetrahedron_tag>::type settings;
    
    settings.cell_size = f;
    settings.cell_radius_edge_ratio = 1.5;
    
    
    viennagrid::config::tetrahedral_3d_domain tet_domain;
    viennamesh::run_algo<viennamesh::cgal_delaunay_tetrahedron_tag>(hull_domain, tet_domain, settings);


    viennagrid::io::vtk_writer<viennagrid::config::tetrahedral_3d_domain, viennagrid::config::tetrahedral_3d_cell> vtk_writer;
    vtk_writer(tet_domain, "cube_meshed.vtu");
    
}
