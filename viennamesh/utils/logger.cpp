#include "viennamesh/utils/logger.hpp"

viennamesh::Logger & viennamesh::logger()
{
  static bool is_init = false;
  static Logger logger_;

  if (!is_init)
  {
    logger_.register_callback( new OStreamCallback<CoutColorFormater>(std::cout) );
    is_init = true;
  }

  return logger_;
}
