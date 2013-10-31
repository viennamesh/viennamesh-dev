#ifndef VIENNAMESH_ALGORITHM_NETGEN_TETRAHEDRON_MESHER_HPP
#define VIENNAMESH_ALGORITHM_NETGEN_TETRAHEDRON_MESHER_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/mesh/netgen_tetrahedron.hpp"

#include "viennamesh/utils/utils.hpp"


namespace viennamesh
{
  struct netgen_tetrahedron_tag {};

  namespace result_of
  {
    template<>
    struct works_in_place<netgen_tetrahedron_tag>
    {
      static const bool value = true;
    };

    template<>
    struct algorithm_info<netgen_tetrahedron_tag>
    {
      static const std::string name() { return "Netgen 5.0.0 Triangle Hull to Tetrahedron Mesher"; }
    };

    template<typename mesh_type>
    struct best_matching_native_input_mesh<netgen_tetrahedron_tag, mesh_type>
    {
      typedef netgen_tetrahedron_mesh type;
    };

    template<typename mesh_type>
    struct best_matching_native_output_mesh<netgen_tetrahedron_tag, mesh_type>
    {
      typedef netgen_tetrahedron_mesh type;
    };


    template<typename mesh_type>
    struct best_matching_native_input_segmentation<netgen_tetrahedron_tag, mesh_type>
    {
      typedef NoSegmentation type;
    };

    template<typename mesh_type>
    struct best_matching_native_output_segmentation<netgen_tetrahedron_tag, mesh_type>
    {
      typedef NoSegmentation type;
    };
  }


  template<>
  struct native_algorithm_impl<netgen_tetrahedron_tag>
  {
    typedef netgen_tetrahedron_tag algorithm_tag;

    template<typename native_mesh_type, typename native_segmentation_type>
    static algorithm_feedback run( native_mesh_type & native_mesh,
                      native_segmentation_type & native_segmentation,
                      ParameterSet const & parameters )
    {
      algorithm_feedback feedback( result_of::algorithm_info<algorithm_tag>::name() );
      nglib::Ng_Meshing_Parameters mesh_parameters;

//             mesh_parameters.optvolmeshenable = 1;
//             mesh_parameters.optsteps_3d = 3;

      parameters.copyScalar( "cell_size", mesh_parameters.maxh );
//       if ( !settings.cell_size.is_ignored() ) mesh_parameters.maxh = settings.cell_size();
//             mesh_parameters.optvolmeshenable = 0;
//             mesh_parameters.optsteps_3d = 1;
//
//             double h = 10.0;

      int segment_index = 0;
      for (typename native_mesh_type::netgen_mesh_container_type::const_iterator it = native_mesh.meshes.begin(); it != native_mesh.meshes.end(); ++it, ++segment_index)
      {
//                 nglib::Ng_Meshing_Parameters mesh_parameters;
//
//                 if (segment_index == 0)
//                 {
//                     mesh_parameters.maxh = 2.0;
//                     nglib::Ng_RestrictMeshSizeGlobal (it->second, 2.0);
//                 }

        nglib::Ng_Result result = nglib::Ng_GenerateVolumeMesh(it->second, &mesh_parameters);
      }

      feedback.set_success();
      return feedback;
    }

  };

}

#endif
