#ifndef VIENNAMESH_ALGORITHM_IO_MESH_WRITER_HPP
#define VIENNAMESH_ALGORITHM_IO_MESH_WRITER_HPP

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
#include "viennamesh_plugin.hpp"

namespace viennamesh
{

  class mesh_writer : public plugin_algorithm
  {
    friend struct vmesh_writer_proxy;

  public:

    mesh_writer();
    static std::string name();
    bool run(viennamesh::algorithm_handle &);

  private:

    template<typename WriterProxyT>
    bool write_all( mesh_handle input_mesh, std::string const & filename );

    bool write_mphtxt( mesh_handle input_mesh, std::string const & filename );
  };

}



#endif
