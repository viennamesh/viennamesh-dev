/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#ifndef VIENNADATA_CONFIG_OBJECT_IDENTIFIER_HPP
#define VIENNADATA_CONFIG_OBJECT_IDENTIFIER_HPP

#include "viennadata/forwards.h"

/** @file viennadata/config/object_identifier.hpp
    @brief This file provides the default object identification mechanism in ViennaData.
*/


namespace viennadata
{
  namespace config
  {
    /** @brief Traits class that specifies the object identification mechanism.
    * 
    * Custom identification mechanisms can be provided by proper specializations.
    * Requirements:
    *   - typedef 'tag' provides a tag for the identification mechanism: Either 'pointer_based_id' or 'object_provided_id'
    *   - typedef 'id_type' that specifies the data type of the ID (e.g. int)
    *   - static member function id() that provides the ID for the element
    * 
    * E.g. for a class SomeClass that provides a member function int getID(), one writes
    * 
    * template <>
    * struct element_identifier<SomeClass>
    * {
    *   typedef element_provided_id         tag;
    *   typedef int                         id_type;
    * 
    *   static id_type get(SomeClass const & el) { return el.getID(); }
    * };
    * 
    * @tparam ObjectType  The type of the object the data is associated with
    */
    template <typename ObjectType>
    struct object_identifier
    {
      typedef pointer_based_id        tag;
      typedef const ObjectType *      id_type;
      
      /** @brief per default, the element address is the id */
      static id_type get(ObjectType const & obj) { return &obj; }
    };

    //add here some custom element identifier
  }
} //namespace viennadata


#endif

