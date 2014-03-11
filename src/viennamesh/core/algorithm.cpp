#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{

  parameter_handle_t<parameter_link> make_parameter_link(algorithm_handle const & algorithm, string const & para_name)
  {
    return parameter_handle_t<parameter_link>( new parameter_link(algorithm, para_name) );
  }

  bool base_algorithm::run()
  {
    LoggingStack stack( string("Algoritm: ") + name() );
    outputs.clear_non_references();

    try
    {
      bool success = run_impl();

      if (!success)
        error(1) << "Algorithm failed" << std::endl;

      return success;
    }
    catch ( algorithm_exception const & ex )
    {
      error(1) << "Algorithm failed!" << std::endl;
      error(1) << ex.what() << std::endl;
      return false;
    }
  }



}
