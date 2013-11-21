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



      template<typename ViennaGridMeshT, typename UnvisitedCellMapT>
      void neighbor_mark( ViennaGridMeshT const & mesh, UnvisitedCellMapT & unvisitied_cells )
      {
        bool found = true;
        while (found)
        {
          found = false;

          for (typename UnvisitedCellMapT::iterator ucit = unvisitied_cells.begin(); ucit != unvisitied_cells.end(); )
          {
            typedef typename viennagrid::result_of::cell_tag<ViennaGridMeshT>::type CellTagType;
            typedef typename viennagrid::result_of::facet_tag<ViennaGridMeshT>::type FacetTagType;
            typedef typename viennagrid::result_of::const_neighbor_range<ViennaGridMeshT, CellTagType, FacetTagType>::type NeighborRangeType;
            typedef typename viennagrid::result_of::iterator<NeighborRangeType>::type NeighborIteratorType;

            NeighborRangeType neighbors( mesh, ucit->second );
            NeighborIteratorType ncit = neighbors.begin();
            for (; ncit != neighbors.end(); ++ncit)
            {
              typename UnvisitedCellMapT::iterator ucit2 = unvisitied_cells.find( ncit->id() );
              if (ucit2 == unvisitied_cells.end())
                break;
            }

            if (ncit != neighbors.end())
            {
              found = true;
              unvisitied_cells.erase( ucit++ );
            }
            else
              ++ucit;
          }
        }
      }




      template<typename ViennaGridMeshT, typename AlgorithmT>
      bool generic_run( const_parameter_handle const & geometry )
      {
        typedef typename viennagrid::result_of::point<ViennaGridMeshT>::type PointType;
        typedef typename viennamesh::result_of::point_container<PointType>::type PointContainerType;
        output_parameter_proxy<PointContainerType> seed_point = output_proxy<PointContainerType>("default");

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

        MeshHandleType meshed_geometry = mesher->get_output<ViennaGridMeshT>( "default" );
        if (!meshed_geometry)
          return false;


        typedef typename viennagrid::result_of::cell_id<ViennaGridMeshT>::type CellIDType;
        typedef typename viennagrid::result_of::const_cell_handle<ViennaGridMeshT>::type ConstCellHandleType;

        typedef typename viennagrid::result_of::cell_range<ViennaGridMeshT>::type CellRangeType;
        typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellIteratorType;

        CellRangeType cells(meshed_geometry->value());

        if (!cells.empty())
        {
          typedef std::map<CellIDType, ConstCellHandleType> UnvisitedCellMapType;
          UnvisitedCellMapType unvisited_cells;

          for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
            unvisited_cells[ cit->id() ] = cit.handle();

          while (!unvisited_cells.empty())
          {
            for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
            {
              typename UnvisitedCellMapType::iterator ucit = unvisited_cells.find( cit->id() );
              if (ucit == unvisited_cells.end())
                continue;

              seed_point().push_back( viennagrid::centroid(*cit) );
              unvisited_cells.erase( ucit );

              neighbor_mark( meshed_geometry->value(), unvisited_cells );
            }
          }
        }
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

        if (geometry->is_convertable_to<viennagrid::brep_1d_mesh>())
        {
          info(5) << "Found 1D boundary representation, using 1d mesher" << std::endl;
          return generic_run<viennagrid::line_1d_mesh, mesher1d::algorithm>(geometry);
        }

        if (geometry->is_convertable_to<triangle::input_mesh>())
        {
          info(5) << "Found 2D boundary representation, using triangle mesher" << std::endl;
          return generic_run<viennagrid::triangular_2d_mesh, triangle::algorithm>(geometry);
        }

        if (geometry->is_convertable_to<tetgen::input_mesh>())
        {
          info(5) << "Found 3D boundary representation, using tetgen mesher" << std::endl;
          return generic_run<viennagrid::tetrahedral_3d_mesh, tetgen::algorithm>(geometry);
        }

        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }
    };

  }
}



#endif
