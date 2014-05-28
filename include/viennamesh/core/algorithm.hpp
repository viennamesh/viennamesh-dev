#ifndef VIENNAMESH_CORE_ALGORITHM_HPP
#define VIENNAMESH_CORE_ALGORITHM_HPP

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

#include "viennamesh/core/parameter.hpp"
#include "viennamesh/core/basic_parameters.hpp"

#include "viennamesh/utils/utils.hpp"
#include "viennamesh/utils/logger.hpp"
#include "viennamesh/core/exceptions.hpp"

namespace viennamesh
{
  class base_algorithm;

#ifdef VIENNAMESH_USES_BOOST
  typedef boost::shared_ptr<base_algorithm> algorithm_handle;
  typedef boost::shared_ptr<const base_algorithm> const_algorithm_handle;
#endif

#ifdef VIENNAMESH_USES_CPP11
  typedef std::shared_ptr<base_algorithm> algorithm_handle;
  typedef std::shared_ptr<const base_algorithm> const_algorithm_handle;
#endif


  class base_parameter_interface;
  class base_input_parameter_interface;
  class output_parameter_interface;
  class parameter_link;

  parameter_handle_t<parameter_link> make_parameter_link(algorithm_handle const & algorithm, std::string const & para_name);


  class base_algorithm : public enable_shared_from_this<base_algorithm>
  {
    template<typename ValueT>
    friend class output_parameter_proxy;

    friend class base_parameter_interface;
    friend class base_input_parameter_interface;
    friend class output_parameter_interface;


  public:
    virtual ~base_algorithm() {}

    algorithm_handle handle() { return shared_from_this(); }
    const_algorithm_handle chandle() const { return shared_from_this(); }
    const_algorithm_handle handle() const { return chandle(); }


    void set_default_source( algorithm_handle const & default_source_ )
    { default_source = default_source_; }

    void unset_default_source()
    { default_source = algorithm_handle(); }


  private:

    void set_input_impl( std::string const & name, const_parameter_handle const & parameter )
    {
      if (!is_input_registered(name))
        warning(1) << "Input parameter \"" << name << "\" is set but was not registered" << std::endl;
      inputs.set(name, parameter);
    }

    void set_input_impl( std::string const & name, parameter_handle const & parameter )
    {
      if (!is_input_registered(name))
        warning(1) << "Input parameter \"" << name << "\" is set but was not registered" << std::endl;
      inputs.set(name, parameter);
    }


    template<typename TypeT>
    void generic_set_input( std::string const & name, TypeT const & value )
    { set_input_impl( name, static_cast<parameter_handle const &>(make_parameter(value)) ); }

    template<typename ValueT>
    void generic_set_input( std::string const & name, parameter_handle_t< parameter_wrapper<ValueT> > const & parameter )
    { set_input_impl(name, static_cast<parameter_handle const &>(parameter)); }

  public:


    // sets an input parameter
    void set_input( std::string const & name, const_parameter_handle const & parameter )
    { set_input_impl(name, parameter); }

    void set_input( std::string const & name, parameter_handle const & parameter )
    { set_input_impl(name, parameter); }

    template<typename TypeT>
    void set_input( std::string const & name, TypeT const & value )
    { generic_set_input(name, value); }

    template<typename ValueT>
    void set_input( std::string const & name, parameter_handle_t< const parameter_wrapper<ValueT> > const & parameter )
    { generic_set_input(name, parameter); }

    template<typename ValueT>
    void set_input( std::string const & name, parameter_handle_t< parameter_wrapper<ValueT> > const & parameter )
    { set_input_impl(name, static_cast<parameter_handle const &>(parameter)); }

    void set_input( std::string const & name, parameter_handle_t<parameter_link> const & parameter )
    { set_input_impl(name, static_cast<parameter_handle const &>(parameter)); }

    void set_input( std::string const & name, char const * value )
    { set_input( name, make_parameter( std::string(value) ) ); }

    void set_input( std::string const & name, char * value )
    { set_input( name, make_parameter( std::string(value) ) ); }

    void set_input( std::string const & name, bool value )
    { generic_set_input(name, value); }

    void set_input( std::string const & name, int value )
    { generic_set_input(name, value); }

    void set_input( std::string const & name, double value )
    { generic_set_input(name, value); }


    void link_input( std::string const & name, algorithm_handle const & source_algorithm, std::string const & source_name )
    { set_input( name, make_parameter_link(source_algorithm, source_name) ); }


    // unsets an input parameter
    void unset_input( std::string const & name )
    {
      if (!is_input_registered(name))
        warning(1) << "Input parameter \"" << name << "\" is unset but was not registered" << std::endl;
      inputs.unset(name);
    }

