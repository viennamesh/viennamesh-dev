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
#include <ostream>
#include <istream>

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
    STL_BINARY,
    GRD
  };

  FileType from_filename( std::string filename );

  std::ostream & operator<<(std::ostream & stream, FileType file_type);
  std::istream & operator>>(std::istream & stream, FileType & file_type);

  std::string make_filename(std::string const & filename,
                            FileType ft);

  std::string make_filename(std::string const & filename,
                            FileType ft,
                            int index);
}

#endif
