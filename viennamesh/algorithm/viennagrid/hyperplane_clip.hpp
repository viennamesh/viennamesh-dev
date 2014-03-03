#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_DIRECTIONAL_CLIP_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_DIRECTIONAL_CLIP_HPP

#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/boundary.hpp"

#include "viennamesh/core/algorithm.hpp"


namespace viennamesh
{
  namespace directional_clip
  {


    template<typename PointT>
    bool on_positive_hyperplane_side( PointT const & hyperplane_point, PointT const & hyperplane_normal, PointT const & to_test )
    {
      return viennagrid::inner_prod( hyperplane_normal, to_test-hyperplane_point ) > -1e-6 * viennagrid::norm_2(to_test-hyperplane_point);
    }

    template<typename PointT>
    bool on_hyperplane( PointT const & hyperplane_point, PointT const & hyperplane_normal, PointT const & to_test )
    {
      return std::abs(viennagrid::inner_prod( hyperplane_normal, to_test-hyperplane_point )) <= 1e-6 * viennagrid::norm_2(to_test-hyperplane_point);
    }


    // clip a single triangle
    template<typename WrappedConfigT, typename DstMeshT, typename VertexCopyMapT, typename PointT>
    std::vector<typename viennagrid::result_of::triangle_handle<DstMeshT>::type>
    clip_element( viennagrid::element<viennagrid::triangle_tag, WrappedConfigT> const & triangle,
                  DstMeshT & dst_mesh, VertexCopyMapT & vertex_copy_map,
                  PointT const & hyperplane_point, PointT const & hyperplane_normal)
    {
      std::vector<typename viennagrid::result_of::triangle_handle<DstMeshT>::type> result;

      typedef viennagrid::element<viennagrid::triangle_tag, WrappedConfigT> TriangleType;

      typedef typename viennagrid::result_of::const_vertex_range<TriangleType>::type ConstVertexOfTriangleRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVertexOfTriangleRangeType>::type ConstLinesOfTriangleIteratorType;

      typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type DstVertexHandleType;
      typedef typename viennagrid::result_of::triangle_handle<DstMeshT>::type DstTriangleHandleType;

      ConstVertexOfTriangleRangeType vertices(triangle);

      // check how many points lay on the clip hyperplane
      bool on_clip_plane[3];
      int on_clip_plane_count = 0;
      for (int i = 0; i < vertices.size(); ++i)
      {
        on_clip_plane[i] = on_hyperplane( hyperplane_point, hyperplane_normal, viennagrid::point(vertices[i]) );
        if (on_clip_plane[i])
          ++on_clip_plane_count;
      }

      // if there is exactly one vertex laying on the clip hyperplane
      if (on_clip_plane_count == 1)
      {
        int on_clip_plane_index = (on_clip_plane[0] ? 0 : (on_clip_plane[1] ? 1 : 2));
        int not_on_clip_plane_index0 = (on_clip_plane_index == 0 ? 1 : 0);
        int not_on_clip_plane_index1 = (on_clip_plane_index == 2 ? 1 : 2);

        bool not_on_clip_plane_vertex0_on_clip_side = on_positive_hyperplane_side(hyperplane_point, hyperplane_normal, viennagrid::point(vertices[not_on_clip_plane_index0]));
        bool not_on_clip_plane_vertex1_on_clip_side = on_positive_hyperplane_side(hyperplane_point, hyperplane_normal, viennagrid::point(vertices[not_on_clip_plane_index1]));

        // both other vertices lay on clip side -> do nothing
        if (not_on_clip_plane_vertex0_on_clip_side && not_on_clip_plane_vertex1_on_clip_side)
          return result;

        // both other vertices lay on the preserve side -> copy triangle
        if (!not_on_clip_plane_vertex0_on_clip_side && !not_on_clip_plane_vertex1_on_clip_side)
        {
          result.push_back(viennagrid::make_triangle( dst_mesh,
                                                      vertex_copy_map(vertices[0]),
                                                      vertex_copy_map(vertices[1]),
                                                      vertex_copy_map(vertices[2]) ));
          return result;
        }
      }

      // count elements which lay on the the clip side
      bool on_clip_side[3];
      int clip_vertex_count = 0;
      for (int i = 0; i < vertices.size(); ++i)
      {
        on_clip_side[i] = on_positive_hyperplane_side(hyperplane_point, hyperplane_normal, viennagrid::point(vertices[i]));
        if (on_clip_side[i])
          ++clip_vertex_count;
      }

      if (clip_vertex_count == 2) // exactly two elements lay on the clip side -> create one element
      {
        int to_preserve_index = (!on_clip_side[0] ? 0 : (!on_clip_side[1] ? 1 : 2));
        int to_clip_index1 = (to_preserve_index == 0 ? 1 : 0);
        int to_clip_index2 = (to_preserve_index == 2 ? 1 : 2);


        //                             |
        //                             | pd1
        //                             |---/ \  to_clip_vertex1
        //                             | /    \
        //                             X       \
        //                           / |p1      \
        //                         /   |         \
        //                       /     |    pd2   \
        //    to_preserve_vertex ------X-----------  to_clip_vertex2
        //                        qd   |p2
        //                             |


        DstVertexHandleType vh0 = vertex_copy_map( vertices[to_preserve_index] );

        double qd = viennagrid::inner_prod( hyperplane_normal, viennagrid::point(vertices[to_preserve_index])-hyperplane_point );
        double pd1 = viennagrid::inner_prod( hyperplane_normal, viennagrid::point(vertices[to_clip_index1])-hyperplane_point );
        double pd2 = viennagrid::inner_prod( hyperplane_normal, viennagrid::point(vertices[to_clip_index2])-hyperplane_point );

        PointT p1 = viennagrid::point(vertices[to_clip_index1]) - (pd1 / (pd1-qd)) * (viennagrid::point(vertices[to_clip_index1]) - viennagrid::point(vertices[to_preserve_index]));
        DstVertexHandleType vh1 = viennagrid::make_unique_vertex( dst_mesh, p1 );

        PointT p2 = viennagrid::point(vertices[to_clip_index2]) - (pd2 / (pd2-qd)) * (viennagrid::point(vertices[to_clip_index2]) - viennagrid::point(vertices[to_preserve_index]));
        DstVertexHandleType vh2 = viennagrid::make_unique_vertex( dst_mesh, p2 );

        result.push_back(viennagrid::make_triangle( dst_mesh, vh0, vh1, vh2 ));
        return result;
      }
      else if (clip_vertex_count == 1) // exactly one elements lay on the clip side -> create two element
      {
        int to_clip_index = (on_clip_side[0] ? 0 : (on_clip_side[1] ? 1 : 2));
        int to_preserve_index1 = (to_clip_index == 0 ? 1 : 0);
        int to_preserve_index2 = (to_clip_index == 2 ? 1 : 2);

        //                          |
        //                          | pd1
        //                          |---/ \  to_preserve_vertex1 (preserve_vh1)
        //                          | / /  \
        //                          X  /    \
        //                        / |p3 (vh3)\
        //                      /   | /       \
        //                    /     |/  pd2    \
        //    to_clip_vertex  ------X-----------  to_preserve_vertex2 (preserve_vh2)
        //                     qd   |p4 (vh4)
        //                          |

        DstVertexHandleType preserve_vh1 = vertex_copy_map( vertices[to_preserve_index1] );
        DstVertexHandleType preserve_vh2 = vertex_copy_map( vertices[to_preserve_index2] );

        double qd = viennagrid::inner_prod( hyperplane_normal, viennagrid::point(vertices[to_clip_index])-hyperplane_point );
        double pd1 = viennagrid::inner_prod( hyperplane_normal, viennagrid::point(vertices[to_preserve_index1])-hyperplane_point );
        double pd2 = viennagrid::inner_prod( hyperplane_normal, viennagrid::point(vertices[to_preserve_index2])-hyperplane_point );

        PointT p3 = viennagrid::point(vertices[to_preserve_index1]) - (pd1 / (pd1-qd)) * (viennagrid::point(vertices[to_preserve_index1]) - viennagrid::point(vertices[to_clip_index]));
        DstVertexHandleType vh3 = viennagrid::make_unique_vertex( dst_mesh, p3 );

        PointT p4 = viennagrid::point(vertices[to_preserve_index2]) - (pd2 / (pd2-qd)) * (viennagrid::point(vertices[to_preserve_index2]) - viennagrid::point(vertices[to_clip_index]));
        DstVertexHandleType vh4 = viennagrid::make_unique_vertex( dst_mesh, p4 );


        result.push_back( viennagrid::make_triangle( dst_mesh, preserve_vh1, preserve_vh2, vh4 ) );
        result.push_back( viennagrid::make_triangle( dst_mesh, preserve_vh1, vh3, vh4 ) );
        return result;
      }

      return result;
    }




