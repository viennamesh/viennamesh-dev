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

#include "viennamesh/core/algorithm_pipeline.hpp"
#include "viennamesh/core/algorithm_factory.hpp"

namespace viennamesh
{
  bool algorithm_pipeline::add_algorithm( pugi::xml_node const & algorithm_node )
  {
    pugi::xml_attribute algorithm_name_attribute = algorithm_node.attribute("name");
    if ( algorithm_name_attribute.empty() )
    {
      error(1) << "Algorithm has no name attribute" << std::endl;
      return false;
    }
    string algorithm_name = algorithm_name_attribute.as_string();

    pugi::xml_attribute algorithm_id_attribute = algorithm_node.attribute("id");
    if ( algorithm_id_attribute.empty() )
    {
      error(1) << "Algorithm \"" << algorithm_name << "\" has no id attribute" << std::endl;
      return false;
    }
    string algorithm_id = algorithm_id_attribute.as_string();

    algorithm_handle algorithm = viennamesh::algorithm_factory().create_by_id( algorithm_id );

    if (!algorithm)
    {
      error(1) << "Algorithm with id \"" << algorithm_id << "\" creation from factory failed" << std::endl;
      return false;
    }

    pugi::xml_node default_source_algorithm_node = algorithm_node.child("default_source");
    if (default_source_algorithm_node)
    {
      string default_source_algorithm = default_source_algorithm_node.text().as_string();

      std::map<string, algorithm_handle>::iterator ait = algorithms.find( default_source_algorithm );
      if (ait == algorithms.end())
      {
        error(1) << "Default source algorithm \"" << default_source_algorithm << "\" was not found" << std::endl;
        return false;
      }

      algorithm->set_default_source( ait->second );
    }


    for (pugi::xml_node paramater_node = algorithm_node.child("parameter");
          paramater_node;
          paramater_node = paramater_node.next_sibling("parameter"))
    {
      pugi::xml_attribute parameter_name_attribute = paramater_node.attribute("name");
      if (parameter_name_attribute.empty())
      {
        error(1) << "Parameter has no name attribute" << std::endl;
        return false;
      }
      string parameter_name = parameter_name_attribute.as_string();

      pugi::xml_attribute parameter_type_attribute = paramater_node.attribute("type");
      if (parameter_type_attribute.empty())
      {
        error(1) << "Parameter \"" << parameter_name << "\" has no type attribute" << std::endl;
        return false;
      }
      string parameter_type = parameter_type_attribute.as_string();


      string parameter_value = paramater_node.text().as_string();


      if (parameter_type == "xml")
      {
        size_t number_of_children = std::distance(paramater_node.children().begin(), paramater_node.children().end());

        if ( number_of_children == 0 )
        {
          error(1) << "Parameter \"" << parameter_name << "\" is type xml but has no children." << std::endl;
          return false;
        }

        if ( number_of_children != 1 )
          warning(1) << "Parameter \"" << parameter_name << "\" is type xml but has more than one children" << std::endl;

        algorithm->set_input( parameter_name, paramater_node.first_child() );
      }
      else
      {
        if (parameter_value.empty())
        {
          error(1) << "Parameter \"" << parameter_name << "\" has no value" << std::endl;
          return false;
        }

        if (parameter_type == "string")
        {
          algorithm->set_input( parameter_name, parameter_value );
        }
        else if (parameter_type == "bool")
        {
          algorithm->set_input( parameter_name, lexical_cast<bool>(parameter_value) );
        }
        else if (parameter_type == "int")
        {
          algorithm->set_input( parameter_name, lexical_cast<int>(parameter_value) );
        }
        else if (parameter_type == "double")
        {
          algorithm->set_input( parameter_name, lexical_cast<double>(parameter_value) );
        }
        else if (parameter_type == "point")
        {
          algorithm->set_input( parameter_name, stringtools::vector_from_string<double>(parameter_value) );
        }
        else if (parameter_type == "dynamic")
        {
          string source_algorithm_name = parameter_value.substr( 0, parameter_value.find("/") );
          string source_parameter_name = parameter_value.substr( parameter_value.find("/")+1 );

          std::map<string, algorithm_handle>::iterator ait = algorithms.find( source_algorithm_name );
          if (ait == algorithms.end())
          {
            error(1) << "Dynamic parameter \"" << parameter_name << "\": algorithm \"" << source_algorithm_name << "\" was not found" << std::endl;
            return false;
          }

          algorithm->link_input( parameter_name, ait->second, source_parameter_name );
        }
        else
        {
          error(1) << "Parameter \"" << parameter_name << "\": type \"" << parameter_type << "\" is not supported" << std::endl;
          return false;
        }
      }

    }


    std::pair<AlgorithmMapType::iterator, bool> result =
      algorithms.insert( std::make_pair(algorithm_name, algorithm) );

    if (!result.second)
    {
      error(1) << "Duplicated algorithm with \"" << algorithm_name << "\" and id \"" << algorithm_id << "\n" << std::endl;
      return false;
    }

    algorithm_order.push_back( result.first );

    return true;
  }


  bool algorithm_pipeline::from_xml( pugi::xml_node const & xml )
  {
    for (pugi::xml_node algorithm_node = xml.child("algorithm");
          algorithm_node;
          algorithm_node = algorithm_node.next_sibling("algorithm"))
    {
      if (!add_algorithm( algorithm_node ))
      {
        clear();
        return false;
      }
    }

    return true;
  }

  bool algorithm_pipeline::run()
  {
    for (std::vector<AlgorithmMapType::iterator>::iterator ait = algorithm_order.begin(); ait != algorithm_order.end(); ++ait)
    {
      if (!(*ait)->second->run())
        return false;
    }

    return true;
  }

  void algorithm_pipeline::clear()
  {
    algorithms.clear();
  }


  void algorithm_pipeline::set_base_path( std::string const & path )
  {
    for (AlgorithmMapType::iterator ait = algorithms.begin(); ait != algorithms.end(); ++ait)
      (*ait).second->base_path() = path;
  }


}
