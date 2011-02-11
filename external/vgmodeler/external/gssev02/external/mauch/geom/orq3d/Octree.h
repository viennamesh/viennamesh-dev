// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2000-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file Octree.h
  \brief A class for an octree in 3-D.
*/

#if !defined(__geom_Octree_h__)
#define __geom_Octree_h__

#include "ORQ.h"

#include <string>

BEGIN_NAMESPACE_GEOM

//
//----------------------------OctreeNode--------------------------------
//

//! A node in an octree.
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
class OctreeNode
{
private:

  //
  // Private types.
  //

  typedef ORQ<RecordType,MultiKeyType,KeyType> types;

public:
    
  //
  // public typedefs
  //

  //! A pointer to the record type.
  typedef typename types::value_type value_type;
  //! A pointer to the value_type.
  typedef typename types::pointer pointer;
  //! A const pointer to the value_type.
  typedef typename types::const_pointer const_pointer;
  //! A reference to the value_type.
  typedef typename types::reference reference;
  //! A const reference to the value_type.
  typedef typename types::const_reference const_reference;
  //! The size type.
  typedef typename types::size_type size_type;
  //! The multi-key type.
  typedef typename types::multi_key_type multi_key_type;
  //! The key type.
  typedef typename types::key_type key_type;
  //! A Cartesian point.
  typedef typename types::point_type point_type;
  //! Bounding box.
  typedef typename types::bbox_type bbox_type;
  //! Semi-open interval.
  typedef typename types::semi_open_interval_type semi_open_interval_type;

protected:

  //! The domain of this node.
  semi_open_interval_type _domain;
  
public:

  //--------------------------------------------------------------------------
  //! \name Destructor.
  //@{

  //! Trivial destructor.
  virtual 
  ~OctreeNode()
  {}

  //@}   
  //--------------------------------------------------------------------------
  //! \name Accesors.
  //@{

  //! Return the domain.
  const semi_open_interval_type& 
  domain() const 
  { 
    return _domain; 
  }

  //@}   
  //--------------------------------------------------------------------------
  //! \name Insert records.
  //@{

  //! Add a record. Return the OctreeNode containing the record.
  virtual 
  OctreeNode* 
  insert(value_type record_pointer, const int leaf_size) = 0;

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
  window_query_check_domain(OutputIterator iter, const bbox_type& window) 
    const = 0;

  //@}   
  //--------------------------------------------------------------------------
  //! \name File I/O.
  //@{

  //! Print the octree node
  virtual 
  void 
  put(std::ostream& out) const = 0;

  //! Print the octree node as a tree.
  virtual 
  void 
  print(std::ostream& out, std::string tabbing) const = 0;

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

  virtual 
  void 
  check() const = 0;

  //@}   
};

//! Write to a file stream.
/*! \relates OctreeNode */
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
std::ostream& 
operator<<(std::ostream& out, 
	    const OctreeNode<RecordType,MultiKeyType,KeyType,OutputIterator>& 
	    node)
{
  node.put(out);
  return out;
}


//
//-------------------------OctreeBranch------------------------------
//

//
// Forward declarations
//

template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
class OctreeLeaf;

//! A branch in an octree.
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
class OctreeBranch :
  public OctreeNode<RecordType,MultiKeyType,KeyType,OutputIterator>
{
private:

  typedef OctreeNode<RecordType,MultiKeyType,KeyType,OutputIterator> base_type;
  typedef OctreeNode<RecordType,MultiKeyType,KeyType,OutputIterator> NodeType;
  typedef OctreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator> LeafType;

public:

  //
  // public typedefs
  //

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
  //! A Cartesian point.
  typedef typename base_type::point_type point_type;
  //! Bounding box.
  typedef typename base_type::bbox_type bbox_type;
  //! Semi-open interval.
  typedef typename base_type::semi_open_interval_type semi_open_interval_type;

private:

  //! The midpoint of the domain.
  point_type _midpoint;
  //! The eight octants.
  NodeType* _octant[8];
  
private:

  //
  // Not implemented
  //

  //! Default constructor not implemented.
  OctreeBranch();

  //! Copy constructor not implemented
  OctreeBranch(const OctreeBranch&);

  //! Assignment operator not implemented
  OctreeBranch& 
  operator=(const OctreeBranch&);

public:

  //--------------------------------------------------------------------------
  //! \name Constructors and destructor.
  //@{

  //! Construct from the Cartesian domain.
  OctreeBranch(const semi_open_interval_type& domain);

  //! Destructor.
  virtual 
  ~OctreeBranch();

  //@}
  //--------------------------------------------------------------------------
  //! \name Insert records.
  //@{

  //! Add a record.
  NodeType* 
  insert(value_type record_pointer, const int leaf_size);

  //@}
  //--------------------------------------------------------------------------
  //! \name Window queries.
  //@{
  
  //! Get the records in the node and children.  Return the # of records.
  size_type
  report(OutputIterator iter) const;

  //! Get the records in the window.  Return the # of records inside.
  size_type 
  window_query(OutputIterator iter, const bbox_type& window) const;

  //! Get the records in the window.  Return the # of records inside.
  size_type 
  window_query_check_domain(OutputIterator iter, const bbox_type& window) 
    const;

  //@}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  //@{

  //! Print the octree.
  void 
  put(std::ostream& out) const;

  //! Print the octree as a tree.
  void 
  print(std::ostream& out, std::string tabbing) const;

  //@}
  //--------------------------------------------------------------------------
  //! \name Memory usage.
  //@{

  //! Return the memory usage of this branch and its children.
  size_type 
  memory_usage() const;

  //@}
  //--------------------------------------------------------------------------
  //! \name Validity check.
  //@{

  void 
  check() const;

  //@}

private:

  //! Return the octant index containing the record.
  int 
  octant_index(value_type record_pointer) const;

  //! Return the domain for the given octant.
  semi_open_interval_type 
  octant_domain(int index) const;

};


