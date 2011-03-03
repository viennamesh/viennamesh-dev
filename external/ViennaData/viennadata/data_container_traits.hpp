/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#ifndef VIENNADATA_CONTAINER_TRAITS_HPP
#define VIENNADATA_CONTAINER_TRAITS_HPP

#include <map>

#include "viennadata/forwards.h"
#include "viennadata/object_identifier.hpp"
#include "viennadata/dispatch_traits.hpp"
#include "viennadata/storage_traits.hpp"

/** @file viennadata/data_container_traits.hpp
    @brief Manipulations of the different storage containers are implemented here.
*/

namespace viennadata
{
  // 8 possibilities in total:
  // [pointer based|id based] identification
  // [full key|no key] dispatch
  // [sparse|dense] storage
  
  
  /** @brief Helper class: Checks for valid access without key. If no key is supplied, than full dispatch is clearly illegal. */
  template <typename key_dispatch_tag>
  struct IS_ACCESS_WITHOUT_KEY_ALLOWED
  {
    typedef typename key_dispatch_tag::ERROR_ACCESS_CALLED_WITHOUT_KEY_ARGUMENT  check_type;
  };
  
  /** @brief No key is allowed when using a type based dispatch */
  template <>
  struct IS_ACCESS_WITHOUT_KEY_ALLOWED<type_key_dispatch_tag>
  {
    typedef void check_type; ///some dummy type definition
  };
  
  
  
  
  
  /** @brief Helper class: Reserve memory based on the selected identification mechanism. Nothing needs to be done when a map container is used. */
  template <typename container_type,
            typename object_identification_tag,
            typename storage_tag>
  struct container_reservation_dispatcher
  {
    //by default, do nothing: (some map-based mechanism)
    static void reserve(container_type & container, long num) {  }
  };
  
  /** @brief When used with dense data storage, the underlying vector container needs to be resized. */
  template <typename container_type>
  struct container_reservation_dispatcher<container_type, object_provided_id, dense_data_tag>
  {
    //id based identification:
    static void reserve(container_type & container, long num) { container.resize(num); }
  };
  
  
  
  
  
  ////////////////// container_key_value_pair ////////////////////////////
  /** @brief Accesses the data based on the provided (key_type, value_type) pair 
   * 
   * @tparam key_type          Type of the key
   * @tparam value_type        Type of the data 
   * @tparam key_dispatch_tag  A tag indicating full or type based key dispatch
   */
  template <typename key_type,
            typename value_type,
            typename key_dispatch_tag>
  struct container_key_value_pair
  {
    typedef std::map< key_type, value_type >   key_value_type;  // with key object based dispatch    
    
    /** @brief Accesses the container for the given object ID and the provided key */
    template <typename container_type,
              typename id_type>
    static value_type & access(container_type & cont,
                               id_type const & id,
                               key_type const & key)
    {
      return cont[id][key];
    }
  };
  
  /** @brief Specialization of container_key_value_pair for type-based key dispatch (type_key_dispatch_tag) */
  template <typename key_type,
            typename value_type>
  struct container_key_value_pair <key_type, value_type, type_key_dispatch_tag>
  {
    typedef value_type   key_value_type;  // with key object based dispatch    
    
    /** @brief Accesses the container for the given object ID.  Since using type-based dispatch, key argument is ignored. */
    template <typename container_type, typename id_type>
    static value_type & access(container_type & cont, id_type const & id, key_type const & key)
    {
      return cont[id];
    }

    //also allow access without key here, because uniquely defined:
    /** @brief Accesses the container for the given object ID. Since using type-based dispatch, no key is needed here. */
    template <typename container_type, typename id_type>
    static value_type & access(container_type & cont, id_type const & id)
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
   * @tparam key_type      The type of the key used for access
   * @tparam value_type    Type of the data that is stored for the element
   * @tparam object_type  The type of the object the data is associated with
   * @tparam object_identification_tag  Determines the dispatch mechanism for objects the data is associated with. Either 'pointer_based_id' (the default) or 'object_provided_id' (requires suitable overload of 'object_identifier')
   * @tparam key_dispatch_tag   Determines the method for key dispatch (either full dispatch ('full_key_dispatch_tag', default) or dispatch based on key type 'type_key_dispatch_tag')
   * @tparam storage_tag   Specifies dense ('dense_data_tag') or sparse ('storage_data_tag', default) storage of data. The former requires element-identification by id, cf. class 'object_identifier'
   */
  template <typename key_type,
            typename value_type,
            typename object_identification_tag,
            typename key_dispatch_tag,
            typename storage_tag>
  struct container_erasure_dispatcher
  {
    
