#ifndef VIENNAMESH_ALGORITHM_TDR_WRITER_HPP
#define VIENNAMESH_ALGORITHM_TDR_WRITER_HPP

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

#include "viennameshpp/plugin.hpp"

namespace viennamesh
{

class tdr_writer : public plugin_algorithm
{
public:
  tdr_writer();

  static std::string name();
  std::string id() const;

  bool run(viennamesh::algorithm_handle &);
};

} //end of namespace viennamesh

#endif
