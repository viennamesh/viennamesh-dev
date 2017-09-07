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

#include <sstream>

#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkTriangle.h>

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
/*
    info(1) << "Number of vertices of mesh to write " << viennagrid::vertices(input_mesh()).size() << std::endl;
    info(1) << "Number of cells of mesh to write " << viennagrid::cells(input_mesh()).size() << std::endl;
*/
    int vertices = 0;
    int elements = 0;

    for (size_t i = 0; i < input_mesh.size(); ++i)
    {
      vertices += viennagrid::vertices(input_mesh(i)).size();
      elements += viennagrid::cells(input_mesh(i)).size();
    }

    viennamesh::info(1) << "Number of Meshes  : " << input_mesh.size() << std::endl;
    viennamesh::info(1) << "Number of Vertices: " << vertices << std::endl;
    viennamesh::info(1) << "Number of Elements: " << elements << std::endl;

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

        case VTP:
        {
          std::ostringstream ostr;

          ostr << local_filename;

          auto polydata = vtkSmartPointer<vtkPolyData>::New();
          auto points = vtkSmartPointer<vtkPoints>::New();
          auto cells = vtkSmartPointer<vtkCellArray>::New();
          auto data = vtkSmartPointer<vtkDoubleArray>::New();
          data->SetNumberOfComponents(1);

          size_t NNodes = viennagrid::vertex_count(mesh);
          size_t NElements = viennagrid::cells(mesh).size();
          size_t cell_dimension = viennagrid::cell_dimension(mesh);
          size_t geometric_dimension = viennagrid::geometric_dimension(mesh);
          
          //Initialize x-, y-, and, z-coordinates
          //
          //create pointer to iterate over viennagrid_array
          viennagrid_numeric* ptr_coords = nullptr;

          //get pointer to coordinates array from the mesh
          viennagrid_mesh_vertex_coords_pointer(mesh.internal(), &ptr_coords);

          for (size_t i = 0; i < NNodes; ++i)
          {
            if (geometric_dimension == 2)
            {
              double xyz[3] = {*(ptr_coords++), *(ptr_coords++), 0.0};
              points->InsertNextPoint(xyz);
            }

            else
            {
              double xyz[3] = {*(ptr_coords++), *(ptr_coords++), *(ptr_coords++)};
              points->InsertNextPoint(xyz);
            }
          }

          int triIdx = 0;

          //Iterate over all triangles in the mesh
          viennagrid_element_id * element_ids_begin;
          viennagrid_element_id * element_ids_end;
          viennagrid_dimension topological_dimension = geometric_dimension;		//produces segmentation fault if not set to 2 for 2d and to 3 for 3d case

          //get elements from mesh
          viennagrid_mesh_elements_get(mesh.internal(), topological_dimension, &element_ids_begin, &element_ids_end);

          viennagrid_element_id * boundary_vertex_ids_begin;
          viennagrid_element_id * boundary_vertex_ids_end;
          viennagrid_dimension boundary_topological_dimension = 0;
          viennagrid_element_id triangle_id;

          //outer for loop iterates over all elements with dimension = topological_dimension (2 for triangles, 3 for tetrahedrons)
          //inner for loop iterates over all elements with dimension = boundary_topological_dimension (0 for vertices)
          for (viennagrid_element_id * vit = element_ids_begin; vit != element_ids_end; ++vit)
          {
            //get vertices of triangle
            triangle_id = *vit;
            viennagrid_element_boundary_elements(mesh.internal(), triangle_id, boundary_topological_dimension, &boundary_vertex_ids_begin, &boundary_vertex_ids_end);

            auto tmptriangle = vtkSmartPointer<vtkTriangle>::New();

            int vid = 0;
            for (viennagrid_element_id * bit = boundary_vertex_ids_begin; bit != boundary_vertex_ids_end; ++bit)
            {
                viennagrid_element_id vertex_id = *bit;

                tmptriangle->GetPointIds()->SetId(vid, *bit);

                ++vid;
            }

            cells->InsertNextCell(tmptriangle);
          }

          polydata->SetPoints(points);
          polydata->SetPolys(cells);

          auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
          writer->SetFileName(ostr.str().c_str());
          writer->SetInputData(polydata);
          // writer->SetDataModeToAscii();
          writer->SetDataModeToBinary();
          writer->Write();

          break;
        }

        default:
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED, "File type \"" + lexical_cast<std::string>(ft) + "\" not supported");
      }
    }

    return true;
  }
}
