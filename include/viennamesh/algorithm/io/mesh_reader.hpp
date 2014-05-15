#ifndef VIENNAMESH_ALGORITHM_IO_MESH_READER_HPP
#define VIENNAMESH_ALGORITHM_IO_MESH_READER_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/io/common.hpp"
#include "pugixml/pugixml.hpp"

namespace viennamesh
{
  namespace io
  {
    class mesh_reader : public base_algorithm
    {
    public:
      mesh_reader();

      std::string name() const;
      std::string id() const;

      bool run_impl();

    private:

      template<typename CellTagT, unsigned int GeometricDimensionV>
      bool generic_read_vtk( std::string const & filename );

      template<int GeometricDimensionV>
      bool read_seed_points( pugi::xml_document const & xml );

      bool read_vmesh( std::string const & filename );

      bool load( std::string const & filename, FileType file_type );


      required_input_parameter_interface<std::string>   filename;
      optional_input_parameter_interface<std::string>   filetype;

      output_parameter_interface                        output_mesh;
      output_parameter_interface                        output_quantities;
      output_parameter_interface                        output_seed_points;
      output_parameter_interface                        output_hole_points;
    };

  }

}



#endif
