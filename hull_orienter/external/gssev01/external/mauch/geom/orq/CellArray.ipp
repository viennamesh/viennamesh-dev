// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2000-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file CellArray.ipp
  \brief A class for a cell array in N-D.
*/

#if !defined(__geom_CellArray_ipp__)
#error This file is an implementation detail of the class CellArray.
#endif

BEGIN_NAMESPACE_GEOM


//
// Memory usage.
//

  
template<int N, typename _Record, typename _MultiKey, typename _Key,
	 typename _MultiKeyAccessor>
inline
typename CellArray<N,_Record,_MultiKey,_Key,_MultiKeyAccessor>::SizeType
CellArray<N,_Record,_MultiKey,_Key,_MultiKeyAccessor>::
getMemoryUsage() const {
  SizeType usage = 0;
  for (typename DenseArray::const_iterator i = _cellArray.begin();
	i != _cellArray.end(); ++i) {
    usage += i->size() * sizeof(Record);
  }
  usage += _cellArray.size() * sizeof(Cell);
  return usage;
}


//
// Mathematical member functions
//


template<int N, typename _Record, typename _MultiKey, typename _Key,
	 typename _MultiKeyAccessor>
template<class OutputIterator>
inline
typename CellArray<N,_Record,_MultiKey,_Key,_MultiKeyAccessor>::SizeType
CellArray<N,_Record,_MultiKey,_Key,_MultiKeyAccessor>::
computeWindowQuery(OutputIterator iter, const BBox& window) const {
  //
  // Convert the multi-key to array index coordinates.
  //

  // A multi-index and the lower and upper bounds of the index window.
  MultiIndex mi, lo, hi;
  convertMultiKeyToIndices(window.getLowerCorner(), lo);
  convertMultiKeyToIndices(window.getUpperCorner(), hi);
  
  //  
  // Truncate the index window to lie within the cell array.
  //

  {
    for (int n = 0; n != N; ++n) {
      lo[n] = std::max(0, lo[n]);
      hi[n] = std::min(getExtents()[n] - 1, hi[n]);
    }
  }

  // The interior portion of the index window.
  BBox interior(lo + 1, hi - 1);

  // The number of records in the window.
  SizeType count = 0;
  typename Cell::const_iterator recordIter;
  typename Cell::const_iterator recordIterEnd;

  //
  // Iterate over the cells in the index window.
  //

  int n = N-1;
  mi = lo;
  while (mi[N-1] <= hi[N-1]) {
    if (n == 0) {
      for (mi[0] = lo[0]; mi[0] <= hi[0]; ++mi[0]) {
	// Iterate over the records in the cell.
	const Cell& cell = _cellArray(mi);
	recordIter = cell.begin();
	recordIterEnd = cell.end();

	// If this is an interior cell.
	if (interior.isIn(mi)) {
	  for (; recordIter != recordIterEnd; ++recordIter) {
	    // No need to check if it is in the window.
	    *iter = *recordIter;
	    ++iter;
	  }
	  count += SizeType(cell.size());
	}
	else {
	  for (; recordIter != recordIterEnd; ++recordIter) {
	    // If the record is in the window.
	    if (window.isIn(getMultiKey(*recordIter))) {
	      *iter = *recordIter;
	      ++iter;
	      ++count;
	    }
	  }
	}
      }
      ++n;
    }
    else if (mi[n-1] > hi[n-1]) {
      mi[n-1] = lo[n-1];
      ++mi[n];
      ++n;
    }
    else {
      --n;
    }
  }

  return count;
}


//
// File I/O
//


template<int N, typename _Record, typename _MultiKey, typename _Key,
	 typename _MultiKeyAccessor>
inline
void
CellArray<N,_Record,_MultiKey,_Key,_MultiKeyAccessor>::
put(std::ostream& out) const {
  Base::put(out);

  for (typename DenseArray::const_iterator i = _cellArray.begin();
	i != _cellArray.end(); ++i) {
    const Cell& b = *i;
    typename Cell::const_iterator iter(b.begin());
    while (iter != b.end()) {
      out << getMultiKey(*(iter++)) << '\n';
    }
  }
}
    
END_NAMESPACE_GEOM

// End of file.
