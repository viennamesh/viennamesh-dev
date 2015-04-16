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
// #include "viennagrid/algorithm/volume.hpp"
#include "viennagridpp/algorithm/geometric_transform.hpp"
#include "viennagridpp/algorithm/centroid.hpp"
#include "viennagridpp/algorithm/distance.hpp"
// #include "viennagrid/algorithm/refine.hpp"
#include "viennagridpp/io/vtk_writer.hpp"
#include "viennagridpp/mesh/object_creation.hpp"

#include "spherical_harmonics.hpp"
#include "generalized_moment.hpp"


// viennagrid::point_t reflect(viennagrid::point_t const & pt, viennagrid::point_t const & axis)
// {
//   return pt - 2.0 * axis * viennagrid::inner_prod(pt, axis);
// }
//
//
// template<bool mesh_is_const>
// double distance(viennagrid::base_mesh<mesh_is_const> const & mesh, viennagrid::point_t const & pt)
// {
//   typedef viennagrid::base_mesh<mesh_is_const> MeshType;
// //   typedef typename viennagrid::result_of::point<MeshType>::type PointType;
//
//   typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
//   typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;
//
//   ConstCellRangeType cells(mesh);
//   if (cells.empty())
//     return -1;
//
//   ConstCellRangeIterator cit = cells.begin();
//   double d = viennagrid::distance(*cit, pt);
//   ++cit;
//
//   for (; cit != cells.end(); ++cit)
//   {
//     double tmp = viennagrid::distance(*cit, pt);
//     d = std::min(d, tmp);
//   }
//
//   return d;
// }
//
//
// template<bool mesh_is_const>
// bool check_mirror(viennagrid::base_mesh<mesh_is_const> const & mesh,
//                   viennagrid::point_t axis,
//                   double tolerance)
// {
//   axis /= viennagrid::norm_2(axis);
//
//   typedef viennagrid::base_mesh<mesh_is_const> MeshType;
//   typedef typename viennagrid::result_of::point<MeshType>::type PointType;
//
//   typedef typename viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
//   typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;
//
//   double max = -1.0;
//   ConstVertexRangeType vertices(mesh);
//   for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
//   {
//     PointType reflected = reflect( viennagrid::get_point(*vit), axis );
//     double d = distance(mesh, reflected);
//     max = std::max(max, d);
//   }
//
//   std::cout << "Max distance = " << max  << std::endl;
//   return max < tolerance;
// }
//
//
//
// template<bool mesh_is_const>
// bool check_rotational(viennagrid::base_mesh<mesh_is_const> const & mesh,
//                       viennagrid::point_t axis, int frequency,
//                       double tolerance)
// {
//   axis /= viennagrid::norm_2(axis);
//
//   double angle = 2*M_PI / frequency;
//   double cos_angle = std::cos(angle);
//   double sin_angle = std::sin(angle);
//
//   ublas::matrix<double> rot(3,3);
//
//   rot(0,0) = axis[0]*axis[0] * (1-cos_angle) + cos_angle;
//   rot(0,1) = axis[0]*axis[1] * (1-cos_angle) - axis[2]*sin_angle;
//   rot(0,2) = axis[0]*axis[2] * (1-cos_angle) + axis[1]*sin_angle;
//
//   rot(1,0) = axis[1]*axis[0] * (1-cos_angle) + axis[2]*sin_angle;
//   rot(1,1) = axis[1]*axis[1] * (1-cos_angle) + cos_angle;
//   rot(1,2) = axis[1]*axis[2] * (1-cos_angle) - axis[0]*sin_angle;
//
//   rot(2,0) = axis[2]*axis[0] * (1-cos_angle) - axis[1]*sin_angle;
//   rot(2,1) = axis[2]*axis[1] * (1-cos_angle) + axis[0]*sin_angle;
//   rot(2,2) = axis[2]*axis[2] * (1-cos_angle) + cos_angle;
//
//
//   typedef viennagrid::base_mesh<mesh_is_const> MeshType;
//   typedef typename viennagrid::result_of::point<MeshType>::type PointType;
//
//   typedef typename viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
//   typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;
//
//   double max = -1.0;
//   ConstVertexRangeType vertices(mesh);
//   for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
//   {
//     PointType reflected = ublas::prod(rot, viennagrid::get_point(*vit));
//     double d = distance(mesh, reflected);
//     max = std::max(max, d);
//   }
//
//   std::cout << "Max distance = " << max  << std::endl;
//   return max < tolerance;
// }








