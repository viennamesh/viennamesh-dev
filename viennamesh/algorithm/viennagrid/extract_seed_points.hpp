#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_EXTRACT_SEED_POINTS_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_EXTRACT_SEED_POINTS_HPP

#include "viennamesh/core/algorithm.hpp"

#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/mesh/neighbor_iteration.hpp"


namespace viennamesh
{
  namespace extract_seed_points
  {
    template<typename MeshT, typename UnvisitedCellMapT>
    void neighbor_mark( MeshT const & mesh, UnvisitedCellMapT & unvisitied_cells )
    {
      bool found = true;
      while (found)
      {
        found = false;

        for (typename UnvisitedCellMapT::iterator ucit = unvisitied_cells.begin(); ucit != unvisitied_cells.end(); )
        {
          typedef typename viennagrid::result_of::cell_tag<MeshT>::type CellTagType;
          typedef typename viennagrid::result_of::facet_tag<MeshT>::type FacetTagType;
          typedef typename viennagrid::result_of::const_neighbor_range<MeshT, CellTagType, FacetTagType>::type NeighborRangeType;
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

    template<typename MeshSegmentT, typename SeedPointContainerT>
    void extract_seed_points( MeshSegmentT const & mesh, SeedPointContainerT & seed_points, int segment_id )
    {
      typedef typename viennagrid::result_of::cell_id<MeshSegmentT>::type CellIDType;
      typedef typename viennagrid::result_of::const_cell_handle<MeshSegmentT>::type ConstCellHandleType;

      typedef typename viennagrid::result_of::const_cell_range<MeshSegmentT>::type CellRangeType;
      typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellIteratorType;

      CellRangeType cells(mesh);

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

            seed_points.push_back( std::make_pair(viennagrid::centroid(*cit), segment_id) );
            unvisited_cells.erase( ucit );

            neighbor_mark( mesh, unvisited_cells );
          }
        }
      }
    }

    template<typename MeshSegmentT, typename SegmentationT, typename SeedPointContainerT>
    void extract_seed_points( MeshSegmentT const & mesh, SegmentationT const & segmentation, SeedPointContainerT & seed_points )
    {
      if (segmentation.empty())
        extract_seed_points( mesh, seed_points, 0);
      else
        for (typename SegmentationT::const_iterator sit = segmentation.begin(); sit != segmentation.end(); ++sit)
          extract_seed_points( *sit, seed_points, sit->id() );
    }





    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "ViennaGrid Seed Point Extractor"; }

      template<typename MeshT, typename SegmentationT>
      bool generic_run()
      {
        typedef typename viennagrid::result_of::point<MeshT>::type PointType;
        typedef typename viennamesh::result_of::seed_point_container<PointType>::type PointContainerType;
        output_parameter_proxy<PointContainerType> seed_points = output_proxy<PointContainerType>("default");

        typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;
        typedef typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type SegmentedMeshHandleType;

        SegmentedMeshHandleType segmented_meshed_geometry = get_input<SegmentedMeshType>( "default" );
        if (segmented_meshed_geometry)
        {
          extract_seed_points( segmented_meshed_geometry().mesh, segmented_meshed_geometry().segmentation, seed_points() );
          return true;
        }
        else
        {
          typedef typename viennamesh::result_of::const_parameter_handle<MeshT>::type MeshHandleType;
          MeshHandleType meshed_geometry = get_input<MeshT>( "default" );
          if (!meshed_geometry)
            return false;

          extract_seed_points( meshed_geometry(), seed_points(), 0 );
          return true;
        }

        return false;
      }

      bool run_impl()
      {
        if (generic_run<viennagrid::line_1d_mesh, viennagrid::line_1d_segmentation>())
          return true;

        if (generic_run<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>())
          return true;

        if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>())
          return true;


        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }

    private:
    };

  }
}

#endif
