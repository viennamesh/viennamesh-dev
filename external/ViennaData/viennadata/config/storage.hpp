/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#ifndef VIENNADATA_CONFIG_STORAGE_HPP
#define VIENNADATA_CONFIG_STORAGE_HPP

#include "viennadata/forwards.h"

/** @file viennadata/config/storage.hpp
    @brief This file sets the default storage scheme to sparse. Switching to a dense storage scheme should be done individually by the library user in his own files
*/


namespace viennadata
{
  namespace config
  {
    /** @brief A traits class that specifies whether data should be stored in a dense (i.e. vector) or a sparse (i.e. map) manner.
    * 
    * For dense storage it is required that ViennaData knows how to get ids from the elements (see element_identifier), otherwise it falls back to sparse storage.
    * Add custom element identifier in your own code by overloading storage_traits appropriately.
    * E.g. to store data of type double for class 'MyClass' in a dense manner, use
    *
    * template < typename KeyType>
    * struct storage_traits< KeyType, double, MyClass>
    * {
    *   typedef dense_data_tag    tag;
    * };
    *
    * Mind that this overload has to be placed in namespace viennadata.
    * 
    * @tparam KeyType     The type of the key used for access
    * @tparam DataType    Type of the data that is stored for the element
    * @tparam ObjectType  The type of the object the data is associated with
    */
    template <typename KeyType,
              typename DataType,
              typename ObjectType>
    struct storage
    {
      typedef sparse_data_tag    tag;
    };

  } //namespace config  
  
} //namespace viennadata


#endif

