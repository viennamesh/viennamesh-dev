#ifndef VIENNAMESH_CORE_BASIC_PARAMETERS_HPP
#define VIENNAMESH_CORE_BASIC_PARAMETERS_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include <sstream>

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/segmented_mesh.hpp"

#include "viennamesh/core/parameter.hpp"
#include "viennamesh/core/mesh_quantities.hpp"

namespace viennamesh
{
  template<>
  struct type_information<bool>
  {
    static void init()
    {
//       converter::get().register_conversion<bool, int>( &static_cast_convert<bool, int> );
    }

    static std::string name()
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

    static std::string name()
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

    static std::string name()
    {
      return "double";
    }
  };



  template<>
  struct type_information<dynamic_point>
  {
    static bool string_to_point_conversion( std::string const & input, dynamic_point & output )
    {
      output = dynamic_point_from_string(input);
      return true;
    }

    static void init()
    {
      converter::get().register_conversion<std::string, dynamic_point>( &string_to_point_conversion );
    }

    static std::string name()
    {
      return "dynamic_point";
    }
  };

  template<>
  struct type_information<std::string>
  {
    typedef std::string SelfT;

    static void init() {}

    static std::string name()
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
      type_properties::get().set_property<SelfType>( "geometric_dimension", lexical_cast<std::string>(geometric_dimension) );
      type_properties::get().set_property<SelfType>( "cell_type", CellTag::name() );
    }

    static std::string name()
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
      type_properties::get().set_property<SelfType>( "geometric_dimension", lexical_cast<std::string>(geometric_dimension) );
      type_properties::get().set_property<SelfType>( "cell_type", CellTag::name() );
    }

    static std::string name()
    {
      std::stringstream ss;
      ss << "viennagrid::mesh< cell type = " << CellTag::name() << ", geometric dimension = " << viennagrid::result_of::geometric_dimension<MeshType>::value << ">";

      return ss.str();
    }
  };



  template<typename SegmentIDT, typename VertexKeyT, typename CellKeyT, typename ValueT>
  struct type_information< segmented_mesh_quantities<SegmentIDT, VertexKeyT, CellKeyT, ValueT> >
  {
    typedef segmented_mesh_quantities<SegmentIDT, VertexKeyT, CellKeyT, ValueT> SelfType;

    static void init()
    {
      type_properties::get().set_property<SelfType>( "is_mesh", "false" );
    }

    static std::string name()
    {
      std::stringstream ss;
      ss << "segmented_mesh_quantities" << std::endl;
      return ss.str();
    }
  };



  template<typename KeyT, typename ValueT>
  struct type_information< mesh_quantities<KeyT, ValueT> >
  {
    typedef mesh_quantities<KeyT, ValueT> SelfType;

    static void init()
    {
      type_properties::get().set_property<SelfType>( "is_mesh", "false" );
    }

    static std::string name()
    {
      std::stringstream ss;
      ss << "mesh_quantities" << std::endl;
      return ss.str();
    }
  };


}


#endif
