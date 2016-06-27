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

#include "mesh_writer.hpp"

#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/mphtxt_writer.hpp"

#include "pugixml.hpp"

#include "viennameshpp/core.hpp"

namespace viennamesh
{
  mesh_writer::mesh_writer() {}
  std::string mesh_writer::name() { return "mesh_writer"; }


  bool mesh_writer::run(viennamesh::algorithm_handle &)
  {
    string_handle filename = get_required_input<string_handle>("filename");
    string_handle filetype = get_input<string_handle>("filetype");

    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    quantity_field_handle quantity_field = get_input<viennagrid::quantity_field>("quantities");

    if (input_mesh.size() != 1 && quantity_field.valid())
      warning(1) << "Input mesh count is " << lexical_cast<std::string>(input_mesh.size()) << " and quantity fields found -> ignoring quantity fields" << std::endl;

    info(1) << "Number of vertices of mesh to write " << viennagrid::vertices(input_mesh()).size() << std::endl;
    info(1) << "Number of cells of mesh to write " << viennagrid::cells(input_mesh()).size() << std::endl;

    if (!filename.valid())
      VIENNAMESH_ERROR(VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED, "Input parameter \"filename\" not found");

    info(1) << "Writing mesh to file \"" << filename() << "\"" << std::endl;

    FileType ft;
    if (filetype.valid())
      ft = lexical_cast<FileType>( filetype() );
    else
      ft = from_filename( filename() );

    std::string filename_no_extension = filename().substr(0, filename().rfind("."));

    info(1) << "Using file type " << lexical_cast<std::string>(ft) << std::endl;

    viennagrid_dimension geometric_dimension = viennagrid::geometric_dimension( input_mesh() );
    viennagrid_dimension cell_dimension = viennagrid::topologic_dimension( input_mesh() );

    if (input_mesh.size() != 1)
      info(1) << "Found " << input_mesh.size() << " meshes" << std::endl;

    for (int i = 0; i != input_mesh.size(); ++i)
    {
      viennagrid::mesh mesh = input_mesh(i);
      std::string local_filename;
      if (input_mesh.size() == 1)
        local_filename = make_filename(filename(), ft);
      else
        local_filename = make_filename(filename(), ft, i);

      LoggingStack ls("Mesh " + lexical_cast<std::string>(i));
      info(1) << "Using filename \"" << local_filename << "\"" << std::endl;
      info(1) << "Found geometric dimension: " << (int)geometric_dimension << std::endl;
      info(1) << "Found cell dimension: " << (int)cell_dimension << std::endl;

      switch (ft)
      {
        case VTK:
        {
          viennagrid::io::vtk_writer<viennagrid::mesh> writer;

          if (input_mesh.size() == 1 && quantity_field.valid())
          {
            int quantity_field_count = quantity_field.size();
            for (int i = 0; i != quantity_field_count; ++i)
            {
              viennagrid::quantity_field current = quantity_field(i);

              info(1) << "Found quantity field \"" << current.get_name() << "\" with cell dimension " << (int)current.topologic_dimension() << " and values dimension " << (int)current.values_per_quantity() << std::endl;

              if ( (current.topologic_dimension() != 0) && (current.topologic_dimension() != cell_dimension) )
              {
                error(1) << "Values dimension " << (int)current.values_per_quantity() << " for quantitiy field \"" << current.get_name() << "\" not supported -> skipping" << std::endl;
                continue;
              }

              if ( (current.values_per_quantity() != 1) && (current.values_per_quantity() != 3) )
              {
                error(1) << "Values dimension " << (int)current.values_per_quantity() << " for quantitiy field \"" << current.get_name() << "\" not supported -> skipping" << std::endl;
                continue;
              }


              if ( current.topologic_dimension() == 0 )
              {
                if ( current.values_per_quantity() == 1 )
                {
                  writer.add_scalar_data_on_vertices( current, current.get_name() );
                }
                else
                {
                  writer.add_vector_data_on_vertices( current, current.get_name() );
                }
              }
              else if ( current.topologic_dimension() == cell_dimension )
              {
                if ( current.values_per_quantity() == 1 )
                {
                  writer.add_scalar_data_on_cells( current, current.get_name() );
                }
                else
                {
                  writer.add_vector_data_on_cells( current, current.get_name() );
                }
              }

            }
          }

          writer( mesh, local_filename );
          break;
        }

        case COMSOL_MPHTXT:
        {
          if ( geometric_dimension != 3 || cell_dimension != 3)
            VIENNAMESH_ERROR(VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED, "MPHTXT writer not supported for geometric dimension " + lexical_cast<std::string>(geometric_dimension) + " and cell dimension " + lexical_cast<std::string>(cell_dimension));

          viennagrid::io::mphtxt_writer writer;
          writer( mesh, local_filename );
          break;
        }

        default:
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED, "File type \"" + lexical_cast<std::string>(ft) + "\" not supported");
      }
    }

    return true;
  }
}
