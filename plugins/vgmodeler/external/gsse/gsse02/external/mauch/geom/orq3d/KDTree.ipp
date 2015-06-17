// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2000-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file KDTree.ipp
  \brief A class for a kd-tree in 3-D.
*/

#if !defined(__geom_KDTree_ipp__)
#error This file is an implementation detail of the class KDTree.
#endif

BEGIN_NAMESPACE_GEOM

//-----------------------------KDTreeBranch-------------------------------
  
//
// Constructors
//
  
// Construct from a set of grid elements.
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
KDTreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
KDTreeBranch(const std::vector<value_type>& xsorted,
	      const std::vector<value_type>& ysorted,
	      const std::vector<value_type>& zsorted, 
	      const size_type leaf_size)
{
  assert(size_type(xsorted.size()) > leaf_size);
  assert(xsorted.size() == ysorted.size() && 
	  ysorted.size() == zsorted.size());

  //
  // Determine the splitting direction.
  //
      
  key_type xspread = xsorted.back()->multi_key()[0] 
    - xsorted.front()->multi_key()[0];
  key_type yspread = ysorted.back()->multi_key()[1] 
    - ysorted.front()->multi_key()[1];
  key_type zspread = zsorted.back()->multi_key()[2] 
    - zsorted.front()->multi_key()[2];
  if (xspread > yspread && xspread > zspread) {
    _split_dimension = 0;
  }
  else if (yspread > xspread && yspread > zspread) {
    _split_dimension = 1;
  }
  else {
    _split_dimension = 2;
  }
  // Name the other two dimensions.
  int second_dimension = (_split_dimension + 1) % 3;
  int third_dimension = (_split_dimension + 2) % 3;
      
  //
  // Name the input vectors.
  //
      
  const std::vector<value_type>* sv[3];
  sv[0] = &xsorted;
  sv[1] = &ysorted;
  sv[2] = &zsorted;

  const std::vector<value_type>& split_sorted = *sv[_split_dimension];
  const std::vector<value_type>& second_sorted = *sv[second_dimension];
  const std::vector<value_type>& third_sorted = *sv[third_dimension];
     
  //
  // Compute the median.
  //

  const int median_index = xsorted.size() / 2;
  const size_type left_size = median_index;
  const size_type right_size = xsorted.size() - median_index;
  const multi_key_type& median_point 
    = split_sorted[ median_index ]->multi_key();
  _split_value = median_point[_split_dimension];

  //
  // Vectors for the subtrees.
  //
  std::vector<value_type> sub[3];
  sub[0].reserve(right_size);
  sub[1].reserve(right_size);
  sub[2].reserve(right_size);
  typename std::vector<value_type>::const_iterator iter;

  //
  // Make the left subtree.
  //

  std::copy(split_sorted.begin(), split_sorted.begin() + median_index, 
	     back_inserter(sub[_split_dimension]));

  for (iter = second_sorted.begin(); iter != second_sorted.end(); 
	++iter) {
    if (ads::less_composite_compare(_split_dimension, 
				      (*iter)->multi_key(), median_point)) {
      sub[second_dimension].push_back(*iter);
    }
  }

  for (iter = third_sorted.begin(); iter != third_sorted.end(); 
	++iter) {
    if (ads::less_composite_compare(_split_dimension, 
				      (*iter)->multi_key(), median_point)) {
      sub[third_dimension].push_back(*iter);
    }
  }

  // If the left subtree is a leaf.
  if (left_size <= leaf_size) {
    std::vector<value_type> left_leaf(split_sorted.begin(), 
				       split_sorted.begin() 
				       + left_size);
    _left = new LeafType(left_leaf);
  }
  else {
    _left = new KDTreeBranch(sub[0], sub[1], sub[2], leaf_size);
  }

  sub[0].clear();
  sub[1].clear();
  sub[2].clear();

  //
  // Make the right subtree.
  //

  std::copy(split_sorted.begin() + median_index, split_sorted.end(),
	     back_inserter(sub[_split_dimension]));

  for (iter = second_sorted.begin(); iter != second_sorted.end(); 
	++iter) {
    if (! ads::less_composite_compare(_split_dimension, (*iter)->multi_key(),
					median_point)) {
      sub[second_dimension].push_back(*iter);
    }
  }

  for (iter = third_sorted.begin(); iter != third_sorted.end(); 
	++iter) {
    if (! ads::less_composite_compare(_split_dimension, (*iter)->multi_key(),
					median_point)) {
      sub[third_dimension].push_back(*iter);
    }
  }

  // If the right subtree is a leaf.
  if (right_size <= leaf_size) {
    std::vector<value_type> 
      right_leaf(split_sorted.begin() + median_index, 
		  split_sorted.end());
    _right = new LeafType(right_leaf);
  }
  else {
    _right = new KDTreeBranch(sub[0], sub[1], sub[2], leaf_size);
  }
}

