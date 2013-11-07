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

}


#endif
