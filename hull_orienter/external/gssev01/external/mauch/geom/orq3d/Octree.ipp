// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2000-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file Octree.ipp
  \brief A class for an Octree in 3-D.
*/

#if !defined(__geom_Octree_ipp__)
#error This file is an implementation detail of the class Octree.
#endif

BEGIN_NAMESPACE_GEOM

//-----------------------------OctreeBranch-------------------------------

//
// OctreeBranch Constructors and Destructors
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
OctreeBranch(const semi_open_interval_type& domain) :
  _midpoint(domain.getLowerCorner() + (domain.getUpperCorner() - domain.getLowerCorner()) / 2.0)
{
  base_type::_domain = domain;
  for (int i = 0; i < 8; ++i) {
    _octant[i] = 0;
  }
}

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
~OctreeBranch()
{
  for (int i = 0; i < 8; ++i) {
    if (_octant[i]) {
      delete _octant[i];
    }
  }
}

//
// Accesors
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
int 
OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
octant_index(value_type record_pointer) const
{
  if (record_pointer->multi_key()[2] < _midpoint[2]) {
    if ( record_pointer->multi_key()[1] < _midpoint[1]) {
      if (record_pointer->multi_key()[0] < _midpoint[0])
	return 0;
      else
	return 1;
    }
    else {
      if (record_pointer->multi_key()[0] < _midpoint[0])
	return 2;
      else
	return 3;
    }
  }
  else {
    if (record_pointer->multi_key()[1] < _midpoint[1]) {
      if (record_pointer->multi_key()[0] < _midpoint[0])
	return 4;
      else
	return 5;
    }
    else {
      if (record_pointer->multi_key()[0] < _midpoint[0])
	return 6;
      else
	return 7;
    }
  }
}

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
typename OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::semi_open_interval_type
OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
octant_domain(int index) const
{
  point_type min, max;

  if (index & 1) {
    min[0] = _midpoint[0];
    max[0] = base_type::_domain.getUpperCorner()[0];
  }
  else {
    min[0] = base_type::_domain.getLowerCorner()[0];
    max[0] = _midpoint[0];
  }
    
  if (index & 2) {
    min[1] = _midpoint[1];
    max[1] = base_type::_domain.getUpperCorner()[1];
  }
  else {
    min[1] = base_type::_domain.getLowerCorner()[1];
    max[1] = _midpoint[1];
  }
    
  if (index & 4) {
    min[2] = _midpoint[2];
    max[2] = base_type::_domain.getUpperCorner()[2];
  }
  else {
    min[2] = base_type::_domain.getLowerCorner()[2];
    max[2] = _midpoint[2];
  }

  return semi_open_interval_type(min, max);
}

//
// Add grid elements.
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
OctreeNode<RecordType,MultiKeyType,KeyType,OutputIterator>*
OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
insert(value_type record_pointer, const int leaf_size)
{
  int index = octant_index(record_pointer);
  if (_octant[ index ] == 0) {
    _octant[ index ] = new LeafType(octant_domain(index));
  }
  _octant[ index ] = _octant[ index ]->insert(record_pointer, leaf_size);
  return this;
}

//
// Mathematical member functions
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
typename OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::size_type
OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
report(OutputIterator iter) const
{
  int count = 0;
  for (int i = 0; i < 8; ++i) {
    if (_octant[i]) {
      count += _octant[i]->report(iter);
    }
  }
  return count;
}

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
typename OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::size_type
OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
window_query(OutputIterator iter, const bbox_type& window) const
{
  int count = 0;
  if (doOverlap(window, base_type::_domain)) {
    for (int i = 0; i < 8; ++i) {
      if (_octant[i]) {
	count += _octant[i]->window_query(iter, window);
      }
    }
  }
  return count;
}

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
typename OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::size_type
OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
window_query_check_domain(OutputIterator iter, const bbox_type& window) const
{
  int count = 0;
  if (window.isIn(base_type::_domain)) {
    for (int i = 0; i < 8; ++i) {
      if (_octant[i]) {
	count += _octant[i]->report(iter);
      }
    }
  }
  else if (doOverlap(window, base_type::_domain)) {
    for (int i = 0; i < 8; ++i) {
      if (_octant[i]) {
	count += _octant[i]->window_query_check_domain(iter, window);
      }
    }
  }
  return count;
}