    template<typename SrcMeshT, typename SrcSegmentationT, typename DstMeshT, typename DstSegmentationT, typename PointT>
    void hyperplane_clip(SrcMeshT const & src_mesh, SrcSegmentationT const & src_segmentation,
                         DstMeshT & dst_mesh, DstSegmentationT & dst_segmentation,
                         PointT const & hyperplane_point, PointT const & hyperplane_normal)
    {
      typedef typename viennagrid::result_of::const_cell_range<SrcMeshT>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;
      typedef typename viennagrid::result_of::cell<SrcMeshT>::type CellType;

      typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type DstMeshVertexHandleType;
      typedef typename viennagrid::result_of::cell_handle<DstMeshT>::type DstMeshCellHandleType;

      PointT normalized_hyperplane_normal = hyperplane_normal / viennagrid::norm_2(hyperplane_normal);

      viennagrid::vertex_copy_map<SrcMeshT, DstMeshT> vertex_map( dst_mesh );

      ConstCellRangeType cells(src_mesh);
      for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      {
        typedef typename viennagrid::result_of::const_vertex_range<CellType>::type ConstVertexOnCellRangeType;
        typedef typename viennagrid::result_of::iterator<ConstVertexOnCellRangeType>::type ConstVertexOnCellIteratorType;

        bool vertices_on_clip_side = false;
        bool vertices_on_preserve_side = false;

        ConstVertexOnCellRangeType vertices(*cit);
        for (ConstVertexOnCellIteratorType vcit = vertices.begin(); vcit != vertices.end(); ++vcit)
        {
          bool on_clip_side = on_positive_hyperplane_side( hyperplane_point, normalized_hyperplane_normal, viennagrid::point(*vcit) );
          bool on_preserve_side = on_positive_hyperplane_side( hyperplane_point, -normalized_hyperplane_normal, viennagrid::point(*vcit) );

          if (on_clip_side)
            vertices_on_clip_side = true;
          if (on_preserve_side)
            vertices_on_preserve_side = true;
        }

        if (vertices_on_preserve_side && !vertices_on_clip_side)
        {
          std::vector<DstMeshVertexHandleType> vertex_handles;
          for (ConstVertexOnCellIteratorType vcit = vertices.begin(); vcit != vertices.end(); ++vcit)
            vertex_handles.push_back( vertex_map(*vcit) );

          DstMeshCellHandleType new_cell = viennagrid::make_cell(dst_mesh,
                                                                 vertex_handles.begin(), vertex_handles.end());

          viennagrid::add( dst_segmentation, new_cell,
              viennagrid::segment_ids(src_segmentation, *cit).begin(),
              viennagrid::segment_ids(src_segmentation, *cit).end() );
        }

        if (vertices_on_preserve_side && vertices_on_clip_side)
        {
          std::vector<DstMeshCellHandleType> new_cells = clip_element( *cit, dst_mesh, vertex_map, hyperplane_point, normalized_hyperplane_normal );

          for (unsigned int i = 0; i < new_cells.size(); ++i)
            viennagrid::add( dst_segmentation, new_cells[i],
              viennagrid::segment_ids(src_segmentation, *cit).begin(),
              viennagrid::segment_ids(src_segmentation, *cit).end() );
        }
      }
    }



