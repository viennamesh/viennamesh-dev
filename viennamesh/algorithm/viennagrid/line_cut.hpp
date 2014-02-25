#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_LINE_CUT_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_LINE_CUT_HPP

#include "viennagrid/algorithm/angle.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennamesh/core/algorithm.hpp"



namespace viennamesh
{

  namespace line_cut
  {


    class secont_segment_id_provider
    {
    public:

      secont_segment_id_provider(int highest_segment_id_) : highest_segment_id(highest_segment_id_) {}

      int operator() (int segment_id)
      {
        std::map<int, int>::iterator second_segment_id_it = second_segment_id_map.find(segment_id);
        if (second_segment_id_it != second_segment_id_map.end())
          return second_segment_id_it->second;
        else
          return (second_segment_id_map[segment_id] = highest_segment_id++);
      }

    private:
      int highest_segment_id;
      std::map<int, int> second_segment_id_map;
    };


    template<typename SrcMeshT, typename SrcSegmentationT, typename DstMeshT, typename DstSegmentationT, typename PointT>
    void line_cut( SrcMeshT const & src_mesh, SrcSegmentationT const & src_segmentation,
                   DstMeshT & dst_mesh, DstSegmentationT & dst_segmentation,
                   PointT const & ray_pt, PointT const & ray_dir)
    {
      typedef typename viennagrid::result_of::segment_id<SrcSegmentationT>::type SegmentIDType;

      typedef typename viennagrid::result_of::const_line_range<SrcMeshT>::type ConstLineRangeType;
      typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;

      typedef typename viennagrid::result_of::line<SrcMeshT>::type SrcLineType;
      typedef typename viennagrid::result_of::point<SrcMeshT>::type SrcMeshPointType;
      typedef typename viennagrid::result_of::coord<SrcMeshPointType>::type SrcMeshCoordType;

      viennagrid::vertex_copy_map<SrcMeshT, DstMeshT> vertex_map(dst_mesh);

      typedef typename viennagrid::result_of::line_handle<DstMeshT>::type DstLineHandleType;
      typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type DstVertexHandleType;

      typedef std::vector<SegmentIDType> SegmentIDContainerType;
      typedef std::map<SrcMeshCoordType, std::pair<DstVertexHandleType, SegmentIDContainerType> > InsertedPointContainerType;
      InsertedPointContainerType inserted_points;


      typedef typename viennagrid::result_of::segment_id_range<SrcSegmentationT, SrcLineType>::type SegmentIDRange;
      secont_segment_id_provider second_segment_id(src_segmentation.size());

      PointT ray_normal;
      ray_normal[0] = -ray_dir[1];
      ray_normal[1] = ray_dir[0];

      ConstLineRangeType lines(src_mesh);
      for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
      {
        SrcMeshPointType const & lp0 = viennagrid::point(viennagrid::vertices(*lit)[0]);
        SrcMeshPointType const & lp1 = viennagrid::point(viennagrid::vertices(*lit)[1]);

        PointT line_dir = lp1-lp0;
        PointT dir_ray_pt_line = lp0 - ray_pt;

        SrcMeshCoordType ray_in_ray = viennagrid::inner_prod(ray_dir, ray_dir);
        SrcMeshCoordType ray_in_line = viennagrid::inner_prod(ray_dir, line_dir);
        SrcMeshCoordType line_in_line = viennagrid::inner_prod(line_dir, line_dir);

        SegmentIDRange segment_ids = viennagrid::segment_ids(src_segmentation, *lit);

        SrcMeshCoordType denominator = ray_in_ray * line_in_line - ray_in_line * ray_in_line;


        if ( std::abs(denominator) < 1e-6 * ray_in_ray * line_in_line )
        {

          if ( std::abs(ray_dir[0] * dir_ray_pt_line[1] - dir_ray_pt_line[0] * ray_dir[1]) < 1e-6 * viennagrid::norm_2(ray_dir) * viennagrid::norm_2(dir_ray_pt_line) )
          {
            // lines are parallel -> no splitting
            DstLineHandleType new_line = viennagrid::make_line(dst_mesh,
                                                              vertex_map(viennagrid::vertices(*lit)[0]),
                                                              vertex_map(viennagrid::vertices(*lit)[1]));

            for (typename SegmentIDRange::const_iterator sit = segment_ids.begin(); sit != segment_ids.end(); ++sit)
            {
              viennagrid::add( dst_segmentation[*sit], new_line );
              viennagrid::add( dst_segmentation[ second_segment_id(*sit) ], new_line );
            }
          }
          else
          {
            // no intersection -> no splitting
            DstLineHandleType new_line = viennagrid::make_line(dst_mesh,
                                  vertex_map(viennagrid::vertices(*lit)[0]),
                                  vertex_map(viennagrid::vertices(*lit)[1]));
            double side = viennagrid::inner_prod(ray_normal, lp0 - ray_pt);

            for (typename SegmentIDRange::const_iterator sit = segment_ids.begin(); sit != segment_ids.end(); ++sit)
            {
              int segment_id = (side > 0) ? *sit : second_segment_id(*sit);
              viennagrid::add( dst_segmentation[segment_id], new_line );
            }
          }
        }
        else
        {
          //Lines are not parallel: Compute minimizers s, t:
          SrcMeshPointType dir_distance = ray_pt - lp0;  //any vector connecting two points on V and W

          SrcMeshCoordType v_in_dir_distance = viennagrid::inner_prod(ray_dir, dir_distance);
          SrcMeshCoordType w_in_dir_distance = viennagrid::inner_prod(line_dir, dir_distance);

          SrcMeshCoordType s = (ray_in_line * w_in_dir_distance - line_in_line * v_in_dir_distance) / denominator;
          SrcMeshCoordType t = (ray_in_ray * w_in_dir_distance - ray_in_line * v_in_dir_distance) / denominator;

          if ( (-1e-6 < t) && (t < 1+1e-6) )
          {
            // line ray intersection -> splitting
            DstVertexHandleType new_pt = viennagrid::make_vertex( dst_mesh, ray_pt+s*ray_dir );
            std::vector<SegmentIDType> segments( segment_ids.size() );
            std::copy( segment_ids.begin(), segment_ids.end(), segments.begin()  );

            inserted_points[s] = std::make_pair(new_pt, segments);

            DstLineHandleType new_line1 = viennagrid::make_line(dst_mesh,
                                  vertex_map(viennagrid::vertices(*lit)[0]),
                                  new_pt);
            double side1 = viennagrid::inner_prod(ray_normal, lp0 - ray_pt);

            DstLineHandleType new_line2 = viennagrid::make_line(dst_mesh,
                                  new_pt,
                                  vertex_map(viennagrid::vertices(*lit)[1]));
            double side2 = viennagrid::inner_prod(ray_normal, lp1 - ray_pt);

            if (side1 && side2)
              std::cout << "Element on both sides, should not happen" << std::endl;

            for (typename SegmentIDRange::const_iterator sit = segment_ids.begin(); sit != segment_ids.end(); ++sit)
            {
              int segment_id1 = (side1 > 0) ? *sit : second_segment_id(*sit);
              viennagrid::add( dst_segmentation[segment_id1], new_line1 );

              int segment_id2 = (side2 > 0) ? *sit : second_segment_id(*sit);
              viennagrid::add( dst_segmentation[segment_id2], new_line2 );
            }
          }
          else
          {
            // no intersection -> no splitting
            DstLineHandleType new_line = viennagrid::make_line(dst_mesh,
                                  vertex_map(viennagrid::vertices(*lit)[0]),
                                  vertex_map(viennagrid::vertices(*lit)[1]));
            double side = viennagrid::inner_prod(ray_normal, lp0 - ray_pt);

            for (typename SegmentIDRange::const_iterator sit = segment_ids.begin(); sit != segment_ids.end(); ++sit)
            {
              int segment_id = (side > 0) ? *sit : second_segment_id(*sit);
              viennagrid::add( dst_segmentation[segment_id], new_line );
            }
          }
        }
      }

      if (inserted_points.empty())
        return;

      if (inserted_points.size() == 1)
      {
        std::cout << "ERROR" << std::endl;
        return;
      }

      typename InsertedPointContainerType::const_iterator ipit0 = inserted_points.begin();
      typename InsertedPointContainerType::const_iterator ipit1 = ipit0; ++ipit1;

      for (; ipit1 != inserted_points.end(); ++ipit0, ++ipit1)
      {
        DstLineHandleType new_line = viennagrid::make_line(dst_mesh, ipit0->second.first, ipit1->second.first);

        for (typename SegmentIDContainerType::const_iterator sit0 = ipit0->second.second.begin(); sit0 != ipit0->second.second.end(); ++sit0)
        {
          for (typename SegmentIDContainerType::const_iterator sit1 = ipit1->second.second.begin(); sit1 != ipit1->second.second.end(); ++sit1)
          {
            if (*sit0 == *sit1)
            {
              viennagrid::add( dst_segmentation[*sit0], new_line );
              viennagrid::add( dst_segmentation[ second_segment_id(*sit0) ], new_line );
            }
          }
        }
      }

    }

    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "ViennaGrid Line Cut"; }

