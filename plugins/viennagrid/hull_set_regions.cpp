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

#include "hull_set_regions.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include <memory>
#include <set>
#include <iterator>

namespace viennamesh
{


  template<typename MeshT, typename ElementT, typename AccessorT, typename ToSetT>
  void set_neighbors(MeshT const & mesh,
                     ElementT const & element,
                     AccessorT & accessor,
                     ToSetT to_set,
                     ToSetT invalid,
                     viennagrid_int connector_topologic_dimension)
  {
    if (accessor.get(element) != invalid)
      return;

    accessor.set(element, to_set);

    typedef typename viennagrid::result_of::const_element_range<ElementT, 1>::type ConstBoundaryLineRangeType;
    typedef typename viennagrid::result_of::iterator<ConstBoundaryLineRangeType>::type ConstBoundaryLineRangeIterator;

    ConstBoundaryLineRangeType boundary_lines(element);
    for (ConstBoundaryLineRangeIterator blit = boundary_lines.begin(); blit != boundary_lines.end(); ++blit)
    {
      typedef typename viennagrid::result_of::const_coboundary_range<MeshT, 2>::type ConstCoboundaryTriangleRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCoboundaryTriangleRangeType>::type ConstCoboundaryTriangleRangeIterator;

      ConstCoboundaryTriangleRangeType neigbor_triangles(mesh, *blit);

//       if (neigbor_triangles.size() == 1)
//         std::cout << "ERROR!!!" << std::endl;

      // skip lines which have not exactly 2 coboundary triangles
      if (neigbor_triangles.size() != 2)
        continue;

      for (ConstCoboundaryTriangleRangeIterator ntit = neigbor_triangles.begin(); ntit != neigbor_triangles.end(); ++ntit)
      {
        // skip this
        if (*ntit == element)
          continue;

        set_neighbors(mesh, *ntit, accessor, to_set, invalid, connector_topologic_dimension);
      }
    }
  }


  struct poly_line;

  struct patch
  {
    patch(int id_) : id(id_) {}

    int id;
    std::vector< shared_ptr<poly_line> > interface_lines;
  };



  struct poly_line
  {
    poly_line( std::vector< shared_ptr<patch> > const & patches_ ) : patches(patches_) {}

    typedef viennagrid::mesh MeshType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    std::vector<ElementType> lines;
    std::vector< shared_ptr<patch> > patches;
  };



  hull_set_regions::hull_set_regions() {}
  std::string hull_set_regions::name() { return "hull_set_regions"; }

  bool hull_set_regions::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    mesh_handle output_mesh = make_data<mesh_handle>();

    mesh_handle output_mesh2 = make_data<mesh_handle>();


    typedef viennagrid::mesh                                                MeshType;
    typedef viennagrid::result_of::element<MeshType>::type                  ElementType;
    typedef viennagrid::result_of::const_cell_range<MeshType>::type         ConstCellRangeType;
    typedef viennagrid::result_of::iterator<ConstCellRangeType>::type       ConstCellRangeIterator;

    ConstCellRangeType cells(input_mesh());

    std::vector<int> cell_region_container( cells.size(), -1 );
    typedef viennagrid::result_of::accessor< std::vector<int>, ElementType >::type RegionAccessorType;
    RegionAccessorType cell_region( cell_region_container );

    int region_count = 0;
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if (cell_region.get(*cit) == -1)
        set_neighbors( input_mesh(), *cit, cell_region, region_count++, -1, 2 );
    }

    info(1) << "Number of regions: " << region_count << std::endl;

    viennagrid::element_copy_map<double> copy_map( output_mesh(), 1e-8, false );
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      ElementType new_element = copy_map(*cit);
      viennagrid::add( output_mesh().get_or_create_region( cell_region.get(*cit) ), new_element);
    }


//     for (auto triangle : viennagrid::cells(output_mesh()))
//     {
//       if (viennagrid::regions(output_mesh(), triangle).size() != 1)
//       {
//         std::cout << "ERROR!" << std::endl;
//         std::cout << "  " << triangle << std::endl;
//         std::cout << "  ";
//         for (auto region : viennagrid::regions(output_mesh(), triangle))
//           std::cout << region.id() << " ";
//         std::cout << std::endl;
//       }
//     }


//     viennagrid::element_copy_map<double> copy_map2( output_mesh2(), 1e-8 );
//     for (auto line : viennagrid::elements(output_mesh(), 1))
//     {
//       typedef viennagrid::result_of::const_coboundary_range<MeshType>::type ConstCoboundaryTriangleRangeType;
//
//       ConstCoboundaryTriangleRangeType triangles( output_mesh(),line,2 );
//
//       if (triangles.size() == 1)
//         std::cout << "ERROR!!" << std::endl;
//
//
//       std::set<int> region_ids;
//       for (auto cotri : triangles)
//       {
//         for (auto region : viennagrid::regions(output_mesh(), cotri))
//         {
//           region_ids.insert( region.id() );
//         }
//       }
//
//       if (triangles.size() > 2 && region_ids.size() <= 2)
//       {
//         std::cout << "SOMETHING WRONG" << std::endl;
//
//         for (auto cotri : triangles)
//         {
//           copy_map2(cotri);
//
//           std::cout << "   " << cotri << std::endl;
//           std::cout << "   ";
//           for (auto region : viennagrid::regions(output_mesh(), cotri))
//             std::cout << region.id() << "  ";
//           std::cout << std::endl;
//         }
//       }
//     }


