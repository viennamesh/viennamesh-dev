#ifndef VIENNAMESH_STATISTICS_FORWARDS_HPP
#define VIENNAMESH_STATISTICS_FORWARDS_HPP

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

// #include "viennamesh/forwards.hpp"
// #include "viennamesh/core/exceptions.hpp"

namespace viennamesh
{
  struct lower_is_better_tag;
  struct higher_is_better_tag;

  namespace result_of
  {
    template<typename MetricTagT>
    struct metric_ordering_tag;
  }
}

#endif
