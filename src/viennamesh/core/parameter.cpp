#include "viennamesh/core/parameter.hpp"

namespace viennamesh
{
  std::pair<std::string, bool> type_properties::get_property( type_info_wrapper const & ti, std::string const & name ) const
  {
    std::map<type_info_wrapper, PropertyMapType>::const_iterator tpit = type_properties_map.find(ti);
    if (tpit != type_properties_map.end())
    {
      PropertyMapType::const_iterator pmit = tpit->second.find(name);
      if (pmit != tpit->second.end())
      {
        return std::pair<std::string, bool>(pmit->second, true);
      }
    }

    return std::pair<std::string, bool>("", false);
  }

  bool type_properties::match_properties( type_info_wrapper const & ti, PropertyMapType const & to_match ) const
  {
    if (to_match.empty())
      return true;

    std::map<type_info_wrapper, PropertyMapType>::const_iterator pmit = type_properties_map.find(ti);

    if (pmit == type_properties_map.end())
      return false;

    PropertyMapType const & properties = pmit->second;

    for (PropertyMapType::const_iterator it = to_match.begin(); it != to_match.end(); ++it)
    {
      PropertyMapType::const_iterator pit = properties.find( it->first );
      if ( pit == properties.end() )
        return false;

      if ( it->second != pit->second )
        return false;
    }

    return true;
  }

  bool type_properties::match_property( type_info_wrapper const & ti, std::string const & key, std::string const & value ) const
  {
    std::map<type_info_wrapper, PropertyMapType>::const_iterator pmit = type_properties_map.find(ti);

    if (pmit == type_properties_map.end())
      return false;

    PropertyMapType const & properties = pmit->second;

    PropertyMapType::const_iterator pit = properties.find(key);
    if ( pit == properties.end() )
      return false;

    return pit->second == value;
  }

  void type_properties::print() const
  {
    for (std::map<type_info_wrapper, PropertyMapType>::const_iterator tit = type_properties_map.begin(); tit != type_properties_map.end(); ++tit)
    {
      std::cout << "Type: " << tit->first.name() << std::endl;
      for (PropertyMapType::const_iterator pit = tit->second.begin(); pit != tit->second.end(); ++pit)
      {
        std::cout << "  " << pit->first << " - " << pit->second << std::endl;
      }
    }
  }

  type_info_wrapper converter::get_type_id( const_parameter_handle const & tmp )
  { return typeid(*tmp); }

  shared_ptr<base_conversion_function> converter::convert_function( const_parameter_handle const & input, const_parameter_handle const & output_mesh )
  {
    ConversionFunctionMapMapType::iterator ipit = conversions.find(typeid(*input));
    if (ipit != conversions.end())
    {
      ConversionFunctionMapType::iterator opit = ipit->second.find(typeid(*output_mesh));
      if ( opit != ipit->second.end() )
      {
        return opit->second;
      }
    }

    return shared_ptr<base_conversion_function>();
  }




  shared_ptr<base_conversion_function> converter::best_convert_function( const_parameter_handle const & input, std::map<std::string, std::string> const & properties )
  {
    ConversionFunctionMapMapType::iterator ipit = conversions.find(typeid(*input));
    if (ipit != conversions.end())
    {
      ConversionFunctionMapType::iterator best = ipit->second.end();
      for (ConversionFunctionMapType::iterator opit = ipit->second.begin(); opit != ipit->second.end(); ++opit)
      {
        if ( type_properties::get().match_properties(opit->second->output_type(), properties) )
        {
          if (best == ipit->second.end())
            best = opit;
          else
          {
            if (best->second->function_depth > opit->second->function_depth)
              best = opit;
          }
        }
      }

      if (best != ipit->second.end())
        return best->second;
    }

    return shared_ptr<base_conversion_function>();
  }

  shared_ptr<base_conversion_function> converter::best_convert_function( const_parameter_handle const & input, std::string const & property_key, std::string const & property_value )
  {
    ConversionFunctionMapMapType::iterator ipit = conversions.find(typeid(*input));
    if (ipit != conversions.end())
    {
      ConversionFunctionMapType::iterator best = ipit->second.end();
      for (ConversionFunctionMapType::iterator opit = ipit->second.begin(); opit != ipit->second.end(); ++opit)
      {
        if ( type_properties::get().match_property(opit->second->output_type(), property_key, property_value) )
        {
          if (best == ipit->second.end())
            best = opit;
          else
          {
            if (best->second->function_depth > opit->second->function_depth)
              best = opit;
          }
        }
      }

      if (best != ipit->second.end())
        return best->second;
    }

    return shared_ptr<base_conversion_function>();
  }



  void converter::print_conversions(const_parameter_handle const & input) const
  {
    LoggingStack ls("Supported conversion functions");
    info(10) << "Source type: [" << &typeid(*input) << "] " << std::endl;
    ConversionFunctionMapMapType::const_iterator ipit = conversions.find(typeid(*input));
    if (ipit != conversions.end())
    {
      for (ConversionFunctionMapType::const_iterator opit = ipit->second.begin(); opit != ipit->second.end(); ++opit)
      {
        info(10) << "Supports conversion to (" << opit->second->function_depth << "): [" << opit->first.get() << "] " << std::endl;
      }
    }
  }











}
