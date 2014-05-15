#include "viennamesh/algorithm/vgmodeler/vgmodeler_adapt_hull.hpp"

#include "vgmodeler/vgmodeler.hpp"

namespace viennamesh
{
  namespace vgmodeler
  {
    adapt_hull::adapt_hull() :
      input_mesh(*this, parameter_information("mesh","mesh","The input mesh, triangular 3d with oriented hull segmentation supported")),
      cell_size(*this, parameter_information("cell_size","double","The desired maximum size of triangles, all triangles will be at most this size")),
      output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, same type of mesh as input mesh")) {}

    std::string adapt_hull::name() const { return "VGModeler hull adaption"; }
    std::string adapt_hull::id() const { return "vgmodeler_adapt_hull"; }

    bool adapt_hull::run_impl()
    {
      typedef viennagrid::triangular_3d_mesh                MeshType;
      typedef viennagrid::triangular_3d_segmentation        SegmentationType;
      typedef viennagrid::triangular_hull_3d_segmentation   OrientedSegmentationType;

      typedef viennagrid::segmented_mesh<MeshType, SegmentationType> SegmentedMeshType;
      typedef viennagrid::segmented_mesh<MeshType, OrientedSegmentationType> OrientedSegmentedMeshType;

      {
        viennamesh::result_of::const_parameter_handle<OrientedSegmentedMeshType>::type imp = input_mesh.get<OrientedSegmentedMeshType>();
        if (imp)
        {
          output_parameter_proxy<OrientedSegmentedMeshType> omp(output_mesh);

          ::vgmodeler::hull_adaptor adaptor;
          if (cell_size.valid())
            adaptor.maxsize() = cell_size();
          adaptor.process( imp().mesh, imp().segmentation, omp().mesh, omp().segmentation );

          return true;
        }
      }

//       {
//         viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type imp = input_mesh.get<SegmentedMeshType>();
//         if (imp)
//         {
//           output_parameter_proxy<OrientedSegmentedMeshType> omp(output_mesh);
//
//           viennamesh::algorithm_handle segmenting_algo( new hull_segmenting() );
//           segmenting_algo->set_input( "mesh", imp );
//           segmenting_algo->run();
//
//           viennamesh::result_of::const_parameter_handle<OrientedSegmentedMeshType>::type tmp =
//             dynamic_handle_cast<OrientedSegmentedMeshType>(segmenting_algo->get_output("mesh"));
//
//           ::vgmodeler::hull_adaptor adaptor;
//           if (cell_size.valid())
//             adaptor.maxsize() = cell_size();
//           adaptor.process( tmp().mesh, tmp().segmentation, omp().mesh, omp().segmentation );
//
//           return true;
//         }
//       }
//
//       {
//         viennamesh::result_of::const_parameter_handle<MeshType>::type imp = input_mesh.get<MeshType>();
//         if (imp)
//         {
//           output_parameter_proxy<OrientedSegmentedMeshType> omp(output_mesh);
//
//           viennamesh::algorithm_handle segmenting_algo( new hull_segmenting() );
//           segmenting_algo->set_input( "mesh", imp );
//           segmenting_algo->run();
//
//           viennamesh::result_of::const_parameter_handle<OrientedSegmentedMeshType>::type tmp =
//             dynamic_handle_cast<OrientedSegmentedMeshType>(segmenting_algo->get_output("mesh"));
//
//           ::vgmodeler::hull_adaptor adaptor;
//           if (cell_size.valid())
//             adaptor.maxsize() = cell_size();
//           adaptor.process( tmp().mesh, tmp().segmentation, omp().mesh, omp().segmentation );
//
//           return true;
//         }
//       }

      return false;
    }



  }
}
