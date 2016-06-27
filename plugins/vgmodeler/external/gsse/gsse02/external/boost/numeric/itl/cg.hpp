
#include <boost/numeric/mtl/mtl.hpp>


template < class LinearOperator, class HilbertSpace, class Iteration >
int
cg(const LinearOperator& A, HilbertSpace& x, const HilbertSpace& b, Iteration& iter)
{
  typedef HilbertSpace TmpVec;
  typedef typename mtl::Collection<HilbertSpace>::value_type Scalar;

  Scalar rho, rho_1, alpha, beta;
  TmpVec p(size(x)), q(size(x)), r(size(x)), z(size(x));
  
  // r = b - A*x;
  r = b;
  r -= A*x;

  while (! iter.finished(r)) {
    rho = dot(r, r);
    
    if (iter.first())
      p = r;
    else {
      beta = rho / rho_1;
      p = r + beta * p;
    }
    
    q = A * p;

    alpha = rho / dot(p, q);
    
    x += alpha * p;
    r -= alpha * q;

    rho_1 = rho;
    
    ++iter;
  }

  return iter.error_code();
}
