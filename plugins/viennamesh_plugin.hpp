#ifndef _VIENNAMESH_PLUGIN_PLUGIN_HPP_
#define _VIENNAMESH_PLUGIN_PLUGIN_HPP_

#include "viennamesh/core.hpp"
#include "viennagrid/core.hpp"

namespace viennamesh
{
  namespace result_of
  {
    template<typename ValueT>
    struct unpack_data
    {
      typedef ValueT type;
    };

    template<typename ValueT>
    struct unpack_data< viennamesh::data_handle<ValueT> >
    {
      typedef ValueT type;
    };
  }


  class plugin_algorithm
  {
  public:

    typedef data_handle<viennagrid_mesh> mesh_handle;
    typedef data_handle<viennamesh_string> string_handle;
    typedef data_handle<viennamesh_point_container> point_container_handle;
    typedef data_handle<viennamesh_seed_point_container> seed_point_container_handle;


    bool init(viennamesh::algorithm_handle algorithm_in)
    {
      algorithm_wrapper = algorithm_in.internal();
      return true;
    }

    template<typename SomethingT>
    typename result_of::data_handle<SomethingT>::type make_data()
    { return algorithm().context().make_data<typename result_of::unpack_data<SomethingT>::type>(); }

    template<typename SomethingT>
    typename result_of::data_handle<SomethingT>::type make_data(SomethingT const & data)
    {
      typename result_of::data_handle<SomethingT>::type handle = make_data<SomethingT>();
      handle() = data;
      return handle;
    }



    std::size_t input_count(std::string const & name)
    {
      if ( get_input(name) )
        return 1;

      std::size_t count = 0;
      abstract_data_handle tmp = get_input(name + "[" + lexical_cast<std::string>(count++) + "]");
      while (tmp)
        tmp = get_input(name + "[" + lexical_cast<std::string>(count++) + "]");

      return count-1;
    }

    template<typename DataT>
    std::size_t input_count(std::string const & name)
    {
      if ( get_input<typename result_of::unpack_data<DataT>::type>(name) )
        return 1;

      std::size_t count = 0;
      abstract_data_handle tmp = get_input<typename result_of::unpack_data<DataT>::type>(name + "[" + lexical_cast<std::string>(count++) + "]");
      while (tmp)
        tmp = get_input<typename result_of::unpack_data<DataT>::type>(name + "[" + lexical_cast<std::string>(count++) + "]");

      return count-1;
    }


    abstract_data_handle get_input(std::string const & name)
    { return algorithm().get_input(name); }

    abstract_data_handle get_input(std::string const & name, std::size_t pos)
    { return get_input(name + "[" + lexical_cast<std::string>(pos) + "]"); }

    template<typename DataT>
    typename result_of::data_handle<DataT>::type get_input(std::string const & name)
    { return algorithm().get_input<typename result_of::unpack_data<DataT>::type>(name); }

    template<typename DataT>
    typename result_of::data_handle<DataT>::type get_input(std::string const & name, std::size_t pos)
    { return get_input<typename result_of::unpack_data<DataT>::type>(name + "[" + lexical_cast<std::string>(pos) + "]"); }

    template<typename DataT>
    std::vector<DataT> get_input_vector(std::string const & name)
    {
      std::vector<DataT> tmp( input_count<DataT>(name) );
      if (tmp.empty())
        return tmp;

      if (tmp.size() == 1)
      {
        if (get_input<DataT>(name))
        {
          tmp[0] = get_input<DataT>(name)();
          return tmp;
        }
        if (get_input<DataT>(name, 0))
        {
          tmp[0] = get_input<DataT>(name, 0)();
          return tmp;
        }
      }

      for (typename std::vector<DataT>::size_type pos = 0; pos != tmp.size(); ++pos)
        tmp[pos] = get_input<DataT>(name, pos)();

      return tmp;
    }


    template<typename DataT>
    typename result_of::data_handle<DataT>::type get_required_input(std::string const & name)
    {
      typename result_of::data_handle<DataT>::type result = algorithm().get_input<typename result_of::unpack_data<DataT>::type>(name);

      if (!result)
      {
        // TODO throw
      }

      return result;
    }



