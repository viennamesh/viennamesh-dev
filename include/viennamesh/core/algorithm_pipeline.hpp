#ifndef VIENNAMESH_CORE_ALGORITHM_PIPELINE_HPP
#define VIENNAMESH_CORE_ALGORITHM_PIPELINE_HPP

#include "viennamesh/core/algorithm.hpp"
#include "pugixml/pugixml.hpp"

namespace viennamesh
{

  class algorithm_pipeline
  {
  public:

    typedef std::map<string, algorithm_handle> AlgorithmMapType;


    bool add_algorithm( pugi::xml_node const & algorithm_node );
    bool from_xml( pugi::xml_node const & xml );
    bool run();

    void clear();

    void set_base_path( std::string const & path );

  private:

    AlgorithmMapType algorithms;
    std::vector<AlgorithmMapType::iterator> algorithm_order;
  };


}

#endif