namespace viennamesh
{



  double sin_2p(int two_p, double beta)
  {
    int p = two_p/2;

    double result = 0.0;
    for (int l = 0; l <= p; ++l)
      result += S(p, l) * SphericalHarmonic(2*l,0).real(beta, 0);

    return result;
  }





  double cross_2p(int two_p, point_t const & s, point_t const & w)
  {
    return std::pow( viennagrid::norm_2(viennagrid::cross_prod(s,w)), two_p );
  }

  double cross_2p_sh(int two_p, point_t const & s, point_t const & w)
  {
    double w_theta;
    double w_phi;
    double r;
    to_spherical(w, w_theta, w_phi, r);

    point_t z = viennagrid::make_point(0,0,1);
    point_t axis = viennagrid::cross_prod(z, s);
    double alpha = std::acos( viennagrid::inner_prod(s, z) / viennagrid::norm_2(s) );
    ublas::matrix<double> rot = rotation(axis, -alpha);

//     std::cout << s/viennagrid::norm_2(s) << "  " << ublas::prod(rot, viennagrid::make_point(0,0,1)) << std::endl;

    int p = two_p/2;

    double result = 0.0;
    for (int l = 0; l <= p; ++l)
    {
      double tmp = 0.0;
      for (int m_ = -2*l; m_ <= 2*l; ++m_)
      {
        tmp += D_real(2*l, 0, m_, rot) * SphericalHarmonic(2*l,m_).real(w_theta, w_phi);
      }

      result += tmp * S(p,l);
    }

    return result * std::pow( viennagrid::norm_2(s), two_p );
  }



































}




namespace viennamesh
{
  symmetry_detection_3d::symmetry_detection_3d() {}
  std::string symmetry_detection_3d::name() { return "symmetry_detection_3d"; }

  bool symmetry_detection_3d::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );
    int cell_dimension = viennagrid::cell_dimension( input_mesh() );

    data_handle<int> p = get_required_input<int>("p");
    data_handle<double> relative_integrate_tolerance = get_required_input<double>("relative_integrate_tolerance");
    data_handle<double> absolute_integrate_tolerance = get_required_input<double>("absolute_integrate_tolerance");
    data_handle<int> max_iteration_count = get_required_input<int>("max_iteration_count");
    data_handle<double> mirror_symmetry_tolerance = get_required_input<double>("mirror_symmetry_tolerance");
    data_handle<double> rotational_symmetry_tolerance = get_required_input<double>("rotational_symmetry_tolerance");

    if (geometric_dimension != 3)
      return false;

    if (cell_dimension != 2)
      return false;





    typedef viennagrid::mesh_t MeshType;
    typedef point_t PointType;

    typedef viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;


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

    MeshType mesh;
    viennagrid::copy( input_mesh(), mesh );
    viennagrid::scale( mesh, 1.0/max_size );


    RealGeneralizedMoment m_real(2*p(), mesh, relative_integrate_tolerance(), absolute_integrate_tolerance(), max_iteration_count());
//     ComplexGeneralizedMoment m_complex(2*p(), mesh, relative_integrate_tolerance(), absolute_integrate_tolerance(), max_iteration_count());

    double sphere_radius = 1.0;
    if (get_input<double>("sphere_radius").valid())
      sphere_radius = get_input<double>("sphere_radius")();

    MeshType sphere;
    viennagrid::make_sphere_hull( sphere, viennagrid::make_point(0,0,0), sphere_radius, 4 );

