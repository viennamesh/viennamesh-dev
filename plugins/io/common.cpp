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

#include <algorithm>
#include "common.hpp"

namespace viennamesh
{
  bool extension_found( std::string const & filename, std::string const & extension )
  {
    std::string ext_w_pt = ".";
    ext_w_pt += extension;

    if (filename.length() < ext_w_pt.length())
      return false;

    std::transform( ext_w_pt.begin(), ext_w_pt.end(), ext_w_pt.begin(), ::toupper );

    return filename.rfind(ext_w_pt) == filename.length()-ext_w_pt.length();
  }

  FileType from_filename( std::string filename )
  {
    std::transform( filename.begin(), filename.end(), filename.begin(), ::toupper );

    if ( extension_found(filename, "vtu") || extension_found(filename, "pvd") )
      return VTK;

    if ( extension_found(filename, "vmesh") )
      return VMESH;

    if ( extension_found(filename, "poly") )
      return TETGEN_POLY;

    if ( extension_found(filename, "mesh") )
      return NETGEN_MESH;

    if ( extension_found(filename, "deva") )
      return GTS_DEVA;

    if ( extension_found(filename, "bnd") )
      return SYNOPSIS_BND;

    if ( extension_found(filename, "mphtxt") )
      return COMSOL_MPHTXT;

    if ( extension_found(filename, "tess") )
      return NEPER_TESS;

    if ( extension_found(filename, "step") )
      return OCC_STEP;

    if ( extension_found(filename, "iges") )
      return OCC_IGES;

    if ( extension_found(filename, "tdr") )
      return SENTAURUS_TDR;

    if ( extension_found(filename, "str") )
      return SILVACO_STR;

    if ( extension_found(filename, "stl") )
      return STL;

    return UNKNOWN;
  }

  FileType from_string( std::string str )
  {
    std::transform( str.begin(), str.end(), str.begin(), ::toupper );

    if (str == "VTK")
      return VTK;
    if (str == "VMESH")
      return VMESH;

    if (str == "TETGEN_POLY")
      return TETGEN_POLY;
    if (str == "NETGEN_MESH")
      return NETGEN_MESH;

    if (str == "GTS_DEVA")
      return GTS_DEVA;
    if (str == "SYNOPSIS_BND")
      return SYNOPSIS_BND;
    if (str == "COMSOL_MPHTXT")
      return COMSOL_MPHTXT;
    if (str == "NEPER_TESS")
      return NEPER_TESS;

    if (str == "OCC_STEP")
      return NEPER_TESS;
    if (str == "OCC_IGES")
      return NEPER_TESS;

    if (str == "SENTAURUS_TDR")
      return SENTAURUS_TDR;

    if (str == "SILVACO_STR")
      return SILVACO_STR;

    if (str == "STL")
      return STL;
    if (str == "STL_ASCII")
      return STL_ASCII;
    if (str == "STL_BINARY")
      return STL_BINARY;

    return UNKNOWN;
  }

  std::string to_string( FileType file_type )
  {
    switch (file_type)
    {
      case VTK:
        return "VTK";
      case VMESH:
        return "VMESH";

      case TETGEN_POLY:
        return "TETGEN_POLY";
      case NETGEN_MESH:
        return "NETGEN_MESH";

      case GTS_DEVA:
        return "GTS_DEVA";
      case SYNOPSIS_BND:
        return "SYNOPSIS_BND";
      case COMSOL_MPHTXT:
        return "COMSOL_MPHTXT";
      case NEPER_TESS:
        return "NEPER_TESS";

      case OCC_STEP:
        return "OCC_STEP";
      case OCC_IGES:
        return "OCC_IGES";

      case SENTAURUS_TDR:
        return "SENTAURUS_TDR";

      case SILVACO_STR:
        return "SILVACO_STR";

      case STL:
        return "STL";
      case STL_ASCII:
        return "STL_ASCII";
      case STL_BINARY:
        return "STL_BINARY";

      default:
        return "UNKNOWN";
    }
  }
}
