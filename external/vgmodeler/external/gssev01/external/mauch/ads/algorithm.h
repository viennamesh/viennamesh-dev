// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
// ----------------------------------------------------------------------------

/*! 
  \file ads/algorithm.h
  \brief Includes the algorithm files.
*/

/*!
  \page ads_algorithm Algorithm Package

  The algorithm package defines 
  \ref algorithm_min_max "min and max functions"
  for three, four or 
  five arguments.  They have the same kind of interface as std::min(), 
  which returns the minimum of two arguments,  For example:
  - ads::min(const T& a, const T& b, const T& c)
  - ads::min(const T& a, const T& b, const T& c, Compare comp)

  It also defines the 
  \ref algorithm_sign "ads::sign() function".

  There are functions for \ref algorithm_sort "sorting and ranking".

  I have implementations of the 
  \ref algorithm_is_sorted "is_sorted() functions".  I implement them here 
  to avoid dependence on the SGI extensions.

  There are \ref ads_algorithm_skipElements "functions" for skipping elements
  in a sequence.

  The \ref algorithm_insertion_sort "insertion_sort()" functions are useful
  for sorting nearly sorted sequences.

  The \ref algorithm_statistics "statistics" functions compute the minimum,
  maximum and mean of a range of elements.

  The \ref algorithm_unique "areElementsUnique()" functions determine if 
  the unsorted elements in a range are unique.

  The ads::Triplet class is analogous to std::pair.  The ads::OrderedPair
  class is a pair in which the first element precedes the second.

  There are \ref algorithm_extremeElement "functions" to find the extreme 
  element (minimum or maximum) in a sequence.
*/

#if !defined(__ads_algorithm_h__)
#define __ads_algorithm_h__

#include "algorithm/extremeElement.h"
#include "algorithm/min_max.h"
#include "algorithm/sign.h"
#include "algorithm/is_sorted.h"
#include "algorithm/insertion_sort.h"
#include "algorithm/OrderedPair.h"
#include "algorithm/skipElements.h"
#include "algorithm/sort.h"
#include "algorithm/statistics.h"
#include "algorithm/Triplet.h"
#include "algorithm/unique.h"

#endif

// End of file
