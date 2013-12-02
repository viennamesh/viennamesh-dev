#ifndef VIENNAMESH_ALGORITHM_MESHER1D_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_MESHER1D_GENERATOR_HPP

#include "viennagrid/algorithm/norm.hpp"
#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/viennagrid/extract_seed_points.hpp"

namespace viennamesh
{
  namespace mesher1d
  {
    template<typename MeshT>
    class vertex_handle_point_sorter_1d
    {
    public:
      typedef typename viennagrid::result_of::const_vertex_handle<MeshT>::type ConstVertexHandleType;

      vertex_handle_point_sorter_1d(MeshT const & mesh_) : mesh(mesh_) {}

      bool operator()( ConstVertexHandleType const & v0, ConstVertexHandleType const & v1 ) const
      {
        return
          viennagrid::point(mesh, viennagrid::dereference_handle(mesh, v0) )[0] <
          viennagrid::point(mesh, viennagrid::dereference_handle(mesh, v1) )[0];
      }

    private:
      MeshT const & mesh;
    };

    template<typename MeshT>
    class point_sorter_1d
    {
    public:
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;

      bool operator()( PointType const & p0, PointType const & p1 ) const
      { return p0 [0] < p1[0]; }

      bool operator()( std::pair<PointType, int> const & sp0, std::pair<PointType, int> const & sp1 ) const
      { return sp0.first[0] < sp1.first[0]; }
    };



