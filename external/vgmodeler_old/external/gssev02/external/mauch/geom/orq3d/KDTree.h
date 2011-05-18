// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2000-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file KDTree.h
  \brief A class for a kd-tree in 3-D.
*/

#if !defined(__KDTree_h__)
#define __KDTree_h__

// If we are debugging the whole geom package.
#if defined(DEBUG_geom) && !defined(DEBUG_KDTree)
#define DEBUG_KDTree
#endif

#ifdef DEBUG_KDTree
// Debug the base class as well.
#ifndef DEBUG_ORQ
#define DEBUG_ORQ
#endif
#endif

#include "ORQ.h"
#include "RecordCompare.h"

#include "../../ads/functor/composite_compare.h"

#include <vector>
#include <algorithm>

BEGIN_NAMESPACE_GEOM

//
//---------------------------KDTreeNode----------------------------------
//

//! Abstract base class for nodes in a KDTree.
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
class KDTreeNode {
private:

  //
  // Private types.
  //

  typedef ORQ<RecordType,MultiKeyType,KeyType> ORQT;

public:
    
  //
  // Public types.
  //

  //! The record type.
  typedef typename ORQT::record_type record_type;
  //! A pointer to the record type.
  typedef typename ORQT::value_type value_type;
  //! A pointer to the value_type.
  typedef typename ORQT::pointer pointer;
  //! A const pointer to the value_type.
  typedef typename ORQT::const_pointer const_pointer;
  //! A reference to the value_type.
  typedef typename ORQT::reference reference;
  //! A const reference to the value_type.
  typedef typename ORQT::const_reference const_reference;
  //! The size type.
  typedef typename ORQT::size_type size_type;
  //! The multi-key type.
  typedef typename ORQT::multi_key_type multi_key_type;
  //! The key type.
  typedef typename ORQT::key_type key_type;
  //! Bounding box.
  typedef typename ORQT::bbox_type bbox_type;
  //! Semi-open interval.
  typedef typename ORQT::semi_open_interval_type semi_open_interval_type;

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  //@{

  //! Virtual destructor.  We need this because we have other virtual functions.
  virtual
  ~KDTreeNode()
  {}

  //@}
  //--------------------------------------------------------------------------
  //! \name Window queries.
  //@{

  //! Get the records in the node and children.  Return the # of records.
  virtual 
  size_type
  report(OutputIterator iter) const = 0;

  //! Get the records in the window.  Return the # of records inside.
  virtual 
  size_type
  window_query(OutputIterator iter, const bbox_type& window) const = 0;

  //! Get the records in the window.  Return the # of records inside.
  virtual 
  size_type
  window_query(OutputIterator iter, bbox_type& domain, 
		const bbox_type& window) const = 0;

  //@}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  //@{

  //! Print the records.
  virtual 
  void 
  put(std::ostream& out) const = 0;

  //@}
  //--------------------------------------------------------------------------
  //! \name Memory usage.
  //@{

  //! Return the memory usage of this node and its children.
  virtual 
  size_type
  memory_usage() const = 0;

  //@}
  //--------------------------------------------------------------------------
  //! \name Validity check.
  //@{

  // Check the validity of the node.
  virtual 
  void 
  check(const bbox_type& window) const = 0;

  //@}
};

//! Write to a file stream.
/*! \relates KDTreeNode */
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
std::ostream& 
operator<<(std::ostream& out, 
	    const KDTreeNode<RecordType,MultiKeyType,KeyType,
	    OutputIterator>& node)
{
  node.put(out);
  return out;
}

//
//---------------------------KDTreeLeaf----------------------------------
//

