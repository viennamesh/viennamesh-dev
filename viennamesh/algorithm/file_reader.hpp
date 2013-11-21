#ifndef VIENNAMESH_ALGORITHM_FILE_READER_HPP
#define VIENNAMESH_ALGORITHM_FILE_READER_HPP

#include "viennamesh/core/algorithm.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/tetgen_poly_reader.hpp"


namespace viennamesh
{


  class file_reader : public base_algorithm
  {
  public:

    string name() const { return "ViennaGrid FileReader"; }

    bool run_impl()
    {
      const_string_parameter_handle param = get_required_input<string>("filename");

      string filename = param->value();
      string extension = filename.substr( filename.rfind(".")+1 );

      parameter_handle result;

      if (extension == "mesh")
      {
        info(5) << "Found .mesh extension, using ViennaGrid Netgen Reader" << std::endl;
        typedef viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> MeshType;

        output_parameter_proxy<MeshType> output_mesh = output_proxy<MeshType>("default");

        viennagrid::io::netgen_reader reader;
        reader(output_mesh().mesh, output_mesh().segmentation, filename);

        return true;
      }
      else if (extension == "poly")
      {
        info(5) << "Found .poly extension, using ViennaGrid Tetgen poly Reader" << std::endl;

        viennagrid::io::tetgen_poly_reader reader;

        try
        {
          typedef viennagrid::brep_3d_mesh MeshType;

          output_parameter_proxy<MeshType> output_mesh = output_proxy<MeshType>("default");
          output_parameter_proxy<point_3d_container> output_hole_points = output_proxy<point_3d_container>("hole_points");
          output_parameter_proxy<seed_point_3d_container> output_seed_points = output_proxy<seed_point_3d_container>("seed_points");

          point_3d_container hole_points;
          seed_point_3d_container seed_points;

          output_mesh().clear();
          reader(output_mesh(), filename, hole_points, seed_points);

          if (!hole_points.empty())
          {
            info(1) << "Found hole points (" << hole_points.size() << ")" << std::endl;
            output_hole_points() = hole_points;
          }
          else
            unset_output("hole_points");


          if (!seed_points.empty())
          {
            info(1) << "Found seed points (" << seed_points.size() << ")" << std::endl;
            output_seed_points() = seed_points;
          }
          else
            unset_output("seed_points");

          return true;
        }
        catch (viennagrid::io::bad_file_format_exception const & ) {}

//         try
//         {
//           typedef viennagrid::brep_2d_mesh MeshType;
//
//           output_parameter_proxy<MeshType> output_mesh = output_proxy<MeshType>("default");
//           output_parameter_proxy<point_2d_container> output_hole_points = output_proxy<point_2d_container>("hole_points");
//           output_parameter_proxy<seed_point_2d_container> output_seed_points = output_proxy<seed_point_2d_container>("seed_points");
//
//
//           point_2d_container hole_points;
//           seed_point_2d_container seed_points;
//
//           reader(output_mesh(), filename, hole_points, seed_points);
//
//
//           if (!hole_points.empty())
//           {
//             info(1) << "Found hole points (" << hole_points.size() << ")" << std::endl;
//             output_hole_points() = hole_points;
//           }
//
//           if (!seed_points.empty())
//           {
//             info(1) << "Found seed points (" << seed_points.size() << ")" << std::endl;
//             output_seed_points() = seed_points;
//           }
//
//           return true;
//         }
//         catch (viennagrid::io::bad_file_format_exception const & ) {}

        return false;
      }
      else if (extension == "vtu" || extension == "pvd")
      {
        info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Reader" << std::endl;

        std::ifstream file(filename.c_str());
        if (file)
        {
          int geometric_dimension = 0;
          int topologic_dimension = 0;
          int num_cells = 0;

          string line;
          while (std::getline(file, line))
          {
            if ( line.find("<Piece") != string::npos )
            {
              string::size_type pos1 = line.find("NumberOfCells") + 15;
              string::size_type pos2 = line.find("\"", pos1+1);
              num_cells = atoi(line.substr(pos1, pos2-pos1).c_str());
              break;
            }
          }

          while (std::getline(file, line))
          {
            if ( line.find("<Points>") != string::npos )
            {
              while (std::getline(file, line))
              {
//                 if ( line.find("<DataArray") != string::npos )
                {
                  string::size_type pos1 = line.find("NumberOfComponents") + 20;
                  string::size_type pos2 = line.find("\"", pos1+1);
                  geometric_dimension = atoi(line.substr(pos1, pos2-pos1).c_str());
                  break;
                }
              }
              break;
            }
          }

          while (std::getline(file, line))
          {
            if ( line.find("<Cells>") != string::npos )
            {
              while (std::getline(file, line))
              {
                if ( line.find("<DataArray") != string::npos )
                {
                  int counter = -1;
                  while (file)
                  {
                    double bla;
                    file >> bla;
                    ++counter;
                  }

                  topologic_dimension = counter / num_cells - 1;
                }
              }
              break;
            }
          }

          info(5) << "Geometric dimension: " << geometric_dimension << std::endl;
          info(5) << "Topologic dimension: " << topologic_dimension << std::endl;

          if ( (geometric_dimension == 3) && (topologic_dimension == 2) )
          {
            typedef viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> MeshType;
            output_parameter_proxy<MeshType> output_mesh = output_proxy<MeshType>("default");

            viennagrid::io::vtk_reader<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> vtk_writer;
            vtk_writer(output_mesh().mesh, output_mesh().segmentation, filename);
            return true;
          }
          else if ( (geometric_dimension == 3) && (topologic_dimension == 3) )
          {
            typedef viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> MeshType;
            output_parameter_proxy<MeshType> output_mesh = output_proxy<MeshType>("default");

            viennagrid::io::vtk_reader<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> vtk_writer;
            vtk_writer(output_mesh().mesh, output_mesh().segmentation, filename);
            return true;
          }
          else
          {
            error(1) << "Combination of geometric and topologic dimension is not supported" << std::endl;
            return false;
          }
        }
      }
      else
      {
        error(1) << "Unsupported extension: " << extension << std::endl;
        return false;
      }

      return false;
    }

  private:

  };



}



#endif
