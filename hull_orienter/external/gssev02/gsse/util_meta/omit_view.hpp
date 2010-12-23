/* ============================================================================
   Copyright (c) 2010 Philipp Schwaha                           philipp@gsse.at        
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

// the meta functions omit_view and omit_view_c provide a view into a
// sequence omitting the item at a given index
//

#ifndef OMIT_VIEW_HPP
#define OMIT_VIEW_HPP

// *** boost::mpl includes
// 
#include <boost/mpl/advance.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/mpl/joint_view.hpp>

namespace gsse
{

// omit an element at an index provided as an integral constant (eg., boost::mpl::long_)
//
template<typename SequenceType, typename position>
struct omit_view
{
   typedef typename boost::mpl::begin<SequenceType>::type begin_part_1_iterator_t;
   typedef typename boost::mpl::advance<begin_part_1_iterator_t, position>::type end_part_1_iterator_t;

   typedef typename boost::mpl::iterator_range<
      begin_part_1_iterator_t,
      end_part_1_iterator_t
      >::type part_1_view_t;

   typedef typename boost::mpl::next<end_part_1_iterator_t>::type begin_part_2_iterator_t;
   typedef typename boost::mpl::end<SequenceType>::type end_part_2_iterator_t;

   typedef typename boost::mpl::iterator_range<
      begin_part_2_iterator_t,
      end_part_2_iterator_t
      >::type part_2_view_t;

   typedef typename boost::mpl::joint_view<part_1_view_t, part_2_view_t>::type type;
};


// omit an element at an index provided as a number
//
template<typename SequenceType, size_t position>
struct omit_view_c
{
   typedef typename boost::mpl::begin<SequenceType>::type begin_part_1_iterator_t;
   typedef typename boost::mpl::advance_c<begin_part_1_iterator_t, position>::type end_part_1_iterator_t;

   typedef typename boost::mpl::iterator_range<
      begin_part_1_iterator_t,
      end_part_1_iterator_t
      >::type part_1_view_t;

   typedef typename boost::mpl::next<end_part_1_iterator_t>::type begin_part_2_iterator_t;
   typedef typename boost::mpl::end<SequenceType>::type end_part_2_iterator_t;

   typedef typename boost::mpl::iterator_range<
      begin_part_2_iterator_t,
      end_part_2_iterator_t
      >::type part_2_view_t;

   typedef typename boost::mpl::joint_view<part_1_view_t, part_2_view_t>::type type;
};

template<typename SequenceType>
struct omit_view_c<SequenceType, 0>
{
   typedef typename boost::mpl::next<typename boost::mpl::begin<SequenceType>::type>::type begin_iterator_t;
   typedef typename boost::mpl::end<SequenceType>::type end_iterator_t;

   typedef typename boost::mpl::iterator_range<
      begin_iterator_t,
      end_iterator_t
      >::type type;
};

}

#endif // OMIT_VIEW_HPP

