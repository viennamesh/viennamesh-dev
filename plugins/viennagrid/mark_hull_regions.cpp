#include "mark_hull_regions.hpp"

#include "viennagridpp/algorithm/geometry.hpp"
#include "viennagridpp/algorithm/centroid.hpp"
#include "viennagridpp/algorithm/intersect.hpp"
#include <boost/array.hpp>

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





  template<typename Element1T, typename Element2T>
  bool same_orientation(Element1T const & lhs, Element2T const & rhs)
  {
    boost::array<Element1T, 3> lhs_vtx;
    boost::array<Element2T, 3> rhs_vtx;

    for (int i = 0; i != 3; ++i)
    {
      lhs_vtx[i] = viennagrid::vertices(lhs)[i];
      rhs_vtx[i] = viennagrid::vertices(rhs)[i];
    }

    // lhs_vtx[0] and lhs_vtx[1] are shared
    if ( ((lhs_vtx[0] == rhs_vtx[0]) && (lhs_vtx[1] == rhs_vtx[2])) ||
         ((lhs_vtx[0] == rhs_vtx[1]) && (lhs_vtx[1] == rhs_vtx[0])) ||
         ((lhs_vtx[0] == rhs_vtx[2]) && (lhs_vtx[1] == rhs_vtx[1])) )
      return true;

    // lhs_vtx[1] and lhs_vtx[2] are shared
    if ( ((lhs_vtx[1] == rhs_vtx[0]) && (lhs_vtx[2] == rhs_vtx[2])) ||
         ((lhs_vtx[1] == rhs_vtx[1]) && (lhs_vtx[2] == rhs_vtx[0])) ||
         ((lhs_vtx[1] == rhs_vtx[2]) && (lhs_vtx[2] == rhs_vtx[1])) )
      return true;

    // lhs_vtx[2] and lhs_vtx[0] are shared
    if ( ((lhs_vtx[2] == rhs_vtx[0]) && (lhs_vtx[0] == rhs_vtx[2])) ||
         ((lhs_vtx[2] == rhs_vtx[1]) && (lhs_vtx[0] == rhs_vtx[0])) ||
         ((lhs_vtx[2] == rhs_vtx[2]) && (lhs_vtx[0] == rhs_vtx[1])) )
      return true;

    return false;
  }



  template<typename MeshT, typename TriangleT, typename RegionAccessorT>
  void mark_smalled_neighbor_triangles( MeshT const & mesh,
                                        TriangleT const & triangle,
                                        RegionAccessorT & pos_orient,
                                        RegionAccessorT & neg_orient,
                                        int region_id )
  {
    typedef typename viennagrid::result_of::point<MeshT>::type          PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type      CoordType;

    typedef typename viennagrid::result_of::element<MeshT>::type           ElementType;

    bool invert_cell_orientation = false;
    if (pos_orient.get(triangle) == region_id)
      invert_cell_orientation = false;
    else if (neg_orient.get(triangle) == region_id)
      invert_cell_orientation = true;
    else
    {
//       std::cout << "ERROR!! pos nor neg is region_id = " << region_id << "      pos = " << pos_orient.get(triangle) << " neg = " << neg_orient.get(triangle) << std::endl;
    }


    // calculate the center of the triangle
    PointType center = viennagrid::centroid(triangle);

    // ... and its normal
    PointType normal = viennagrid::normal_vector(triangle);
    // correcting the normal direction
    if (invert_cell_orientation)
    {
      normal = -normal;
    }

    // normalizing the normal vector
    normal /= viennagrid::norm_2(normal);

    typedef typename viennagrid::result_of::element_range<TriangleT>::type             LinesOnTriangleRangeType;
    typedef typename viennagrid::result_of::iterator<LinesOnTriangleRangeType>::type    LinesOnTriangleIteratorType;

    // iterating over all boundary lines
    LinesOnTriangleRangeType lines( triangle, 1 );
    for ( LinesOnTriangleIteratorType lit = lines.begin(); lit != lines.end(); ++lit )
    {
      ElementType line = *lit;

      boost::array<PointType, 2> lp;
      lp[0] = viennagrid::get_point( viennagrid::vertices(line)[0] );
      lp[1] = viennagrid::get_point( viennagrid::vertices(line)[1] );

      // calculating the line vector
      PointType line_vector = lp[1]-lp[0];
      // ... and normalizing it
      line_vector /= viennagrid::norm_2(line_vector);

      // calculating the center of the boundary line
      PointType line_center = (lp[0]+lp[1]) / 2.0;
      // calculate the vector facing towards the current triangle
      PointType line_to_triangle_vector = viennagrid::cross_prod( normal, line_vector );
      // ... and normalizing it
      line_to_triangle_vector /= viennagrid::norm_2(line_to_triangle_vector);

      // check and correnct the orentietion of the vector facing towards the current triangle
      if (viennagrid::inner_prod(line_to_triangle_vector, center - line_center) < 0)
        line_to_triangle_vector = -line_to_triangle_vector;

      typedef typename viennagrid::result_of::coboundary_range<MeshT>::type CoboundaryRangeType;
      typedef typename viennagrid::result_of::iterator<CoboundaryRangeType>::type CoboundaryIteratorType;

      CoboundaryRangeType coboundary_triangles(mesh, line, 2);

      // smallest angle, triangle with smallest angle and facing outward flag of this triangle
      CoordType smallest_angle = std::numeric_limits<CoordType>::max();
      TriangleT smallest_angle_triangle;

      // iterating over all coboundary triangles of the current line
      for (CoboundaryIteratorType ctit = coboundary_triangles.begin(); ctit != coboundary_triangles.end(); ++ctit)
      {
        TriangleT neighbor_triangle = *ctit;

        // is the coboundary triangle the current triangle -> skip neighbor triangle
        if (triangle == neighbor_triangle)
        {
          continue;
        }

        // triangle already is in region with id region_id -> skip line
        if ((pos_orient.get(neighbor_triangle) == region_id) ||
            (neg_orient.get(neighbor_triangle) == region_id))
        {
          smallest_angle = std::numeric_limits<CoordType>::max();
          break;
        }

        // triangle already is in 2 regions -> skip neighbor triangle
        if ((pos_orient.get(neighbor_triangle) != -1) &&
            (neg_orient.get(neighbor_triangle) != -1))
        {
          continue;
        }

        // calculating the center of the neighbor triangle
        PointType neighbor_center = viennagrid::centroid(neighbor_triangle); // (np[0]+np[1]+np[2])/3.0;

        // calculating the vector from the line center towards the neighbor triangle
        PointType line_to_neighbor_triangle_vector = neighbor_center - line_center;
        // ... and normalizing it
        line_to_neighbor_triangle_vector /= viennagrid::norm_2(line_to_neighbor_triangle_vector);

        // projecting the vector facing to the neighbor triangle onto the 2D coordinate system
        CoordType x = viennagrid::inner_prod( line_to_triangle_vector, line_to_neighbor_triangle_vector );
        CoordType y = viennagrid::inner_prod( normal, line_to_neighbor_triangle_vector );

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
          smallest_angle_triangle = neighbor_triangle;
        }
      }

      // is a triangle found -> call mark_facing_shortes_angle recursively
      if (smallest_angle != std::numeric_limits<CoordType>::max())
      {
        if ( same_orientation(triangle, smallest_angle_triangle) == invert_cell_orientation)    // XOR
          neg_orient.set(smallest_angle_triangle, region_id);
        else
          pos_orient.set(smallest_angle_triangle, region_id);


        mark_smalled_neighbor_triangles(mesh,
                                        smallest_angle_triangle,
                                        pos_orient,
                                        neg_orient,
                                        region_id);
      }

    }
  }






  template<typename MeshT, typename RegionAccessorT, typename NumericConfigT>
  void mark_outer_hull(MeshT const & mesh,
                       RegionAccessorT & pos_orient,
                       RegionAccessorT & neg_orient,
                       NumericConfigT numeric_config)
  {
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<MeshT>::type CoordType;

    std::pair<PointType, PointType> bb = viennagrid::bounding_box(mesh);
    PointType outside_point = bb.first - viennagrid::make_point(1,1,1) * viennagrid::norm_2(bb.first-bb.second) * 0.1;

    typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

    ConstCellRangeType cells(mesh);
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      // calculating the center of the triangle
      PointType r = viennagrid::centroid(*cit);

      // calculating the normal vector of the triangle
      PointType n = viennagrid::normal_vector(*cit);
      // ... and normalizing it
      n /= viennagrid::norm_2(n);

      // calculating the ray vector from the center of the triangle to the seed point
      PointType d = outside_point - r;

      // projecting the normalized ray vector onto the normal vector
      CoordType p = viennagrid::inner_prod( d, n ) / viennagrid::norm_2(d);

      // if the projection is near zero (happens when the ray vector and the triangle are co-linear) -> skip this triangle
      if ( std::abs(p) < viennagrid::detail::absolute_tolerance<CoordType>(numeric_config) )
        continue;


      ConstCellRangeIterator cit2 = cells.begin();
      for (; cit2 != cells.end(); ++cit2)
      {
        // no self intersect test
        if (*cit == *cit2)
          continue;

        // in case of intersection -> nothing to do with the triangle
        if (viennagrid::element_line_intersect(*cit2, r, outside_point, numeric_config))
          break;
      }

      // if there was no intersection -> mark this triangle and all neighbor triangles recursively
      if (cit2 == cells.end())
      {
        if ( p > 0 )
          pos_orient.set(*cit, 0);
        else
          neg_orient.set(*cit, 0);

        mark_smalled_neighbor_triangles(mesh,
                                        *cit,
                                        pos_orient,
                                        neg_orient,
                                        0);

        break;
      }
    }
  }









  template<typename MeshT, typename RegionAccessorT>
  int mark_hulls(MeshT const & mesh,
                 RegionAccessorT & pos_orient, RegionAccessorT & neg_orient,
                 int start_region_id = 1)
  {
    int region_id = start_region_id;

    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<MeshT>::type CoordType;

    typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

    ConstCellRangeType cells(mesh);
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      // triangle already is in two regions
      if ( (pos_orient.get(*cit) != -1) && (neg_orient.get(*cit) != -1) )
        continue;

      if (pos_orient.get(*cit) == -1)
        pos_orient.set(*cit, region_id);
      else
        neg_orient.set(*cit, region_id);

      mark_smalled_neighbor_triangles(mesh,
                                      *cit,
                                      pos_orient,
                                      neg_orient,
                                      region_id);

      ++region_id;
    }

    return region_id-1;
  }









  template<typename MeshT>
  bool check_orientation(MeshT const & mesh)
  {
    typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

    typedef typename viennagrid::result_of::element<MeshT>::type ElementType;

    ConstCellRangeType cells(mesh);
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      typedef typename viennagrid::result_of::const_neighbor_range<MeshT>::type ConstNeighborRangeType;
      typedef typename viennagrid::result_of::iterator<ConstNeighborRangeType>::type ConstNeighborRangeIterator;

      ConstNeighborRangeType neighbor_cells(mesh, *cit, 1, 2);
      for (ConstNeighborRangeIterator ncit = neighbor_cells.begin(); ncit != neighbor_cells.end(); ++ncit)
      {
        if ( !same_orientation(*cit, *ncit) )
          return false;
      }
    }

    return true;
  }






  template<typename MeshT, typename ElementT>
  int positive_orientation_region(MeshT const & mesh, ElementT const & element)
  {
    typedef typename viennagrid::result_of::region_range<MeshT, ElementT>::type ElementRegionRangeType;
    typedef typename viennagrid::result_of::iterator<ElementRegionRangeType>::type ElementRegionRangeIterator;

    int smallest_region = -1;
    ElementRegionRangeType regions(mesh, element);
    for (ElementRegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      if ((smallest_region == -1) || ((*rit).id() < smallest_region))
        smallest_region = (*rit).id();
    }

    return smallest_region;
  }









  mark_hull_regions::mark_hull_regions() {}
  std::string mark_hull_regions::name() { return "mark_hull_regions"; }

  bool mark_hull_regions::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::region<MeshType>::type RegionType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;


    std::vector<int> positive_orientation_regions( viennagrid::cells(input_mesh()).size(), -1 );
    viennagrid::result_of::accessor< std::vector<int>, ElementType >::type pos_orient(positive_orientation_regions);

    std::vector<int> negative_orientation_regions( viennagrid::cells(input_mesh()).size(), -1 );
    viennagrid::result_of::accessor< std::vector<int>, ElementType >::type neg_orient(negative_orientation_regions);

    mark_outer_hull( input_mesh(), pos_orient, neg_orient, 1e-6);
    int region_count = mark_hulls( input_mesh(), pos_orient, neg_orient, 1 );
    std::cout << "Region count = " << region_count << std::endl;




    mesh_handle output_mesh = make_data<mesh_handle>();

    std::vector<RegionType> output_regions;
    for (int i = 0; i != region_count; ++i)
      output_regions.push_back( output_mesh().get_make_region(i) );


    typedef viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

    typedef viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
    typedef viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;


    typedef viennagrid::result_of::element_copy_map<>::type CopyMapType;
    CopyMapType cm( output_mesh(), false );


    ConstVertexRangeType vertices( input_mesh() );
    for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
      cm(*vit);

    ConstCellRangeType cells( input_mesh() );
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      int por = pos_orient.get(*cit);
      int nor = neg_orient.get(*cit);

      if (por == 0 && nor == 0)
      {
        std::cout << "ERROR" << std::endl;
        continue;
      }


      if (nor == 0)
      {
        ElementType new_cell = cm(*cit);
        viennagrid::add( output_regions[por-1], new_cell );
      }
      else if (por == 0)
      {
        ElementType new_cell = viennagrid::make_triangle( output_mesh(),
                                                          cm( viennagrid::vertices(*cit)[0] ),
                                                          cm( viennagrid::vertices(*cit)[2] ),
                                                          cm( viennagrid::vertices(*cit)[1] ) );
        viennagrid::add( output_regions[nor-1], new_cell );
      }
      else
      {
        if (por < nor)
        {
          ElementType new_cell = cm(*cit);
          viennagrid::add( output_regions[por-1], new_cell );
          viennagrid::add( output_regions[nor-1], new_cell );
        }
        else
        {
          ElementType new_cell = viennagrid::make_triangle( output_mesh(),
                                                            cm( viennagrid::vertices(*cit)[0] ),
                                                            cm( viennagrid::vertices(*cit)[2] ),
                                                            cm( viennagrid::vertices(*cit)[1] ) );
          viennagrid::add( output_regions[por-1], new_cell );
          viennagrid::add( output_regions[nor-1], new_cell );
        }
      }
    }






    typedef viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
    typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

    RegionRangeType regions( output_mesh() );
    for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      std::cout << "Checking region " << (*rit).id() << std::endl;
      MeshType mesh;
      CopyMapType tmpcm( mesh, false );

      typedef viennagrid::result_of::const_vertex_range<RegionType>::type ConstRegionVertexRangeType;
      typedef viennagrid::result_of::iterator<ConstRegionVertexRangeType>::type ConstRegionVertexRangeIterator;

      typedef viennagrid::result_of::const_cell_range<RegionType>::type ConstRegionCellRangeType;
      typedef viennagrid::result_of::iterator<ConstRegionCellRangeType>::type ConstRegionCellRangeIterator;


      ConstRegionVertexRangeType vertices( *rit );
      for (ConstRegionVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
        cm(*vit);


      ConstRegionCellRangeType cells( *rit );
      for (ConstRegionCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
        int por = positive_orientation_region(output_mesh(), *cit);

        if (por == (*rit).id())
        {
          tmpcm(*cit);
        }
        else
        {
          viennagrid::make_triangle( mesh,
                                     tmpcm( viennagrid::vertices(*cit)[0] ),
                                     tmpcm( viennagrid::vertices(*cit)[2] ),
                                     tmpcm( viennagrid::vertices(*cit)[1] ) );
        }
      }

      if (!check_orientation(mesh))
        std::cout << "Error checking orientation of region " << (*rit).id() << std::endl;
    }








    set_output("mesh", output_mesh);
    return true;
  }

}




