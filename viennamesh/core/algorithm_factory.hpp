#ifndef VIENNAMESH_CORE_ALGORITHM_FACTORY_HPP
#define VIENNAMESH_CORE_ALGORITHM_FACTORY_HPP

#include "viennamesh/core/dynamic_algorithm.hpp"


namespace viennamesh
{

  struct ParameterInfo
  {
    string name;
    bool required;
    string type;
    string description;
  };


  class BaseAlgorithmInformation : public enable_shared_from_this<BaseAlgorithmInformation>
  {
  public:

    virtual ~BaseAlgorithmInformation() {}

    virtual AlgorithmHandle create() const = 0;
    virtual string name() const = 0;
    virtual std::map<string, string> const & features() const = 0;
  };

  template<typename AlgorithmT>
  class AlgorithmInformation : public BaseAlgorithmInformation
  {
  public:
    AlgorithmHandle create() const { return AlgorithmHandle( new AlgorithmT() ); }
    string name() const { return AlgorithmT::name(); }
    std::map<string, string> const & features() const { return feature_map(); }

    static std::map<string, string> & feature_map()
    {
      static std::map<string, string> feature_map_;
      return feature_map_;
    }
  };


  template<typename AlgorithmT>
  void set_feature( string const & feature_key, string const & feature_value )
  {
    AlgorithmInformation<AlgorithmT>::feature_map()[feature_key] = feature_value;
  }


  typedef shared_ptr<BaseAlgorithmInformation> AlgorithmInformationHandle;


  class AlgorithmFactory
  {
  public:


    template<typename AlgorithmT>
    void register_algorithm()
    {
      string algorithm_name = AlgorithmT::name();

      std::map<string, AlgorithmInformationHandle>::iterator it = algorithms.find(algorithm_name);
      if (it != algorithms.end())
        return;

      algorithms.insert( std::make_pair(algorithm_name, AlgorithmInformationHandle( new AlgorithmInformation<AlgorithmT>())) );
    }



    std::list<AlgorithmInformationHandle> matching_algorithms( string const & expression ) const
    {
      std::list<AlgorithmInformationHandle> result;

      string feature_key = expression.substr(0, expression.find("=="));
      string feature_value = expression.substr(expression.find("==")+2);

      for (std::map<string, AlgorithmInformationHandle>::const_iterator ait = algorithms.begin(); ait != algorithms.end(); ++ait)
      {
        std::map<string, string> const & features = ait->second->features();
        std::map<string, string>::const_iterator fit = features.find(feature_key);
        if ((fit != features.end()) && (fit->second == feature_value))
          result.push_back(ait->second);
      }

      return result;
    }

    AlgorithmHandle create_from_name(string const & algorithm_name) const
    {
      std::map<string, AlgorithmInformationHandle>::const_iterator it = algorithms.find(algorithm_name);
      if (it == algorithms.end())
        return AlgorithmHandle();
      return it->second->create();
    }

    AlgorithmHandle create_from_expression(string const & expression) const
    {
      std::list<AlgorithmInformationHandle> algos = matching_algorithms(expression);
      if (algos.empty())
        return AlgorithmHandle();
      return algos.front()->create();
    }


  private:

    std::map<string, AlgorithmInformationHandle> algorithms;

  };


}



#endif
