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

    bool operator==( type_info_wrapper const & rhs ) const
    {
      return *info_ == *rhs.info_;
    }

    bool operator!=( type_info_wrapper const & rhs ) const
    {
      return !(*this == rhs);
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



  namespace result_of
  {
    template<unsigned int dim>
    struct point;

    template<>
    struct point<1>
    {
      typedef viennagrid::config::point_type_1d type;
    };

    template<>
    struct point<2>
    {
      typedef viennagrid::config::point_type_2d type;
    };

    template<>
    struct point<3>
    {
      typedef viennagrid::config::point_type_3d type;
    };




    template<typename PointT>
    struct point_container
    {
      typedef std::vector<PointT> type;
    };

    template<typename PointT>
    struct seed_point_container
    {
      typedef std::vector< std::pair<PointT, int> > type;
    };

  }


  typedef result_of::point<1>::type point_1d;
  typedef result_of::point<2>::type point_2d;
  typedef result_of::point<3>::type point_3d;

  typedef result_of::point_container<point_1d>::type point_1d_container;
  typedef result_of::point_container<point_2d>::type point_2d_container;
  typedef result_of::point_container<point_3d>::type point_3d_container;

  typedef result_of::seed_point_container<point_1d>::type seed_point_1d_container;
  typedef result_of::seed_point_container<point_2d>::type seed_point_2d_container;
  typedef result_of::seed_point_container<point_3d>::type seed_point_3d_container;




  namespace result_of
  {
    template<typename CellTagT, unsigned int GeometricDimensionV>
    struct default_mesh;

    // vertex
    template<>
    struct default_mesh<viennagrid::vertex_tag, 1>
    { typedef viennagrid::vertex_1d_mesh type; };
    template<>
    struct default_mesh<viennagrid::vertex_tag, 2>
    { typedef viennagrid::vertex_2d_mesh type; };
    template<>
    struct default_mesh<viennagrid::vertex_tag, 3>
    { typedef viennagrid::vertex_3d_mesh type; };

    // line
    template<>
    struct default_mesh<viennagrid::line_tag, 1>
    { typedef viennagrid::line_1d_mesh type; };
    template<>
    struct default_mesh<viennagrid::line_tag, 2>
    { typedef viennagrid::line_2d_mesh type; };
    template<>
    struct default_mesh<viennagrid::line_tag, 3>
    { typedef viennagrid::line_3d_mesh type; };

    // triangle
    template<>
    struct default_mesh<viennagrid::triangle_tag, 2>
    { typedef viennagrid::triangular_2d_mesh type; };
    template<>
    struct default_mesh<viennagrid::triangle_tag, 3>
    { typedef viennagrid::triangular_3d_mesh type; };

    // quadrilateral
    template<>
    struct default_mesh<viennagrid::quadrilateral_tag, 2>
    { typedef viennagrid::quadrilateral_2d_mesh type; };
    template<>
    struct default_mesh<viennagrid::quadrilateral_tag, 3>
    { typedef viennagrid::quadrilateral_3d_mesh type; };

    // polygon
    template<>
    struct default_mesh<viennagrid::polygon_tag, 2>
    { typedef viennagrid::polygonal_2d_mesh type; };
    template<>
    struct default_mesh<viennagrid::polygon_tag, 3>
    { typedef viennagrid::polygonal_3d_mesh type; };

    // PLC
    template<>
    struct default_mesh<viennagrid::plc_tag, 3>
    { typedef viennagrid::plc_3d_mesh type; };

    // Tetrahedron
    template<>
    struct default_mesh<viennagrid::tetrahedron_tag, 3>
    { typedef viennagrid::tetrahedral_3d_mesh type; };

    // Hexahedron
    template<>
    struct default_mesh<viennagrid::hexahedron_tag, 3>
    { typedef viennagrid::hexahedral_3d_mesh type; };

  }

}

#endif
