#ifndef _VIENNAMESH_H_
#define _VIENNAMESH_H_

#include "viennagrid/viennagrid.h"


/*****************************************************************************************************
 *****************************************************************************************************
 *
 *                                        ViennaMesh
 *
 *****************************************************************************************************
 *****************************************************************************************************/


#ifdef __cplusplus
  #define DYNAMIC_EXPORT extern "C"
#else
  #define DYNAMIC_EXPORT
#endif


#define VIENNAMESH_VERSION_MAJOR 2
#define VIENNAMESH_VERSION_MINOR 0
#define VIENNAMESH_VERSION_PATCH 0

#define VIENNAMESH_VERSION VIENNAMESH_VERSION_MAJOR*10000 + VIENNAMESH_VERSION_MINOR * 100 + VIENNAMESH_VERSION_PATCH



typedef int viennamesh_error;
#define VIENNAMESH_SUCCESS 0
#define VIENNAMESH_UNKNOWN_ERROR 1
#define VIENNAMESH_VIENNAGRID_ERROR 2
#define VIENNAMESH_ERROR_INVALID_CONTEXT 3
#define VIENNAMESH_ERROR_ERROR_MANAGEMENT 4
#define VIENNAMESH_ERROR_INVALID_ARGUMENT 5

#define VIENNAMESH_ERROR_DIFFERENT_CONTEXT 6
#define VIENNAMESH_ERROR_DATA_TYPE_NOT_REGISTERED 7
#define VIENNAMESH_ERROR_NO_CONVERSION_TO_DATA_TYPE 8
#define VIENNAMESH_ERROR_CONVERSION_FAILED 9

#define VIENNAMESH_ERROR_ALGORITHM_ALREADY_REGISTERED 10
#define VIENNAMESH_ERROR_ALGORITHM_NOT_REGISTERED 11
#define VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED 12

#define VIENNAMESH_ERROR_REQUIRED_INPUT_PARAMETER_NOT_FOUND 13
#define VIENNAMESH_ERROR_REQUIRED_INPUT_PARAMETER_NOT_FOUND_OR_NOT_CONVERTABLE 14
#define VIENNAMESH_ERROR_SIZING_FUNCTION 15

#define VIENNAMESH_ERROR_INPUT_PARAMETER_PUSH_BACK_FAILED 16

DYNAMIC_EXPORT const char * viennamesh_error_string(viennamesh_error error);


/*****************************************************************************************************
 *                                Special Data Types
 *****************************************************************************************************/

typedef struct viennamesh_string_t * viennamesh_string;
DYNAMIC_EXPORT viennamesh_error viennamesh_string_make(viennamesh_string * string);
DYNAMIC_EXPORT viennamesh_error viennamesh_string_delete(viennamesh_string string);
DYNAMIC_EXPORT viennamesh_error viennamesh_string_set(viennamesh_string string, const char * cstr);
DYNAMIC_EXPORT viennamesh_error viennamesh_string_get(viennamesh_string string, const char ** cstr);


typedef struct viennamesh_point_t * viennamesh_point;
DYNAMIC_EXPORT viennamesh_error viennamesh_point_make(viennamesh_point * point);
DYNAMIC_EXPORT viennamesh_error viennamesh_point_delete(viennamesh_point point);
DYNAMIC_EXPORT viennamesh_error viennamesh_point_set(viennamesh_point point,
                                                     viennagrid_numeric * values,
                                                     int size);
DYNAMIC_EXPORT viennamesh_error viennamesh_point_get(viennamesh_point point,
                                                     viennagrid_numeric ** values,
                                                     int * size);


typedef struct viennamesh_seed_point_t * viennamesh_seed_point;
DYNAMIC_EXPORT viennamesh_error viennamesh_seed_point_make(viennamesh_seed_point * seed_point);
DYNAMIC_EXPORT viennamesh_error viennamesh_seed_point_delete(viennamesh_seed_point seed_point);
DYNAMIC_EXPORT viennamesh_error viennamesh_seed_point_set(viennamesh_seed_point seed_point,
                                                          viennagrid_numeric * values, int size, int region);
DYNAMIC_EXPORT viennamesh_error viennamesh_seed_point_get(viennamesh_seed_point seed_point,
                                                          viennagrid_numeric ** values, int * size, int * region);



/*****************************************************************************************************
 *                                Context
 *****************************************************************************************************/

typedef struct viennamesh_context_t * viennamesh_context;
typedef const char * (*viennamesh_build_environment_function)();

DYNAMIC_EXPORT viennamesh_error viennamesh_context_make(viennamesh_context * context);
DYNAMIC_EXPORT viennamesh_error viennamesh_context_retain(viennamesh_context context);
DYNAMIC_EXPORT viennamesh_error viennamesh_context_release(viennamesh_context context);

typedef void * viennamesh_plugin;
DYNAMIC_EXPORT viennamesh_error viennamesh_context_load_plugin(viennamesh_context context,
                                                               const char * plugin_filename,
                                                               viennamesh_plugin * plugin);
