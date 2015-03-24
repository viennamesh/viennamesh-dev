#include "viennamesh/backend/api.h"

#include "viennamesh/backend/backend_data.hpp"
#include "viennamesh/backend/backend_algorithm.hpp"
#include "viennamesh/backend/backend_context.hpp"
#include "viennamesh/backend/backend_logger.hpp"

#include "viennagrid/point.hpp"


struct viennamesh_string_t
{
  std::string string;
};



viennamesh_error viennamesh_string_make(viennamesh_string * string)
{
  *string = new viennamesh_string_t;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_string_delete(viennamesh_string string)
{
  delete string;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_string_set(viennamesh_string string, const char * cstr)
{
  string->string = std::string(cstr);
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_string_get(viennamesh_string string, const char ** cstr)
{
  *cstr = string->string.c_str();
  return VIENNAMESH_SUCCESS;
}




struct viennamesh_point_t
{
  viennagrid::point_t point;
};

viennamesh_error viennamesh_point_make(viennamesh_point * point)
{
  *point = new viennamesh_point_t;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_point_delete(viennamesh_point point)
{
  delete point;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_point_set(viennamesh_point point, viennagrid_numeric * values, int size)
{
  point->point.resize(size);
  std::copy(values, values + size, point->point.begin());
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_point_get(viennamesh_point point, viennagrid_numeric ** values, int * size)
{
  if (point->point.empty())
    *values = NULL;
  else
    *values = &point->point[0];

  *size = point->point.size();
  return VIENNAMESH_SUCCESS;
}




struct viennamesh_seed_point_t
{
  viennagrid::point_t point;
  int region;
};

viennamesh_error viennamesh_seed_point_make(viennamesh_seed_point * seed_point)
{
  *seed_point = new viennamesh_seed_point_t;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_seed_point_delete(viennamesh_seed_point seed_point)
{
  delete seed_point;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_seed_point_set(viennamesh_seed_point seed_point,
                                           viennagrid_numeric * values, int size, int region)
{
  seed_point->point.resize(size);
  std::copy(values, values + size, seed_point->point.begin());
  seed_point->region = region;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_seed_point_get(viennamesh_seed_point seed_point,
                                           viennagrid_numeric ** values, int * size,  int * region)
{
  if (seed_point->point.empty())
    *values = NULL;
  else
    *values = &seed_point->point[0];

  *size = seed_point->point.size();
  *region = seed_point->region;
  return VIENNAMESH_SUCCESS;
}

















viennamesh_error viennamesh_context_make(viennamesh_context * context)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  try
  {
    *context = new viennamesh_context_t;
  }
  catch (...)
  {
    return VIENNAMESH_UNKNOWN_ERROR;
  }
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_context_retain(viennamesh_context context)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  try
  {
    context->retain();
  }
  catch (...)
  {
    return viennamesh::handle_error(context);
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_context_release(viennamesh_context context)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  try
  {
    context->release();
  }
  catch (...)
  {
    return viennamesh::handle_error(context);
  }

  return VIENNAMESH_SUCCESS;
}




viennamesh_error viennamesh_context_load_plugin(viennamesh_context context,
                                   const char * plugin_filename,
                                   viennamesh_plugin * plugin)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  try
  {
    *plugin = context->load_plugin(plugin_filename);
  }
  catch (...)
  {
    return viennamesh::handle_error(context);
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_context_load_plugins_in_directory(viennamesh_context context,
                                                 const char * directory_name)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  try
  {
    context->load_plugins_in_directory(directory_name);
  }
  catch (...)
  {
    return viennamesh::handle_error(context);
  }

  return VIENNAMESH_SUCCESS;
}





viennamesh_error viennamesh_context_get_error(viennamesh_context context,
                                              viennamesh_error * error_code,
                                              const char ** error_function,
                                              const char ** error_file,
                                              int * error_line,
                                              const char ** error_message)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  try
  {
    if (error_code)
      *error_code = context->error_code();

    if (error_function)
      *error_function = context->error_function().c_str();

    if (error_file)
      *error_file = context->error_file().c_str();

    if (error_line)
      *error_line = context->error_line();

    if (error_message)
      *error_message = context->error_message().c_str();
  }
  catch (...)
  {
    return VIENNAMESH_ERROR_ERROR_MANAGEMENT;
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_context_set_error(viennamesh_context context,
                                              viennamesh_error error_code,
                                              const char * error_function,
                                              const char * error_file,
                                              int error_line,
                                              const char * error_message)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  try
  {
    context->set_error(error_code, error_function, error_file, error_line, error_message);
  }
  catch (...)
  {
    return VIENNAMESH_ERROR_ERROR_MANAGEMENT;
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_context_clear_error(viennamesh_context context)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  try
  {
    context->clear_error();
  }
  catch (...)
  {
    return VIENNAMESH_ERROR_ERROR_MANAGEMENT;
  }

  return VIENNAMESH_SUCCESS;
}




viennamesh_error viennamesh_registered_data_type_get_count(viennamesh_context context,
                                              int * count)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  if (!count)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *count = context->registered_data_type_count();
  }
  catch (...)
  {
    return viennamesh::handle_error(context);
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_registered_data_type_get_name(viennamesh_context context,
                                             int index,
                                             const char ** data_type_name)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  if (!data_type_name)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *data_type_name = context->registered_data_type_name(index).c_str();
  }
  catch (...)
  {
    return viennamesh::handle_error(context);
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_data_type_register(viennamesh_context context,
                                  const char * data_type_name,
                                  viennamesh_data_make_function make_function,
                                  viennamesh_data_delete_function delete_function)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  if (!data_type_name)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    context->register_data_type( std::string(data_type_name), make_function, delete_function);
  }
  catch (...)
  {
    return viennamesh::handle_error(context);
  }

  return VIENNAMESH_SUCCESS;
}





viennamesh_error viennamesh_data_wrapper_make(viennamesh_context context,
                         const char * data_type_name,
                         viennamesh_data_wrapper * data)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  if (!data || !data_type_name)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *data = context->make_data( std::string(data_type_name) );
  }
  catch (...)
  {
    return viennamesh::handle_error(context);
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_data_get_context(viennamesh_data_wrapper data,
                                                    viennamesh_context * context)
{
  if (!data || !context)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *context = data->data_template()->context();
  }
  catch (...)
  {
    return VIENNAMESH_UNKNOWN_ERROR;
  }

  return VIENNAMESH_SUCCESS;
}



viennamesh_error viennamesh_data_wrapper_get_size(viennamesh_data_wrapper data,
                                                  int * size)
{
  if (!data || !size)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *size = data->size();
  }
  catch (...)
  {
    return viennamesh::handle_error(data->context());
  }


  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_data_wrapper_resize(viennamesh_data_wrapper data,
                                                int size)
{
  if (!data)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    data->resize(size);
  }
  catch (...)
  {
    return viennamesh::handle_error(data->context());
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_data_wrapper_internal_get(viennamesh_data_wrapper data,
                                                      int position,
                                                      viennamesh_data * internal_data)
{
  if (!data || !internal_data)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *internal_data = data->data(position);
  }
  catch (...)
  {
    return viennamesh::handle_error(data->context());
  }

  return VIENNAMESH_SUCCESS;
}



viennamesh_error viennamesh_data_wrapper_internal_make(viennamesh_data_wrapper data,
                                                       int position)
{
  if (!data)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    data->make_data(position);
  }
  catch (...)
  {
    return viennamesh::handle_error(data->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_data_wrapper_internal_set(viennamesh_data_wrapper data,
                                                      int position,
                                                      viennamesh_data internal_data)
{
  if (!data || !internal_data)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    data->set_data(position, internal_data);
  }
  catch (...)
  {
    return viennamesh::handle_error(data->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_data_wrapper_retain(viennamesh_data_wrapper data)
{
  if (!data)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    data->retain();
  }
  catch (...)
  {
    return viennamesh::handle_error(data->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_data_wrapper_release(viennamesh_data_wrapper data)
{
  if (!data)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    data->release();
  }
  catch (...)
  {
    return viennamesh::handle_error(data->context());
  }

  return VIENNAMESH_SUCCESS;
}







viennamesh_error viennamesh_data_conversion_register(viennamesh_context context,
                                        const char * data_type_from,
                                        const char * data_type_to,
                                        viennamesh_data_convert_function convert_function)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  if (!data_type_from || !data_type_to)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    context->register_conversion_function(data_type_from, data_type_to, convert_function);
  }
  catch (...)
  {
    return viennamesh::handle_error(context);
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_data_wrapper_convert(viennamesh_data_wrapper data_from,
                                    viennamesh_data_wrapper data_to)
{
  if (!data_from || !data_to)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    data_from->context()->convert( data_from, data_to );
  }
  catch (...)
  {
    return viennamesh::handle_error(data_from->context());
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_data_type_get_name(viennamesh_data_wrapper data,
                                  const char ** data_type_name)
{
  if (!data || !data_type_name)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *data_type_name = data->data_template()->name().c_str();
  }
  catch (...)
  {
    return viennamesh::handle_error(data->context());
  }

  return VIENNAMESH_SUCCESS;
}









viennamesh_error viennamesh_algorithm_register(viennamesh_context context,
                                  const char * algorithm_name,
                                  viennamesh_algorithm_make_function make_function,
                                  viennamesh_algorithm_delete_function delete_function,
                                  viennamesh_algorithm_init_function init_function,
                                  viennamesh_algorithm_run_function run_function)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  if (!algorithm_name)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    context->register_algorithm(algorithm_name,
                                make_function, delete_function,
                                init_function, run_function);
  }
  catch (...)
  {
    return viennamesh::handle_error(context);
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_algorithm_make(viennamesh_context context,
                              const char * algorithm_name,
                              viennamesh_algorithm_wrapper * algorithm)
{
  if (!context)
    return VIENNAMESH_ERROR_INVALID_CONTEXT;

  if (!algorithm || !algorithm_name)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *algorithm = context->make_algorithm(algorithm_name);
  }
  catch (...)
  {
    return viennamesh::handle_error(context);
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_algorithm_retain(viennamesh_algorithm_wrapper algorithm)
{
  if (!algorithm)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    algorithm->retain();
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_algorithm_release(viennamesh_algorithm_wrapper algorithm)
{
  if (!algorithm)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    algorithm->release();
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_algorithm_set_base_path(viennamesh_algorithm_wrapper algorithm,
                                       const char * path)
{
  if (!algorithm)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    algorithm->set_base_path(path);
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_algorithm_get_base_path(viennamesh_algorithm_wrapper algorithm,
                                       const char ** path)
{
  if (!algorithm || !path)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *path = algorithm->base_path().c_str();
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_algorithm_get_name(viennamesh_algorithm_wrapper algorithm,
                                  const char ** algorithm_name)
{
  if (!algorithm || !algorithm_name)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *algorithm_name = algorithm->name().c_str();
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_algorithm_get_context(viennamesh_algorithm_wrapper algorithm,
                                                    viennamesh_context * context)
{
  if (!algorithm || !context)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *context = algorithm->context();
  }
  catch (...)
  {
    return VIENNAMESH_UNKNOWN_ERROR;
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_algorithm_get_internal_algorithm(viennamesh_algorithm_wrapper algorithm,
                                                viennamesh_algorithm * internal_algorithm)
{
  if (!algorithm || !internal_algorithm)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *internal_algorithm = algorithm->internal_algorithm();
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}





viennamesh_error viennamesh_algorithm_set_default_source(viennamesh_algorithm_wrapper algorithm,
                                                           viennamesh_algorithm_wrapper source_algorithm)
{
  if (!algorithm || !source_algorithm)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    algorithm->set_default_source(source_algorithm);
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_algorithm_unset_default_source(viennamesh_algorithm_wrapper algorithm)
{
  if (!algorithm)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    algorithm->unset_default_source();
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_algorithm_unset_input(viennamesh_algorithm_wrapper algorithm,
                                     const char * name)
{
  if (!algorithm || !name)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    algorithm->unset_input(name);
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_algorithm_set_input(viennamesh_algorithm_wrapper algorithm,
                                   const char * name,
                                   viennamesh_data_wrapper data)
{
  if (!algorithm || !name || !data)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    algorithm->set_input(name, data);
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_algorithm_link_input(viennamesh_algorithm_wrapper algorithm,
                                    const char * name,
                                    viennamesh_algorithm_wrapper source_algorithm,
                                    const char * source_name)
{
  if (!algorithm || !name || !source_algorithm || !source_name)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    algorithm->link_input(name, source_algorithm, source_name);
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_algorithm_get_input(viennamesh_algorithm_wrapper algorithm,
                                   const char * name,
                                   viennamesh_data_wrapper * data)
{
  if (!algorithm || !name || !data)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *data = algorithm->get_input(name);
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_algorithm_get_input_with_type(viennamesh_algorithm_wrapper algorithm,
                                             const char * name,
                                             const char * data_type,
                                             viennamesh_data_wrapper * data)
{
  if (!algorithm || !name || !data_type || !data)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *data = algorithm->get_input(name, data_type);
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_algorithm_set_output(viennamesh_algorithm_wrapper algorithm,
                                    const char * name,
                                    viennamesh_data_wrapper data)
{
  if (!algorithm || !name || !data)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    algorithm->set_output(name, data);
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_algorithm_get_output(viennamesh_algorithm_wrapper algorithm,
                                    const char * name,
                                    viennamesh_data_wrapper * data)
{
  if (!algorithm || !name || !data)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *data = algorithm->get_output(name);
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_algorithm_get_output_with_type(viennamesh_algorithm_wrapper algorithm,
                                              const char * name,
                                              const char * data_type,
                                              viennamesh_data_wrapper * data)
{
  if (!algorithm || !name || !data_type || !data)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    *data = algorithm->get_output(name, data_type);
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_algorithm_init(viennamesh_algorithm_wrapper algorithm)
{
  if (!algorithm)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    algorithm->init();
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_algorithm_run(viennamesh_algorithm_wrapper algorithm)
{
  if (!algorithm)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  try
  {
    algorithm->run();
  }
  catch (...)
  {
    return viennamesh::handle_error(algorithm->context());
  }

  return VIENNAMESH_SUCCESS;
}







viennamesh_error viennamesh_log_info_line(const char * message, int log_level)
{
  if (message)
    viennamesh::backend::info(log_level) << message;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_error_line(const char * message, int log_level)
{
  if (message)
    viennamesh::backend::error(log_level) << message;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_warning_line(const char * message, int log_level)
{
  if (message)
    viennamesh::backend::warning(log_level) << message;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_debug_line(const char * message, int log_level)
{
  if (message)
    viennamesh::backend::debug(log_level) << message;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_stack_line(const char * message, int log_level)
{
  if (message)
    viennamesh::backend::logger().stack(log_level) << message;
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_increase_indentation()
{
  viennamesh::backend::logger().increase_indentation();
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_decrease_indentation()
{
  viennamesh::backend::logger().decrease_indentation();
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_enable_capturing()
{
  viennamesh::backend::StdCapture::get().start();
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_disable_capturing()
{
  viennamesh::backend::StdCapture::get().finish();
  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_log_get_info_level(int * log_level)
{
  if (log_level)
    *log_level = viennamesh::backend::logger().get_log_level<viennamesh::backend::info_tag>();
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_get_error_level(int * log_level)
{
  if (log_level)
    *log_level = viennamesh::backend::logger().get_log_level<viennamesh::backend::error_tag>();
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_get_warning_level(int * log_level)
{
  if (log_level)
    *log_level = viennamesh::backend::logger().get_log_level<viennamesh::backend::warning_tag>();
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_get_debug_level(int * log_level)
{
  if (log_level)
    *log_level = viennamesh::backend::logger().get_log_level<viennamesh::backend::debug_tag>();
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_get_stack_level(int * log_level)
{
  if (log_level)
    *log_level = viennamesh::backend::logger().get_log_level<viennamesh::backend::stack_tag>();
  return VIENNAMESH_SUCCESS;
}



viennamesh_error viennamesh_log_set_info_level(int log_level)
{
  viennamesh::backend::logger().set_log_level<viennamesh::backend::info_tag>(log_level);
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_set_error_level(int log_level)
{
  viennamesh::backend::logger().set_log_level<viennamesh::backend::error_tag>(log_level);
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_set_warning_level(int log_level)
{
  viennamesh::backend::logger().set_log_level<viennamesh::backend::warning_tag>(log_level);
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_set_debug_level(int log_level)
{
  viennamesh::backend::logger().set_log_level<viennamesh::backend::debug_tag>(log_level);
  return VIENNAMESH_SUCCESS;
}

viennamesh_error viennamesh_log_set_stack_level(int log_level)
{
  viennamesh::backend::logger().set_log_level<viennamesh::backend::stack_tag>(log_level);
  return VIENNAMESH_SUCCESS;
}


viennamesh_error viennamesh_log_add_logging_file(char const * filename, viennamesh_log_callback_handle * handle)
{
  if (!filename)
    return VIENNAMESH_ERROR_INVALID_ARGUMENT;

  viennamesh_log_callback_handle tmp = viennamesh::backend::logger().register_file_callback(filename);
  if (handle)
    *handle = tmp;
  return VIENNAMESH_SUCCESS;
}

