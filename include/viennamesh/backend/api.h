#ifndef _VIENNAMESH_BACKEND_BACKEND_H_
#define _VIENNAMESH_BACKEND_BACKEND_H_

#ifdef __cplusplus
  #define DYNAMIC_EXPORT extern "C"
#else
  #define DYNAMIC_EXPORT
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                        ViennaMesh
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

typedef int viennamesh_error;

#define VIENNAMESH_SUCCESS 0
#define VIENNAMESH_ERROR_INVALID_CONTEXT 1
#define VIENNAMESH_ERROR_INVALID_ARGUMENT 2
#define VIENNAMESH_ERROR_DATA_TYPE_NOT_REGISTERED 3
#define VIENNAMESH_ERROR_BINARY_FORMAT_FOR_DATA_TYPE_ALREADY_REGISTERED 4
#define VIENNAMESH_ERROR_BINARY_FORMAT_FOR_DATA_TYPE_NOT_REGISTERED 5
#define VIENNAMESH_ERROR_NO_CONVERSION_TO_DATA_TYPE 6
#define VIENNAMESH_ERROR_NO_CONVERSION_TO_DATA_TYPE_WITH_BINARY_FORMAT 7
#define VIENNAMESH_ERROR_ALGORITHM_ALREADY_REGISTERED 8
#define VIENNAMESH_ERROR_ALGORITHM_NOT_REGISTERED 9
#define VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED 10
#define VIENNAMESH_ERROR_NOT_THE_SAME_CONTEXT 11



typedef struct viennamesh_context_t * viennamesh_context;
typedef const char * (*viennamesh_build_environment_function)();

DYNAMIC_EXPORT int viennamesh_context_make(viennamesh_context * context);
DYNAMIC_EXPORT int viennamesh_context_retain(viennamesh_context context);
DYNAMIC_EXPORT int viennamesh_context_release(viennamesh_context context);

typedef void * viennamesh_plugin;
DYNAMIC_EXPORT int viennamesh_context_load_plugin(viennamesh_context context,
                                                  const char * plugin_filename,
                                                  viennamesh_plugin * plugin);
// DYNAMIC_EXPORT int viennamesh_context_unload_plugin(viennamesh_context context, viennamesh_plugin * plugin);


//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                Parameter & Data Structures
//////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void * viennamesh_data;

typedef int (*viennamesh_data_make_function)(viennamesh_data * data);
typedef int (*viennamesh_data_delete_function)(viennamesh_data data);

typedef struct viennamesh_data_wrapper_t * viennamesh_data_wrapper;


DYNAMIC_EXPORT int viennamesh_registered_data_type_get_count(viennamesh_context context,
                                                             int * count);
DYNAMIC_EXPORT int viennamesh_registered_data_type_get_name(viennamesh_context context,
                                                            int index,
                                                            const char ** data_type_name);

DYNAMIC_EXPORT int viennamesh_data_type_register(viennamesh_context context,
                                                 const char * data_type_name,
                                                 const char * data_type_binary_format,
                                                 viennamesh_data_make_function make_function,
                                                 viennamesh_data_delete_function delete_function);
DYNAMIC_EXPORT int viennamesh_data_type_unregister(viennamesh_context context,
                                                   const char * data_type_name,
                                                   const char * data_type_binary_format);




DYNAMIC_EXPORT int viennamesh_data_wrapper_make(viennamesh_context context,
                                        const char * data_type_name,
                                        const char * data_type_binary_format,
                                        viennamesh_data_wrapper * data);
DYNAMIC_EXPORT int viennamesh_data_wrapper_internal_get(viennamesh_data_wrapper data,
                                                viennamesh_data * internal_data);
DYNAMIC_EXPORT int viennamesh_data_wrapper_internal_set(viennamesh_data_wrapper data,
                                                viennamesh_data internal_data);

DYNAMIC_EXPORT int viennamesh_data_wrapper_retain(viennamesh_data_wrapper data);
DYNAMIC_EXPORT int viennamesh_data_wrapper_release(viennamesh_data_wrapper data);

DYNAMIC_EXPORT int viennamesh_data_wrapper_type_get_name(viennamesh_data_wrapper data,
                                                 const char ** data_type_name);
DYNAMIC_EXPORT int viennamesh_data_wrapper_type_get_binary_format(viennamesh_data_wrapper data,
                                                          const char ** data_type_binary_format);


// Conversion


typedef int (*viennamesh_data_convert_function)(viennamesh_data from, viennamesh_data to);

DYNAMIC_EXPORT int viennamesh_data_conversion_register(viennamesh_context context,
                                                       const char * data_type_from,
                                                       const char * binary_format_from,
                                                       const char * data_type_to,
                                                       const char * binary_format_to,
                                                       viennamesh_data_convert_function convert_function);

DYNAMIC_EXPORT int viennamesh_data_wrapper_convert(viennamesh_data_wrapper data_from,
                                           viennamesh_data_wrapper data_to);

