#ifndef VIENNAMESH_CORE_BASIC_PARAMETERS_HPP
#define VIENNAMESH_CORE_BASIC_PARAMETERS_HPP

#include "viennagrid/config/default_configs.hpp"
#include "viennamesh/core/parameter.hpp"

namespace viennamesh
{
  template<>
  struct static_init<bool>
  {
    typedef bool SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init<bool>::init" << std::endl;

        Converter::get().register_conversion<bool, int>( &static_cast_convert<bool, int> );
        Converter::get().register_conversion<int, bool>( &static_cast_convert<int, bool> );

        TypeProperties::get().set_property<SelfT>( "is_scalar", "true" );
      }
    }
  };

  template<>
  struct static_init<int>
  {
    typedef int SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init<int>::init" << std::endl;

        Converter::get().register_conversion<int, double>( &static_cast_convert<int, double> );
        Converter::get().register_conversion<double, int>( &static_cast_convert<double, int> );

        TypeProperties::get().set_property<SelfT>( "is_scalar", "true" );
      }
    }
  };

  template<>
  struct static_init<double>
  {
    typedef double SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init<double>::init" << std::endl;

        Converter::get().register_conversion<int, double>( &static_cast_convert<int, double> );
        Converter::get().register_conversion<double, int>( &static_cast_convert<double, int> );

        TypeProperties::get().set_property<SelfT>( "is_scalar", "true" );
      }
    }
  };

  template<>
  struct static_init<string>
  {
    typedef string SelfT;

    static void init()
    {
      info(10) << "static_init<string>::init" << std::endl;
    }
  };

  template<typename WrappedMeshConfig, typename WrappedSegmentationConfig>
  struct static_init< MeshWrapper<viennagrid::mesh<WrappedMeshConfig>, viennagrid::segmentation<WrappedSegmentationConfig> > >
  {
    typedef MeshWrapper<viennagrid::mesh<WrappedMeshConfig>, viennagrid::segmentation<WrappedSegmentationConfig> > SelfT;

    static void init()
    {
      TypeProperties::get().set_property<SelfT>( "is_mesh", "true" );
      TypeProperties::get().set_property<SelfT>( "is_viennagrid", "true" );
    }
  };

  template<typename WrappedMeshConfig>
  struct static_init< MeshWrapper<viennagrid::mesh<WrappedMeshConfig>, NoSegmentation > >
  {
    typedef MeshWrapper<viennagrid::mesh<WrappedMeshConfig>, NoSegmentation > SelfT;

    static void init()
    {
      TypeProperties::get().set_property<SelfT>( "is_mesh", "true" );
      TypeProperties::get().set_property<SelfT>( "is_viennagrid", "true" );
    }
  };




  typedef std::vector< std::pair<viennagrid::config::point_type_2d, int> > seed_point_2d_container;

  template<>
  struct static_init<seed_point_2d_container>
  {
    typedef seed_point_2d_container SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init<seed_point_2d_container>::init" << std::endl;
      }
    }
  };

  typedef std::vector< std::pair<viennagrid::config::point_type_3d, int> > seed_point_3d_container;

  template<>
  struct static_init<seed_point_3d_container>
  {
    typedef seed_point_3d_container SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init<seed_point_3d_container>::init" << std::endl;
      }
    }
  };




  typedef std::vector<viennagrid::config::point_type_2d> point_2d_container;

  template<>
  struct static_init<point_2d_container>
  {
    typedef point_2d_container SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init<point_2d_container>::init" << std::endl;
      }
    }
  };

  typedef std::vector<viennagrid::config::point_type_3d> point_3d_container;

  template<>
  struct static_init<point_3d_container>
  {
    typedef point_3d_container SelfT;

    static void init()
    {
      static bool to_init = true;
      if (to_init)
      {
        to_init = false;
        info(10) << "static_init<point_3d_container>::init" << std::endl;
      }
    }
  };



}


#endif
