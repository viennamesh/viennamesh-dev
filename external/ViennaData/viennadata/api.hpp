/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#ifndef VIENNADATA_INTERFACE_HPP
#define VIENNADATA_INTERFACE_HPP

#include "forwards.h"
#include "viennadata/data_container.hpp"
#include "viennadata/key_value_registration.hpp"
#include "viennadata/config/default_data_for_key.hpp"

/** @file viennadata/api.hpp
    @brief This file contains all the public interface of ViennaData
*/

/**
 * \defgroup public_interface Public Interface
 * @brief This module contains all the functions needed by library users of ViennaData.
 */


namespace viennadata
{

  //////////// data access ////////////////////
  /** @brief A proxy class for accessing data with key type and key object dispatch. Only used internally in ViennaData 
    *
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  class data_accessor_with_key
  {
    public:
      data_accessor_with_key(KeyType const & key) : key_(key) {}
      
      template <typename ObjectType>
      DataType & operator()(ObjectType const & el)
      {
        //std::cout << "Get data from element" << std::endl;
        return data_container<KeyType, DataType, ObjectType>::instance().access(el, key_);
      }
      
    private:
      KeyType const & key_;    
  };

  /** @brief A proxy class for accessing data with key type dispatch, but no key object dispatch. Only used internally in ViennaData 
    *
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  class data_accessor_no_key
  {
    public:
      data_accessor_no_key() {}
      
      template <typename ObjectType>
      DataType & operator()(ObjectType const & el)
      {
        //std::cout << "Get data from element" << std::endl;
        return data_container<KeyType, DataType, ObjectType>::instance().access(el);
      }
  };
  
  
  // data getter and setter:
  /** @brief The main access function within ViennaData. Allows to access data stored with a particular key, using full dispatch (type and object)
    *
    * Should be called in the form
    *   <pre>viennadata:access<KeyType, DataType>(my_key)(my_obj) = some_value;</pre>
    * and
    *   <pre>some_value = viennadata:access<KeyType, DataType>(my_key)(my_obj);</pre>
    * This would write and read data stored using a key 'my_key' of type 'KeyType' from 'my_obj'.
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  data_accessor_with_key<KeyType, DataType> access(KeyType const & key)
  {
    return data_accessor_with_key<KeyType, DataType>(key);
  }

  /** @brief The main access function within ViennaData using a default data type. Allows to access data stored with a particular key, using full dispatch (type and object)
    *
    * Should be called in the form
    *   <pre>viennadata:access<KeyType>(my_key)(my_obj) = some_value;</pre>
    * and
    *   <pre>some_value = viennadata:access<KeyType>(my_key)(my_obj);</pre>
    * This would write and read data stored using a key 'my_key' of type 'KeyType' from 'my_obj'.
    * 
    * Default data type is set by specializations of the class viennadata::config::default_data_for_key. See examples/tutorials/default_data_type.cpp for an example. 
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    */
  template <typename KeyType>
  data_accessor_with_key<KeyType, typename config::default_data_for_key<KeyType>::type> access(KeyType const & key)
  {
    return data_accessor_with_key<KeyType, typename config::default_data_for_key<KeyType>::type>(key);
  }



  /** @brief The main access function within ViennaData. Allows to access data stored with a particular key, using type based dispatch. 
    *
    * Should be called in the form 
    *   <pre>viennadata:access<KeyType, DataType>()(my_obj) = some_value;</pre>
    * and 
    *   <pre>some_value = viennadata:access<KeyType, DataType>()(my_obj);</pre>
    * This would write and read data stored using a key of type 'KeyType' from 'my_obj'.
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  data_accessor_no_key<KeyType, DataType> access()
  {
    return data_accessor_no_key<KeyType, DataType>();
  }
  
  /** @brief The main access function within ViennaData, using a default data type.
    *
    * Should be called in the form 
    *   <pre>viennadata:access<KeyType>()(my_obj) = some_value;</pre>
    * and 
    *   <pre>some_value = viennadata:access<KeyType>()(my_obj);</pre>
    * This would write and read data stored using a key of type 'KeyType' from 'my_obj'.
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    */
  template <typename KeyType>
  data_accessor_no_key<KeyType, typename config::default_data_for_key<KeyType>::type > access()
  {
    return data_accessor_no_key<KeyType, typename config::default_data_for_key<KeyType>::type >();
  }
  
