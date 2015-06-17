#include <boost/numeric/mtl/mtl.hpp>

template <class Matrix, class Vector, class VectorB, class Iteration>
int
bicgstab(const Matrix& A, Vector& x, const VectorB& b, Iteration& iter)
{
  typedef typename mtl::Collection<Vector>::value_type T;
  T rho_1, rho_2, alpha, beta, omega;
  typedef Vector TmpVec;
  TmpVec p(size(x)), phat(size(x)), s(size(x)), shat(size(x)), 
    t(size(x)), v(size(x)), r(size(x)), rtilde(size(x));

  // r = b - A*x;
  r = b;
  r -= A*x;
  rtilde = r;

  while (! iter.finished(r)) {
    
    rho_1 = dot(rtilde, r);
    if (rho_1 == T(0.)) {
      iter.fail(2, "bicg breakdown #1");
      break;
    }
    
    if (iter.first())
      p = r;
    else {
      if (omega == T(0.)) {
	iter.fail(3, "bicg breakdown #2");
	break;
      }
      
      beta = (rho_1 / rho_2) * (alpha / omega);
      p = r + beta * (p - omega * v);
    }
    phat = p;
    v = A * phat;

    alpha = rho_1 / dot(v, rtilde);
    s = r - alpha * v;
    
    if (iter.finished(s)) {
      x += alpha * phat;
      break;
    }
    
    shat = s;
    t = A * shat;
    omega = dot(t, s) / dot(t, t);
    
    x += omega * shat + alpha * phat;
    r = s - omega * t;
    
    rho_2 = rho_1;
    
    ++iter;
  }
  
  return iter.error_code();
}