DYNAMIC_EXPORT int viennamesh_data_wrapper_convert_to(viennamesh_data_wrapper data_from,
                                              const char * data_type_to,
                                              const char * binary_format_to,
                                              viennamesh_data_wrapper * data_to);











typedef struct viennamesh_algorithm_wrapper_t * viennamesh_algorithm_wrapper;
typedef void * viennamesh_algorithm;

typedef int (*viennamesh_algorithm_init_function)(viennamesh_algorithm_wrapper algorithm);
typedef int (*viennamesh_algorithm_run_function)(viennamesh_algorithm_wrapper algorithm);

typedef int (*viennamesh_algorithm_make_function)(viennamesh_algorithm * algorithm);
typedef int (*viennamesh_algorithm_delete_function)(viennamesh_algorithm algorithm);



DYNAMIC_EXPORT int viennamesh_algorithm_register(viennamesh_context context,
                                                 const char * algorithm_name,
                                                 viennamesh_algorithm_make_function make_function,
                                                 viennamesh_algorithm_delete_function delete_function,
                                                 viennamesh_algorithm_init_function init_function,
                                                 viennamesh_algorithm_run_function run_function);

DYNAMIC_EXPORT int viennamesh_algorithm_make(viennamesh_context context,
                                             const char * algorithm_name,
                                             viennamesh_algorithm_wrapper * algorithm);
DYNAMIC_EXPORT int viennamesh_algorithm_retain(viennamesh_algorithm_wrapper algorithm);
DYNAMIC_EXPORT int viennamesh_algorithm_release(viennamesh_algorithm_wrapper algorithm);

DYNAMIC_EXPORT int viennamesh_algorithm_get_name(viennamesh_algorithm_wrapper algorithm,
                                                 const char ** algorithm_name);
DYNAMIC_EXPORT int viennamesh_algorithm_get_context(viennamesh_algorithm_wrapper algorithm,
                                                    viennamesh_context * context);

DYNAMIC_EXPORT int viennamesh_algorithm_get_internal_algorithm(viennamesh_algorithm_wrapper algorithm,
                                                               viennamesh_algorithm * internal_algorithm);

DYNAMIC_EXPORT int viennamesh_algorithm_set_default_source(viennamesh_algorithm_wrapper algorithm,
                                                           viennamesh_algorithm_wrapper source_algorithm);
DYNAMIC_EXPORT int viennamesh_algorithm_unset_default_source(viennamesh_algorithm_wrapper algorithm);

DYNAMIC_EXPORT int viennamesh_algorithm_unset_input(viennamesh_algorithm_wrapper algorithm,
                                                    const char * name);
DYNAMIC_EXPORT int viennamesh_algorithm_set_input(viennamesh_algorithm_wrapper algorithm,
                                                  const char * name,
                                                  viennamesh_data_wrapper data);
DYNAMIC_EXPORT int viennamesh_algorithm_link_input(viennamesh_algorithm_wrapper algorithm,
                                                   const char * name,
                                                   viennamesh_algorithm_wrapper source_algorithm,
                                                   const char * source_name);
DYNAMIC_EXPORT int viennamesh_algorithm_get_input(viennamesh_algorithm_wrapper algorithm,
                                                  const char * name,
                                                  viennamesh_data_wrapper * data);
DYNAMIC_EXPORT int viennamesh_algorithm_get_input_with_type(viennamesh_algorithm_wrapper algorithm,
                                                            const char * name,
                                                            const char * data_type,
                                                            const char * binary_format,
                                                            viennamesh_data_wrapper * data);


DYNAMIC_EXPORT int viennamesh_algorithm_set_output(viennamesh_algorithm_wrapper algorithm,
                                                  const char * name,
                                                  viennamesh_data_wrapper data);
DYNAMIC_EXPORT int viennamesh_algorithm_get_output(viennamesh_algorithm_wrapper algorithm,
                                                  const char * name,
                                                  viennamesh_data_wrapper * data);
DYNAMIC_EXPORT int viennamesh_algorithm_get_output_with_type(viennamesh_algorithm_wrapper algorithm,
                                                            const char * name,
                                                            const char * data_type,
                                                            const char * binary_format,
                                                            viennamesh_data_wrapper * data);



DYNAMIC_EXPORT int viennamesh_algorithm_init(viennamesh_algorithm_wrapper algorithm);
DYNAMIC_EXPORT int viennamesh_algorithm_run(viennamesh_algorithm_wrapper algorithm);






DYNAMIC_EXPORT int viennamesh_log_info_line(const char * message, int log_level, const char * category);
DYNAMIC_EXPORT int viennamesh_log_error_line(const char * message, int log_level, const char * category);
DYNAMIC_EXPORT int viennamesh_log_warning_line(const char * message, int log_level, const char * category);
DYNAMIC_EXPORT int viennamesh_log_debug_line(const char * message, int log_level, const char * category);
DYNAMIC_EXPORT int viennamesh_log_stack_line(const char * message, int log_level, const char * category);

DYNAMIC_EXPORT int viennamesh_log_increase_indentation();
DYNAMIC_EXPORT int viennamesh_log_decrease_indentation();





#endif
