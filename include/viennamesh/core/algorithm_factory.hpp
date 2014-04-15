#ifndef VIENNAMESH_CORE_ALGORITHM_FACTORY_HPP
#define VIENNAMESH_CORE_ALGORITHM_FACTORY_HPP

#include "viennamesh/core/algorithm.hpp"


namespace viennamesh
{


  class base_algorithm_information : public enable_shared_from_this<base_algorithm_information>
  {
  public:

    virtual ~base_algorithm_information() {}

    virtual algorithm_handle create() const = 0;
    virtual string const & name() const = 0;
    virtual std::map<string, string> const & features() const = 0;
  };

  template<typename AlgorithmT>
  class algorithm_information : public base_algorithm_information
  {
  public:
    algorithm_information(string const & algorithm_name_) : algorithm_name(algorithm_name_) {}

    algorithm_handle create() const { return algorithm_handle( new AlgorithmT() ); }
    string const & name() const { return algorithm_name; }
    std::map<string, string> const & features() const { return feature_map(); }

    static std::map<string, string> & feature_map()
    {
      static std::map<string, string> feature_map_;
      return feature_map_;
    }

    string algorithm_name;
  };


  template<typename AlgorithmT>
  void set_feature( string const & feature_key, string const & feature_value )
  {
    algorithm_information<AlgorithmT>::feature_map()[feature_key] = feature_value;
  }


  typedef shared_ptr<base_algorithm_information> AlgorithmInformationHandle;


  class algorithm_factory_t
  {
  public:

    algorithm_factory_t();

    template<typename AlgorithmT>
    void register_algorithm();

    std::list<AlgorithmInformationHandle> matching_algorithms( std::string const & expression ) const;

    algorithm_handle create_from_name(std::string const & algorithm_name) const;
    algorithm_handle create_from_expression(std::string const & expression) const;

  private:

    std::map<string, AlgorithmInformationHandle> algorithms;

  };





  algorithm_factory_t & algorithm_factory();

  template<typename AlgorithmT>
  class register_algorithm_handle
  {
  public:

    register_algorithm_handle(string const & algorithm_name)
    {
      algorithm_factory().register_algorithm<AlgorithmT>(algorithm_name);
    }

  private:
  };


}



#endif
