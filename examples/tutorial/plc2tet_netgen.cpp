#include <iostream>


#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennagrid/io/tetgen_poly_reader.hpp"

#include "viennamesh/algorithm/cgal_plc_mesher.hpp"
#include "viennagrid/mesh/neighbor_iteration.hpp"
#include "viennagrid/algorithm/geometry.hpp"

#include "viennagrid/algorithm/seed_point_segmenting.hpp"


#include "viennamesh/algorithm/vgmodeler_hull_adaption.hpp"
#include "viennamesh/algorithm/netgen_tetrahedron_mesher.hpp"








int main()
{
    viennagrid::plc_3d_mesh plc_mesh;


    viennagrid::io::tetgen_poly_reader reader;
    reader(plc_mesh, "../data/big_and_small_cube.poly");



    viennagrid::triangular_3d_mesh triangulated_plc_mesh;
    viennamesh::ParameterSet plc_settings;

    viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_mesh, triangulated_plc_mesh, plc_settings );

    {
        viennagrid::io::vtk_writer<viennagrid::triangular_3d_mesh> vtk_writer;
        vtk_writer(triangulated_plc_mesh, "meshed_plc_hull");
    }





    typedef viennagrid::result_of::point<viennagrid::triangular_3d_mesh>::type point_type;
    viennagrid::triangular_hull_3d_segmentation triangulated_plc_segmentation(triangulated_plc_mesh);

    std::vector< std::pair< int, point_type > > seed_points;
    seed_points.push_back( std::make_pair(0, point_type(0.0, 0.0, 0.0)) );
    seed_points.push_back( std::make_pair(1, point_type(0.0, 0.0, 20.0)) );

    viennagrid::mark_face_segments( triangulated_plc_mesh, triangulated_plc_segmentation, seed_points.begin(), seed_points.end() );




    viennagrid::triangular_3d_mesh oriented_adapted_hull_mesh;
    viennagrid::triangular_hull_3d_segmentation oriented_adapted_hull_segmentation(oriented_adapted_hull_mesh);

    viennamesh::ParameterSet vgm_settings;
    vgm_settings.set("cell_size", 3.0);

    viennamesh::run_algo< viennamesh::vgmodeler_hull_adaption_tag >( triangulated_plc_mesh, triangulated_plc_segmentation,
                                                                     oriented_adapted_hull_mesh, oriented_adapted_hull_segmentation,
                                                                     vgm_settings );


    {
        viennagrid::io::vtk_writer<viennagrid::triangular_3d_mesh> vtk_writer;
        vtk_writer(oriented_adapted_hull_mesh, "netgen_adapt_hull");
    }



    viennagrid::tetrahedral_3d_mesh tetrahedron_mesh;
    viennagrid::tetrahedral_3d_segmentation tetrahedron_segmentation(tetrahedron_mesh);

    viennamesh::ParameterSet netgen_settings;
    netgen_settings.set("cell_size", 3.0);

    viennamesh::run_algo< viennamesh::netgen_tetrahedron_tag >( oriented_adapted_hull_mesh, oriented_adapted_hull_segmentation,
                                                                tetrahedron_mesh, tetrahedron_segmentation,
                                                                netgen_settings );


    {
        viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_mesh> vtk_writer;
        vtk_writer(tetrahedron_mesh, tetrahedron_segmentation, "netgen_volume");
    }



}