//
//----------------------------OctreeLeaf--------------------------------
//

//! A leaf in an octree.
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
class OctreeLeaf :
  public OctreeNode<RecordType,MultiKeyType,KeyType,OutputIterator>
{
private:

  typedef OctreeNode<RecordType,MultiKeyType,KeyType,OutputIterator> base_type;
  typedef OctreeNode<RecordType,MultiKeyType,KeyType,OutputIterator> NodeType;
  typedef OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator> 
  BranchType;

public:

  //
  // public typedefs
  //

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
  // private typedefs
  //

  typedef std::vector<value_type> container_type;
  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;

private:

  //! The records
  container_type _records;
  
private:

  //
  // Not implemented
  //

  //! Default constructor not implemented.
  OctreeLeaf();

  //! Copy constructor not implemented
  OctreeLeaf(const OctreeLeaf&);

  //! Assignment operator not implemented
  OctreeLeaf& 
  operator=(const OctreeLeaf&);

public:

  //--------------------------------------------------------------------------
  //! \name Constructors and destructor.
  //@{

  //! Construct from a Cartesian domain.
  OctreeLeaf(const semi_open_interval_type& domain) :
    _records()
  {
    base_type::_domain = domain;
  }

  //! Trivual destructor.
  virtual 
  ~OctreeLeaf()
  {}

  //@}
  //--------------------------------------------------------------------------
  //! \name Insert records.
  //@{

  //! Add a record.
  NodeType* 
  insert(value_type record_pointer, const int leaf_size);

  //@}
  //--------------------------------------------------------------------------
  //! \name Window queries.
  //@{
  
  //! Get the records in the node and children.  Return the # of records.
  size_type
  report(OutputIterator iter) const;

  //! Get the records in the window.  Return the # of records inside.
  size_type 
  window_query(OutputIterator iter, const bbox_type& window) const;

  //! Get the records in the window.  Return the # of records inside.
  size_type 
  window_query_check_domain(OutputIterator iter, const bbox_type& window) 
    const;

  //@}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  //@{

  //! Print the octree.
  void 
  put(std::ostream& out) const;

  //! Print the octree in tree form.
  void 
  print(std::ostream& out, std::string tabbing) const;

  //@}
  //--------------------------------------------------------------------------
  //! \name Memory usage.
  //@{

  //! Return the memory usage of this leaf.
  size_type 
  memory_usage() const
  {
    return (sizeof(OctreeLeaf) + _records.size() * sizeof(value_type));
  }

  //@}
  //--------------------------------------------------------------------------
  //! \name Validity check.
  //@{

  void 
  check() const
  {
    for (const_iterator i = _records.begin(); i != _records.end(); ++i) {
      assert(base_type::_domain.isIn((*i)->multi_key()));
    }
  }

  //@}
};




//
//---------------------------Octree class---------------------------------
//

//! An octree in 3-D.
/*!
  An octree in 3-D holding pointers to RecordType.
  RecordType must have the member function multi_key() that returns 
  a const reference to the multi-key of the record.

  OutputIterator is the typename that you will use in the window_query()
  function.  You have to specify it as a template parameter because 
  of the way Octree is implemented.  C++ does not allow templated 
  virtual functions.

  Use window_query if the number of records returned by a typical
  window query is small. Use window_query_check_domain only if the
  number of records returned is much larger than the leaf size.

  This implementation of Octree stored the domain information at each
  branch and leaf.  This choice increases the memory usage but avoids 
  the computational cost of having to compute the domain as the 
  window query progresses.
*/
template <typename RecordType,
	  typename MultiKeyType = typename RecordType::multi_key_type,
	  typename KeyType = typename RecordType::key_type,
	  typename OutputIterator = RecordType**>
