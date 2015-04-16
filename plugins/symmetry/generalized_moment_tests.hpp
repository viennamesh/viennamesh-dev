#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_GENERALIZED_MOMENT_TESTS_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_GENERALIZED_MOMENT_TESTS_HPP

#include "generalized_moment.hpp"

namespace viennamesh
{


  template<typename T>
  T rotate_SH(int l, int m,
               double alpha, double beta, double gamma,
               double theta, double phi)
  {
    T result = 0.0;
    for (int m_ = -l; m_ <= l; ++m_)
    {
      result += D<T>(l, m, m_, alpha, beta, gamma) * SphericalHarmonic<T>(l,m_)(theta, phi);
    }
    return result;
  }



  template<typename T>
  void test_rotate_D(int max_l, double tolerance)
  {
    int alpha_count = 10;
    int beta_count = 10;
    int gamma_count = 10;

    double alpha_step = 2*M_PI/alpha_count;
    double beta_step = 2*M_PI/beta_count;
    double gamma_step = 2*M_PI/gamma_count;


    int theta_count = 10;
    int phi_count = 20;

    double theta_step = M_PI/theta_count;
    double phi_step = 2*M_PI/phi_count;

    for (int alpha_index = 0; alpha_index != alpha_count; ++ alpha_index)
      for (int beta_index = 0; beta_index != beta_count; ++ beta_index)
        for (int gamma_index = 0; gamma_index != gamma_count; ++ gamma_index)
        {
          double alpha = alpha_index*alpha_step;
          double beta = beta_index*beta_step;
          double gamma = gamma_index*gamma_step;

          ublas::matrix<double> rot = euler_rotation_zyz(alpha, beta, gamma);

          for (int theta_index = 0; theta_index != theta_count; ++theta_index)
            for (int phi_index = 0; phi_index != phi_count; ++phi_index)
            {
              double theta = theta_step*theta_index;
              double phi = phi_step*phi_index;

              std::pair<double, double> rotated = prod(rot, theta, phi);

              for (int l = 0; l <= max_l; ++l)
                for (int m = -l; m <= l; ++m)
                {
                  T sh_rot = SphericalHarmonic<T>(l,m)(rotated.first, rotated.second);
                  T sh_rot_sum = rotate_SH<T>(l, m, alpha, beta, gamma, theta, phi);

                  if ( std::abs(sh_rot-sh_rot_sum) > tolerance )
                    std::cout << "(alpha = " << alpha << ", beta = " << beta << ", gamma = " << gamma << ") (theta = " << theta << ", phi = " << phi << ")   direct rotated = " << sh_rot << " rotated using formular = " << sh_rot_sum << "      error = " << std::abs(sh_rot-sh_rot_sum) << std::endl;
                }
            }
        }
  }


  template<typename T>
  void test_D(int max_l, double tolerance)
  {
    int alpha_count = 10;
    int beta_count = 10;
    int gamma_count = 10;

    double alpha_step = 2*M_PI/alpha_count;
    double beta_step = 2*M_PI/beta_count;
    double gamma_step = 2*M_PI/gamma_count;


    int theta_count = 10;
    int phi_count = 20;

    double theta_step = M_PI/theta_count;
    double phi_step = 2*M_PI/phi_count;

    for (int alpha_index = 0; alpha_index != alpha_count; ++ alpha_index)
      for (int beta_index = 0; beta_index != beta_count; ++ beta_index)
        for (int gamma_index = 0; gamma_index != gamma_count; ++ gamma_index)
        {
          double alpha = alpha_index*alpha_step;
          double beta = beta_index*beta_step;
          double gamma = gamma_index*gamma_step;

          ublas::matrix<double> rot = euler_rotation_zyz(alpha, beta, gamma);

          for (int theta_index = 0; theta_index != theta_count; ++theta_index)
            for (int phi_index = 0; phi_index != phi_count; ++phi_index)
            {
              double theta = theta_step*theta_index;
              double phi = phi_step*phi_index;

              for (int l = 0; l <= max_l; ++l)
                for (int m = -l; m <= l; ++m)
                  for (int m_ = -l; m_ <= l; ++m_)
                  {

                    T direct = D<T>(l, m, m_, alpha, beta, gamma);
                    T integrate = D_integrate<T>(l, m, m_, alpha, beta, gamma);

                    if ( std::abs(direct-integrate) > tolerance )
                      std::cout << "(alpha = " << alpha << ", beta = " << beta << ", gamma = " << gamma << ") (theta = " << theta << ", phi = " << phi << ")   direct = " << direct << " integrated = " << integrate << "      error = " << std::abs(direct-integrate) << std::endl;
                  }
            }
        }
  }






//   double sin_2p(int two_p, double beta)
//   {
//     int p = two_p/2;
//
//     double result = 0.0;
//     for (int l = 0; l <= p; ++l)
//       result += S(p, l) * SphericalHarmonic(2*l,0).real(beta, 0);
//
//     return result;
//   }
//
//
//
//
//
//   double cross_2p(int two_p, point_t const & s, point_t const & w)
//   {
//     return std::pow( viennagrid::norm_2(viennagrid::cross_prod(s,w)), two_p );
//   }
//
//   double cross_2p_sh(int two_p, point_t const & s, point_t const & w)
//   {
//     double w_theta;
//     double w_phi;
//     double r;
//     to_spherical(w, w_theta, w_phi, r);
//
//     point_t z = viennagrid::make_point(0,0,1);
//     point_t axis = viennagrid::cross_prod(z, s);
//     double alpha = std::acos( viennagrid::inner_prod(s, z) / viennagrid::norm_2(s) );
//     ublas::matrix<double> rot = rotation(axis, -alpha);
//
// //     std::cout << s/viennagrid::norm_2(s) << "  " << ublas::prod(rot, viennagrid::make_point(0,0,1)) << std::endl;
//
//     int p = two_p/2;
//
//     double result = 0.0;
//     for (int l = 0; l <= p; ++l)
//     {
//       double tmp = 0.0;
//       for (int m_ = -2*l; m_ <= 2*l; ++m_)
//       {
//         tmp += D_real(2*l, 0, m_, rot) * SphericalHarmonic(2*l,m_).real(w_theta, w_phi);
//       }
//
//       result += tmp * S(p,l);
//     }
//
//     return result * std::pow( viennagrid::norm_2(s), two_p );
//   }



}

#endif
