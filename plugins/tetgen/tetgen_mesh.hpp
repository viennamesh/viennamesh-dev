#ifndef VIENNAMESH_ALGORITHM_TETGEN_MESH_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_MESH_HPP

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

#ifndef TETLIBRARY
  #define TETLIBRARY
#endif
#include "external/tetgen.h"


namespace viennamesh
{
  namespace tetgen
  {
    typedef tetgenio mesh;
  }

  viennamesh_error convert(viennagrid_plc input, tetgen::mesh & output);
  viennamesh_error convert(viennagrid::mesh const & input, tetgen::mesh & output);
  viennamesh_error convert(tetgen::mesh const & input, viennagrid::mesh & output);

  template<>
  viennamesh_error internal_convert<viennagrid_plc, tetgen::mesh>(viennagrid_plc const & input, tetgen::mesh & output);
  template<>
  viennamesh_error internal_convert<viennagrid_mesh, tetgen::mesh>(viennagrid_mesh const & input, tetgen::mesh & output);
  template<>
  viennamesh_error internal_convert<tetgen::mesh, viennagrid_mesh>(tetgen::mesh const & input, viennagrid_mesh & output);

  namespace result_of
  {
    template<>
    struct data_information<tetgen::mesh>
    {
      static std::string type_name() { return "tetgen::mesh"; }
      static viennamesh_data_make_function make_function() { return viennamesh::generic_make<tetgen::mesh>; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::generic_delete<tetgen::mesh>; }
    };
  }


}

#endif
