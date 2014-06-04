%module(docstring="The ViennaMesh Python interface") pyviennamesh

%feature("autodoc", "3");
%typemap("doc") std::string const & parameter_name "$1_name (C++ type: $1_type) -- The name of the parameter"
%typemap("doc") bool value "$1_name (C++ type: $1_type) -- A boolean value"
%typemap("doc") int value "$1_name (C++ type: $1_type) -- An integer value"
%typemap("doc") double value "$1_name (C++ type: $1_type) -- An double value"
%typemap("doc") std::string const & value "$1_name (C++ type: $1_type) -- A string value"
%typemap("doc") viennamesh::algorithm_handle const & source_algorithm "$1_name (C++ type: $1_type) -- The source algorithm"
%typemap("doc") std::size_t index "$1_name (C++ type: $1_type) -- The index of the parameter, starting with 0"
%typemap("doc") std::string const & algorithm_id "$1_name (C++ type: $1_type) -- The algorithm id, see User Manual/Algorithms for algorithm ids"


%feature("autodoc", "3");

#define VIENNAMESH_USES_BOOST

%include <cpointer.i>
%include <std_string.i>
%include <boost_shared_ptr.i>
%shared_ptr(viennamesh::base_algorithm)

%{
  #include <boost/shared_ptr.hpp>
  #include "viennamesh/forwards.hpp"
  #include "viennamesh/core/parameter.hpp"
  #include "viennamesh/core/algorithm.hpp"
  #include "viennamesh/core/algorithm_factory.hpp"
%}



// %include "viennagrid/point.hpp"
// %include "viennagrid/storage/static_array.hpp"
// %template(static_array_1d) viennagrid::static_array<double, viennagrid::cartesian_cs<1>::dim>;
// %template(static_array_2d) viennagrid::static_array<double, viennagrid::cartesian_cs<2>::dim>;
// %template(static_array_3d) viennagrid::static_array<double, viennagrid::cartesian_cs<3>::dim>;
// %template(point_1d) viennagrid::spatial_point<double, viennagrid::cartesian_cs<1> >;
// %template(point_2d) viennagrid::spatial_point<double, viennagrid::cartesian_cs<2> >;
// %template(point_3d) viennagrid::spatial_point<double, viennagrid::cartesian_cs<3> >;

// %pointer_functions(double, doublep);


namespace viennamesh
{
  class base_algorithm;

  typedef boost::shared_ptr<base_algorithm> algorithm_handle;
  typedef boost::shared_ptr<const base_algorithm> const_algorithm_handle;


  %nodefaultctor base_parameter_interface;
  class base_parameter_interface
  {
  public:
    %feature("docstring") { Returns the name of the parameter }
    std::string const & name() const;

    %feature("docstring") { Returns the type of the parameter }
    std::string const & type() const;

    %feature("docstring") { Returns the description of the parameter }
    std::string const & description() const;
  };

  %nodefaultctor output_parameter_interface;
  class output_parameter_interface : public base_parameter_interface
  {};

  %nodefaultctor base_input_parameter_interface;
  class base_input_parameter_interface : public base_parameter_interface
  {
  public:
    %feature("docstring") { Returns the default value (if available) of the parameter }
    std::string default_value_string() const;
    %feature("docstring") { Returns restrictions (if available) of the parameter }
    std::string check_string() const;
  };



  %nodefaultctor base_algorithm;
  class base_algorithm
  {
  public:

    %feature("docstring") { Sets a specific input parameter }
    void set_input( std::string const & parameter_name, bool value );
    void set_input( std::string const & parameter_name, int value );
    void set_input( std::string const & parameter_name, double value );
    void set_input( std::string const & parameter_name, std::string const & value );

    %feature("docstring") { Links an input parameter to the output parameter of another algorithm }
    void link_input( std::string const & parameter_name, algorithm_handle const & source_algorithm, std::string const & source_parameter_name );

    %feature("docstring") { Sets another algorithm as default source }
    void set_default_source( algorithm_handle const & source_algorithm );

    %feature("docstring") { Executes the algorithm }
    void run();

    %feature("docstring") { Returns the number of input parameter }
    std::size_t input_parameter_size() const;
    %feature("docstring") { Returns the input parameter of a specified index }
    base_input_parameter_interface const & input_parameter(std::size_t index) const;

    %feature("docstring") { Returns the number of output parameter }
    std::size_t output_parameter_size() const;
    %feature("docstring") { Returns the output parameter of a specified index }
    output_parameter_interface const & output_parameter(std::size_t index) const;
  };


  class algorithm_factory_t
  {
  public:
    %feature("docstring") { Creates an algorithm based on the algorithm id (see \ref section-list-of-algorithms for algorithm ids). }
    algorithm_handle create_by_id(std::string const & algorithm_id) const;

    %feature("docstring") { Returns the number of registered algorithms }
    std::size_t registered_algorithms_size() const { return algorithms.size(); }

    %feature("docstring") { Returns the the name of a registered algorithm based on its index }
    std::string const & registered_algorithm_name(std::size_t algorithm_index) const;
  };

  %feature("docstring") { Returns the algorithm factory. }
  algorithm_factory_t & algorithm_factory();
}
