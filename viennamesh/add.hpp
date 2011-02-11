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


#ifndef VIENNAMESH_ADD_HPP
#define VIENNAMESH_ADD_HPP

namespace viennamesh {

template<typename ObjT>   
struct add   
{
   add(ObjT& obj) : obj(obj) {}
   
   template<typename EleT>
   void operator()(EleT const& ele)
   {
      obj.add(ele);
   }   
   
   template<typename EleT>
   void operator()(EleT& ele)
   {
      obj.add(ele);
   }
   
   ObjT & obj;
};
   
  
// namespace detail {
//    
// template<typename ObjT>
// struct add_unary
// {
//    add_unary(ObjT& obj) : obj(obj) {};
//    
//    template<typename EleT>
//    void operator()(EleT& ele)
//    {
//       obj.add(ele);
//    }
//    
//    ObjT & obj;
// };
// 
// } // end namespace detail
// 
// namespace result_of {
// 
// template <typename ObjT>
// struct add
// {
//    typedef viennamesh::detail::add_unary<ObjT> type;
// };
//    
// }// end namespace result_of
// 
// template <typename ObjT>
// inline viennamesh::detail::add_unary<ObjT>
// add(ObjT & obj)
// {
//    return viennamesh::detail::add_unary<ObjT>(obj);
// }
   
} // end namespace viennamesh   

#endif