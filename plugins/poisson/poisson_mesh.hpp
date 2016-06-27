#ifndef VIENNAMESH_ALGORITHM_POISSON_MESH_HPP
#define VIENNAMESH_ALGORITHM_POISSON_MESH_HPP

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
#include "viennameshpp/plugin.hpp"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <vector>
#include <utility> // defines std::pair
#include <CGAL/Point_with_normal_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Scale_space_surface_reconstruction_3.h>

#ifndef POISSONLIBRARY
  #define POISSONLIBRARY
#endif


namespace viennamesh
{
  namespace poisson
  {
	  typedef CGAL::Simple_cartesian<double> Kernel2;
    typedef Kernel2::Point_3 Point_3;
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef CGAL::Point_with_normal_3<Kernel> Point_with_normal;
    typedef std::vector<Point_with_normal> PointList;
    typedef Kernel::Point_3 Point;
    typedef Kernel::Vector_3 Vector;
    typedef std::pair<Point, Vector> PointVectorPair;
    typedef std::list<PointVectorPair> PairList;
    typedef CGAL::Surface_mesh_default_triangulation_3 STr;
    typedef CGAL::Surface_mesh_complex_2_in_triangulation_3<STr> C2t3;
    typedef CGAL::Scale_space_surface_reconstruction_3< Kernel >    Reconstruction;
    typedef std::list< Point >                                      Point_collection;
  }

  namespace result_of
  {
    template<>
    struct data_information<poisson::Vector>
    {
      static std::string type_name() { return "poisson::Vector"; }
      static viennamesh_data_make_function make_function() { return viennamesh::generic_make<poisson::Vector>; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::generic_delete<poisson::Vector>; }
    };
  }


}

#endif
