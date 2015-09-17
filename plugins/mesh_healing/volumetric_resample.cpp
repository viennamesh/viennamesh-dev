/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include <numeric>
#include "volumetric_resample.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/inclusion.hpp"
#include "viennagrid/algorithm/centroid.hpp"


// #include <CGAL/Simple_cartesian.h>
// #include <CGAL/AABB_tree.h>
// #include <CGAL/AABB_traits.h>
// #include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
// #include <CGAL/AABB_face_graph_triangle_prist abimitive.h>


namespace viennamesh
{

//   enum OctTreePosition
//   {
//     OCP_LLL = 0,
//     OCP_LLU = 1,
//     OCP_LUL = 2,
//     OCP_LUU = 3,
//
//     OCP_ULL = 4,
//     OCP_ULU = 5,
//     OCP_UUL = 6,
//     OCP_UUU = 7
//   };
//
//
//   template<typename MeshT>
//   class OctTree
//   {
//   public:
//
//     typedef typename viennagrid::result_of::element<MeshT>::type ElementType;
//     typedef typename viennagrid::result_of::point<MeshT>::type PointType;
//
//     ~OctTree()
//     {
//       for (int i = 0; i != 8; ++i)
//         delete children[i];
//     }
//
//     OctTree(PointType ll_, PointType ur_, int depth) : ll(std::min(ll_, ur_)), ur(std::max(ll_, ur_))
//     {
//       if (depth != 0)
//       {
//         PointType center = (ll+ur)/2.0;
//
//         children[OCP_LLL] = new OctTree( viennagrid::make_point(ll[0], ll[1], ll[2]), center );
//         children[OCP_LLU] = new OctTree( viennagrid::make_point(ll[0], ll[1], ur[2]), center );
//         children[OCP_LUL] = new OctTree( viennagrid::make_point(ll[0], ur[1], ll[2]), center );
//         children[OCP_LUU] = new OctTree( viennagrid::make_point(ll[0], ur[1], ur[2]), center );
//
//         children[OCP_ULL] = new OctTree( viennagrid::make_point(ur[0], ll[1], ll[2]), center );
//         children[OCP_ULU] = new OctTree( viennagrid::make_point(ur[0], ll[1], ur[2]), center );
//         children[OCP_UUL] = new OctTree( viennagrid::make_point(ur[0], ur[1], ll[2]), center );
//         children[OCP_UUU] = new OctTree( viennagrid::make_point(ur[0], ur[1], ur[2]), center );
//       }
//     }
//
//
//     OctTree * operator()(PointType const & pt)
//     {
//       if (pt[0] < ll[0] || pt[1] < ll[1] || pt[2] < ll[2] ||
//           pt[0] > ur[0] || pt[1] > ur[1] || pt[2] > ur[2] )
//       {
//         return 0;
//       }
//
//       if ( leaf() )
//         return this;
//
//       int index = 0b000;
//
//       if (pt[0] > (ll[0]+ur[0])/2.0)
//         index = index | 0b001;
//
//       if (pt[1] > (ll[1]+ur[1])/2.0)
//         index = index | 0b010;
//
//       if (pt[2] > (ll[2]+ur[2])/2.0)
//         index = index | 0b100;
//
//       return (*children[index])(pt);
//     }
//
//
//     void add_element(ElementType const & element)
//     {
//
//     }
//
//
//
//
//     bool leaf() const
//     {
//       return children[0] == NULL;
//     }
//
//
//     PointType ll;
//     PointType ur;
//
//     OctTree * children[8];
//     std::vector<ElementType> cells;
//   };









  volumetric_resample::volumetric_resample() {}
  std::string volumetric_resample::name() { return "volumetric_resample"; }

  bool volumetric_resample::run(viennamesh::algorithm_handle &)
  {
    data_handle<int> sample_count = get_required_input<int>("sample_count");

    mesh_handle reference_mesh = get_required_input<mesh_handle>("reference_mesh");
    mesh_handle base_mesh = get_required_input<mesh_handle>("base_mesh");

    mesh_handle output_mesh = make_data<mesh_handle>();

    int region_count = reference_mesh().region_count();


    typedef viennagrid::mesh                                        MeshType;
    typedef viennagrid::result_of::element<MeshType>::type          ElementType;
    typedef viennagrid::result_of::cell_range<MeshType>::type       CellRangeType;
    typedef viennagrid::result_of::iterator<CellRangeType>::type    CellRangeIterator;


    MeshType tmp;

    viennagrid::copy( base_mesh(), tmp );
    CellRangeType cells( tmp );

    const int NOT_SPECIFIED = -1;
    const int OUTSIDE_MESH = -2;

    std::vector<int> region_container( cells.size(), NOT_SPECIFIED );
    typedef viennagrid::result_of::accessor< std::vector<int>, ElementType >::type RegionAccessor;
    RegionAccessor region(region_container);



    for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if ( region.get(*cit) != NOT_SPECIFIED )
        continue;

      point pt_a = viennagrid::get_point( viennagrid::vertices(*cit)[0] );
      point pt_b = viennagrid::get_point( viennagrid::vertices(*cit)[1] );
      point pt_c = viennagrid::get_point( viennagrid::vertices(*cit)[2] );
      point pt_d = viennagrid::get_point( viennagrid::vertices(*cit)[3] );

      std::vector<double> weights(region_count+1, 0.0);
      for (int i = 0; i < sample_count(); ++i)
      {
        double a = -1;
        double b = -1;
        double c = -1;
        double d = -1;

        while (a+b+c+d < 1e-6)
        {
          a = static_cast<double>(rand())/RAND_MAX;
          b = static_cast<double>(rand())/RAND_MAX;
          c = static_cast<double>(rand())/RAND_MAX;
          d = static_cast<double>(rand())/RAND_MAX;
        }

        point sample_point = (a*pt_a + b*pt_b + c*pt_c + d*pt_d) / (a+b+c+d);

        std::vector<int> local_hits(region_count, 0);
        int total_local_hits = 0;

        CellRangeType src_cells( reference_mesh() );
        for (CellRangeIterator scit = src_cells.begin(); scit != src_cells.end(); ++scit)
        {
          if (viennagrid::is_inside(*scit, sample_point))
          {
            typedef viennagrid::result_of::region_range<ElementType>::type RegionRangeType;
            typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

            RegionRangeType regions(*scit);
            for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
            {
              ++total_local_hits;
              local_hits[(*rit).id()]++;
            }
          }
        }

        if (total_local_hits == 0)
          weights[region_count] += 1.0;
        else
        {
          int sum = std::accumulate(local_hits.begin(), local_hits.end(), 0);
          for (int i = 0; i != region_count; ++i)
            weights[i] += static_cast<double>(local_hits[i])/static_cast<double>(sum);
        }
      }

      std::vector<double>::iterator max = std::max_element( weights.begin(), weights.end() );
      int region_id = max - weights.begin();

      if (*max > 0.9*sample_count() && region_id != region_count)
        region.set(*cit, region_id);

//       if (region_id != region_count)
//       {
//         region(*cit) = region_id;
//       }
    }



    typedef viennagrid::result_of::element_copy_map<>::type ElementCopyMap;
    ElementCopyMap copy_map( output_mesh() );
    cells = CellRangeType(tmp);

    for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if ( region.get(*cit) != NOT_SPECIFIED && region.get(*cit) != OUTSIDE_MESH )
      {
        ElementType element = copy_map(*cit);
        viennagrid::add( output_mesh().get_or_create_region(region.get(*cit)), element );
      }
    }

    set_output( "mesh", output_mesh );

    return true;
  }

}
