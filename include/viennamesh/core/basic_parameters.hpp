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
  struct type_information<std::string>
  {
    typedef std::string SelfT;

    static void init() {}

    static std::string name()
    {
      return "string";
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
  struct type_information< std::map<std::string, std::string> >
  {
    static bool string_to_point_conversion( std::string const & input, std::map<std::string, std::string> & output )
    {
      output.clear();

      std::list<std::string> split_mappings = stringtools::split_string( input, ";" );
      for (std::list<std::string>::const_iterator mit = split_mappings.begin(); mit != split_mappings.end(); ++mit)
      {
        std::list<std::string> from_to = stringtools::split_string( *mit, "," );

        if (from_to.size() != 2)
          return false;

        std::list<std::string>::const_iterator it = from_to.begin();

        std::string src = *it++;
        std::string dst = *it;

        output[src] = dst;
      }

      return true;
    }

    static void init()
    {
      converter::get().register_conversion<std::string, std::map<std::string, std::string> >( &string_to_point_conversion );
    }

    static std::string name()
    {
      return "std::map<std::string, std::string>";
    }
  };


  template<unsigned int dim>
  bool string_to_point_container( std::string const & input, typename viennamesh::result_of::point_container< typename viennamesh::result_of::point<dim>::type >::type & output )
  {
    output.clear();
    typedef typename viennamesh::result_of::point<dim>::type PointType;
    std::list<std::string> split_mappings = stringtools::split_string_brackets( input, "," );
    for (std::list<std::string>::const_iterator mit = split_mappings.begin(); mit != split_mappings.end(); ++mit)
    {
      std::vector<double> tmp = stringtools::vector_from_string<double>(*mit);
      PointType point;

      if (tmp.size() != point.size())
      {
        error(1) << "String to point container conversion: dimension missmatch" << std::endl;
        return false;
      }

      std::copy( tmp.begin(), tmp.end(), point.begin() );
      output.push_back(point);
    }

    return true;
  }


  template<>
  struct type_information<point_1d_container>
  {
    static bool string_to_point_conversion( std::string const & input, point_1d_container & output )
    {
      return string_to_point_container<1>(input, output);
    }

    static void init()
    {
      converter::get().register_conversion<std::string, point_1d_container>( &string_to_point_conversion );
    }

    static std::string name()
    {
      return "point_1d_container";
    }
  };

  template<>
  struct type_information<point_2d_container>
  {
    static bool string_to_point_conversion( std::string const & input, point_2d_container & output )
    {
      return string_to_point_container<2>(input, output);
    }

    static void init()
    {
      converter::get().register_conversion<std::string, point_2d_container>( &string_to_point_conversion );
    }

    static std::string name()
    {
      return "point_2d_container";
    }
  };

  template<>
  struct type_information<point_3d_container>
  {
    static bool string_to_point_conversion( std::string const & input, point_3d_container & output )
    {
      return string_to_point_container<3>(input, output);
    }

    static void init()
    {
      converter::get().register_conversion<std::string, point_3d_container>( &string_to_point_conversion );
    }

    static std::string name()
    {
      return "point_3d_container";
    }
  };


  template<unsigned int dim>
  bool string_to_seed_point_container( std::string const & input, typename viennamesh::result_of::seed_point_container< typename viennamesh::result_of::point<dim>::type >::type & output )
  {
    output.clear();
    typedef typename viennamesh::result_of::point<dim>::type PointType;
    std::list<std::string> split_mappings = stringtools::split_string_brackets( input, ";" );
    for (std::list<std::string>::const_iterator mit = split_mappings.begin(); mit != split_mappings.end(); ++mit)
    {
//       std::cout << "Seed Point string: " << *mit << std::endl;
      std::list<std::string> from_to = stringtools::split_string_brackets( *mit, "," );

      if (from_to.size() != 2)
      {
        error(1) << "String to seed point container conversion: an entry has no point and no segment id: " << *mit << std::endl;
        return false;
      }

      std::list<std::string>::const_iterator it = from_to.begin();

      std::string point_string = *it;
      ++it;
      std::string segment_id = *it;

//       std::cout << "Point string: " << point_string << std::endl;
      std::vector<double> tmp = stringtools::vector_from_string<double>(point_string);
      PointType point;

      if (tmp.size() != point.size())
      {
        error(1) << "Seed point to string conversion: dimension missmatch" << std::endl;
        return false;
      }

      std::copy( tmp.begin(), tmp.end(), point.begin() );
      output.push_back( std::make_pair(point, lexical_cast<int>(segment_id)) );
    }

    return true;
  }

  template<>
  struct type_information<seed_point_1d_container>
  {
    static bool string_to_seed_point_conversion( std::string const & input, seed_point_1d_container & output )
    {
      return string_to_seed_point_container<1>(input, output);
    }

    static void init()
    {
      converter::get().register_conversion<std::string, seed_point_1d_container>( &string_to_seed_point_conversion );
    }

    static std::string name()
    {
      return "seed_point_1d_container";
    }
  };

  template<>
  struct type_information<seed_point_2d_container>
  {
    static bool string_to_seed_point_conversion( std::string const & input, seed_point_2d_container & output )
    {
      return string_to_seed_point_container<2>(input, output);
    }

    static void init()
    {
//       std::cout << "AFFDASEFDDDDDEFEFAEAFE" << std::endl;
      converter::get().register_conversion<std::string, seed_point_2d_container>( &string_to_seed_point_conversion );
    }

    static std::string name()
    {
      return "seed_point_2d_container";
    }
  };

  template<>
  struct type_information<seed_point_3d_container>
  {
    static bool string_to_seed_point_conversion( std::string const & input, seed_point_3d_container & output )
    {
      return string_to_seed_point_container<3>(input, output);
    }

    static void init()
    {
      converter::get().register_conversion<std::string, seed_point_3d_container>( &string_to_seed_point_conversion );
    }

    static std::string name()
    {
      return "seed_point_3d_container";
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
