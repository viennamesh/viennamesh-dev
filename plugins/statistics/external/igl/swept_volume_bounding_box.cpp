#include "swept_volume_bounding_box.h"

IGL_INLINE void igl::swept_volume_bounding_box(
  const size_t & n,
  const std::function<Eigen::RowVector3d(const size_t vi, const double t)> & V,
  const size_t & steps,
  Eigen::AlignedBox3d & box)
{
  using namespace Eigen;
  box.setEmpty();
  const VectorXd t = VectorXd::LinSpaced(steps,0,1);
  // Find extent over all time steps
  for(int ti = 0;ti<t.size();ti++)
  {
    for(size_t vi = 0;vi<n;vi++)
    {
      box.extend(V(vi,t(ti)).transpose());
    }
  }
}
