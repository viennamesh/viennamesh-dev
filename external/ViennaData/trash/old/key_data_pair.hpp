/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */

#ifndef VIENNADATA_KEY_DATA_PAIR_GUARD
#define VIENNADATA_KEY_DATA_PAIR_GUARD

#include <map>
#include <vector>
#include <list>
#include <set>
#include <iostream>
#include <algorithm>
#include <memory>

#include "viennadata/forwards.h"
#include "viennadata/key_data_wrapper.hpp"

namespace viennadata
{

  template <typename QuanManType>
  class key_data_pair
  {
    public:

      template <typename QuanType, typename KeyType>
      key_data_pair(QuanType const & qt, KeyType const & kt) : pQuanInterface(new key_data_wrapper<QuanManType, QuanType, KeyType>()),
                                                             isOwner(true) {}

      //copy ctor:
      key_data_pair(const key_data_pair & other)
      {
        //std::cout << "key_data_pair copy constructor called for " << this << " with other=" << &other << "!" << std::endl;
        pQuanInterface = other.pQuanInterface;
        isOwner = true;
        other.isOwner = false;
      }

      ~key_data_pair()
      {
        if (isOwner)
        {
          delete pQuanInterface;
          //std::cout << "Deleting pQuanInterface" << std::endl;
        }
      }


      bool operator==(key_data_pair<QuanManType> const & other) const
      {
        //std::cout << "Comparing in key_data_pair" << std::endl;
        return *pQuanInterface == *(other.pQuanInterface);
      }

      void release(QuanManType & src) { pQuanInterface->release(src); }
      void eraseQuantity(QuanManType & src) { pQuanInterface->eraseQuantity(src); }

      void copyTo(QuanManType & src, QuanManType & dest) { pQuanInterface->copyTo(src, dest); }

      void transferTo(QuanManType & src, QuanManType & dest) { pQuanInterface->transferTo(src, dest); }

      bool sameKey(key_interface const & rhs) const
      {
        return pQuanInterface->sameKey(rhs);
      }

    private:
      key_data_interface<QuanManType> * pQuanInterface;
      mutable bool isOwner;
  };
  
} //namespace

#endif