    /** @brief Erase data for a particular key */
    template <typename container_type, typename id_type>
    static void erase(container_type & cont, id_type const & id, key_type const & key)
    {
      cont[id].erase(key);
    }
    
    /** @brief Erase all data associated with keys of the supplied type for the object */
    template <typename container_type, typename id_type>
    static void erase(container_type & cont, id_type const & id)
    {
      cont.erase(id);
    }
    
  };

  /** @brief Spezialization: Provides erase functionality for type-based key dispatch */
  template <typename key_type,
            typename value_type,
            typename object_identification_tag,
            typename storage_tag>
  struct container_erasure_dispatcher < key_type, 
                                        value_type,
                                        object_identification_tag,
                                        type_key_dispatch_tag,
                                        storage_tag >
  {
    /** @brief Erase data for the provided key */
    template <typename container_type, typename id_type>
    static void erase(container_type & cont, id_type const & id, key_type const & key)
    {
      cont.erase(id);
    }
    
    /** @brief Erase data for this key type (using type-based dispatch here!) */
    template <typename container_type, typename id_type>
    static void erase(container_type & cont, id_type const & id)
    {
      cont.erase(id);
    }
  };  

  /** @brief For a dense data storage with vectors, delete element by resetting. */
  template <typename key_type, typename value_type, typename key_dispatch_tag>
  struct container_erasure_dispatcher < key_type, value_type, object_provided_id, key_dispatch_tag, dense_data_tag >
  {
    typedef typename container_key_value_pair< key_type,
                                               value_type,
                                               key_dispatch_tag >::key_value_type   key_value_type;
                                               
    /** @brief Erase data for this key type*/
    template <typename container_type, typename id_type>
    static void erase(container_type & cont, id_type const & id, key_type const & key)
    {
      cont[id] = key_value_type();
    }
    
    /** @brief Erase data for this key type*/
    template <typename container_type, typename id_type>
    static void erase(container_type & cont, id_type const & id)
    {
      cont[id] = key_value_type();
    }
  };

  /** @brief Resolves the ambiguity erasing elements using dense storage with type-based dispatch */
  template <typename key_type, typename value_type>
  struct container_erasure_dispatcher < key_type, value_type, object_provided_id, type_key_dispatch_tag, dense_data_tag >
  {
    typedef typename container_key_value_pair< key_type,
                                               value_type,
                                               type_key_dispatch_tag >::key_value_type   key_value_type;
                                               
    /** @brief Erase data for this key type (using type-based dispatch here!) */
    template <typename container_type, typename id_type>
    static void erase(container_type & cont, id_type const & id, key_type const & key)
    {
      cont[id] = key_value_type();
    }
    
    /** @brief Erase data for this key type (using type-based dispatch here!) */
    template <typename container_type, typename id_type>
    static void erase(container_type & cont, id_type const & id)
    {
      cont[id] = key_value_type();
    }
  };

  
  
  ///////////////////////// storage traits //////////////////////////
  
  
  /** @brief Helper class for the deduction of the correct storage type. Default case. */
  template <typename key_type,
            typename value_type,
            typename object_type,
            typename object_identification_tag,
            typename key_dispatch_tag,
            typename storage_tag>
  struct container_storage_traits
  {
    //default case:
    typedef typename object_identifier<object_type>::id_type     id_type;
    typedef std::map< id_type,
                      typename container_key_value_pair< key_type,
                                                         value_type,
                                                         key_dispatch_tag >::key_value_type
                    >                                              container_type;
                    
     /** @brief Reserves memory for up to 'num' objects. */               
     static void reserve(container_type & cont, long num) { }
  };
  
