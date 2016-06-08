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

#include <set>
#include <map>
#include <iterator>

#include "mesh_partitioning.hpp"
#include "metis.h"


namespace viennamesh
{
  metis_mesh_partitioning::metis_mesh_partitioning() {}
  std::string metis_mesh_partitioning::name() { return "metis_mesh_partitioning"; }

  bool metis_mesh_partitioning::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    data_handle<int> region_count = get_required_input<int>("region_count");
    data_handle<bool> multi_mesh_output = get_input<bool>("multi_mesh_output");

    int cell_dimension = viennagrid::cell_dimension( input_mesh() );

    typedef viennagrid::mesh                                                MeshType;
    typedef viennagrid::result_of::const_cell_range<MeshType>::type         ConstCellRangeType;
    typedef viennagrid::result_of::iterator<ConstCellRangeType>::type       ConstCellRangeIterator;

    typedef viennagrid::result_of::element<MeshType>::type                  ElementType;
    typedef viennagrid::result_of::const_element_range<ElementType>::type   ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type    ConstElementRangeIterator;


    info(1) << "Using region count " << region_count() << std::endl;


    std::vector<idx_t> eptr;
    std::vector<idx_t> eind;

    eptr.push_back(0);



    ConstCellRangeType cells( input_mesh() );
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      ConstElementRangeType vertices(*cit, 0);
      for (ConstElementRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
        eind.push_back( (*vit).id().index() );

      eptr.push_back( eind.size() );
    }


    idx_t num_nodes = viennagrid::vertices( input_mesh() ).size();
    idx_t num_elements = eptr.size()-1;



    idx_t result;

    viennagrid::vector<idx_t> epart(num_elements);
    std::vector<idx_t> npart(num_nodes);

    idx_t ncommon = cell_dimension;
    idx_t nparts = region_count();

    METIS_PartMeshDual(&num_elements, &num_nodes,
                       &eptr[0], &eind[0],
                       NULL, NULL,
                       &ncommon, &nparts,
                       NULL, NULL,
                       &result, &epart[0], &npart[0]);



    mesh_handle output_mesh = make_data<mesh_handle>();

    typedef viennagrid::result_of::element_copy_map<>::type ElementCopyMapType;

    if ( multi_mesh_output.valid() && multi_mesh_output() )
    {
      output_mesh.resize( region_count() );
      std::vector<ElementCopyMapType> copy_maps;

      for (int i = 0; i != region_count(); ++i)
      {
        ElementCopyMapType copy_map( output_mesh(i) );
        for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
        {
          int part = epart[(*cit).id().index()];
          if (part == i)
            copy_map( *cit );
        }
      }
    }
    else
    {
      ElementCopyMapType copy_map( output_mesh(), false );

      for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
        ElementType cell = copy_map( *cit );
        viennagrid::add( output_mesh().get_or_create_region(epart[(*cit).id().index()]), cell );
      }
    }

    set_output( "mesh", output_mesh );

    return true;
  }

}
