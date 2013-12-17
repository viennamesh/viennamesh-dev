#ifndef VIENNAMESH_CORE_BASIC_PARAMETERS_HPP
#define VIENNAMESH_CORE_BASIC_PARAMETERS_HPP

#include <sstream>

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/segmented_mesh.hpp"
#include "viennamesh/core/parameter.hpp"

namespace viennamesh
{
  template<>
  struct type_information<bool>
  {
    static void init()
    {
//       converter::get().register_conversion<bool, int>( &static_cast_convert<bool, int> );
    }

    static string name()
    {
      return "bool";
    }
  };

  template<>
  struct type_information<int>
  {
    static void init()
    {
      converter::get().register_conversion<int, double>( &static_cast_convert<int, double> );
//       converter::get().register_conversion<int, bool>( &static_cast_convert<int, bool> );
    }

    static string name()
    {
      return "int";
    }
  };

  template<>
  struct type_information<double>
  {
    static void init()
    {
      converter::get().register_conversion<double, int>( &static_cast_convert<double, int> );
    }

    static string name()
    {
      return "double";
    }
  };


  template<>
  struct type_information<string>
  {
    typedef string SelfT;

    static void init() {}

    static string name()
    {
      return "string";
    }
  };

  template<typename WrappedMeshConfigT, typename WrappedSegmentationConfigT>
  struct type_information< viennagrid::segmented_mesh<viennagrid::mesh<WrappedMeshConfigT>, viennagrid::segmentation<WrappedSegmentationConfigT> > >
  {
    typedef viennagrid::mesh<WrappedMeshConfigT> MeshType;
    typedef viennagrid::segmentation<WrappedSegmentationConfigT> SegmentationType;
    typedef typename viennagrid::result_of::cell_tag<MeshType>::type CellTag;

    typedef viennagrid::segmented_mesh<MeshType, SegmentationType> SelfType;

    static void init()
    {
      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "is_viennagrid", "true" );
      type_properties::get().set_property<SelfType>( "is_segmented", "true" );

      int geometric_dimension = viennagrid::result_of::geometric_dimension<MeshType>::value;
      type_properties::get().set_property<SelfType>( "geometric_dimension", boost::lexical_cast<string>(geometric_dimension) );
      type_properties::get().set_property<SelfType>( "cell_type", CellTag::name() );

      info(1) << "init: cell_type = " << CellTag::name() << std::endl;
    }

    static string name()
    {
      std::stringstream ss;
      ss << "viennagrid::segmented_mesh< cell type = " << CellTag::name() << ", geometric dimension = " << viennagrid::result_of::geometric_dimension<MeshType>::value << ">";

      return ss.str();
    }
  };

  template<typename WrappedMeshConfigT>
  struct type_information< viennagrid::mesh<WrappedMeshConfigT> >
  {
    typedef viennagrid::mesh<WrappedMeshConfigT> MeshType;
    typedef typename viennagrid::result_of::cell_tag<MeshType>::type CellTag;

    typedef MeshType SelfType;

    static void init()
    {
      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "is_viennagrid", "true" );
      type_properties::get().set_property<SelfType>( "is_segmented", "false" );

      int geometric_dimension = viennagrid::result_of::geometric_dimension<MeshType>::value;
      type_properties::get().set_property<SelfType>( "geometric_dimension", boost::lexical_cast<string>(geometric_dimension) );
      type_properties::get().set_property<SelfType>( "cell_type", CellTag::name() );

      info(1) << "init: cell_type = " << CellTag::name() << std::endl;
    }

    static string name()
    {
      std::stringstream ss;
      ss << "viennagrid::mesh< cell type = " << CellTag::name() << ", geometric dimension = " << viennagrid::result_of::geometric_dimension<MeshType>::value << ">";

      return ss.str();
    }
  };


}


#endif
