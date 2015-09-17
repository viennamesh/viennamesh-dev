#include "netgen_mesh.hpp"
#include "viennagrid/viennagrid.hpp"

namespace viennamesh
{
  viennamesh_error convert(viennagrid::mesh const & input, netgen::mesh & output)
  {
    typedef viennagrid::mesh                                                MeshType;

    typedef viennagrid::result_of::point<MeshType>::type                    PointType;
    typedef viennagrid::result_of::element<MeshType>::type                  ElementType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type      ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type    ConstElementIteratorType;


    typedef viennagrid::result_of::region_range<MeshType>::type             RegionRangeType;
    typedef viennagrid::result_of::iterator<RegionRangeType>::type          RegionIteratorType;

    int region_count = input.region_count();

    RegionRangeType regions(input);
    for (RegionIteratorType rit = regions.begin(); rit != regions.end(); ++rit)
    {
      int netgen_sid = (*rit).id() + 1;
      output.AddFaceDescriptor( (::netgen::FaceDescriptor (netgen_sid, netgen_sid, 0, netgen_sid)) );
    }

    std::map<ElementType, int> vertex_index_map;
    ConstElementRangeType vertices(input, 0);
    for (ConstElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      PointType p = viennagrid::get_point(*vit);
      vertex_index_map[*vit] = output.AddPoint( ::netgen::Point3d( p[0], p[1], p[2] ) );
    }

    ConstElementRangeType triangles(input, 2);
    for (ConstElementIteratorType tit = triangles.begin(); tit != triangles.end(); ++tit)
    {
      typedef viennagrid::result_of::region_range<ElementType>::type ElementRegionRangeType;

      int indices[3];
      for (int i = 0; i < 3; ++i)
        indices[i] = vertex_index_map[ viennagrid::vertices(*tit)[i] ];

      ElementRegionRangeType element_regions(*tit);

      if ((element_regions.size() <= 0) && (region_count > 1))
      {
        VIENNAMESH_ERROR(VIENNAMESH_ERROR_CONVERSION_FAILED, "convert to netgen::mesh failed: one element is in no region");
      }

      if (element_regions.size() > 2)
      {
        VIENNAMESH_ERROR(VIENNAMESH_ERROR_CONVERSION_FAILED, "convert to netgen::mesh failed: on element is in more than two regions");
      }

      int region_id0 = region_count > 1 ? (*element_regions.begin()).id() : 0;
      int region_id1 = -1;
      if (element_regions.size() == 2)
      {
        region_id1 = (*(++element_regions.begin())).id();

        // lower region id has positive orientation
        if (region_id1 < region_id0)
          std::swap(region_id0, region_id1);
      }

      ::netgen::Element2d el(3);
      el.SetIndex ( region_id0+1 );
      el.PNum(1) = indices[0];
      el.PNum(2) = indices[2];
      el.PNum(3) = indices[1];
      output.AddSurfaceElement (el);

      if (element_regions.size() == 2)
      {
        ::netgen::Element2d el(3);
        el.SetIndex ( region_id1+1 );
        el.PNum(1) = indices[0];
        el.PNum(2) = indices[1];
        el.PNum(3) = indices[2];
        output.AddSurfaceElement (el);
      }
    }

    return VIENNAMESH_SUCCESS;
  }



  viennamesh_error convert(netgen::mesh const & input, viennagrid::mesh & output)
  {
    typedef viennagrid::mesh                                    MeshType;
    typedef viennagrid::result_of::point<MeshType>::type        PointType;
    typedef viennagrid::result_of::element<MeshType>::type      ElementType;
    typedef viennagrid::result_of::region<MeshType>::type       RegionType;


    int num_points = input.GetNP();
    int num_tets = input.GetNE();

    std::vector<ElementType> vertices( num_points );

    for (int i = 1; i <= num_points; ++i)
    {
      PointType point = viennagrid::make_point(input.Point(i)[0], input.Point(i)[1], input.Point(i)[2]);
      vertices[i-1] = viennagrid::make_vertex( output, point );
    }


    for (int i = 0; i < num_tets; ++i)
    {
        ::netgen::ElementIndex ei = i;
        RegionType region = output.get_or_create_region( input[ei].GetIndex() );
        viennagrid::make_tetrahedron( region,
                                      vertices[input[ei][0]-1],
                                      vertices[input[ei][1]-1],
                                      vertices[input[ei][2]-1],
                                      vertices[input[ei][3]-1]);
    }

    return VIENNAMESH_SUCCESS;
  }



  template<>
  viennamesh_error internal_convert<viennagrid_mesh, netgen::mesh>(viennagrid_mesh const & input, netgen::mesh & output)
  { return convert( viennagrid::mesh(input), output ); }

  template<>
  viennamesh_error internal_convert<netgen::mesh, viennagrid_mesh>(netgen::mesh const & input, viennagrid_mesh & output)
  {
    viennagrid::mesh output_pp(output);
    return convert( input, output_pp );
  }

}
