#include <iostream>

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennagrid/io/tetgen_poly_reader.hpp"

#include "viennamesh/algorithm/cgal_delaunay_tetrahedron_mesher.hpp"
#include "viennamesh/algorithm/cgal_plc_mesher.hpp"



struct functor
{
  double operator() (double x, double y, double z) const
  {
    return x / 10.0 * 2.0 + 0.2;
  }
};




int main()
{
    viennagrid::plc_3d_mesh plc_mesh;

    viennagrid::io::tetgen_poly_reader reader;
    reader(plc_mesh, "../cube.poly");

    viennamesh::ConstParameterSet plc_settings;

    viennagrid::triangular_3d_mesh triangle_mesh;
    viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_mesh, triangle_mesh, plc_settings );


    viennagrid::io::vtk_writer<viennagrid::triangular_3d_mesh> vtk_hull_writer;
    vtk_hull_writer(triangle_mesh, "cube_hull");

    viennamesh::ConstParameterSet deltet_settings;

    functor f;

    deltet_settings.set("cell_size", 2.0);
    deltet_settings.set("cell_radius_edge_ratio", 1.5);


    viennagrid::tetrahedral_3d_mesh tet_mesh;
    viennamesh::run_algo<viennamesh::cgal_delaunay_tetrahedron_tag>(triangle_mesh, tet_mesh, deltet_settings);


    viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_mesh> vtk_volume_writer;
    vtk_volume_writer(tet_mesh, "cube_poly_meshed");
}