  //dense storage when providing ID
  /** @brief Helper class for the deduction of the correct storage type. Dense storage with provided ID. */
  template <typename key_type,
            typename value_type,
            typename object_type,
            typename key_dispatch_tag>
  struct container_storage_traits <key_type, value_type, object_type,
                                   object_provided_id, key_dispatch_tag, dense_data_tag>
  {
    typedef typename object_identifier<object_type>::id_type     id_type;
    typedef std::vector< typename container_key_value_pair< key_type,
                                                            value_type,
                                                            key_dispatch_tag >::key_value_type
                       >                                           container_type;
                       
     /** @brief Reserves memory for up to 'num' objects. */               
     static void reserve(container_type & cont, long num) { cont.resize(num); }                  
  };
  
  
  /** @brief The main container manipulation class. 
   * 
   * @tparam key_type                  Type of the key
   * @tparam value_type                Type of the data
   * @tparam object_type               Type of the object the data is stored for
   * @tparam object_identification_tag Identification mechanism (either by address or by ID)
   * @tparam key_dispatch_tag          Dispatch by key object and type or only by type
   * @tparam storage_tag               Store data either in a dense or in a sparse manner
   */
  template <typename key_type,
            typename value_type,
            typename object_type,
            typename object_identification_tag = typename object_identifier<object_type>::tag,
            typename key_dispatch_tag = typename dispatch_traits<key_type>::tag,
            typename storage_tag = typename storage_traits<key_type, value_type, object_type>::tag>
  struct container_traits
  {
    // the datatype:
    typedef typename container_storage_traits<key_type, value_type, object_type,
                                              object_identification_tag,
                                              key_dispatch_tag,
                                              storage_tag>::container_type    container_type;

    /** @brief Accesses the data associated with the supplied key for the provided object */
    static value_type & access(container_type & cont, object_type const & obj, key_type const & key)
    {
       //std::cout << "Accessing sparse data by pointer" << std::endl;
       return container_key_value_pair <key_type,
                                        value_type,
                                        key_dispatch_tag>::access(cont, object_identifier<object_type>::id(obj), key);
    }

    /** @brief Accesses the data associated with the particular key type if type-based dispatch is used */
    static value_type & access(container_type & cont, object_type const & obj)
    {
      typedef typename IS_ACCESS_WITHOUT_KEY_ALLOWED<key_dispatch_tag>::check_type   some_type;
       return container_key_value_pair <key_type,
                                        value_type,
                                        key_dispatch_tag>::access(cont, object_identifier<object_type>::id(obj));
    }

    /** @brief Copies all data of the particular key type (including degenerate case of type based dispatch, where only single data is moved) */
    template <typename container_src_type, typename object_src_type>
    static void copy(container_src_type & cont_src, 
                     object_src_type const & obj_src,
                     container_type & cont_dest, 
                     object_type const & obj_dest)
    {
      //TODO: can be improved if cont_src[id_src] is actually empty, because there is no move necessary then...
       cont_dest[object_identifier<object_type>::id(obj_dest)] = cont_src[object_identifier<object_src_type>::id(obj_src)];
    }
    

    /** @brief Reserves memory for data using up to 'num' objects. */
    static void reserve(container_type & cont, long num)
    {
       container_storage_traits<key_type, value_type, object_type,
                                object_identification_tag,
                                key_dispatch_tag,
                                storage_tag>::reserve(cont, num);
    }

    /** @brief Erases data for a particular key */
    static void erase(container_type & cont, 
                      object_type const & obj,
                      key_type const & key)
    {
      //cont[object_identifier<object_type>::id(element)].erase(key);
      container_erasure_dispatcher<key_type, value_type,
                                       object_identification_tag,
                                       key_dispatch_tag,
                                       storage_tag>::erase(cont, object_identifier<object_type>::id(obj), key);
    }

    /** @brief Erases data for all keys of that type */
    static void erase(container_type & cont,
                      object_type const & obj)
    {
      container_erasure_dispatcher<key_type, value_type,
                                       object_identification_tag,
                                       key_dispatch_tag,
                                       storage_tag>::erase(cont, object_identifier<object_type>::id(obj));
    }

    /** @brief Checks whether data for a particular object with a particular key is already stored.
     *
     * @return Returns NULL if no data is found, otherwise returns a pointer to the data
     */ 
    static value_type * find(container_type & cont,
                             object_type const & obj,
                             key_type const & key)
    {
      typename std::map< key_type, value_type >::iterator it = cont[object_identifier<object_type>::id(obj)].find(key);
      if (it == cont[object_identifier<object_type>::id(obj)].end())
        return NULL;
      
      return &(it->second);
    }

  };



} // namespace viennadata


#endif
