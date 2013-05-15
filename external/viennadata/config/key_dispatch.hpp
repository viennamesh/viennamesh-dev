/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#ifndef VIENNADATA_TRAITS_KEY_DISPATCH_HPP
#define VIENNADATA_TRAITS_KEY_DISPATCH_HPP

#include "viennadata/forwards.h"

/** @file viennadata/config/key_dispatch.hpp
    @brief This file provides the default key dispatch method.
*/

namespace viennadata
{
  namespace config
  {
  
    /** @brief A traits class that specifies whether data is accessed using full key dispatches (object and type), or using the key type only.
    * 
    * For changing the default full dispatch to a type-based dispatch, overload this class appropriately
    * E.g. to use a type-based dispatch for keys of type QuickKey, write
    *
    * template <>
    * struct key_dispatch<QuickKey>
    * {
    *   typedef type_key_dispatch_tag    tag;
    * };
    *
    * Mind that this overload has to be placed in namespace viennadata.
    * 
    * @tparam KeyType      The type of the key for which the dispatch scheme is specified.
    */
    template <typename KeyType>
    struct key_dispatch
    {
      typedef full_key_dispatch_tag    tag;
    };
    
  } //config
} //namespace viennadata


// Provide a convenience macro for key dispatch:
/** @brief A convenience macro for enabling type-based key dispatch: arg_CLASS denotes the key type for which type-based dispatch should be enabled */
#define VIENNADATA_ENABLE_TYPE_BASED_KEY_DISPATCH(arg_CLASS) \
namespace viennadata { namespace config { \
    template <> struct key_dispatch<arg_CLASS> { typedef type_key_dispatch_tag    tag; }; \
} }


#endif