//! Class for a leaf in a KDTree.
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
class KDTreeLeaf : 
  public KDTreeNode<RecordType,MultiKeyType,KeyType,OutputIterator>
{
private:

  typedef KDTreeNode<RecordType,MultiKeyType,KeyType,OutputIterator> NodeType;

public:

  //
  // Public types.
  //

  //! The record type.
  typedef typename NodeType::record_type record_type;
  //! A pointer to the record type.
  typedef typename NodeType::value_type value_type;
  //! A pointer to the value_type.
  typedef typename NodeType::pointer pointer;
  //! A const pointer to the value_type.
  typedef typename NodeType::const_pointer const_pointer;
  //! A reference to the value_type.
  typedef typename NodeType::reference reference;
  //! A const reference to the value_type.
  typedef typename NodeType::const_reference const_reference;
  //! The size type.
  typedef typename NodeType::size_type size_type;
  //! The multi-key type.
  typedef typename NodeType::multi_key_type multi_key_type;
  //! The key type.
  typedef typename NodeType::key_type key_type;
  //! Bounding box.
  typedef typename NodeType::bbox_type bbox_type;
  //! Semi-open interval.
  typedef typename NodeType::semi_open_interval_type semi_open_interval_type;

private:

  //
  // Private types.
  //

  typedef std::vector<value_type> container_type;
  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;

private:

  //
  // Member data
  //
    
  //! The records
  container_type _records;

  //
  // Not implemented
  //

  //! Copy constructor not implemented
  KDTreeLeaf(const KDTreeLeaf&);

  //! Assignment operator not implemented
  KDTreeLeaf& 
  operator=(const KDTreeLeaf&);


public:
    
  //--------------------------------------------------------------------------
  //! \name Constructors and destructor.
  //@{

  //! Construct from a vector of records.
  KDTreeLeaf(const std::vector<value_type>& records) :
    _records(records)
  {}

  //! Trivial destructor.
  virtual 
  ~KDTreeLeaf()
  {}

  //@}
  //--------------------------------------------------------------------------
  //! \name Window queries.
  //@{

  // Get the records.  Return the # of records.
  size_type
  report(OutputIterator iter) const;

  // Get the records in the window.  Return the # of records inside.
  size_type 
  window_query(OutputIterator iter, const bbox_type& window) const;

  // Get the records in the window.  Return the # of records inside.
  size_type
  window_query(OutputIterator iter, bbox_type& domain, 
		const bbox_type& window) const;

  //@}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  //@{

  // Print the records.
  void 
  put(std::ostream& out) const
  {
    for (const_iterator i = _records.begin(); i != _records.end(); ++i) {
      out << **i << '\n';
    }
  }

  //@}
  //--------------------------------------------------------------------------
  //! \name Memory usage.
  //@{

  //! Return the memory usage of this leaf.
  size_type
  memory_usage() const
  {
    return (sizeof(KDTreeLeaf) + _records.size() * sizeof(value_type));
  }

  //@}
  //--------------------------------------------------------------------------
  //! \name Validity check.
  //@{

  void 
  check(const bbox_type& domain) const
  {
    typename bbox_type::Point p;
    for (const_iterator i = _records.begin(); i != _records.end(); ++i) {
      p[0] = (*i)->multi_key()[0];
      p[1] = (*i)->multi_key()[1];
      p[2] = (*i)->multi_key()[2];
      assert(domain.isIn(p));
    }
  }

  //@}
};

//! Write to a file stream.
/*! \relates KDTreeLeaf */
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
std::ostream& 
operator<<(std::ostream& out, 
	    const KDTreeLeaf<RecordType,MultiKeyType,KeyType,
	    OutputIterator>& x)
{
  x.put(out);
  return x;
}


//
//-------------------------KDTreeBranch------------------------------
//

