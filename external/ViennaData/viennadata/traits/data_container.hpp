/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#ifndef VIENNADATA_TRAITS_DATA_CONTAINER_HPP
#define VIENNADATA_TRAITS_DATA_CONTAINER_HPP

#include <map>

#include "viennadata/forwards.h"
#include "viennadata/config/object_identifier.hpp"
#include "viennadata/config/key_dispatch.hpp"
#include "viennadata/config/storage.hpp"

/** @file viennadata/traits/data_container.hpp
    @brief Manipulations of the different storage containers are implemented here.
*/

namespace viennadata
{
  namespace traits{
      
    // 8 possibilities in total:
    // [pointer based|id based] identification
    // [full key|no key] dispatch
    // [sparse|dense] storage
    
    
    /** @brief Helper class: Checks for valid access without key. If no key is supplied, than full dispatch is clearly illegal. */
    template <typename KeyDispatchTag>
    struct IS_ACCESS_WITHOUT_KEY_ALLOWED
    {
      typedef typename KeyDispatchTag::ERROR_ACCESS_CALLED_WITHOUT_KEY_ARGUMENT  check_type;
    };
    
    /** @brief No key is allowed when using a type based dispatch */
    template <>
    struct IS_ACCESS_WITHOUT_KEY_ALLOWED<type_key_dispatch_tag>
    {
      typedef void check_type; ///some dummy type definition
    };
    
    
    
    
    
    /** @brief Helper class: Reserve memory based on the selected identification mechanism. Nothing needs to be done when a map container is used. */
    template <typename ContainerType,
              typename ObjectIdentificationTag,
              typename StorageTag>
    struct container_reservation_dispatcher
    {
      //by default, do nothing: (some map-based mechanism)
      static void reserve(ContainerType & container, long num) {  }
    };
    
    /** @brief When used with dense data storage, the underlying vector container needs to be resized. */
    template <typename ContainerType>
    struct container_reservation_dispatcher<ContainerType, object_provided_id, dense_data_tag>
    {
      //id based identification:
      static void reserve(ContainerType & container, long num) { container.resize(num); }
    };
    
    
    
    
    
    ////////////////// container_key_value_pair ////////////////////////////
    /** @brief Accesses the data based on the provided (KeyType, DataType) pair 
    * 
    * @tparam KeyType          Type of the key
    * @tparam DataType        Type of the data 
    * @tparam KeyDispatchTag  A tag indicating full or type based key dispatch
    */
    template <typename KeyType,
              typename DataType,
              typename KeyDispatchTag>
    struct container_key_value_pair
    {
      typedef std::map< KeyType, DataType >   key_DataType;  // with key object based dispatch    
      
      /** @brief Accesses the container for the given object ID and the provided key */
      template <typename ContainerType,
                typename IdType>
      static DataType & access(ContainerType & cont,
                               IdType const & id,
                               KeyType const & key)
      {
        return cont[id][key];
      }
    };
    
    /** @brief Specialization of container_key_value_pair for type-based key dispatch (type_key_dispatch_tag) */
    template <typename KeyType,
              typename DataType>
    struct container_key_value_pair <KeyType, DataType, type_key_dispatch_tag>
    {
      typedef DataType   key_DataType;  // with key object based dispatch    
      
      /** @brief Accesses the container for the given object ID.  Since using type-based dispatch, key argument is ignored. */
      template <typename ContainerType, typename IdType>
      static DataType & access(ContainerType & cont, IdType const & id, KeyType const & key)
      {
        return cont[id];
      }

      //also allow access without key here, because uniquely defined:
      /** @brief Accesses the container for the given object ID. Since using type-based dispatch, no key is needed here. */
      template <typename ContainerType, typename IdType>
      static DataType & access(ContainerType & cont, IdType const & id)
      {
        return cont[id];
      }
    };
    
    ///////////////////////// erasure dispatch //////////////////////////
    