  //////////// erase data ////////////////////
  /** @brief A proxy class for erasing data. Only used internally in ViennaData 
    *
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  class data_erasor_with_key
  {
    public:
      data_erasor_with_key(KeyType const & k) : key(k) {}
      
      template <typename ObjectType>
      void operator()(ObjectType const & el)
      {
        data_container<KeyType, DataType, ObjectType>::instance().erase(el, key);
      }
    private:
      KeyType const & key;
  };
  
  //prevent messing around with all-type and supplied key:
  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <typename KeyType>
  class data_erasor_with_key<KeyType, all>
  {
    typedef typename error_indicator<KeyType>::ERROR_NO_KEY_ARGUMENT_ALLOWED_WHEN_USING_ERASE_FUNCTION_WITH_ALL   error_type;
  };

  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <typename DataType>
  class data_erasor_with_key<all, DataType>
  {
    typedef typename error_indicator<DataType>::ERROR_NO_KEY_ARGUMENT_ALLOWED_WHEN_USING_ERASE_FUNCTION_WITH_ALL   error_type;
  };

  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <>
  class data_erasor_with_key<all, all>
  {
    //force linker error here
  };
  
  /** @brief A proxy class for erasing data. Only used internally in ViennaData 
    *
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  class data_erasor_no_key
  {
    public:
      data_erasor_no_key() {}
      
      template <typename ObjectType>
      void operator()(ObjectType const & el)
      {
        data_container<KeyType, DataType, ObjectType>::instance().erase(el);
      }
  };

  /** @brief Specialization for erasing all data for a particular KeyType by providing viennadata::all for the DataType */
  template <typename KeyType>
  class data_erasor_no_key <KeyType, all>
  {
    public:
      data_erasor_no_key() {}
      
      template <typename ObjectType>
      void operator()(ObjectType const & el)
      {
        key_value_registration<ObjectType>::instance().template erase_key_all<KeyType>(el);
      }
  };

  /** @brief Specialization for erasing all data of a particular DataType by providing viennadata::all for the KeyType */
  template <typename DataType>
  class data_erasor_no_key <all, DataType>
  {
    public:
      data_erasor_no_key() {}
      
      template <typename ObjectType>
      void operator()(ObjectType const & el)
      {
        key_value_registration<ObjectType>::instance().template erase_all_value<DataType>(el);
      }
  };
  
  /** @brief Specialization for erasing all data using viennadata::all */
  template <>
  class data_erasor_no_key <all, all>
  {
    public:
      data_erasor_no_key() {}
      
      template <typename ObjectType>
      void operator()(ObjectType const & el)
      {
        key_value_registration<ObjectType>::instance().erase_all_all(el);
      }
  };
  
  /** @brief Erase data associated with key 'key'. The object is obtained via a proxy class.
    * 
    * Should be called in the form 
    *   <pre>viennadata:erase<KeyType, DataType>(my_key)(my_obj);</pre>
    * This would erase data stored using a key 'my_key' of type 'KeyType' from 'my_obj'.
    *
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  data_erasor_with_key<KeyType, DataType> erase(KeyType const & key)
  {
    return data_erasor_with_key<KeyType, DataType>(key);
  }

  /** @brief Erase data associated with key 'key' for the default data type. The object is obtained via a proxy class.
    * 
    * Should be called in the form 
    *   <pre>viennadata:erase<KeyType>(my_key)(my_obj);</pre>
    * This would erase data stored using a key 'my_key' of type 'KeyType' from 'my_obj'.
    *
    * Default data type is set by specializations of the class viennadata::config::default_data_for_key. See examples/tutorials/default_data_type.cpp for an example. 
    *
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    */
  template <typename KeyType>
  data_erasor_with_key<KeyType, typename config::default_data_for_key<KeyType>::type> erase(KeyType const & key)
  {
    return data_erasor_with_key<KeyType, typename config::default_data_for_key<KeyType>::type>(key);
  }

