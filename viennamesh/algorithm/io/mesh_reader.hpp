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

      string name() const { return "ViennaGrid Mesh Reader"; }
      bool run_impl();

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
