/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#ifndef VIENNADATA_DATA_CONTAINER_HPP
#define VIENNADATA_DATA_CONTAINER_HPP


#include <map>

#include "viennadata/forwards.h"
#include "viennadata/config/object_identifier.hpp"
#include "viennadata/traits/data_container.hpp"
#include "viennadata/key_value_registration.hpp"

/** @file viennadata/data_container.hpp
    @brief The main data storage class is implemented here. However, most operations are forwarded to viennadata/data_container_traits.hpp, where dispatches with respect to the underlying storage scheme are carried out.
*/


namespace viennadata
{

  /** @brief The central container class holding the data. Uses the singleton pattern for a particular key, value and object type triple
   * 
   * @tparam KeyType      The type of the key used for access
   * @tparam DataType    Type of the data that is stored for the element
   * @tparam ObjectType  The type of the object the data is associated with
   * 
   */
  template <typename KeyType,
            typename DataType,
            typename ObjectType>
  class data_container
  {
    private:
      /** @brief The type of the data container */
      typedef typename traits::container<KeyType, DataType, ObjectType>::container_type    container_type;

      container_type container; ///here is the data stored

      /** @brief Private CTOR due to singleton pattern */
      data_container() {}
      /** @brief Private copy CTOR due to singleton pattern */
      data_container(data_container const & other) {}
      /** @brief Private DTOR due to singleton pattern */
      ~data_container() {}

    public:
      /** @brief Returns the only instance of data_container (Singleton Pattern) */
      static data_container & instance()
      {
        static data_container * da = NULL;
        if (da == NULL)
        {
          da = new data_container();
          key_value_registration<ObjectType>::instance().template add<KeyType, DataType>();
        }

        return *da;
      }

      /** @brief Accesses data for object 'obj' stored at key 'key' using a full key dispatch (object and type). Used internally only, use viennadata::access() instead */
      DataType & access(ObjectType const & obj,
                        KeyType const & key)
      {
        return traits::container<KeyType, DataType, ObjectType>::access(container, obj, key);
      }

      /** @brief Accesses data for object 'obj' stored at key 'key' using a type based key dispatch. Used internally only, use viennadata::access() instead */
      DataType & access(ObjectType const & obj)
      {
        return traits::container<KeyType, DataType, ObjectType>::access(container, obj);
      }

      //////////////////  copy data: ////////////////////
      
      //multiple copy - stage 1: this is the source, forward the request to the destination:
      /** @brief Copy data from one object to another. This is the first stage of the process. Called via viennadata::copy().
       *
       * @param obj_src      The source object
       * @param obj_dest     The destination object
       */
      template <typename ObjectDestType>
      void copy(ObjectType const & obj_src,
                ObjectDestType const & obj_dest)
      {
        data_container<KeyType, DataType, ObjectDestType>::instance().copy(container, obj_src, obj_dest);
      }
      
      
      //multiple copy - stage 2: this is the destination: Perform the copy
      /** @brief Copy data from one object to another. This is the second stage of the process. Called via viennadata::copy().
       *
       * @param cont_src     The container of the source object
       * @param obj_src      The source object
       * @param obj_dest     The destination object
       */
      template <typename ContainerSrcType,
                typename ObjectSrcType>
      void copy(ContainerSrcType & cont_src,
                ObjectSrcType const & obj_src,
                ObjectType const & obj_dest)
      {
        traits::container<KeyType, DataType, ObjectType>::copy(cont_src, obj_src, container, obj_dest);
      }

      //////////////////  erase data: ////////////////////
      /** @brief Erases data stored for a particular key. Called via viennadata::erase().
       * 
       * @param obj       The object for which data should be deleted
       * @param key       The key the data is associated with
       */
      void erase(ObjectType const & obj,
                 KeyType const & key)
      {
        traits::container<KeyType, DataType, ObjectType>::erase(container, obj, key);
      }

      // erase data associated with all keys of key_type
      /** @brief Erases all data stored the particular key type. Called via viennadata::erase().
       * 
       * @param obj       The object for which data should be deleted
       */
      void erase(ObjectType const & obj)
      {
        traits::container<KeyType, DataType, ObjectType>::erase(container, obj);
      }

      // reserve memory if a vector type is used
      /** @brief Reserves memory for data associated with up to 'num' objects. Object IDs have to be in the range 0...(num-1). */
      void reserve(long num)
      {
        //std::cout << "Reserving..." << std::endl;
        traits::container<KeyType, DataType, ObjectType>::reserve(container, num);
      }
      
      /** @brief Checks whether data for a particular object with a particular key is already stored.
       *
       * @return Returns NULL if no data is found, otherwise returns a pointer to the data
       */ 
      DataType * find(ObjectType const & obj,
                      KeyType const & key)
      {
        return traits::container<KeyType, DataType, ObjectType>::find(container, obj, key);
      }

      /** @brief Checks whether data for a particular object with a particular key is already stored.
       *
       * @return Returns NULL if no data is found, otherwise returns a pointer to the data
       */ 
      DataType * find(ObjectType const & obj)
      {
        return traits::container<KeyType, DataType, ObjectType>::find(container, obj);
      }

  };

} // namespace viennadata

#endif