    template<typename SrcMeshT, typename DstMeshT, typename PointT>
    void hyperplane_clip(SrcMeshT const & src_mesh, DstMeshT & dst_mesh,
                         PointT const & hyperplane_point, PointT const & hyperplane_normal)
    {
      typedef typename viennagrid::result_of::const_cell_range<SrcMeshT>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;
      typedef typename viennagrid::result_of::cell<SrcMeshT>::type CellType;

      typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type DstMeshVertexHandleType;
      typedef typename viennagrid::result_of::cell_handle<DstMeshT>::type DstMeshCellHandleType;

      PointT normalized_hyperplane_normal = hyperplane_normal / viennagrid::norm_2(hyperplane_normal);

      viennagrid::vertex_copy_map<SrcMeshT, DstMeshT> vertex_map( dst_mesh );

      ConstCellRangeType cells(src_mesh);
      for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      {
        typedef typename viennagrid::result_of::const_vertex_range<CellType>::type ConstVertexOnCellRangeType;
        typedef typename viennagrid::result_of::iterator<ConstVertexOnCellRangeType>::type ConstVertexOnCellIteratorType;

        bool vertices_on_clip_side = false;
        bool vertices_on_preserve_side = false;

        ConstVertexOnCellRangeType vertices(*cit);
        for (ConstVertexOnCellIteratorType vcit = vertices.begin(); vcit != vertices.end(); ++vcit)
        {
          bool on_clip_side = on_positive_hyperplane_side( hyperplane_point, normalized_hyperplane_normal, viennagrid::point(*vcit) );
          bool on_preserve_side = on_positive_hyperplane_side( hyperplane_point, -normalized_hyperplane_normal, viennagrid::point(*vcit) );

          if (on_clip_side)
            vertices_on_clip_side = true;
          if (on_preserve_side)
            vertices_on_preserve_side = true;
        }

        if (vertices_on_preserve_side && !vertices_on_clip_side)
        {
          std::vector<DstMeshVertexHandleType> vertex_handles;
          for (ConstVertexOnCellIteratorType vcit = vertices.begin(); vcit != vertices.end(); ++vcit)
            vertex_handles.push_back( vertex_map(*vcit) );

          DstMeshCellHandleType new_cell = viennagrid::make_cell(dst_mesh,
                                                                 vertex_handles.begin(), vertex_handles.end());
        }

        if (vertices_on_preserve_side && vertices_on_clip_side)
        {
          std::vector<DstMeshCellHandleType> new_cells = clip_element( *cit, dst_mesh, vertex_map, hyperplane_point, normalized_hyperplane_normal );
        }
      }
    }







    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "ViennaGrid Hyperplane Clip"; }

