/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef QUANTITY_CONCEPTS_HH_ID
#define QUANTITY_CONCEPTS_HH_ID

// *** BOOST includes
//
#define BOOST_NO_STD_ITERATOR_TRAITS
#include <boost/concept_check.hpp>

// *** GSSE includes
// 
#include "gsse/quantity_traits.hpp"



namespace gsse
{
namespace concepts
{

template <class TT>
struct GSSEQuantityIteratorConcept
{
  void constraints() 
  {
     // [RH][TODO] .. this style is deprectated .. but as I can see, boost::concept check 
     //  does not provide it as before.. only on newer compilers??
     //
     //boost::function_requires< boost::BidirectionalIteratorConcept<TT> >();
  }
};

template <class QK>
struct GSSEQuantityKeyConcept
{
  void constraints()
  {
     // [RH][TODO] .. this style is deprectated .. but as I can see, boost::concept check 
     //  does not provide it as before.. only on newer compilers??
     //
//     boost::function_requires< boost::AssignableConcept<QK> >();
//     boost::function_requires< boost::EqualityComparableConcept<QK> >();
//     boost::function_requires< boost::LessThanComparableConcept<QK> >();
  }
};

template <class QKI>
struct GSSEQuantityKeyIteratorConcept
{
  void constraints() 
  {
    boost::function_requires< GSSEQuantityIteratorConcept<QKI> >();    
  }
};

template <class TT>
struct GSSEQuantityHandleIteratorConcept
{
  void constraints() 
  {
    boost::function_requires< GSSEQuantityIteratorConcept<TT> >();    

    TT iter;

    bool b = iter.valid();
    boost::ignore_unused_variable_warning(b);
    iter.reset();
  }
};


template <class QC>
struct GSSEQuantityConcept
{
  void constraints() 
  {
    QC qc;
    unsigned int i = qc.object_size();
    i = qc.key_size();

    typedef typename quantity_traits<QC>::key_iterator key_iterator;
    
    key_iterator ki;

    ki = qc.key_begin();
    ki = qc.key_end();
    
    boost::function_requires< GSSEQuantityKeyIteratorConcept< key_iterator> >();   
    
    typedef typename quantity_traits<QC>::handle_iterator handle_iterator;

    handle_iterator hi;

    boost::function_requires< GSSEQuantityHandleIteratorConcept< handle_iterator> >();   

    hi = qc.handle_begin();
    hi = qc.handle_end();

     // [RH][TODO] .. this style is deprectated .. but as I can see, boost::concept check 
     //  does not provide it as before.. only on newer compilers??
     //
    // boost::function_requires< boost::DefaultConstructibleConcept<typename QC::storage_type> >();
    boost::function_requires< GSSEQuantityKeyConcept<typename QC::object_handle_type> >();   
    boost::function_requires< GSSEQuantityKeyConcept<typename QC::quan_key_type> >();   

    typedef typename quantity_traits<QC>::storage_type storage_type;
    typedef typename quantity_traits<QC>::object_handle_type object_handle_type;
    typedef typename quantity_traits<QC>::quan_key_type quan_key_type;

     // [RH][TODO] .. this style is deprectated .. but as I can see, boost::concept check 
     //  does not provide it as before.. only on newer compilers??
     //
    //boost::function_requires< boost::DefaultConstructibleConcept<storage_type> >();   
    boost::function_requires< GSSEQuantityKeyConcept<object_handle_type> >();   
    boost::function_requires< GSSEQuantityKeyConcept<quan_key_type> >();   

    QC const qc2;

    storage_type m;
    object_handle_type o;
    quan_key_type q;

    qc.insert(o, q, m);
    qc.insert(q, m);

    m = qc.retrieve_quantity(o, q);
    m = qc2.retrieve_quantity(o, q);

    m = qc(o, q);
    m = qc2(o, q);
  }
};

}
}

#endif
