#ifndef VIENNAMESH_ALGORITHM_MESHER1D_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_MESHER1D_GENERATOR_HPP

#include "viennagrid/algorithm/norm.hpp"
#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  namespace mesher1d
  {
    template<typename MeshT>
    class VertexHandlePointSorter1D
    {
    public:
      typedef typename viennagrid::result_of::const_vertex_handle<MeshT>::type ConstVertexHandleType;

      VertexHandlePointSorter1D(MeshT const & mesh_) : mesh(mesh_) {}

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
    class PointSorter1D
    {
    public:
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;

      bool operator()( PointType const & p0, PointType const & p1 ) const
      { return p0 [0] < p1[0]; }

      bool operator()( std::pair<PointType, int> const & sp0, std::pair<PointType, int> const & sp1 ) const
      { return sp0.first[0] < sp1.first[0]; }
    };



    class Algorithm : public BaseAlgorithm
    {
    public:

      string name() const { return "Triangle 1.6 mesher"; }


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
        GeometryHandleType input_geometry = get_required_input<GeometryT>("default");

        // query possible output parameters: mesh and segmented mesh
        OutputParameterProxy<OutputMeshT> output_mesh = output_proxy<OutputMeshT>("default");
        OutputParameterProxy<OutputSegmentedMesh> output_segmented_mesh = output_proxy<OutputSegmentedMesh>("default");

        // query cell size input parameter
        double cell_size = -1.0;
        copy_input( "cell_size", cell_size );

        // query seed points input parameter
        SeedPoint1DContainer seed_points;
        typedef viennamesh::result_of::const_parameter_handle<SeedPoint1DContainer>::type ConstSeedPointContainerHandle;
        ConstSeedPointContainerHandle seed_points_handle = get_input<SeedPoint1DContainer>("seed_points");
        if (seed_points_handle && !seed_points_handle->get().empty())
          seed_points = seed_points_handle->get();

        // query hole points input parameter
        Point1DContainer hole_points;
        typedef viennamesh::result_of::const_parameter_handle<Point1DContainer>::type ConstPointContainerHandle;
        ConstPointContainerHandle hole_points_handle = get_input<Point1DContainer>("hole_points");
        if (hole_points_handle && !hole_points_handle->get().empty())
          hole_points = hole_points_handle->get();



        // decide, if mesh or segmented mesh is used (seed points available?)
        OutputMeshT * mesh = NULL;
        OutputSegmentationT * segmentation = NULL;

        if (seed_points.empty())
          mesh = &output_mesh();
        else
        {
          mesh = &output_segmented_mesh().mesh;
          segmentation = &output_segmented_mesh().segmentation;
        }


        // copy and sort vertices
        ConstVertexRangeType vertices( input_geometry->get() );
        std::list<GeometryVertexHandleType> sorted_geometry_points;
        for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
          sorted_geometry_points.push_back( vit.handle() );
        sorted_geometry_points.sort( VertexHandlePointSorter1D<GeometryT>(input_geometry->get()) );


        // query and determine minimal line length
        double relative_min_geometry_point_distance = 1e-10;
        copy_input( "relative_min_geometry_point_distance", relative_min_geometry_point_distance );

        double absolute_min_geometry_point_distance =
          (viennagrid::point(input_geometry->get(), sorted_geometry_points.back())[0] -
          viennagrid::point(input_geometry->get(), sorted_geometry_points.front())[0]) * relative_min_geometry_point_distance;
        copy_input( "absolute_min_geometry_point_distance", absolute_min_geometry_point_distance );


        // remove points which would lead to too short lines
        {
          typename std::list<GeometryVertexHandleType>::iterator vhit0 = sorted_geometry_points.begin();
          typename std::list<GeometryVertexHandleType>::iterator vhit1 = vhit0; ++vhit1;

          while (vhit1 != sorted_geometry_points.end())
          {
            double length = std::abs(viennagrid::point(input_geometry->get(), *vhit0)[0] -
                                     viennagrid::point(input_geometry->get(), *vhit1)[0]);

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
          viennagrid::make_vertex(*mesh, viennagrid::point(input_geometry->get(), *vhit) );

          sorted_points.push_back(vertex_handle);
        }


        // determine unused segment ID
        int default_segment_id = -1;
        for (SeedPoint1DContainer::iterator it = seed_points.begin(); it != seed_points.end(); ++it)
          default_segment_id = std::max(it->second, default_segment_id);
        ++default_segment_id;


        // sort seed points and hole points
        std::sort( seed_points.begin(), seed_points.end(), PointSorter1D<GeometryT>() );
        std::sort( hole_points.begin(), hole_points.end(), PointSorter1D<GeometryT>() );

        SeedPoint1DContainer::iterator spit = seed_points.begin();
        Point1DContainer::iterator hpit = hole_points.begin();


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
            output_segment_id = default_segment_id;


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

            for (unsigned int i = 0; i < lines_in_between; ++i)
            {
              OutputVertexHandleType tmp = viennagrid::make_vertex( *mesh, PointType(start+delta*(i+2)) );

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




      bool run_impl()
      {
        return generic_run_impl<viennagrid::vertex_1d_mesh, viennagrid::line_1d_mesh, viennagrid::line_1d_segmentation>();
      }
    };


  }

}

#endif
