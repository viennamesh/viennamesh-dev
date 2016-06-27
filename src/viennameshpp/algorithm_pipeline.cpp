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

#include <list>
#include <boost/config/posix_features.hpp>
#include "viennameshpp/algorithm_pipeline.hpp"

namespace viennamesh
{






  void algorithm_pipeline_element::change_log_levels()
  {
    if (info_log_level >= 0)
    {
      int tmp;
      viennamesh_log_get_info_level(&tmp);
      viennamesh_log_set_info_level(info_log_level);
      info_log_level = tmp;
    }

    if (error_log_level >= 0)
    {
      int tmp;
      viennamesh_log_get_error_level(&tmp);
      viennamesh_log_set_error_level(error_log_level);
      error_log_level = tmp;
    }

    if (warning_log_level >= 0)
    {
      int tmp;
      viennamesh_log_get_warning_level(&tmp);
      viennamesh_log_set_warning_level(warning_log_level);
      warning_log_level = tmp;
    }

    if (debug_log_level >= 0)
    {
      int tmp;
      viennamesh_log_get_debug_level(&tmp);
      viennamesh_log_set_debug_level(debug_log_level);
      debug_log_level = tmp;
    }

    if (stack_log_level >= 0)
    {
      int tmp;
      viennamesh_log_get_stack_level(&tmp);
      viennamesh_log_set_stack_level(stack_log_level);
      stack_log_level = tmp;
    }
  }



  std::list<std::string> split_string_brackets( std::string const & str, std::string const & delimiter )
  {
    std::list<std::string> tokens;

    std::vector<int> bracket_count(str.size());

    std::string::size_type pos = 0;
    int bc = 0;
    for (pos = 0; pos != str.size(); ++pos)
    {
      if (str[pos] == '(')
        ++bc;

      bracket_count[pos] = bc;

      if (str[pos] == ')')
        --bc;
    }

    pos = 0;
    std::string::size_type old_pos = 0;
    while (pos < str.size())
    {
      std::string::size_type new_pos = str.find(delimiter, pos);
      if (new_pos == std::string::npos)
      {
        tokens.push_back( str.substr(old_pos, str.size()-old_pos) );
        return tokens;
      }

      if (bracket_count[new_pos] != 0)
      {
        pos = new_pos+delimiter.size();
        continue;
      }


      tokens.push_back( str.substr(old_pos, new_pos-old_pos) );
      pos = new_pos+delimiter.size();
      old_pos = pos;
    }

    if (pos == str.size())
      return tokens;

    std::cout << "something went wrong..." << std::endl;

    return std::list<std::string>();
  }




