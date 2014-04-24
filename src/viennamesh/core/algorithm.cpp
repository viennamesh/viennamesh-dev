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

    for (RegisteredParameterMapType::iterator ipit = input_parameters.begin(); ipit != input_parameters.end(); ++ipit)
      (*ipit).second->reset();

    try
    {
      bool success = run_impl();

      if (!success)
        error(1) << "Algorithm failed!" << std::endl;

      return success;
    }
    catch ( std::exception const & ex )
    {
      error(1) << "Algorithm failed!" << std::endl;
      error(1) << ex.what() << std::endl;
      return false;
    }
  }

  void base_algorithm::register_input_parameter( base_parameter_interface & input_parameter )
  {
    if ( !input_parameters.insert(std::make_pair(input_parameter.name(), &input_parameter)).second )
      error(1) << "Input parameter \"" << input_parameter.name() << "\" of algorithm \"" << name() << "\" registered twice. This is a bug, please report!" << std::endl;
  }

  void base_algorithm::register_output_parameter( base_parameter_interface & output_parameter )
  {
    if ( !output_parameters.insert(std::make_pair(output_parameter.name(), &output_parameter)).second )
      error(1) << "Output parameter \"" << output_parameter.name() << "\" of algorithm \"" << name() << "\" registered twice. This is a bug, please report!" << std::endl;
  }

  bool base_algorithm::is_input_registered(std::string const & name) const
  {
    return input_parameters.find(name) != input_parameters.end();
  }

  bool base_algorithm::is_output_registered(std::string const & name) const
  {
    return output_parameters.find(name) != output_parameters.end();
  }

}