  /** @brief Erase all data associated with keys of 'KeyType'.
    *
    * Should be called in the form 
    *   <pre>viennadata:erase<KeyType, DataType>()(my_obj);</pre>
    * This would erase data stored using a key of type 'KeyType' from 'my_obj'.
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  data_erasor_no_key<KeyType, DataType> erase()
  {
    return data_erasor_no_key<KeyType, DataType>();
  }

  /** @brief Erase all data associated with keys of 'KeyType' using a default data type.
    *
    * Should be called in the form 
    *   <pre>viennadata:erase<KeyType>()(my_obj);</pre>
    * This would erase data stored using a key of type 'KeyType' from 'my_obj'.
    * 
    * Default data type is set by specializations of the class viennadata::config::default_data_for_key. See examples/tutorials/default_data_type.cpp for an example. 
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    */
  template <typename KeyType>
  data_erasor_no_key<KeyType, typename config::default_data_for_key<KeyType>::type> erase()
  {
    return data_erasor_no_key<KeyType, typename config::default_data_for_key<KeyType>::type>();
  }

  
  
  //////////////////////// Copy data //////////////////////////////////
  /** @brief A proxy class for copying data associated with a particular key from one object to another. Only used internally in ViennaData 
    *
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  class data_copy_with_key
  {
    public:
      data_copy_with_key(KeyType const & key) : key_(key) {}
      
      template <typename ObjectSrcType,
                typename ObjectDestType>
      void operator()(ObjectSrcType const & el_src,
                      ObjectDestType const & el_dest)
      {
        access<KeyType, DataType>(key_)(el_dest) = access<KeyType, DataType>(key_)(el_src);
      }
    private:
      KeyType const & key_;
  };
  
  //prevent messing around with all-type and supplied key:
  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <typename KeyType>
  class data_copy_with_key<KeyType, all>
  {
    typedef typename error_indicator<KeyType>::ERROR_NO_KEY_ARGUMENT_ALLOWED_WHEN_USING_COPY_FUNCTION_WITH_ALL   error_type;
  };

  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <typename DataType>
  class data_copy_with_key<all, DataType>
  {
    typedef typename error_indicator<DataType>::ERROR_NO_KEY_ARGUMENT_ALLOWED_WHEN_USING_COPY_FUNCTION_WITH_ALL   error_type;
  };

  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <>
  class data_copy_with_key<all, all>
  {
    //force linker error here
  };
  
  /** @brief Copies data associated with key 'key' from one object to another.
    *
    * Should be called in the form 
    *   <pre>viennadata:copy<KeyType, DataType>()(src_obj, dest_obj);</pre>
    * This would copy data stored using a key of type 'KeyType' from 'src_obj' to 'dest_obj'.
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  data_copy_with_key<KeyType, DataType> copy(KeyType const & key)
  {
    return data_copy_with_key<KeyType, DataType>(key);
  }

  /** @brief Copies data associated with key 'key' from one object to another using a default data type.
    *
    * Should be called in the form 
    *   <pre>viennadata:copy<KeyType>()(src_obj, dest_obj);</pre>
    * This would copy data stored using a key of type 'KeyType' from 'src_obj' to 'dest_obj'.
    * 
    * Default data type is set by specializations of the class viennadata::config::default_data_for_key. See examples/tutorials/default_data_type.cpp for an example. 
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    */
  template <typename KeyType>
  data_copy_with_key<KeyType, typename config::default_data_for_key<KeyType>::type> copy(KeyType const & key)
  {
    return data_copy_with_key<KeyType, typename config::default_data_for_key<KeyType>::type>(key);
  }

  
  /** @brief A proxy class for copying data associated with a particular key type from one object to another. Only used internally in ViennaData 
    *
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  class data_copy_no_key
  {
    public:
      data_copy_no_key() {}
      
      template <typename object_src_type, typename object_dest_type>
      void operator()(object_src_type const & el_src, object_dest_type const & el_dest)
      {
        data_container<KeyType, DataType, object_src_type>::instance().copy(el_src, el_dest);
      }
  };
  
  /** @brief Specialization for copying all data associated with a particular key type from one object to another using viennadata::all */
  template <typename KeyType>
  class data_copy_no_key <KeyType, all>
  {
    public:
      data_copy_no_key() {}
      
      template <typename ObjectType>
      void operator()(ObjectType const & el_src, ObjectType const & el_dest)
      {
        key_value_registration<ObjectType>::instance().template copy_key_all<KeyType>(el_src, el_dest);
      }
      
