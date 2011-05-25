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

#ifndef VIENNADATA_FORWARDS_GUARD
#define VIENNADATA_FORWARDS_GUARD

#include <map>
#include <vector>
#include <list>
#include <set>
#include <iostream>
#include <algorithm>
#include <memory>



namespace viennadata
{
  
    ////////////////////// tags ////////////////////////
        //forward declaration for disabling storage banks:
    struct no_storage_bank_tag;


    //storage policy tag:
    struct dense_data_tag {};      //for a property that is accessed on every element *and* is required to be fast
    struct sparse_data_tag {};     //properties that will be accessed on some elements only. Default policy.

    
    //key dispatch based on type only
    struct full_key_dispatch_tag {};
    struct type_key_dispatch_tag {};

    
    // add further performance-critical keys here


    /////////////////// forward declarations ////////////////////

    struct no_id;
    struct has_id;

    template <typename QuanManType>
    class key_data_pair;

    template <typename QuanManType, typename QuanType, typename KeyType>
    struct key_data_wrapper;

    template <typename ElementType, typename IDHandler, typename StorageBankType>
    class SingleBankQuantityManager;

    template <typename ElementType,
              typename IDHandler,
              typename StorageBankType = no_storage_bank_tag >
    class QuantityManager;


    template <typename ElementType,
              typename QuanType,
              typename KeyType,
              typename KeyDispatchTag>
    class QuanMan_data_holder;

    template <typename ElementType,
              typename QuanType,
              typename KeyType,
              typename StorageTag,
              typename AccessTag >
    class QuanMan_element_dispatcher;

    template <typename ElementType, typename StorageBankType>
    class StorageBankHolder;
    
    
    
    /////////////////// config /////////////////////
    

    template <typename KeyType>
    struct QuantityManagerStorageScheme
    {
      typedef sparse_data_tag        result_type;
    };


    //convenience function: deduces storage scheme from KeyType and IDHandling
    template <typename KeyType, typename IDHandler>
    struct QuantityManagerFinalStorageScheme
    {
      //default policy: use StorageScheme
      typedef typename QuantityManagerStorageScheme<KeyType>::result_type      result_type;
    };

    template <typename KeyType>
    struct QuantityManagerFinalStorageScheme <KeyType, no_id>
    {
      //no id: fall back to SpareStorageTag
      typedef sparse_data_tag         result_type;
    };


    template <typename T>
    struct SetKeyDispatch
    {
      typedef full_key_dispatch_tag    result_type;
    };

    // partial specialisations of the form
    // template <>
    // struct SetKeyDispatch< MyKeyClass >
    // {
    //   typedef TypeBasedDispatch    result_type;
    // };
    // to be supplied by user
  
} //namespace

#endif
