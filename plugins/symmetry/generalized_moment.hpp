#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_GENERALIZED_MOMENT_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_GENERALIZED_MOMENT_HPP

#include "common.hpp"
#include "integrate.hpp"
#include <boost/numeric/ublas/io.hpp>

namespace viennamesh
{






  
  std::complex<double> D_complex_integrate_(int l, int m, int m_,
                              ublas::matrix<double> const & rot,
                              double theta, double phi)
  {
    std::pair<double, double> rotated = prod(rot, theta, phi);

    return SphericalHarmonic(l, m).complex(rotated.first, rotated.second) *
           SphericalHarmonic(l, m_).conj_complex(theta, phi) *
           std::sin(theta);
  }

  std::complex<double> D_complex_integrate(int l, int m, int m_,
                                           ublas::matrix<double> const & rot)
  {
    int theta_count = 500;
    int phi_count = 1000;

    double theta_step = M_PI / theta_count;
    double phi_step = 2.0*M_PI / phi_count;

    std::complex<double> result = 0.0;

    for (int theta_index = 0; theta_index != theta_count; ++theta_index)
      for (int phi_index = 0; phi_index != phi_count; ++phi_index)
      {
        double theta = theta_index * theta_step + theta_step*0.5;
        double phi = phi_index * phi_step + phi_step*0.5;

        result += D_complex_integrate_(l, m, m_, rot, theta, phi);
      }

    return result * theta_step * phi_step;
  }

  std::complex<double> d_complex_integrate(int l, int m, int m_, double beta)
  {
    return D_complex_integrate(l, m, m_, rotation_y(-beta));
  }

  std::complex<double> D_complex_integrate(int l, int m, int m_,
                             double alpha, double beta, double gamma)
  {
    ublas::matrix<double> rot = euler_rotation_zyz(alpha, beta, gamma);
    return D_complex_integrate(l, m, m_, rot);
  }



//   std::complex<double> D_complex(int l, int m, int m_,
//                                  double /*alpha*/, double beta, double gamma)
//   {
//     assert(m == 0);
//
//     // A signal-processing framework for reflection, doi>10.1145/1027411.1027416, (31).1
//     // http://cseweb.ucsd.edu/~ravir/p1004-ramamoorthi.pdf
//     std::complex<double> d_ = std::sqrt( 4*M_PI / (2.0*l+1.0) ) * SphericalHarmonic(l,m_).conj_complex(beta, M_PI);
//
//     // A signal-processing framework for reflection, doi>10.1145/1027411.1027416, (27)
//     // http://cseweb.ucsd.edu/~ravir/p1004-ramamoorthi.pdf
//     d_ *= std::exp( std::complex<double>(0,1)*static_cast<double>(m_)*gamma );
//
//     // Maple procedures for the coupling of angular momenta. IX. Wigner D-functions and rotation matrices,         doi:10.1016/j.cpc.2005.12.008, (4)
//     // http://www.atomic-theory.uni-jena.de/pub/p186.b06.cpc-racahIX-original.pdf
//     d_ *= power_mone(l-m_);
//
// //     std::cout << d_ << std::endl;
//
//     return d_;
//   }


  class Jacobi
  {
  public:

    Jacobi(long n_, long a_, long b_) : n(n_), a(a_), b(b_) {}

    template<typename T>
    T operator()(T const & x) const
    {
      T sum = 0;

      for (int s = 0; s <= std::min(n,n+a); ++s)
      {
        sum += 1.0 / (factorial(s) * factorial(n+a-s) * factorial(b+s) * factorial(n-s)) *
               std::pow((x-1)/2, n-s) * std::pow((x+1)/2, s);
      }

      return factorial(n+a) * factorial(n+b) * sum;
    }

  private:
    long n;
    long a;
    long b;
  };


  double d_complex(int l, int m, int m_, double beta)
  {
    if (m < 0 && m_ < 0)
      return power_mone(m-m_) * d_complex(l, -m, -m_, beta);
    if (m_ < 0)
      return power_mone(l+m) * d_complex(l, m, -m_, M_PI - beta);
    if (m < 0)
      return power_mone(m-m_) * d_complex(l, -m, -m_, beta);


    return std::sqrt( (factorial(l+m_) * factorial(l-m_)) / (factorial(l+m) * factorial(l-m)) ) *
           std::pow(std::sin(beta/2), m_-m) * std::pow(std::cos(beta/2), m_+m) *
           Jacobi(l-m_, m_-m, m_+m)(std::cos(beta));
  }

