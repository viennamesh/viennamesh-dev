#ifndef VIENNAMESH_ALGORITHM_IO_COMMON_HPP
#define VIENNAMESH_ALGORITHM_IO_COMMON_HPP

#include "viennamesh/forwards.hpp"

namespace viennamesh
{
  namespace io
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
      NEPER_TESS
    };

    FileType from_filename( string filename );
    FileType from_string( string str );
    string to_string( FileType file_type );
  }
}



#endif