  private:
    // queries an input parameter
    const_parameter_handle get_input( std::string const & name ) const
    {
      if (!is_input_registered(name))
        warning(1) << "Input parameter \"" << name << "\" is accessed but was not registered" << std::endl;

      const_parameter_handle parameter = inputs.get(name);

      if (parameter)
        return parameter;

      if (default_source)
        return default_source->get_output(name);

      return const_parameter_handle();
    }

  public:

    // references an output parameter to a specific value, doesn't take ownership, remembers pointer to value
    template<typename TypeT>
    void set_output( std::string const & name, TypeT & value )
    { outputs.set( name, make_reference_parameter(value) ); }

    void set_output( std::string const & name, parameter_handle const & handle )
    { outputs.set( name, parameter_handle( new parameter_handle_reference(handle) ) ); }

    void set_output( std::string const & name, parameter_handle & handle )
    { outputs.set( name, parameter_handle( new parameter_handle_reference(handle) ) ); }

  public:

    // queries an output parameter
    const_parameter_handle get_output( std::string const & name ) const
    { return outputs.get(name); }

    parameter_handle get_output( std::string const & name )
    { return outputs.get(name); }

    // queries an output parameter of special type
    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get_output( std::string const & name ) const
    { return outputs.get<ValueT>(name); }

    template<typename ValueT>
    typename result_of::parameter_handle<ValueT>::type get_output( std::string const & name )
    { return outputs.get<ValueT>(name); }

    // unsets an output parameter
    void unset_output( std::string const & name ) { outputs.unset(name); } // TODO needed?

//     // clears all output parameters
//     void clear_outputs() { outputs.clear(); } // TODO needed?

    // runs the algorithm
    bool run();

    // returns the algorithm name
    virtual std::string name() const = 0;
    virtual std::string id() const = 0;
//     virtual std::string type() const = 0;
//     virtual std::string category() const = 0;
//     virtual std::string description() const = 0;

    std::string const & base_path() const { return base_path_; }
    std::string & base_path() { return base_path_; }

  protected:

    virtual bool run_impl() = 0;

  private:

    typedef std::map<std::string, base_input_parameter_interface *> RegisteredInputParameterMapType;
    RegisteredInputParameterMapType input_parameters;

    typedef std::map<std::string, output_parameter_interface *> RegisteredOutputParameterMapType;
    RegisteredOutputParameterMapType output_parameters;

    bool is_input_registered(std::string const & name) const;
    bool is_output_registered(std::string const & name) const;

    void register_input_parameter( base_input_parameter_interface & input_parameter );
    void register_output_parameter( output_parameter_interface & output_parameter );

  public:

    typedef const_transform_iterator<RegisteredInputParameterMapType::const_iterator, dereference_second<RegisteredInputParameterMapType::value_type> > const_input_parameter_iterator;

    const_input_parameter_iterator input_parameter_begin() const { return const_input_parameter_iterator(input_parameters.begin()); }
    const_input_parameter_iterator input_parameter_end() const { return const_input_parameter_iterator(input_parameters.end()); }

    std::size_t input_parameter_size() const { return input_parameters.size(); }
    base_input_parameter_interface const & input_parameter(std::size_t index) const
    {
      const_input_parameter_iterator it = input_parameter_begin();
      std::advance(it, index);
      return *it;
    }

    typedef const_transform_iterator<RegisteredOutputParameterMapType::const_iterator, dereference_second<RegisteredOutputParameterMapType::value_type> > const_output_parameter_iterator;

    const_output_parameter_iterator output_parameter_begin() const { return const_output_parameter_iterator(output_parameters.begin()); }
    const_output_parameter_iterator output_parameter_end() const { return const_output_parameter_iterator(output_parameters.end()); }

    std::size_t output_parameter_size() const { return output_parameters.size(); }
    output_parameter_interface const & output_parameter(std::size_t index) const
    {
      const_output_parameter_iterator it = output_parameter_begin();
      std::advance(it, index);
      return *it;
    }


  private:

    algorithm_handle default_source;

    const_parameter_set inputs;
    parameter_set outputs;

    std::string base_path_;
  };


  class parameter_link : public base_parameter
  {
  public:

    parameter_link(algorithm_handle const & algorithm, std::string const & para_name) : algorithm_(algorithm), parameter_name_(para_name) {}

    parameter_handle unpack() { return algorithm_->get_output(parameter_name_); }
    const_parameter_handle unpack() const { return algorithm_->get_output(parameter_name_); }
    bool is_reference() const { return false; }
    std::string type_name() const { return "parameter_link"; }

  private:

    algorithm_handle algorithm_;
    std::string parameter_name_;
  };
}

#include "viennamesh/core/parameter_interface.hpp"

#endif
