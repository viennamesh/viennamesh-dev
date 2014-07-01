#ifndef VIENNAMESH_CORE_STREAM_OPERATORS_HPP
#define VIENNAMESH_CORE_STREAM_OPERATORS_HPP

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


#include <ostream>
#include "viennamesh/forwards.hpp"

namespace viennamesh
{
  std::ostream & operator<<( std::ostream & os, dynamic_point const & p );

  template<typename CoordType, typename CoordinateSystem>
  std::ostream & operator<<( std::ostream & os, std::vector<viennagrid::spatial_point<CoordType,CoordinateSystem> > const & pc )
  {
    for (typename std::vector<viennagrid::spatial_point<CoordType,CoordinateSystem> >::const_iterator it = pc.begin(); it != pc.end(); ++it)
    {
      if (it != pc.begin())
        os << ",";
      std::cout << *it;
    }

    return os;
  }

  template<typename CoordType, typename CoordinateSystem>
  std::ostream & operator<<( std::ostream & os, std::vector< std::pair<viennagrid::spatial_point<CoordType,CoordinateSystem>, int> > const & spc )
  {
    for (typename std::vector< std::pair<viennagrid::spatial_point<CoordType,CoordinateSystem>, int> >::const_iterator it = spc.begin(); it != spc.end(); ++it)
    {
      if (it != spc.begin())
        os << ";";
      std::cout << (*it).first << "," << (*it).second;
    }

    return os;
  }


  std::ostream & operator<<( std::ostream & os, std::map<string,string> const & ssm );
}

#endif
