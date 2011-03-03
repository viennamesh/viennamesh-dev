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

/** @file viennadata/interface.hpp
    @brief This file contains all the public interface of ViennaData
*/

/**
 * \defgroup public_interface Public Interface
 * @brief for library users of ViennaData
 */


namespace viennadata
{

  //////////// data access ////////////////////
  /** @brief A proxy class for accessing data with key type and key object dispatch. Only used internally in ViennaData 
    *
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  class data_accessor_with_key
  {
    public:
      data_accessor_with_key(key_type const & key) : key_(key) {}
      
      template <typename object_type>
      value_type & operator()(object_type const & el)
      {
        //std::cout << "Get data from element" << std::endl;
        return data_container<key_type, value_type, object_type>::instance().access(el, key_);
      }
      
    private:
      key_type const & key_;    
  };

  /** @brief A proxy class for accessing data with key type dispatch, but no key object dispatch. Only used internally in ViennaData 
    *
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  class data_accessor_no_key
  {
    public:
      data_accessor_no_key() {}
      
      template <typename object_type>
      value_type & operator()(object_type const & el)
      {
        //std::cout << "Get data from element" << std::endl;
        return data_container<key_type, value_type, object_type>::instance().access(el);
      }
  };
  
  
  // data getter and setter:
  /** @brief The main access function within ViennaData. Allows to access data stored with a particular key, using full dispatch (type and object)
    *
    * Should be called in the form
    *   <pre>viennadata:access<key_type, value_type>(my_key)(my_obj) = some_value;</pre>
    * and
    *   <pre>some_value = viennadata:access<key_type, value_type>(my_key)(my_obj);</pre>
    * This would write and read data stored using a key 'my_key' of type 'key_type' from 'my_obj'.
    * 
    * \ingroup public_interface
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  data_accessor_with_key<key_type, value_type> access(key_type const & key)
  {
    return data_accessor_with_key<key_type, value_type>(key);
  }

  /** @brief The main access function within ViennaData. Allows to access data stored with a particular key, using type based dispatch. 
    *
    * Should be called in the form 
    *   <pre>viennadata:access<key_type, value_type>()(my_obj) = some_value;</pre>
    * and 
    *   <pre>some_value = viennadata:access<key_type, value_type>()(my_obj);</pre>
    * This would write and read data stored using a key of type 'key_type' from 'my_obj'.
    * 
    * \ingroup public_interface
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  data_accessor_no_key<key_type, value_type> access()
  {
    return data_accessor_no_key<key_type, value_type>();
  }
  
  
  //////////// erase data ////////////////////
  /** @brief A proxy class for erasing data. Only used internally in ViennaData 
    *
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  class data_erasor_with_key
  {
    public:
      data_erasor_with_key(key_type const & k) : key(k) {}
      
      template <typename object_type>
      void operator()(object_type const & el)
      {
        data_container<key_type, value_type, object_type>::instance().erase(el, key);
      }
    private:
      key_type const & key;
  };
  
  //prevent messing around with all-type and supplied key:
  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <typename key_type>
  class data_erasor_with_key<key_type, all>
  {
    typedef typename error_indicator<key_type>::ERROR_NO_KEY_ARGUMENT_ALLOWED_WHEN_USING_ERASE_FUNCTION_WITH_ALL   error_type;
  };

  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <typename value_type>
  class data_erasor_with_key<all, value_type>
  {
    typedef typename error_indicator<value_type>::ERROR_NO_KEY_ARGUMENT_ALLOWED_WHEN_USING_ERASE_FUNCTION_WITH_ALL   error_type;
  };

  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <>
  class data_erasor_with_key<all, all>
  {
    //force linker error here
  };
  
  /** @brief A proxy class for erasing data. Only used internally in ViennaData 
    *
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  class data_erasor_no_key
  {
    public:
      data_erasor_no_key() {}
      
      template <typename object_type>
      void operator()(object_type const & el)
      {
        data_container<key_type, value_type, object_type>::instance().erase(el);
      }
  };

  /** @brief Specialization for erasing all data for a particular key_type by providing viennadata::all for the value_type */
  template <typename key_type>
  class data_erasor_no_key <key_type, all>
  {
    public:
      data_erasor_no_key() {}
      
      template <typename object_type>
      void operator()(object_type const & el)
      {
        key_value_registration<object_type>::instance().template erase_key_all<key_type>(el);
      }
  };

  /** @brief Specialization for erasing all data of a particular value_type by providing viennadata::all for the key_type */
  template <typename value_type>
  class data_erasor_no_key <all, value_type>
  {
    public:
      data_erasor_no_key() {}
      
      template <typename object_type>
      void operator()(object_type const & el)
      {
        key_value_registration<object_type>::instance().template erase_all_value<value_type>(el);
      }
  };
  
  /** @brief Specialization for erasing all data using viennadata::all */
  template <>
  class data_erasor_no_key <all, all>
  {
    public:
      data_erasor_no_key() {}
      
      template <typename object_type>
      void operator()(object_type const & el)
      {
        key_value_registration<object_type>::instance().erase_all_all(el);
      }
  };
  
