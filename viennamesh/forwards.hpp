#ifndef VIENNAMESH_FORWARDS_HPP
#define VIENNAMESH_FORWARDS_HPP

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

    template<typename TypeT>
    static type_info_wrapper get()
    {
      return type_info_wrapper( typeid(TypeT) );
    }

  private:
    std::type_info const * info_;
  };

  struct NoSegmentation {};
}

#endif
