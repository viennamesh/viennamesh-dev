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

#include "viennamesh/core/algorithm_factory.hpp"
#include "viennamesh/algorithm/all.hpp"


namespace viennamesh
{

  algorithm_factory_t::algorithm_factory_t()
  {
    register_algorithm<viennamesh::io::mesh_reader>();
    register_algorithm<viennamesh::io::string_reader>();
    register_algorithm<viennamesh::io::mesh_writer>();

#ifdef VIENNAMESH_WITH_TRIANGLE
    register_algorithm<viennamesh::triangle::make_mesh>();
    register_algorithm<viennamesh::triangle::make_hull>();
#endif

#ifdef VIENNAMESH_WITH_TETGEN
    register_algorithm<viennamesh::tetgen::make_mesh>();
#endif

#ifdef VIENNAMESH_WITH_NETGEN
    register_algorithm<viennamesh::netgen::make_mesh>();
    register_algorithm<viennamesh::netgen::csg_make_mesh>();
  #ifdef VIENNAMESH_NETGEN_WITH_OPENCASCADE
    register_algorithm<viennamesh::netgen::occ_make_mesh>();
  #endif
#endif

#ifdef VIENNAMESH_WITH_VGMODELER
    register_algorithm<viennamesh::vgmodeler::adapt_hull>();
#endif

    register_algorithm<viennamesh::make_line_mesh>();

    register_algorithm<viennamesh::affine_transform>();
    register_algorithm<viennamesh::extract_boundary>();
    register_algorithm<viennamesh::extract_plc>();
    register_algorithm<viennamesh::hyperplane_clip>();
    register_algorithm<viennamesh::laplace_smooth>();
    register_algorithm<viennamesh::line_coarsening>();
    register_algorithm<viennamesh::seed_point_segmenting>();

    register_algorithm<viennamesh::map_segments>();
    register_algorithm<viennamesh::merge_meshes>();

    register_algorithm<viennamesh::make_statistic>();
  }

  template<typename AlgorithmT>
  void algorithm_factory_t::register_algorithm() //string const & algorithm_name)
  {
    AlgorithmT algo;

    std::map<std::string, AlgorithmInformationHandle>::iterator it = algorithms.find(algo.id());
    if (it != algorithms.end())
    {
      if (it->second->type_string() != typeid(AlgorithmT).name())
      {
        error(1) << "Algorithm id " << algo.id() << " is used multiple time with different algorithms" << std::endl;
        exit(-1);
      }
      return;
    }

    algorithms.insert( std::make_pair(algo.id(), AlgorithmInformationHandle( new algorithm_information<AlgorithmT>())) );
  }

  algorithm_handle algorithm_factory_t::create_by_id(std::string const & algorithm_id) const
  {
    std::map<std::string, AlgorithmInformationHandle>::const_iterator it = algorithms.find(algorithm_id);
    if (it == algorithms.end())
      return algorithm_handle();
    return it->second->create();
  }


//   std::list<AlgorithmInformationHandle> algorithm_factory_t::matching_algorithms( std::string const & expression ) const
//   {
//     std::list<AlgorithmInformationHandle> result;
//
//     string feature_key = expression.substr(0, expression.find("=="));
//     string feature_value = expression.substr(expression.find("==")+2);
//
//     for (std::map<string, AlgorithmInformationHandle>::const_iterator ait = algorithms.begin(); ait != algorithms.end(); ++ait)
//     {
//       std::map<string, string> const & features = ait->second->features();
//       std::map<string, string>::const_iterator fit = features.find(feature_key);
//       if ((fit != features.end()) && (fit->second == feature_value))
//         result.push_back(ait->second);
//     }
//
//     return result;
//   }

//   algorithm_handle algorithm_factory_t::create_by_expression(std::string const & expression) const
//   {
//     std::list<AlgorithmInformationHandle> algos = matching_algorithms(expression);
//     if (algos.empty())
//       return algorithm_handle();
//     return algos.front()->create();
//   }


  algorithm_factory_t & algorithm_factory()
  {
    static algorithm_factory_t factory_;
    return factory_;
  }
}
