#include "viennamesh/algorithm/seed_point_segmenting.hpp"

#include "viennagrid/mesh/segmentation.hpp"
#include "viennagrid/mesh/coboundary_iteration.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/geometry.hpp"

#include "viennamesh/algorithm/intersect.hpp"

namespace viennamesh
{

  template<typename IteratorT>
  IteratorT circular_next(IteratorT it, IteratorT const & begin, IteratorT const & end)
  {
    if (it == end)
      it = begin;

    ++it;
    if (it == end)
      return begin;
    return it;
  }

  template<typename IteratorT>
  IteratorT circular_prev(IteratorT it, IteratorT begin, IteratorT end)
  {
    if (it == begin)
      it = end;

    --it;
    return it;
  }


  template<typename MeshT, typename SegmentHandleT, typename VisitedStateAccessorT, typename TriangleHandleT>
  void mark_facing_shortes_angle( MeshT & mesh_obj, SegmentHandleT & segment, VisitedStateAccessorT visited_state_accessor, TriangleHandleT triangle_handle, bool triangle_faces_outward )
  {
    typedef typename viennagrid::result_of::point<MeshT>::type          PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type      CoordType;

    typedef typename viennagrid::result_of::line<MeshT>::type           LineType;
    typedef typename viennagrid::result_of::triangle<MeshT>::type       TriangleType;
    typedef typename viennagrid::result_of::vertex_handle<MeshT>::type  VertexHandleType;

    typedef viennagrid::static_array<VertexHandleType, 3> VertexHandleContainerType;

    TriangleType & triangle = viennagrid::dereference_handle(segment, triangle_handle);

    // if this triangle already is in the segment -> nothing to do
    if (visited_state_accessor(triangle))
      return;
//     if (viennagrid::is_in_segment(segment, triangle))
//       return;


//     if (!viennagrid::segment_ids(segment.parent(), triangle).empty())
//       return;

    // adding the triangle and its orientation to the segment
    {
      viennagrid::add( segment, triangle );
      *viennagrid::segment_element_info( segment, triangle ) = triangle_faces_outward;
    }

    // ... and set the visited flag
    visited_state_accessor(triangle) = true;

    VertexHandleContainerType vtx;
    vtx[0] = viennagrid::vertices(triangle).handle_at(0);
    vtx[1] = viennagrid::vertices(triangle).handle_at(1);
    vtx[2] = viennagrid::vertices(triangle).handle_at(2);

    viennagrid::static_array<PointType,3> p;
    p[0] = viennagrid::point( viennagrid::vertices(triangle)[0] );
    p[1] = viennagrid::point( viennagrid::vertices(triangle)[1] );
    p[2] = viennagrid::point( viennagrid::vertices(triangle)[2] );

    // calculate the center of the triangle
    PointType center = viennagrid::centroid(triangle);

    // ... and its normal
    PointType normal = viennagrid::normal_vector(triangle);
    // correcting the normal direction
    if (!triangle_faces_outward)
      normal = -normal;
    // normalizing the normal vector
    normal /= viennagrid::norm_2(normal);

    typedef typename viennagrid::result_of::line_range<TriangleType>::type             LinesOnTriangleRangeType;
    typedef typename viennagrid::result_of::iterator<LinesOnTriangleRangeType>::type    LinesOnTriangleIteratorType;

    // iterating over all boundary lines
    LinesOnTriangleRangeType lines( triangle );
    for ( LinesOnTriangleIteratorType lit = lines.begin(); lit != lines.end(); ++lit )
    {
      LineType & line = *lit;

      viennagrid::static_array<PointType, 2> lp;

      lp[0] = viennagrid::point( viennagrid::vertices(line)[0] );
      lp[1] = viennagrid::point( viennagrid::vertices(line)[1] );


      // calculating the line vector
      PointType line_vector = lp[1]-lp[0];
      // ... and normalizing it
      line_vector /= viennagrid::norm_2(line_vector);

      // calculating the center of the boundary line
      PointType line_center = (lp[0]+lp[1]) / 2.0;
      // calculate the vector facing towards the current triangle
      PointType line_to_triangle_vector = viennagrid::cross_prod( -normal, line_vector );
      // ... and normalizing it
      line_to_triangle_vector /= viennagrid::norm_2(line_to_triangle_vector);

      // check and correnct the orentietion of the vector facing towards the current triangle
      if (viennagrid::inner_prod(line_to_triangle_vector, center - line_center) < 0)
        line_to_triangle_vector = -line_to_triangle_vector;

      typedef typename viennagrid::result_of::coboundary_range<MeshT, viennagrid::line_tag, viennagrid::triangle_tag>::type   CoboundaryRangeType;
      typedef typename viennagrid::result_of::iterator<CoboundaryRangeType>::type                                               CoboundaryIteratorType;

      CoboundaryRangeType coboundary_triangles(mesh_obj, lit.handle());

      // smallest angle, triangle with smallest angle and facing outward flag of this triangle
      CoordType smallest_angle = std::numeric_limits<CoordType>::max();
      TriangleHandleT smallest_angle_triangle;
      bool smallest_angle_triangle_faces_outward;

      // iterating over all coboundary triangles of the current line
      for (CoboundaryIteratorType it = coboundary_triangles.begin(); it != coboundary_triangles.end(); ++it)
      {
        TriangleHandleT handle = it.handle();
        TriangleType & neighbor_triangle = *it;

        // is the coboundary triangle the current triangle -> skipping
        if (handle == triangle_handle)
            continue;

        VertexHandleContainerType nvtx;
        nvtx[0] = viennagrid::elements<viennagrid::vertex_tag>(neighbor_triangle).handle_at(0);
        nvtx[1] = viennagrid::elements<viennagrid::vertex_tag>(neighbor_triangle).handle_at(1);
        nvtx[2] = viennagrid::elements<viennagrid::vertex_tag>(neighbor_triangle).handle_at(2);

        viennagrid::static_array<PointType,3> np;
        np[0] = viennagrid::point( mesh_obj, nvtx[0] );
        np[1] = viennagrid::point( mesh_obj, nvtx[1] );
        np[2] = viennagrid::point( mesh_obj, nvtx[2] );

        // calculating the center of the neighbor triangle
        PointType neighbor_center = (np[0]+np[1]+np[2])/3.0;
        // calculating the vector from the line center towards the neighbor triangle
        PointType line_to_neighbor_triangle_vector = neighbor_center - line_center;
        // ... and normalizing it
        line_to_neighbor_triangle_vector /= viennagrid::norm_2(line_to_neighbor_triangle_vector);

        // projecting the vector facing to the neighbor triangle onto the 2D coordinate system
        CoordType x = viennagrid::inner_prod( line_to_triangle_vector, line_to_neighbor_triangle_vector );
        CoordType y = viennagrid::inner_prod( -normal, line_to_neighbor_triangle_vector );

        // normalizing the 2D vector
        CoordType tmp = std::sqrt( x*x + y*y );
        x /= tmp;
        y /= tmp;



        // calculate the angle
        CoordType angle_center_vectors = std::acos( x );
        // calculate the corrected oriented angle; if y < 0 the angle has to be corrected
        CoordType oriented_angle = (y > 0) ? angle_center_vectors : 2.0*M_PI - angle_center_vectors;

        // if the current angle is smaller than the best -> yay! better choice found
        if (oriented_angle < smallest_angle)
        {
          smallest_angle          = oriented_angle;
          smallest_angle_triangle = handle;

          typename VertexHandleContainerType::iterator vtx_it;
          typename VertexHandleContainerType::iterator nvtx_it;

          for (vtx_it = vtx.begin(); vtx_it != vtx.end(); ++vtx_it)
          {
            nvtx_it = std::find(nvtx.begin(), nvtx.end(), *vtx_it);
            if (nvtx_it != nvtx.end())
              break;
          }


          // topology check
          if ( ( *circular_next(vtx_it, vtx.begin(), vtx.end()) == *circular_next(nvtx_it, nvtx.begin(), nvtx.end()) ) ||
                ( *circular_prev(vtx_it, vtx.begin(), vtx.end()) == *circular_prev(nvtx_it, nvtx.begin(), nvtx.end()) ) )
          {
            smallest_angle_triangle_faces_outward = !triangle_faces_outward;

          }
          else if ( ( *circular_next(vtx_it, vtx.begin(), vtx.end()) == *circular_prev(nvtx_it, nvtx.begin(), nvtx.end()) ) ||
                    ( *circular_prev(vtx_it, vtx.begin(), vtx.end()) == *circular_next(nvtx_it, nvtx.begin(), nvtx.end()) ) )
          {
            smallest_angle_triangle_faces_outward = triangle_faces_outward;
          }
          else
          {
            std::cout << "Something went wrong..." << std::endl;
          }
        }
      }

      // is a triangle found -> call mark_facing_shortes_angle recursively
      if (smallest_angle != std::numeric_limits<CoordType>::max())
      {
        mark_facing_shortes_angle(mesh_obj, segment, visited_state_accessor, smallest_angle_triangle, smallest_angle_triangle_faces_outward);
      }

    }
  }




