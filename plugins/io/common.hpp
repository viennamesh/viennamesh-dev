#ifndef VIENNAMESH_ALGORITHM_IO_COMMON_HPP
#define VIENNAMESH_ALGORITHM_IO_COMMON_HPP

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

#include <string>

namespace viennamesh
{
  enum FileType
  {
    UNKNOWN,
    VTK,
    VMESH,
    TETGEN_POLY,
    NETGEN_MESH,
    GTS_DEVA,
    SYNOPSIS_BND,
    COMSOL_MPHTXT,
    NEPER_TESS,
    OCC_STEP,
    OCC_IGES,
    SENTAURUS_TDR,
    SILVACO_STR,
    STL,
    STL_ASCII,
    STL_BINARY
  };

  FileType from_filename( std::string filename );
  FileType from_string( std::string str );
  std::string to_string( FileType file_type );
}

#endif