  bool algorithm_pipeline::add_algorithm( pugi::xml_node const & algorithm_node )
  {
    pugi::xml_attribute algorithm_name_attribute = algorithm_node.attribute("name");

    std::string algorithm_name;
    if ( !algorithm_name_attribute.empty() )
      algorithm_name = algorithm_name_attribute.as_string();

    pugi::xml_attribute algorithm_type_attribute = algorithm_node.attribute("type");
    if ( algorithm_type_attribute.empty() )
    {
      error(1) << "Algorithm \"" << algorithm_name << "\" has no type attribute" << std::endl;
      return false;
    }
    std::string algorithm_type = algorithm_type_attribute.as_string();

    algorithm_pipeline_element pipeline_element(algorithm_name);
    try
    {
      pipeline_element.algorithm = context.make_algorithm( algorithm_type );
    }
    catch (viennamesh::exception const & ex)
    {
      error(1) << "Algorithm with id \"" << algorithm_type << "\" creation from factory failed" << std::endl;
      return false;
    }

    {
      pugi::xml_attribute algorithm_info_log_level_attribute = algorithm_node.attribute("info_log_level");
      if ( !algorithm_info_log_level_attribute.empty() )
        pipeline_element.info_log_level = boost::lexical_cast<int>( algorithm_info_log_level_attribute.as_string() );
    }

    {
      pugi::xml_attribute algorithm_error_log_level_attribute = algorithm_node.attribute("error_log_level");
      if ( !algorithm_error_log_level_attribute.empty() )
        pipeline_element.error_log_level = boost::lexical_cast<int>( algorithm_error_log_level_attribute.as_string() );
    }

    {
      pugi::xml_attribute algorithm_warning_log_level_attribute = algorithm_node.attribute("warning_log_level");
      if ( !algorithm_warning_log_level_attribute.empty() )
        pipeline_element.warning_log_level = boost::lexical_cast<int>( algorithm_warning_log_level_attribute.as_string() );
    }

    {
      pugi::xml_attribute algorithm_debug_log_level_attribute = algorithm_node.attribute("warning_debug_level");
      if ( !algorithm_debug_log_level_attribute.empty() )
        pipeline_element.debug_log_level = boost::lexical_cast<int>( algorithm_debug_log_level_attribute.as_string() );
    }

    {
      pugi::xml_attribute algorithm_stack_log_level_attribute = algorithm_node.attribute("warning_stack_level");
      if ( !algorithm_stack_log_level_attribute.empty() )
        pipeline_element.stack_log_level = boost::lexical_cast<int>( algorithm_stack_log_level_attribute.as_string() );
    }




    algorithm_handle & algorithm = pipeline_element.algorithm;
    if (!algorithm.valid())
    {
      error(1) << "Algorithm with id \"" << algorithm_type << "\" creation from factory failed" << std::endl;
      return false;
    }


    pugi::xml_node default_source_algorithm_node = algorithm_node.child("default_source");
    if (default_source_algorithm_node)
    {
      std::string default_source_algorithm_name = default_source_algorithm_node.text().as_string();

      algorithm_pipeline_element * default_source_element = get_element( default_source_algorithm_name );
      if (!default_source_element)
        return false;

      algorithm.set_default_source( default_source_element->algorithm );
      ++(default_source_element->reference_count);
      pipeline_element.referenced_elements.push_back( default_source_element );
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
      std::string parameter_name = parameter_name_attribute.as_string();

      pugi::xml_attribute parameter_type_attribute = paramater_node.attribute("type");
      if (parameter_type_attribute.empty())
      {
        error(1) << "Parameter \"" << parameter_name << "\" has no type attribute" << std::endl;
        return false;
      }

      std::string parameter_type = parameter_type_attribute.as_string();
      std::string parameter_value = paramater_node.text().as_string();


      if (parameter_type == "xml")
      {
        std::stringstream ss;

        for (pugi::xml_node child = paramater_node.first_child(); child; child = child.next_sibling())
          child.print(ss);

        algorithm.set_input( parameter_name, ss.str() );
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
          algorithm.push_back_input( parameter_name, parameter_value );
        }
        else if (parameter_type == "bool")
        {
          algorithm.push_back_input( parameter_name, boost::lexical_cast<bool>(parameter_value) );
        }
        else if (parameter_type == "int")
        {
          algorithm.push_back_input( parameter_name, boost::lexical_cast<int>(parameter_value) );
        }
        else if (parameter_type == "double")
        {
          algorithm.push_back_input( parameter_name, boost::lexical_cast<double>(parameter_value) );
        }
        else if (parameter_type == "point")
        {
          algorithm.push_back_input( parameter_name, boost::lexical_cast<point>(parameter_value) );
        }
        else if ((parameter_type == "points") || (parameter_type == "matrix"))
        {
          std::list<std::string> split_mappings = split_string_brackets( parameter_value, "," );
          point_container points;
          for (std::list<std::string>::const_iterator sit = split_mappings.begin();
                                                      sit != split_mappings.end();
                                                    ++sit)
            points.push_back( boost::lexical_cast<point>(*sit) );

          data_handle<viennamesh_point> point_handle = context.make_data<point>(points);

          algorithm.set_input( parameter_name, point_handle );
        }
        else if (parameter_type == "seed_point")
        {
          std::list<std::string> split_mappings = split_string_brackets( parameter_value, ";" );

          if (split_mappings.size() != 2)
          {
            error(1) << "String to seed point conversion: an entry has no point and no region id: " << parameter_value << std::endl;
            return false;
          }

          std::list<std::string>::const_iterator it = split_mappings.begin();

//           std::cout << "seed point to point: " << *it << std::endl;
          point p = boost::lexical_cast<point>( *it );
          ++it;
//           std::cout << "seed point to region id: " << *it << std::endl;
          viennagrid_int region_id = boost::lexical_cast<viennagrid_int>( *it );

          algorithm.push_back_input( parameter_name, seed_point(p, region_id) );
        }
        else if (parameter_type == "dynamic")
        {
          std::string source_algorithm_name = parameter_value.substr( 0, parameter_value.find("/") );
          std::string source_parameter_name = parameter_value.substr( parameter_value.find("/")+1 );

          algorithm_pipeline_element * default_source_element = get_element( source_algorithm_name );
          if (!default_source_element)
            return false;

          algorithm.link_input( parameter_name, default_source_element->algorithm, source_parameter_name );
          ++(default_source_element->reference_count);
          pipeline_element.referenced_elements.push_back( default_source_element );
        }
        else
        {
          error(1) << "Parameter \"" << parameter_name << "\": type \"" << parameter_type << "\" is not supported" << std::endl;
          return false;
        }
      }

    }