//
// File IO
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
void
OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
put(std::ostream& out) const
{
  for (int i = 0; i < 8; ++i) {
    if (_octant[i]) {
      _octant[i]->put(out);
    }
  }
}

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
void 
OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
print(std::ostream& out, std::string tabbing) const
{
  out << tabbing << base_type::_domain << '\n';
  std::string new_tabbing(tabbing);
  new_tabbing.append("  ");
  for (int i = 0; i < 8; ++i) {
    if (_octant[i]) {
      _octant[i]->print(out, new_tabbing);
    }
  }
}

//
// Memory usage.
//
    
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
typename OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::size_type
OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
memory_usage() const
{
  int count = sizeof(OctreeBranch);
  for (int i = 0; i < 8; ++i) {
    if (_octant[i]) {
      count += _octant[i]->memory_usage();
    }
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
OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator>::
check() const
{
  for (int i = 0; i < 8; ++i) {
    if (_octant[i]) {
      _octant[i]->check();
    }
  }
}

//-----------------------------OctreeLeaf-------------------------------

//
// Add grid elements.
//
      
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
OctreeNode<RecordType,MultiKeyType,KeyType,OutputIterator>*
OctreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::
insert(value_type record_pointer, const int leaf_size)
{
  // If this leaf is full.
  if (static_cast<int>(_records.size()) == leaf_size) {
    // Replace the leaf with a branch.
    BranchType* branch = new BranchType(base_type::_domain);
    for (const_iterator i = _records.begin(); i != _records.end(); ++i){
      branch->insert(*i, leaf_size);
    }
    branch->insert(record_pointer, leaf_size);
    delete this;
    return branch;
  }
  _records.push_back(record_pointer);
  return this;
}

//
// Mathematical member functions
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
typename OctreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::size_type
OctreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::
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
typename OctreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::size_type
OctreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::
window_query(OutputIterator iter, const bbox_type& window) const
{
  // copy the window into keys for faster access.
  const key_type window_xmin = window.getLowerCorner()[0];
  const key_type window_ymin = window.getLowerCorner()[1];
  const key_type window_zmin = window.getLowerCorner()[2];
  const key_type window_xmax = window.getUpperCorner()[0];
  const key_type window_ymax = window.getUpperCorner()[1];
  const key_type window_zmax = window.getUpperCorner()[2];
  const_iterator records_end = _records.end(); 
  int count = 0;
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
typename OctreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::size_type
OctreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::
window_query_check_domain(OutputIterator iter, const bbox_type& window) const
{
  int count = 0;
  if (window.isIn(base_type::_domain)) {
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

//
// File IO
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
void
OctreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::
put(std::ostream& out) const
{
  for (const_iterator i = _records.begin(); i != _records.end(); ++i) {
    out << **i << '\n';
  }
}

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
void 
OctreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator>::
print(std::ostream& out, std::string tabbing) const
{
  out << tabbing << base_type::_domain << '\n';
  for (const_iterator i = _records.begin();
	i != _records.end();
	++i) {
    out << tabbing << **i << '\n';
  }
}

//-----------------------------Octree-----------------------------------

//
// File I/O
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
void
Octree<RecordType,MultiKeyType,KeyType,OutputIterator>::
put(std::ostream& out) const
{
  out << num_records() << " grid elements" 
      << '\n'
      << "domain = " << domain()
      << '\n';
  _root->put(out);
}

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
void
Octree<RecordType,MultiKeyType,KeyType,OutputIterator>::
print(std::ostream& out) const
{
  out << num_records() << " grid elements" << '\n';
  out << "domain = " << domain() << '\n';

  std::string empty;
  _root->print(out, empty);
}

END_NAMESPACE_GEOM

// End of file.
