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

#include <set>
#include <map>
#include <iterator>
#include <iomanip>
#include <fstream>
#include <complex>
#include <functional>

#include "detection_3d.hpp"
#include "viennagrid/algorithm/geometric_transform.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/mesh/object_creation.hpp"

#include "spherical_harmonics.hpp"
#include "generalized_moment.hpp"
#include "generalized_moment_tests.hpp"
#include "check_symmetries.hpp"




namespace viennamesh
{
  symmetry_detection_3d::symmetry_detection_3d() {}
  std::string symmetry_detection_3d::name() { return "symmetry_detection_3d"; }











  bool symmetry_detection_3d::run(viennamesh::algorithm_handle &)
  {


//
//     std::cout << "dynamic: " << jacobi_polynom<double>(4,2,2) << std::endl;
//     std::cout << "static: " << static_jacobi_polynom<double,4>(2,2) << std::endl;
//
//     return true;






    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );
    int cell_dimension = viennagrid::cell_dimension( input_mesh() );

    data_handle<int> p = get_required_input<int>("p");
    data_handle<double> relative_integrate_tolerance = get_required_input<double>("relative_integrate_tolerance");
    data_handle<double> absolute_integrate_tolerance = get_required_input<double>("absolute_integrate_tolerance");
//     data_handle<int> max_iteration_count = get_required_input<int>("max_iteration_count");
    data_handle<double> mirror_symmetry_tolerance = get_required_input<double>("mirror_symmetry_tolerance");
    data_handle<double> rotational_symmetry_tolerance = get_required_input<double>("rotational_symmetry_tolerance");

    if (geometric_dimension != 3)
      return false;

    if (cell_dimension != 2)
      return false;



    typedef viennagrid::mesh    MeshType;
    typedef point               PointType;

    typedef viennagrid::result_of::const_vertex_range<MeshType>::type       ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type     ConstVertexRangeIterator;


    double max_size = 0.0;
    {
      ConstVertexRangeType vertices(input_mesh());
      for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
      {
        double cur_size = viennagrid::norm_2( viennagrid::get_point(*vit) );
        if (cur_size > max_size)
          max_size = cur_size;
      }
    }

    info(1) << "Before start" << std::endl;

    MeshType mesh;
    viennagrid::copy( input_mesh(), mesh );
    viennagrid::scale( mesh, 1.0/max_size );

    info(1) << "After copy/scale" << std::endl;


    viennautils::Timer timer;
    timer.start();
    RealGeneralizedMoment m_real(2*p(), mesh);
//     , relative_integrate_tolerance(), absolute_integrate_tolerance(), max_iteration_count());

    info(1) << "After calculating generalized moment (!!! took " << timer.get() << "sec !!!)" << std::endl;

    double sphere_radius = 1.0;
    if (get_input<double>("sphere_radius").valid())
      sphere_radius = get_input<double>("sphere_radius")();

    MeshType sphere;
    viennagrid::make_sphere_hull( sphere, viennagrid::make_point(0,0,0), sphere_radius, 4 );

    viennagrid::quantity_field gradient_field_real(0, 1);
    gradient_field_real.set_name("gradient_real");

    ConstVertexRangeType vertices(sphere);
    for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      PointType const & pt = viennagrid::get_point(*vit);

      double theta;
      double phi;
      double r;
      to_spherical(pt, theta, phi, r);

      double grad_real = m_real.grad(theta, phi, 1e-2);
      gradient_field_real.set(*vit, grad_real);
    }

//     {
//       int bench_count = 100000;
//       std::vector<double> v(bench_count);
//       viennamesh::LoggingStack s("bench");
//
//       for (int i = 0; i != bench_count; ++i)
//         v[i] = m_real.grad(i*0.1, i*0.2, 1e-2);
//     }

    info(1) << "After calculating sphere" << std::endl;

    set_output("sphere", sphere);
    set_output("mesh", mesh);

    quantity_field_handle quantities = make_data<viennagrid::quantity_field>();
    quantities.set(gradient_field_real);
    set_output("sphere_quantities", quantities);


//     m_real.print();
//     std::cout << std::endl;
//     std::cout << "m_real hast mirror symmetry: " << std::boolalpha << m_real.z_mirror_symmetry( mirror_symmetry_tolerance() ) << std::endl;
//     m_real.rotation_symmetry_angles();
//     // rotational_symmetry_tolerance() );
//     std::cout << std::endl;



    data_handle<viennamesh_point> rotation_vector = get_input<viennamesh_point>("rotation_vector");
    data_handle<int> rotational_frequencies = get_input<int>("rotational_frequencies");

    if (rotation_vector.valid())
    {
      for (int i = 0; i != rotation_vector.size(); ++i)
      {
        point new_z = rotation_vector(i);
        info(1) << "Using rotation vector " << new_z << std::endl;
        RealGeneralizedMoment rotated_m = m_real.get_rotated(new_z);

//         rotated_m.print();
//         std::cout << std::endl;
        info(1) << "rotated_m (z = "<< new_z << ") hast mirror symmetry: " << std::boolalpha << rotated_m.z_mirror_symmetry( mirror_symmetry_tolerance() ) << std::endl;
//         rotated_m.rotation_symmetry_angles();
//         rotated_m.rotation_symmetry_angles( rotational_symmetry_tolerance() );
        rotated_m.check_rotation_symmetry(M_PI);

        if (rotational_frequencies.valid())
        {
          for (int i = 0; i != rotational_frequencies.size(); ++i)
          {
            int rotational_frequency = rotational_frequencies(i);
            double angle = 2*M_PI/rotational_frequency;
            info(1) << "Using rotational frequency " << rotational_frequency << " (angle = " << angle << ") error = " << rotated_m.check_rotation_symmetry(angle) << std::endl;
          }
        }
      }
    }


    return true;
  }

}