    //general case: call erase of the underlying map-type
    /** @brief A dispatch facility that unwraps type-erasued key-value type pairs
    * 
    * Refer to Alexandrescu, "Modern C++ Design" for details on type erasure
    * 
    * @tparam KeyType      The type of the key used for access
    * @tparam DataType    Type of the data that is stored for the element
    * @tparam ObjectType  The type of the object the data is associated with
    * @tparam ObjectIdentificationTag  Determines the dispatch mechanism for objects the data is associated with. Either 'pointer_based_id' (the default) or 'object_provided_id' (requires suitable overload of 'object_identifier')
    * @tparam KeyDispatchTag   Determines the method for key dispatch (either full dispatch ('full_KeyDispatchTag', default) or dispatch based on key type 'type_KeyDispatchTag')
    * @tparam StorageTag   Specifies dense ('dense_data_tag') or sparse ('storage_data_tag', default) storage of data. The former requires element-identification by id, cf. class 'object_identifier'
    */
    template <typename KeyType,
              typename DataType,
              typename ObjectIdentificationTag,
              typename KeyDispatchTag,
              typename StorageTag>
    struct container_erasure_dispatcher
    {
      
      /** @brief Erase data for a particular key */
      template <typename ContainerType, typename IdType>
      static void erase(ContainerType & cont, IdType const & id, KeyType const & key)
      {
        cont[id].erase(key);
      }
      
      /** @brief Erase all data associated with keys of the supplied type for the object */
      template <typename ContainerType, typename IdType>
      static void erase(ContainerType & cont, IdType const & id)
      {
        cont.erase(id);
      }
      
    };

    /** @brief Spezialization: Provides erase functionality for type-based key dispatch */
    template <typename KeyType,
              typename DataType,
              typename ObjectIdentificationTag,
              typename StorageTag>
    struct container_erasure_dispatcher < KeyType, 
                                          DataType,
                                          ObjectIdentificationTag,
                                          type_key_dispatch_tag,
                                          StorageTag >
    {
      /** @brief Erase data for the provided key */
      template <typename ContainerType, typename IdType>
      static void erase(ContainerType & cont, IdType const & id, KeyType const & key)
      {
        cont.erase(id);
      }
      
      /** @brief Erase data for this key type (using type-based dispatch here!) */
      template <typename ContainerType, typename IdType>
      static void erase(ContainerType & cont, IdType const & id)
      {
        cont.erase(id);
      }
    };  

    /** @brief For a dense data storage with vectors, delete element by resetting. */
    template <typename KeyType, typename DataType, typename KeyDispatchTag>
    struct container_erasure_dispatcher < KeyType, DataType, object_provided_id, KeyDispatchTag, dense_data_tag >
    {
      typedef typename container_key_value_pair< KeyType,
                                                 DataType,
                                                 KeyDispatchTag >::key_DataType   key_DataType;
                                                
      /** @brief Erase data for this key type*/
      template <typename ContainerType, typename IdType>
      static void erase(ContainerType & cont, IdType const & id, KeyType const & key)
      {
        cont[id] = key_DataType();
      }
      
      /** @brief Erase data for this key type*/
      template <typename ContainerType, typename IdType>
      static void erase(ContainerType & cont, IdType const & id)
      {
        cont[id] = key_DataType();
      }
    };

    /** @brief Resolves the ambiguity erasing elements using dense storage with type-based dispatch */
    template <typename KeyType, typename DataType>
    struct container_erasure_dispatcher < KeyType, DataType, object_provided_id, type_key_dispatch_tag, dense_data_tag >
    {
      typedef typename container_key_value_pair< KeyType,
                                                 DataType,
                                                 type_key_dispatch_tag >::key_DataType   key_DataType;
                                                
      /** @brief Erase data for this key type (using type-based dispatch here!) */
      template <typename ContainerType, typename IdType>
      static void erase(ContainerType & cont, IdType const & id, KeyType const & key)
      {
        cont[id] = key_DataType();
      }
      
      /** @brief Erase data for this key type (using type-based dispatch here!) */
      template <typename ContainerType, typename IdType>
      static void erase(ContainerType & cont, IdType const & id)
      {
        cont[id] = key_DataType();
      }
    };

    
    
