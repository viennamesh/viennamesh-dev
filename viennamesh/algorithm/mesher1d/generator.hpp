#ifndef VIENNAMESH_ALGORITHM_MESHER1D_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_MESHER1D_GENERATOR_HPP

#include "viennagrid/algorithm/norm.hpp"
#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  namespace mesher1d
  {
//     template<typename InputMeshT, typename InputSegmentationT,
//              typename OutputMeshT, typename OutputSegmentationT>
//     void adapt( viennagrid::vertex_1d_mesh const & input_mesh,
//                 viennagrid::line_1d_mesh & output_mesh, viennagrid::line_1d_segmentation & output_segmentation,
//                 ConstParameterSet const & parameters )
//     {
//       typedef typename viennagrid::result_of::coord<InputMeshT>::type NumericType;
//       typedef typename viennagrid::result_of::point<InputMeshT>::type PointType;
//
//       typedef typename InputSegmentationT::const_iterator ConstSegmentHandleIteratorType;
//       typedef typename viennagrid::result_of::segment_handle<InputSegmentationT>::type InputSegmentHandleType;
//       typedef typename viennagrid::result_of::segment_handle<OutputSegmentationT>::type OutputSegmentHandleType;
//
//       typedef typename viennagrid::result_of::const_vertex_range<InputMeshT>::type ConstVertexRangeType;
//       typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;
//
//       typedef typename viennagrid::result_of::const_vertex_handle<InputMeshT>::type ConstInputVertexHandleType;
//       typedef typename viennagrid::result_of::vertex_handle<OutputMeshT>::type OutputVertexHandleType;
//
//       std::map<ConstInputVertexHandleType, OutputVertexHandleType> vertex_map;
//       ConstVertexRangeType vertices(input_mesh);
//       for(ConstVertexIteratorType it = vertices.begin(); it != vertices.end(); ++it)
//         vertex_map[ it.handle() ] = viennagrid::make_vertex( output_mesh, viennagrid::point(output_mesh, *it) );
//
//       double cell_size = -1.0;
//       parameters.copy_if_present( "cell_size", cell_size );
//
//       for (ConstSegmentHandleIteratorType sit = input_segmentation.begin(); sit != input_segmentation.end(); ++sit)
//       {
//         OutputSegmentHandleType output_segment = output_segmentation.get_make_segment( sit->id() );
//
//         typedef typename viennagrid::result_of::const_line_range<InputSegmentHandleType>::type ConstLineRangeType;
//         typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;
//
//         ConstLineRangeType lines(*sit);
//         for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
//         {
//           PointType start = viennagrid::point(input_mesh, viennagrid::vertices(*lit)[0]);
//           PointType end = viennagrid::point(input_mesh, viennagrid::vertices(*lit)[1]);
//
//           double length = viennagrid::norm_2(end-start);
//           unsigned int new_line_count = 1;
//           if (cell_size > 0.0)
//             new_line_count = static_cast<unsigned int>(length / cell_size + 0.5);
//
//           if (new_line_count <= 1)
//           {
//             viennagrid::make_line( output_segment,
//                                    vertex_map[viennagrid::vertices(*lit).handle_at(0)],
//                                    vertex_map[viennagrid::vertices(*lit).handle_at(1)] );
//           }
//           else
//           {
//             unsigned int lines_in_between = new_line_count - 2;
//             PointType delta = (end-start) / new_line_count;
//
//
//             OutputVertexHandleType last_vertex = viennagrid::make_vertex( output_mesh, start+delta );
//
//             viennagrid::make_line( output_segment,
//                                    vertex_map[viennagrid::vertices(*lit).handle_at(0)],
//                                    last_vertex );
//
//             for (unsigned int i = 0; i < lines_in_between; ++i)
//             {
//               OutputVertexHandleType tmp = viennagrid::make_vertex( output_mesh, start+delta*(i+2) );
//               viennagrid::make_line( output_segment, last_vertex, tmp );
//               last_vertex = tmp;
//             }
//
//             viennagrid::make_line( output_segment,
//                                    last_vertex,
//                                    vertex_map[viennagrid::vertices(*lit).handle_at(1)] );
//           }
//
//         }
//       }
//     }
//
//
//
//     template<typename SegmentedMeshT>
//     bool adapt( ConstParameterSet const & inputs, ParameterSet & outputs )
//     {
//       typedef typename viennamesh::result_of::const_parameter_handle<SegmentedMeshT>::type ConstMeshParameterType;
//
//       ConstMeshParameterType input_mesh = inputs.get<SegmentedMeshT>( "default" );
//
//       if (!input_mesh)
//         return false;
//
//       viennamesh::OutputParameterProxy<SegmentedMeshT> output_mesh(outputs, "default");
//
//       adapt( input_mesh->get().mesh, input_mesh->get().segmentation, output_mesh().mesh, output_mesh().segmentation, inputs );
//
//       output_mesh.set();
//
//       return true;
//     }



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



    template<typename MeshT>
    class no_segment_handle
    {
    public:

      typedef int segmend_id_type;

      no_segment_handle(MeshT & mesh_) : mesh(mesh_) {}

    private:
      MeshT & mesh;
    };


    template<typename MeshT>
    class no_segmentation
    {
    private:
      no_segmentation(MeshT & mesh_) : mesh(mesh_) {}

    public:
      MeshT & mesh;
    };




//     template<typename SegmentedMeshT>
//     bool adapt( ConstParameterSet const & inputs, ParameterSet & outputs )
//     {
//       typedef typename viennamesh::result_of::const_parameter_handle<SegmentedMeshT>::type ConstMeshParameterType;
//
//       ConstMeshParameterType input_mesh = inputs.get<SegmentedMeshT>( "default" );
//
//       if (!input_mesh)
//         return false;
//
//       viennamesh::OutputParameterProxy<SegmentedMeshT> output_mesh(outputs, "default");
//
//       adapt( input_mesh->get().mesh, input_mesh->get().segmentation, output_mesh().mesh, output_mesh().segmentation, inputs );
//
//       output_mesh.set();
//
//       return true;
//     }








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

        typedef typename viennagrid::result_of::vertex_handle<OutputMeshT>::type OutputVertexHandleType;

        typedef typename viennagrid::result_of::const_vertex_range<GeometryT>::type ConstVertexRangeType;
        typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;



        GeometryHandleType input_geometry = get_required_input<GeometryT>("default");
        OutputParameterProxy<OutputSegmentedMesh> output_mesh = output_proxy<OutputSegmentedMesh>("default");

        double cell_size = -1.0;
        copy_input( "cell_size", cell_size );


        std::vector<OutputVertexHandleType> sorted_points;

        ConstVertexRangeType vertices( input_geometry->get() );
        for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
        {
          OutputVertexHandleType vertex_handle =
          viennagrid::make_vertex(
            output_mesh().mesh,
            viennagrid::point(input_geometry->get(), *vit) );

          sorted_points.push_back(vertex_handle);
        }


        std::sort( sorted_points.begin(), sorted_points.end(), VertexHandlePointSorter1D<OutputMeshT>(output_mesh().mesh) );



        SeedPoint1DContainer seed_points;
        typedef viennamesh::result_of::const_parameter_handle<SeedPoint1DContainer>::type ConstSeedPointContainerHandle;
        ConstSeedPointContainerHandle seed_points_handle = get_input<SeedPoint1DContainer>("seed_points");
        if (seed_points_handle && !seed_points_handle->get().empty())
          seed_points = seed_points_handle->get();

        Point1DContainer hole_points;
        typedef viennamesh::result_of::const_parameter_handle<Point1DContainer>::type ConstPointContainerHandle;
        ConstPointContainerHandle hole_points_handle = get_input<Point1DContainer>("hole_points");
        if (hole_points_handle && !hole_points_handle->get().empty())
          hole_points = hole_points_handle->get();



        std::sort( seed_points.begin(), seed_points.end(), PointSorter1D<GeometryT>() );
        std::sort( hole_points.begin(), hole_points.end(), PointSorter1D<GeometryT>() );

        SeedPoint1DContainer::iterator spit = seed_points.begin();
        Point1DContainer::iterator hpit = hole_points.begin();

        int default_segment_id = -1;
        for (SeedPoint1DContainer::iterator it = seed_points.begin(); it != seed_points.end(); ++it)
          default_segment_id = std::max(it->second, default_segment_id);
        ++default_segment_id;



        for (unsigned int i = 1; i < sorted_points.size(); ++i)
        {
          double start = viennagrid::point( output_mesh().mesh, viennagrid::dereference_handle(output_mesh().mesh, sorted_points[i-1]) )[0];
          double end = viennagrid::point( output_mesh().mesh, viennagrid::dereference_handle(output_mesh().mesh, sorted_points[i]) )[0];

          while ( (hpit != hole_points.end()) && ((*hpit)[0] < start) )
            ++hpit;

          if ( (hpit != hole_points.end()) && ((*hpit)[0] < end) )
          {
            continue;
          }

          while ( (spit != seed_points.end()) && (spit->first[0] < start) )
            ++spit;

          int output_segment_id;
          if ( (spit != seed_points.end()) && (spit->first[0] < end) )
          {
            output_segment_id = spit->second;
          }
          else
            output_segment_id = default_segment_id;


          OutputSegmentHandleType output_segment = output_mesh().segmentation.get_make_segment( output_segment_id );

          double length = std::abs(end-start);

          unsigned int new_line_count = 1;
          if (cell_size > 0.0)
            new_line_count = static_cast<unsigned int>(length / cell_size + 0.5);

          if (new_line_count <= 1)
          {
            viennagrid::make_line( output_segment,
                                   sorted_points[i-1],
                                   sorted_points[i] );
          }
          else
          {
            unsigned int lines_in_between = new_line_count - 2;
            double delta = (end-start) / new_line_count;


            OutputVertexHandleType last_vertex = viennagrid::make_vertex( output_mesh().mesh, PointType(start+delta) );

            viennagrid::make_line( output_segment,
                                   sorted_points[i-1],
                                   last_vertex );

            for (unsigned int i = 0; i < lines_in_between; ++i)
            {
              OutputVertexHandleType tmp = viennagrid::make_vertex( output_mesh().mesh, PointType(start+delta*(i+2)) );
              viennagrid::make_line( output_segment, last_vertex, tmp );
              last_vertex = tmp;
            }

            viennagrid::make_line( output_segment,
                                   last_vertex,
                                   sorted_points[i] );
          }
        }

//         return output_mesh.set();
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
