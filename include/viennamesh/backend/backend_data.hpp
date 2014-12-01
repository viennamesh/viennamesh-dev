#ifndef _VIENNAMESH_BACKEND_DATA_HPP_
#define _VIENNAMESH_BACKEND_DATA_HPP_

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "viennamesh/backend/backend_forwards.hpp"


struct viennamesh_data_wrapper_t
{
public:

  viennamesh_data_wrapper_t() : internal_data_(0), own_internal_data(true), use_count_(1) {}
  viennamesh_data_wrapper_t(viennamesh::binary_format_template binary_format_template_in) : internal_data_(0), binary_format_template_(binary_format_template_in), own_internal_data(true), use_count_(1)
  {
#ifdef VIENNAMESH_BACKEND_RETAIN_RELEASE_LOGGING
    std::cout << "New data at " << this << std::endl;
#endif
  }

  std::string type_name();
  std::string binary_format();

  viennamesh_context context();

  void make_data();
  void set_data( viennamesh_data internal_data_in );
  viennamesh_data data() { return internal_data_; }

  viennamesh::binary_format_template binary_format_template() { return binary_format_template_;}

  void retain() { ++use_count_; }
  bool release()
  {
    if (--use_count_ <= 0)
    {
      delete_this();
      return false;
    }

    return true;
  }

private:
  viennamesh::binary_format_template binary_format_template_;

  viennamesh_data internal_data_;
  bool own_internal_data;

  void release_internal_data();
  void delete_this();
  int use_count_;
};



namespace viennamesh
{

  class binary_format_template_t
  {
  public:

    binary_format_template_t() {}

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

    void set_make_delete_function(viennamesh_data_make_function make_function_in, viennamesh_data_delete_function delete_function_in)
    {
      make_function_ = make_function_in;
      delete_function_ = delete_function_in;
    }

    void add_conversion_function(std::string const & to_data_type,
                                 std::string const & to_binary_format,
                                 viennamesh_data_convert_function convert_function)
    {
      convert_functions[to_data_type][to_binary_format] = convert_function;
    }

    void convert(viennamesh_data_wrapper from, viennamesh_data_wrapper to) const
    {
      DataTypeBinaryFormatConvertFunctionMap::const_iterator it = convert_functions.find( to->type_name() );
      if (it == convert_functions.end())
        throw viennamesh::error_t(VIENNAMESH_ERROR_NO_CONVERSION_TO_DATA_TYPE);

      BinaryFormatConvertFunctionMap::const_iterator jt = it->second.find( to->binary_format() );
      if (jt == it->second.end())
        throw viennamesh::error_t(VIENNAMESH_ERROR_NO_CONVERSION_TO_DATA_TYPE_WITH_BINARY_FORMAT);

      jt->second( from->data(), to->data() );
    }

    viennamesh::data_template data_template() { return data_template_; }
    void set_data_template(viennamesh::data_template data_template_in) { data_template_ = data_template_in; }

    std::string & binary_format() { return binary_format_; }
    std::string const & binary_format() const { return binary_format_; }

  private:
    viennamesh::data_template data_template_;
    std::string binary_format_;

    viennamesh_data_make_function make_function_;
    viennamesh_data_delete_function delete_function_;

    typedef std::map<std::string, viennamesh_data_convert_function> BinaryFormatConvertFunctionMap;
    typedef std::map<std::string, BinaryFormatConvertFunctionMap> DataTypeBinaryFormatConvertFunctionMap;

    DataTypeBinaryFormatConvertFunctionMap convert_functions;
  };

  class data_template_t
  {
  public:

    data_template_t() {}

    binary_format_template_t & get_binary_format_template(std::string const & data_type_binary_format_)
    {
      std::map<std::string, binary_format_template_t>::iterator it = binary_format_templates.find(data_type_binary_format_);
      if (it == binary_format_templates.end())
        throw viennamesh::error_t(VIENNAMESH_ERROR_BINARY_FORMAT_FOR_DATA_TYPE_NOT_REGISTERED);
      return it->second;
    }
    binary_format_template_t const & get_binary_format_template(std::string const & data_type_binary_format_) const
    {
      std::map<std::string, binary_format_template_t>::const_iterator it = binary_format_templates.find(data_type_binary_format_);
      if (it == binary_format_templates.end())
        throw viennamesh::error_t(VIENNAMESH_ERROR_BINARY_FORMAT_FOR_DATA_TYPE_NOT_REGISTERED);
      return it->second;
    }

    bool register_data_type(std::string const & data_type_binary_format_,
                            viennamesh_data_make_function make_function_,
                            viennamesh_data_delete_function delete_function_)
    {
      std::map<std::string, binary_format_template_t>::iterator it = binary_format_templates.find(data_type_binary_format_);
      if (it != binary_format_templates.end())
        throw viennamesh::error_t(VIENNAMESH_ERROR_BINARY_FORMAT_FOR_DATA_TYPE_ALREADY_REGISTERED);

      binary_format_templates[data_type_binary_format_].set_make_delete_function(make_function_, delete_function_);
      binary_format_templates[data_type_binary_format_].set_data_template(this);

      return true;
    }

    viennamesh_data make_data(std::string const & data_type_binary_format_) const
    {
      return get_binary_format_template(data_type_binary_format_).make_data();
    }

    viennamesh_context context() { return context_; }
    void set_context(viennamesh_context context_in) { context_ = context_in; }

    std::string & name() { return name_; }
    std::string const & name() const { return name_; }

  private:
    viennamesh_context context_;
    std::string name_;

    std::map<std::string, binary_format_template_t> binary_format_templates;
  };

}


#endif
