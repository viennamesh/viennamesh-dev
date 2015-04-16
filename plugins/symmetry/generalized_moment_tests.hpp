#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_GENERALIZED_MOMENT_TESTS_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_GENERALIZED_MOMENT_TEST_HPP

#include "generalized_moment.hpp"

namespace viennamesh
{

  template<typename T>
  T rotate_SH(int l, int m,
               double alpha, double beta, double gamma,
               double theta, double phi);

  template<>
  double rotate_SH<double>(int l, int m,
                            double alpha, double beta, double gamma,
                            double theta, double phi)
  {
    double result = 0.0;
    for (int m_ = -l; m_ <= l; ++m_)
    {
      result += D_real(l, m, m_, alpha, beta, gamma) * SphericalHarmonic(l,m_).real(theta, phi);
    }
    return result;
  }

  template<>
  std::complex<double> rotate_SH< std::complex<double> >(int l, int m,
                                                          double alpha, double beta, double gamma,
                                                          double theta, double phi)
  {
    std::complex<double> result = 0.0;
    for (int m_ = -l; m_ <= l; ++m_)
    {
      result += D_complex(l, m, m_, alpha, beta, gamma) * SphericalHarmonic(l,m_).complex(theta, phi);
    }
    return result;
  }




  template<typename T>
  void test_rotate_D()
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

              for (int l = 0; l <= 4; ++l)
                for (int m = -l; m <= l; ++m)
                {
      //             std::complex<double> sh_rot = SphericalHarmonic(l,m).complex(rotated.first, rotated.second);
      //             std::complex<double> sh_rot_sum = SH_rotated_complex(l, m, alpha, beta, gamma, theta, phi);
      //
      //             if ( std::abs(sh_rot-sh_rot_sum) > 1e-6 )
      //               std::cout << sh_rot << " " << sh_rot_sum << "      " << std::abs(sh_rot-sh_rot_sum) << std::endl;


                  std::complex<double> sh_rot = SphericalHarmonic(l,m).real(rotated.first, rotated.second);
                  std::complex<double> sh_rot_sum = SH_rotated_real(l, m, alpha, beta, gamma, theta, phi);

                  if ( std::abs(sh_rot-sh_rot_sum) > 1e-6 )
                    std::cout << sh_rot << " " << sh_rot_sum << "      " << std::abs(sh_rot-sh_rot_sum) << std::endl;
                }
            }
        }
  }




}

#endif