      template <typename object_src_type, typename object_dest_type>
      void operator()(object_src_type const & el_src, object_dest_type const & el_dest)
      {
        typedef typename error_indicator<object_src_type>::ERROR_SOURCE_AND_DESTINATION_MUST_BE_OF_SAME_TYPE_WHEN_USING_COPY_WITH_ALL  error_type;
      }
  };

  /** @brief Specialization for copying all data of a particular DataType from one object to another using viennadata::all for the KeyType */
  template <typename DataType>
  class data_copy_no_key <all, DataType>
  {
    public:
      data_copy_no_key() {}
      
      template <typename ObjectType>
      void operator()(ObjectType const & el_src,
                      ObjectType const & el_dest)
      {
        key_value_registration<ObjectType>::instance().template copy_all_value<DataType>(el_src, el_dest);
      }
      
      template <typename object_src_type,
                typename object_dest_type>
      void operator()(object_src_type const & el_src, object_dest_type const & el_dest)
      {
        typedef typename error_indicator<object_src_type>::ERROR_SOURCE_AND_DESTINATION_MUST_BE_OF_SAME_TYPE_WHEN_USING_COPY_WITH_ALL  error_type;
      }
  };
  
  /** @brief Specialization for copying all data from one object to another using viennadata::all */
  template <>
  class data_copy_no_key <all, all>
  {
    public:
      data_copy_no_key() {}
      
      template <typename ObjectType>
      void operator()(ObjectType const & el_src,
                      ObjectType const & el_dest)
      {
        key_value_registration<ObjectType>::instance().copy_all_all(el_src, el_dest);
      }
      
      template <typename object_src_type, typename object_dest_type>
      void operator()(object_src_type const & el_src,
                      object_dest_type const & el_dest)
      {
        typedef typename error_indicator<object_src_type>::ERROR_SOURCE_AND_DESTINATION_MUST_BE_OF_SAME_TYPE_WHEN_USING_COPY_WITH_ALL  error_type;
      }
  };
  
  
  
  /** @brief Copies data associated with all keys of type 'KeyType' from one object to another.
    *
    * Should be called in the form 
    *   <pre>viennadata:copy<KeyType, DataType>()(src_obj, dest_obj);</pre>
    * This would copy data stored using a key of type 'KeyType' from 'src_obj' to 'dest_obj'.
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType, typename DataType>
  data_copy_no_key<KeyType, DataType> copy()
  {
    return data_copy_no_key<KeyType, DataType>();
  }
  
  /** @brief Copies data associated with all keys of type 'KeyType' from one object to another using a default data type.
    *
    * Should be called in the form 
    *   <pre>viennadata:copy<KeyType>()(src_obj, dest_obj);</pre>
    * This would copy data stored using a key of type 'KeyType' from 'src_obj' to 'dest_obj'.
    * 
    * Default data type is set by specializations of the class viennadata::config::default_data_for_key. See examples/tutorials/default_data_type.cpp for an example. 
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    */
  template <typename KeyType>
  data_copy_no_key<KeyType, typename config::default_data_for_key<KeyType>::type> copy()
  {
    return data_copy_no_key<KeyType, typename config::default_data_for_key<KeyType>::type>();
  }