    void set_output(std::string const & name, abstract_data_handle data)
    { algorithm().set_output(name, data); }
    void set_output(std::string const & name, std::size_t pos, abstract_data_handle data)
    { algorithm().set_output(name + "[" + lexical_cast<std::string>(pos) + "]", data); }

    template<typename DataT>
    void set_output(std::string const & name, data_handle<DataT> data)
    { set_output(name, static_cast<abstract_data_handle>(data) ); }
    template<typename DataT>
    void set_output(std::string const & name, std::size_t pos, data_handle<DataT> data)
    { set_output(name, pos, static_cast<abstract_data_handle>(data) ); }

    template<typename DataT>
    void set_output(std::string const & name, DataT data)
    { set_output( name, make_data<DataT>(data) ); }
    template<typename DataT>
    void set_output(std::string const & name, std::size_t pos, DataT data)
    { set_output( name, pos, make_data<DataT>(data) ); }

    void set_output(std::string const & name, point_t const & point)
    {
      data_handle<viennamesh_point_container> tmp = make_data<viennamesh_point_container>();
      convert(point, tmp());
      set_output(name, tmp);
    }
    void set_output(std::string const & name, std::size_t pos, point_t const & point)
    {
      data_handle<viennamesh_point_container> tmp = make_data<viennamesh_point_container>();
      convert(point, tmp());
      set_output(name, pos, tmp);
    }

    template<typename DataT>
    void set_output_vector(std::string const & name, std::vector<DataT> const & data_vector)
    {
      for (typename std::vector<DataT>::size_type i = 0; i != data_vector.size(); ++i)
        set_output( name, i, make_data<DataT>(data_vector[i]) );
    }

  private:

    algorithm_handle algorithm() { return algorithm_handle(algorithm_wrapper); }
    viennamesh_algorithm_wrapper algorithm_wrapper;
  };


  template<typename AlgorithmT>
  int generic_make_algorithm(viennamesh_algorithm * algorithm)
  {
    AlgorithmT * tmp = new AlgorithmT();
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

    {
      const char * name_;
      viennamesh_algorithm_get_name(algorithm, &name_);
      viennamesh::LoggingStack stack( std::string("Running algorithm \"") + name_ + "\"");

      viennamesh::algorithm_handle algorithm_handle(algorithm);

      if (!((AlgorithmT*)internal_algorithm)->run(algorithm_handle))
        return VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED;
    }

    return VIENNAMESH_SUCCESS;
  }








  template<typename T>
  int generic_make(viennamesh_data * data)
  {
    T * tmp = new T;
    *data = tmp;

    return VIENNAMESH_SUCCESS;
  }

  template<typename T>
  int generic_delete(viennamesh_data data)
  {
    delete (T*)data;
    return VIENNAMESH_SUCCESS;
  }




  template<typename FromT, typename ToT>
  int generic_convert(viennamesh_data from_, viennamesh_data to_)
  {
    *static_cast<ToT*>(to_) = *static_cast<FromT*>(from_);
    return VIENNAMESH_SUCCESS;
  }



  template<typename T>
  void register_data_type(context_handle & ctx)
  {
    ctx.register_data_type<T>( generic_make<T>, generic_delete<T> );
  }

  template<typename FromT, typename ToT>
  void register_convert(context_handle & ctx)
  {
    ctx.register_conversion<FromT,ToT>( generic_convert<FromT,ToT> );
  }



  template<typename AlgorithmT>
  void register_algorithm( context_handle & ctx )
  {
    ctx.register_algorithm( AlgorithmT::name(),
                            generic_make_algorithm<AlgorithmT>,
                            generic_delete_algorithm<AlgorithmT>,
                            generic_algorithm_init<AlgorithmT>,
                            generic_algorithm_run<AlgorithmT> );
  }


  std::string local_binary_format();
  void plugin_init( context_handle & context );
}


DYNAMIC_EXPORT int viennamesh_plugin_init(viennamesh_context ctx_);
DYNAMIC_EXPORT int viennamesh_version();


#endif