    ///////////////////////// storage traits //////////////////////////
    
    
    /** @brief Helper class for the deduction of the correct storage type. Default case. */
    template <typename KeyType,
              typename DataType,
              typename ObjectType,
              typename ObjectIdentificationTag,
              typename KeyDispatchTag,
              typename StorageTag>
    struct container_storage
    {
      //default case:
      typedef typename viennadata::config::object_identifier<ObjectType>::id_type     id_type;
      typedef std::map< id_type,
                        typename container_key_value_pair< KeyType,
                                                          DataType,
                                                          KeyDispatchTag >::key_DataType
                      >                                              container_type;
                      
      /** @brief Reserves memory for up to 'num' objects. */               
      static void reserve(container_type & cont, long num) { }
    };
    
    //dense storage when providing ID
    /** @brief Helper class for the deduction of the correct storage type. Dense storage with provided ID. */
    template <typename KeyType,
              typename DataType,
              typename ObjectType,
              typename KeyDispatchTag>
    struct container_storage <KeyType, DataType, ObjectType,
                              object_provided_id, KeyDispatchTag, dense_data_tag>
    {
      typedef typename viennadata::config::object_identifier<ObjectType>::id_type     id_type;
      typedef std::vector< typename container_key_value_pair< KeyType,
                                                              DataType,
                                                              KeyDispatchTag >::key_DataType
                        >                                           container_type;
                        
      /** @brief Reserves memory for up to 'num' objects. */               
      static void reserve(container_type & cont, long num) { cont.resize(num); }                  
    };
    
   
    /** @brief Resizes a vector automatically in order to allow valid access */
    template <typename StorageTag>
    struct container_auto_resize
    {
      template <typename ContainerType, typename SizeType>
      static void apply(ContainerType const & c, SizeType const & s) {}
    };
    
    /** @brief Resizes a vector automatically in order to allow valid access. Specialization for dense data access. */
    template <>
    struct container_auto_resize<dense_data_tag>
    {
      template <typename ContainerType>
      static void apply(ContainerType & c, size_t s)
      {
        if (s >= c.size())
        {
          //std::cout << "Auto resize to " << s + 1 << std::endl;
          c.resize(s + 1);
        }
      }
    };
    
    
    
    /** @brief The main container manipulation class. 
    * 
    * @tparam KeyType                  Type of the key
    * @tparam DataType                Type of the data
    * @tparam ObjectType               Type of the object the data is stored for
    * @tparam ObjectIdentificationTag Identification mechanism (either by address or by ID)
    * @tparam KeyDispatchTag          Dispatch by key object and type or only by type
    * @tparam StorageTag               Store data either in a dense or in a sparse manner
    */
    template <typename KeyType,
              typename DataType,
              typename ObjectType,
              typename ObjectIdentificationTag = typename viennadata::config::object_identifier<ObjectType>::tag,
              typename KeyDispatchTag = typename viennadata::config::key_dispatch<KeyType>::tag,
              typename StorageTag = typename viennadata::config::storage<KeyType, DataType, ObjectType>::tag>
    struct container
    {
      // the datatype:
      typedef typename container_storage<KeyType, DataType, ObjectType,
                                        ObjectIdentificationTag,
                                        KeyDispatchTag,
                                        StorageTag>::container_type    container_type;

      /** @brief Accesses the data associated with the supplied key for the provided object */
      static DataType & access(container_type & cont, ObjectType const & obj, KeyType const & key)
      {
        //std::cout << "Accessing sparse data by pointer" << std::endl;
        
        //make sure that for dense data access the resizing is done:
        container_auto_resize<StorageTag>::apply(cont, viennadata::config::object_identifier<ObjectType>::get(obj));
        
        return container_key_value_pair <KeyType,
                                          DataType,
                                          KeyDispatchTag>::access(cont, viennadata::config::object_identifier<ObjectType>::get(obj), key);
      }

