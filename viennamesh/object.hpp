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


#ifndef VIENNAMESH_OBJECT_HPP
#define VIENNAMESH_OBJECT_HPP

#include <boost/fusion/include/make_map.hpp>

namespace viennamesh {

// ---------------------------------------------------------------------------   
template<typename K0, typename T0>
inline 
typename boost::fusion::result_of::make_map<K0, T0>::type
object(T0 const & t0)
{
   return boost::fusion::make_map<K0>(t0);
}


template<typename K0, typename T0>
inline 
typename boost::fusion::result_of::make_map<K0, T0>::type
object(T0 & t0)
{
   return boost::fusion::make_map<K0>(t0);
}

// TODO think about activating more than one pair for the viennamesh::object
// the boost pair would be sufficient, for now.
// however, using fusion::map supports future extensions ..


// ---------------------------------------------------------------------------   
// template<typename K0, typename K1, typename T0, typename T1>
// inline 
// typename boost::fusion::result_of::make_map<K0,K1,T0,T1>::type
// object(T0 const & t0, T1 const& t1)
// {
//    return boost::fusion::make_map<K0, K1>(t0, t1);
// }
// 
// 
// template<typename K0, typename K1, typename T0, typename T1>
// inline 
// typename boost::fusion::result_of::make_map<K0,K1,T0,T1>::type
// object(T0 & t0, T1 & t1)
// {
//    return boost::fusion::make_map<K0, K1>(t0, t1);
// }
// ---------------------------------------------------------------------------   

} // end namespace viennamesh   
#endif