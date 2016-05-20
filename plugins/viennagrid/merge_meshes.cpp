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

#include "merge_meshes.hpp"

namespace viennamesh
{
  template<bool mesh_is_const>
  void merge_meshes_impl(viennagrid::base_mesh<mesh_is_const> const & src_mesh,
                         viennagrid::mesh const & dst_mesh,
                         double tolerance, bool region_offset)
  {
    viennagrid::result_of::element_copy_map<>::type copy_map(dst_mesh, tolerance, false);

    typedef viennagrid::base_mesh<mesh_is_const>                                    SrcMeshType;

    typedef typename viennagrid::result_of::element<SrcMeshType>::type              CellType;

    typedef typename viennagrid::result_of::const_cell_range<SrcMeshType>::type     ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type      ConstCellIteratorType;

    typedef typename viennagrid::result_of::region_range<SrcMeshType>::type         SrcRegionRangeType;
    typedef typename viennagrid::result_of::iterator<SrcRegionRangeType>::type      SrcRegionRangeIterator;

    int region_id_offset = dst_mesh.region_count();
    int source_region_count = src_mesh.region_count();

    ConstCellRangeType cells(src_mesh);
    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      CellType cell = copy_map(*cit);

      if (source_region_count <= 1)
        viennagrid::add( dst_mesh.get_or_create_region(region_offset ? region_id_offset : 0), cell );
      else
      {
        SrcRegionRangeType region_range(*cit);
        for (SrcRegionRangeIterator rit = region_range.begin(); rit != region_range.end(); ++rit)
        {
          viennagrid::add( dst_mesh.get_or_create_region((*rit).id() + (region_offset ? region_id_offset : 0)), cell );
        }
      }
    }
  }



  merge_meshes::merge_meshes() {}
  std::string merge_meshes::name() { return "merge_meshes"; }

  bool merge_meshes::run(viennamesh::algorithm_handle &)
  {
    mesh_handle output_mesh = make_data<mesh_handle>();
    mesh_handle input_mesh = get_input<mesh_handle>("mesh");

    bool region_offset = true;
    if ( get_input<bool>("region_offset").valid() )
      region_offset = get_input<bool>("region_offset")();

    double tolerance = 1e-6;
    if ( get_input<double>("tolerance").valid() )
      tolerance = get_input<double>("tolerance")();

    info(1) << "Using region offset: " << std::boolalpha << region_offset << std::endl;

    int merged_count = 0;
    if (input_mesh.valid())
    {
      int mesh_count = input_mesh.size();
      for (int i = 0; i != mesh_count; ++i)
      {
        merge_meshes_impl( input_mesh(i), output_mesh(), (merged_count == 0 ? -1 : tolerance), region_offset );
        ++merged_count;
      }
    }

    int mesh_index = 0;
    while (true)
    {
      std::string input_name = "mesh" + boost::lexical_cast<std::string>(mesh_index);
      mesh_handle another_input_mesh = get_input<mesh_handle>(input_name);

      if (!another_input_mesh.valid())
        break;

      int mesh_count = another_input_mesh.size();
      for (int i = 0; i != mesh_count; ++i)
      {
        merge_meshes_impl( another_input_mesh(i), output_mesh(), (merged_count == 0 ? -1 : tolerance), region_offset );
        ++merged_count;
      }

      ++mesh_index;
    }

//



//     if (input_mesh)
//       merge_meshes_impl( input_mesh(), output_mesh() );
//
//     int index = 0;
//     input_mesh = get_input<mesh_handle>("mesh[" + lexical_cast<std::string>(index++) + "]");
//     while (input_mesh)
//     {
//       merge_meshes_impl( input_mesh(), output_mesh() );
//       input_mesh = get_input<mesh_handle>("mesh[" + lexical_cast<std::string>(index++) + "]");
//     }

    info(1) << "Merged " << merged_count << " meshes" << std::endl;

    set_output( "mesh", output_mesh );

    return true;
  }

}
