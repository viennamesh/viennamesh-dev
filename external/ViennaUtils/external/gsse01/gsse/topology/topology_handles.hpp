/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
 
     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef TOPOLOGY_HANDLES_HH_ID
#define TOPOLOGY_HANDLES_HH_ID

#include <iostream>

namespace gsse
{

namespace detail_topology 
{

/// @brief Vertex object handle type
///
/// This is always the lowest dimensional object
///

template<class Topology>  
struct vertex_handle_int
{
  typedef vertex_handle_int<Topology> self;
  typedef Topology topology_type;

  int h;

  vertex_handle_int()               {}
  vertex_handle_int(int hh) : h(hh) {}
 
  operator int()  const { return h;}
  self& operator=(int hh) { h = hh; return *this;}
  self& operator++() { ++h; return *this;}
  self& operator--() { --h; return *this;}
  self& operator+=(int d) { h+=d; return *this;}
  self& operator-=(int d) { h-=d; return *this;} 
  bool  operator<(self& d) { return h < d.h;} 

 friend std::ostream& operator<<(std::ostream& ostr, vertex_handle_int vhi)
 {
 	ostr << vhi.h;
	return ostr;
 }
 friend std::istream& operator>>(std::istream& istr, vertex_handle_int& vhi)
 {
 	istr >> vhi.h;
	return istr;
 }
};

  ///
  /// @brief Edge object handle type
  ///
  template<class Topology>  
  struct edge_handle_int
  {
    typedef edge_handle_int<Topology> self;
    typedef Topology topology_type;
    int h;
    edge_handle_int() {}
    edge_handle_int(int hh) : h(hh) {}
 
    operator int()  const { return h;}
    self& operator=(int hh) { h = hh; return *this;}
    self& operator++() { ++h; return *this;}
    self& operator--() { --h; return *this;}
    self& operator+=(int d) { h+=d; return *this;}
    self& operator-=(int d) { h-=d; return *this;} 
  };

  ///
  /// @brief Facet object handle type
  ///
  template<class Topology>  
  struct facet_handle_int
  {
    typedef facet_handle_int<Topology> self;
    typedef Topology topology_type;
    int h;

    facet_handle_int() {}
    facet_handle_int(int hh) : h(hh) {}
 
    operator int()  const { return h;}
    self& operator=(int hh) { h = hh; return *this;}
    self& operator++() { ++h; return *this;}
    self& operator--() { --h; return *this;}
    self& operator+=(int d) { h+=d; return *this;}
    self& operator-=(int d) { h-=d; return *this;} 
  };


  /// @brief Cell object handle type
  ///
  /// This is always the highest dimensional object
  ///
  template<class Topology> 
  struct cell_handle_int
  {
    typedef cell_handle_int<Topology> self;
    typedef Topology topology_type;
    int h;
    cell_handle_int() {}
    cell_handle_int(int hh) : h(hh) {}
 
    operator int()  const { return h;}
    self& operator=(int hh) { h = hh; return *this;}
    self& operator++() { ++h; return *this;}
    self& operator--() { --h; return *this;}
    self& operator+=(int d) { h+=d; return *this;}
    self& operator-=(int d) { h-=d; return *this;}
  };

} // end of namespace::detail_topology

} // end of namespace::gsse 
#endif