  /** @brief Erase data associated with key 'key'. The object is obtained via a proxy class.
    * 
    * Should be called in the form 
    *   <pre>viennadata:erase<key_type, value_type>(my_key)(my_obj);</pre>
    * This would erase data stored using a key 'my_key' of type 'key_type' from 'my_obj'.
    *
    * \ingroup public_interface
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  data_erasor_with_key<key_type, value_type> erase(key_type const & key)
  {
    return data_erasor_with_key<key_type, value_type>(key);
  }

  /** @brief Erase all data associated with keys of 'key_type'.
    *
    * Should be called in the form 
    *   <pre>viennadata:erase<key_type, value_type>()(my_obj);</pre>
    * This would erase data stored using a key of type 'key_type' from 'my_obj'.
    * 
    * \ingroup public_interface
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  data_erasor_no_key<key_type, value_type> erase()
  {
    return data_erasor_no_key<key_type, value_type>();
  }


  
  //////////////////////// Move data //////////////////////////////////
  /** @brief A proxy class for moving data associated with a particular key from one object to another. Only used internally in ViennaData 
    *
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  class data_mover_with_key
  {
    public:
      data_mover_with_key(key_type const & key) : key_(key) {}
      
      template <typename object_src_type,
                typename object_dest_type>
      void operator()(object_src_type const & el_src,
                      object_dest_type const & el_dest)
      {
        //std::cout << "Get data from element" << std::endl;
        access<key_type, value_type>(key_)(el_dest) = access<key_type, value_type>(key_)(el_src);
        erase<key_type, value_type>(key_)(el_src);
      }
    private:
      key_type const & key_;
  };
  
  /** @brief Moves data associated with key 'key' from one object to another.
    *
    * Should be called in the form 
    *   <pre>viennadata:move<key_type, value_type>(key)(src_obj, dest_obj);</pre>
    * This would move data stored using a key 'key' of type 'key_type' from 'src_obj' to 'dest_obj'.
    * 
    * \ingroup public_interface
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  data_mover_with_key<key_type, value_type> move(key_type const & key)
  {
    return data_mover_with_key<key_type, value_type>(key);
  }
  
  
  /** @brief A proxy class for moving data associated with a particular key type from one object to another. Only used internally in ViennaData 
    *
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  class data_mover_no_key
  {
    public:
      data_mover_no_key() {}
      
      template <typename object_src_type, typename object_dest_type>
      void operator()(object_src_type const & el_src, object_dest_type const & el_dest)
      {
        data_container<key_type, value_type, object_src_type>::instance().copy(el_src, el_dest);
        data_container<key_type, value_type, object_src_type>::instance().erase(el_src);
      }
  };
  
  /** @brief Moves data associated with all keys of type 'key_type' from one object to another.
    *
    * Should be called in the form 
    *   <pre>viennadata:move<key_type, value_type>()(src_obj, dest_obj);</pre>
    * This would move data stored using a key of type 'key_type' from 'src_obj' to 'dest_obj'.
    * 
    * \ingroup public_interface
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  data_mover_no_key<key_type, value_type> move()
  {
    return data_mover_no_key<key_type, value_type>();
  }
  
  
  //////////////////////// Copy data //////////////////////////////////
  /** @brief A proxy class for copying data associated with a particular key from one object to another. Only used internally in ViennaData 
    *
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  class data_copy_with_key
  {
    public:
      data_copy_with_key(key_type const & key) : key_(key) {}
      
      template <typename object_src_type,
                typename object_dest_type>
      void operator()(object_src_type const & el_src,
                      object_dest_type const & el_dest)
      {
        access<key_type, value_type>(key_)(el_dest) = access<key_type, value_type>(key_)(el_src);
      }
    private:
      key_type const & key_;
  };
  
  //prevent messing around with all-type and supplied key:
  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <typename key_type>
  class data_copy_with_key<key_type, all>
  {
    typedef typename error_indicator<key_type>::ERROR_NO_KEY_ARGUMENT_ALLOWED_WHEN_USING_COPY_FUNCTION_WITH_ALL   error_type;
  };

  /** @brief Specialization that prevents supplying a key argument when using viennadata::all */
  template <typename value_type>
  class data_copy_with_key<all, value_type>
  {
    typedef typename error_indicator<value_type>::ERROR_NO_KEY_ARGUMENT_ALLOWED_WHEN_USING_COPY_FUNCTION_WITH_ALL   error_type;
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
    *   <pre>viennadata:copy<key_type, value_type>()(src_obj, dest_obj);</pre>
    * This would copy data stored using a key of type 'key_type' from 'src_obj' to 'dest_obj'.
    * 
    * \ingroup public_interface
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  data_copy_with_key<key_type, value_type> copy(key_type const & key)
  {
    return data_copy_with_key<key_type, value_type>(key);
  }
  
  
  /** @brief A proxy class for copying data associated with a particular key type from one object to another. Only used internally in ViennaData 
    *
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  class data_copy_no_key
  {
    public:
      data_copy_no_key() {}
      
      template <typename object_src_type, typename object_dest_type>
      void operator()(object_src_type const & el_src, object_dest_type const & el_dest)
      {
        data_container<key_type, value_type, object_src_type>::instance().copy(el_src, el_dest);
      }
  };
  
  /** @brief Specialization for copying all data associated with a particular key type from one object to another using viennadata::all */
  template <typename key_type>
  class data_copy_no_key <key_type, all>
  {
    public:
      data_copy_no_key() {}
      