class Octree :
  public ORQ<RecordType,MultiKeyType,KeyType>
{
private:

  //
  // Private types.
  //

  typedef ORQ<RecordType,MultiKeyType,KeyType> base_type;

public:

  //
  // public typedefs
  //

  //! A pointer to RecordType.
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
  // private typedefs
  //

  typedef OctreeNode<RecordType,MultiKeyType,KeyType,OutputIterator> NodeType;
  typedef OctreeBranch<RecordType,MultiKeyType,KeyType,OutputIterator> 
  BranchType;
  typedef OctreeLeaf<RecordType,MultiKeyType,KeyType,OutputIterator> LeafType;

  //
  // Member data
  //

  //! The root of the octree.
  NodeType* _root;

  //! The leaf size.
  int _leaf_size;

private:

  //
  // Not implemented
  //

  //! Default constructor not implemented.
  Octree();

  //! Copy constructor not implemented
  Octree(const Octree&);

  //! Assignment operator not implemented
  Octree& 
  operator=(const Octree&);

public:

  //--------------------------------------------------------------------------
  //! \name Constructors and destructor.
  //@{

  //! Construct from a Cartesian domain.
  /*!
    leaf_size is the maximum number of records that are stored in a leaf.
    Choose this value to be about the number records that you expect a 
    window query to return.  The default value is 8.
  */
  Octree(const semi_open_interval_type& domain, const int leaf_size = 8) :
    base_type(),
    _leaf_size(leaf_size)
  {
    _root = new LeafType(domain);
  }

  //! Construct from a Cartesian domain and a range of records.
  /*!
    leaf_size is the maximum number of records that are stored in a leaf.
    Choose this value to be about the number records that you expect a 
    window query to return.  The default value is 8.
  */
  template <class InputIterator>
  Octree(const semi_open_interval_type& domain, 
	  InputIterator first, InputIterator last, 
	  const int leaf_size = 8) :
    base_type(),
    _leaf_size(leaf_size)
  {
    _root = new LeafType(domain);
    insert(first, last);
  }

  //! Destructor.
  ~Octree()
  { 
    delete _root; 
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

  //! Return the domain spanned by the records.
  const semi_open_interval_type& 
  domain() const 
  { 
    return _root->domain(); 
  }
  
  // @}
  //--------------------------------------------------------------------------
  //! \name Insert records.
  // @{

  //! Add a single record.
  void 
  insert(value_type record_pointer)
  {
    _root = _root->insert(record_pointer, _leaf_size);
    increment_num_records();
  }

  //! Add a range of records.
  template <typename InputIterator>
  void 
  insert(InputIterator first, InputIterator last)
  {
    while (first != last) {
      insert(&*first);
      ++ first;
    }
  }
  
  // @}
  //--------------------------------------------------------------------------
  //! \name Window queries.
  // @{

  //! Get the records in the node and children.  Return the # of records.
  size_type 
  report(OutputIterator iter) const
  {
    return _root->report(iter);
  }

  //! Get the records in the window.  Return the # of records inside.
  size_type 
  window_query(OutputIterator iter, const bbox_type& window) const
  {
    return _root->window_query(iter, window);
  }

  //! Get the records in the window.  Return the # of records inside.
  /*!
    Use this version of window query only if the number of records 
    returned is much larger than the leaf size.
  */
  size_type 
  window_query_check_domain(OutputIterator iter, 
			     const bbox_type& window) const
  {
    return _root->window_query_check_domain(iter, window);
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  // @{

  //! Print the octree.
  void 
  put(std::ostream& out) const;

  //! Print the structure of the octree.
  void 
  print(std::ostream& out) const;

  // @}
  //--------------------------------------------------------------------------
  //! \name Memory usage.
  // @{

  //! Return the memory usage of the octree.
  size_type 
  memory_usage() const
  {
    return (sizeof(Octree) + _root->memory_usage());
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Validity check.
  // @{

  //! Check the validity of the octree.
  void 
  check() const
  {
    _root->check();
  }

  // @}

protected:

  //! Increment the number of records.
  void
  increment_num_records()
  {
    base_type::increment_num_records();
  }
};

//! Write to a file stream.
/*! \relates Octree */
template <typename RecordType, typename MultiKeyType, typename KeyType,
	  typename OutputIterator>
inline
std::ostream& 
operator<<(std::ostream& out, 
	    const Octree<RecordType,MultiKeyType,KeyType,OutputIterator>& x)
{
  x.put(out);
  return out;
}

END_NAMESPACE_GEOM

#define __geom_Octree_ipp__
#include "Octree.ipp"
#undef __geom_Octree_ipp__

#endif

// End of file.
