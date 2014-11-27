#include <string>
#include <deque>
#include <vector>

#include <cassert>
#include <iostream>
#include <dlfcn.h>

#include "viennamesh/core.hpp"




// class dynamic_library
// {
// public:
//
//   dynamic_library() : dl(0) {}
//
//   bool load(std::string const & filename)
//   {
//     dl = dlopen(filename.c_str(), RTLD_NOW);
//     if (!dl)
//       return false;
//
//     return true;
//   }
//
//   template<typename FunctionT>
//   bool get_function(std::string const & function_symbol_name, FunctionT & function)
//   {
//     void * f = dlsym(dl, function_symbol_name.c_str());
//     if (!f)
//       return false;
//
//     function = (FunctionT)f;
//     return true;
//   }
//
// private:
//
//   void * dl;
// };




template<typename T>
int generic_make(viennamesh_data * data)
{
  T * tmp = new T;
  *data = tmp;

//   std::cout << "New data at " << *data << std::endl;

  return VIENNAMESH_SUCCESS;
}

template<typename T>
int generic_delete(viennamesh_data data)
{
//   std::cout << "Delete data at " << data << std::endl;

  delete (T*)data;
  return VIENNAMESH_SUCCESS;
}

template<typename FromT, typename ToT>
int generic_convert(viennamesh_data from_, viennamesh_data to_)
{
  FromT * from = (FromT*)from_;
  ToT * to = (ToT*)to_;

  *to = *from;
  return VIENNAMESH_SUCCESS;
}





class square
{
public:

  bool init(viennamesh::algorithm_handle algorithm)
  {
    std::cout << "INIT" << std::endl;
    return true;
  }

  bool run(viennamesh::algorithm_handle algorithm)
  {
    std::cout << "YAY!" << std::endl;

    viennamesh::data_handle<double> my_double = algorithm.get_input<double>("value");
    std::cout << "From the algorithm: " << my_double() << std::endl;

    viennamesh::data_handle<double> squared = algorithm.context().make_data<double>();
    squared() = my_double()*my_double();

    algorithm.set_output( "value", squared );

    return true;
  }

private:
};








template<typename AlgorithmT>
int generic_make_algorithm(viennamesh_algorithm * algorithm)
{
  AlgorithmT * tmp = new AlgorithmT;
  *algorithm = tmp;

  return VIENNAMESH_SUCCESS;
}

template<typename AlgorithmT>
int generic_delete_algorithm(viennamesh_algorithm algorithm)
{
  delete (AlgorithmT*)algorithm;
  return VIENNAMESH_SUCCESS;
}


template<typename AlgorithmT>
int generic_algorithm_init(viennamesh_algorithm_wrapper algorithm)
{
  viennamesh_algorithm internal_algorithm;
  viennamesh_algorithm_get_internal_algorithm(algorithm, &internal_algorithm);

  if (!((AlgorithmT*)internal_algorithm)->init( viennamesh::algorithm_handle(algorithm) ))
    return VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED;

  return VIENNAMESH_SUCCESS;
}


template<typename AlgorithmT>
int generic_algorithm_run(viennamesh_algorithm_wrapper algorithm)
{
  viennamesh_algorithm internal_algorithm;
  viennamesh_algorithm_get_internal_algorithm(algorithm, &internal_algorithm);

  std::cout << "--------------------------------------------------------------------" << std::endl;
  if (!((AlgorithmT*)internal_algorithm)->run( viennamesh::algorithm_handle(algorithm) ))
    return VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED;
  std::cout << "--------------------------------------------------------------------" << std::endl;

  return VIENNAMESH_SUCCESS;
}




int main()
{
  viennamesh::context_handle context;


  context.register_data_type<int>( generic_make<int>, generic_delete<int> );
  context.register_data_type<double>( generic_make<double>, generic_delete<double> );
  context.register_conversion<int,double>( generic_convert<int,double> );


  viennamesh::data_handle<int> my_int = context.make_data<int>();
  viennamesh::data_handle<double> my_double = context.make_data<double>();

  double x;
  my_double.use(x);

  my_int() = 42;
  viennamesh::convert( my_int, my_double );

  std::cout << "generic_get<double>(my_double) = " << my_double() << std::endl;
  std::cout << "x = " << x << std::endl;





  context.register_algorithm( "square",
                              generic_make_algorithm<square>, generic_delete_algorithm<square>,
                              generic_algorithm_init<square>, generic_algorithm_run<square> );


  viennamesh::algorithm_handle algorithm = context.make_algorithm("square");
  viennamesh::algorithm_handle algorithm1 = context.make_algorithm("square");

  algorithm.set_input( "value", my_int );
  algorithm.run();

//   algorithm1.set_input( "test", algorithm.get_output("squared") );
  algorithm1.link_input( "value", algorithm, "value" );
  algorithm1.run();

  std::cout << algorithm1.get_output<double>("value")() << std::endl;

  return -1;
}
