#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_DIRECTIONAL_CLIP_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_DIRECTIONAL_CLIP_HPP

#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/refine.hpp"

#include "viennamesh/core/algorithm.hpp"


namespace viennamesh
{

  namespace hyperplane_clip
  {

    template<typename PointT>
    bool on_positive_hyperplane_side( PointT const & hyperplane_point, PointT const & hyperplane_normal, PointT const & to_test )
    {
      return viennagrid::inner_prod( hyperplane_normal, to_test-hyperplane_point ) > 1e-8 * viennagrid::norm_2(to_test-hyperplane_point);
    }

    template<typename PointT>
    struct on_positive_hyperplane_side_functor
    {
      typedef bool result_type;

      on_positive_hyperplane_side_functor(PointT const & hyperplane_point_, PointT const & hyperplane_normal_) :
          hyperplane_point(hyperplane_point_),
          hyperplane_normal(hyperplane_normal_) {}

      template<typename ElementT>
      bool operator()(ElementT const & element) const
      {
        PointT centroid = viennagrid::centroid(element);
        return on_positive_hyperplane_side(hyperplane_point, hyperplane_normal, centroid);
      }

      PointT hyperplane_point;
      PointT hyperplane_normal;
    };



    template<typename SrcMeshT, typename DstMeshT, typename PointT, typename LineRefinementTagContainerT, typename LineRefinementVertexHandleContainerT>
    void mark_edges_to_refine(SrcMeshT const & src_mesh, DstMeshT & dst_mesh,
                              PointT const & hyperplane_point, PointT const & hyperplane_normal,
                              LineRefinementTagContainerT & line_refinement_tag_accessor,
                              LineRefinementVertexHandleContainerT & line_refinement_vertex_handle_accessor)
    {
      typedef typename viennagrid::result_of::coord<SrcMeshT>::type NumericType;
      typedef typename viennagrid::result_of::const_line_range<SrcMeshT>::type ConstLineRangeType;
      typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;

      ConstLineRangeType lines(src_mesh);
      for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
      {
        NumericType distance0 = viennagrid::inner_prod( hyperplane_normal, viennagrid::point( viennagrid::vertices(*lit)[0] )-hyperplane_point );
        NumericType distance1 = viennagrid::inner_prod( hyperplane_normal, viennagrid::point( viennagrid::vertices(*lit)[1] )-hyperplane_point );

        NumericType tolerance = 1e-8 * viennagrid::volume(*lit);

        if (distance0 > distance1)
          std::swap(distance0, distance1);

        if (distance0 < -tolerance && distance1 > tolerance)
        {
          line_refinement_tag_accessor(*lit) = true;

          PointT const & pt0 = viennagrid::point( viennagrid::vertices(*lit)[0] );
          PointT const & pt1 = viennagrid::point( viennagrid::vertices(*lit)[1] );

          double qd = viennagrid::inner_prod( hyperplane_normal, pt0-hyperplane_point );
          double pd = viennagrid::inner_prod( hyperplane_normal, pt1-hyperplane_point );

          PointT new_pt = pt1 - (pd / (pd-qd)) * (pt1 - pt0);
          line_refinement_vertex_handle_accessor(*lit) = viennagrid::make_unique_vertex( dst_mesh, new_pt );
        }
        else
          line_refinement_tag_accessor(*lit) = false;
      }
    }