  std::complex<double> D_complex(int l, int m, int m_,
                                 double alpha, double beta, double gamma)
  {
    return std::conj(d_complex(l,m,m_,-beta) *
           std::exp( std::complex<double>(0, -m*alpha) ) *
           std::exp( std::complex<double>(0, -m_*gamma) ));
  }





  std::complex<double> D_complex(int l, int m, int m_, point_t const & s)
  {
    double s_theta;
    double s_phi;
    double s_r;
    to_spherical(s, s_theta, s_phi, s_r);
    return D_complex(l, m, m_, 0, -s_theta, -s_phi);
  }


  std::complex<double> C_complex_(int two_l, int m, int two_p, point_t const & s)
  {
    if (two_l%2 != 0)
      abort();

    if (two_p%2 != 0)
      abort();

    return std::pow(viennagrid::norm_2(s), two_p) * D_complex(two_l, 0, m, s);
  }






  template<bool mesh_is_const>
  std::complex<double> C_complex(int two_l, int m, int two_p,
                                 viennagrid::base_mesh<mesh_is_const> const & mesh,
                                 double relative_integrate_tolerance, double absolute_integrate_tolerance, int max_integrate_iterations)
  {
    typedef viennagrid::base_mesh<mesh_is_const> MeshType;
    typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRange;
    typedef typename viennagrid::result_of::iterator<ConstCellRange>::type ConstCellIterator;

    if (two_l%2 != 0)
      abort();

    if (two_p%2 != 0)
      abort();

    std::complex<double> result = 0.0;
    int l = two_l/2;
    int p = two_p/2;


    std::function< std::complex<double> (point_t const &) > cf =
        std::bind< std::complex<double> >(C_complex_, two_l, m, two_p, std::placeholders::_1);


    ConstCellRange cells( mesh );
    for (ConstCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      result += integrate( *cit, cf, relative_integrate_tolerance, absolute_integrate_tolerance, max_integrate_iterations );
    }

    return result * S(p,l);
  }




















  double D_real_(int l, int m, int m_,
                 ublas::matrix<double> const & rot,
                 double theta, double phi)
  {
    std::pair<double, double> rotated = prod(rot, theta, phi);

    return SphericalHarmonic(l, m).real(rotated.first, rotated.second) *
           SphericalHarmonic(l, m_).real(theta, phi) *
           std::sin(theta);
  }


  double D_real(int l, int m, int m_,
                ublas::matrix<double> const & rot)
  {
    int theta_count = 500;
    int phi_count = 1000;

    double theta_step = M_PI / theta_count;
    double phi_step = 2.0*M_PI / phi_count;

    double result = 0.0;

    for (int theta_index = 0; theta_index != theta_count; ++theta_index)
      for (int phi_index = 0; phi_index != phi_count; ++phi_index)
      {
        double theta = theta_index * theta_step + theta_step*0.5;
        double phi = phi_index * phi_step + phi_step*0.5;

        result += D_real_(l, m, m_, rot, theta, phi);
      }

    return result * theta_step * phi_step;
  }

  double D_real_integrate(int l, int m, int m_, double beta)
  {
    return D_real(l, m, m_, rotation_y(-beta));
  }

  double D_real_integrate(int l, int m, int m_,
                          double alpha, double beta, double gamma)
  {
    ublas::matrix<double> rot = euler_rotation_zyz(alpha, beta, gamma);

//     ublas::prod(rotation_y(-beta), rotation_z(-gamma));
//     rot = ublas::prod(rotation_z(-alpha), rot);

    return D_real(l, m, m_, rot);
  }


  double D_real(int two_l, int m, double m_, double s_theta, double s_phi)
  {
    assert(m == 0);

    if (m_ < 0)
    {
      return std::sqrt(2) * power_mone(two_l+m_) *
             std::sqrt( static_cast<double>(factorial(two_l - m_)) / static_cast<double>(factorial(two_l + m_)) ) *
             AssocLegendre(two_l, m_)( std::cos(-s_theta) ) * std::sin(m_ * -s_phi);
    }
    else if (m_ == 0)
    {
      return power_mone(two_l) * AssocLegendre(two_l,0)(std::cos(-s_theta));
    }
    else // (m_ > 0)
    {
      return std::sqrt(2) * power_mone(two_l) *
             std::sqrt( static_cast<double>(factorial(two_l - m_)) / static_cast<double>(factorial(two_l + m_)) ) *
             AssocLegendre(two_l, m_)( std::cos(-s_theta) ) * std::cos(m_ * -s_phi);
    }
  }




