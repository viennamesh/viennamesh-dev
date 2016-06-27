#include "data.hpp"


std::string viennamesh_data_wrapper_t::type_name()
{
  return data_template()->name();
}

viennamesh_context viennamesh_data_wrapper_t::context()
{
  return data_template()->context();
}


void viennamesh_data_wrapper_t::make_data(int position)
{
  if (position < 0 || position >= size())
    return;

  release_internal_data(position);

  internal_data[position].data = data_template()->make_data();
  internal_data[position].own_data = true;
}

void viennamesh_data_wrapper_t::set_data(int position, viennamesh_data internal_data_in)
{
  if (position < 0 || position >= size())
    return;

  release_internal_data(position);

  internal_data[position].data = internal_data_in;
  internal_data[position].own_data = false;
}

viennamesh_data viennamesh_data_wrapper_t::data(int position)
{
  if (position < 0 || position >= size())
    return NULL;

  return internal_data[position].data;
}

void viennamesh_data_wrapper_t::resize(int new_size)
{
  if (new_size == size())
    return;

  int old_size = size();

  if (new_size < old_size)
  {
    for (int i = new_size; i < old_size; ++i)
      release_internal_data(i);
  }

  internal_data.resize(new_size);

  if (new_size > old_size)
  {
    for (int i = old_size; i < new_size; ++i)
      make_data(i);
  }
}




void viennamesh_data_wrapper_t::release_internal_data(int position)
{
  if (position < 0 || position >= size())
    return;

  if ( internal_data[position].own_data && internal_data[position].data )
  {
    data_template()->delete_data( internal_data[position].data );
  }
}

void viennamesh_data_wrapper_t::release_internal_data()
{
  release_internal_data(0);
}


void viennamesh_data_wrapper_t::delete_this()
{
#ifdef VIENNAMESH_BACKEND_RETAIN_RELEASE_LOGGING
  std::cout << "Delete data at " << this << std::endl;
#endif

  for (int i = 0; i != size(); ++i)
    release_internal_data(i);

  delete this;
}
