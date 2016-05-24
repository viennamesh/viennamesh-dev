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

#include "change_cell_region.hpp"

#include <set>
#include "viennagrid/algorithm/centroid.hpp"

namespace viennamesh
{
  change_cell_region::change_cell_region() {}
  std::string change_cell_region::name() { return "change_cell_region"; }

  bool change_cell_region::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    mesh_handle output_mesh = make_data<mesh_handle>();

    data_handle<viennamesh_string> cells_to_change_input = get_required_input<viennamesh_string>("cells_to_change");
    std::vector<std::string> cells_to_change = cells_to_change_input.get_vector();

    viennagrid_dimension cell_dim = viennagrid::cell_dimension( input_mesh() );


    typedef viennagrid::mesh MeshType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;
    typedef viennagrid::result_of::region<MeshType>::type RegionType;
    typedef viennagrid::result_of::region_range<ElementType>::type RegionRangeType;

    typedef viennagrid::result_of::element_range<MeshType>::type ElementRangeType;
    typedef viennagrid::result_of::iterator<ElementRangeType>::type ElementRangeIterator;

    std::map<ElementType, RegionType> cell_region_map;

    ElementRangeType input_cells( input_mesh(), cell_dim );
    for (ElementRangeIterator cit = input_cells.begin(); cit != input_cells.end(); ++cit)
    {
      RegionRangeType input_regions(*cit);
      if (input_regions.empty())
        continue;

      cell_region_map[*cit] = output_mesh().get_or_create_region( (*input_regions.begin()).id() );
    }

    for (std::size_t i = 0; i != cells_to_change.size(); ++i)
    {
      // assume input is in format "cell_index;new_region_id"
      viennagrid_int cell_index = lexical_cast<viennagrid_int>(cells_to_change[i].substr(0, cells_to_change[i].find(";")));
      viennagrid_region_id new_region_id = lexical_cast<viennagrid_region_id>(cells_to_change[i].substr(cells_to_change[i].find(";")+1));

      viennagrid_element_id cell_id = viennagrid_compose_element_id(cell_dim, cell_index);
      std::cout << "Changing cell with ID " << cell_id << " (index = " << cell_index << ", cell_dim = " << (int)cell_dim << ") to region " << new_region_id << std::endl;

      cell_region_map[ ElementType(input_mesh(), cell_id) ] = output_mesh().get_or_create_region(new_region_id);
    }

    viennagrid::result_of::element_copy_map<>::type copy_map( output_mesh(), false );
    for (ElementRangeIterator cit = input_cells.begin(); cit != input_cells.end(); ++cit)
    {
      ElementType new_element = copy_map(*cit);
      std::map<ElementType, RegionType>::iterator it = cell_region_map.find(*cit);
      if (it != cell_region_map.end())
      {
        viennagrid::add((*it).second, new_element);
      }
    }

    set_output( "mesh", output_mesh );

    return true;
  }

}