    template<typename SrcMeshT, typename SrcSegmentationT, typename DstMeshT, typename DstSegmentationT, typename PointT>
    void hyperplane_clip(SrcMeshT const & src_mesh, SrcSegmentationT const & src_segmentation,
                         DstMeshT & dst_mesh, DstSegmentationT & dst_segmentation,
                         PointT const & hyperplane_point, PointT const & hyperplane_normal)
    {
      typedef typename viennagrid::result_of::cell_tag<SrcMeshT>::type CellTag;
      typedef typename viennagrid::result_of::vertex<SrcMeshT>::type VertexType;
      typedef typename viennagrid::result_of::line<SrcMeshT>::type LineType;
      typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type DstMeshVertexHandleType;
      typedef typename viennagrid::result_of::point<SrcMeshT>::type PointType;

      viennagrid::vertex_copy_map<SrcMeshT, DstMeshT> vertex_map( dst_mesh );

      std::deque<bool> line_refinement_tag_container;
      typename viennagrid::result_of::accessor<std::deque<bool>, LineType>::type line_refinement_tag_accessor(line_refinement_tag_container);

      std::deque<DstMeshVertexHandleType> line_refinement_vertex_handle_container;
      typename viennagrid::result_of::accessor<std::deque<DstMeshVertexHandleType>, LineType>::type line_refinement_vertex_handle_accessor(line_refinement_vertex_handle_container);

      mark_edges_to_refine(src_mesh, dst_mesh, hyperplane_point, hyperplane_normal, line_refinement_tag_accessor, line_refinement_vertex_handle_accessor);


      viennagrid::simple_refine<CellTag>(src_mesh, src_segmentation,
                                         dst_mesh, dst_segmentation,
                                         line_refinement_tag_accessor,
                                         vertex_map,
                                         line_refinement_vertex_handle_accessor);
    }



    template<typename SrcMeshT, typename DstMeshT, typename PointT>
    void hyperplane_clip(SrcMeshT const & src_mesh, DstMeshT & dst_mesh,
                         PointT const & hyperplane_point, PointT const & hyperplane_normal)
    {
      typedef typename viennagrid::result_of::cell_tag<SrcMeshT>::type CellTag;
      typedef typename viennagrid::result_of::vertex<SrcMeshT>::type VertexType;
      typedef typename viennagrid::result_of::line<SrcMeshT>::type LineType;

      typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type DstMeshVertexHandleType;

      viennagrid::vertex_copy_map<SrcMeshT, DstMeshT> vertex_map( dst_mesh );

      std::deque<bool> line_refinement_tag_container;
      typename viennagrid::result_of::accessor<std::deque<bool>, LineType>::type line_refinement_tag_accessor(line_refinement_tag_container);

      std::deque<DstMeshVertexHandleType> line_refinement_vertex_handle_container;
      typename viennagrid::result_of::accessor<std::deque<DstMeshVertexHandleType>, LineType>::type line_refinement_vertex_handle_accessor(line_refinement_vertex_handle_container);

      mark_edges_to_refine(src_mesh, dst_mesh, hyperplane_point, hyperplane_normal, line_refinement_tag_accessor, line_refinement_vertex_handle_accessor);

      viennagrid::simple_refine<CellTag>(src_mesh, dst_mesh,
                                         line_refinement_tag_accessor,
                                         vertex_map,
                                         line_refinement_vertex_handle_accessor);
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
            SegmentedMeshType tmp;

            hyperplane_clip(input_mesh().mesh, input_mesh().segmentation,
                            tmp.mesh, tmp.segmentation,
                            hyperplane_point, hyperplane_normal );

            viennagrid::copy( tmp.mesh, tmp.segmentation,
                              output_mesh().mesh, output_mesh().segmentation,
                              on_positive_hyperplane_side_functor<PointType>(hyperplane_point, -hyperplane_normal) );

            return true;
          }
        }

        {
          typename viennamesh::result_of::const_parameter_handle<MeshT>::type input_mesh = get_input<MeshT>("default");
          if (input_mesh)
          {
            output_parameter_proxy<MeshT> output_mesh = output_proxy<MeshT>( "default" );
            MeshT tmp;

            hyperplane_clip(input_mesh(), tmp, hyperplane_point, hyperplane_normal );
            viennagrid::copy( tmp, output_mesh(),
                              on_positive_hyperplane_side_functor<PointType>(hyperplane_point, -hyperplane_normal) );

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


        unsigned int mesh_geometric_dimension = lexical_cast<unsigned int>( mesh->get_property("geometric_dimension").first );

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

        if (generic_run<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>(base_hyperplane_point(), base_hyperplane_normal()))
          return true;

        if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>(base_hyperplane_point(), base_hyperplane_normal()))
          return true;


        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }

    private:
    };

  }
}

#endif
