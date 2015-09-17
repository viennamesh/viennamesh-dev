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

#include "map_regions.hpp"
#include "boost/algorithm/string.hpp"
#include <set>

namespace viennamesh
{
  template<bool mesh_is_const, typename SegmentIDMapT>
  void map_regions_impl(viennagrid::base_mesh<mesh_is_const> const & src_mesh,
                        viennagrid::mesh const & dst_mesh,
                        SegmentIDMapT const & region_id_map)
  {
    typedef viennagrid::base_mesh<mesh_is_const>                                  SrcMeshType;
    typedef viennagrid::mesh                                                      DstMeshType;

    viennagrid::result_of::element_copy_map<>::type copy_map(dst_mesh, false);

    typedef typename viennagrid::result_of::const_cell_range<SrcMeshType>::type   ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type    ConstCellIteratorType;

    typedef typename viennagrid::result_of::region_range<SrcMeshType>::type       SrcRegionRangeType;
    typedef typename viennagrid::result_of::iterator<SrcRegionRangeType>::type    SrcRegionRangeIterator;
    typedef typename viennagrid::result_of::region_id<SrcMeshType>::type          SrcRegionIDType;
    typedef typename viennagrid::result_of::region_id<DstMeshType>::type          DstRegionIDType;

    typedef typename viennagrid::result_of::element<DstMeshType>::type            CellType;

    std::map<SrcRegionIDType, std::string> region_id_name_map;
    std::map<std::string, DstRegionIDType> region_name_id_map;

    SrcRegionRangeType src_regions(src_mesh);
    for (SrcRegionRangeIterator rit = src_regions.begin(); rit != src_regions.end(); ++rit)
    {
      region_id_name_map[ (*rit).id() ]   = (*rit).get_name();
      region_name_id_map[ (*rit).get_name() ] = (*rit).id();
    }


    ConstCellRangeType cells(src_mesh);
    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      CellType cell = copy_map(*cit );
      std::set<DstRegionIDType> dst_region_ids;

      SrcRegionRangeType element_regions(*cit);
      for (SrcRegionRangeIterator rit = element_regions.begin(); rit != element_regions.end(); ++rit)
      {
        typename SegmentIDMapT::const_iterator dst_region_id_it = region_id_map.find( region_id_name_map[(*rit).id()] );
        if (dst_region_id_it != region_id_map.end())
          dst_region_ids.insert( region_name_id_map[dst_region_id_it->second] );
        else
          dst_region_ids.insert( (*rit).id() );
      }

      for (typename std::set<DstRegionIDType>::const_iterator dst_region_id_it = dst_region_ids.begin(); dst_region_id_it != dst_region_ids.end(); ++dst_region_id_it)
        viennagrid::add( dst_mesh.get_or_create_region(*dst_region_id_it), cell );
    }
  }



  map_regions::map_regions() {}
  std::string map_regions::name() { return "map_regions"; }

  bool map_regions::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    string_handle input_region_mapping = get_required_input<string_handle>("region_mapping");

    std::map<std::string, std::string> region_mapping;
    {
      std::string tmp = input_region_mapping();

      std::list<std::string> mappings;
      boost::algorithm::split( mappings, tmp, boost::is_any_of(";") );
//       std::list<std::string> mappings = stringtools::split_string( input_region_mapping(), ";" );

      for (std::list<std::string>::const_iterator sit = mappings.begin(); sit != mappings.end(); ++sit)
      {
        std::list<std::string> from_to;
        boost::algorithm::split( from_to, *sit, boost::is_any_of(",") );

//         std::list<std::string> from_to = stringtools::split_string( *sit, "," );
        if (from_to.size() != 2)
        {
          return false;
        }

        std::list<std::string>::const_iterator it = from_to.begin();
        std::string from = *it;
        ++it;
        std::string to = *it;

        region_mapping[from] = to;
      }
    }

    for (std::map<std::string, std::string>::const_iterator it = region_mapping.begin(); it != region_mapping.end(); ++it)
    {
      info(1) << "Mapping region " << it->first << " to " << it->second << std::endl;
    }

    mesh_handle output_mesh = make_data<mesh_handle>();

    map_regions_impl( input_mesh(), output_mesh(), region_mapping );

    set_output( "mesh", output_mesh );
    return true;
  }

}
