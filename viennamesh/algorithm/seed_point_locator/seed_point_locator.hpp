#ifndef VIENNAMESH_ALGORITHM_SEED_POINT_LOCATOR_SEED_POINT_LOCATOR_HPP
#define VIENNAMESH_ALGORITHM_SEED_POINT_LOCATOR_SEED_POINT_LOCATOR_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/triangle/triangle.hpp"
#include "viennamesh/algorithm/tetgen/tetgen.hpp"

#include "viennagrid/algorithm/centroid.hpp"

namespace viennamesh
{
  namespace seed_point_locator
  {

    class Algorithm : public BaseAlgorithm
    {
    public:

      string name() const { return "Seed Point Locator"; }

      bool run_impl()
      {
        ConstParameterHandle geometry = inputs.get("default");

        if (geometry->is_convertable_to<triangle::InputMesh>())
        {
          AlgorithmHandle mesher = AlgorithmHandle( new viennamesh::triangle::Algorithm() );

          mesher->set_input( "default", geometry );

          if (!mesher->run())
            return false;

          typedef viennagrid::triangular_2d_mesh ViennaGridMeshType;
          typedef viennamesh::result_of::parameter_handle<ViennaGridMeshType>::type MeshHandleType;

          MeshHandleType meshed_geometry = mesher->get_output<ViennaGridMeshType>( "default" );
          if (!meshed_geometry)
            return false;

          typedef viennagrid::result_of::point<ViennaGridMeshType>::type PointType;
          typedef viennagrid::result_of::cell_range<ViennaGridMeshType>::type CellRangeType;

          CellRangeType cells(meshed_geometry->value);
          if (!cells.empty())
            outputs.set( "default", viennagrid::centroid(cells[0]) );

          return true;
        }

        if (geometry->is_convertable_to<tetgen::InputMesh>())
        {
          AlgorithmHandle mesher = AlgorithmHandle( new viennamesh::tetgen::Algorithm() );

          mesher->set_input( "default", geometry );

          if (!mesher->run())
            return false;

          typedef viennagrid::tetrahedral_3d_mesh ViennaGridMeshType;
          typedef viennamesh::result_of::parameter_handle<ViennaGridMeshType>::type MeshHandleType;

          MeshHandleType meshed_geometry = mesher->get_output<ViennaGridMeshType>( "default" );
          if (!meshed_geometry)
            return false;

          typedef viennagrid::result_of::point<ViennaGridMeshType>::type PointType;
          typedef viennagrid::result_of::cell_range<ViennaGridMeshType>::type CellRangeType;

          CellRangeType cells(meshed_geometry->value);
          if (!cells.empty())
            outputs.set( "default", viennagrid::centroid(cells[0]) );
          return true;
        }

        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }
    };

  }
}



#endif
