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

    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "Seed Point Locator"; }



      template<typename ViennaGridMeshT, typename AlgorithmT>
      bool generic_run( const_parameter_handle const & geometry )
      {
        typedef typename viennagrid::result_of::point<ViennaGridMeshT>::type PointType;
        output_parameter_proxy<PointType> seed_point = output_proxy<PointType>("default");

        algorithm_handle mesher( new AlgorithmT() );
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
        const_parameter_handle geometry = get_input("default");

        if (geometry->is_convertable_to<triangle::input_mesh>())
          return generic_run<viennagrid::triangular_2d_mesh, triangle::algorithm>(geometry);

        if (geometry->is_convertable_to<tetgen::input_mesh>())
          return generic_run<viennagrid::tetrahedral_3d_mesh, tetgen::algorithm>(geometry);

        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }
    };

  }
}



#endif
