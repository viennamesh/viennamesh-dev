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


///// three arguments for macro /////

/** @brief A convenience macro for dense data storage for a particular tuple of key type, data type and object type */
#define VIENNADATA_ENABLE_DENSE_DATA_STORAGE_FOR_KEY_DATA_OBJECT(arg_KEYCLASS, arg_DATACLASS, arg_OBJECTCLASS) \
namespace viennadata { namespace config { \
    template <>  struct storage<arg_KEYCLASS, arg_DATACLASS, arg_OBJECTCLASS> { typedef dense_data_tag    tag; }; \
} }

/** @brief A convenience macro for sparse data storage for a particular tuple of key type, data type and object type.
 * 
 * Makes sense only if the dense data storage is enabled for a large class of (key, data, object)-type tuples.
 */
#define VIENNADATA_DISABLE_DENSE_DATA_STORAGE_FOR_KEY_DATA_OBJECT(arg_KEYCLASS, arg_DATACLASS, arg_OBJECTCLASS) \
namespace viennadata { namespace config { \
    template <>  struct storage<arg_KEYCLASS, arg_DATACLASS, arg_OBJECTCLASS> { typedef sparse_data_tag    tag; }; \
} }

///// two arguments for macro //////

// (key, data):

/** @brief A convenience macro for dense data storage for a particular tuple of key type and data type */
#define VIENNADATA_ENABLE_DENSE_DATA_STORAGE_FOR_KEY_DATA(arg_KEYCLASS, arg_DATACLASS) \
namespace viennadata { namespace config { \
    template <typename ObjectType>  struct storage<arg_KEYCLASS, arg_DATACLASS, ObjectType> { typedef dense_data_tag    tag; }; \
} }

/** @brief A convenience macro for sparse data storage for a particular pair of key type and data type.
 * 
 * Makes sense only if the dense data storage is enabled for a large class of (key, data, object)-type tuples.
 */
#define VIENNADATA_DISABLE_DENSE_DATA_STORAGE_FOR_KEY_DATA(arg_KEYCLASS, arg_DATACLASS) \
namespace viennadata { namespace config { \
    template <typename ObjectType>  struct storage<arg_KEYCLASS, arg_DATACLASS, ObjectType> { typedef sparse_data_tag    tag; }; \
} }



// (key, object):

/** @brief A convenience macro for dense data storage for a particular tuple of key type and object type */
#define VIENNADATA_ENABLE_DENSE_DATA_STORAGE_FOR_KEY_OBJECT(arg_KEYCLASS, arg_OBJECTCLASS) \
namespace viennadata { namespace config { \
    template <typename DataType>  struct storage<arg_KEYCLASS, DataType, arg_OBJECTCLASS> { typedef dense_data_tag    tag; }; \
} }

/** @brief A convenience macro for sparse data storage for a particular pair of key type and object type
 * 
 * Makes sense only if the dense data storage is enabled for a large class of (key, data, object)-type tuples.
 */
#define VIENNADATA_DISABLE_DENSE_DATA_STORAGE_FOR_KEY_OBJECT(arg_KEYCLASS, arg_OBJECTCLASS) \
namespace viennadata { namespace config { \
    template <typename DataType>  struct storage<arg_KEYCLASS, DataType, arg_OBJECTCLASS> { typedef sparse_data_tag    tag; }; \
} }


// (data, object):

/** @brief A convenience macro for dense data storage for a particular tuple of data type and object type */
#define VIENNADATA_ENABLE_DENSE_DATA_STORAGE_FOR_DATA_OBJECT(arg_DATACLASS, arg_OBJECTCLASS) \
namespace viennadata { namespace config { \
    template <typename KeyType>  struct storage<KeyType, arg_DATACLASS, arg_OBJECTCLASS> { typedef dense_data_tag    tag; }; \
} }

/** @brief A convenience macro for sparse data storage for a particular pair of data type and object type
 * 
 * Makes sense only if the dense data storage is enabled for a large class of (key, data, object)-type tuples.
 */
#define VIENNADATA_DISABLE_DENSE_DATA_STORAGE_FOR_DATA_OBJECT(arg_DATACLASS, arg_OBJECTCLASS) \
namespace viennadata { namespace config { \
    template <typename KeyType>  struct storage<KeyType, arg_DATACLASS, arg_OBJECTCLASS> { typedef sparse_data_tag    tag; }; \
} }


///// one argument for macro //////

/** @brief A convenience macro for dense data storage for a certain key type */
#define VIENNADATA_ENABLE_DENSE_DATA_STORAGE_FOR_KEY(arg_KEYCLASS) \
namespace viennadata { namespace config { \
    template <typename DataType, typename ObjectType> \
    struct storage<arg_KEYCLASS, DataType, ObjectType> { typedef dense_data_tag    tag; }; \
} }

/** @brief A convenience macro for dense data storage for a certain data type */
#define VIENNADATA_ENABLE_DENSE_DATA_STORAGE_FOR_DATA(arg_DATACLASS) \
namespace viennadata { namespace config { \
    template <typename KeyType, typename ObjectType> \
    struct storage<KeyType, arg_DATACLASS, ObjectType> { typedef dense_data_tag    tag; }; \
} }

/** @brief A convenience macro for dense data storage for a certain object type */
#define VIENNADATA_ENABLE_DENSE_DATA_STORAGE_FOR_OBJECT(arg_OBJECTCLASS) \
namespace viennadata { namespace config { \
    template <typename KeyType, typename DataType> \
    struct storage<KeyType, DataType, arg_OBJECTCLASS> { typedef dense_data_tag    tag; }; \
} }

//Note that selective disablers with one argument do not make sense, since there is no 'stronger' global enabler for dense storage.

#endif

