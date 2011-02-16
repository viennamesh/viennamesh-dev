/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Karl Rupp                                rupp@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_WRAPPER_WRAPPERBASE_HPP
#define VIENNAMESH_WRAPPER_WRAPPERBASE_HPP

//#include <iterator>
#include <boost/iterator/iterator_facade.hpp>

namespace viennamesh {

// -------------------------------------------------------------------------------------------   
template<typename DatastructureTag, typename Datastructure>
struct wrapper
{
};
// -------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------
// template<typename T, typename DerefResultT>
// struct iterator_base_2 
//    :  public boost::iterator_facade< 
//          iterator_base_2<T, DerefResultT>,      // self type
//          std::size_t,                           // type of objects the iterator is traversing
//          //boost::random_access_traversal_tag,    // iteartor traversal concept
//          std::random_access_iterator_tag,
//          DerefResultT,                          // result type of derefencing the iterator
//          std::size_t                            // difference type
//       >
// {
// private:
//    typedef iterator_base_2<T, DerefResultT>           self_type;
//    typedef typename self_type::iterator_facade_       base_type;
//    
// public:
//    iterator_base_2(T& obj) : obj_(obj),pos_(0) {}
//    explicit iterator_base_2(T& obj, std::size_t newpos) : obj_(obj),pos_(newpos) {}
//    
//    inline std::size_t   pos() const { return pos_; }         
//    inline std::size_t & pos()       { return pos_; }
//    
//    inline T &    obj() const { return obj_; }
//    inline T &    obj()       { return obj_; }      
//    
//    friend class boost::iterator_core_access;
//    
//    bool equal(self_type const& other) const
//    {
//        return this->pos_ == other.pos_;
//    }   
//    
//    void increment()              { pos_++;  }
//    void decrement()              { pos_--;  }
//    
//    void advance(typename base_type::difference_type n)   { pos_+=n; }
//    
//    typename base_type::difference_type 
//    distance_to(self_type const& other) const
//    {
//       return other.pos()-this->pos();
//    }
//    
//    virtual DerefResultT dereference() = 0;
//    
//    T &    obj_;   
//    std::size_t pos_;
// };

template<typename T>
struct iterator_base : public std::iterator<std::forward_iterator_tag, T >
{
   typedef iterator_base<T>            self_type;
   
   iterator_base(T& obj)                 : obj_(obj),pos_(0)      {}
   iterator_base(T & obj, size_t newpos) : obj_(obj),pos_(newpos) {}   
   
   bool operator!=(const iterator_base &other) const 
   {
      return !(this->pos() == other.pos());
   }

   bool operator!=(const iterator_base &other) 
   {
      return !(this->pos() == other.pos());
   }

   void operator++()          { ++pos_; } // prefix  increment   
   void operator++(int)       { pos_++; } // postfix increment      

   inline size_t   pos() const { return pos_; }         
   inline size_t & pos()       { return pos_; }
   
   inline T &    obj() const { return obj_; }
   inline T &    obj()       { return obj_; }   

   T &    obj_;
   size_t pos_;   
};
// -------------------------------------------------------------------------------------------

} // end namespace viennamesh

#endif