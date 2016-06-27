#ifndef VIENNAMESH_ALGORITHM_IO_MESH_READER_HPP
#define VIENNAMESH_ALGORITHM_IO_MESH_READER_HPP

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

#include "common.hpp"
#include "viennameshpp/plugin.hpp"

namespace viennamesh
{
  class mesh_reader : public plugin_algorithm
  {
  public:
    mesh_reader();

    static std::string name();
    std::string id() const;

    bool run(viennamesh::algorithm_handle &);

  private:

    bool load( std::string const & filename, FileType file_type );
  };

}



#endif
