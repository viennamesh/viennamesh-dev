#include "viennameshpp/common.hpp"

namespace viennamesh
{
  std::string extract_filename( std::string const & path )
  {
    std::string path_delimiters = "\\/";

    std::size_t pos = path.find_last_of(path_delimiters);
    if (pos == std::string::npos)
      return path;
    return path.substr(pos+1, std::string::npos);
  }

  std::string extract_path( std::string const & path )
  {
    std::string path_delimiters = "\\/";

    std::size_t pos = path.find_last_of(path_delimiters);
    if (pos == std::string::npos)
      return "";
    return path.substr(0, pos+1);
  }
}