//! Class for an internal node in a KDTree.
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
class KDTreeBranch : 
  public KDTreeNode<RecordType,MultiKeyType,KeyType,OutputIterator>
{
private:

  typedef KDTreeNode<RecordType,MultiKeyType,KeyType,OutputIterator> NodeType;
  typedef KDTreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator> LeafType;

public:

  //
  // Public types.
  //

  //! The record type.
  typedef typename NodeType::record_type record_type;
  //! A pointer to RecordType.
  typedef typename NodeType::value_type value_type;
  //! A pointer to the value_type.
  typedef typename NodeType::pointer pointer;
  //! A const pointer to the value_type.
  typedef typename NodeType::const_pointer const_pointer;
  //! A reference to the value_type.
  typedef typename NodeType::reference reference;
  //! A const reference to the value_type.
  typedef typename NodeType::const_reference const_reference;
  //! The size type.
  typedef typename NodeType::size_type size_type;
  //! The multi-key type.
  typedef typename NodeType::multi_key_type multi_key_type;
  //! The key type.
  typedef typename NodeType::key_type key_type;
  //! Bounding box.
  typedef typename NodeType::bbox_type bbox_type;
  //! Semi-open interval.
  typedef typename NodeType::semi_open_interval_type semi_open_interval_type;

private:

  //
  // Not implemented
  //
    
  // Default constructor not implemented.
  KDTreeBranch();

  // Copy constructor not implemented
  KDTreeBranch(const KDTreeBranch&);

  // Assignment operator not implemented
  KDTreeBranch& 
  operator=(const KDTreeBranch&);

protected:

  //
  // Member data
  //

  //! The left sub-tree.
  NodeType* _left;

  //! The right sub-tree.
  NodeType* _right;

  //! The splitting dimension
  int _split_dimension;

  //! The splitting value.
  key_type _split_value;

public:

  //--------------------------------------------------------------------------
  //! \name Constructor and destructor.
  //@{

  //! Construct from sorted records.
  KDTreeBranch(const std::vector<value_type>& xsorted,
		const std::vector<value_type>& ysorted,
		const std::vector<value_type>& zsorted,
		const size_type leaf_size);

  //! Destructor.  Delete this and the left and right branches.
  virtual 
  ~KDTreeBranch()
  {
    delete _left;
    delete _right;
  }

  //@}
  //--------------------------------------------------------------------------
  //! \name Window queries.
  //@{

  // Get the records.  Return the # of records.
  size_type
  report(OutputIterator iter) const
  { 
    return _left->report(iter) + _right->report(iter);
  }

  // Get the records in the window.  Return the # of records inside.
  size_type
  window_query(OutputIterator iter, const bbox_type& window) const;

  // Get the records in the window.  Return the # of records inside.
  size_type
  window_query(OutputIterator iter, bbox_type& domain, 
		const bbox_type& window) const;

  //@}
  //--------------------------------------------------------------------------
  //! \name Memory usage.
  //@{

  //! Return the memory usage of this branch and its children.
  size_type
  memory_usage() const
  {
    return (sizeof(KDTreeBranch) + _left->memory_usage() 
	     + _right->memory_usage());
  }

  //@}
  //--------------------------------------------------------------------------
  //! \name Validity check.
  //@{

  // Check for validity.
  void 
  check(const bbox_type& window) const;
    
  //@}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  //@{

  // Print the records.
  void 
  put(std::ostream& out) const
  {
    _left->put(out);
    _right->put(out);
  }

  //@}
};

//! Write to a file stream.
/*! \relates KDTreeBranch */
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
std::ostream& 
operator<<(std::ostream& out, 
	    const KDTreeBranch<RecordType,MultiKeyType,KeyType,
	    OutputIterator>& x)
{
  x.put(out);
  return x;
}


//
//---------------------------KDTree--------------------------------------
//

//! A kd-tree in 3-D.
/*!
  An kd-tree in 3-D holding pointers to RecordType.  RecordType
  must have the member function multi_key() that returns a const
  reference to the multi-key of the record.

  OutputIterator is the typename that you will use in the
  window_query() function.  You have to specify it as a template
  parameter because of the way KDTree is implemented.  C++ does
  not allow templated virtual functions.
*/
template <typename RecordType,
	  typename MultiKeyType = typename RecordType::multi_key_type,
	  typename KeyType = typename RecordType::key_type,
	  typename OutputIterator = RecordType**>
