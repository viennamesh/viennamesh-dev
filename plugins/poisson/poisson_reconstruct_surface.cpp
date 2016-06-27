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
#include "poisson_mesh.hpp"
#include "poisson_reconstruct_surface.hpp"
#include <CGAL/IO/read_xyz_points.h>
#include <fstream>
#include <CGAL/pca_estimate_normals.h>
#include <CGAL/mst_orient_normals.h>
#include <CGAL/property_map.h>
#include <CGAL/Implicit_surface_3.h>
#include <CGAL/Poisson_reconstruction_function.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <utility> // defines std::pair
#include "viennagrid/viennagrid.hpp"

namespace viennamesh
{
  namespace poisson
  {
    typedef Kernel::FT FT;
    typedef Kernel::Sphere_3 Sphere;
    typedef CGAL::Poisson_reconstruction_function<Kernel> Poisson_reconstruction_function;
    typedef CGAL::Implicit_surface_3<Kernel, Poisson_reconstruction_function> Surface_3;
    struct poisson_reconstruction_options
    {
      double min_triangle_angle;        //minimum angle of the generated triangles (in degrees)
      double max_triangle_size_mult;    //maximum area of the generated triangles  (in times of the average spacing of points)
      double approximation_error_mult;  //maximal acceptable approximation error   (in times of the average spacing of points)
      double max_triangle_size_abs;     //maximum area of the generated triangles  (as absolute value)
      double approximation_error_abs;   //maximal acceptable approximation error   (as absolute value)
    };

    int get_id_of_vertex(Point p, C2t3 & input)
    {
      typedef C2t3::Vertex_iterator                 Vertex_iterator;
      int pos =0;
      {
        Vertex_iterator begin = input.vertices_begin();
        for ( ; begin != input.vertices_end(); ++begin, ++pos)
          if(begin->point()==p)
            return pos;
      }
      return -1;
    }

    void reconstruct_surface_impl(PointList const & input, viennagrid::mesh output,
                        struct poisson_reconstruction_options options)
    {
      // Creates implicit function from the read points using the default solver.
      // Note: this method requires an iterator over points
      // + property maps to access each point's position and normal.
      // The position property map can be omitted here as we use iterators over Point_3 elements.

      Poisson_reconstruction_function function(input.begin(), input.end(),
                                               CGAL::make_normal_of_point_with_normal_pmap(PointList::value_type()) );

      // Computes the Poisson indicator function f()
      // at each vertex of the triangulation.
      if ( !function.compute_implicit_function() )
        return;
      // Computes average spacing
      FT average_spacing = CGAL::compute_average_spacing(input.begin(), input.end(), 6);
      if(options.max_triangle_size_abs == -1.0)
        options.max_triangle_size_abs = average_spacing*options.max_triangle_size_mult;
      if(options.approximation_error_abs == -1.0)
        options.approximation_error_abs = average_spacing*options.approximation_error_mult;
      // Gets one point inside the implicit surface
      // and computes implicit function bounding sphere radius.
      Point inner_point = function.get_inner_point();
      Sphere bsphere = function.bounding_sphere();
      FT radius = std::sqrt(bsphere.squared_radius());
      // Defines the implicit surface: requires defining a
      // conservative bounding sphere centered at inner point.
      FT sm_sphere_radius = 5.0 * radius;


      FT sm_dichotomy_error = options.approximation_error_abs/1000.0; // Dichotomy error must be << options.approximation_error_mult
      Surface_3 surface(function,
                        Sphere(inner_point,sm_sphere_radius*sm_sphere_radius),
                        sm_dichotomy_error/sm_sphere_radius);
      // Defines surface mesh generation criteria

      CGAL::Surface_mesh_default_criteria_3<STr> criteria(options.min_triangle_angle,  // Min triangle angle (degrees)
                                                          options.max_triangle_size_abs,  // Max triangle size
                                                          options.approximation_error_abs); // Approximation error
      // Generates surface mesh with manifold option
      STr tr; // 3D Delaunay triangulation for surface mesh generation
      C2t3 c2t3(tr); // 2D complex in 3D Delaunay triangulation
      CGAL::make_surface_mesh(c2t3,                                 // reconstructed mesh
                              surface,                              // implicit surface
                              criteria,                             // meshing criteria
                              CGAL::Manifold_with_boundary_tag());  // require manifold mesh