      template<typename MeshT, typename SegmentationT>
      bool generic_run( dynamic_point const & base_hyperplane_point, dynamic_point const & base_hyperplane_normal )
      {
        typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

        const int geometric_dimension = viennagrid::result_of::geometric_dimension<MeshT>::value;
        typedef typename viennamesh::result_of::point<geometric_dimension>::type PointType;

        PointType hyperplane_point;
        PointType hyperplane_normal;

        std::copy( base_hyperplane_point.begin(), base_hyperplane_point.end(), hyperplane_point.begin() );
        std::copy( base_hyperplane_normal.begin(), base_hyperplane_normal.end(), hyperplane_normal.begin() );

        {
          typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type input_mesh = get_input<SegmentedMeshType>("default");
          if (input_mesh)
          {
            output_parameter_proxy<SegmentedMeshType> output_mesh = output_proxy<SegmentedMeshType>( "default" );

            hyperplane_clip(input_mesh().mesh, input_mesh().segmentation,
                            output_mesh().mesh, output_mesh().segmentation,
                            hyperplane_point, hyperplane_normal );
            return true;
          }
        }

        {
          typename viennamesh::result_of::const_parameter_handle<MeshT>::type input_mesh = get_input<MeshT>("default");
          if (input_mesh)
          {
            output_parameter_proxy<MeshT> output_mesh = output_proxy<MeshT>( "default" );

            hyperplane_clip(input_mesh(), output_mesh(), hyperplane_point, hyperplane_normal );
            return true;
          }
        }

        return false;
      }

      bool run_impl()
      {
        viennamesh::const_parameter_handle mesh = get_input("default");
        if (!mesh)
        {
          error(1) << "Input Parameter 'default' (type: mesh) is missing" << std::endl;
          return false;
        }

        viennamesh::result_of::const_parameter_handle<dynamic_point>::type base_hyperplane_point = get_required_input<dynamic_point>("hyperplane_point");
        viennamesh::result_of::const_parameter_handle<dynamic_point>::type base_hyperplane_normal = get_required_input<dynamic_point>("hyperplane_normal");


        int mesh_geometric_dimension = lexical_cast<int>( mesh->get_property("geometric_dimension").first );

        if (mesh_geometric_dimension != base_hyperplane_point().size())
        {
          error(1) << "Dimension missmatch, mesh has geometric dimension " << mesh_geometric_dimension << " but hyperplane point has dimension " << base_hyperplane_point().size() << std::endl;
          return false;
        }

        if (mesh_geometric_dimension != base_hyperplane_normal().size())
        {
          error(1) << "Dimension missmatch, mesh has geometric dimension " << mesh_geometric_dimension << " but hyperplane normal has dimension " << base_hyperplane_normal().size() << std::endl;
          return false;
        }


        if (generic_run<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>(base_hyperplane_point(), base_hyperplane_normal()))
          return true;


        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }

    private:
    };

  }
}

#endif
