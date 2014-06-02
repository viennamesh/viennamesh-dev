#ifndef VIENNAMESH_CORE_ALGORITHM_FACTORY_HPP
#define VIENNAMESH_CORE_ALGORITHM_FACTORY_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "viennamesh/core/algorithm.hpp"


namespace viennamesh
{


  class base_algorithm_information : public enable_shared_from_this<base_algorithm_information>
  {
  public:

    virtual ~base_algorithm_information() {}

    virtual algorithm_handle create() const = 0;

    virtual std::string id() const = 0;
    virtual std::string type_string() const = 0;
  };

  template<typename AlgorithmT>
  class algorithm_information : public base_algorithm_information
  {
  public:
    algorithm_information() {}

    algorithm_handle create() const { return algorithm_handle( new AlgorithmT() ); }

    std::string id() const { return algorithm.id(); }
    std::string type_string() const { return typeid(algorithm).name(); }

  private:
    AlgorithmT algorithm;
  };


  template<typename AlgorithmT>
  void set_feature( std::string const & feature_key, std::string const & feature_value )
  {
    algorithm_information<AlgorithmT>::feature_map()[feature_key] = feature_value;
  }


  typedef shared_ptr<base_algorithm_information> AlgorithmInformationHandle;


  class algorithm_factory_t
  {
  public:

    algorithm_factory_t();

    algorithm_handle create_by_id(std::string const & algorithm_id) const;
    std::size_t registered_algorithms_size() const { return algorithms.size(); }
    std::string const & registered_algorithm_name(std::size_t algorithm_index) const;

//     std::list<AlgorithmInformationHandle> matching_algorithms( std::string const & expression ) const;
//     algorithm_handle create_by_expression(std::string const & expression) const;

  private:

    template<typename AlgorithmT>
    void register_algorithm();

    std::map<std::string, AlgorithmInformationHandle> algorithms;
  };

  algorithm_factory_t & algorithm_factory();



//   Does not work due to compile unit issues in libraries
//
//   template<typename AlgorithmT>
//   class register_algorithm_handle
//   {
//   public:
//
//     register_algorithm_handle()
//     {
//       algorithm_factory().register_algorithm<AlgorithmT>();
//     }
//
//   private:
//   };


}



#endif
