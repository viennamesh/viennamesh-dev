#ifndef VIENNAMESH_UTILS_UTILS_HPP
#define VIENNAMESH_UTILS_UTILS_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include <boost/shared_ptr.hpp>

namespace viennamesh
{
  template<typename IteratorT, typename FunctorT>
  class transform_iterator
  {
  public:

    typedef typename FunctorT::result_type          value_type;
    typedef typename FunctorT::pointer              pointer;
    typedef typename FunctorT::reference            reference;
    typedef typename IteratorT::iterator_category   iterator_category;
    typedef ptrdiff_t                               difference_type;

    transform_iterator(IteratorT const & it_) : it(it_) {}

    value_type & operator*() { return functor(*it); }
    value_type const & operator*() const { return functor(*it); }

    transform_iterator & operator++ ()
    {
      ++it;
      return *this;
    }

    transform_iterator  operator++ (int)
    {
      transform_iterator result(*this);
      ++(*this);
      return result;
    }


    transform_iterator & operator-- ()
    {
      --it;
      return *this;
    }

    transform_iterator  operator-- (int)
    {
      transform_iterator result(*this);
      --(*this);
      return result;
    }

    bool operator==( transform_iterator<IteratorT, FunctorT> const & other ) const
    {
      return it == other.it;
    }

    bool operator!=( transform_iterator<IteratorT, FunctorT> const & other ) const
    {
      return !(*this == other);
    }

  private:
    IteratorT it;
    FunctorT functor;
  };



  template<typename IteratorT, typename FunctorT>
  class const_transform_iterator
  {
  public:

    typedef typename FunctorT::result_type          value_type;
    typedef typename FunctorT::pointer              pointer;
    typedef typename FunctorT::reference            reference;
    typedef typename IteratorT::iterator_category   iterator_category;
    typedef ptrdiff_t                               difference_type;

    const_transform_iterator(IteratorT const & it_) : it(it_) {}

    value_type const & operator*() const { return functor(*it); }

    const_transform_iterator & operator++ ()
    {
      ++it;
      return *this;
    }

    const_transform_iterator  operator++ (int)
    {
      const_transform_iterator result(*this);
      ++(*this);
      return result;
    }


    const_transform_iterator & operator-- ()
    {
      --it;
      return *this;
    }

    const_transform_iterator  operator-- (int)
    {
      const_transform_iterator result(*this);
      --(*this);
      return result;
    }

    bool operator==( const_transform_iterator<IteratorT, FunctorT> const & other ) const
    {
      return it == other.it;
    }

    bool operator!=( const_transform_iterator<IteratorT, FunctorT> const & other ) const
    {
      return !(*this == other);
    }

  private:
    IteratorT it;
    FunctorT functor;
  };




  template<typename PairT>
  class dereference_second {};

  template<typename FirstT, typename SecondT>
  class dereference_second< std::pair<FirstT, SecondT*> >
  {
  public:
    typedef SecondT result_type;
    typedef SecondT * pointer;
    typedef SecondT & reference;

    result_type & operator()( std::pair<FirstT, SecondT*> & p )
    { return *(p.second); }

    result_type const & operator()( std::pair<FirstT, SecondT*> const & p ) const
    { return *(p.second); }
  };


}

#endif
