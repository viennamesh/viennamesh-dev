#include "viennamesh/backend/backend_context.hpp"

int viennamesh_context_t::registered_data_type_count() const { return data_types.size(); }

std::string const & viennamesh_context_t::registered_data_type_name(int index_) const
{
  if (index_ < 0 || index_ >= registered_data_type_count())
    handle_error(VIENNAMESH_ERROR_INVALID_ARGUMENT);

  std::map<std::string, viennamesh::data_template_t>::const_iterator it = data_types.begin();
  std::advance(it, index_);
  return it->first;
}

viennamesh::data_template_t & viennamesh_context_t::get_data_type(std::string const & data_type_name_)
{
  std::map<std::string, viennamesh::data_template_t>::iterator it = data_types.find(data_type_name_);
  if (it == data_types.end())
    throw viennamesh::error(VIENNAMESH_ERROR_DATA_TYPE_NOT_REGISTERED);

  return it->second;
}

// viennamesh::data_template_t const & viennamesh_context::get_data_type(std::string const & data_type_name_) const
// {
//   std::map<std::string, viennamesh::data_template_t>::const_iterator it = data_types.find(data_type_name_);
//   if (it == data_types.end())
//     throw viennamesh::error(VIENNAMESH_ERROR_DATA_TYPE_NOT_REGISTERED);
//
//   return it->second;
// }

void viennamesh_context_t::register_data_type(std::string const & data_type_name_,
                                            std::string const & data_type_binary_format_,
                                            viennamesh_data_make_function make_function_,
                                            viennamesh_data_delete_function delete_function_)
{
  if (data_type_name_.empty())
    handle_error(VIENNAMESH_ERROR_INVALID_ARGUMENT);

  std::map<std::string, viennamesh::data_template_t>::iterator it = data_types.find(data_type_name_);
  if (it == data_types.end())
  {
    // TODO logging
    it = data_types.insert( std::make_pair(data_type_name_, viennamesh::data_template_t()) ).first;
    it->second.name() = data_type_name_;
    it->second.set_context(this);
  }

  it->second.register_data_type(data_type_binary_format_, make_function_, delete_function_);
}

viennamesh_data_wrapper viennamesh_context_t::make_data(std::string const & data_type_name_,
                                                     std::string const & data_type_binary_format_)
{
  viennamesh_data_wrapper result = new viennamesh_data_wrapper_t(&get_data_type(data_type_name_).get_binary_format_template(data_type_binary_format_));
  try
  {
    result->make_data();
  }
  catch (...)
  {
    delete result;
    throw;
  }

  return result;
}

void viennamesh_context_t::register_conversion_function(std::string const & data_type_from,
                                  std::string const & binary_format_from,
                                  std::string const & data_type_to,
                                  std::string const & binary_format_to,
                                  viennamesh_data_convert_function convert_function)
{
  get_data_type(data_type_from).get_binary_format_template(binary_format_from).add_conversion_function(data_type_to, binary_format_to, convert_function);
}

void viennamesh_context_t::convert(viennamesh_data_wrapper from, viennamesh_data_wrapper to)
{
  std::string from_data_type_name = from->type_name();
  std::string from_data_binary_format = from->binary_format();

  get_data_type(from_data_type_name).get_binary_format_template(from_data_binary_format).convert( from, to );
}

viennamesh_data_wrapper viennamesh_context_t::convert_to(viennamesh_data_wrapper from,
                            std::string const & data_type_name_,
                            std::string const & data_type_binary_format_)
{
  viennamesh_data_wrapper result = make_data(data_type_name_, data_type_binary_format_);
  convert(from, result);
  return result;
}

viennamesh::algorithm_template viennamesh_context_t::get_algorithm_template(std::string const & algorithm_name_)
{
  std::map<std::string, viennamesh::algorithm_template_t>::iterator it = algorithm_templates.find(algorithm_name_);
  if (it == algorithm_templates.end())
    throw viennamesh::error(VIENNAMESH_ERROR_DATA_TYPE_NOT_REGISTERED);

  return &it->second;
}

// viennamesh::algorithm_template const & viennamesh_context::get_algorithm_template(std::string const & algorithm_name_) const
// {
//   std::map<std::string, viennamesh::algorithm_template>::const_iterator it = algorithm_templates.find(algorithm_name_);
//   if (it == algorithm_templates.end())
//     throw viennamesh::error(VIENNAMESH_ERROR_DATA_TYPE_NOT_REGISTERED);
//
//   return it->second;
// }

