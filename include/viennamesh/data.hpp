#ifndef _VIENNAMESH_DATA_HPP_
#define _VIENNAMESH_DATA_HPP_

#include <cassert>
#include "viennamesh/forwards.hpp"
#include "viennamesh/basic_data.hpp"

namespace viennamesh
{

  class abstract_data_handle
  {
    friend class context_handle;
    friend class algorithm_handle;

  protected:

    abstract_data_handle() : data(0) {}
    abstract_data_handle(viennamesh_data_wrapper data_, bool retain_ = true) : data(data_)
    {
      if (retain_)
        retain();
    }

  public:

    abstract_data_handle(abstract_data_handle const & handle_) : data(handle_.data) { retain(); }

    ~abstract_data_handle()
    {
      release();
    }

    abstract_data_handle & operator=(abstract_data_handle const & handle_)
    {
      release();
      data = handle_.data;
      retain();
      return *this;
    }

    bool valid() const { return data != 0; }
    operator bool() const { return valid(); }

    viennamesh_data_wrapper internal() const { return const_cast<viennamesh_data_wrapper>(data); }

  protected:

    void retain()
    {
      if (data)
        viennamesh_data_wrapper_retain(data);
    }

    void release()
    {
      if (data)
        viennamesh_data_wrapper_release(data);
    }

    viennamesh_data_wrapper data;
  };






  template<typename DataT>
  class data_handle : public abstract_data_handle
  {
    template<typename FromT, typename ToT>
    friend void convert(data_handle<FromT> const & from, data_handle<ToT> const & to);

    friend class context_handle;
    friend class algorithm_handle;

    data_handle() : internal_data(0) {}
    data_handle(viennamesh_data_wrapper data_, bool retain_ = true) : abstract_data_handle(data_, retain_)
    {
      viennamesh_data_wrapper_internal_get(data, (viennamesh_data*)&internal_data);
    }

  public:

    data_handle(data_handle<DataT> const & handle_) : abstract_data_handle(handle_.data), internal_data(handle_.internal_data) {}

    data_handle & operator=(data_handle<DataT> const & handle_)
    {
      release();
      data = handle_.data;
      internal_data = handle_.internal_data;
      retain();
      return *this;
    }

    void use(DataT & data_reference)
    {
      if (data)
        viennamesh_data_wrapper_internal_set(data, (viennamesh_data)&data_reference);

      internal_data = &data_reference;
    }



    DataT const & operator()() const
    {
      assert(internal_data);
      return *internal_data;
    }

    DataT & operator()()
    {
      assert(internal_data);
      return *internal_data;
    }

  private:

    int make(viennamesh_context context,
             std::string const & binary_format)
    {
      release();
      int result = viennamesh_data_wrapper_make(context,
                                  result_of::data_information<DataT>::type_name().c_str(),
                                  binary_format.c_str(),
                                  &data );

      if (result != VIENNAMESH_SUCCESS)
        return result;

      return viennamesh_data_wrapper_internal_get(data, (viennamesh_data*)&internal_data);
    }

    int make(viennamesh_context context)
    {
      return make( context, result_of::data_information<DataT>::local_binary_format() );
    }

    DataT * internal_data;
  };





  template<>
  class data_handle<viennamesh_string> : public abstract_data_handle
  {
    template<typename FromT, typename ToT>
    friend void convert(data_handle<FromT> const & from, data_handle<ToT> const & to);

    friend class context_handle;
    friend class algorithm_handle;

    data_handle() : internal_data(0) {}
    data_handle(viennamesh_data_wrapper data_, bool retain_ = true) : abstract_data_handle(data_, retain_)
    {
      viennamesh_data_wrapper_internal_get(data, (viennamesh_data*)&internal_data);
    }

  public:

    data_handle(data_handle<viennamesh_string> const & handle_) : abstract_data_handle(handle_.data), internal_data(handle_.internal_data) {}

    data_handle & operator=(data_handle<viennamesh_string> const & handle_)
    {
      release();
      data = handle_.data;
      internal_data = handle_.internal_data;
      retain();
      return *this;
    }

    void use(viennamesh_string & data_reference)
    {
      if (data)
        viennamesh_data_wrapper_internal_set(data, (viennamesh_data)&data_reference);

      internal_data = &data_reference;
    }



    const std::string operator()() const
    {
      assert(internal_data);
      const char * tmp;
      viennamesh_string_get( *internal_data, &tmp );
      return tmp;
    }

    void set(std::string const & str)
    {
      viennamesh_string_set( *internal_data, str.c_str() );
    }

  private:

    int make(viennamesh_context context,
             std::string const & binary_format)
    {
      release();
      int result = viennamesh_data_wrapper_make(context,
                                  result_of::data_information<viennamesh_string>::type_name().c_str(),
                                  binary_format.c_str(),
                                  &data );

      if (result != VIENNAMESH_SUCCESS)
        return result;

      return viennamesh_data_wrapper_internal_get(data, (viennamesh_data*)&internal_data);
    }

    int make(viennamesh_context context)
    {
      return make( context, result_of::data_information<viennamesh_string>::local_binary_format() );
    }

    viennamesh_string * internal_data;
  };




  template<typename FromT, typename ToT>
  void convert(data_handle<FromT> const & from, data_handle<ToT> const & to)
  {
    viennamesh_data_wrapper_convert( from.data, to.data );
  }





}

#endif
