#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_GENERALIZED_MOMENT_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_GENERALIZED_MOMENT_HPP

#include "common.hpp"
#include "integrate.hpp"

namespace viennamesh
{
  template<typename T>
  T C_to_integrate(int two_l, int m, int two_p, point_t const & s)
  {
    assert(two_l%2 == 0);
    assert(two_p%2 == 0);
    return std::pow(viennagrid::norm_2(s), two_p) * D<T>(two_l, 0, m, s);
  }


  double S(int p, int l)
  {
    double sum = 0.0;
    for (int k = l; k <= 2*l; ++k)
    {
      sum += power_mone(k) * static_cast<double>(std::pow(2.0, 2*p+1) * factorial(p) * factorial(2*k) * factorial(p+k-l)) /
              static_cast<double>( factorial(2*(p+k-l)+1) * factorial(k-l) * factorial(k) * factorial(2*l-k) );
    }

    return sum * std::sqrt( (4*l+1)*M_PI ) / static_cast<double>(std::pow(2.0, 2*l));
  }



  template<typename T, bool mesh_is_const>
  T C(int two_l, int m, int two_p,
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

    T result = 0.0;
    int l = two_l/2;
    int p = two_p/2;


    std::function< T (point_t const &) > cf =
        std::bind< T >(C_to_integrate<T>, two_l, m, two_p, std::placeholders::_1);


    ConstCellRange cells( mesh );
    for (ConstCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      result += integrate( *cit, cf, relative_integrate_tolerance, absolute_integrate_tolerance, max_integrate_iterations );
    }

    return result * S(p,l);
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
          values[l][m+2*l] = viennamesh::C<CT>(2*l, m, 2*p(), mesh,
                                               relative_integrate_tolerance,
                                               absolute_integrate_tolerance,
                                               max_integrate_iterations);

    }

    double operator()(double theta, double phi) const
    {
      CType sum = 0.0;
      for (int l = 0; l <= p(); ++l)
      {
        for (int m = -2*l; m <= 2*l; ++m)
        {
          sum += this->C(2*l, m) * SphericalHarmonic<CT>(2*l,m)(theta, phi);
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
