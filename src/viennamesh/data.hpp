#ifndef _VIENNAMESH_BACKEND_DATA_HPP_
#define _VIENNAMESH_BACKEND_DATA_HPP_

#include <cassert>
#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "forwards.hpp"
#include "viennamesh/cpp_error.hpp"
#include "logger.hpp"



struct viennamesh_internal_data_t
{
  viennamesh_internal_data_t() : data(0), own_data(true) {}

  viennamesh_data data;
  bool own_data;
};


struct viennamesh_data_wrapper_t
{
public:

  viennamesh_data_wrapper_t(viennamesh::data_template data_template_in) : data_template_(data_template_in), internal_data(1), use_count_(1)
  {
#ifdef VIENNAMESH_BACKEND_RETAIN_RELEASE_LOGGING
    std::cout << "New data at " << this << std::endl;
#endif
    make_data(0);
  }

  std::string type_name();

  viennamesh_context context();

  void make_data(int position);
  void set_data(int position, viennamesh_data internal_data_in);
  viennamesh_data data(int position);

  int size() const { return internal_data.size(); }
  void resize(int size_);

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

  std::vector<viennamesh_internal_data_t> internal_data;

  void release_internal_data(int position);
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
      viennamesh_error result = make_function_(&data);
      if (result != VIENNAMESH_SUCCESS)
        VIENNAMESH_ERROR(result, "Data creation failed");
      return data;
    }

    void delete_data(viennamesh_data data) const
    {
      viennamesh_error result = delete_function_( data );
      if (result != VIENNAMESH_SUCCESS)
        VIENNAMESH_ERROR(result, "Data deletion failed");
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
      {
//         viennamesh::backend::error(1) << "No conversion found from data type \"" << from->type_name() << "\" to \"" << to->type_name() << "\"" << std::endl;
        VIENNAMESH_ERROR(VIENNAMESH_ERROR_NO_CONVERSION_TO_DATA_TYPE, "No conversion found from data type \"" + from->type_name() + "\" to \"" + to->type_name() + "\"");
      }

      to->resize( from->size() );
      for (int i = 0; i != from->size(); ++i)
      {
        to->make_data(i);
        it->second( from->data(i), to->data(i) );
      }
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
