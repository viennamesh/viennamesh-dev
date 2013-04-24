#include <iostream>

#include "viennagrid/domain/config.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennagrid/io/poly_reader.hpp"

#include "viennamesh/algorithm/cgal_delaunay_tetrahedron_mesher.hpp"
#include "viennamesh/algorithm/cgal_plc_mesher.hpp"



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
    viennagrid::config::plc_3d_domain plc_domain;
    
    viennagrid::io::poly_reader reader;
    reader(plc_domain, "../../examples/data/cube.poly");
    
    
    viennamesh::result_of::settings<viennamesh::cgal_plc_3d_mesher_tag>::type plc_settings;//(0.3, 0.0);
    
    viennagrid::config::triangular_3d_domain triangle_domain;
    viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_domain, triangle_domain, plc_settings );
    
    
    viennagrid::io::vtk_writer<viennagrid::config::triangular_3d_domain, viennagrid::config::triangular_3d_cell> vtk_hull_writer;
    vtk_hull_writer(triangle_domain, "cube_hull.vtu");
    
    viennamesh::result_of::settings<viennamesh::cgal_delaunay_tetrahedron_tag>::type deltet_settings;
    
    functor f;
    
    deltet_settings.cell_size = 2.0;
    deltet_settings.cell_radius_edge_ratio = 1.5;
    
    
    viennagrid::config::tetrahedral_3d_domain tet_domain;
    viennamesh::run_algo<viennamesh::cgal_delaunay_tetrahedron_tag>(triangle_domain, tet_domain, deltet_settings);


    viennagrid::io::vtk_writer<viennagrid::config::tetrahedral_3d_domain, viennagrid::config::tetrahedral_3d_cell> vtk_volume_writer;
    vtk_volume_writer(tet_domain, "cube_poly_meshed.vtu");    
}