      template<typename MeshT, typename SegmentationT>
      bool generic_run()
      {
        typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

        const int geometric_dimension = viennagrid::result_of::geometric_dimension<MeshT>::value;
        typedef typename viennamesh::result_of::point<geometric_dimension>::type PointType;
        typedef viennamesh::dynamic_point DynamicPointType;

        typename viennamesh::result_of::const_parameter_handle<DynamicPointType>::type base_point = get_required_input<DynamicPointType>("point");
        typename viennamesh::result_of::const_parameter_handle<DynamicPointType>::type base_direction = get_required_input<DynamicPointType>("direction");

        if ((base_point().size() != geometric_dimension) || (base_direction().size() != geometric_dimension))
          return false;

        PointType point;
        PointType direction;

        std::copy( base_point().begin(), base_point().end(), point.begin() );
        std::copy( base_direction().begin(), base_direction().end(), direction.begin() );

        {
          typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type input_mesh = get_input<SegmentedMeshType>("default");
          if (input_mesh)
          {
            output_parameter_proxy<SegmentedMeshType> output_mesh = output_proxy<SegmentedMeshType>( "default" );

            line_cut(input_mesh().mesh, input_mesh().segmentation,
                     output_mesh().mesh, output_mesh().segmentation,
                     point, direction);

            return true;
          }
        }

        {
//           typename viennamesh::result_of::const_parameter_handle<MeshT>::type input_mesh = get_input<MeshT>("default");
//           if (input_mesh)
//           {
//             output_parameter_proxy<MeshT> output_mesh = output_proxy<MeshT>( "default" );
//             output_mesh() = input_mesh();
//             coarsen(output_mesh(), angle);
//
//             return true;
//           }
        }

        return false;
      }

      bool run_impl()
      {
        if (generic_run<viennagrid::line_2d_mesh, viennagrid::line_2d_segmentation>())
          return true;


        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }

    private:
    };

  }
}

#endif