//     viennagrid::quantity_field gradient_field_complex(0, 1);
    viennagrid::quantity_field gradient_field_real(0, 1);
//     gradient_field_complex.set_name("gradient_complex");
    gradient_field_real.set_name("gradient_real");

    ConstVertexRangeType vertices(sphere);
    for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      PointType const & pt = viennagrid::get_point(*vit);

      double theta;
      double phi;
      double r;
      to_spherical(pt, theta, phi, r);

//       double grad_complex = m_complex.grad(theta, phi, 1e-2);
      double grad_real = m_real.grad(theta, phi, 1e-2);

//       gradient_field_complex.set(*vit, grad_complex);
      gradient_field_real.set(*vit, grad_real);
    }

    set_output("sphere", sphere);
    set_output("mesh", mesh);

    quantity_field_handle quantities = make_data<viennagrid::quantity_field>();
    quantities.set(gradient_field_real);
//     quantities.push_back(gradient_field_complex);
    set_output("sphere_quantities", quantities);


    m_real.print();
    std::cout << std::endl;
    std::cout << "m_real hast mirror symmetry: " << std::boolalpha << m_real.z_mirror_symmetry( mirror_symmetry_tolerance() ) << std::endl;
    m_real.rotation_symmetry_angles( rotational_symmetry_tolerance() );
    std::cout << std::endl;



    data_handle<viennamesh_point> rotation_vector = get_input<viennamesh_point>("rotation_vector");

    if (rotation_vector.valid())
    {
      for (int i = 0; i != rotation_vector.size(); ++i)
      {
        point_t new_z = rotation_vector(i);
        std::cout << "Using rotation vector " << new_z << std::endl;
        RealGeneralizedMoment rotated_m = m_real.get_rotated(new_z);

        rotated_m.print();
        std::cout << std::endl;
        std::cout << "rotated_m (z = "<< new_z << ") hast mirror symmetry: " << std::boolalpha << rotated_m.z_mirror_symmetry( mirror_symmetry_tolerance() ) << std::endl;
        rotated_m.rotation_symmetry_angles( rotational_symmetry_tolerance() );
        std::cout << std::endl;
      }
    }





//     m_complex.print();
//     std::cout << std::endl;
//     std::cout << "m_complex hast mirror symmetry: " << std::boolalpha << m_complex.z_mirror_symmetry( mirror_symmetry_tolerance() ) << std::endl;












    int max_l = 3;

//     {
//       double alpha = 0.381;
//       double beta = 3.81;
//       double gamma = 1.92;
//
//       for (int l = 1; l <= max_l; ++l)
//       {
//         std::cout << "l = " << l << std::endl;
//         std::cout << std::endl;
//
//         for (int m = -l; m <= l; ++m)
//         {
//           for (int m_ = -l; m_ <= l; ++m_)
//           {
//   //           std::complex<double> d_full = std::complex<double>(d_complex(l,m,m_,beta),0);
//   //           std::complex<double> d_integrate = d_complex_integrate(l,m,m_,beta);
//   //
//   //           std::cout << d_full << " " <<
//   //                        d_integrate << "        " <<
//   //                        std::abs(d_full-d_integrate) << "     " <<
//   //                        ((std::abs(d_full-d_integrate) > 1e-5) ? "ERROR!" : "") << std::endl;
//
//             std::complex<double> full = D_complex(l, m, m_, alpha, beta, gamma);
//             std::complex<double> integrate = D_complex_integrate(l, m, m_, alpha, beta, gamma);
//
//             std::cout << full << " " <<
//                         integrate << "        " <<
//                         std::abs(full-integrate) << "     " <<
//                         ((std::abs(full-integrate) > 1e-5) ? "ERROR!" : "") << std::endl;
//   //           std::cout << std::endl;
//           }
//
//   //         std::cout << std::endl;
//           std::cout << std::endl;
//         }
//
//         std::cout << std::endl << std::endl;
//       }
//     }

