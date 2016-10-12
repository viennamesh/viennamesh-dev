#ifndef VIENNAMESH_ALGORITHM_CGAL_MESH_HPP
#define VIENNAMESH_ALGORITHM_CGAL_MESH_HPP

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

#include <list>
#include "viennameshpp/plugin.hpp"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

#ifndef CGALLIBRARY
  #define CGALLIBRARY
#endif


namespace viennamesh
{
  namespace cgal
  {
    typedef CGAL::Simple_cartesian<double> Kernel;
    typedef CGAL::Polyhedron_3<Kernel> polyhedron_surface_mesh;
    typedef Kernel::Point_3 Point_3;
  }

  viennamesh_error convert(viennagrid::mesh const & input, cgal::polyhedron_surface_mesh & output);
  viennamesh_error convert(cgal::polyhedron_surface_mesh const & input, viennagrid::mesh & output);

  template<>
  viennamesh_error internal_convert<viennagrid_mesh, cgal::polyhedron_surface_mesh>(viennagrid_mesh const & input, cgal::polyhedron_surface_mesh & output);
  template<>
  viennamesh_error internal_convert<cgal::polyhedron_surface_mesh, viennagrid_mesh>(cgal::polyhedron_surface_mesh const & input, viennagrid_mesh & output);

  namespace result_of
  {
    template<>
    struct data_information<cgal::polyhedron_surface_mesh>
    {
      static std::string type_name() { return "cgal::polyhedron_surface_mesh"; }
      static viennamesh_data_make_function make_function() { return viennamesh::generic_make<cgal::polyhedron_surface_mesh>; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::generic_delete<cgal::polyhedron_surface_mesh>; }
    };
  }


}

#endif
