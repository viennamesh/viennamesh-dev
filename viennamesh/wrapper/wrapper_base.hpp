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

namespace viennamesh {

// -------------------------------------------------------------------------------------------   
template<typename DatastructureTag, typename Datastructure>
struct wrapper
{
};
// -------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------
template<typename T>
struct iterator_base
{
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

   inline size_t pos() const { return pos_; }         
   inline size_t pos()       { return pos_; }
   
   inline T &    obj() const { return obj_; }
   inline T &    obj()       { return obj_; }   

   T &    obj_;
   size_t pos_;   
};
// -------------------------------------------------------------------------------------------

} // end namespace viennamesh

#endif