/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 René Heinzl         rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha
     Copyright (c) 2005-2006 Michael Spevak
     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef DOMAIN_CONCEPTS_HH_ID
#define DOMAIN_CONCEPTS_HH_ID

// *** BOOST includes
//
#define BOOST_NO_STD_ITERATOR_TRAITS
#include <boost/concept_check.hpp>


// *** GSSE includes
//
#include "gsse/segment_concept.hpp"


namespace gsse
{
namespace concepts
{


template <class TC>
struct GSSEDomainConcept
{
  void constraints()
  {
    typedef typename domain_traits<TC>::segment_iterator segment_iterator;
    segment_iterator sit;

    typedef typename domain_traits<TC>::segment_t segment_type;


    TC tc;
    sit = tc.segment_begin();
    sit = tc.segment_end();
    segment_type s = *sit;


    typedef typename domain_traits<TC>::vertex_type vertex_type;
    
    vertex_type v;
    tc.get_point(v);

    typedef typename domain_traits<TC>::segment_on_vertex_iterator segment_on_vertex_iterator;

    segment_on_vertex_iterator sovit;
    
    segment_on_vertex_iterator sovit2(sit, v);

    boost::function_requires<GSSEIteratorConcept<segment_on_vertex_iterator> >();
    boost::function_requires<GSSERAIteratorConcept<segment_iterator> >();
    boost::function_requires<GSSESegmentConcept<segment_type> >();

  }
};

}
}


#endif
