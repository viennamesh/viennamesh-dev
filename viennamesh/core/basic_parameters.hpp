#ifndef VIENNAMESH_CORE_BASIC_PARAMETERS_HPP
#define VIENNAMESH_CORE_BASIC_PARAMETERS_HPP

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/segmented_mesh.hpp"
#include "viennamesh/core/parameter.hpp"

namespace viennamesh
{
  template<>
  struct static_init_impl<bool>
  {
    static void init()
    {
      converter::get().register_conversion<bool, int>( &static_cast_convert<bool, int> );
    }
  };

  template<>
  struct static_init_impl<int>
  {
    static void init()
    {
      converter::get().register_conversion<int, double>( &static_cast_convert<int, double> );
      converter::get().register_conversion<int, bool>( &static_cast_convert<int, bool> );
    }
  };

  template<>
  struct static_init_impl<double>
  {
    static void init()
    {
      converter::get().register_conversion<double, int>( &static_cast_convert<double, int> );
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

  template<typename WrappedMeshConfigT, typename WrappedSegmentationConfigT>
  struct static_init_impl< viennagrid::segmented_mesh<viennagrid::mesh<WrappedMeshConfigT>, viennagrid::segmentation<WrappedSegmentationConfigT> > >
  {
    typedef viennagrid::segmented_mesh<viennagrid::mesh<WrappedMeshConfigT>, viennagrid::segmentation<WrappedSegmentationConfigT> > SelfT;

    static void init()
    {
      type_properties::get().set_property<SelfT>( "is_mesh", "true" );
      type_properties::get().set_property<SelfT>( "is_viennagrid", "true" );
    }
  };

  template<typename WrappedMeshConfigT>
  struct static_init_impl< viennagrid::mesh<WrappedMeshConfigT> >
  {
    typedef viennagrid::mesh<WrappedMeshConfigT> SelfT;

    static void init()
    {
      type_properties::get().set_property<SelfT>( "is_mesh", "true" );
      type_properties::get().set_property<SelfT>( "is_viennagrid", "true" );
    }
  };


}


#endif
