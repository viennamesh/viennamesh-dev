/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */


  /******************* ID Handling *******************************/

#ifndef VIENNADATA_KEY_VALUE_REGISTRATION_HPP
#define VIENNADATA_KEY_VALUE_REGISTRATION_HPP

#include <assert.h>
#include <map>
#include <vector>
#include <list>
#include <set>
#include <iostream>
#include <algorithm>
#include <memory>

#include "viennadata/key_value_pair.hpp"
#include "viennadata/vlog.hpp"


/** @file viennadata/key_value_registration.hpp
    @brief In this file all pairs of (key-type, data-type) are stored in order to support operations using viennadata::all.
*/


namespace viennadata
{
  /** @brief This class holds (key-type, data-type) pairs. Uses a singleton pattern to 
   *
   * @tparam object_type  Type of the object for which the data is stored
   */
  template <typename object_type>
  class key_value_registration
  {
      typedef std::vector<key_value_pair<object_type> >       container_type;

      /** @brief Private CTOR due to singleton pattern */
      key_value_registration() {}
      /** @brief Private copy CTOR due to singleton pattern */
      key_value_registration(key_value_registration const &) {}
      /** @brief Private DTOR due to singleton pattern */
      ~key_value_registration() {}
      
    public:

      /** @brief Returns the only instance of that type (singleton pattern) */
      static key_value_registration & instance()
      {
        static key_value_registration * kvr = NULL;
        if (kvr == NULL)
          kvr = new key_value_registration();

        return *kvr;
      }

      /** @brief Adds a new key-value type pair (if not already existing) */
      template <typename key_type,
                typename value_type>
      void add()
      {
        key_value_pair<object_type> kve;
        kve.template add<key_type, value_type>();

        //search whether already registered:
        if (std::find(container.begin(), container.end(), kve) == container.end())
          container.push_back(kve);
      }

/*      template <typename key_type, typename value_type>
      void remove()
      {
        typedef typename container_type::iterator   ContIter;

        key_value_pair<object_type> kve;
        kve.template add<key_type, value_type>();

        ContIter ci = std::find(container.begin(), container.end(), kve);
        if (ci != container.end())
          container.erase(ci);
      }*/

      //////////////////////// erase ////////////////////////////////////
      /** @brief Erases all data associated with the provided object for a particular key */
      template <typename key_type>
      void erase_key_all(object_type const & obj) const
      {
        typedef typename container_type::const_iterator   ContIter;

        viennadata::vlog << "key_value_registration::erase<key, all> called!" << std::endl;

        key_value_pair<object_type> kve;
        kve.template add<key_type, all>();

        for (ContIter it = container.begin();
             it != container.end();
             ++it)
        {
          if (*it == kve)
          {
            viennadata::vlog << "FOUND!" << std::endl;
            it->erase(obj);
          }
          else
            viennadata::vlog << "not found!" << std::endl;
        }
      }

      /** @brief Erases all data of a particular value type, no matter which key has been used */
      template <typename value_type>
      void erase_all_value(object_type const & obj) const
      {
        typedef typename container_type::const_iterator   ContIter;

        viennadata::vlog << "key_value_registration::erase<all, value> called!" << std::endl;

        key_value_pair<object_type> kve;
        kve.template add<all, value_type>();

        for (ContIter it = container.begin();
             it != container.end();
             ++it)
        {
          if (*it == kve)
          {
            std::cout << "FOUND!" << std::endl;
            it->erase(obj);
          }
          else
            viennadata::vlog << "not found!" << std::endl;
        }
      }

      /** @brief Erase all data associated with the provided object */
      void erase_all_all(object_type const & obj) const
      {
        typedef typename container_type::const_iterator   ContIter;

        viennadata::vlog << "key_value_registration::erase<all, all> called!" << std::endl;

        for (ContIter it = container.begin();
             it != container.end();
             ++it)
        {
            it->erase(obj);
        }
      }


      //////////////////////// copy ////////////////////////////////////
      /** @brief Copies all data associated with particular keys of the supplied type to another object
       * 
       * @param src    The source object
       * @param dest   The destination object
       */
      template <typename key_type>
      void copy_key_all(object_type const & src,
                        object_type const & dest) const
      {
        typedef typename container_type::const_iterator   ContIter;

        viennadata::vlog << "key_value_registration::copy<key, all> called!" << std::endl;

        key_value_pair<object_type> kve;
        kve.template add<key_type, all>();

        for (ContIter it = container.begin();
             it != container.end();
             ++it)
        {
          if (*it == kve)
          {
            viennadata::vlog << "FOUND!" << std::endl;
            it->copy(src, dest);
          }
          else
          viennadata::vlog << "not found!" << std::endl;
        }
      }

      /** @brief Copy all data of a certain type to another object, no matter which key was used
       * 
       * @param src    The source object
       * @param dest   The destination object
       */
      template <typename value_type>
      void copy_all_value(object_type const & src,
                          object_type const & dest) const
      {
        typedef typename container_type::const_iterator   ContIter;

        viennadata::vlog << "key_value_registration::copy<all, value> called!" << std::endl;

        key_value_pair<object_type> kve;
        kve.template add<all, value_type>();

        for (ContIter it = container.begin();
             it != container.end();
             ++it)
        {
          if (*it == kve)
          {
            viennadata::vlog << "FOUND!" << std::endl;
            it->copy(src, dest);
          }
          else
            viennadata::vlog << "not found!" << std::endl;
        }
      }

      /** @brief Copies all data to another object
       * 
       * @param src    The source object
       * @param dest   The destination object
       */
      void copy_all_all(object_type const & src,
                        object_type const & dest) const
      {
        typedef typename container_type::const_iterator   ContIter;

        viennadata::vlog << "key_value_registration::copy<all, all> called!" << std::endl;

        for (ContIter it = container.begin();
             it != container.end();
             ++it)
        {
            it->copy(src, dest);
        }
      }


    private:
      container_type container;
  };

}

#endif