  double D_real(int l, int m, double m_,
                double alpha, double beta, double gamma)
  {
    std::complex<double> result;
    if (m == 0)
    {
      if (m_ < 0)
      {
        result = (power_mone(m_) * D_complex(l,m,-m_,alpha,beta,gamma) - D_complex(l,m,m_,alpha,beta,gamma)) *
                std::complex<double>(0,1) / std::sqrt(2);
      }
      else if (m_ == 0)
      {
        result = D_complex(l,m,m_,alpha,beta,gamma);
      }
      else // (m_ > 0)
      {
        result = (power_mone(m_) * D_complex(l,m,m_,alpha,beta,gamma) + D_complex(l,m,-m_,alpha,beta,gamma)) / std::sqrt(2);
      }
    }
    else if (m_ == 0)
    {
      if (m < 0)
      {
        result = (- power_mone(m) * D_complex(l,-m,m_,alpha,beta,gamma) + D_complex(l,m,m_,alpha,beta,gamma)) * std::complex<double>(0,1) / std::sqrt(2);
      }
      else if (m > 0)
      {
        result = (power_mone(m) * D_complex(l,m,m_,alpha,beta,gamma) + D_complex(l,-m,m_,alpha,beta,gamma)) / std::sqrt(2);
      }
    }
    else
    {
      if (m > 0 && m_ > 0)
      {
        m = std::abs(m);
        m_ = std::abs(m_);
        result = 1.0 / 2.0 *
                 (+ D_complex(l,-m,-m_,alpha,beta,gamma)
                  + power_mone(m_) * D_complex(l,-m,m_,alpha,beta,gamma)
                  + power_mone(m) * D_complex(l,m,-m_,alpha,beta,gamma)
                  + power_mone(m+m_) * D_complex(l,m,m_,alpha,beta,gamma));
      }

      if (m > 0 && m_ < 0)
      {
        m = std::abs(m);
        m_ = std::abs(m_);
        result = std::complex<double>(0,1) / 2.0 *
                 (- D_complex(l,-m,-m_,alpha,beta,gamma)
                  + power_mone(m_) * D_complex(l,-m,m_,alpha,beta,gamma)
                  - power_mone(m) * D_complex(l,m,-m_,alpha,beta,gamma)
                  + power_mone(m+m_) * D_complex(l,m,m_,alpha,beta,gamma));
      }

      if (m < 0 && m_ > 0)
      {
        m = std::abs(m);
        m_ = std::abs(m_);
        result = std::complex<double>(0,1) / 2.0 *
                 (+ D_complex(l,-m,-m_,alpha,beta,gamma)
                  + power_mone(m_) * D_complex(l,-m,m_,alpha,beta,gamma)
                  - power_mone(m) * D_complex(l,m,-m_,alpha,beta,gamma)
                  - power_mone(m+m_) * D_complex(l,m,m_,alpha,beta,gamma));
      }

      if (m < 0 && m_ < 0)
      {
        m = std::abs(m);
        m_ = std::abs(m_);
        result = 1.0 / 2.0 *
                 (+ D_complex(l,-m,-m_,alpha,beta,gamma)
                  - power_mone(m_) * D_complex(l,-m,m_,alpha,beta,gamma)
                  - power_mone(m) * D_complex(l,m,-m_,alpha,beta,gamma)
                  + power_mone(m+m_) * D_complex(l,m,m_,alpha,beta,gamma));
      }
    }

    return result.real();
  }


