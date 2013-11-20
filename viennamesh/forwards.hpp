#ifndef VIENNAMESH_FORWARDS_HPP
#define VIENNAMESH_FORWARDS_HPP

#include "viennagrid/config/default_configs.hpp"

#if __cplusplus > 199711L
  #include <functional>
  #include <memory>
#else
  #include <boost/function.hpp>
  #include <boost/shared_ptr.hpp>
  #include <boost/enable_shared_from_this.hpp>
#endif

namespace viennamesh
{
#if __cplusplus > 199711L
  using std::shared_ptr;
  using std::function;
  using std::enable_shared_from_this;
  using std::dynamic_pointer_cast;
  using std::static_pointer_cast;
#else
  using boost::shared_ptr;
  using boost::function;
  using boost::enable_shared_from_this;
  using boost::dynamic_pointer_cast;
  using boost::static_pointer_cast;
#endif

  using std::string;


  class type_info_wrapper
  {
  public:
    type_info_wrapper() : info_(0) {}
    type_info_wrapper( std::type_info const & info ) : info_(&info) {}

    bool operator<( type_info_wrapper const & rhs ) const
    {
      if (info_ && rhs.info_)
        return info_->before(*rhs.info_);

      return false;
    }

    string name() const
    {
      return info_->name();
    }

    std::type_info const * get() const
    { return info_; }

    template<typename TypeT>
    static type_info_wrapper make()
    {
      return type_info_wrapper( typeid(TypeT) );
    }

  private:
    std::type_info const * info_;
  };



  typedef viennagrid::config::point_type_1d point_1d;
  typedef viennagrid::config::point_type_2d point_2d;
  typedef viennagrid::config::point_type_3d point_3d;

  typedef std::vector<point_1d> point_1d_container;
  typedef std::vector<point_2d> point_2d_container;
  typedef std::vector<point_3d> point_3d_container;

  typedef std::vector< std::pair<point_1d, int> > seed_point_1d_container;
  typedef std::vector< std::pair<point_2d, int> > seed_point_2d_container;
  typedef std::vector< std::pair<point_3d, int> > seed_point_3d_container;
}

#endif