  template<typename MeshT, typename SegmentHandleT, typename PointT, typename NumericConfigT>
  void detect_and_mark_face_segment( MeshT & mesh, SegmentHandleT & segment, PointT const & seed_point, NumericConfigT numeric_config )
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type CoordType;

    typedef typename viennagrid::result_of::element<MeshT, viennagrid::triangle_tag>::type TriangleType;
    typedef typename viennagrid::result_of::handle<MeshT, viennagrid::triangle_tag>::type  TriangleHandleType;
    typedef typename viennagrid::result_of::element_range<MeshT, viennagrid::triangle_tag>::type TriangleRangeType;
    typedef typename viennagrid::result_of::iterator<TriangleRangeType>::type TriangleIteratorType;

//     std::pair<PointType, PointType> bounding_box = viennagrid::bounding_box(mesh);
//     PointType outside_point = bounding_box.first - bounding_box.second;

    std::vector<bool> visited_state( viennagrid::id_upper_bound<TriangleType>(mesh).get()+1, false );
    typename viennagrid::result_of::accessor< std::vector<bool>, TriangleType >::type visited_state_accessor = viennagrid::make_accessor<TriangleType>(visited_state);

    // iterating over all triangles
    TriangleRangeType triangles(mesh);
    for (TriangleIteratorType it = triangles.begin(); it != triangles.end(); ++it)
    {
      TriangleType const & triangle = *it;

      // has this triangle already been visited? -> skipping
      if (visited_state_accessor(triangle))
        continue;

      // if this triangle already is in a segment -> skipping
//       if (!viennagrid::segment_ids(segmentation, triangle).empty())
//       {
//         typedef typename viennagrid::result_of::segment_id_range<SegmentationT, TriangleType>::type SegmentIDRangeType;
//         SegmentIDRangeType ids = viennagrid::segment_ids(segmentation, triangle);
//         int s = viennagrid::segment_ids(segmentation, triangle).size();
//         continue;
//       }

      // calculating the center of the triangle
      PointType r = viennagrid::centroid(triangle);

      // calculating the normal vector of the triangle
      PointType n = viennagrid::normal_vector(triangle);
      // ... and normalizing it
      n /= viennagrid::norm_2(n);

      // calculating the ray vector from the center of the triangle to the seed point
      PointType d = seed_point - r;

      // projecting the normalized ray vector onto the normal vector
      CoordType p = viennagrid::inner_prod( d, n ) / viennagrid::norm_2(d);

      // if the projection is near zero (happens when the ray vector and the triangle are co-linear) -> skip this triangle
      if ( std::abs(p) < viennagrid::detail::absolute_tolerance<CoordType>(numeric_config) )
        continue;

      // if the projection if negative, the triangle faces outward
      bool faces_outward = p < 0;

      // iterating over all triangles which are not yet in any segment and check for intersection
      TriangleIteratorType jt = triangles.begin();
      for (; jt != triangles.end(); ++jt)
      {
        if (it == jt) continue; // no self intersect test

        TriangleType const & to_test = *jt;

        // if triangle to_test already is in a segment -> skip intersection test
//         if (!viennagrid::segment_ids(segmentation, to_test).empty())
//           continue;

        // in case of intersection -> nothing to do with the triangle
        if (viennagrid::element_line_intersect(to_test, r, seed_point, numeric_config))
          break;
      }

      // if there was no intersection -> mark this triangle and all neighbor triangles recursively
      if (jt == triangles.end())
      {
//         if (!faces_outward)
//           n = -n;
//         typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentHandleType;
//         SegmentHandleType segment = segmentation.make_segment();

        mark_facing_shortes_angle( mesh, segment, visited_state_accessor, it.handle(), faces_outward );
        return;
      }
    }
  }


  /** @brief This function detects and marks a face segment based on multiple seed points */
  template< typename MeshT, typename SegmentationT, typename SeedPointIteratorT, typename NumericConfigT >
  void mark_face_segments( MeshT & mesh, SegmentationT & segmentation_, SeedPointIteratorT seed_points_it, SeedPointIteratorT const & seed_points_end, NumericConfigT numeric_config )
  {
    for ( ; seed_points_it != seed_points_end; ++seed_points_it)
    {
      typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentHandleType;
      SegmentHandleType segment = segmentation_(seed_points_it->second);
      detect_and_mark_face_segment(mesh, segment, seed_points_it->first, numeric_config);
    }
  }


  seed_point_segmenting::seed_point_segmenting() :
    input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented and non-segmented triangular 3d supported")),
    input_seed_points(*this, parameter_information("seed_points","seed_point_3d_container","The seed points for segment identification")),
    output_mesh(*this, parameter_information("mesh","mesh","The output mesh, segmented oriented triangular 3d")) {}

  std::string seed_point_segmenting::name() const { return "ViennaGrid Hull Segmenting"; }
  std::string seed_point_segmenting::id() const { return "seed_point_segmenting"; }


  bool seed_point_segmenting::run_impl()
  {
    typedef viennagrid::triangular_3d_mesh          InputMeshType;
    typedef viennagrid::triangular_3d_segmentation  InputSegmentationType;
    typedef viennagrid::segmented_mesh<InputMeshType, InputSegmentationType> InputSegmentedMeshType;

    typedef viennagrid::triangular_3d_mesh                OutputMeshType;
    typedef viennagrid::triangular_hull_3d_segmentation   OutputSegmentationType;
    typedef viennagrid::segmented_mesh<OutputMeshType, OutputSegmentationType> OutputSegmentedMeshType;

    {
      viennamesh::result_of::const_parameter_handle<InputSegmentedMeshType>::type imp = input_mesh.get<InputSegmentedMeshType>();
      if (imp)
      {
        output_parameter_proxy<OutputSegmentedMeshType> omp(output_mesh);

        viennagrid::vertex_copy_map<InputMeshType, OutputMeshType> vertex_map( omp().mesh );
        typedef viennagrid::result_of::const_cell_range<InputMeshType>::type ConstInputCellRangeType;
        typedef viennagrid::result_of::iterator<ConstInputCellRangeType>::type ConstInputCellIteratorType;

        ConstInputCellRangeType cells( imp().mesh );
        for (ConstInputCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
          vertex_map.copy_element(*cit);

        mark_face_segments(omp().mesh, omp().segmentation, input_seed_points().begin(), input_seed_points().end(), 1e-8);
        return true;
      }
    }

    {
      viennamesh::result_of::const_parameter_handle<InputMeshType>::type imp = input_mesh.get<InputMeshType>();
      if (imp)
      {
        output_parameter_proxy<OutputSegmentedMeshType> omp(output_mesh);

        viennagrid::vertex_copy_map<InputMeshType, OutputMeshType> vertex_map( omp().mesh );
        typedef viennagrid::result_of::const_cell_range<InputMeshType>::type ConstInputCellRangeType;
        typedef viennagrid::result_of::iterator<ConstInputCellRangeType>::type ConstInputCellIteratorType;

        ConstInputCellRangeType cells( imp() );
        for (ConstInputCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
          vertex_map.copy_element(*cit);

        mark_face_segments(omp().mesh, omp().segmentation, input_seed_points().begin(), input_seed_points().end(), 1e-8);
        return true;
      }
    }

    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }

}
