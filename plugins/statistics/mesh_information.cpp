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

#include "mesh_information.hpp"

#include "viennagridpp/algorithm/volume.hpp"
#include "viennagridpp/algorithm/surface.hpp"

namespace viennamesh
{
  mesh_information::mesh_information() {}
  std::string mesh_information::name() { return "mesh_information"; }

  bool mesh_information::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    typedef viennagrid::mesh                                                MeshType;
    typedef viennagrid::result_of::region<MeshType>::type                   RegionType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type      ConstElementRangeType;

    typedef viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
    typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionIteratorType;


    int topologic_dimension = viennagrid::topologic_dimension( input_mesh() );
    int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );

    info(1) << "Topologic dimension = " << topologic_dimension << std::endl;
    info(1) << "Geometric dimension = " << geometric_dimension << std::endl;

    for (int i = 0; i != topologic_dimension; ++i)
    {
      ConstElementRangeType elements( input_mesh(), i );
      info(1) << "  #element (topo-dim " << i << ") = " << elements.size() << std::endl;
    }

    info(1) << "volume  = " << viennagrid::volume( input_mesh() ) << std::endl;
    info(1) << "surface = " << viennagrid::surface( input_mesh() ) << std::endl;

    RegionRangeType regions(input_mesh());
    info(1) << "Number of regions: " << regions.size() << std::endl;

    for (RegionIteratorType rit = regions.begin(); rit != regions.end(); ++rit)
    {
      info(1) << "  Region " << (*rit).id() << std::endl;
      info(1) << "    name = " << (*rit).name() << std::endl;

      typedef viennagrid::result_of::const_element_range<RegionType>::type ConstRegionElementRangeType;
      typedef viennagrid::result_of::iterator<ConstRegionElementRangeType>::type ConstRegionElementIteratorType;

      for (int i = 0; i != topologic_dimension; ++i)
      {
        ConstRegionElementRangeType elements(*rit, i);
        int count = 0;
        for (ConstRegionElementIteratorType eit = elements.begin(); eit != elements.end(); ++eit)
          ++count;
        info(1) << "      #element (topo-dim " << i << ") = " << count << std::endl;
      }

      info(1) << "    volume  = " << viennagrid::volume(*rit) << std::endl;
      info(1) << "    surface = " << viennagrid::surface(*rit) << std::endl;
    }

    return true;
  }

}
