#ifndef VIENNAMESH_ALGORITHM_IO_PLC_READER_HPP
#define VIENNAMESH_ALGORITHM_IO_PLC_READER_HPP

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
  class plc_reader : public plugin_algorithm
  {
  public:
    plc_reader();

    static std::string name();
    std::string id() const;

    bool run(viennamesh::algorithm_handle &);
  };

}



#endif
