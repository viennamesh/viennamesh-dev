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
#include "viennameshpp/forwards.hpp"

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

    if ( extension_found(filename, "grd") )
      return GRD;

    return UNKNOWN;
  }




  std::istream & operator>>(std::istream & stream, FileType & file_type)
  {
    std::string str;
    stream >> str;

    std::transform( str.begin(), str.end(), str.begin(), ::toupper );

    if (str == "VTK")
      file_type = VTK;
    else if (str == "VMESH")
      file_type = VMESH;

    else if (str == "TETGEN_POLY")
      file_type = TETGEN_POLY;
    else if (str == "NETGEN_MESH")
      file_type = NETGEN_MESH;

    else if (str == "GTS_DEVA")
      file_type = GTS_DEVA;
    else if (str == "SYNOPSIS_BND")
      file_type = SYNOPSIS_BND;
    else if (str == "COMSOL_MPHTXT")
      file_type = COMSOL_MPHTXT;
    else if (str == "NEPER_TESS")
      file_type = NEPER_TESS;

    else if (str == "OCC_STEP")
      file_type = NEPER_TESS;
    else if (str == "OCC_IGES")
      file_type = NEPER_TESS;

    else if (str == "SENTAURUS_TDR")
      file_type = SENTAURUS_TDR;

    else if (str == "SILVACO_STR")
      file_type = SILVACO_STR;

    else if (str == "STL")
      file_type = STL;
    else if (str == "STL_ASCII")
      file_type = STL_ASCII;
    else if (str == "STL_BINARY")
      file_type = STL_BINARY;
    else if (str == "GRD")
      file_type = GRD;
    else
      file_type = UNKNOWN;

    return stream;
  }

  std::ostream & operator<<(std::ostream & stream, FileType file_type)
  {
    switch (file_type)
    {
      case VTK:
        stream << "VTK";
        break;
      case VMESH:
        stream << "VMESH";
        break;

      case TETGEN_POLY:
        stream << "TETGEN_POLY";
        break;
      case NETGEN_MESH:
        stream << "NETGEN_MESH";
        break;

      case GTS_DEVA:
        stream << "GTS_DEVA";
        break;
      case SYNOPSIS_BND:
        stream << "SYNOPSIS_BND";
        break;
      case COMSOL_MPHTXT:
        stream << "COMSOL_MPHTXT";
        break;
      case NEPER_TESS:
        stream << "NEPER_TESS";
        break;

      case OCC_STEP:
        stream << "OCC_STEP";
        break;
      case OCC_IGES:
        stream << "OCC_IGES";
        break;

      case SENTAURUS_TDR:
        stream << "SENTAURUS_TDR";
        break;

      case SILVACO_STR:
        stream << "SILVACO_STR";
        break;

      case STL:
        stream << "STL";
        break;
      case STL_ASCII:
        stream << "STL_ASCII";
        break;
      case STL_BINARY:
        stream << "STL_BINARY";
        break;
      case GRD:
        stream << "GRD";
        break;
      default:
        stream << "UNKNOWN";
    }

    return stream;
  }



  std::string make_filename(std::string const & filename,
                            FileType ft)
  {
    std::string filename_no_extension = filename.substr(0, filename.rfind("."));
//     std::string file_extension = filename.substr(filename.rfind(".")+1);

    if (ft == VTK)
      return filename_no_extension;
    else
      return filename;
  }

  std::string make_filename(std::string const & filename,
                            FileType ft,
                            int index)
  {
    std::string filename_no_extension = filename.substr(0, filename.rfind("."));
    std::string file_extension = filename.substr(filename.rfind(".")+1);

    if (ft == VTK)
      return filename_no_extension + "_" + lexical_cast<std::string>(index);
    else
      return filename_no_extension + "_" + lexical_cast<std::string>(index) + "." + file_extension;
  }

}