class KDTree :
  public ORQ<RecordType,MultiKeyType,KeyType>
{
private:

  //
  // Private types.
  //

  typedef ORQ<RecordType,MultiKeyType,KeyType> base_type;

public:

  //
  // Public types.
  //

  //! The record type.
  typedef typename base_type::record_type record_type;
  //! A pointer to the record type.
  typedef typename base_type::value_type value_type;
  //! A pointer to the value_type.
  typedef typename base_type::pointer pointer;
  //! A const pointer to the value_type.
  typedef typename base_type::const_pointer const_pointer;
  //! A reference to the value_type.
  typedef typename base_type::reference reference;
  //! A const reference to the value_type.
  typedef typename base_type::const_reference const_reference;
  //! The size type.
  typedef typename base_type::size_type size_type;
  //! The multi-key type.
  typedef typename base_type::multi_key_type multi_key_type;
  //! The key type.
  typedef typename base_type::key_type key_type;
  //! Bounding box.
  typedef typename base_type::bbox_type bbox_type;
  //! Semi-open interval.
  typedef typename base_type::semi_open_interval_type semi_open_interval_type;

private:

  //
  // Private types.
  //

  typedef KDTreeNode<RecordType,MultiKeyType,KeyType,OutputIterator> NodeType;
  typedef KDTreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator> 
  BranchType;
  typedef KDTreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator> LeafType;

  //
  // Member data
  //

  //! The root of the tree.
  NodeType* _root;

  //! The domain of the kd-tree.
  bbox_type _domain;

private:

  //
  // Not implemented
  //

  //! Default constructor not implemented.
  KDTree();

  //! Copy constructor not implemented
  KDTree(const KDTree&);

  //! Assignment operator not implemented
  KDTree& 
  operator=(const KDTree&);

public:

  //--------------------------------------------------------------------------
  //! \name Constructors and destructor.
  //@{

  //! Construct from a range of records.
  /*!
    \param first is the beginning of the range of records.
    \param last is the end of the range of records.
    \param leaf_size is the maximum number of records that are stored in a
    leaf.  Choose this value to be about the number records that you
    expect a window query to return.  The default value is 8.
  */
  template <class InputIterator>
  KDTree(InputIterator first, InputIterator last, 
	  const size_type leaf_size = 8);

  //! Destructor.  Delete the tree.
  ~KDTree()
  {
    if (_root) {
      delete _root;
    }
  }
  
  // @}
  //--------------------------------------------------------------------------
  //! \name Accesors.
  // @{

  //
  // Inherited.
  //

  //! Return the number of records.
  size_type 
  num_records() const 
  { 
    return base_type::num_records();
  }

  //! Return true if the grid is empty.
  bool 
  is_empty() const 
  { 
    return base_type::is_empty();
  }

  //
  // New.
  //

  //! Return the domain containing the records.
  const bbox_type& 
  domain() const 
  { 
    return _domain; 
  }
  
  // @}
  //--------------------------------------------------------------------------
  //! \name Window queries.
  // @{

  //! Get the records in the window.  Return the # of records inside.
  size_type
  window_query(OutputIterator iter, const bbox_type& window) const
  {
    if (_root) {
      return _root->window_query(iter, window);
    }
    return 0;
  }

  //! Get the records in the window.  Return the # of records inside.
  /*!
    Use this version of window query only if the number of records 
    returned is much larger than the leaf size.

    This implementation of KDTree does not store the domain
    information at the branches and leaves.  This choice decreases
    the memory usage but incurs the computational cost of having to
    compute the domain as the window query progresses if you use this
    function.
  */
  size_type
  window_query_check_domain(OutputIterator iter, const bbox_type& window)
    const
  {
    if (_root) {
      bbox_type domain(_domain);
      return _root->window_query(iter, domain, window);
    }
    return 0;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  // @{

  //! Print the records.
  void 
  put(std::ostream& out) const;

  // @}
  //--------------------------------------------------------------------------
  //! \name Memory usage.
  // @{

  //! Return the memory usage of the tree.
  size_type
  memory_usage() const
  {
    return (sizeof(KDTree) + _root->memory_usage());
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Validity check.
  // @{

  //! Check the validity of the kd-tree.
  void 
  check() const
  {
    if (_root) {
      _root->check(_domain);
    }
  }

  // @}

protected:

  //! Increment the number of records.
  void
  increment_num_records()
  {
    base_type::increment_num_records();
  }

  void
  set_num_records(const size_type num)
  {
    base_type::set_num_records(num);
  }
};

//! Write to a file stream.
/*! \relates KDTree */
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
std::ostream& 
operator<<(std::ostream& out, 
	    const KDTree<RecordType,MultiKeyType,KeyType,OutputIterator>& x)
{
  x.put(out);
  return out;
}

END_NAMESPACE_GEOM

#define __geom_KDTree_ipp__
#include "KDTree.ipp"
#undef __geom_KDTree_ipp__

#endif

// End of file.
