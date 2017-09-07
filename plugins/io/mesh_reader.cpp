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

#include "mesh_reader.hpp"

#include <string>
#include <vector>
#include <boost/algorithm/string/split.hpp>

#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/bnd_reader.hpp"
#include "viennagrid/io/stl_reader.hpp"
#include "viennagrid/io/gts_deva_reader.hpp"
#include "viennagrid/io/dfise_grd_dat_reader.hpp"

#include <vtkXMLPolyDataReader.h>
#include <vtkSmartPointer.h>
#include <vtkIdList.h>
#include <vtkPolyData.h>


#include "viennameshpp/core.hpp"

namespace viennamesh
{
  mesh_reader::mesh_reader() {}
  std::string mesh_reader::name() { return "mesh_reader"; }


  bool mesh_reader::load( std::string const & filename, FileType filetype )
  {
    std::string path = extract_path( filename );

    info(1) << "Reading mesh from file \"" << filename << "\"" << std::endl;

    mesh_handle output_mesh = make_data<mesh_handle>();

    bool success = false;

    switch (filetype)
    {
    case SYNOPSIS_BND:
      {
        info(5) << "Found .bnd extension, using ViennaGrid BND Reader" << std::endl;

        viennagrid::io::bnd_reader reader;
        reader(output_mesh(), filename);

        success = true;
        break;
      }
    case NETGEN_MESH:
      {
        info(5) << "Found .mesh extension, using ViennaGrid Netgen Reader" << std::endl;

        viennagrid::io::netgen_reader reader;
        reader(output_mesh(), filename);

        success = true;
        break;
      }

    case STL:
    case STL_ASCII:
    case STL_BINARY:
      {
        info(5) << "Found .stl extension, using ViennaGrid STL Reader" << std::endl;

        data_handle<double> vertex_tolerance = get_input<double>("vertex_tolerance");

        viennagrid::io::stl_reader<> reader;
        if (vertex_tolerance.valid())
          reader = viennagrid::io::stl_reader<>( vertex_tolerance() );

        if (filetype == STL)
          reader(output_mesh(), filename);
        else if (filetype == STL_ASCII)
          reader.read_ascii(output_mesh(), filename);
        else if (filetype == STL_BINARY)
          reader.read_binary(output_mesh(), filename);

        success = true;
        break;
      }

    case GTS_DEVA:
      {
        info(5) << "Found .deva extension, using ViennaMesh GTS deva Reader" << std::endl;

        data_handle<bool> input_load_geometry = get_input<bool>("load_geometry");
        bool load_geometry = false;

        if (input_load_geometry.valid())
          load_geometry = input_load_geometry();

        viennagrid::io::gts_deva_reader reader;
        reader(output_mesh(), filename, load_geometry);

        success = true;
        break;
      }

    case VTK:
      {
        info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Reader" << std::endl;

        viennagrid::io::vtk_reader<viennagrid::mesh> reader;

        data_handle<bool> use_local_points = get_input<bool>("use_local_points");
        if (use_local_points.valid())
          reader.set_use_local_points( use_local_points() );

        reader(output_mesh(), filename);


        std::vector<viennagrid::quantity_field> quantity_fields = reader.quantity_fields();
        if (!quantity_fields.empty())
        {
          for (std::size_t i = 0; i != quantity_fields.size(); ++i)
          {
            info(1) << "Found quantity field \"" << quantity_fields[i].get_name() << "\" for topologic dimension " <<
                       (int)quantity_fields[i].topologic_dimension() << " and with values per quantity " <<
                       (int)quantity_fields[i].values_per_quantity() << std::endl;
          }

          quantity_field_handle output_quantity_fields = make_data<viennagrid::quantity_field>();
          output_quantity_fields.set(quantity_fields);
          set_output( "quantities", output_quantity_fields );
        }

        success = true;
        break;
      }

    case VTP:
      {
        info(5) << "Found .vtp extension" << std::endl;

        //ViennaGrid typedefs
        typedef viennagrid::mesh                                                        MeshType;
        typedef viennagrid::result_of::element<MeshType>::type                          VertexType;

        //read vtp file
        auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
        reader->SetFileName(filename.c_str());
        reader->Update();

        auto polydata = reader->GetOutput();

        //get basic mesh information
        int num_points = polydata->GetNumberOfPoints();
        int num_elements = polydata->GetNumberOfCells();

        //create empty vector of size NNodes containing viennagrid vertices
        std::vector<VertexType> vertex_handles(num_points);

        //iterating all pragmatic vertices and store their coordinates in the viennagrid vertices
        for(size_t i = 0; i < num_points; ++i)
        {
          double coords[3];
          polydata->GetPoint(i, coords);

          vertex_handles[i] = viennagrid::make_vertex( output_mesh(), viennagrid::make_point(coords[0], coords[1], coords[2]));
        } //end of for loop iterating all pragmatic vertices 

        //iterating all pragmatic elements and createg their corresponding viennagrid triangles
        for (size_t i = 0; i < num_elements; ++i)
        {
          auto celltype = polydata->GetCellType(i);

          if (celltype == 5)
          {

            auto idlist = vtkSmartPointer<vtkIdList>::New();
            polydata->GetCellPoints(i, idlist);

            viennagrid::make_triangle( output_mesh(), vertex_handles[ idlist->GetId(0) ], vertex_handles[ idlist->GetId(1) ], vertex_handles[ idlist->GetId(2) ]);
          }

          else
          {
            error(1) << "Found something else than a triangle!" << std::endl;
          }
        } //end of iterating all pragmatic elements

      
        //*/
        success = true;
        break;        
      }
    case GRD:
      {
        try
        {
          viennagrid::io::dfise_grd_dat_reader reader(filename);

          std::vector<viennagrid::quantity_field> quantity_fields;

          data_handle<viennamesh_string> datafiles = get_input<std::string>("datafiles");

          if (datafiles.valid())
          {
            std::vector<std::string> split_datafiles;
            std::string tmp_datafiles = datafiles();
            boost::algorithm::split(split_datafiles, tmp_datafiles, boost::is_any_of(","));
            for (unsigned int i = 0; i < split_datafiles.size(); ++i)
            {
            // future use:
            //  reader.read_dataset(datafiles(i));
              reader.read_dataset(split_datafiles[i]);
            }
          }

          data_handle<bool> extrude_contacts = get_input<bool>("extrude_contacts");
          //TODO make this uniform with TDR reader TODO
          reader.to_viennagrid( output_mesh(), quantity_fields, extrude_contacts.valid() ? extrude_contacts() : true );

          quantity_field_handle output_quantity_fields = make_data<viennagrid::quantity_field>();
          output_quantity_fields.set(quantity_fields);
          set_output( "quantities", output_quantity_fields );

          success = true;
        }
        catch(viennagrid::io::dfise_grd_dat_reader::error const & e)
        {
          error(1) << "GRID reader: got error: " << e.what() << std::endl;
        }
        break;
      }
    default:
      {
        error(1) << "Unsupported extension: " << lexical_cast<std::string>(filetype) << std::endl;
        return false;
      }
    }

    if (success)
    {
      info(1) << "Successfully read a mesh" << std::endl;
      info(1) << "  Geometric dimension = " << (int)viennagrid::geometric_dimension( output_mesh() ) << std::endl;
      info(1) << "  Cell dimension = " << (int)viennagrid::cell_dimension( output_mesh() ) << std::endl;
      info(1) << "  Vertex count =  " << viennagrid::vertices( output_mesh() ).size() << std::endl;
      info(1) << "  Cell count = " << viennagrid::cells( output_mesh() ).size() << std::endl;

      set_output("mesh", output_mesh);
    }

    return success;
  }




  bool mesh_reader::run(viennamesh::algorithm_handle &)
  {
    string_handle filename = get_required_input<string_handle>("filename");
    string_handle filetype = get_input<string_handle>("filetype");

    FileType ft;
    if (filetype.valid())
      ft = lexical_cast<FileType>( filetype() );
    else
      ft = from_filename( filename() );

    info(1) << "Using file type " << lexical_cast<std::string>(ft) << std::endl;

    std::string path = base_path();

    if (!path.empty())
    {
      info(1) << "Using base path: " << path << std::endl;
      return load( path + "/" + filename(), ft);
    }
    else
      return load(filename(), ft);
  }

}
