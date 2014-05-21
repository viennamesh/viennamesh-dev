#ifndef VIENNAMESH_ALGORITHM_CGAL_TETRAHEDRON_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_CGAL_TETRAHEDRON_GENERATOR_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/cgal/mesh.hpp"



namespace viennamesh
{
  namespace cgal
  {
    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "CGAL tetrahedron mesher"; }


      bool run_impl()
      {
        typedef cgal::input_mesh InputMeshType;
        typedef cgal::output_mesh OutputMeshType;

        viennamesh::result_of::const_parameter_handle<InputMeshType>::type input_mesh = get_required_input<InputMeshType>("default");
        output_parameter_proxy<OutputMeshType> output_mesh = output_proxy<OutputMeshType>("default");

        OutputMeshType::Mesh_mesh mesh_mesh( input_mesh().polyhedron );
        InputMeshType::feature_lines_type feature_lines( input_mesh().feature_lines );
        mesh_mesh.add_features( feature_lines.begin(), feature_lines.end() );


        double cell_size = 0.0;
        double max_radius_edge_ratio = 0.0;
        double min_facet_angle = 0.0;

        copy_input( "cell_size", cell_size );
        copy_input( "max_radius_edge_ratio", max_radius_edge_ratio );
        copy_input( "min_facet_angle", min_facet_angle );

        min_facet_angle *= 180.0/M_PI;

        info(1) << "Using cell size: " << cell_size << std::endl;
        info(1) << "Using radius edge ratio size: " << max_radius_edge_ratio << std::endl;
        info(1) << "Using facet angle: " << min_facet_angle << std::endl;


        typedef CGAL::Mesh_criteria_3<OutputMeshType::Tr> MeshCriteriaType;
        MeshCriteriaType criteria(
          CGAL::parameters::edge_size = cell_size,
          CGAL::parameters::facet_size = cell_size,
          CGAL::parameters::facet_distance = cell_size/10.0,
          CGAL::parameters::facet_angle = min_facet_angle,
          CGAL::parameters::facet_topology = CGAL::FACET_VERTICES_ON_SAME_SURFACE_PATCH,
          CGAL::parameters::cell_radius_edge_ratio = max_radius_edge_ratio,
          CGAL::parameters::cell_size = cell_size);

        output_mesh().tetrahedron_triangulation = CGAL::make_mesh_3<OutputMeshType::C3t3>(mesh_mesh, criteria);

        return true;
      }
    };
  }
}


#endif