      template <typename object_type>
      void operator()(object_type const & el_src, object_type const & el_dest)
      {
        key_value_registration<object_type>::instance().template copy_key_all<key_type>(el_src, el_dest);
      }
      
      template <typename object_src_type, typename object_dest_type>
      void operator()(object_src_type const & el_src, object_dest_type const & el_dest)
      {
        typedef typename error_indicator<object_src_type>::ERROR_SOURCE_AND_DESTINATION_MUST_BE_OF_SAME_TYPE_WHEN_USING_COPY_WITH_ALL  error_type;
      }
  };

  /** @brief Specialization for copying all data of a particular value_type from one object to another using viennadata::all for the key_type */
  template <typename value_type>
  class data_copy_no_key <all, value_type>
  {
    public:
      data_copy_no_key() {}
      
      template <typename object_type>
      void operator()(object_type const & el_src,
                      object_type const & el_dest)
      {
        key_value_registration<object_type>::instance().template copy_all_value<value_type>(el_src, el_dest);
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
      
      template <typename object_type>
      void operator()(object_type const & el_src,
                      object_type const & el_dest)
      {
        key_value_registration<object_type>::instance().copy_all_all(el_src, el_dest);
      }
      
      template <typename object_src_type, typename object_dest_type>
      void operator()(object_src_type const & el_src,
                      object_dest_type const & el_dest)
      {
        typedef typename error_indicator<object_src_type>::ERROR_SOURCE_AND_DESTINATION_MUST_BE_OF_SAME_TYPE_WHEN_USING_COPY_WITH_ALL  error_type;
      }
  };
  
  
  
  /** @brief Copies data associated with all keys of type 'key_type' from one object to another.
    *
    * Should be called in the form 
    *   <pre>viennadata:copy<key_type, value_type>()(src_obj, dest_obj);</pre>
    * This would copy data stored using a key of type 'key_type' from 'src_obj' to 'dest_obj'.
    * 
    * \ingroup public_interface
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type, typename value_type>
  data_copy_no_key<key_type, value_type> copy()
  {
    return data_copy_no_key<key_type, value_type>();
  }
  
  

  //////////// memory allocation for data ////////////////////
  
  /** @brief A proxy class for reserving memory needed for storing data associated with a particular key type. Only used internally in ViennaData 
    *
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type,
            typename value_type>
  class data_reservation_proxy
  {
    public:
      data_reservation_proxy(long num) : num_(num) {}
      
      template <typename object_type>
      void operator()(object_type const & e)
      {
        data_container<key_type, value_type, object_type>::instance().reserve(num_);
      }
    
    private:
      long num_;
  };
  

  /** @brief Reserves memory for storing data associated with a particular key type.
    *
    * Should be called in the form 
    *   <pre>viennadata:reserve<key_type, value_type>(num)(my_obj);</pre>
    * This would reserve memory for num objects with the same type as my_obj for data storage using a key of type 'key_type'. Object IDs have to be in the range 0...(num-1) then.
    * 
    * \ingroup public_interface
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    * @param  num            Number of objects for which memory should be reserved. Object IDs have to be in the range 0...(num-1) then.
    */
  template <typename key_type,
            typename value_type>
  data_reservation_proxy<key_type, value_type> reserve(long num)
  {
    return data_reservation_proxy<key_type, value_type>(num);
  }
  
  
  /////////// find sparse data /////////////
  /** @brief A proxy class for finding data associated with a particular key. Only used internally in ViennaData 
    *
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    */
  template <typename key_type, typename value_type>
  class data_find_proxy
  {
    public:
      data_find_proxy(key_type const & key) : key_(key) {}
      
      template <typename object_type>
      value_type * operator()(object_type const & el)
      {
        return data_container<key_type, value_type, object_type>::instance().find(el, key_);
      }
    
    private:
      key_type const & key_;
  };

  /** @brief Reserves memory for storing data associated with a particular key type.
    *
    * Should be called in the form 
    *   <pre>viennadata:find<key_type, value_type>(my_key)(my_obj);</pre>
    * This would search for data associated with my_obj using a key of type 'key_type'. If found, a pointer to the data is returned, otherwise NULL.
    * 
    * \ingroup public_interface
    * @tparam key_type       Type of the key
    * @tparam value_type     Type of the data
    * @param  key            The key object
    */
  template <typename key_type, typename value_type>
  data_find_proxy<key_type, value_type> find(key_type const & key)
  {
    return data_find_proxy<key_type, value_type>(key);
  }
  
} //namespace viennadata


#endif 

