#ifndef _VIENNAMESH_ALGORITHM_HPP_
#define _VIENNAMESH_ALGORITHM_HPP_

#include "viennamesh/forwards.hpp"
#include "viennamesh/data.hpp"

namespace viennamesh
{

  class algorithm_handle
  {
    friend class context_handle;

  public:

    algorithm_handle() : algorithm(0) {}
    algorithm_handle(algorithm_handle const & handle_) : algorithm(handle_.algorithm)
    {
      retain();
    }
    algorithm_handle(viennamesh_algorithm_wrapper algorithm_) : algorithm(algorithm_)
    {
      retain();
    }

    ~algorithm_handle()
    {
      release();
    }

    algorithm_handle & operator=(algorithm_handle const & handle_)
    {
      release();
      algorithm = handle_.algorithm;
      retain();
      return *this;
    }


    void set_input(std::string const & name, abstract_data_handle data)
    {
      viennamesh_algorithm_set_input(algorithm, name.c_str(), data.internal());
    }

    void link_input(std::string const & name, algorithm_handle const & source_algorithm, std::string const & source_name)
    {
      viennamesh_algorithm_link_input( internal(), name.c_str(), source_algorithm.internal(), source_name.c_str() );
    }

    abstract_data_handle get_input(std::string const & name)
    {
      viennamesh_data_wrapper data_;
      viennamesh_algorithm_get_input(algorithm, name.c_str(), &data_);

      return abstract_data_handle(data_);
    }

    template<typename DataT>
    data_handle<DataT> get_input(std::string const & name)
    {
      viennamesh_data_wrapper data_;
      viennamesh_algorithm_get_input_with_type(algorithm, name.c_str(),
                                               result_of::data_information<DataT>::type_name().c_str(),
                                               result_of::data_information<DataT>::local_binary_format().c_str(),
                                               &data_);

      return data_handle<DataT>(data_, false);
    }


    void set_output(std::string const & name, abstract_data_handle data)
    {
      viennamesh_algorithm_set_output(algorithm, name.c_str(), data.internal());
    }

    abstract_data_handle get_output(std::string const & name)
    {
      viennamesh_data_wrapper data_;
      viennamesh_algorithm_get_output(algorithm, name.c_str(), &data_);

      return abstract_data_handle(data_);
    }

    template<typename DataT>
    data_handle<DataT> get_output(std::string const & name)
    {
      viennamesh_data_wrapper data_;
      viennamesh_algorithm_get_output_with_type(algorithm, name.c_str(),
                                               result_of::data_information<DataT>::type_name().c_str(),
                                               result_of::data_information<DataT>::local_binary_format().c_str(),
                                               &data_);

      return data_handle<DataT>(data_, true);
    }




    void init()
    {
      viennamesh_algorithm_init(algorithm);
    }

    void run()
    {
      viennamesh_algorithm_run(algorithm);
    }

    context_handle context();

    viennamesh_algorithm_wrapper internal() const { return const_cast<viennamesh_algorithm_wrapper>(algorithm); }

  private:

    void retain()
    {
      if (algorithm)
        viennamesh_algorithm_retain(algorithm);
    }

    void release()
    {
      if (algorithm)
        viennamesh_algorithm_release(algorithm);
    }

    void make(viennamesh_context context, std::string const & algorithm_name)
    {
      release();
      viennamesh_algorithm_make(context, algorithm_name.c_str(), &algorithm);
      init();
    }

    viennamesh_algorithm_wrapper algorithm;
  };

}

#endif
