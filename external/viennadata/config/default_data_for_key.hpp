/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#ifndef VIENNADATA_CONFIG_DEFAULT_DATA_FOR_KEY_HPP
#define VIENNADATA_CONFIG_DEFAULT_DATA_FOR_KEY_HPP

#include "viennadata/forwards.h"

/** @file viennadata/config/default_data_for_key.hpp
    @brief This file provides the prototype for setting default data types.
*/


namespace viennadata
{
  namespace config
  {
    /** @brief A traits class that specifies the default data type for a key. By default, no key type has a default data type.
    * 
    * Setting a default data type allows to omit the data type template argument. For example, if the default data type for a key type MyKey
    * is set to double, the following two lines of code are equivalent:
    * viennadata::access<MyKey, double>(key)(obj) = 3.1415;
    * viennadata::access<MyKey>(key)(obj) = 3.1415;
    *
    * The required specialization of 'default_data_for_key' is:
    * 
    * template <>
    * struct default_data_for_key<MyKey>
    * {
    *   typedef double    type;
    * };
    *
    * Mind that this overload has to be placed in namespace viennadata::config.
    * 
    * @tparam KeyType      The type of the key used for access
    */
    template <typename KeyType>
    struct default_data_for_key
    {
      //by default, the data type must not be omitted, thus force a compiler error in this case (no matching function found)
    };

  } //namespace config  
  
} //namespace viennadata

/** @brief A convenience macro for default data types: arg_KEYCLASS denotes the key type, arg_DATACLASS denotes the data type */
#define VIENNADATA_ENABLE_DEFAULT_DATA_TYPE_FOR_KEY(arg_KEYCLASS, arg_DATACLASS) \
namespace viennadata { namespace config { \
  template <> struct default_data_for_key<arg_KEYCLASS> {  typedef arg_DATACLASS  type; }; \
} }


#endif