  double D_real(int two_l, int m, double m_, point_t const & s)
  {
    double s_r = std::sqrt( s[0]*s[0] + s[1]*s[1] + s[2]*s[2] );
    double s_phi= atan2(s[1],s[0]);

    if (m_ < 0)
    {
      return std::sqrt(2) * power_mone(two_l+m_) *
             std::sqrt( static_cast<double>(factorial(two_l - m_)) / static_cast<double>(factorial(two_l + m_)) ) *
             AssocLegendre(two_l, m_)( s[2]/s_r/*std::cos(-s_theta)*/ ) * std::sin(m_ * (-s_phi));
    }
    else if (m_ == 0)
    {
      return power_mone(two_l) * AssocLegendre(two_l,0)(s[2]/s_r);
    }
    else // (m_ > 0)
    {
      return std::sqrt(2) * power_mone(two_l) *
             std::sqrt( static_cast<double>(factorial(two_l - m_)) / static_cast<double>(factorial(two_l + m_)) ) *
             AssocLegendre(two_l, m_)( s[2]/s_r/*std::cos(-s_theta)*/ ) * std::cos(m_ * (-s_phi));
    }


//     std::complex<double> tmp;
//     if (m_ < 0)
//     {
//       tmp = (power_mone(m_) * D_complex(two_l,0,-m_,s) - D_complex(two_l,0,m_,s)) *
//               std::complex<double>(0,1) / std::sqrt(2);
//     }
//     else if (m_ == 0)
//     {
//       tmp = D_complex(two_l,0,0,s);
//     }
//     else // (m_ > 0)
//     {
//       tmp = (power_mone(m_) * D_complex(two_l,0,m_,s) + D_complex(two_l,0,-m_,s)) / std::sqrt(2);
//     }
//     return tmp.real();
  }




  double C_real_(int two_l, int m, int two_p, point_t const & s)
  {
    if (two_l%2 != 0)
      abort();

    if (two_p%2 != 0)
      abort();

    return std::pow(viennagrid::norm_2(s), two_p) * D_real(two_l, 0, m, s);//  D(0, m, l, s);
  }






  template<bool mesh_is_const>
  double C_real(int two_l, int m, int two_p,
                viennagrid::base_mesh<mesh_is_const> const & mesh,
                double relative_integrate_tolerance, double absolute_integrate_tolerance, int max_integrate_iterations)
  {
    typedef viennagrid::base_mesh<mesh_is_const> MeshType;
    typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRange;
    typedef typename viennagrid::result_of::iterator<ConstCellRange>::type ConstCellIterator;

    if (two_l%2 != 0)
      abort();

    if (two_p%2 != 0)
      abort();

    double result = 0.0;
    int l = two_l/2;
    int p = two_p/2;


    std::function< double (point_t const &) > cf =
        std::bind< double >(C_real_, two_l, m, two_p, std::placeholders::_1);


    ConstCellRange cells( mesh );
    for (ConstCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      result += integrate( *cit, cf, relative_integrate_tolerance, absolute_integrate_tolerance, max_integrate_iterations );
    }

    return result * S(p,l);
  }


}













namespace viennamesh
{
  template<typename T>
  T C(int two_l, int m, int two_p,
                viennagrid::mesh_t const & mesh,
                double relative_integrate_tolerance, double absolute_integrate_tolerance, int max_integrate_iterations);


  template<>
  double C<double>(int two_l, int m, int two_p,
                   viennagrid::mesh_t const & mesh,
                   double relative_integrate_tolerance, double absolute_integrate_tolerance, int max_integrate_iterations)
  {
    return C_real(two_l, m, two_p, mesh, relative_integrate_tolerance, absolute_integrate_tolerance, max_integrate_iterations);
  }

  template<>
  std::complex<double> C< std::complex<double> >(int two_l, int m, int two_p,
                   viennagrid::mesh_t const & mesh,
                   double relative_integrate_tolerance, double absolute_integrate_tolerance, int max_integrate_iterations)
  {
    return C_complex(two_l, m, two_p, mesh, relative_integrate_tolerance, absolute_integrate_tolerance, max_integrate_iterations);
  }



  template<typename T>
  T D(int l, int m, int two_p,
      double alpha, double beta, double gamma);

  template<>
  double D<double>(int l, int m, int m_,
      double alpha, double beta, double gamma)
  {
    return D_real(l, m, m_, alpha, beta, gamma);
  }

  template<>
  std::complex<double> D< std::complex<double> >(int l, int m, int m_,
      double alpha, double beta, double gamma)
  {
    return D_complex(l, m, m_, alpha, beta, gamma);
  }


  template<typename CT>
  class GeneralizedMoment
  {
  public:

    typedef CT CType;

    template<typename MeshT>
    GeneralizedMoment(int two_p_,
            MeshT const & mesh,
            double relative_integrate_tolerance, double absolute_integrate_tolerance, int max_integrate_iterations)
    {
      assert(two_p_ % 2 == 0);
      set_p(two_p_/2);

      for (int l = 0; l <= p(); ++l)
        for (int m = -2*l; m <= 2*l; ++m)
          values[l][m+2*l] = viennamesh::C_real(2*l, m, 2*p(), mesh,
                                                relative_integrate_tolerance, absolute_integrate_tolerance,
                                                max_integrate_iterations);

    }

