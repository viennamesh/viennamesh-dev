/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_ITERATOR_HH_ID
#define GSSE_ITERATOR_HH_ID

#include <boost/iterator/iterator_facade.hpp>

template <typename IteratorT>
struct gsse_iterator : 
   public boost::iterator_facade< gsse_iterator<IteratorT>,
                                  typename IteratorT::value_type, 
                                  boost::random_access_traversal_tag,
                                  typename IteratorT::value_type, 
                                  unsigned int>
{
   typedef gsse_iterator<IteratorT> self;

   // =================================================
   // constructors
   //
   gsse_iterator() :actual(), begin(), end() {}

   gsse_iterator(IteratorT const& actual, IteratorT const& end)
      : actual(actual), begin(actual), end(end) {}

   gsse_iterator(IteratorT const& actual, IteratorT const& begin, IteratorT const& end)
      : actual(actual), begin(begin), end(end) {}
   


   // =================================================
   // 
   //
   bool equal(self const& other) const {return actual == other.actual;}

   void increment() {++actual;}
   void decrement() {--actual;}
   void advance(unsigned int i) {actual += i;}
   typename IteratorT::value_type dereference() const {return *actual;}
   
   unsigned int difference(self const& other) const {return actual - other.actual;}
   unsigned int distance_to(self const& other) const {return actual - other.actual;}

   // =================================================
   // gsse concepts
   //
   bool valid() const {return actual != end;}
   void reset() {actual = begin;}


  friend class boost::iterator_core_access;

private:

   IteratorT actual;
   IteratorT begin;
   IteratorT end;
};



template <typename IteratorT, typename BaseType>
struct gsse_lvalue_iterator : 
  public boost::iterator_facade< gsse_lvalue_iterator<IteratorT, BaseType>,
                                  typename IteratorT::value_type, 
                                  boost::random_access_traversal_tag,
                                  typename IteratorT::value_type&, 
                                  unsigned int>
{
  typedef gsse_lvalue_iterator<IteratorT, BaseType> self;

  gsse_lvalue_iterator() :actual(), begin(), end() {}

  gsse_lvalue_iterator(IteratorT const& actual, IteratorT const& end, BaseType & base)
    : actual(actual), begin(actual), end(end), base_(base) {}

  gsse_lvalue_iterator(IteratorT const& actual, IteratorT const& begin, IteratorT const& end, 
		       BaseType & base)
    : actual(actual), begin(begin), end(end), base_(&base) {}
   
  bool equal(self const& other) const {return actual == other.actual;}

   void increment() {++actual;}
   void decrement() {--actual;}
   void advance(unsigned int i) {actual += i;}
   typename IteratorT::value_type& dereference() const {return *actual;}
   
   unsigned int difference(self const& other) const {return actual - other.actual;}

   bool valid() {return actual != end;}

   void reset() {actual = begin;}
  
   BaseType & base() const {return *base_;}

   unsigned int distance_to(self const& other) const {return actual - other.actual;}

   friend class boost::iterator_core_access;

private:

  IteratorT actual;
  IteratorT begin;
  IteratorT end;
  BaseType  *base_;
};

template <typename IteratorT, typename BaseType>
struct gsse_clvalue_iterator : 
  public boost::iterator_facade< gsse_clvalue_iterator<IteratorT, BaseType>,
                                  const typename IteratorT::value_type, 
                                  boost::random_access_traversal_tag,
                                  const typename IteratorT::value_type&, 
                                  unsigned int>
{
  typedef gsse_clvalue_iterator<IteratorT, BaseType> self;

  gsse_clvalue_iterator() :actual(), begin(), end() {}

  gsse_clvalue_iterator(IteratorT const& actual, IteratorT const& end, BaseType & base)
    : actual(actual), begin(actual), end(end), base_(base) {}

  gsse_clvalue_iterator(IteratorT const& actual, IteratorT const& begin, IteratorT const& end, 
			BaseType & base)
    : actual(actual), begin(begin), end(end), base_(&base) {}
  
  bool equal(self const& other) const {return actual == other.actual;}

  void increment() {++actual;}
  void decrement() {--actual;}
  void advance(unsigned int i) {actual += i;}
  typename  IteratorT::value_type const& dereference() const {return *actual;}
   
  unsigned int difference(self const& other) const {return actual - other.actual;}
  
  bool valid() {return actual != end;}
  
  void reset() {actual = begin;}
  
  BaseType & base() const {return *base_;}
  
  friend class boost::iterator_core_access;

private:

  IteratorT actual;
  IteratorT begin;
  IteratorT end;
  BaseType *base_;

};



#endif
