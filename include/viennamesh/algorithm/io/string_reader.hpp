#ifndef VIENNAMESH_ALGORITHM_IO_STRING_READER_HPP
#define VIENNAMESH_ALGORITHM_IO_STRING_READER_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  namespace io
  {

    class string_reader : public base_algorithm
    {
    public:

      string name() const { return "ViennaGrid String Reader"; }
      bool run_impl()
      {
        const_string_parameter_handle param = get_required_input<string>("filename");

        std::ifstream file( param().c_str() );

        if (!file)
          return false;

        output_parameter_proxy<string> output = output_proxy<string>("default");

        output() = stringtools::read_stream(file);

        return true;
      }

    private:

      template<typename CellTagT, unsigned int GeometricDimensionV>
      bool generic_read_vtk( string const & filename );

      template<int GeometricDimensionV>
      bool read_seed_points( pugi::xml_document const & xml );

      bool read_vmesh( string const & filename );

      bool load( string const & filename, FileType file_type );

    };

  }

}



#endif
