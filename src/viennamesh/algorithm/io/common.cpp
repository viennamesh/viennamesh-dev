#include "viennamesh/algorithm/io/common.hpp"

namespace viennamesh
{
  namespace io
  {
    bool extension_found( string const & filename, string const & extension )
    {
      string ext_w_pt = ".";
      ext_w_pt += extension;

      if (filename.length() < ext_w_pt.length())
        return false;

      std::transform( ext_w_pt.begin(), ext_w_pt.end(), ext_w_pt.begin(), ::toupper );

      return filename.rfind(ext_w_pt) == filename.length()-ext_w_pt.length();
    }

    FileType from_filename( string filename )
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

      return UNKNOWN;
    }

    FileType from_string( string str )
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

      return UNKNOWN;
    }

    string to_string( FileType file_type )
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

        default:
          return "UNKNOWN";
      }
    }
  }
}