    template<typename GeometryT, typename OutputMeshT, typename OutputSegmentationT>
    bool generate_mesh( GeometryT const & geometry, OutputMeshT * mesh, OutputSegmentationT * segmentation,
                        double cell_size, bool use_different_segment_ids_for_unknown_segments, double min_geometry_point_distance,
                        seed_point_1d_container const & seed_points_, point_1d_container const & hole_points_ )
    {
      typedef typename viennagrid::result_of::point<GeometryT>::type PointType;

      typedef typename viennamesh::result_of::const_parameter_handle<GeometryT>::type GeometryHandleType;

//       typedef typename viennagrid::result_of::segment_handle<OutputSegmentationT>::type OutputSegmentHandleType;
//       typedef typename viennagrid::segmented_mesh<OutputMeshT, OutputSegmentationT> OutputSegmentedMesh;

      typedef typename viennagrid::result_of::const_vertex_handle<GeometryT>::type GeometryVertexHandleType;
      typedef typename viennagrid::result_of::vertex_handle<OutputMeshT>::type OutputVertexHandleType;

      typedef typename viennagrid::result_of::const_vertex_range<GeometryT>::type ConstVertexRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

      seed_point_1d_container seed_points(seed_points_);
      point_1d_container hole_points(hole_points_);

      // copy and sort vertices
      ConstVertexRangeType vertices( geometry );
      std::list<GeometryVertexHandleType> sorted_geometry_points;
      for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
        sorted_geometry_points.push_back( vit.handle() );
      sorted_geometry_points.sort( vertex_handle_point_sorter_1d<GeometryT>(geometry) );

      // remove points which would lead to too short lines
      {
        typename std::list<GeometryVertexHandleType>::iterator vhit0 = sorted_geometry_points.begin();
        typename std::list<GeometryVertexHandleType>::iterator vhit1 = vhit0; ++vhit1;

        while (vhit1 != sorted_geometry_points.end())
        {
          double length = std::abs(viennagrid::point(geometry, *vhit0)[0] - viennagrid::point(geometry, *vhit1)[0]);

          if (length < min_geometry_point_distance)
          {
            sorted_geometry_points.erase(vhit1++);
          }
          else
          {
            ++vhit0;
            ++vhit1;
          }
        }
      }

      // create points in output mesh
      std::vector<OutputVertexHandleType> sorted_points;
      for (typename std::list<GeometryVertexHandleType>::iterator vhit = sorted_geometry_points.begin(); vhit != sorted_geometry_points.end(); ++vhit)
      {
        OutputVertexHandleType vertex_handle = viennagrid::make_vertex(*mesh, viennagrid::point(geometry, *vhit) );
        sorted_points.push_back(vertex_handle);
      }


      // determine unused segment ID
      int default_segment_id = -1;
      for (seed_point_1d_container::const_iterator it = seed_points.begin(); it != seed_points.end(); ++it)
        default_segment_id = std::max(it->second, default_segment_id);
      ++default_segment_id;
      info(10) << "Default segment id: " << default_segment_id << std::endl;


      // sort seed points and hole points
      std::sort( seed_points.begin(), seed_points.end(), point_sorter_1d<GeometryT>() );
      std::sort( hole_points.begin(), hole_points.end(), point_sorter_1d<GeometryT>() );

      seed_point_1d_container::const_iterator spit = seed_points.begin();
      point_1d_container::const_iterator hpit = hole_points.begin();


      // iterate over all intervals in sorted point array
      for (unsigned int i = 1; i < sorted_points.size(); ++i)
      {
        // getting start and end point of the current interval
        double start = viennagrid::point( *mesh, sorted_points[i-1])[0];
        double end = viennagrid::point( *mesh, sorted_points[i])[0];


        // updating the hole point iterator to fit into the current interval
        while ( (hpit != hole_points.end()) && ((*hpit)[0] < start) )
          ++hpit;

        // if the hole point is inside the current interval -> skip current interval
        if ( (hpit != hole_points.end()) && ((*hpit)[0] < end) )
          continue;

        // updating the seed point iterator to fit into the current interval
        while ( (spit != seed_points.end()) && (spit->first[0] < start) )
          ++spit;


        // determining the segment ID of the current interval
        int output_segment_id;
        if ( (spit != seed_points.end()) && (spit->first[0] < end) )
          output_segment_id = spit->second;
        else
        {
          output_segment_id = default_segment_id;
          if (use_different_segment_ids_for_unknown_segments)
            ++default_segment_id;
        }


        // calculate the interval length and the line count in the current interval
        double length = std::abs(end-start);
        unsigned int new_line_count = 1;
        if (cell_size > 0.0)
          new_line_count = static_cast<unsigned int>(length / cell_size + 0.5);


        // if only one line should be created
        if (new_line_count <= 1)
        {
          if (segmentation)
            viennagrid::make_line( segmentation->get_make_segment(output_segment_id), sorted_points[i-1], sorted_points[i] );
          else
            viennagrid::make_line( *mesh, sorted_points[i-1], sorted_points[i] );
        }
        else
        {
          // create all line segments
          unsigned int lines_in_between = new_line_count - 2;
          double delta = (end-start) / new_line_count;

          OutputVertexHandleType last_vertex = viennagrid::make_vertex( *mesh, PointType(start+delta) );

          if (segmentation)
            viennagrid::make_line( segmentation->get_make_segment(output_segment_id), sorted_points[i-1], last_vertex );
          else
            viennagrid::make_line( *mesh, sorted_points[i-1], last_vertex );

          for (unsigned int j = 0; j < lines_in_between; ++j)
          {
            OutputVertexHandleType tmp = viennagrid::make_vertex( *mesh, PointType(start+delta*(j+2)) );

            if (segmentation)
              viennagrid::make_line( segmentation->get_make_segment(output_segment_id), last_vertex, tmp );
            else
              viennagrid::make_line( *mesh, last_vertex, tmp );

            last_vertex = tmp;
          }

          if (segmentation)
            viennagrid::make_line( segmentation->get_make_segment(output_segment_id), last_vertex, sorted_points[i] );
          else
            viennagrid::make_line( *mesh, last_vertex, sorted_points[i] );
        }
      }

      return true;
    }











    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "ViennaGrid 1D line mesher"; }





