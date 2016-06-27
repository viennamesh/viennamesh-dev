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

#include <memory>

#include "tdr_reader.hpp"
#include "sentaurus_tdr_reader.hpp"
#include "viennameshpp/core.hpp"

namespace viennamesh
{
  tdr_reader::tdr_reader() {}
  std::string tdr_reader::name() { return "tdr_reader"; }






  template<typename MeshT>
  void fill_triangle_contacts(MeshT const & mesh)
  {
    typedef typename viennagrid::result_of::element<MeshT>::type ElementType;

    typedef typename viennagrid::result_of::region<MeshT>::type RegionType;
    typedef typename viennagrid::result_of::id<RegionType>::type RegionIDType;

    typedef typename viennagrid::result_of::region_range<MeshT>::type RegionRangeType;
    typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

    RegionRangeType regions(mesh);
    std::vector<RegionIDType> region_contacts;
    for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      if ( (*rit).get_name().find("_contact") != std::string::npos )
        region_contacts.push_back( (*rit).id() );
    }

    typedef typename viennagrid::result_of::element_range<RegionType>::type ElementRangeType;
    typedef typename viennagrid::result_of::iterator<ElementRangeType>::type ElementRangeIterator;



    for (std::size_t i = 0; i != region_contacts.size(); ++i)
    {
      RegionType region = mesh.get_region( region_contacts[i] );
      ElementRangeType triangles(region, 2);

      std::set< std::vector<ElementType> > new_triangles;

      for (ElementRangeIterator tit = triangles.begin(); tit != triangles.end(); ++tit)
      {
        typedef typename viennagrid::result_of::neighbor_range<RegionType>::type NeighborElementRangeType;
        typedef typename viennagrid::result_of::iterator<NeighborElementRangeType>::type NeighborElementRangeIterator;

        NeighborElementRangeType neighbor_triangles(region, *tit, 0, 2);
        for (NeighborElementRangeIterator ntit = neighbor_triangles.begin(); ntit != neighbor_triangles.end(); ++ntit)
        {
          std::vector<ElementType> triangle(3);

          ElementType shared_vertex;
          for (int i = 0; i != 3; ++i)
            for (int j = 0; j != 3; ++j)
            {
              if ( viennagrid::vertices(*tit)[i] == viennagrid::vertices(*ntit)[j] )
                triangle[0] = viennagrid::vertices(*tit)[i];
            }

//           ElementType vertex_t;
          for (int i = 0; i != 3; ++i)
          {
            if (viennagrid::regions(viennagrid::vertices(*tit)[i]).size() == 1)
              triangle[1] = viennagrid::vertices(*tit)[i];
          }

//           ElementType vertex_nt;
          for (int i = 0; i != 3; ++i)
          {
            if (viennagrid::regions(viennagrid::vertices(*ntit)[i]).size() == 1)
              triangle[2] = viennagrid::vertices(*ntit)[i];
          }

          std::sort( triangle.begin(), triangle.end() );

          new_triangles.insert(triangle);

//           ElementType triangle = viennagrid::make_triangle( region, shared_vertex, vertex_t, vertex_nt );
        }
      }

      for (typename std::set< std::vector<ElementType> >::iterator it = new_triangles.begin(); it != new_triangles.end(); ++it)
        viennagrid::make_triangle( region, (*it)[0], (*it)[1], (*it)[2] );
    }
  }




  bool tdr_reader::run(viennamesh::algorithm_handle &)
  {
    string_handle filename = get_required_input<string_handle>("filename");

    std::string path = base_path();
    std::string full_filename;

    if (!path.empty())
    {
      info(1) << "Using base path: " << path << std::endl;
      full_filename = path + "/" + filename();
    }
    else
      full_filename = filename();


    shared_ptr<H5File> file( new H5File(full_filename.c_str(), H5F_ACC_RDWR) );

    if (file->getNumObjs()!=1)
    {
      error(1) << "File has not exactly one collection (number of collections = " << file->getNumObjs() << std::endl;
      return false;
    }

    tdr_geometry geometry;
    geometry.read_collection(file->openGroup("collection"));

    geometry.correct_vertices();

    bool extrude_contacts = true;
    if ( get_input<bool>("extrude_contacts").valid() )
      extrude_contacts = get_input<bool>("extrude_contacts")();

    double extrude_contacts_scale = 1.0;
    if ( get_input<double>("extrude_contacts_scale").valid() )
      extrude_contacts_scale = get_input<double>("extrude_contacts_scale")();

    mesh_handle output_mesh = make_data<mesh_handle>();
    geometry.to_viennagrid( output_mesh(), extrude_contacts, extrude_contacts_scale );

    if ( get_input<bool>("fill_triangle_contacts").valid() && get_input<bool>("fill_triangle_contacts")() )
      fill_triangle_contacts( output_mesh() );






    std::vector<viennagrid::quantity_field> quantity_fields = geometry.quantity_fields( output_mesh() );
    if (!quantity_fields.empty())
    {
      quantity_field_handle output_quantity_fields = make_data<viennagrid::quantity_field>();
      output_quantity_fields.resize( quantity_fields.size() );

      for (std::size_t i = 0; i != quantity_fields.size(); ++i)
        output_quantity_fields.set(i, quantity_fields[i]);

      set_output( "quantities", output_quantity_fields );
    }

    set_output("mesh", output_mesh);

    return true;
  }

}
