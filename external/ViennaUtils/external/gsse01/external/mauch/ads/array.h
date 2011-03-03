// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

#if !defined(__ads_array_h__)
#define __ads_array_h__

// CONTINUE Add documentation for SparseArray and StaticArrayOfArrays.

/*! 
  \file array.h
  \brief Includes the array classes.
*/

/*!
  \page ads_array Array Package

  The array package has several array classes:
  - ads::FixedArray has the array size and the element type as template 
    parameters.  It is used to represent Cartesian points in several packages.
  - ads::Array is an N-D, dynamically-sized array.  The dimension and
    the element type are specified as template parameters.
    It may either allocate its own memory or wrap externally allocated memory.
  - ads::SparseArray is an N-D dynamically-sized, sparse array.  For 1-D 
    (\ref SparseArray1 "SparseArray<1,T>") it 
    uses index compression.  For higher dimensions 
    (\ref SparseArray2 "SparseArray<2,T>") 
    it uses index/offset compression.
  - ads::ArrayWithNullHoles is a dynamically-sized, 1-D array with holes.
    It supports efficient deletion of elements and insertion of elements 
    into holes.
  - ads::StaticArrayOfArrays is a static array of arrays (no, really).  
    It is an efficient way to represent static, sparse, 2-D arrays.

  The ads::IndexRange class is used to specify the index ranges of arrays.

  The array classes are STL-compliant containers.  (Consult
  "Generic Programming and the STL" by Matthew H. Austern.)  Such containers
  define the following nine types:
  - \c value_type
  - \c pointer
  - \c const_pointer
  - \c reference
  - \c const_reference
  - \c iterator
  - \c const_iterator
  - \c difference_type
  - \c size_type
  .
  STL-compliant containers provide accessors to the beginning and 
  end of the range of elements:
  - <tt>const_iterator begin() const</tt>
  - <tt>const_iterator end() const</tt>
  .
  as well as the corresponding manipulators:
  - <tt>iterator begin()</tt>
  - <tt>iterator end()</tt>
  .
  There are three member functions relating to the number of elements
  in the container.
  - <tt>size_type size() const</tt>
  - <tt>bool empty() const</tt>
  - <tt>size_type max_size() const</tt>
  .
  Finally there is a member function for swapping the contents of two 
  containers.
  - <tt>void swap(container&)</tt>

  Each array class supports the
  standard mathematical operations.  The ads::FixedArray class does not 
  share implementation with the dynamic arrays.

  There are two policy classes which implement functionality for ads::Array:
  - ads::ArrayContainer manages the array size, the beginning and the end of
    the element storage and container indexing.  This class either allocates
    memory or references external memory.  The functions that 
    operate on the array elements are implemented in this class.
  - ads::ArrayIndexing defines the index ranges, indexing, and file output.
*/

#include "array/FixedArray.h"
#include "array/Array.h"
#include "array/ArrayWithNullHoles.h"
#include "array/IndexIterator.h"
#include "array/SparseArray.h"
#include "array/StaticArrayOfArrays.h"

#endif

// End of file.