  //////////////////////// Move data //////////////////////////////////
  /** @brief A proxy class for moving data associated with a particular key from one object to another. Only used internally in ViennaData 
    *
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  class data_mover_with_key
  {
    public:
      data_mover_with_key(KeyType const & key) : key_(key) {}
      
      template <typename ObjectSrcType,
                typename ObjectDestType>
      void operator()(ObjectSrcType const & el_src,
                      ObjectDestType const & el_dest)
      {
        //std::cout << "Get data from element" << std::endl;
        access<KeyType, DataType>(key_)(el_dest) = access<KeyType, DataType>(key_)(el_src);
        erase<KeyType, DataType>(key_)(el_src);
      }
    private:
      KeyType const & key_;
  };
  
  
  //prevent messing around with all-type and supplied key:
  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <typename KeyType>
  class data_mover_with_key<KeyType, all>
  {
    typedef typename error_indicator<KeyType>::ERROR_NO_KEY_ARGUMENT_ALLOWED_WHEN_USING_COPY_FUNCTION_WITH_ALL   error_type;
  };

  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <typename DataType>
  class data_mover_with_key<all, DataType>
  {
    typedef typename error_indicator<DataType>::ERROR_NO_KEY_ARGUMENT_ALLOWED_WHEN_USING_COPY_FUNCTION_WITH_ALL   error_type;
  };

  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <>
  class data_mover_with_key<all, all>
  {
    //force linker error here
  };
  
  
  /** @brief Moves data associated with key 'key' from one object to another.
    *
    * Should be called in the form 
    *   <pre>viennadata:move<KeyType, DataType>(key)(src_obj, dest_obj);</pre>
    * This would move data stored using a key 'key' of type 'KeyType' from 'src_obj' to 'dest_obj'.
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  data_mover_with_key<KeyType, DataType> move(KeyType const & key)
  {
    return data_mover_with_key<KeyType, DataType>(key);
  }
  
  /** @brief Moves data associated with key 'key' from one object to another using a default data type specified for the key type.
    *
    * Should be called in the form 
    *   <pre>viennadata:move<KeyType>(key)(src_obj, dest_obj);</pre>
    * This would move data stored using a key 'key' of type 'KeyType' from 'src_obj' to 'dest_obj'.
    * 
    * Default data type is set by specializations of the class viennadata::config::default_data_for_key. See examples/tutorials/default_data_type.cpp for an example. 
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    */
  template <typename KeyType>
  data_mover_with_key<KeyType, typename config::default_data_for_key<KeyType>::type> move(KeyType const & key)
  {
    return data_mover_with_key<KeyType, typename config::default_data_for_key<KeyType>::type>(key);
  }


  /** @brief A proxy class for moving data associated with a particular key type from one object to another. Only used internally in ViennaData 
    *
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  class data_mover_no_key
  {
    public:
      data_mover_no_key() {}
      
      template <typename object_src_type, typename object_dest_type>
      void operator()(object_src_type const & el_src, object_dest_type const & el_dest)
      {
        data_copy_no_key<KeyType, DataType>()(el_src, el_dest);
        data_erasor_no_key<KeyType, DataType>()(el_src);
      }
  };
  
  /** @brief Moves data associated with all keys of type 'KeyType' from one object to another.
    *
    * Should be called in the form 
    *   <pre>viennadata:move<KeyType, DataType>()(src_obj, dest_obj);</pre>
    * This would move data stored using a key of type 'KeyType' from 'src_obj' to 'dest_obj'.
    * 
    * \ingroup public_interface
    * @tparam KeyType      Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  data_mover_no_key<KeyType, DataType> move()
  {
    return data_mover_no_key<KeyType, DataType>();
  }
  
  /** @brief Moves data associated with all keys of type 'KeyType' from one object to another using a default data type specified for the key type.
    *
    * Should be called in the form 
    *   <pre>viennadata:move<KeyType>()(src_obj, dest_obj);</pre>
    * This would move data stored using a key of type 'KeyType' from 'src_obj' to 'dest_obj'.
    * 
    * Default data type is set by specializations of the class viennadata::config::default_data_for_key. See examples/tutorials/default_data_type.cpp for an example. 
    * 
    * \ingroup public_interface
    * @tparam KeyType      Type of the key
    */
  template <typename KeyType>
  data_mover_no_key<KeyType, typename config::default_data_for_key<KeyType>::type> move()
  {
    return data_mover_no_key<KeyType, typename config::default_data_for_key<KeyType>::type>();
  }


  //////////// memory allocation for data ////////////////////
  
  /** @brief A proxy class for reserving memory needed for storing data associated with a particular key type. Only used internally in ViennaData 
    *
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType,
            typename DataType>
  class data_reservation_proxy
  {
    public:
      data_reservation_proxy(long num) : num_(num) {}
      
      template <typename ObjectType>
      void operator()(ObjectType const & e)
      {
        data_container<KeyType, DataType, ObjectType>::instance().reserve(num_);
      }
    
    private:
      long num_;
  };
  

  /** @brief Reserves memory for storing data associated with a particular key type.
    *
    * Should be called in the form 
    *   <pre>viennadata:reserve<KeyType, DataType>(num)(my_obj);</pre>
    * This would reserve memory for num objects with the same type as my_obj for data storage using a key of type 'KeyType'. Object IDs have to be in the range 0...(num-1) then.
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    * @param  num            Number of objects for which memory should be reserved. Object IDs have to be in the range 0...(num-1) then.
    */
  template <typename KeyType,
            typename DataType>
  data_reservation_proxy<KeyType, DataType> reserve(long num)
  {
    return data_reservation_proxy<KeyType, DataType>(num);
  }
  