//     std::vector< ElementType > interface_lines;


//     std::map< std::vector< std::shared_ptr<patch> >, std::shared_ptr<poly_line> > interface_poly_lines;
//     std::vector< std::shared_ptr<patch> > patches;
//
//     for (int i = 0; i < region_count; ++i)
//       patches.push_back( std::make_shared<patch>(i) );
//
//
//     for (auto line : viennagrid::elements(output_mesh(), 1))
//     {
//       typedef viennagrid::result_of::const_coboundary_range<MeshType, 2>::type ConstCoboundaryTriangleRangeType;
//       ConstCoboundaryTriangleRangeType coboundary_triangles(output_mesh(),line);
//
//       if (coboundary_triangles.size() != 2)
//       {
//         std::vector< std::shared_ptr<patch> > local_patches;
//
//         for (auto triangle : coboundary_triangles)
//         {
//           typedef viennagrid::result_of::region_range<MeshType, ElementType>::type RegionRangeType;
//           RegionRangeType regions(output_mesh(), triangle);
//
//           if (regions.size() != 1)
//             std::cout << "ANOTHER ERROR" << std::endl;
//
//           local_patches.push_back( patches[(*regions.begin()).id()] );
//         }
//
//
//         std::sort( local_patches.begin(), local_patches.end() );
//         local_patches.erase( std::unique(local_patches.begin(), local_patches.end()), local_patches.end() );
//
//         auto iplit = interface_poly_lines.find( local_patches );
//         if (iplit == interface_poly_lines.end())
//         {
//           std::cout << "No poly line found for ";
//           for (auto x : local_patches)
//             std::cout << x->id << " ";
//           std::cout << std::endl;
//
//           iplit = interface_poly_lines.insert( std::make_pair(local_patches, std::make_shared<poly_line>(local_patches)) ).first;
//         }
//
//         iplit->second->lines.push_back(line);
//         for (auto pit : local_patches)
//           pit->interface_lines.push_back( iplit->second );
//       }
//     }
//
//     std::cout << "Number of interface poly lines: " << interface_poly_lines.size() << std::endl;
//
//     for (int i = 0; i < patches.size(); ++i)
//       std::cout << "Patch " << i << " has " << patches[i]->interface_lines.size() << " interface lines" << std::endl;
//
//
//     for(auto ipl : interface_poly_lines)
//     {
//       if (ipl.first.size() == 2)
//       {
//         std::cout << " Poly line has only two patches: " << ipl.second->patches[0]->id << " " << ipl.second->patches[1]->id << std::endl;
//
// //         std::copy(
//       }
//     }


//     for (int i = 0; i < interface_lines.size(); ++i)
//       std::cout << "Region " << i << " has " << interface_lines[i].size() << " interface lines" << std::endl;




//     typedef viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
//     RegionRangeType regions( output_mesh() );
// //     typedef vienangrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;
// //     for (auto it = regions.begin(); it != regions.end(); ++it)
//     std::multimap<int, int> region_elements;
//
//     for (auto region : regions)
//       region_elements.insert( std::make_pair<int,int>(viennagrid::vertices(region).size(), region.id()) );
//
//
//     for (auto p : region_elements)
//       std::cout << " Region " << p.second << " has " << p.first << " cells" << std::endl;




//     int line_count = 0;
//     for (auto line : viennagrid::elements(output_mesh(),1))
//     {
//       if (viennagrid::coboundary_elements(output_mesh(),line,2).size() == 1)
//       {
//         PointType lp0 = viennagrid::get_point(line, 0);
//         PointType lp1 = viennagrid::get_point(line, 1);
//
//         double best_distance = -1;
//         ElementType best_line;
//         for (auto other_line : viennagrid::elements(output_mesh(),1))
//         {
//           if (line.id() == other_line.id())
//             continue;
//
//           PointType olp0 = viennagrid::get_point(other_line, 0);
//           PointType olp1 = viennagrid::get_point(other_line, 1);
//
//           double distance = std::min(
//             viennagrid::distance(lp0, olp0) + viennagrid::distance(lp1, olp1),
//             viennagrid::distance(lp0, olp1) + viennagrid::distance(lp1, olp0)
//           );
//
//           if ((best_distance < 0) || (distance < best_distance))
//           {
//             best_distance = distance;
//             best_line = other_line;
//           }
//         }
//
//         ++line_count;
//         std::cout << "Line " << line << " has only 1 tri, best matching other line: " << best_line << " (best distance = " << best_distance << std::endl;
//       }
//     }
//
//     std::cout << "Number of lines with only 1 co-boundary triangle: " << line_count << std::endl;



//     mesh_handle output_mesh2 = make_data<mesh_handle>();
//     viennagrid::element_copy_map<double> copy_map2( output_mesh2(), 1e-8 );
//
//     for (auto tri : viennagrid::elements(output_mesh(), 2))
//     {
//       for (auto line : viennagrid::elements(tri,1))
//       {
//         if (viennagrid::coboundary_elements(output_mesh(),line,2).size() == 1)
//         {
//           std::cout << "  " << tri << "  region = ";
//           for (auto region : viennagrid::regions(output_mesh(),tri))
//             std::cout << region.id() << " ";
//           std::cout << std::endl;
//
//           copy_map2(tri);
//         }
//
//       }
//     }


    set_output( "mesh", output_mesh );

    return true;
  }

}