      /** @brief Accesses the data associated with the particular key type if type-based dispatch is used */
      static DataType & access(container_type & cont, ObjectType const & obj)
      {
        typedef typename IS_ACCESS_WITHOUT_KEY_ALLOWED<KeyDispatchTag>::check_type   some_type;
        
        //make sure that for dense data access the resizing is done:
        container_auto_resize<StorageTag>::apply(cont, viennadata::config::object_identifier<ObjectType>::get(obj));
        
        return container_key_value_pair <KeyType,
                                          DataType,
                                          KeyDispatchTag>::access(cont, viennadata::config::object_identifier<ObjectType>::get(obj));
      }

      /** @brief Copies all data of the particular key type (including degenerate case of type based dispatch, where only single data is moved) */
      template <typename ContainerSrcType, typename ObjectSrcType>
      static void copy(ContainerSrcType & cont_src, 
                       ObjectSrcType const & obj_src,
                       container_type & cont_dest, 
                       ObjectType const & obj_dest)
      {
        container_auto_resize<StorageTag>::apply(cont_src, viennadata::config::object_identifier<ObjectSrcType>::get(obj_src));
        container_auto_resize<StorageTag>::apply(cont_dest, viennadata::config::object_identifier<ObjectType>::get(obj_dest));
        
        //TODO: can be improved if cont_src[id_src] is actually empty, because there is no move necessary then...
        cont_dest[viennadata::config::object_identifier<ObjectType>::get(obj_dest)] =
          cont_src[viennadata::config::object_identifier<ObjectSrcType>::get(obj_src)];
      }
      

      /** @brief Reserves memory for data using up to 'num' objects. */
      static void reserve(container_type & cont, long num)
      {
        container_storage<KeyType, DataType, ObjectType,
                          ObjectIdentificationTag,
                          KeyDispatchTag,
                          StorageTag>::reserve(cont, num);
      }

      /** @brief Erases data for a particular key */
      static void erase(container_type & cont, 
                        ObjectType const & obj,
                        KeyType const & key)
      {
        container_auto_resize<StorageTag>::apply(cont, viennadata::config::object_identifier<ObjectType>::get(obj));
        
        //cont[object_identifier<ObjectType>::id(element)].erase(key);
        container_erasure_dispatcher<KeyType, DataType,
                                     ObjectIdentificationTag,
                                     KeyDispatchTag,
                                     StorageTag>::erase(cont, viennadata::config::object_identifier<ObjectType>::get(obj), key);
      }

      /** @brief Erases data for all keys of that type */
      static void erase(container_type & cont,
                        ObjectType const & obj)
      {
        container_auto_resize<StorageTag>::apply(cont, viennadata::config::object_identifier<ObjectType>::get(obj));
        
        container_erasure_dispatcher<KeyType, DataType,
                                     ObjectIdentificationTag,
                                     KeyDispatchTag,
                                     StorageTag>::erase(cont, viennadata::config::object_identifier<ObjectType>::get(obj));
      }

      /** @brief Checks whether data for a particular object with a particular key is already stored.
      *
      * @return Returns NULL if no data is found, otherwise returns a pointer to the data
      */ 
      static DataType * find(container_type & cont,
                             ObjectType const & obj,
                             KeyType const & key)
      {
        container_auto_resize<StorageTag>::apply(cont, viennadata::config::object_identifier<ObjectType>::get(obj));
        
        typename std::map< KeyType, DataType >::iterator it = cont[viennadata::config::object_identifier<ObjectType>::get(obj)].find(key);
        if (it == cont[viennadata::config::object_identifier<ObjectType>::get(obj)].end())
          return NULL;
        
        return &(it->second);
      }

      /** @brief Checks whether data for a particular object with a particular key is already stored. Key-type dispatch version.
      *
      * @return Returns NULL if no data is found, otherwise returns a pointer to the data
      */ 
      static DataType * find(container_type & cont,
                             ObjectType const & obj)
      {
        //container_auto_resize<StorageTag>::apply(cont, viennadata::config::object_identifier<ObjectType>::get(obj));
        
        typename container_type::iterator it = cont.find(viennadata::config::object_identifier<ObjectType>::get(obj));
        if (it == cont.end())
          return NULL;
        
        return &(it->second);
      }

    };

  } //namespace traits

} // namespace viennadata


#endif
