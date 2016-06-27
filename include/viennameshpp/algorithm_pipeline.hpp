#ifndef VIENNAMESH_CORE_ALGORITHM_PIPELINE_HPP
#define VIENNAMESH_CORE_ALGORITHM_PIPELINE_HPP

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

#include "viennameshpp/core.hpp"
#include "pugixml.hpp"

#include <list>

namespace viennamesh
{

  struct algorithm_pipeline_element
  {
    algorithm_pipeline_element(std::string const & name_) : name(name_), reference_count(0), info_log_level(-1), error_log_level(-1), warning_log_level(-1), debug_log_level(-1), stack_log_level(-1) {}

    std::string name;
    algorithm_handle algorithm;
    std::vector<algorithm_pipeline_element *> referenced_elements;
    int reference_count;

    void change_log_levels();

    int info_log_level;
    int error_log_level;
    int warning_log_level;
    int debug_log_level;
    int stack_log_level;
  };


  class algorithm_pipeline
  {
  public:

    algorithm_pipeline(viennamesh::context_handle & context_) : context(context_) {}

    bool add_algorithm( pugi::xml_node const & algorithm_node );
    bool from_xml( pugi::xml_node const & xml );

    bool run(bool cleanup_after_algorithm_step = false);

    void clear();

    void set_base_path( std::string const & path );

  private:

    algorithm_pipeline_element * get_element(std::string const & algorithm_name);

    viennamesh::context_handle & context;
    std::list<algorithm_pipeline_element> algorithms;
  };


}

#endif