//     {
//       double alpha = 0.381;
//       double beta = 3.81;
//       double gamma = 1.92;
//
//       for (int l = 1; l <= max_l; ++l)
//       {
//         std::cout << "l = " << l << std::endl;
//         std::cout << std::endl;
//
//         for (int m = -l; m <= l; ++m)
//         {
//           for (int m_ = -l; m_ <= l; ++m_)
//           {
//             double full = D_real(l, m, m_, alpha, beta, gamma);
//             double integrate = D_real_integrate(l, m, m_, alpha, beta, gamma);
//
//             std::cout << full << " " <<
//                         integrate << "        " <<
//                         std::abs(full-integrate) << "     " <<
//                         ((std::abs(full-integrate) > 1e-5) ? "ERROR!" : "") << std::endl;
//   //           std::cout << std::endl;
//           }
//
//   //         std::cout << std::endl;
//           std::cout << std::endl;
//         }
//
//         std::cout << std::endl << std::endl;
//       }
//     }





//     double alpha = 0.381;
//     double beta = 3.81;
//     double gamma = 1.92;
//     ublas::matrix<double> rot = euler_rotation_zyz(alpha, beta, gamma);
//
//     int theta_count = 10;
//     int phi_count = 20;
//
//     double theta_step = M_PI/theta_count;
//     double phi_step = 2*M_PI/phi_count;
//
//     for (int theta_index = 0; theta_index != theta_count; ++theta_index)
//       for (int phi_index = 0; phi_index != phi_count; ++phi_index)
//       {
//         double theta = theta_step*theta_index;
//         double phi = phi_step*phi_index;
//
//         std::pair<double, double> rotated = prod(rot, theta, phi);
//
//         for (int l = 0; l <= 4; ++l)
//           for (int m = -l; m <= l; ++m)
//           {
// //             std::complex<double> sh_rot = SphericalHarmonic(l,m).complex(rotated.first, rotated.second);
// //             std::complex<double> sh_rot_sum = SH_rotated_complex(l, m, alpha, beta, gamma, theta, phi);
// //
// //             if ( std::abs(sh_rot-sh_rot_sum) > 1e-6 )
// //               std::cout << sh_rot << " " << sh_rot_sum << "      " << std::abs(sh_rot-sh_rot_sum) << std::endl;
//
//
//             std::complex<double> sh_rot = SphericalHarmonic(l,m).real(rotated.first, rotated.second);
//             std::complex<double> sh_rot_sum = SH_rotated_real(l, m, alpha, beta, gamma, theta, phi);
//
//             if ( std::abs(sh_rot-sh_rot_sum) > 1e-6 )
//               std::cout << sh_rot << " " << sh_rot_sum << "      " << std::abs(sh_rot-sh_rot_sum) << std::endl;
//           }
//       }







//     std::cout << D_complex(2, 0, -2, 0, 1.31, 0) << "  " << D_complex_full(2, 0, 0, 1.31) << std::endl;
//     std::cout << D_complex(2, 0, 0, 0, 1.31, 0) << "  " << D_complex_full(2, 0, 0, 1.31) << std::endl;
//     std::cout << D_complex(2, 0, 0, 0, 1.31, 0) << "  " << D_complex_full(2, 0, 0, 1.31) << std::endl;
//     std::cout << D_complex(2, 0, 0, 0, 1.31, 0) << "  " << D_complex_full(2, 0, 0, 1.31) << std::endl;





//     point_t new_z = viennagrid::make_point(1,1,1);
//     RealGeneralizedMoment rotated_m = m_real.get_rotated(new_z);
//
//     rotated_m.print();
//     std::cout << std::endl;
//     std::cout << "rotated_m (z = "<< new_z << ") hast mirror symmetry: " << std::boolalpha << rotated_m.z_mirror_symmetry( mirror_symmetry_tolerance() ) << std::endl;
//     rotated_m.rotation_symmetry_angles( rotational_symmetry_tolerance() );
//     std::cout << std::endl;






    return true;
  }

}
