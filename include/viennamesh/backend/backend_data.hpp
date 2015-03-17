#ifndef _VIENNAMESH_BACKEND_DATA_HPP_
#define _VIENNAMESH_BACKEND_DATA_HPP_

#include <cassert>
#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "viennamesh/backend/backend_forwards.hpp"


struct viennamesh_data_wrapper_t
{
public:

  viennamesh_data_wrapper_t() : internal_data_(0), own_internal_data(true), use_count_(1) {}
  viennamesh_data_wrapper_t(viennamesh::data_template data_template_in) : data_template_(data_template_in), internal_data_(0), own_internal_data(true), use_count_(1)
  {
#ifdef VIENNAMESH_BACKEND_RETAIN_RELEASE_LOGGING
    std::cout << "New data at " << this << std::endl;
#endif
  }

  std::string type_name();

  viennamesh_context context();

  void make_data();
  void set_data( viennamesh_data internal_data_in );
  viennamesh_data data() { return internal_data_; }

  viennamesh::data_template data_template() { return data_template_;}

  void retain() { ++use_count_; }
  bool release()
  {
    assert(use_count_ > 0);

    if (--use_count_ <= 0)
    {
      delete_this();
      return false;
    }

    return true;
  }

private:
  viennamesh::data_template data_template_;

  viennamesh_data internal_data_;
  bool own_internal_data;

  void release_internal_data();
  void delete_this();
  int use_count_;
};



namespace viennamesh
{
  class data_template_t
  {
  public:

    data_template_t() {}


    viennamesh_data make_data() const
    {
      viennamesh_data data;
      int result = make_function_(&data);
      if (result != VIENNAMESH_SUCCESS)
        throw viennamesh::error_t(result);
      return data;
    }

    void delete_data(viennamesh_data data) const
    {
      int result = delete_function_( data );
      if (result != VIENNAMESH_SUCCESS)
        throw viennamesh::error_t(result);
    }

    void set_make_delete_function(viennamesh_data_make_function make_function_in,
                                  viennamesh_data_delete_function delete_function_in)
    {
      make_function_ = make_function_in;
      delete_function_ = delete_function_in;
    }



    void add_conversion_function(std::string const & to_data_type,
                                 viennamesh_data_convert_function convert_function)
    {
      convert_functions[to_data_type] = convert_function;
    }

    void convert(viennamesh_data_wrapper from, viennamesh_data_wrapper to) const
    {
      ConvertFunctionMap::const_iterator it = convert_functions.find( to->type_name() );
      if (it == convert_functions.end())
        throw viennamesh::error_t(VIENNAMESH_ERROR_NO_CONVERSION_TO_DATA_TYPE);

      it->second( from->data(), to->data() );
    }


    viennamesh_context context() { return context_; }
    void set_context(viennamesh_context context_in) { context_ = context_in; }

    std::string & name() { return name_; }
    std::string const & name() const { return name_; }

  private:
    viennamesh_context context_;
    std::string name_;

    viennamesh_data_make_function make_function_;
    viennamesh_data_delete_function delete_function_;

    typedef std::map<std::string, viennamesh_data_convert_function> ConvertFunctionMap;
    ConvertFunctionMap convert_functions;
  };

}


#endif