//
// Mathematical member functions
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
typename KDTreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
size_type
KDTreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
window_query(OutputIterator iter, const bbox_type& window) const
{
  if (_split_value < window.getLowerCorner()[_split_dimension]) {
    return _right->window_query(iter, window);
  }
  else if (_split_value > window.getUpperCorner()[_split_dimension]) {
    return _left->window_query(iter, window);
  }
  return (_left->window_query(iter, window) +
	   _right->window_query(iter, window));
}

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
typename KDTreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
size_type
KDTreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
window_query(OutputIterator iter, bbox_type& domain, const bbox_type& window)
  const
{
  size_type count = 0;

  // If the domain of the left sub-tree intersects the window.
  if (_split_value >= window.getLowerCorner()[_split_dimension]) {
    // Make the domain of the left sub-tree.
    key_type max = domain.getUpperCorner()[_split_dimension];
    domain.setUpperCoordinate(_split_dimension, _split_value);

    // If the domain lies inside the window.
    if (window.isIn(domain)) {
      // Report the records in the left sub-tree.
      count += _left->report(iter);
    }
    else {
      // Do a window query of the left sub-tree.
      count += _left->window_query(iter, domain, window);
    }

    // Reset the domain.
    domain.setUpperCoordinate(_split_dimension, max);
  }

  // If the domain of the right sub-tree intersects the window.
  if (_split_value <= window.getUpperCorner()[_split_dimension]) {
    // Make the domain of the right sub-tree.
    key_type min = domain.getLowerCorner()[_split_dimension];
    domain.setLowerCoordinate(_split_dimension, _split_value);

    // If the domain lies inside the window.
    if (window.isIn(domain)) {
      // Report the records in the right sub-tree.
      count += _right->report(iter);
    }
    // If the domain intersects the window.
    else {
      // Do a window query of the right sub-tree.
      count += _right->window_query(iter, domain, window);
    }

    // Reset the domain.
    domain.setLowerCoordinate(_split_dimension, min);
  }

  return count;
}

//
// Validity check.
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
void
KDTreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
check(const bbox_type& window) const
{
  assert(window.getLowerCorner()[_split_dimension] <= _split_value &&
	  _split_value <= window.getUpperCorner()[_split_dimension]);

  bbox_type win(window);
  win.setUpperCoordinate(_split_dimension, _split_value);
  _left->check(win);

  win = window;
  win.setLowerCoordinate(_split_dimension, _split_value);
  _right->check(win);
}

//-----------------------------KDTreeLeaf-------------------------------

