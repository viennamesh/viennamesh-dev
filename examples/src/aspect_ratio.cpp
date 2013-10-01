#include <iostream>


#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennagrid/io/poly_reader.hpp"

#include "viennamesh/algorithm/cgal_plc_mesher.hpp"
#include "viennagrid/mesh/neighbour_iteration.hpp"
#include "viennagrid/algorithm/geometry.hpp"

#include "viennagrid/algorithm/seed_point_segmenting.hpp"



#include "viennamesh/algorithm/vgmodeler_hull_adaption.hpp"
#include "viennamesh/algorithm/netgen_tetrahedron_mesher.hpp"


#include "viennamesh/statistics/element_metrics.hpp"





int main()
{
    viennagrid::plc_3d_mesh plc_mesh;
    
    
    viennagrid::io::poly_reader reader;
    reader(plc_mesh, "../../examples/data/big_and_small_cube.poly");
    
    
    viennagrid::triangular_3d_mesh triangulated_plc_mesh;
    viennamesh::result_of::settings<viennamesh::cgal_plc_3d_mesher_tag>::type plc_settings(0.0, 0.0);
    
    viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_mesh, triangulated_plc_mesh, plc_settings );
    
    {
        viennagrid::io::vtk_writer<viennagrid::triangular_3d_mesh> vtk_writer;
        vtk_writer(triangulated_plc_mesh, "meshed_plc_hull.vtu");
    }

    
    
    typedef viennagrid::result_of::point<viennagrid::triangular_3d_mesh>::type point_type;
    
    std::cout << "Num triangles after PLC meshing: " << viennagrid::elements<viennagrid::triangle_tag>( triangulated_plc_mesh ).size() << std::endl;
    
    
//     typedef viennagrid::result_of::segmentation<viennagrid::config::triangular_3d_mesh, viennagrid::triangle_tag>::type segmentation_type;
    viennagrid::triangular_hull_3d_segmentation triangulated_plc_segmentation(triangulated_plc_mesh);
    
    
    std::vector< std::pair< int, point_type > > seed_points;
    seed_points.push_back( std::make_pair(0, point_type(0.0, 0.0, 0.0)) );
    seed_points.push_back( std::make_pair(1, point_type(0.0, 0.0, 20.0)) );
    
    viennagrid::mark_face_segments( triangulated_plc_mesh, triangulated_plc_segmentation, seed_points.begin(), seed_points.end() );

    
    viennagrid::triangular_3d_mesh oriented_adapted_hull_mesh;
    viennagrid::triangular_hull_3d_segmentation oriented_adapted_hull_segmentation(oriented_adapted_hull_mesh);
    viennamesh::result_of::settings<viennamesh::vgmodeler_hull_adaption_tag>::type vgm_settings;
    
    vgm_settings.cell_size = 3.0;
    
    viennamesh::run_algo< viennamesh::vgmodeler_hull_adaption_tag >( triangulated_plc_mesh, triangulated_plc_segmentation,
                                                                     oriented_adapted_hull_mesh, oriented_adapted_hull_segmentation,
                                                                     vgm_settings );
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::triangular_3d_mesh> vtk_writer;
        vtk_writer(oriented_adapted_hull_mesh, "netgen_adapt_hull");
    }


    
    viennagrid::tetrahedral_3d_mesh tetrahedron_mesh;
    viennagrid::tetrahedral_3d_segmentation tetrahedron_segmentation(tetrahedron_mesh);
    viennamesh::result_of::settings<viennamesh::netgen_tetrahedron_tag>::type netgen_settings;
    
    netgen_settings.cell_size = 3.0;
    
    viennamesh::run_algo< viennamesh::netgen_tetrahedron_tag >( oriented_adapted_hull_mesh, oriented_adapted_hull_segmentation,
                                                                tetrahedron_mesh, tetrahedron_segmentation,
                                                                netgen_settings );
    


    std::deque<double> aspect_ratio;
    viennagrid::result_of::accessor< std::deque<double>, viennagrid::tetrahedral_3d_cell>::type aspect_ratio_accessor(aspect_ratio);


    typedef viennagrid::result_of::element_range<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedron_tag>::type tetrahedron_range_type;
    typedef viennagrid::result_of::iterator<tetrahedron_range_type>::type tetrahedron_range_iterator;

    tetrahedron_range_type tetrahedrons = viennagrid::elements( tetrahedron_mesh );
    for (tetrahedron_range_iterator tetit = tetrahedrons.begin(); tetit != tetrahedrons.end(); ++tetit)
        aspect_ratio_accessor(*tetit) = viennamesh::aspect_ratio( *tetit );
    
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_mesh> vtk_writer;
        viennagrid::io::add_scalar_data_on_cells(vtk_writer, aspect_ratio_accessor, "aspect_ratio");
        vtk_writer(tetrahedron_mesh, tetrahedron_segmentation, "netgen_volume");
    }

    
    
}
