/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */


#ifndef VIENNADATA_STORAGEBANK_GUARD
#define VIENNADATA_STORAGEBANK_GUARD

#include <map>
#include <vector>
#include <list>
#include <set>
#include <iostream>
#include <algorithm>
#include <memory>

#include "forwards.h"

namespace viennadata
{



    //default interface: provide storage banks:
    template <typename QuanMan, typename ElementType, typename StorageBankType>
    class StorageBankInterface : public StorageBankHolder<ElementType, StorageBankType>
    {
        typedef StorageBankHolder<ElementType, StorageBankType>    BaseType;

      public:
        typedef std::list< key_data_pair<QuanMan> > ContainerType;

        void removeStorageBank(StorageBankType & bank)
        {
          typedef typename ContainerType::iterator      ListIterator;

          const StorageBankType & oldBank = BaseType::getCurrentBank();
          setStorageBank(bank);

          //iterate over all (key,quantity)-pairs:
          for (ListIterator lit  = quankeylist.begin();
                            lit != quankeylist.end();
                          ++lit)
          {
            //std::cout << "Releasing pair..." << std::endl;
            lit->releasePair(*this);
          }

          setStorageBank(oldBank);
        }
      protected:
        static ContainerType quankeylist; //stores all registered pairs of quantities and keys


    };

    template <typename QuanMan, typename ElementType, typename StorageScheme>
    std::list< key_data_pair<QuanMan> > StorageBankInterface<QuanMan, ElementType, StorageScheme>::quankeylist = std::list< key_data_pair<QuanMan> > ();

    //default case: Quantity-Manager is working on a storage bank basis:
    template <  typename ElementType,
                typename IDHandler,
                typename StorageBankType,
                typename QuanType, 
                typename KeyType >
    class QuantityManagerSelector
    {
        typedef SingleBankQuantityManager<  ElementType,
                                            IDHandler, 
                                            StorageBankType >         SingleBankQMType;

      public:
        typedef QuanMan_element_dispatcher<SingleBankQMType,
                                           QuanType,
                                           KeyType,
                                           typename SetKeyDispatch<KeyType>::result_type,
                                           typename QuantityManagerFinalStorageScheme<KeyType, IDHandler>::result_type
                                          >                            QuanManType;

        typedef typename std::map < StorageBankType *, QuanManType >::iterator   StorageBankIterator;

        QuanManType & getQuantityManager()
        {
          return bank_to_qm[StorageBankHolder<ElementType, StorageBankType>::pBank];
        }

        void eraseQuanMan(StorageBankType const & bank)
        {
          bank_to_qm.erase(&bank);
        }

      private:
        std::map< const StorageBankType *,
                  QuanManType       >       bank_to_qm;
    };


    template <typename ElementType, typename STLContainerType>
    class DereferencingIterator : public std::iterator < std::forward_iterator_tag, ElementType >
    {
        typedef typename STLContainerType::iterator     STLIterator;
  
      public:
        DereferencingIterator(STLIterator const & it) : stl_iterator(it) {}
  
        ElementType & operator*() const { return **stl_iterator; }
        ElementType * operator->() const { return *stl_iterator; }
  
        DereferencingIterator & operator++() { ++stl_iterator; return *this; }
        DereferencingIterator & operator++(int) { DereferencingIterator tmp = *this; ++*this; return tmp; }
  
        bool operator==(DereferencingIterator const & other) const { return stl_iterator == other.stl_iterator; }
        bool operator!=(DereferencingIterator const & other) const { return stl_iterator != other.stl_iterator; }
  
      private:
        STLIterator stl_iterator;
    };

    //the default bank holder:
    template <typename ElementType, typename StorageBankType>
    class StorageBankHolder
    {
        typedef DereferencingIterator<const StorageBankType, std::set < const StorageBankType * > >   StorageBankIterator;

      public:
        //provide public iterator typedef:
        typedef StorageBankIterator   iterator;
  
        //extension for threads: wrap in thread_id/segment-map
        static const StorageBankType & getCurrentBank() { return *pBank; }
  
        static StorageBankIterator begin()
        {
          return StorageBankIterator(storage_banks.begin());
        }

        static StorageBankIterator end()
        {
          return StorageBankIterator(storage_banks.end());
        }

        static void setStorageBank(StorageBankType const & bank)
        {
          if (storage_banks.find(&bank) == storage_banks.end())
            storage_banks.insert(&bank);
          pBank = &bank;
        }


      private:
        template <  typename QM_ElementType,
                    typename IDHandler,
                    typename QM_StorageBankType,
                    typename QuanType, 
                    typename KeyType >
        friend class QuantityManagerSelector;

//         static void setBank(StorageBankType const & bank)
//         {
//         }

        static std::set < StorageBankType const * > storage_banks;
        //the current segment
        static const StorageBankType * pBank;
    };

    template <typename ElementType, typename StorageBankType>
    std::set < StorageBankType const * > StorageBankHolder<ElementType, StorageBankType>::storage_banks = std::set < StorageBankType const * > ();

    template <typename ElementType, typename StorageBankType>
    const StorageBankType * StorageBankHolder<ElementType, StorageBankType>::pBank = 0;


    //specialisation: Do not use storage banks:
    template <  typename ElementType,
                typename IDHandler,
                typename QuanType, 
                typename KeyType    >
    class QuantityManagerSelector <ElementType, IDHandler, no_storage_bank_tag, QuanType, KeyType>
    {
        typedef SingleBankQuantityManager<  ElementType,
                                            IDHandler, 
                                            no_storage_bank_tag >         SingleBankQMType;

      public:
        typedef QuanMan_element_dispatcher<SingleBankQMType,
                                           QuanType,
                                           KeyType,
                                           typename SetKeyDispatch<KeyType>::result_type,
                                           typename QuantityManagerFinalStorageScheme<KeyType, IDHandler>::result_type
                                          >                            QuanManType;

        QuanManType & getQuantityManager()
        {
          return qm;
        }

        void eraseQuanMan(no_storage_bank_tag const & dummy)
        {
          //do nothing, because there is only one qm
        }

      private:
        QuanManType qm;
    };

}

#endif
