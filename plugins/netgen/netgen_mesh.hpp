#ifndef VIENNAMESH_ALGORITHM_NETGEN_MESH_HPP
#define VIENNAMESH_ALGORITHM_NETGEN_MESH_HPP

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

#include <list>
#include "viennameshpp/plugin.hpp"

#include "external/libsrc/csg/csg.hpp"


namespace viennamesh
{
  namespace netgen
  {
    typedef ::netgen::Mesh mesh;
  }



  viennamesh_error convert(viennagrid::mesh const & input, netgen::mesh & output);
  viennamesh_error convert(netgen::mesh const & input, viennagrid::mesh & output);

  template<>
  viennamesh_error internal_convert<viennagrid_mesh, netgen::mesh>(viennagrid_mesh const & input, netgen::mesh & output);
  template<>
  viennamesh_error internal_convert<netgen::mesh, viennagrid_mesh>(netgen::mesh const & input, viennagrid_mesh & output);

  namespace result_of
  {
    template<>
    struct data_information<netgen::mesh>
    {
      static std::string type_name() { return "netgen::mesh"; }
      static viennamesh_data_make_function make_function() { return viennamesh::generic_make<netgen::mesh>; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::generic_delete<netgen::mesh>; }

    };
  }


}

#endif
