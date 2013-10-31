#ifndef VIENNAMESH_CORE_SETTINGS_HPP
#define VIENNAMESH_CORE_SETTINGS_HPP

#include <map>
#include "viennamesh/forwards.hpp"

namespace viennamesh
{
  template<typename T>
  class ParameterWrapper;

  class BaseParameter : public enable_shared_from_this<BaseParameter>
  {
  public:
    virtual ~BaseParameter() {}

    virtual bool isScalar() const = 0;
    virtual double getScalar() const = 0;

    template<typename T>
    shared_ptr< ParameterWrapper<T> > cast()
    {
      return dynamic_pointer_cast< ParameterWrapper<T> >( shared_from_this() );
    }

    template<typename T>
    shared_ptr< const ParameterWrapper<T> > cast() const
    {
      return dynamic_pointer_cast< const ParameterWrapper<T> >( shared_from_this() );
    }
  };


  typedef double ScalarType;

  namespace result_of
  {
    template<typename T>
    struct is_scalar
    { static const bool value = false; };

    template<>
    struct is_scalar<int>
    { static const bool value = true; };

    template<>
    struct is_scalar<float>
    { static const bool value = true; };

    template<>
    struct is_scalar<double>
    { static const bool value = true; };




    template<typename T, bool is_scalar>
    struct scalar_dispatcher_helper;

    template<typename T>
    struct scalar_dispatcher_helper<T,false>
    {
      static ScalarType get( T const & value ) { return ScalarType(); }
    };

    template<typename T>
    struct scalar_dispatcher_helper<T,true>
    {
      static ScalarType get( T const & value ) { return value; }
    };

    template<typename T>
    struct scalar_dispatcher
    {
      static ScalarType get( T const & value ) { return scalar_dispatcher_helper<T, is_scalar<T>::value>::get(value); }
    };
  }


  typedef function<ScalarType (ScalarType)> ScalarField1DFunction;
  typedef function<ScalarType (ScalarType, ScalarType)> ScalarField2DFunction;
  typedef function<ScalarType (ScalarType, ScalarType, ScalarType)> ScalarField3DFunction;

  template<typename T>
  class ParameterWrapper : public BaseParameter
  {
  public:

    ParameterWrapper() {}
    ParameterWrapper( T const & value_ ) : value(value_) {}

    virtual bool isScalar() const { return result_of::is_scalar<T>::value; }
    virtual double getScalar() const { return result_of::scalar_dispatcher<T>::get(value); }

    void set( T const & value_ ) { value = value_; }
    T const & get() const { return value; }

  private:
    T value;
  };

  typedef shared_ptr<BaseParameter> ParameterHandle;
  typedef shared_ptr<const BaseParameter> ConstParameterHandle;

  typedef shared_ptr< ParameterWrapper<double> > DoubleParameterHandle;
  typedef shared_ptr< const ParameterWrapper<double> > ConstDoubleParameterHandle;

  typedef shared_ptr< ParameterWrapper<int> > IntParameterHandle;
  typedef shared_ptr< const ParameterWrapper<int> > ConstIntParameterHandle;

  typedef shared_ptr< ParameterWrapper<bool> > BoolParameterHandle;
  typedef shared_ptr< const ParameterWrapper<bool> > ConstBoolParameterHandle;


  class ParameterSet
  {
  public:

    template<typename T>
    void set( string const & name, T const & value )
    { settings[name] = shared_ptr<BaseParameter>( new ParameterWrapper<T>(value) ); }

    void unset( string const & name )
    { settings.erase(name); }


    ParameterHandle get( string const & name )
    {
      std::map<string, ParameterHandle>::iterator it = settings.find(name);
      if (it != settings.end())
        return it->second;
      else
        return ParameterHandle();
    }

    ConstParameterHandle get( string const & name ) const
    {
      std::map<string, ParameterHandle>::const_iterator it = settings.find(name);
      if (it != settings.end())
        return it->second;
      else
        return ParameterHandle();
    }


    template<typename T>
    shared_ptr< ParameterWrapper<T> > get( string const & name )
    {
      return dynamic_pointer_cast< ParameterWrapper<T> >( get(name) );
    }

    template<typename T>
    shared_ptr< const ParameterWrapper<T> > get( string const & name ) const
    {
      return dynamic_pointer_cast< const ParameterWrapper<T> >( get(name) );
    }



    ParameterHandle getScalar( string const & name )
    {
      ParameterHandle ptr = get(name);
      if (ptr && ptr->isScalar())
        return ptr;
      else return ParameterHandle();
    }

    ConstParameterHandle getScalar( string const & name ) const
    {
      ConstParameterHandle ptr = get(name);
      if (ptr && ptr->isScalar())
        return ptr;
      else return ConstParameterHandle();
    }


    template<typename T>
    bool copy( string const & name, T & dest ) const
    {
      shared_ptr< const ParameterWrapper<T> > ptr = get<T>(name);
      if (ptr)
      {
        dest = ptr->get();
        return true;
      }
      else
        return false;
    }

    bool copyScalar( string const & name, ScalarType & dest ) const
    {
      ConstParameterHandle ptr = get(name);
      if (ptr && ptr->isScalar())
      {
        dest = ptr->getScalar();
        return true;
      }
      else
        return false;
    }

  private:
    std::map<string, ParameterHandle> settings;
  };



  struct ParameterDefinition
  {
    ParameterDefinition() {}
    ParameterDefinition(string const & type_, string const & default_value_, string const & description_) : type(type_), default_value(default_value_), description(description_) {}

    string type;
    string default_value;
    string description;
  };

}

#endif
