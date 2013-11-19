#ifndef VIENNAMESH_ALGORITHM_SEED_POINT_LOCATOR_SEED_POINT_LOCATOR_HPP
#define VIENNAMESH_ALGORITHM_SEED_POINT_LOCATOR_SEED_POINT_LOCATOR_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/triangle.hpp"
#include "viennamesh/algorithm/tetgen.hpp"

#include "viennagrid/algorithm/centroid.hpp"

namespace viennamesh
{
  namespace seed_point_locator
  {

    class Algorithm : public BaseAlgorithm
    {
    public:

      string name() const { return "Seed Point Locator"; }



      template<typename ViennaGridMeshT, typename AlgorithmT>
      bool generic_run( ConstParameterHandle const & geometry )
      {
        typedef typename viennagrid::result_of::point<ViennaGridMeshT>::type PointType;
        OutputParameterProxy<PointType> seed_point = output_proxy<PointType>("default");

        AlgorithmHandle mesher( new AlgorithmT() );
        mesher->set_input( "default", geometry );

        if (!mesher->run())
          return false;

        typedef typename viennamesh::result_of::parameter_handle<ViennaGridMeshT>::type MeshHandleType;

        MeshHandleType meshed_geometry = mesher->get_output<ViennaGridMeshT>( "default" );
        if (!meshed_geometry)
          return false;


        typedef typename viennagrid::result_of::cell_range<ViennaGridMeshT>::type CellRangeType;

        CellRangeType cells(meshed_geometry->get());
        if (!cells.empty())
          seed_point() = viennagrid::centroid(cells[0]);
        else
        {
          error(1) << "No cells found in mesh -> no seed points detected" << std::endl;
          return false;
        }

        return true;
      }


      bool run_impl()
      {
        ConstParameterHandle geometry = get_input("default");

        if (geometry->is_convertable_to<triangle::InputMesh>())
          return generic_run<viennagrid::triangular_2d_mesh, triangle::Algorithm>(geometry);

        if (geometry->is_convertable_to<tetgen::InputMesh>())
          return generic_run<viennagrid::tetrahedral_3d_mesh, tetgen::Algorithm>(geometry);


//         if (generic_run<viennagrid::triangular_2d_mesh, triangle::InputMesh, triangle::Algorithm>(geometry))
//           return true;
//
//         if (generic_run<viennagrid::tetrahedral_3d_mesh, tetgen::InputMesh, tetgen::Algorithm>(geometry))
//           return true;

//         return false;


//         if (geometry->is_convertable_to<triangle::InputMesh>())
//         {
//           typedef viennagrid::triangular_2d_mesh ViennaGridMeshType;
//           typedef viennagrid::result_of::point<ViennaGridMeshType>::type PointType;
//
//
//           OutputParameterProxy<PointType> seed_point = output_proxy<PointType>("default");
//
//           AlgorithmHandle mesher = AlgorithmHandle( new viennamesh::triangle::Algorithm() );
//
//           mesher->set_input( "default", geometry );
//
//           if (!mesher->run())
//             return false;
//
//           typedef viennamesh::result_of::parameter_handle<ViennaGridMeshType>::type MeshHandleType;
//
//           MeshHandleType meshed_geometry = mesher->get_output<ViennaGridMeshType>( "default" );
//           if (!meshed_geometry)
//             return false;
//
//
//           typedef viennagrid::result_of::cell_range<ViennaGridMeshType>::type CellRangeType;
//
//           CellRangeType cells(meshed_geometry->get());
//           if (!cells.empty())
//             seed_point() = viennagrid::centroid(cells[0]);
//           else
//           {
//             error(1) << "No cells found in mesh -> no seed points detected" << std::endl;
//             return false;
//           }
//
//           return true;
//         }
//
//         if (geometry->is_convertable_to<tetgen::InputMesh>())
//         {
//           typedef viennagrid::result_of::point<ViennaGridMeshType>::type PointType;
//           OutputParameterProxy<PointType> seed_point = output_proxy<PointType>("default");
//
//           AlgorithmHandle mesher = AlgorithmHandle( new viennamesh::tetgen::Algorithm() );
//
//           mesher->set_input( "default", geometry );
//
//           if (!mesher->run())
//             return false;
//
//           typedef viennagrid::tetrahedral_3d_mesh ViennaGridMeshType;
//           typedef viennamesh::result_of::parameter_handle<ViennaGridMeshType>::type MeshHandleType;
//
//           MeshHandleType meshed_geometry = mesher->get_output<ViennaGridMeshType>( "default" );
//           if (!meshed_geometry)
//             return false;
//
//           typedef viennagrid::result_of::cell_range<ViennaGridMeshType>::type CellRangeType;
//
//           CellRangeType cells(meshed_geometry->get());
//           if (!cells.empty())
//             seed_point() = viennagrid::centroid(cells[0]);
//           else
//           {
//             error(1) << "No cells found in mesh -> no seed points detected" << std::endl;
//             return false;
//           }
//
//           return true;
//         }

        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }
    };

  }
}



#endif
