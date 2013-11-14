#ifndef VIENNAMESH_CORE_BASIC_PARAMETERS_HPP
#define VIENNAMESH_CORE_BASIC_PARAMETERS_HPP

#include "viennagrid/config/default_configs.hpp"
#include "viennamesh/core/parameter.hpp"

namespace viennamesh
{
  template<>
  struct static_init_impl<bool>
  {
    static void init()
    {
      Converter::get().register_conversion<bool, int>( &static_cast_convert<bool, int> );
    }
  };

  template<>
  struct static_init_impl<int>
  {
    static void init()
    {
      Converter::get().register_conversion<int, double>( &static_cast_convert<int, double> );
      Converter::get().register_conversion<int, bool>( &static_cast_convert<int, bool> );
    }
  };

  template<>
  struct static_init_impl<double>
  {
    static void init()
    {
      Converter::get().register_conversion<double, int>( &static_cast_convert<double, int> );
    }
  };


  template<>
  struct static_init_impl<string>
  {
    typedef string SelfT;

    static void init()
    {
      info(10) << "static_init<string>::init" << std::endl;
    }
  };

  template<typename WrappedMeshConfig, typename WrappedSegmentationConfig>
  struct static_init_impl< SegmentedMesh<viennagrid::mesh<WrappedMeshConfig>, viennagrid::segmentation<WrappedSegmentationConfig> > >
  {
    typedef SegmentedMesh<viennagrid::mesh<WrappedMeshConfig>, viennagrid::segmentation<WrappedSegmentationConfig> > SelfT;

    static void init()
    {
      TypeProperties::get().set_property<SelfT>( "is_mesh", "true" );
      TypeProperties::get().set_property<SelfT>( "is_viennagrid", "true" );
    }
  };

  template<typename WrappedMeshConfig>
  struct static_init_impl< viennagrid::mesh<WrappedMeshConfig> >
  {
    typedef viennagrid::mesh<WrappedMeshConfig> SelfT;

    static void init()
    {
      TypeProperties::get().set_property<SelfT>( "is_mesh", "true" );
      TypeProperties::get().set_property<SelfT>( "is_viennagrid", "true" );
    }
  };


}


#endif
