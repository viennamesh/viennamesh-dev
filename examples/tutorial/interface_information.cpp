#include "viennamesh/viennamesh.hpp"

int main()
{
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  std::cout << "Input parameters" << std::endl;
  for (viennamesh::base_algorithm::const_input_parameter_iterator it = reader->input_parameter_begin();
       it != reader->input_parameter_end(); ++it)
    {
      std::cout << "  " << (*it).name() << std::endl;
      std::cout << "    Type: " << (*it).type() << std::endl;
      std::cout << "    Description: " << (*it).description() << std::endl;
      std::cout << "    Default: " << (*it).default_value() << std::endl;
      std::cout << "    Check: " << (*it).check_string() << std::endl;
    }

  std::cout << "Output parameters" << std::endl;
  for (viennamesh::base_algorithm::const_output_parameter_iterator it = reader->output_parameter_begin();
       it != reader->output_parameter_end(); ++it)
    {
      std::cout << "  " << (*it).name() << std::endl;
      std::cout << "    Type: " << (*it).type() << std::endl;
      std::cout << "    Description: " << (*it).description() << std::endl;
    }
}