  /** @brief Reserves memory for storing data associated with a particular key type and the specified default data type.
    *
    * Should be called in the form 
    *   <pre>viennadata:reserve<KeyType>(num)(my_obj);</pre>
    * This would reserve memory for num objects with the same type as my_obj for data storage using a key of type 'KeyType'. Object IDs have to be in the range 0...(num-1) then.
    * 
    * Default data type is set by specializations of the class viennadata::config::default_data_for_key. See examples/tutorials/default_data_type.cpp for an example. 
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    * @param  num            Number of objects for which memory should be reserved. Object IDs have to be in the range 0...(num-1) then.
    */
  template <typename KeyType>
  data_reservation_proxy<KeyType, typename config::default_data_for_key<KeyType>::type> reserve(long num)
  {
    return data_reservation_proxy<KeyType, typename config::default_data_for_key<KeyType>::type>(num);
  }
  
  /////////// find sparse data /////////////
  /** @brief A proxy class for finding data associated with a particular key. Only used internally in ViennaData 
    *
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType, typename DataType>
  class data_find_proxy_with_key
  {
    public:
      data_find_proxy_with_key(KeyType const & key) : key_(key) {}
      
      template <typename ObjectType>
      DataType * operator()(ObjectType const & el)
      {
        return data_container<KeyType, DataType, ObjectType>::instance().find(el, key_);
      }
    
    private:
      KeyType const & key_;
  };

  /** @brief Reserves memory for storing data associated with a particular key type.
    *
    * Should be called in the form 
    *   <pre>viennadata:find<KeyType, DataType>(my_key)(my_obj);</pre>
    * This would search for data associated with my_obj using a key of type 'KeyType'. If found, a pointer to the data is returned, otherwise NULL.
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    * @param  key            The key object
    */
  template <typename KeyType, typename DataType>
  data_find_proxy_with_key<KeyType, DataType> find(KeyType const & key)
  {
    return data_find_proxy_with_key<KeyType, DataType>(key);
  }

  /** @brief Reserves memory for storing data associated with a particular key type and the specified default data type.
    *
    * Should be called in the form 
    *   <pre>viennadata:find<KeyType>(my_key)(my_obj);</pre>
    * This would search for data associated with my_obj using a key of type 'KeyType'. If found, a pointer to the data is returned, otherwise NULL.
    * 
    * Default data type is set by specializations of the class viennadata::config::default_data_for_key. See examples/tutorials/default_data_type.cpp for an example. 
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    * @param  key            The key object
    */
  template <typename KeyType>
  data_find_proxy_with_key<KeyType, typename config::default_data_for_key<KeyType>::type> find(KeyType const & key)
  {
    return data_find_proxy_with_key<KeyType, typename config::default_data_for_key<KeyType>::type>(key);
  }



  /** @brief A proxy class for finding data associated with a particular key. Only used internally in ViennaData 
    *
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    */
  template <typename KeyType, typename DataType>
  class data_find_proxy_no_key
  {
    public:
      data_find_proxy_no_key() {}
      
      template <typename ObjectType>
      DataType * operator()(ObjectType const & el)
      {
        return data_container<KeyType, DataType, ObjectType>::instance().find(el);
      }
  };

  /** @brief Reserves memory for storing data associated with a particular key type (key type dispatch).
    *
    * Should be called in the form 
    *   <pre>viennadata:find<KeyType, DataType>()(my_obj);</pre>
    * This would search for data associated with my_obj using a key of type 'KeyType'. If found, a pointer to the data is returned, otherwise NULL.
    * 
    * \ingroup public_interface
    * @tparam KeyType       Type of the key
    * @tparam DataType     Type of the data
    * @param  key            The key object
    */
  
  template <typename KeyType, typename DataType>
  data_find_proxy_no_key<KeyType, DataType> find()
  {
    return data_find_proxy_no_key<KeyType, DataType>();
  }

} //namespace viennadata


#endif 

