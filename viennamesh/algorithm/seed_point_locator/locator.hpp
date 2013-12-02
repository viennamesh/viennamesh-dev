#ifndef VIENNAMESH_ALGORITHM_SEED_POINT_LOCATOR_SEED_POINT_LOCATOR_HPP
#define VIENNAMESH_ALGORITHM_SEED_POINT_LOCATOR_SEED_POINT_LOCATOR_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/mesher1d.hpp"
#include "viennamesh/algorithm/triangle.hpp"
#include "viennamesh/algorithm/tetgen.hpp"

#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/mesh/neighbor_iteration.hpp"

namespace viennamesh
{
  namespace seed_point_locator
  {

    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "Seed Point Locator"; }









      template<typename ViennaGridMeshT, typename ViennaGridSegmentationT, typename AlgorithmT>
      bool generic_run( const_parameter_handle const & geometry )
      {
        typedef typename viennagrid::result_of::point<ViennaGridMeshT>::type PointType;
        typedef typename viennamesh::result_of::seed_point_container<PointType>::type PointContainerType;
        output_parameter_proxy<PointContainerType> seed_points = output_proxy<PointContainerType>("default");

        algorithm_handle mesher( new AlgorithmT() );
        mesher->set_input( "default", geometry );
        const_parameter_handle hole_points = get_input( "hole_points" );
        if (hole_points)
        {
          info(5) << "Found hole points, passing to mesher" << std::endl;
          mesher->set_input( "hole_points", hole_points );
        }

        if (!mesher->run())
          return false;

        typedef typename viennamesh::result_of::parameter_handle<ViennaGridMeshT>::type MeshHandleType;
        typedef viennagrid::segmented_mesh<ViennaGridMeshT, ViennaGridSegmentationT> ViennaGridSegmentedMeshType;
        typedef typename viennamesh::result_of::parameter_handle<ViennaGridSegmentedMeshType>::type SegmentedMeshHandleType;


        SegmentedMeshHandleType segmented_meshed_geometry = mesher->get_output<ViennaGridSegmentedMeshType>( "default" );
        if (segmented_meshed_geometry)
          extract_seed_points( segmented_meshed_geometry().mesh, segmented_meshed_geometry().segmentation, seed_points() );
        else
        {
          MeshHandleType meshed_geometry = mesher->get_output<ViennaGridMeshT>( "default" );
          if (!meshed_geometry)
            return false;

          extract_seed_points( meshed_geometry(), seed_points(), 0 );
        }

        if (seed_points().empty())
        {
          error(1) << "No cells found in mesh -> no seed points detected" << std::endl;
          return false;
        }

        return true;
      }


      bool run_impl()
      {
        const_parameter_handle geometry = get_input("default");

        if (geometry->is_convertable_to<viennagrid::brep_1d_mesh>())
        {
          info(5) << "Found 1D boundary representation, using 1d mesher" << std::endl;
          return generic_run<viennagrid::line_1d_mesh, viennagrid::line_1d_segmentation, mesher1d::algorithm>(geometry);
        }

        if (geometry->is_convertable_to<triangle::input_mesh>())
        {
          info(5) << "Found 2D boundary representation, using triangle mesher" << std::endl;
          return generic_run<viennagrid::triangular_2d_mesh, viennagrid::line_2d_segmentation, triangle::algorithm>(geometry);
        }

        if (geometry->is_convertable_to<tetgen::input_mesh>())
        {
          info(5) << "Found 3D boundary representation, using tetgen mesher" << std::endl;
          return generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::line_3d_segmentation, tetgen::algorithm>(geometry);
        }

        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }
    };

  }
}



#endif
