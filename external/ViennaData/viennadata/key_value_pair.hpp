/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */



#ifndef VIENNADATA_KEY_VALUE_MANAGER_HPP
#define VIENNADATA_KEY_VALUE_MANAGER_HPP

#include <map>
#include <vector>
#include <list>
#include <set>
#include <iostream>
#include <algorithm>
#include <memory>

#include "viennadata/data_container.hpp"
#include "viennadata/forwards.h"

/** @file viennadata/key_value_pair.hpp
    @brief Implementation of the compile-time/run-time dispatch needed for (key_type, value_type) pairs in order to support the use of viennadata::all.
*/

namespace viennadata
{
  /////////////////////// erase section //////////////////////////

  /** @brief The interface for type-erasued key-value type pairs on a certain element
   *
   * Refer to Alexandrescu, "Modern C++ Design" for details on type erasure
   *
   * @tparam object_type  The type of the object the data is associated with
   */
  template <typename ObjectType>
  class key_value_pair_interface
  {
    public:
      /** @brief A virtual destructor as recommended for abstract classes */
      virtual ~key_value_pair_interface() {};

      /** @brief Copy data from one object to another */
      virtual void copy(ObjectType const & src, ObjectType const & dest) const = 0;
      
      /** @brief Erase data for the particular object */
      virtual void erase(ObjectType const & src) const = 0;
      
      /** @brief Compare key-value-pairs for the same template arguments */
      virtual bool operator==(key_value_pair_interface<ObjectType> const &) const = 0;
  };


  /** @brief A dispatch facility that unwraps type-erasued key-value type pairs
  *
  * Refer to Alexandrescu, "Modern C++ Design" for details on type erasure
  *
  * @tparam key_type      The type of the key used for access
  * @tparam value_type    Type of the data that is stored for the element
  * @tparam object_type  The type of the object the data is associated with
  */
  template <typename KeyType,
            typename DataType,
            typename ObjectType>
  class key_value_pair_wrapper : public key_value_pair_interface<ObjectType>
  {
      typedef key_value_pair_wrapper<KeyType, DataType, ObjectType>      self_type;
      typedef key_value_pair_wrapper<KeyType, all, ObjectType>           self_type_key_all;
      typedef key_value_pair_wrapper<all, DataType, ObjectType>          self_type_all_value;
      typedef key_value_pair_wrapper<all, all, ObjectType>               self_type_all_all;

    public:
      /// Forwards the copy request to the data container.
      void copy(ObjectType const & src,
                ObjectType const & dest) const
      {
        data_container<KeyType, DataType, ObjectType>::instance().copy(src, dest);
      }

      /// Forwards the erase request to the data container.
      void erase(ObjectType const & src) const
      {
        data_container<KeyType, DataType, ObjectType>::instance().erase(src);
      }

      /** @brief Compares two key-value type erased pairs. Takes also the all-quantifier into account
       *
       * A comparison of <key1, value1> with <key2, value2> will yield false unless key1==key2 and value1==value2
       * A comparison of <key1, all> with <key2, value2> will yield false unless key1==key2.
       * A comparison of <all, value1> with <key2, value2> will yield false unless value1==value2.
       * A comparison of <all, all> with <key2, value2> always returns true
       *
       * @param other    The other type-erased key-value pair
       */
      bool operator==(key_value_pair_interface<ObjectType> const & other) const
      {
        return (dynamic_cast< self_type const *>( &other ) != NULL
                || dynamic_cast< self_type_key_all const *>( &other ) != NULL
                || dynamic_cast< self_type_all_value const *>( &other ) != NULL
                || dynamic_cast< self_type_all_all const *>( &other ) != NULL);
      }

  };


  /** @brief A dispatch facility that unwraps type-erasued key-value type pairs
  *
  * Refer to Alexandrescu, "Modern C++ Design" for details on type erasure
  *
  * @tparam object_type  The type of the object the data is associated with
  */
  template <typename ObjectType>
  class key_value_pair
  {
    public:
      key_value_pair() : key_value(NULL), owner(false) {};

      /** @brief Copy CTOR. Takes ownership of the key-value-pair in order to store pairs in a vector. */
      key_value_pair(key_value_pair const & other)
      {
        key_value = other.key_value;
        owner = true;
        other.owner = false;
      }

      /** @brief Destructor. Destroys the key-value pair only if it is the owner. */
      ~key_value_pair()
      {
        if (owner && key_value != NULL)
          delete key_value;
      }

      /** @brief Compares two key_value_pairs for wrapping the same key and value types. See also key_value_pair_wrapper::operator==() for full details */
      bool operator==(key_value_pair<ObjectType> const & other) const
      {
        return *key_value == *(other.key_value);
      }

      /** @brief Wrap the two supplied types within the key_value_pair 
       * 
       * @tparam key_type    Type of the key to be wrapped
       * @tparam value_type  Type of the data to be wrapped
       */
      template <typename KeyType, typename DataType>
      void add()
      {
        assert(key_value == NULL);
        key_value = new key_value_pair_wrapper<KeyType, DataType, ObjectType>();
      }

      /** @brief Copies data of the wrapped type to another object 
       *
       * @param src  The source object
       * @param dest The destination object
       */
      void copy(ObjectType const & src,
                ObjectType const & dest) const
      {
        key_value->copy(src, dest);
      }

      /** @brief Erases data for the wrapped key and value type from the supplied object */
      void erase(ObjectType const & el) const
      {
        key_value->erase(el);
      }

    private:
      key_value_pair_interface<ObjectType> * key_value;
      mutable bool owner; /// Ownership flag: If true, this object is responsible for deleting the key_value member
  };



}

#endif
