/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#ifndef VIENNADATA_STORAGE_TRAITS_HPP
#define VIENNADATA_STORAGE_TRAITS_HPP

#include "viennadata/forwards.h"

/** @file viennadata/storage_traits.hpp
    @brief This file sets the default storage scheme to sparse. Switching to a dense storage scheme should be done individually by the library user in his own files
*/


namespace viennadata
{

  /** @brief A traits class that specifies whether data should be stored in a dense (i.e. vector) or a sparse (i.e. map) manner.
   * 
   * For dense storage it is required that ViennaData knows how to get ids from the elements (see element_identifier), otherwise it falls back to sparse storage.
   * Add custom element identifier in your own code by overloading storage_traits appropriately.
   * E.g. to store data of type double for class 'MyClass' in a dense manner, use
   *
   * template < typename key_type>
   * struct storage_traits< key_type, double, MyClass>
   * {
   *   typedef dense_data_tag    tag;
   * };
   *
   * Mind that this overload has to be placed in namespace viennadata.
   * 
   * @tparam key_type      The type of the key used for access
   * @tparam value_type    Type of the data that is stored for the element
   * @tparam object_type  The type of the object the data is associated with
   */
  template <typename key_type,
            typename value_type,
            typename object_type>
  struct storage_traits
  {
    typedef sparse_data_tag    tag;
  };

  
  
} //namespace viennadata


#endif