      template<typename GeometrySegmentationT>
      void extract_seed_points( GeometrySegmentationT const & segmentation, point_1d_container const & hole_points,
                                seed_point_1d_container & seed_points )
      {
        int highest_segment_id = -1;
        for (seed_point_1d_container::iterator spit = seed_points.begin(); spit != seed_points.end(); ++spit)
          highest_segment_id = std::max( highest_segment_id, spit->second );
        ++highest_segment_id;

        for (typename GeometrySegmentationT::const_iterator sit = segmentation.begin(); sit != segmentation.end(); ++sit)
        {
          viennagrid::line_1d_mesh viennagrid_mesh;

          generate_mesh<typename GeometrySegmentationT::segment_handle_type, viennagrid::line_1d_mesh, viennagrid::line_1d_segmentation>( *sit, &viennagrid_mesh, NULL, -1, false, -1, seed_point_1d_container(), hole_points );

          unsigned int i = seed_points.size();
          viennamesh::extract_seed_points::extract_seed_points( viennagrid_mesh, seed_points, highest_segment_id++ );
          for (; i < seed_points.size(); ++i)
            info(5) << "Found seed point: " << seed_points[i].first << std::endl;
        }
      }








      template<typename GeometryT, typename GeometrySegmentationT, typename OutputMeshT, typename OutputSegmentationT>
      bool generic_run_impl()
      {
        typedef typename viennagrid::result_of::point<GeometryT>::type PointType;

        typedef viennagrid::segmented_mesh<GeometryT, GeometrySegmentationT> SegmentedGeometryType;

        typedef typename viennamesh::result_of::const_parameter_handle<GeometryT>::type GeometryHandleType;
        typedef typename viennamesh::result_of::const_parameter_handle<SegmentedGeometryType>::type SegmentedGeometryHandleType;

        typedef typename viennagrid::result_of::segment_handle<OutputSegmentationT>::type OutputSegmentHandleType;
        typedef typename viennagrid::segmented_mesh<OutputMeshT, OutputSegmentationT> OutputSegmentedMesh;

        typedef typename viennagrid::result_of::const_vertex_handle<GeometryT>::type GeometryVertexHandleType;
        typedef typename viennagrid::result_of::vertex_handle<OutputMeshT>::type OutputVertexHandleType;

        typedef typename viennagrid::result_of::const_vertex_range<GeometryT>::type ConstVertexRangeType;
        typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;


        GeometryT const * geometry = NULL;
        GeometrySegmentationT const * geometry_segmentation = NULL;

        // query input parameters
        SegmentedGeometryHandleType segmented_geometry_handle = get_input<SegmentedGeometryType>("default");
        if (segmented_geometry_handle)
        {
          geometry = &(segmented_geometry_handle().mesh);
          geometry_segmentation = &(segmented_geometry_handle().segmentation);
        }
        else
        {
          GeometryHandleType geometry_handle = get_input<GeometryT>("default");
          if (!geometry_handle)
            return false;

          geometry = &(geometry_handle());
        }





        // query possible output parameters: mesh and segmented mesh
        output_parameter_proxy<OutputMeshT> output_mesh = output_proxy<OutputMeshT>("default");
        output_parameter_proxy<OutputSegmentedMesh> output_segmented_mesh = output_proxy<OutputSegmentedMesh>("default");

        // query cell size input parameter
        double cell_size = -1.0;
        copy_input( "cell_size", cell_size );
        info(10) << "Using cell size: " << cell_size << std::endl;

        bool make_segmented_mesh = false;
        copy_input( "make_segmented_mesh", make_segmented_mesh );

        bool use_different_segment_ids_for_unknown_segments = false;
        copy_input( "use_different_segment_ids_for_unknown_segments", use_different_segment_ids_for_unknown_segments );
        info(10) << "Using different segment IDs for unknown segments: " << cell_size << std::endl;

        // query seed points input parameter
        seed_point_1d_container seed_points;
        typedef viennamesh::result_of::const_parameter_handle<seed_point_1d_container>::type ConstSeedPointContainerHandle;
        ConstSeedPointContainerHandle seed_points_handle = get_input<seed_point_1d_container>("seed_points");
        if (seed_points_handle && !seed_points_handle().empty())
        {
          info(10) << "Found seed points -> enabling make_segmented_mesh" << std::endl;
          seed_points = seed_points_handle();
          make_segmented_mesh = true;
        }

        // query hole points input parameter
        point_1d_container hole_points;
        typedef viennamesh::result_of::const_parameter_handle<point_1d_container>::type ConstPointContainerHandle;
        ConstPointContainerHandle hole_points_handle = get_input<point_1d_container>("hole_points");
        if (hole_points_handle && !hole_points_handle().empty())
        {
          info(10) << "Found hole points" << std::endl;
          hole_points = hole_points_handle();
        }


        bool extract_segment_seed_points = true;
        copy_input( "extract_segment_seed_points", extract_segment_seed_points );
        if (extract_segment_seed_points && geometry_segmentation && geometry_segmentation->size() >= 1)
        {
          extract_seed_points( *geometry_segmentation, hole_points, seed_points );
        }



        // decide, if mesh or segmented mesh is used (seed points available?)
        OutputMeshT * mesh = NULL;
        OutputSegmentationT * segmentation = NULL;

        if (!make_segmented_mesh)
        {
          info(10) << "Making a non-segmented mesh" << std::endl;
          mesh = &output_mesh();
        }
        else
        {
          info(10) << "Making a segmented mesh" << std::endl;
          mesh = &output_segmented_mesh().mesh;
          segmentation = &output_segmented_mesh().segmentation;
        }


        // copy and sort vertices
        ConstVertexRangeType vertices( *geometry );
        std::list<GeometryVertexHandleType> sorted_geometry_points;
        for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
          sorted_geometry_points.push_back( vit.handle() );
        sorted_geometry_points.sort( vertex_handle_point_sorter_1d<GeometryT>(*geometry) );


        // query and determine minimal line length
        double relative_min_geometry_point_distance = 1e-10;
        copy_input( "relative_min_geometry_point_distance", relative_min_geometry_point_distance );

        double absolute_min_geometry_point_distance =
          (viennagrid::point(*geometry, sorted_geometry_points.back())[0] -
          viennagrid::point(*geometry, sorted_geometry_points.front())[0]) * relative_min_geometry_point_distance;
        copy_input( "absolute_min_geometry_point_distance", absolute_min_geometry_point_distance );



        generate_mesh( *geometry, mesh, segmentation, cell_size, use_different_segment_ids_for_unknown_segments, absolute_min_geometry_point_distance, seed_points, hole_points );

        return true;
      }




/*





      template<typename GeometryT, typename OutputMeshT, typename OutputSegmentationT>
      bool generic_run_impl()
      {
        typedef typename viennagrid::result_of::point<GeometryT>::type PointType;

        typedef typename viennamesh::result_of::const_parameter_handle<GeometryT>::type GeometryHandleType;

        typedef typename viennagrid::result_of::segment_handle<OutputSegmentationT>::type OutputSegmentHandleType;
        typedef typename viennagrid::segmented_mesh<OutputMeshT, OutputSegmentationT> OutputSegmentedMesh;

        typedef typename viennagrid::result_of::const_vertex_handle<GeometryT>::type GeometryVertexHandleType;
        typedef typename viennagrid::result_of::vertex_handle<OutputMeshT>::type OutputVertexHandleType;

        typedef typename viennagrid::result_of::const_vertex_range<GeometryT>::type ConstVertexRangeType;
        typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;



        // query input parameters
        GeometryHandleType geometry_handle = get_required_input<GeometryT>("default");
        GeometryT const & geometry = geometry_handle();

        // query possible output parameters: mesh and segmented mesh
        output_parameter_proxy<OutputMeshT> output_mesh = output_proxy<OutputMeshT>("default");
        output_parameter_proxy<OutputSegmentedMesh> output_segmented_mesh = output_proxy<OutputSegmentedMesh>("default");

        // query cell size input parameter
        double cell_size = -1.0;
        copy_input( "cell_size", cell_size );
        info(10) << "Using cell size: " << cell_size << std::endl;

        bool make_segmented_mesh = false;
        copy_input( "make_segmented_mesh", make_segmented_mesh );

        bool use_different_segment_ids_for_unknown_segments = false;
        copy_input( "use_different_segment_ids_for_unknown_segments", use_different_segment_ids_for_unknown_segments );
        info(10) << "Using different segment IDs for unknown segments: " << cell_size << std::endl;

        // query seed points input parameter
        seed_point_1d_container seed_points;
        typedef viennamesh::result_of::const_parameter_handle<seed_point_1d_container>::type ConstSeedPointContainerHandle;
        ConstSeedPointContainerHandle seed_points_handle = get_input<seed_point_1d_container>("seed_points");
        if (seed_points_handle && !seed_points_handle().empty())
        {
          info(10) << "Found seed points -> enabling make_segmented_mesh" << std::endl;
          seed_points = seed_points_handle();
          make_segmented_mesh = true;
        }

        // query hole points input parameter
        point_1d_container hole_points;
        typedef viennamesh::result_of::const_parameter_handle<point_1d_container>::type ConstPointContainerHandle;
        ConstPointContainerHandle hole_points_handle = get_input<point_1d_container>("hole_points");
        if (hole_points_handle && !hole_points_handle().empty())
        {
          info(10) << "Found hole points" << std::endl;
          hole_points = hole_points_handle();
        }



        // decide, if mesh or segmented mesh is used (seed points available?)
        OutputMeshT * mesh = NULL;
        OutputSegmentationT * segmentation = NULL;

        if (!make_segmented_mesh)
        {
          info(10) << "Making a non-segmented mesh" << std::endl;
          mesh = &output_mesh();
        }
        else
        {
          info(10) << "Making a segmented mesh" << std::endl;
          mesh = &output_segmented_mesh().mesh;
          segmentation = &output_segmented_mesh().segmentation;
        }


        // copy and sort vertices
        ConstVertexRangeType vertices( geometry );
        std::list<GeometryVertexHandleType> sorted_geometry_points;
        for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
          sorted_geometry_points.push_back( vit.handle() );
        sorted_geometry_points.sort( vertex_handle_point_sorter_1d<GeometryT>(geometry) );


        // query and determine minimal line length
        double relative_min_geometry_point_distance = 1e-10;
        copy_input( "relative_min_geometry_point_distance", relative_min_geometry_point_distance );

        double absolute_min_geometry_point_distance =
          (viennagrid::point(geometry, sorted_geometry_points.back())[0] -
          viennagrid::point(geometry, sorted_geometry_points.front())[0]) * relative_min_geometry_point_distance;
        copy_input( "absolute_min_geometry_point_distance", absolute_min_geometry_point_distance );


        // remove points which would lead to too short lines
        {
          typename std::list<GeometryVertexHandleType>::iterator vhit0 = sorted_geometry_points.begin();
          typename std::list<GeometryVertexHandleType>::iterator vhit1 = vhit0; ++vhit1;

          while (vhit1 != sorted_geometry_points.end())
          {
            double length = std::abs(viennagrid::point(geometry, *vhit0)[0] -
                                     viennagrid::point(geometry, *vhit1)[0]);

            if (length < absolute_min_geometry_point_distance)
            {
              sorted_geometry_points.erase(vhit1++);
            }
            else
            {
              ++vhit0;
              ++vhit1;
            }
          }
        }


        // create points in output mesh
        std::vector<OutputVertexHandleType> sorted_points;
        for (typename std::list<GeometryVertexHandleType>::iterator vhit = sorted_geometry_points.begin(); vhit != sorted_geometry_points.end(); ++vhit)
        {
          OutputVertexHandleType vertex_handle =
          viennagrid::make_vertex(*mesh, viennagrid::point(geometry, *vhit) );

          sorted_points.push_back(vertex_handle);
        }


        // determine unused segment ID
        int default_segment_id = -1;
        for (seed_point_1d_container::iterator it = seed_points.begin(); it != seed_points.end(); ++it)
          default_segment_id = std::max(it->second, default_segment_id);
        ++default_segment_id;
        info(10) << "Default segment id: " << default_segment_id << std::endl;


        // sort seed points and hole points
        std::sort( seed_points.begin(), seed_points.end(), point_sorter_1d<GeometryT>() );
        std::sort( hole_points.begin(), hole_points.end(), point_sorter_1d<GeometryT>() );

        seed_point_1d_container::iterator spit = seed_points.begin();
        point_1d_container::iterator hpit = hole_points.begin();


        // iterate over all intervals in sorted point array
        for (unsigned int i = 1; i < sorted_points.size(); ++i)
        {
          // getting start and end point of the current interval
          double start = viennagrid::point( *mesh, sorted_points[i-1])[0];
          double end = viennagrid::point( *mesh, sorted_points[i])[0];


          // updating the hole point iterator to fit into the current interval
          while ( (hpit != hole_points.end()) && ((*hpit)[0] < start) )
            ++hpit;

          // if the hole point is inside the current interval -> skip current interval
          if ( (hpit != hole_points.end()) && ((*hpit)[0] < end) )
            continue;

          // updating the seed point iterator to fit into the current interval
          while ( (spit != seed_points.end()) && (spit->first[0] < start) )
            ++spit;


          // determining the segment ID of the current interval
          int output_segment_id;
          if ( (spit != seed_points.end()) && (spit->first[0] < end) )
            output_segment_id = spit->second;
          else
          {
            output_segment_id = default_segment_id;
            if (use_different_segment_ids_for_unknown_segments)
              ++default_segment_id;
          }


          // calculate the interval length and the line count in the current interval
          double length = std::abs(end-start);
          unsigned int new_line_count = 1;
          if (cell_size > 0.0)
            new_line_count = static_cast<unsigned int>(length / cell_size + 0.5);


          // if only one line should be created
          if (new_line_count <= 1)
          {
            if (segmentation)
              viennagrid::make_line( segmentation->get_make_segment(output_segment_id), sorted_points[i-1], sorted_points[i] );
            else
              viennagrid::make_line( *mesh, sorted_points[i-1], sorted_points[i] );
          }
          else
          {
            // create all line segments
            unsigned int lines_in_between = new_line_count - 2;
            double delta = (end-start) / new_line_count;

            OutputVertexHandleType last_vertex = viennagrid::make_vertex( *mesh, PointType(start+delta) );

            if (segmentation)
              viennagrid::make_line( segmentation->get_make_segment(output_segment_id), sorted_points[i-1], last_vertex );
            else
              viennagrid::make_line( *mesh, sorted_points[i-1], last_vertex );

            for (unsigned int j = 0; j < lines_in_between; ++j)
            {
              OutputVertexHandleType tmp = viennagrid::make_vertex( *mesh, PointType(start+delta*(j+2)) );

              if (segmentation)
                viennagrid::make_line( segmentation->get_make_segment(output_segment_id), last_vertex, tmp );
              else
                viennagrid::make_line( *mesh, last_vertex, tmp );

              last_vertex = tmp;
            }

            if (segmentation)
              viennagrid::make_line( segmentation->get_make_segment(output_segment_id), last_vertex, sorted_points[i] );
            else
              viennagrid::make_line( *mesh, last_vertex, sorted_points[i] );
          }
        }

        return true;
      }*/




      bool run_impl()
      {
        if (generic_run_impl<viennagrid::brep_1d_mesh, viennagrid::brep_1d_segmentation, viennagrid::line_1d_mesh, viennagrid::line_1d_segmentation>())
          return true;

        return false;
      }
    };


  }

}

#endif
