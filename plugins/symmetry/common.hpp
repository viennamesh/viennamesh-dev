#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_COMMON_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_COMMON_HPP


#include <boost/numeric/ublas/matrix.hpp>
namespace ublas = boost::numeric::ublas;


namespace boost
{
  namespace numeric
  {
    namespace ublas
    {
      viennagrid::point_t prod(ublas::matrix<double> const & mat, viennagrid::point_t const & vec)
      {
        assert(mat.size2() == vec.size());
        viennagrid::point_t result( mat.size1() );
        for (std::size_t row = 0; row != mat.size2(); ++row)
        {
          result[row] = 0.0;
          for (std::size_t col = 0; col != mat.size1(); ++col)
            result[row] += mat(row, col) * vec[col];
        }
        return result;
      }
    }
  }
}


namespace viennamesh
{
  void from_spherical(double theta, double phi, double r, point_t & result)
  {
    result.resize(3);
    result[0] = r * std::sin(theta) * std::cos(phi);
    result[1] = r * std::sin(theta) * std::sin(phi);
    result[2] = r * std::cos(theta);
  }

  void to_spherical(point_t const & p, double & theta, double & phi, double & r)
  {
    r = std::sqrt( p[0]*p[0] + p[1]*p[1] + p[2]*p[2] );
    theta = std::acos( p[2] / r );
    phi = atan2(p[1],p[0]);
  }

  ublas::matrix<double> rotation_x(double alpha)
  {
    ublas::matrix<double> R = ublas::identity_matrix<double>(3);
    R(1,1) = std::cos(alpha);
    R(1,2) = -std::sin(alpha);
    R(2,1) = std::sin(alpha);
    R(2,2) = std::cos(alpha);
    return R;
  }

  ublas::matrix<double> rotation_y(double alpha)
  {
    ublas::matrix<double> R = ublas::identity_matrix<double>(3);
    R(0,0) = std::cos(alpha);
    R(0,2) = std::sin(alpha);
    R(2,0) = -std::sin(alpha);
    R(2,2) = std::cos(alpha);
    return R;
  }

  ublas::matrix<double> rotation_z(double alpha)
  {
    ublas::matrix<double> R = ublas::identity_matrix<double>(3);
    R(0,0) = std::cos(alpha);
    R(0,1) = -std::sin(alpha);
    R(1,0) = std::sin(alpha);
    R(1,1) = std::cos(alpha);
    return R;
  }

  ublas::matrix<double> euler_rotation_zyz(double alpha, double beta, double gamma)
  {
    ublas::matrix<double> tmp = ublas::prod( rotation_y(beta), rotation_z(gamma) );
    return ublas::prod( rotation_z(alpha), tmp );
  }


  // http://de.wikipedia.org/wiki/Drehmatrix#Drehmatrizen_des_Raumes_R.C2.B3
  ublas::matrix<double> rotation(point_t axis, double alpha)
  {
    axis /= viennagrid::norm_2(axis);

    double n1 = axis[0];
    double n2 = axis[1];
    double n3 = axis[2];

    double cos_a = std::cos(alpha);
    double sin_a = std::sin(alpha);

    ublas::matrix<double> R = ublas::zero_matrix<double>(3);

    R(0,0) = n1*n1*(1-cos_a) + cos_a;
    R(0,1) = n1*n2*(1-cos_a) - n3*sin_a;
    R(0,2) = n1*n3*(1-cos_a) + n2*sin_a;

    R(1,0) = n2*n1*(1-cos_a) + n3*sin_a;
    R(1,1) = n2*n2*(1-cos_a) + cos_a;
    R(1,2) = n2*n3*(1-cos_a) - n1*sin_a;

    R(2,0) = n3*n1*(1-cos_a) - n2*sin_a;
    R(2,1) = n3*n2*(1-cos_a) + n1*sin_a;
    R(2,2) = n3*n3*(1-cos_a) + cos_a;

    return R;
  }

  std::pair<double, double> prod(ublas::matrix<double> const & rot, double theta, double phi)
  {
    point_t pt;
    from_spherical(theta, phi, 1.0, pt);
    pt = ublas::prod(rot, pt);

    std::pair<double, double> result;
    double tmp;
    to_spherical( pt, result.first, result.second, tmp );
    return result;
  }
}

#endif