      std::cout << "Done:  " <<tr.number_of_vertices() <<" points in the end\n";


      typedef viennagrid::mesh                                   MeshType;
      typedef viennagrid::result_of::element<MeshType>::type    VertexType;
      typedef C2t3::Vertex_iterator                 Vertex_iterator;
      typedef C2t3::Facet_iterator                  Facet_iterator;
      std::vector<VertexType> vertex_handles(tr.number_of_vertices());
      {
        Vertex_iterator begin = c2t3.vertices_begin();
        for (int i=0 ; begin != c2t3.vertices_end(); ++begin, ++i)
        {
          vertex_handles[i] = viennagrid::make_vertex( output,
            viennagrid::make_point(begin->point().x(),begin->point().y(),begin->point().z()));
        }
      }
      Facet_iterator begin = c2t3.facets_begin();
      for ( ; begin != c2t3.facets_end(); ++begin)
      {
        viennagrid::make_triangle(
          output,
          vertex_handles[get_id_of_vertex(begin->first->vertex(1)->point(),c2t3)],
          vertex_handles[get_id_of_vertex(begin->first->vertex(2)->point(),c2t3)],
          vertex_handles[get_id_of_vertex(begin->first->vertex(3)->point(),c2t3)]);
      }
    }



    reconstruct_surface::reconstruct_surface() {}

    std::string reconstruct_surface::name() { return "poisson_reconstruct_surface"; }

    bool reconstruct_surface::run(viennamesh::algorithm_handle &)
    {
      point_handle input_points = get_required_input<point_handle>("points");
      point_handle input_normals = get_required_input<point_handle>("normals");
      data_handle<double> min_angle = get_input<double>("min_triangle_angle");
      data_handle<double> max_size_mult = get_input<double>("max_triangle_size_times_spacing");
      data_handle<double> max_size_abs = get_input<double>("max_triangle_size");
      data_handle<double> error_mult = get_input<double>("approximation_error_times_spacing");
      data_handle<double> error_abs = get_input<double>("approximation_error");


      struct poisson_reconstruction_options options;

      if(min_angle.valid() && min_angle() > 0 && min_angle() < 60)
        options.min_triangle_angle=min_angle();
      else
      {
        warning(1) << "Min angle is < 0 or > 60 (used standard of 20 degree instead): " << min_angle() << std::endl;
        options.min_triangle_angle = 20.0;
      }

      if(max_size_mult.valid() && max_size_mult() > 0)
        options.max_triangle_size_mult=max_size_mult();
      else
      {
        warning(1) << "Max triangle size multiplier is < 0 (used standard of 30 instead): " << max_size_mult() << std::endl;
        options.max_triangle_size_mult = 30.0;
      }

      if(max_size_abs.valid() && max_size_abs() > 0)
        options.max_triangle_size_abs = max_size_abs();
      else
      {
        warning(1) << "Max triangle size absolute value is < 0 (used multiplier instead): " << max_size_abs() << std::endl;
        options.max_triangle_size_abs = -1.0;
      }

      if(error_mult.valid() && error_mult() > 0)
        options.approximation_error_mult = error_mult();
      else
      {
        warning(1) << "Max error multiplier is < 0 (used standard of 0.375 instead): " << error_mult() << std::endl;
        options.approximation_error_mult = 0.375;
      }

      if(error_abs.valid() && error_abs() > 0)
        options.approximation_error_abs = error_abs();
      else
      {
        warning(1) << "Max error absolute value is < 0 (used multiplier instead): " << error_abs() << std::endl;
        options.approximation_error_abs = -1.0;
      }

      PointList points;
      for (int i = 0; i != input_points.size(); ++i)
      {
        points.push_back(Point_with_normal(Point(input_points(i)[0],
                                                 input_points(i)[1],
                                                 input_points(i)[2]),
                                           poisson::Vector(input_normals(i)[0],
                                                           input_normals(i)[1],
                                                           input_normals(i)[2])));
      }

      std::cout << "Done:  " <<points.size() <<" points with normals\n";
      std::cout << "options: " << options.min_triangle_angle;
      PointList const & im = points;
      mesh_handle output = make_data<mesh_handle>();
      reconstruct_surface_impl(im,output(),options);
      set_output("mesh", output);

      return true;
    }
  }
}

// #endif