//
// Mathematical member functions
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
typename KDTreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::size_type
KDTreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::
report(OutputIterator iter) const
{
  for (const_iterator i = _records.begin(); i != _records.end(); ++i) {
    *(iter++) = (*i);
  }
  return _records.size();
}
  
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
typename KDTreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::size_type
KDTreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::
window_query(OutputIterator iter, const bbox_type& window) const
{
  //
  // copy the window into keys for faster access.
  //
  const key_type window_xmin = window.getLowerCorner()[0];
  const key_type window_ymin = window.getLowerCorner()[1];
  const key_type window_zmin = window.getLowerCorner()[2];
  const key_type window_xmax = window.getUpperCorner()[0];
  const key_type window_ymax = window.getUpperCorner()[1];
  const key_type window_zmax = window.getUpperCorner()[2];

  size_type count = 0;
  const_iterator records_end = _records.end(); 
  for (const_iterator i = _records.begin(); i != records_end; ++i) {
    if ((*i)->multi_key()[0] >= window_xmin &&
	 (*i)->multi_key()[0] <= window_xmax &&
	 (*i)->multi_key()[1] >= window_ymin &&
	 (*i)->multi_key()[1] <= window_ymax &&
	 (*i)->multi_key()[2] >= window_zmin &&
	 (*i)->multi_key()[2] <= window_zmax) {
      *(iter++) = (*i);
      ++count;
    }
  }
  return count;
}
  
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
typename KDTreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::size_type
KDTreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::
window_query(OutputIterator iter, bbox_type& domain,
	      const bbox_type& window) const
{
  size_type count = 0;
  if (window.isIn(domain)) {
    for (const_iterator i = _records.begin(); i != _records.end(); 
	  ++i) {
      *(iter++) = (*i);
    }
    count += _records.size();
  }
  else {
    // copy the window into keys for faster access.
    const key_type window_xmin = window.getLowerCorner()[0];
    const key_type window_ymin = window.getLowerCorner()[1];
    const key_type window_zmin = window.getLowerCorner()[2];
    const key_type window_xmax = window.getUpperCorner()[0];
    const key_type window_ymax = window.getUpperCorner()[1];
    const key_type window_zmax = window.getUpperCorner()[2];
    const_iterator records_end = _records.end(); 
    for (const_iterator i = _records.begin(); i != records_end; 
	  ++i) {
      if ((*i)->multi_key()[0] >= window_xmin &&
	   (*i)->multi_key()[0] <= window_xmax &&
	   (*i)->multi_key()[1] >= window_ymin &&
	   (*i)->multi_key()[1] <= window_ymax &&
	   (*i)->multi_key()[2] >= window_zmin &&
	   (*i)->multi_key()[2] <= window_zmax) {
	*(iter++) = (*i);
	++count;
      }
    }
  }
  return count;
}


  

  
//-----------------------------KDTree-----------------------------------

//
// Constructors
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
template <class InputIterator>
inline
KDTree<RecordType,MultiKeyType,KeyType,OutputIterator>::
KDTree(InputIterator first, InputIterator last, 
	const size_type leaf_size) :
  base_type(),
  _root(0),
  _domain()
{
  if (first == last) {
    _domain.setLowerCorner(multi_key_type(0,0,0));
    _domain.setUpperCorner(multi_key_type(-1,-1,-1));
    return;
  }

  // Make 3 vectors of pointers to the records.
  std::vector<value_type> xsorted;
  while (first != last) {
    xsorted.push_back(&*first);
    ++first;
  }

  std::vector<value_type> ysorted(xsorted), zsorted(xsorted);
    
  // Sort the these vectors in the x, y and z coordinates.
  std::sort(xsorted.begin(), xsorted.end(), xless_composite_h<value_type>());
  std::sort(ysorted.begin(), ysorted.end(), yless_composite_h<value_type>());
  std::sort(zsorted.begin(), zsorted.end(), zless_composite_h<value_type>());

  // Determine the domain.
  _domain.setLowerCoordinate(0, xsorted.front()->multi_key()[0]);
  _domain.setLowerCoordinate(1, ysorted.front()->multi_key()[1]);
  _domain.setLowerCoordinate(2, zsorted.front()->multi_key()[2]);
  _domain.setUpperCoordinate(0, xsorted.back()->multi_key()[0]);
  _domain.setUpperCoordinate(1, ysorted.back()->multi_key()[1]);
  _domain.setUpperCoordinate(2, zsorted.back()->multi_key()[2]);

  // The number of records.
  set_num_records(xsorted.size());

  // Make the tree.
  if (num_records() > leaf_size) {
    _root = new BranchType(xsorted, ysorted, zsorted, leaf_size);
  }
  else {
    _root = new LeafType(xsorted);
  }
}


//
// File I/O
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
void
KDTree<RecordType,MultiKeyType,KeyType,OutputIterator>::
put(std::ostream& out) const
{
  out << num_records() << " records" 
      << '\n'
      << "domain = " << domain()
      << '\n';
  if (_root) {
    _root->put(out);
  }
}

END_NAMESPACE_GEOM

// End of file.
