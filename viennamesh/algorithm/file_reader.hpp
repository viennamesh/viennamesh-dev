#ifndef VIENNAMESH_ALGORITHM_FILE_READER_HPP
#define VIENNAMESH_ALGORITHM_FILE_READER_HPP

#include "viennamesh/core/dynamic_algorithm.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/tetgen_poly_reader.hpp"
#include <boost/concept_check.hpp>


namespace viennamesh
{


  class FileReader : public BaseAlgorithm
  {
  public:

    bool run_impl()
    {
      LoggingStack stack( "Algoritm: FileReader" );

      ConstStringParameterHandle param = inputs.get<string>("filename");
      if (!param)
      {
        error(1) << "Input Parameter 'filename' (type: string) is missing" << std::endl;
        return false;
      }

      string filename = param->value;
      string extension = filename.substr( filename.rfind(".")+1 );

      ParameterHandle result;

      if (extension == "mesh")
      {
        info(5) << "Found .mesh extension, using ViennaGrid Netgen Reader" << std::endl;
        typedef ParameterWrapper< MeshWrapper<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> > MeshType;
        shared_ptr<MeshType> output( new MeshType() );

        viennagrid::io::netgen_reader reader;
        reader(output->value.mesh, output->value.segmentation, filename);

        outputs.set("default", output);
        return true;
      }
      else if (extension == "poly")
      {
        info(5) << "Found .poly extension, using ViennaGrid Tetgen polg Reader" << std::endl;

        viennagrid::io::tetgen_poly_reader reader;

        try
        {
          typedef ParameterWrapper< MeshWrapper<viennagrid::plc_3d_mesh, viennagrid::plc_3d_segmentation> > MeshType;
          typedef viennagrid::config::point_type_3d PointType;
          shared_ptr<MeshType> output( new MeshType() );

          std::vector<PointType> hole_points;
          std::vector< std::pair<PointType, int> > seed_points;

          reader(output->value.mesh, filename, hole_points, seed_points);

          if (!hole_points.empty())
            outputs.set("hole_points", hole_points);
          if (!seed_points.empty())
            outputs.set("seed_points", seed_points);
          outputs.set("default", output);
          return true;
        }
        catch (viennagrid::io::bad_file_format_exception const & ) {}

        try
        {
          typedef ParameterWrapper< MeshWrapper<viennagrid::plc_2d_mesh, viennagrid::plc_2d_segmentation> > MeshType;
          typedef viennagrid::config::point_type_2d PointType;
          shared_ptr<MeshType> output( new MeshType() );

          std::vector<PointType> hole_points;
          std::vector< std::pair<PointType, int> > seed_points;

          reader(output->value.mesh, filename, hole_points, seed_points);

          if (!hole_points.empty())
            outputs.set("hole_points", hole_points);
          if (!seed_points.empty())
            outputs.set("seed_points", seed_points);
          outputs.set("default", output);
          return true;
        }
        catch (viennagrid::io::bad_file_format_exception const & ) {}

        return false;
      }
      else if (extension == "vtu" || extension == "pvd")
      {
        info(5) << "Found .vtu/.pvd extension, using ViennaGrid VTK Reader" << std::endl;

        std::ifstream file(filename.c_str());
        if (file)
        {
          int geometric_dimension;
          int topologic_dimension;
          int num_cells;

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
            typedef ParameterWrapper< MeshWrapper<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> > MeshType;
            shared_ptr<MeshType> output( new MeshType() );

            viennagrid::io::vtk_reader<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> vtk_writer;
            vtk_writer(output->value.mesh, output->value.segmentation, filename);

            outputs.set("default", output);
            return true;
          }
          else if ( (geometric_dimension == 3) && (topologic_dimension == 3) )
          {
            typedef ParameterWrapper< MeshWrapper<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> > MeshType;
            shared_ptr<MeshType> output( new MeshType() );

            viennagrid::io::vtk_reader<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> vtk_writer;
            vtk_writer(output->value.mesh, output->value.segmentation, filename);

            outputs.set("default", output);
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
