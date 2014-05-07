#ifndef VIENNAMESH_CORE_PARAMETER_INTERFACE_HPP
#define VIENNAMESH_CORE_PARAMETER_INTERFACE_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{



//   struct parameter_information
//   {
//     parameter_information() {}
//
//     parameter_information(std::string const & type_,
//                           std::string const & description_,
//                           bool required_) :
//       type(type_), description(description_), default_value(""), required(required_) {}
//
//     parameter_information(std::string const & type_,
//                           std::string const & description_,
//                           std::string const & default_value_,
//                           std::string const & valid_range_,
//                           bool required_) :
//       type(type_), description(description_), default_value(default_value_), valid_range(valid_range_), required(required_) {}
//
//     std::string type;
//     std::string description;
//     std::string default_value;
//     std::string valid_range;
//     bool required;
//   };



  class base_parameter_interface
  {
    friend class base_algorithm;

  public:
    base_parameter_interface(base_algorithm & algorithm, std::string const & parameter_name) :
      algorithm_(algorithm), name_(parameter_name) {}

    virtual ~base_parameter_interface() {}

    std::string const & name() const { return name_; }
    base_algorithm & algorithm() { return algorithm_; }
    base_algorithm const & algorithm() const { return algorithm_; }

  private:

    virtual void reset() {}

    base_algorithm & algorithm_;
    std::string name_;
  };


  class output_parameter_interface : public base_parameter_interface
  {
  public:
    output_parameter_interface(base_algorithm & algorithm, std::string const & parameter_name) :
      base_parameter_interface(algorithm, parameter_name)
      {
        algorithm.register_output_parameter(*this);
      }
  };

  enum RequirementFlag
  {
    REQUIRED,
    OPTIONAL,
    DEFAULT
  };

  class dynamic_input_parameter_interface : public base_parameter_interface
  {
  public:
    dynamic_input_parameter_interface(base_algorithm & algorithm,
                              std::string const & parameter_name,
                              RequirementFlag requirement_flag_in) :
        base_parameter_interface(algorithm, parameter_name),
        requirement_flag_(requirement_flag_in)
    { algorithm.register_input_parameter(*this); }

    RequirementFlag requirement_flag() const
    { return requirement_flag_; }

    const_parameter_handle get() const
    {
      if (requirement_flag() == REQUIRED)
        return get_required_impl();
      else
        return get_impl();
    }

    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get() const
    {
      if (requirement_flag() == REQUIRED)
        return get_required_impl<ValueT>();
      else
        return get_impl<ValueT>();
    }

    bool valid() const
    { return get(); }

  private:
    const_parameter_handle get_impl() const
    { return algorithm().get_input(name()); }

    const_parameter_handle get_required_impl() const
    { return algorithm().get_required_input(name()); }


    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get_impl() const
    { return algorithm().get_input<ValueT>(name()); }

    template<typename ValueT>
    typename result_of::const_parameter_handle<ValueT>::type get_required_impl() const
    { return algorithm().get_required_input<ValueT>(name()); }

    RequirementFlag requirement_flag_;
  };

  class dynamic_required_input_parameter_interface : public dynamic_input_parameter_interface
  {
  public:
    dynamic_required_input_parameter_interface(base_algorithm & algorithm,
                                       std::string const & parameter_name) :
        dynamic_input_parameter_interface(algorithm, parameter_name, REQUIRED) {}
  };

  class dynamic_optional_input_parameter_interface : public dynamic_input_parameter_interface
  {
  public:
    dynamic_optional_input_parameter_interface(base_algorithm & algorithm,
                                       std::string const & parameter_name) :
        dynamic_input_parameter_interface(algorithm, parameter_name, OPTIONAL) {}
  };




  template<typename ValueT>
  class no_check
  {
  public:
    bool operator()(ValueT const &)
    { return true; }
  };

  template<typename ValueT>
  class default_value
  {
  public:
    default_value() {}
    default_value(ValueT const & def_) : def(def_) {}

    ValueT const & operator()() const { return def; }

  private:
    ValueT def;
  };


  template<typename ValueT, typename CheckT = no_check<ValueT>, typename DefaultT = default_value<ValueT> >
  class input_parameter_interface : public dynamic_input_parameter_interface
  {
  public:

    input_parameter_interface(base_algorithm & algorithm,
                              std::string const & parameter_name,
                              RequirementFlag requirement_flag) :
      dynamic_input_parameter_interface(algorithm, parameter_name, requirement_flag),
      fetched(false) {}

    input_parameter_interface(base_algorithm & algorithm,
                              std::string const & parameter_name,
                              RequirementFlag requirement_flag,
                              CheckT const & check_in) :
      dynamic_input_parameter_interface(algorithm, parameter_name, requirement_flag),
      fetched(false), check_(check_in) {}

    input_parameter_interface(base_algorithm & algorithm,
                              std::string const & parameter_name,
                              RequirementFlag requirement_flag,
                              DefaultT const & default_in) :
      dynamic_input_parameter_interface(algorithm, parameter_name, requirement_flag),
      fetched(false), default_(default_in) {}

    input_parameter_interface(base_algorithm & algorithm,
                              std::string const & parameter_name,
                              RequirementFlag requirement_flag,
                              CheckT const & check_in,
                              DefaultT const & default_in) :
      dynamic_input_parameter_interface(algorithm, parameter_name, requirement_flag),
      fetched(false), check_(check_in), default_(default_in) {}



    ValueT const & operator()() const
    {
      if (!valid() && (requirement_flag() == DEFAULT))
        return default_();

      return native_handle();
    }

    typename result_of::const_parameter_handle<ValueT>::type get() const
    {
      fetch();
      return native_handle;
    }

    bool valid() const
    {
      fetch();
      return native_handle;
    }

  private:

    void reset()
    {
      fetched = false;
    }

    void fetch() const
    {
      if (!fetched)
      {
        native_handle = dynamic_input_parameter_interface::get<ValueT>();
        fetched = true;
      }
    }

    mutable bool fetched;
    mutable typename result_of::const_parameter_handle<ValueT>::type native_handle;

    CheckT check_;
    DefaultT default_;
  };


  template<typename ValueT>
  class required_input_parameter_interface : public input_parameter_interface<ValueT>
  {
  public:
    required_input_parameter_interface(base_algorithm & algorithm,
                                       std::string const & parameter_name) :
        input_parameter_interface<ValueT>(algorithm, parameter_name, REQUIRED) {}
  };

  template<typename ValueT>
  class optional_input_parameter_interface : public input_parameter_interface<ValueT>
  {
  public:
    optional_input_parameter_interface(base_algorithm & algorithm,
                                       std::string const & parameter_name) :
        input_parameter_interface<ValueT>(algorithm, parameter_name, OPTIONAL) {}
  };

  template<typename ValueT>
  class default_input_parameter_interface : public input_parameter_interface<ValueT>
  {
  public:
    default_input_parameter_interface(base_algorithm & algorithm,
                                      std::string const & parameter_name,
                                      ValueT const & default_) :
        input_parameter_interface<ValueT>(algorithm, parameter_name, DEFAULT, default_value<ValueT>(default_)) {}
  };



  template<typename ValueT>
  class output_parameter_proxy
  {
    friend class base_output_parameter_interface;

  private:
    typedef typename result_of::parameter_handle<ValueT>::type ParameterHandleType;

    void init()
    {
      is_native_ = false;
      used_ = false;

      base_handle = algorithm.outputs.get(name);
      native_handle = dynamic_handle_cast<ValueT>( base_handle );

      if (native_handle)
      {
        is_native_ = true;
      }
      else
      {
        if (base_handle && native_handle && !is_convertable(native_handle, base_handle))
          throw output_not_convertable_to_referenced_value_exception( "Output parameter '" + name + "' is not convertable to referenced value" );

        is_native_ = false;
        native_handle = make_parameter<ValueT>();
      }
    }

    output_parameter_proxy(base_algorithm & algorithm_, string const & name_) : algorithm(algorithm_), name(name_)
    {
      init();
    }

  public:

    output_parameter_proxy( output_parameter_interface & output_interface ) :
      algorithm(output_interface.algorithm()), name(output_interface.name())
    {
      init();
    }

    ~output_parameter_proxy()
    {
      if (used_ && !is_native_)
      {
        if (base_handle)
        {
          if (!convert(native_handle, base_handle))
            error(1) << "output_parameter_proxy::~output_parameter_proxy() - convert failed -> BUG!!" << std::endl;
        }
        else
        {
          algorithm.outputs.set(name, native_handle);
        }
      }
    }

    bool is_native() const { return is_native_; }

    ValueT & operator()() { used_ = true; return native_handle(); }
    ValueT const & operator()() const { return native_handle(); }


    bool operator==( typename viennamesh::result_of::parameter_handle<const ValueT>::type const & ph )
    {
      if (!is_native())
        return false;

      return ph == native_handle;
    }

    bool operator!=( typename viennamesh::result_of::parameter_handle<const ValueT>::type const & ph )
    {
      return !(*this == ph);
    }

  private:
    base_algorithm & algorithm;
    string name;

    bool is_native_;
    bool used_;

    parameter_handle base_handle;
    ParameterHandleType native_handle;
  };


  template<typename ValueT>
  bool operator==( typename viennamesh::result_of::parameter_handle<const ValueT>::type const & ph, output_parameter_proxy<ValueT> const & oop )
  { return oop == ph; }

  template<typename ValueT>
  bool operator!=( typename viennamesh::result_of::parameter_handle<const ValueT>::type const & ph, output_parameter_proxy<ValueT> const & oop )
  { return !(ph == oop); }
}

#endif
