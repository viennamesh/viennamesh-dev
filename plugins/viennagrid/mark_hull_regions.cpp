#include "mark_hull_regions.hpp"

#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/intersect.hpp"
#include <boost/array.hpp>

namespace viennamesh
{


//   template<typename IteratorT>
//   IteratorT circular_next(IteratorT it, IteratorT const & begin, IteratorT const & end)
//   {
//     if (it == end)
//       it = begin;
//
//     ++it;
//     if (it == end)
//       return begin;
//     return it;
//   }
//
//   template<typename IteratorT>
//   IteratorT circular_prev(IteratorT it, IteratorT begin, IteratorT end)
//   {
//     if (it == begin)
//       it = end;
//
//     --it;
//     return it;
//   }





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


  template<typename MeshT>
  bool check_orientation(MeshT const & mesh)
  {
    typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

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
  int positive_orientation_region(ElementT const & element)
  {
    typedef typename viennagrid::result_of::region_range<ElementT>::type ElementRegionRangeType;
    typedef typename viennagrid::result_of::iterator<ElementRegionRangeType>::type ElementRegionRangeIterator;

    int smallest_region = -1;
    ElementRegionRangeType regions(element);
    for (ElementRegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      if ((smallest_region == -1) || ((*rit).id() < smallest_region))
        smallest_region = (*rit).id();
    }

    return smallest_region;
  }






  template<typename MeshT, typename NeighborsT, typename RegionAccessorT, typename ElementT>
  void mark_neighbors(MeshT const & mesh,
                      std::vector<NeighborsT> const & pos_neighbors,
                      std::vector<NeighborsT> const & neg_neighbors,
                      RegionAccessorT & pos_orient,
                      RegionAccessorT & neg_orient,
                      ElementT triangle,
                      bool positive,
                      int region_id)
  {
    if (positive)
    {
      int rid = pos_orient.get(triangle);
      assert(neg_orient.get(triangle) != region_id);
      if (rid == -1)
      {
        pos_orient.set(triangle, region_id);
      }
      else
      {
        assert(rid == region_id);
        return;
      }
    }
    else
    {
      int rid = neg_orient.get(triangle);
      assert(pos_orient.get(triangle) != region_id);
      if (rid == -1)
      {
        neg_orient.set(triangle, region_id);
      }
      else
      {
        assert(rid == region_id);
        return;
      }
    }


    NeighborsT const & neighbors = positive ? pos_neighbors[triangle.id().index()] : neg_neighbors[triangle.id().index()];
    for (typename NeighborsT::const_iterator ntit = neighbors.begin(); ntit != neighbors.end(); ++ntit)
    {
      mark_neighbors(mesh,
                     pos_neighbors, neg_neighbors,
                     pos_orient, neg_orient,
                     *ntit,
                     same_orientation(triangle, *ntit) == positive,
                     region_id);
    }
  }




  template<typename MeshT, typename RegionAccessorT, typename NumericConfigT>
  int mark_hulls(MeshT const & mesh,
                 RegionAccessorT & pos_orient,
                 RegionAccessorT & neg_orient,
                 NumericConfigT numeric_config)
  {
    typedef typename viennagrid::result_of::coord<MeshT>::type CoordType;
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::element<MeshT>::type ElementType;

    std::vector< std::vector<ElementType> > positive_neighbor_triangles( viennagrid::elements(mesh,2).size() );
    std::vector< std::vector<ElementType> > negative_neighbor_triangles( viennagrid::elements(mesh,2).size() );

    typedef typename viennagrid::result_of::const_element_range<MeshT>::type ConstElementRangeType;
    typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

    typedef typename viennagrid::result_of::const_coboundary_range<MeshT>::type ConstCoboundaryRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCoboundaryRangeType>::type ConstCoboundaryIteratorType;


    ConstElementRangeType lines(mesh, 1);
    for (ConstElementIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
    {
      ConstCoboundaryRangeType triangles(mesh, *lit, 2);

      if (triangles.size() < 2)
      {
        error(1) << "Line " << *lit << " has less than 2 co-boundary triangles" << std::endl;
        for (ConstCoboundaryIteratorType tit = triangles.begin(); tit != triangles.end(); ++tit)
          error(1) << "    " << *tit << std::endl;

        VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Topological error: one line has less than 2 co-boundary triangles");
      }

      if (triangles.size() == 2)
      {
        positive_neighbor_triangles[ triangles[0].id().index() ].push_back( triangles[1] );
        negative_neighbor_triangles[ triangles[0].id().index() ].push_back( triangles[1] );

        positive_neighbor_triangles[ triangles[1].id().index() ].push_back( triangles[0] );
        negative_neighbor_triangles[ triangles[1].id().index() ].push_back( triangles[0] );
        continue;
      }


      ElementType v[2] = { viennagrid::vertices(*lit)[0], viennagrid::vertices(*lit)[1] };
      PointType p[2] = { viennagrid::get_point(v[0]), viennagrid::get_point(v[1]) };

      PointType lp = (p[0]+p[1])/2;
      PointType lv = p[1]-p[0];
      lv.normalize();

      PointType xv = viennagrid::make_point(1,0,0);
      if ( std::abs(viennagrid::inner_prod(xv,lv)) > 1.0-viennagrid::detail::absolute_tolerance<CoordType>(numeric_config) )
        xv = viennagrid::make_point(0,1,0);

      PointType yv = viennagrid::cross_prod(lv, xv);
      yv.normalize();
      xv = viennagrid::cross_prod(yv,lv);
      xv.normalize();





      typedef std::map<CoordType, std::pair<ElementType, bool> > SortedNeighborsType;
      SortedNeighborsType sorted_neighbors;

      for (ConstCoboundaryIteratorType tit = triangles.begin(); tit != triangles.end(); ++tit)
      {
        PointType tp = viennagrid::centroid(*tit);
        PointType tn = viennagrid::normal_vector(*tit);

        PointType to_triangle_vector = tp-lp;

        CoordType x = viennagrid::inner_prod(to_triangle_vector, xv);
        CoordType y = viennagrid::inner_prod(to_triangle_vector, yv);

        CoordType angle = std::atan2(y, x);
        bool positive = viennagrid::inner_prod( viennagrid::cross_prod( to_triangle_vector, tn ), lv ) > 0;

        sorted_neighbors[angle] = std::make_pair(*tit, positive);
      }

      assert( sorted_neighbors.size() ==  triangles.size() );

      typename SortedNeighborsType::iterator it0 = sorted_neighbors.begin();
      typename SortedNeighborsType::iterator it1 = it0; ++it1;

      for (; it1 != sorted_neighbors.end(); ++it0, ++it1)
      {
        if ( (*it0).second.second )
        {
          positive_neighbor_triangles[ (*it0).second.first.id().index() ].push_back( (*it1).second.first );
        }
        else
        {
          negative_neighbor_triangles[ (*it0).second.first.id().index() ].push_back( (*it1).second.first );
        }

        if ( (*it1).second.second )
        {
          negative_neighbor_triangles[ (*it1).second.first.id().index() ].push_back( (*it0).second.first );
        }
        else
        {
          positive_neighbor_triangles[ (*it1).second.first.id().index() ].push_back( (*it0).second.first );
        }
      }

      it1 = sorted_neighbors.begin();
      if ( (*it0).second.second )
      {
        positive_neighbor_triangles[ (*it0).second.first.id().index() ].push_back( (*it1).second.first );
      }
      else
      {
        negative_neighbor_triangles[ (*it0).second.first.id().index() ].push_back( (*it1).second.first );
      }

      if ( (*it1).second.second )
      {
        negative_neighbor_triangles[ (*it1).second.first.id().index() ].push_back( (*it0).second.first );
      }
      else
      {
        positive_neighbor_triangles[ (*it1).second.first.id().index() ].push_back( (*it0).second.first );
      }
    }



    std::pair<PointType, PointType> bb = viennagrid::bounding_box(mesh);
    PointType outside_point = bb.first - viennagrid::make_point(1,1,1) * viennagrid::norm_2(bb.first-bb.second) * 0.1;

    ConstElementRangeType triangles(mesh, 2);
    for (ConstElementIteratorType tit = triangles.begin(); tit != triangles.end(); ++tit)
    {
      // calculating the center of the triangle
      PointType r = viennagrid::centroid(*tit);

      // calculating the normal vector of the triangle
      PointType n = viennagrid::normal_vector(*tit);
      // ... and normalizing it
      n /= viennagrid::norm_2(n);

      // calculating the ray vector from the center of the triangle to the seed point
      PointType d = outside_point - r;

      // projecting the normalized ray vector onto the normal vector
      CoordType p = viennagrid::inner_prod( d, n ) / viennagrid::norm_2(d);

      // if the projection is near zero (happens when the ray vector and the triangle are co-linear) -> skip this triangle
      if ( std::abs(p) < viennagrid::detail::absolute_tolerance<CoordType>(numeric_config) )
        continue;


      ConstElementIteratorType tit2 = triangles.begin();
      for (; tit2 != triangles.end(); ++tit2)
      {
        // no self intersect test
        if (*tit == *tit2)
          continue;

        // in case of intersection -> nothing to do with the triangle
        if (viennagrid::element_line_intersect(*tit2, r, outside_point, numeric_config))
          break;
      }

      // if there was no intersection -> mark this triangle and all neighbor triangles recursively
      if (tit2 == triangles.end())
      {
        mark_neighbors(mesh, positive_neighbor_triangles, negative_neighbor_triangles, pos_orient, neg_orient, *tit, p > 0, 0);

        break;
      }
    }


    int region_id = 1;
    bool done = false;
    while (!done)
    {
      done = true;

      ConstElementRangeType triangles(mesh, 2);
      for (ConstElementIteratorType tit = triangles.begin(); tit != triangles.end(); ++tit)
      {
        // triangle already is in two regions
        if ( (pos_orient.get(*tit) != -1) && (neg_orient.get(*tit) != -1) )
          continue;

        bool positive = pos_orient.get(*tit) == -1;

        mark_neighbors(mesh,
                       positive_neighbor_triangles,
                       negative_neighbor_triangles,
                       pos_orient,
                       neg_orient,
                       *tit,
                       positive,
                       region_id);

        done = false;
        ++region_id;
      }


    }

    return region_id-1;
  }





  mark_hull_regions::mark_hull_regions() {}
  std::string mark_hull_regions::name() { return "mark_hull_regions"; }

  bool mark_hull_regions::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    typedef viennagrid::mesh                                  MeshType;
    typedef viennagrid::result_of::region<MeshType>::type     RegionType;
    typedef viennagrid::result_of::element<MeshType>::type    ElementType;


    std::vector<int> positive_orientation_regions( viennagrid::cells(input_mesh()).size(), -1 );
    viennagrid::result_of::accessor< std::vector<int>, ElementType >::type pos_orient(positive_orientation_regions);

    std::vector<int> negative_orientation_regions( viennagrid::cells(input_mesh()).size(), -1 );
    viennagrid::result_of::accessor< std::vector<int>, ElementType >::type neg_orient(negative_orientation_regions);


    int region_count = mark_hulls(input_mesh(), pos_orient, neg_orient, 1e-6 );
    info(1) << "Found " << region_count << " regions " << std::endl;

    mesh_handle output_mesh = make_data<mesh_handle>();

    std::vector<RegionType> output_regions;
    for (int i = 0; i != region_count; ++i)
      output_regions.push_back( output_mesh().get_or_create_region(i) );


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
        VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "A triangle is both in positive and negative regions");
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






//     typedef viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
//     typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;
//
//     RegionRangeType regions( output_mesh() );
//     for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
//     {
//       std::cout << "Checking region " << (*rit).id() << std::endl;
//       MeshType mesh;
//       CopyMapType tmpcm( mesh, false );
//
//       typedef viennagrid::result_of::const_vertex_range<RegionType>::type ConstRegionVertexRangeType;
//       typedef viennagrid::result_of::iterator<ConstRegionVertexRangeType>::type ConstRegionVertexRangeIterator;
//
//       typedef viennagrid::result_of::const_cell_range<RegionType>::type ConstRegionCellRangeType;
//       typedef viennagrid::result_of::iterator<ConstRegionCellRangeType>::type ConstRegionCellRangeIterator;
//
//
//       ConstRegionVertexRangeType vertices( *rit );
//       for (ConstRegionVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
//         cm(*vit);
//
//
//       ConstRegionCellRangeType cells( *rit );
//       for (ConstRegionCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
//       {
//         int por = positive_orientation_region(output_mesh(), *cit);
//
//         if (por == (*rit).id())
//         {
//           tmpcm(*cit);
//         }
//         else
//         {
//           viennagrid::make_triangle( mesh,
//                                      tmpcm( viennagrid::vertices(*cit)[0] ),
//                                      tmpcm( viennagrid::vertices(*cit)[2] ),
//                                      tmpcm( viennagrid::vertices(*cit)[1] ) );
//         }
//       }
//
//       if (!check_orientation(mesh))
//         std::cout << "Error checking orientation of region " << (*rit).id() << std::endl;
//     }








    set_output("mesh", output_mesh);
    return true;
  }

}




