#include "viennamesh/backend/api.h"

#include "viennamesh/backend/backend_data.hpp"
#include "viennamesh/backend/backend_algorithm.hpp"
#include "viennamesh/backend/backend_context.hpp"
#include "viennamesh/backend/backend_logger.hpp"


struct viennamesh_string_t
{
  std::string string;
};



int viennamesh_string_make(viennamesh_string * string)
{
  *string = new viennamesh_string_t;
  return VIENNAMESH_SUCCESS;
}

int viennamesh_string_free(viennamesh_string string)
{
  delete string;
  return VIENNAMESH_SUCCESS;
}

int viennamesh_string_set(viennamesh_string string, const char * cstr)
{
  string->string = std::string(cstr);
  return VIENNAMESH_SUCCESS;
}

int viennamesh_string_get(viennamesh_string string, const char ** cstr)
{
  *cstr = string->string.c_str();
  return VIENNAMESH_SUCCESS;
}





int viennamesh_context_make(viennamesh_context * context)
{
  try
  {
    *context = new viennamesh_context_t;
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}

int viennamesh_context_retain(viennamesh_context context)
{
  try
  {
    if (!context)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    context->retain();
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}

int viennamesh_context_release(viennamesh_context context)
{
  try
  {
    if (!context)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    context->release();
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}




int viennamesh_context_load_plugin(viennamesh_context context,
                                   const char * plugin_filename,
                                   viennamesh_plugin * plugin)
{
  *plugin = context->load_plugin(plugin_filename);
  return VIENNAMESH_SUCCESS;
}

int viennamesh_context_load_plugins_in_directory(viennamesh_context context,
                                                 const char * directory_name)
{
  context->load_plugins_in_directory(directory_name);
  return VIENNAMESH_SUCCESS;
}


int viennamesh_registered_data_type_get_count(viennamesh_context context,
                                              int * count)
{
  try
  {
    if (!context)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_CONTEXT);

    if (!count)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    *count = context->registered_data_type_count();
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}


int viennamesh_registered_data_type_get_name(viennamesh_context context,
                                             int index,
                                             const char ** data_type_name)
{
  try
  {
    if (!context)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_CONTEXT);

    if (!data_type_name)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    *data_type_name = context->registered_data_type_name(index).c_str();
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}


int viennamesh_data_type_register(viennamesh_context context,
                                  const char * data_type_name,
                                  const char * data_type_binary_format,
                                  viennamesh_data_make_function make_function,
                                  viennamesh_data_delete_function delete_function)
{
  try
  {
    if (!context)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_CONTEXT);

    if (!data_type_name)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    context->register_data_type( std::string(data_type_name), data_type_binary_format ? data_type_binary_format : "", make_function, delete_function);
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}





int viennamesh_data_wrapper_make(viennamesh_context context,
                         const char * data_type_name,
                         const char * data_type_binary_format,
                         viennamesh_data_wrapper * data)
{
  try
  {
    if (!context)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_CONTEXT);

    if (!data || !data_type_name)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    *data = context->make_data( std::string(data_type_name), data_type_binary_format ? data_type_binary_format : "" );
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}

int viennamesh_data_wrapper_internal_get(viennamesh_data_wrapper data,
                                 viennamesh_data * internal_data)
{
  try
  {
    if (!data || !internal_data)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    *internal_data = data->data();
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}


int viennamesh_data_wrapper_internal_set(viennamesh_data_wrapper data,
                                 viennamesh_data internal_data)
{
  try
  {
    if (!data || !internal_data)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    data->set_data(internal_data);
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}

int viennamesh_data_wrapper_retain(viennamesh_data_wrapper data)
{
  try
  {
    if (!data)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    data->retain();
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}

int viennamesh_data_wrapper_release(viennamesh_data_wrapper data)
{
  try
  {
    if (!data)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    data->release();
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}







int viennamesh_data_conversion_register(viennamesh_context context,
                                        const char * data_type_from,
                                        const char * binary_format_from,
                                        const char * data_type_to,
                                        const char * binary_format_to,
                                        viennamesh_data_convert_function convert_function)
{
  try
  {
    if (!context)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_CONTEXT);

    if (!data_type_from || !data_type_to)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    context->register_conversion_function(data_type_from, binary_format_from ? binary_format_from : "", data_type_to, binary_format_to ? binary_format_to : "", convert_function );
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}


int viennamesh_data_wrapper_convert(viennamesh_data_wrapper data_from,
                                    viennamesh_data_wrapper data_to)
{
  try
  {
    if (!data_from || !data_to)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    if (data_from->context() != data_to->context())
      throw viennamesh::error_t(VIENNAMESH_ERROR_NOT_THE_SAME_CONTEXT);

    data_from->context()->convert( data_from, data_to );
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}


int viennamesh_data_type_get_name(viennamesh_data_wrapper data,
                                  const char ** data_type_name)
{
  try
  {
    if (!data || !data_type_name)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    *data_type_name = data->binary_format_template()->data_template()->name().c_str();
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}

int viennamesh_data_type_get_binary_format(viennamesh_data_wrapper data,
                                           const char ** data_type_binary_format)
{
  try
  {
    if (!data || !data_type_binary_format)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    *data_type_binary_format = data->binary_format_template()->binary_format().c_str();
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}









int viennamesh_algorithm_register(viennamesh_context context,
                                  const char * algorithm_name,
                                  viennamesh_algorithm_make_function make_function,
                                  viennamesh_algorithm_delete_function delete_function,
                                  viennamesh_algorithm_init_function init_function,
                                  viennamesh_algorithm_run_function run_function)
{
  context->register_algorithm(algorithm_name,
                              make_function, delete_function,
                              init_function, run_function);
  return VIENNAMESH_SUCCESS;
}


int viennamesh_algorithm_make(viennamesh_context context,
                              const char * algorithm_name,
                              viennamesh_algorithm_wrapper * algorithm)
{
  try
  {
    if (!context)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_CONTEXT);

    if (!algorithm || !algorithm_name)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    *algorithm = context->make_algorithm(algorithm_name);
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}

int viennamesh_algorithm_retain(viennamesh_algorithm_wrapper algorithm)
{
  try
  {
    if (!algorithm)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    algorithm->retain();
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}

int viennamesh_algorithm_release(viennamesh_algorithm_wrapper algorithm)
{
  try
  {
    if (!algorithm)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    algorithm->release();
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}

int viennamesh_algorithm_get_name(viennamesh_algorithm_wrapper algorithm,
                                  const char ** algorithm_name)
{
  *algorithm_name = algorithm->name().c_str();
  return VIENNAMESH_SUCCESS;
}

int viennamesh_algorithm_get_context(viennamesh_algorithm_wrapper algorithm,
                                                    viennamesh_context * context)
{
  *context = algorithm->context();
  return VIENNAMESH_SUCCESS;
}


int viennamesh_algorithm_get_internal_algorithm(viennamesh_algorithm_wrapper algorithm,
                                                viennamesh_algorithm * internal_algorithm)
{
  *internal_algorithm = algorithm->internal_algorithm();
  return VIENNAMESH_SUCCESS;
}





int viennamesh_algorithm_set_default_source(viennamesh_algorithm_wrapper algorithm,
                                                           viennamesh_algorithm_wrapper source_algorithm)
{
  algorithm->set_default_source(source_algorithm);
  return VIENNAMESH_SUCCESS;
}

int viennamesh_algorithm_unset_default_source(viennamesh_algorithm_wrapper algorithm)
{
  algorithm->unset_default_source();
  return VIENNAMESH_SUCCESS;
}


int viennamesh_algorithm_unset_input(viennamesh_algorithm_wrapper algorithm,
                                     const char * name)
{
  try
  {
    if (!algorithm || !name)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    algorithm->unset_input(name);
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}


int viennamesh_algorithm_set_input(viennamesh_algorithm_wrapper algorithm,
                                   const char * name,
                                   viennamesh_data_wrapper data)
{
  try
  {
    if (!algorithm || !name || !data)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    algorithm->set_input(name, data);
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}

int viennamesh_algorithm_link_input(viennamesh_algorithm_wrapper algorithm,
                                    const char * name,
                                    viennamesh_algorithm_wrapper source_algorithm,
                                    const char * source_name)
{
  try
  {
    if (!algorithm || !name || !source_algorithm || !source_name)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    algorithm->link_input(name, source_algorithm, source_name);
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}

int viennamesh_algorithm_get_input(viennamesh_algorithm_wrapper algorithm,
                                   const char * name,
                                   viennamesh_data_wrapper * data)
{
  try
  {
    if (!algorithm || !name || !data)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    *data = algorithm->get_input(name);
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}

int viennamesh_algorithm_get_input_with_type(viennamesh_algorithm_wrapper algorithm,
                                             const char * name,
                                             const char * data_type,
                                             const char * binary_format,
                                             viennamesh_data_wrapper * data)
{
  try
  {
    if (!algorithm || !name || !data_type || !data)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    *data = algorithm->get_input(name, data_type, binary_format ? binary_format : "");
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}


int viennamesh_algorithm_set_output(viennamesh_algorithm_wrapper algorithm,
                                                  const char * name,
                                                  viennamesh_data_wrapper data)
{
  algorithm->set_output(name, data);
  return VIENNAMESH_SUCCESS;
}

int viennamesh_algorithm_get_output(viennamesh_algorithm_wrapper algorithm,
                                                  const char * name,
                                                  viennamesh_data_wrapper * data)
{
  *data = algorithm->get_output(name);
  return VIENNAMESH_SUCCESS;
}

DYNAMIC_EXPORT int viennamesh_algorithm_get_output_with_type(viennamesh_algorithm_wrapper algorithm,
                                                            const char * name,
                                                            const char * data_type,
                                                            const char * binary_format,
                                                            viennamesh_data_wrapper * data)
{
  *data = algorithm->get_output(name, data_type, binary_format ? binary_format : "");
  return VIENNAMESH_SUCCESS;
}


int viennamesh_algorithm_init(viennamesh_algorithm_wrapper algorithm)
{
  algorithm->init();
  return VIENNAMESH_SUCCESS;
}

int viennamesh_algorithm_run(viennamesh_algorithm_wrapper algorithm)
{
  try
  {
    if (!algorithm)
      throw viennamesh::error_t(VIENNAMESH_ERROR_INVALID_ARGUMENT);

    algorithm->run();
  }
  catch (viennamesh::error_t err)
  {
    return err;
  }
  return VIENNAMESH_SUCCESS;
}







int viennamesh_log_info_line(const char * message, int log_level, const char * category)
{
  viennamesh::backend::info(log_level, category) << message;
  return VIENNAMESH_SUCCESS;
}

int viennamesh_log_error_line(const char * message, int log_level, const char * category)
{
  viennamesh::backend::error(log_level, category) << message;
  return VIENNAMESH_SUCCESS;
}

int viennamesh_log_warning_line(const char * message, int log_level, const char * category)
{
  viennamesh::backend::warning(log_level, category) << message;
  return VIENNAMESH_SUCCESS;
}

int viennamesh_log_debug_line(const char * message, int log_level, const char * category)
{
  viennamesh::backend::debug(log_level, category) << message;
  return VIENNAMESH_SUCCESS;
}

int viennamesh_log_stack_line(const char * message, int log_level, const char * category)
{
  viennamesh::backend::logger().stack(log_level, category) << message;
  return VIENNAMESH_SUCCESS;
}

int viennamesh_log_increase_indentation()
{
  viennamesh::backend::logger().increase_indentation();
  return VIENNAMESH_SUCCESS;
}

int viennamesh_log_decrease_indentation()
{
  viennamesh::backend::logger().decrease_indentation();
  return VIENNAMESH_SUCCESS;
}



