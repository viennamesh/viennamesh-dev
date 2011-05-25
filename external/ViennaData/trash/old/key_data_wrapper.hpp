/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */

#ifndef VIENNADATA_KEY_DATA_WRAPPER_GUARD
#define VIENNADATA_KEY_DATA_WRAPPER_GUARD

#include <map>
#include <vector>
#include <list>
#include <set>
#include <iostream>
#include <algorithm>
#include <memory>

#include "viennadata/forwards.h"

namespace viennadata
{

  //purpose: type erase so that a particular QuantityManager is able to delete all quantities associated with a particular object
  //concepts used: type erasure, cf. Alexandrescu


  class key_interface
  {
    public:
      virtual ~key_interface() {};

      virtual bool operator==(key_interface const &) const = 0;
  };

  template <typename KeyType>
  class key_wrapper : public key_interface
  {
      typedef key_wrapper< KeyType >   SelfType;

    public:
      key_wrapper(){};
  
      bool operator==(key_interface const & rhs) const
      {
        return dynamic_cast< SelfType const *>( &rhs ) != NULL;
      }
  };


  template <typename QuanManType>
  class key_data_interface
  {
    public:
      virtual ~key_data_interface() {};

      //virtual void print() const = 0;
      virtual void release(QuanManType & src) const = 0;   //releases all data for that particular quantity-key pairing (considers types only!)
      virtual void eraseQuantity(QuanManType & src) const = 0;   //releases all data for that particular quantity-key pairing on a particular element
      virtual void copyTo(QuanManType & src, QuanManType & dest) const = 0;
      virtual void transferTo(QuanManType & src, QuanManType & dest) const = 0;

      virtual bool operator==(key_data_interface const &) const = 0;
      virtual bool sameKey(key_interface const &) const = 0;
  };


  template <typename QuanManType, typename QuanType, typename KeyType>
  class key_data_wrapper : public key_data_interface<QuanManType>
  {

    typedef key_data_wrapper<QuanManType, QuanType, KeyType>    SelfType;

    public:
      key_data_wrapper() : pKeyInterface( new key_wrapper<KeyType>() ) {} ;

      //void print() const {};

      void release(QuanManType & src) const
      {
        src.template releaseQuantity<KeyType, QuanType>();
      }

      void eraseQuantity(QuanManType & src) const
      {
        src.template eraseQuantity<KeyType, QuanType>();
      }

      void copyTo(QuanManType & src, QuanManType & dest) const
      {
        src.template copyPairTo<KeyType, QuanType>(dest);
      }

      void transferTo(QuanManType & src, QuanManType & dest) const
      {
        src.template transferPairTo<KeyType, QuanType>(dest);
      }
  
      //comparison with same type:
      bool operator==(key_data_interface<QuanManType> const & rhs) const
      {
        //std::cout << "Entering virtual comparison" << std::endl;
        //return rhs.compare(*this);
//         SelfType const *p_subclass = dynamic_cast< SelfType const *>( &rhs );
//         return p_subclass != NULL;
        return dynamic_cast< SelfType const *>( &rhs ) != NULL;
      }

      bool sameKey(key_interface const & rhs) const
      {
        return *pKeyInterface == rhs;
      }

    private:
      //wrapper for QuanType
      //wrapper for KeyType
      key_interface * pKeyInterface;

  };

} //namespace

#endif