DYNAMIC_EXPORT viennamesh_error viennamesh_context_load_plugins_in_directory(viennamesh_context context,
                                                                const char * directory_name);
/* DYNAMIC_EXPORT viennamesh_error viennamesh_context_unload_plugin(viennamesh_context context, viennamesh_plugin * plugin); */

DYNAMIC_EXPORT viennamesh_error viennamesh_context_get_error(viennamesh_context context,
                                                             viennamesh_error * error_code,
                                                             const char ** error_function,
                                                             const char ** error_file,
                                                             int * error_line,
                                                             const char ** error_message);
DYNAMIC_EXPORT viennamesh_error viennamesh_context_set_error(viennamesh_context context,
                                                             viennamesh_error error_code,
                                                             const char * error_function,
                                                             const char * error_file,
                                                             int error_line,
                                                             const char * error_message);
DYNAMIC_EXPORT viennamesh_error viennamesh_context_clear_error(viennamesh_context context);



/*****************************************************************************************************
 *                                Parameter & Data Structures
 *****************************************************************************************************/

typedef void * viennamesh_data;

typedef viennamesh_error (*viennamesh_data_make_function)(viennamesh_data * data);
typedef viennamesh_error (*viennamesh_data_delete_function)(viennamesh_data data);

typedef struct viennamesh_data_wrapper_t * viennamesh_data_wrapper;


DYNAMIC_EXPORT viennamesh_error viennamesh_registered_data_type_get_count(viennamesh_context context,
                                                                          int * count);
DYNAMIC_EXPORT viennamesh_error viennamesh_registered_data_type_get_name(viennamesh_context context,
                                                                         int index,
                                                                         const char ** data_type_name);

DYNAMIC_EXPORT viennamesh_error viennamesh_data_type_register(viennamesh_context context,
                                                              const char * data_type_name,
                                                              viennamesh_data_make_function make_function,
                                                              viennamesh_data_delete_function delete_function);
DYNAMIC_EXPORT viennamesh_error viennamesh_data_type_unregister(viennamesh_context context,
                                                                const char * data_type_name);




DYNAMIC_EXPORT viennamesh_error viennamesh_data_wrapper_make(viennamesh_context context,
                                                             const char * data_type_name,
                                                             viennamesh_data_wrapper * data);

DYNAMIC_EXPORT viennamesh_error viennamesh_data_get_context(viennamesh_data_wrapper data,
                                                    viennamesh_context * context);

DYNAMIC_EXPORT viennamesh_error viennamesh_data_wrapper_get_size(viennamesh_data_wrapper data,
                                                                 int * size);
DYNAMIC_EXPORT viennamesh_error viennamesh_data_wrapper_resize(viennamesh_data_wrapper data,
                                                               int size);

DYNAMIC_EXPORT viennamesh_error viennamesh_data_wrapper_internal_get(viennamesh_data_wrapper data,
                                                                     int position,
                                                                     viennamesh_data * internal_data);

DYNAMIC_EXPORT viennamesh_error viennamesh_data_wrapper_retain(viennamesh_data_wrapper data);
DYNAMIC_EXPORT viennamesh_error viennamesh_data_wrapper_release(viennamesh_data_wrapper data);

DYNAMIC_EXPORT viennamesh_error viennamesh_data_wrapper_get_type_name(viennamesh_data_wrapper data,
                                                                      const char ** data_type_name);


// Conversion


typedef viennamesh_error (*viennamesh_data_convert_function)(viennamesh_data from, viennamesh_data to);

DYNAMIC_EXPORT viennamesh_error viennamesh_data_conversion_register(viennamesh_context context,
                                                                    const char * data_type_from,
                                                                    const char * data_type_to,
                                                                    viennamesh_data_convert_function convert_function);

DYNAMIC_EXPORT viennamesh_error viennamesh_data_wrapper_convert(viennamesh_data_wrapper data_from,
                                                                viennamesh_data_wrapper data_to);

DYNAMIC_EXPORT viennamesh_error viennamesh_data_wrapper_convert_to(viennamesh_data_wrapper data_from,
                                                                   const char * data_type_to,
                                                                   viennamesh_data_wrapper * data_to);



/*****************************************************************************************************
 *                                Algorithm
 *****************************************************************************************************/


typedef struct viennamesh_algorithm_wrapper_t * viennamesh_algorithm_wrapper;
typedef void * viennamesh_algorithm;

typedef viennamesh_error (*viennamesh_algorithm_init_function)(viennamesh_algorithm_wrapper algorithm);
typedef viennamesh_error (*viennamesh_algorithm_run_function)(viennamesh_algorithm_wrapper algorithm);

typedef viennamesh_error (*viennamesh_algorithm_make_function)(viennamesh_algorithm * algorithm);
typedef viennamesh_error (*viennamesh_algorithm_delete_function)(viennamesh_algorithm algorithm);



DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_register(viennamesh_context context,
                                                              const char * algorithm_type,
                                                              viennamesh_algorithm_make_function make_function,
                                                              viennamesh_algorithm_delete_function delete_function,
                                                              viennamesh_algorithm_init_function init_function,
                                                              viennamesh_algorithm_run_function run_function);

DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_make(viennamesh_context context,
                                                          const char * algorithm_type,
                                                          viennamesh_algorithm_wrapper * algorithm);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_retain(viennamesh_algorithm_wrapper algorithm);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_release(viennamesh_algorithm_wrapper algorithm);

DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_set_base_path(viennamesh_algorithm_wrapper algorithm,
                                                                   const char * path);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_get_base_path(viennamesh_algorithm_wrapper algorithm,
                                                                   const char ** path);

DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_get_type(viennamesh_algorithm_wrapper algorithm,
                                                              const char ** algorithm_type);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_get_context(viennamesh_algorithm_wrapper algorithm,
                                                                 viennamesh_context * context);

DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_get_internal_algorithm(viennamesh_algorithm_wrapper algorithm,
                                                                            viennamesh_algorithm * internal_algorithm);

DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_set_default_source(viennamesh_algorithm_wrapper algorithm,
                                                                        viennamesh_algorithm_wrapper source_algorithm);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_unset_default_source(viennamesh_algorithm_wrapper algorithm);

DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_clear_inputs(viennamesh_algorithm_wrapper algorithm);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_unset_input(viennamesh_algorithm_wrapper algorithm,
                                                                 const char * name);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_set_input(viennamesh_algorithm_wrapper algorithm,
                                                               const char * name,
                                                               viennamesh_data_wrapper data);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_link_input(viennamesh_algorithm_wrapper algorithm,
                                                                const char * name,
                                                                viennamesh_algorithm_wrapper source_algorithm,
                                                                const char * source_name);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_get_input(viennamesh_algorithm_wrapper algorithm,
                                                               const char * name,
                                                               viennamesh_data_wrapper * data);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_get_input_with_type(viennamesh_algorithm_wrapper algorithm,
                                                                         const char * name,
                                                                         const char * data_type,
                                                                         viennamesh_data_wrapper * data);

DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_clear_outputs(viennamesh_algorithm_wrapper algorithm);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_set_output(viennamesh_algorithm_wrapper algorithm,
                                                                const char * name,
                                                                viennamesh_data_wrapper data);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_get_output(viennamesh_algorithm_wrapper algorithm,
                                                                const char * name,
                                                                viennamesh_data_wrapper * data);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_get_output_with_type(viennamesh_algorithm_wrapper algorithm,
                                                                          const char * name,
                                                                          const char * data_type,
                                                                          viennamesh_data_wrapper * data);



DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_init(viennamesh_algorithm_wrapper algorithm);
DYNAMIC_EXPORT viennamesh_error viennamesh_algorithm_run(viennamesh_algorithm_wrapper algorithm);


/*****************************************************************************************************
 *                                Logging
 *****************************************************************************************************/

typedef int viennamesh_log_callback_handle;


DYNAMIC_EXPORT viennamesh_error viennamesh_log_info_line(const char * message, int log_level);
DYNAMIC_EXPORT viennamesh_error viennamesh_log_error_line(const char * message, int log_level);
DYNAMIC_EXPORT viennamesh_error viennamesh_log_warning_line(const char * message, int log_level);
DYNAMIC_EXPORT viennamesh_error viennamesh_log_debug_line(const char * message, int log_level);
DYNAMIC_EXPORT viennamesh_error viennamesh_log_stack_line(const char * message, int log_level);

DYNAMIC_EXPORT viennamesh_error viennamesh_log_increase_indentation();
DYNAMIC_EXPORT viennamesh_error viennamesh_log_decrease_indentation();

DYNAMIC_EXPORT viennamesh_error viennamesh_log_enable_capturing();
DYNAMIC_EXPORT viennamesh_error viennamesh_log_disable_capturing();


DYNAMIC_EXPORT viennamesh_error viennamesh_log_get_info_level(int * log_level);
DYNAMIC_EXPORT viennamesh_error viennamesh_log_get_error_level(int * log_level);
DYNAMIC_EXPORT viennamesh_error viennamesh_log_get_warning_level(int * log_level);
DYNAMIC_EXPORT viennamesh_error viennamesh_log_get_debug_level(int * log_level);
DYNAMIC_EXPORT viennamesh_error viennamesh_log_get_stack_level(int * log_level);

DYNAMIC_EXPORT viennamesh_error viennamesh_log_set_info_level(int log_level);
DYNAMIC_EXPORT viennamesh_error viennamesh_log_set_error_level(int log_level);
DYNAMIC_EXPORT viennamesh_error viennamesh_log_set_warning_level(int log_level);
DYNAMIC_EXPORT viennamesh_error viennamesh_log_set_debug_level(int log_level);
DYNAMIC_EXPORT viennamesh_error viennamesh_log_set_stack_level(int log_level);


DYNAMIC_EXPORT viennamesh_error viennamesh_log_add_logging_file(char const * filename, viennamesh_log_callback_handle * handle);



#endif