    algorithms.push_back( pipeline_element );

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

  bool algorithm_pipeline::run(bool cleanup_after_algorithm_step)
  {
    for (std::list<algorithm_pipeline_element>::iterator it = algorithms.begin(); it != algorithms.end(); ++it)
    {
      algorithm_pipeline_element & pe = *it;

      pe.change_log_levels();

      {
        std::string stack_name = "Running algorithm";
        if (!pe.name.empty())
          stack_name += " \"" + pe.name + "\"";
        stack_name += " (type = \"" + pe.algorithm.type() + "\")";

        viennamesh::LoggingStack stack(stack_name);
        if (!pe.algorithm.run())
          return false;
      }

      if (cleanup_after_algorithm_step)
      {
        for (std::size_t i = 0; i != pe.referenced_elements.size(); ++i)
          --(pe.referenced_elements[i]->reference_count);

        for (std::list<algorithm_pipeline_element>::iterator it2 = algorithms.begin(); it2 != it;)
        {
          if ((*it2).reference_count <= 0)
          {
            (*it2).algorithm.clear_inputs();
            (*it2).algorithm.clear_outputs();
//             std::cout << "CLEAR ALGORITHM " << (*it2).name << std::endl;
            it2 = algorithms.erase(it2);
          }
          else
            ++it2;
        }
      }

      pe.change_log_levels();
    }

    return true;
  }

  void algorithm_pipeline::clear()
  {
    algorithms.clear();
  }


  void algorithm_pipeline::set_base_path( std::string const & path )
  {
    for (std::list<algorithm_pipeline_element>::iterator it = algorithms.begin(); it != algorithms.end(); ++it)
      (*it).algorithm.set_base_path(path);
  }


  algorithm_pipeline_element * algorithm_pipeline::get_element(std::string const & algorithm_name)
  {
    algorithm_pipeline_element * result = 0;
    for (std::list<algorithm_pipeline_element>::iterator it = algorithms.begin(); it != algorithms.end(); ++it)
    {
      if ( (*it).name == algorithm_name )
      {
        if (result)
        {
          error(1) << "Algorithm name \"" << algorithm_name << "\" is ambiguous" << std::endl;
          return 0;
        }

        result = &(*it);
      }
    }

    if (!result)
      error(1) << "Algorithm with name \"" << algorithm_name << "\" was not found" << std::endl;

    return result;
  }


}