    double operator()(double theta, double phi) const
    {
      CType sum = 0.0;
      for (int l = 0; l <= p(); ++l)
      {
        for (int m = -2*l; m <= 2*l; ++m)
        {
          sum += this->C(2*l, m) * SphericalHarmonic(2*l,m).real(theta, phi);
        }
      }

      return std::real(sum);
    }

    double operator()(point_t const & pt) const
    {
      double theta;
      double phi;
      double r;
      to_spherical(pt, theta, phi, r);
      return (*this)(theta, phi);
    }

    double grad(double theta, double phi, double eps) const
    {
      double theta_m_eps = (*this)(theta-eps, phi);
      double theta_p_eps = (*this)(theta+eps, phi);

      double d_theta = (theta_p_eps-theta_m_eps) / (2.0*eps);

      double phi_m_eps = (*this)(theta, phi-eps);
      double phi_p_eps = (*this)(theta, phi+eps);

      double d_phi = (phi_p_eps-phi_m_eps) / (2.0*eps);

      return std::sqrt(d_theta*d_theta + d_phi*d_phi);
    }

    double grad(point_t const & pt, double eps) const
    {
      double theta;
      double phi;
      double r;
      to_spherical(pt, theta, phi, r);
      return grad(theta, phi, eps);
    }


    void print() const
    {
      for (int l = 0; l <= p(); ++l)
      {
        std::cout << "2l = " << 2*l << std::endl;
        for (int m = -2*l; m <= 2*l; ++m)
        {
          std::cout << "     C(" << 2*l << "," << m << ") = " << C(2*l,m) << std::endl;
        }
        std::cout << std::endl << std::endl;
      }
    }

    CType C(int two_l, int m) const
    {
      assert(two_l % 2 == 0);
      return values[two_l/2][m+two_l];
    }

    int p() const { return p_; }

    GeneralizedMoment get_rotated(double alpha, double beta, double gamma) const
    {
      GeneralizedMoment tmp;
      tmp.set_p( p() );

      for (int l = 0; l <= p() ; ++l)
      {
        for (int m_ = -2*l; m_ <= 2*l; ++m_)
        {
          double sum = 0.0;

          for (int m = -2*l; m <= 2*l; ++m)
            sum += C(2*l,m) * D<CT>(2*l, m_, m, alpha, beta, gamma);

          tmp.values[l][m_+2*l] = sum;
        }
      }

      return tmp;
    }

    GeneralizedMoment get_rotated(point_t new_z) const
    {
      double theta;
      double phi;
      double r;
      to_spherical(new_z, theta, phi, r);

      std::cout << euler_rotation_zyz(0, theta, phi) << std::endl;
      std::cout << euler_rotation_zyz(0, -theta, -phi) << std::endl;

      return get_rotated(0, theta, phi);
    }



    bool z_mirror_symmetry(double tolerance) const
    {
      for (int l = 0; l <= p(); ++l)
      {
        for (int m = -2*l+1; m <= 2*l-1; m+=2)
        {
          if ( std::abs(C(2*l,m)) > tolerance )
            return false;
        }
      }

      return true;
    }

    void rotation_symmetry_angles(double tolerance) const
    {
      for (int l = p(); l >= 0 ; --l)
      {
        for (int m = 2*l; m >= 0; --m)
        {
          if ( (std::abs( C(2*l,m) ) > tolerance) && (std::abs(C(2*l,-m)-C(2*l,m)) > tolerance) )
          {
            double angle = std::abs(2.0 / m * std::atan2( C(2*l,-m) , C(2*l,m) ));
            if (angle < M_PI)
              std::cout << "(" << 2*l << "," << m << ") Found angle: " << angle << "      rot_frequ = " << 2*M_PI/angle << "      " << C(2*l,-m) <<
            "/" << C(2*l,m) << std::endl;
          }
        }
      }
    }


  private:

    void set_p(int p_in)
    {
      values.clear();

      p_ = p_in;
      values.resize(p()+1);
      for (int l = 0; l <= p(); ++l)
        values[l].resize( 4*l+1 );
    }

    GeneralizedMoment() {}

    std::vector< std::vector<CType> > values;

    int p_;
  };

  typedef GeneralizedMoment<double> RealGeneralizedMoment;
  typedef GeneralizedMoment< std::complex<double> > ComplexGeneralizedMoment;

}













#endif
