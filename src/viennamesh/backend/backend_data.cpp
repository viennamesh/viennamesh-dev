#include "viennamesh/backend/backend_data.hpp"

std::string viennamesh_data_wrapper_t::type_name()
{
  return binary_format_template()->data_template()->name();
}

std::string viennamesh_data_wrapper_t::binary_format()
{
  return binary_format_template()->binary_format();
}

viennamesh_context viennamesh_data_wrapper_t::context()
{
  return binary_format_template()->data_template()->context();
}

void viennamesh_data_wrapper_t::set_data( viennamesh_data internal_data_in )
{
  release_internal_data();

  internal_data_ = internal_data_in;
  own_internal_data = false;
}

void viennamesh_data_wrapper_t::make_data()
{
  release_internal_data();

  internal_data_ = binary_format_template_->make_data();
  own_internal_data = true;
}

void viennamesh_data_wrapper_t::release_internal_data()
{
  if (own_internal_data && internal_data_)
    binary_format_template()->delete_data( internal_data_ );
}

void viennamesh_data_wrapper_t::delete_this()
{
  std::cout << "Delete data at " << this << std::endl;
  release_internal_data();
  delete this;
}
