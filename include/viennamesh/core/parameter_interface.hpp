#ifndef VIENNAMESH_CORE_PARAMETER_INTERFACE_HPP
#define VIENNAMESH_CORE_PARAMETER_INTERFACE_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/core/stream_operators.hpp"

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


  struct parameter_information
  {
//     parameter_information() {}
    parameter_information(std::string const & name_,
                          std::string const & type_,
                          std::string const & description_) : name(name_), type(type_), description(description_) {}

    std::string name;
    std::string type;
    std::string description;
  };


  class base_parameter_interface
  {
    friend class base_algorithm;

  public:
    base_parameter_interface(base_algorithm & algorithm,
                             parameter_information const & information_in) :
                        algorithm_(algorithm),
                        information_(information_in) {}

    virtual ~base_parameter_interface() {}

    parameter_information const & information() const { return information_; }
    std::string const & name() const { return information().name; }
    std::string const & type() const { return information().type; }
    std::string const & description() const { return information().description; }

    base_algorithm & algorithm() { return algorithm_; }
    base_algorithm const & algorithm() const { return algorithm_; }

  private:

    virtual void reset() {}

    base_algorithm & algorithm_;
    parameter_information information_;
  };


  class output_parameter_interface : public base_parameter_interface
  {
  public:
    output_parameter_interface(base_algorithm & algorithm, parameter_information const & information_) :
      base_parameter_interface(algorithm, information_)
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

  class base_input_parameter_interface : public base_parameter_interface
  {
  public:
    base_input_parameter_interface(base_algorithm & algorithm,
                              parameter_information const & information_,
                              RequirementFlag requirement_flag_in) :
        base_parameter_interface(algorithm, information_),
        requirement_flag_(requirement_flag_in)
    { algorithm.register_input_parameter(*this); }

    virtual ~base_input_parameter_interface() {}

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

    virtual std::string default_value() const { return "no default value"; }
    virtual std::string check_string() const { return "no value checking"; }

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

  class dynamic_required_input_parameter_interface : public base_input_parameter_interface
  {
  public:
    dynamic_required_input_parameter_interface(base_algorithm & algorithm,
                                               parameter_information const & information_) :
        base_input_parameter_interface(algorithm, information_, REQUIRED) {}
  };

  class dynamic_optional_input_parameter_interface : public base_input_parameter_interface
  {
  public:
    dynamic_optional_input_parameter_interface(base_algorithm & algorithm,
                                               parameter_information const & information_) :
        base_input_parameter_interface(algorithm, information_, OPTIONAL) {}
  };




  template<typename ValueT>
  class no_check
  {
  public:
    void operator()(ValueT const &) const {}
//     {
//           std::stringstream ss;
//           ss << "Check of parameter \"" << name() << "\" failed. Value: " << native_handle() << "(" << check_string() << ")";
//           throw interface_check_failed_exception( ss.str() );
//     }

    std::string to_string() const { return "no value checking"; }
  };

  template<typename ValueT>
  class no_default_value
  {
  public:
    no_default_value() {}

    ValueT const & operator()() const { return tmp; }

    std::string to_string() const { return "no default value"; }
  private:
    ValueT tmp;
  };


  template<typename ValueT>
  class default_value
  {
  public:
    default_value() {}
    default_value(ValueT const & def_) : def(def_) {}

    ValueT const & operator()() const { return def; }

    std::string to_string() const { return lexical_cast<std::string>(def); }

  private:
    ValueT def;
  };


  template<typename ValueT, typename CheckT = no_check<ValueT>, typename DefaultT = no_default_value<ValueT> >
  class input_parameter_interface : public base_input_parameter_interface
  {
  public:

    input_parameter_interface(base_algorithm & algorithm,
                              parameter_information const & information_,
                              RequirementFlag requirement_flag) :
      base_input_parameter_interface(algorithm, information_, requirement_flag),
      fetched(false) {}

    input_parameter_interface(base_algorithm & algorithm,
                              parameter_information const & information_,
                              RequirementFlag requirement_flag,
                              CheckT const & check_in) :
      base_input_parameter_interface(algorithm, information_, requirement_flag),
      fetched(false), check_(check_in) {}

    input_parameter_interface(base_algorithm & algorithm,
                              parameter_information const & information_,
                              RequirementFlag requirement_flag,
                              DefaultT const & default_in) :
      base_input_parameter_interface(algorithm, information_, requirement_flag),
      fetched(false), default_(default_in) {}

    input_parameter_interface(base_algorithm & algorithm,
                              parameter_information const & information_,
                              RequirementFlag requirement_flag,
                              CheckT const & check_in,
                              DefaultT const & default_in) :
      base_input_parameter_interface(algorithm, information_, requirement_flag),
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

    std::string default_value() const { return default_.to_string(); }
    std::string check_string() const { return check_.to_string(); }

  private:

    void reset()
    {
      fetched = false;
    }

    void fetch() const
    {
      if (!fetched)
      {
        native_handle = base_input_parameter_interface::get<ValueT>();

        if (native_handle)
          check_(native_handle());

        fetched = true;
      }
    }

    mutable bool fetched;
    mutable typename result_of::const_parameter_handle<ValueT>::type native_handle;

    CheckT check_;
    DefaultT default_;
  };


  template<typename ValueT, typename CheckT = no_check<ValueT> >
  class required_input_parameter_interface : public input_parameter_interface<ValueT, CheckT>
  {
  public:
    required_input_parameter_interface(base_algorithm & algorithm,
                                       parameter_information const & information_) :
        input_parameter_interface<ValueT, CheckT>(algorithm, information_, REQUIRED) {}

    required_input_parameter_interface(base_algorithm & algorithm,
                                       parameter_information const & information_,
                                       CheckT const & check_in) :
        input_parameter_interface<ValueT, CheckT>(algorithm, information_, REQUIRED, check_in) {}
  };

  template<typename ValueT, typename CheckT = no_check<ValueT> >
  class optional_input_parameter_interface : public input_parameter_interface<ValueT, CheckT>
  {
  public:
    optional_input_parameter_interface(base_algorithm & algorithm,
                                       parameter_information const & information_) :
        input_parameter_interface<ValueT, CheckT>(algorithm, information_, OPTIONAL) {}

    optional_input_parameter_interface(base_algorithm & algorithm,
                                       parameter_information const & information_,
                                       CheckT const & check_in) :
        input_parameter_interface<ValueT, CheckT>(algorithm, information_, OPTIONAL, check_in) {}
  };

  template<typename ValueT, typename CheckT = no_check<ValueT> >
  class default_input_parameter_interface : public input_parameter_interface<ValueT, CheckT, default_value<ValueT> >
  {
  public:
    default_input_parameter_interface(base_algorithm & algorithm,
                                      parameter_information const & information_,
                                      ValueT const & default_) :
        input_parameter_interface<ValueT, CheckT, default_value<ValueT> >(algorithm, information_, DEFAULT, default_value<ValueT>(default_)) {}

    default_input_parameter_interface(base_algorithm & algorithm,
                                      parameter_information const & information_,
                                      CheckT const & check_in,
                                      ValueT const & default_) :
        input_parameter_interface<ValueT, CheckT, default_value<ValueT> >(algorithm, information_, DEFAULT, check_in, default_value<ValueT>(default_)) {}
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

    output_parameter_proxy(base_algorithm & algorithm_, std::string const & name_) : algorithm(algorithm_), name(name_)
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


    bool operator==( typename viennamesh::result_of::parameter_handle<const ValueT>::type const & ph ) const
    {
      if (!is_native())
        return false;

      return ph == native_handle;
    }

    bool operator!=( typename viennamesh::result_of::parameter_handle<const ValueT>::type const & ph ) const
    {
      return !(*this == ph);
    }

  private:
    base_algorithm & algorithm;
    std::string name;

    bool is_native_;
    bool used_;

    parameter_handle base_handle;
    ParameterHandleType native_handle;
  };


  template<typename ValueT>
  bool operator==( typename viennamesh::result_of::parameter_handle<const ValueT>::type const & ph, output_parameter_proxy<ValueT> const & opp )
  { return opp == ph; }

  template<typename ValueT>
  bool operator!=( typename viennamesh::result_of::parameter_handle<const ValueT>::type const & ph, output_parameter_proxy<ValueT> const & opp )
  { return !(ph == opp); }


  template<typename ValueT, typename CheckT, typename DefaultT>
  bool operator==( input_parameter_interface<ValueT, CheckT, DefaultT> const & ipi, output_parameter_proxy<ValueT> const & opp )
  { return ipi.get() == opp; }

  template<typename ValueT, typename CheckT, typename DefaultT>
  bool operator==( output_parameter_proxy<ValueT> const & opp, input_parameter_interface<ValueT, CheckT, DefaultT> const & ipi )
  { return ipi == opp; }


  template<typename ValueT, typename CheckT, typename DefaultT>
  bool operator!=( input_parameter_interface<ValueT, CheckT, DefaultT> const & ipi, output_parameter_proxy<ValueT> const & opp )
  { return !(ipi == opp); }

  template<typename ValueT, typename CheckT, typename DefaultT>
  bool operator!=( output_parameter_proxy<ValueT> const & opp, input_parameter_interface<ValueT, CheckT, DefaultT> const & ipi )
  { return !(opp == ipi); }

}

#endif
