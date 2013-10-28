#include <iostream>

// using std::string;
// #include "external/netgen-5.0.0/libsrc/general/ngexception.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennagrid/io/tetgen_poly_reader.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/neighbor_iteration.hpp"

#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/centroid.hpp"

#include "viennamesh/algorithm/cgal_plc_mesher.hpp"
#include "viennamesh/algorithm/vgmodeler_hull_adaption.hpp"
#include "viennamesh/algorithm/netgen_tetrahedron_mesher.hpp"
#include "viennamesh/algorithm/extract_hull.hpp"
#include "viennamesh/algorithm/extract_plc.hpp"

#include "viennamesh/doctor/triangular_hull.hpp"

#include "viennagrid/algorithm/seed_point_segmenting.hpp"









int main()
{

    typedef viennagrid::tetrahedral_3d_mesh volume_mesh_type;
    typedef viennagrid::tetrahedral_3d_segmentation volume_segmentation_type;
    typedef viennagrid::result_of::point<volume_mesh_type>::type volume_point_type;
//     typedef viennagrid::tetrahedral_3d_view volume_view_type;
//     typedef viennagrid::tetrahedral_3d_cell volume_cell_type;
//     typedef viennagrid::point_type_3d volume_point_type;

    ///////////////////////////////////////////
    // Init and Mesh loading
    ///////////////////////////////////////////

    volume_mesh_type tet_mesh;
    volume_segmentation_type tet_segmentation(tet_mesh);
//     std::deque<volume_view_type> tet_segments;

    viennagrid::io::netgen_reader reader;
    reader(tet_mesh, tet_segmentation, "../../examples/data/half-trigate.mesh");

    viennagrid::triangular_3d_mesh hull_mesh;
    viennagrid::triangular_3d_segmentation hull_segmentation(hull_mesh);
//     std::deque<viennagrid::triangular_3d_view> hull_segments;

    ///////////////////////////////////////////
    // Extract Hull
    ///////////////////////////////////////////

    viennamesh::extract_hull<viennagrid::triangle_tag>( tet_mesh, tet_segmentation, hull_mesh, hull_segmentation );

    {
        viennagrid::io::vtk_writer<viennagrid::triangular_3d_mesh> vtk_writer;
        vtk_writer(hull_mesh, hull_segmentation, "extracted_hull");
    }


    std::deque< std::pair<int, volume_point_type> > segment_seed_points;
    viennamesh::extract_seed_points( tet_segmentation, segment_seed_points );


//     ///////////////////////////////////////////
//     // Extract PLC
//     ///////////////////////////////////////////

    viennagrid::plc_3d_mesh plc_mesh;
    viennamesh::extract_plcs(hull_mesh, hull_segmentation, plc_mesh);

//     {
//         viennagrid::io::vtk_writer<viennagrid::plc_3d_mesh, viennagrid::line_tag> vtk_writer;
//         vtk_writer(plc_mesh, "all_extracted_lines.vtu");
//     }
//
//
//     ///////////////////////////////////////////
//     // PLC -> Hull
//     ///////////////////////////////////////////

    viennagrid::triangular_3d_mesh triangulated_plc_mesh;
    viennamesh::result_of::settings<viennamesh::cgal_plc_3d_mesher_tag>::type plc_settings(0.0, 0.0);

    viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_mesh, triangulated_plc_mesh, plc_settings );


    viennagrid::triangular_hull_3d_segmentation triangulated_plc_segmentation(triangulated_plc_mesh);
    viennagrid::mark_face_segments( triangulated_plc_mesh, triangulated_plc_segmentation, segment_seed_points.begin(), segment_seed_points.end() );



//     std::deque<viennagrid::triangular_3d_view> triangulated_plc_segments;
//     split_in_views( triangulated_plc_mesh, triangulated_plc_segmentation, triangulated_plc_segments);

    {
        viennagrid::io::vtk_writer<viennagrid::triangular_3d_mesh> vtk_writer;
        vtk_writer(triangulated_plc_mesh, "triangulated_plc_mesh");
    }

//     check_hull_topology( triangulated_plc_segments[0], triangulated_plc_segmentation, viennagrid::segment_id_t<>(0) );



//     ///////////////////////////////////////////
//     // Hull Adaption
//     ///////////////////////////////////////////
//
    viennagrid::triangular_3d_mesh adapted_hull_mesh;
    viennagrid::triangular_hull_3d_segmentation adapted_hull_segmentation(adapted_hull_mesh);
    viennamesh::result_of::settings<viennamesh::vgmodeler_hull_adaption_tag>::type vgm_settings;

//     vgm_settings.cell_size = 10.0;

    viennamesh::run_algo< viennamesh::vgmodeler_hull_adaption_tag >( triangulated_plc_mesh, triangulated_plc_segmentation,
                                                                     adapted_hull_mesh, adapted_hull_segmentation,
                                                                     vgm_settings );


//     {
//         viennagrid::io::vtk_writer<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_cell> vtk_writer;
//         vtk_writer(adapted_hull_mesh, "netgen_adapt_hull.vtu");
//     }


//     std::deque<viennagrid::triangular_3d_view> netgen_adapt_hull_segments;
//     split_in_views( adapted_hull_mesh, adapted_hull_segmentation, netgen_adapt_hull_segments );

//     check_hull_topology( netgen_adapt_hull_segments[0], adapted_hull_segmentation, viennagrid::segment_id_t<>(0) );

//
//     ///////////////////////////////////////////
//     // Volume meshing
//     ///////////////////////////////////////////

    viennagrid::tetrahedral_3d_mesh tetrahedron_mesh;
    viennagrid::tetrahedral_3d_segmentation tetrahedron_segmentation(tetrahedron_mesh);
    viennamesh::result_of::settings<viennamesh::netgen_tetrahedron_tag>::type netgen_settings;

//     netgen_settings.cell_size = 10.0;

    viennamesh::run_algo< viennamesh::netgen_tetrahedron_tag >(
                                                                adapted_hull_mesh, adapted_hull_segmentation,
                                                                tetrahedron_mesh, tetrahedron_segmentation,
                                                                netgen_settings );



//     {
//         viennagrid::result_of::cell_range<viennagrid::tetrahedral_3d_mesh>::type range = viennagrid::elements(tetrahedron_mesh);
//         for (viennagrid::result_of::cell_range<viennagrid::tetrahedral_3d_mesh>::type::iterator it = range.begin(); it != range.end(); ++it)
//             viennadata::access<std::string, double>("segment_id")(*it) = tetrahedron_segmentation.segment_info(*it).segment_id;
//     }



//     std::deque< viennagrid::tetrahedral_3d_view > tetrahedron_segments;
//     viennagrid::split_in_views( tetrahedron_mesh, tetrahedron_segmentation, tetrahedron_segments );


    {
        viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_mesh> vtk_writer;
//         viennagrid::io::add_scalar_data_on_cells<std::string, double>(vtk_writer, "segment_id", "segment_id");
        vtk_writer(tetrahedron_mesh, tetrahedron_segmentation, "half-trigate");
    }



}
