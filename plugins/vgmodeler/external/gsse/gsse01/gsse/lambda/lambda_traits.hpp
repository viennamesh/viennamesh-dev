/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef LAMBDA_TRAITS_HH
#define LAMBDA_TRAITS_HH

// *** GSSE includes
//
#include "gsse/domain.hpp"
#include "gsse/lambda/info_iterator.hpp"
#include "gsse/lambda/information_tuple.hpp"
#include "gsse/lambda/quan_access.hpp"



namespace gsse
{

  // standard definition tags for domain indepent iterator access

  struct segment_vertex{};
  struct segment_edge{};
  struct segment_facet{};
  struct segment_cell{};
  struct vertex_segment{};
  struct vertex_cell{};
  struct vertex_edge{};
  struct vertex_facet{};
  struct edge_vertex{};
  struct edge_facet{};
  struct edge_cell{};
  struct facet_vertex{};
  struct facet_edge{};
  struct facet_cell{};
  struct cell_vertex{};
  struct cell_edge{};
  struct cell_facet{};

  struct vertex_edge_left{};
  struct vertex_edge_x{};
  struct vertex_edge_y{};
  struct vertex_edge_z{};


template <typename DomainT>
struct lambda_traits
{
  // The lambda traits provide iterator base for all common
  // iterators. All iterators are implemented in the info_iterator.hh
  // header file. For the reason of convenience all iterator classes
  // can be accessed using a unique interface. 

  // All the commented features are available in the stable patch but
  // not in the PHOENIX2 development patch. After a merge all of this
  // has to work. 

  typedef typename domain_traits<DomainT>::vertex_iterator vertex_on_segment_iterator;
  typedef typename domain_traits<DomainT>::cell_iterator   cell_on_segment_iterator;
  typedef typename domain_traits<DomainT>::edge_iterator   edge_on_segment_iterator;
  typedef typename domain_traits<DomainT>::facet_iterator  facet_on_segment_iterator;
  
  typedef info_iter<vertex_on_segment_iterator, 
		    vertex_begin_functor<vertex_on_segment_iterator>, 
		    merge_element> segment_vertex;
  
  typedef info_iter<cell_on_segment_iterator, 
		    cell_begin_functor<cell_on_segment_iterator>, 
		    merge_element> segment_cell;

  typedef info_iter<edge_on_segment_iterator, 
		    edge_begin_functor<edge_on_segment_iterator>, 
		    merge_element> segment_edge;

  typedef info_iter<facet_on_segment_iterator, 
		    facet_begin_functor<facet_on_segment_iterator>, 
		    merge_element> segment_facet;


private:
   
  typedef typename domain_traits<DomainT>::quan_key_type    quan_key_t;
  typedef typename domain_traits<DomainT>::segment_iterator segment_iterator;
  
public:
   
  typedef information_tuple<no_type, quan_key_t, no_type>           info_tuple_key;
  typedef information_tuple<segment_iterator, no_type, no_type>     info_tuple_segment;
  
//   typedef typename gsse::scalar_quan_t<info_tuple_key>::type        key_access;
//   typedef typename gsse::scalar_quan_create_t<info_tuple_key>::type key_access_create;

  typedef typename domain_traits<DomainT>::cell_on_vertex_iterator  cell_on_vertex_iterator;
  typedef typename domain_traits<DomainT>::edge_on_vertex_iterator  edge_on_vertex_iterator;
  typedef typename domain_traits<DomainT>::facet_on_vertex_iterator facet_on_vertex_iterator;
  typedef typename domain_traits<DomainT>::cell_on_edge_iterator    cell_on_edge_iterator;
  typedef typename domain_traits<DomainT>::vertex_on_edge_iterator  vertex_on_edge_iterator;
  typedef typename domain_traits<DomainT>::facet_on_edge_iterator   facet_on_edge_iterator;
  typedef typename domain_traits<DomainT>::cell_on_facet_iterator   cell_on_facet_iterator;
  typedef typename domain_traits<DomainT>::edge_on_facet_iterator   edge_on_facet_iterator;
  typedef typename domain_traits<DomainT>::vertex_on_facet_iterator vertex_on_facet_iterator;
  typedef typename domain_traits<DomainT>::vertex_on_cell_iterator  vertex_on_cell_iterator;
  typedef typename domain_traits<DomainT>::edge_on_cell_iterator    edge_on_cell_iterator;
  typedef typename domain_traits<DomainT>::facet_on_cell_iterator   facet_on_cell_iterator;

  typedef typename domain_traits<DomainT>::vertex_type vertex_type;
  typedef typename domain_traits<DomainT>::edge_type   edge_type;
  typedef typename domain_traits<DomainT>::facet_type  facet_type;
  typedef typename domain_traits<DomainT>::cell_type   cell_type;
  
  typedef info_iter<cell_on_vertex_iterator,  elem_functor<vertex_type>, merge_element> vertex_cell;
  typedef info_iter<edge_on_vertex_iterator,  elem_functor<vertex_type>, merge_element> vertex_edge;
  typedef info_iter<facet_on_vertex_iterator, elem_functor<vertex_type>, merge_element> vertex_facet;
  
  typedef info_iter<vertex_on_edge_iterator,  elem_functor<edge_type>,   merge_element> edge_vertex;
  typedef info_iter<facet_on_edge_iterator,   elem_functor<edge_type>,   merge_element> edge_facet;
  typedef info_iter<cell_on_edge_iterator,    elem_functor<edge_type>,   merge_element> edge_cell;
  
  typedef info_iter<vertex_on_facet_iterator, elem_functor<facet_type>,  merge_element> facet_vertex;
  typedef info_iter<edge_on_facet_iterator,   elem_functor<facet_type>,  merge_element> facet_edge;
  typedef info_iter<cell_on_facet_iterator,   elem_functor<facet_type>,  merge_element> facet_cell;
  
  typedef info_iter<vertex_on_cell_iterator, elem_functor<cell_type>,   merge_element>  cell_vertex;
  typedef info_iter<facet_on_cell_iterator,  elem_functor<cell_type>,   merge_element>  cell_facet;
  typedef info_iter<edge_on_cell_iterator,   elem_functor<cell_type>,   merge_element>  cell_edge;
};


template <typename DomainT>
struct structured_lambda_traits
{
  // The lambda traits provide iterator base for all common
  // iterators. All iterators are implemented in the info_iterator.hh
  // header file. For the reason of convenience all iterator classes
  // can be accessed using a unique interface. 

  // All the commented features are available in the stable patch but
  // not in the PHOENIX2 development patch. After a merge all of this
  // has to work. 
   
public:
      
   typedef typename domain_traits<DomainT>::vertex_type vertex_type;
   typedef typename domain_traits<DomainT>::edge_type   edge_type;
   typedef typename domain_traits<DomainT>::facet_type  facet_type;
   typedef typename domain_traits<DomainT>::cell_type   cell_type;

   //typedef typename domain_traits<DomainT>::edge_on_vertex_iterator  edge_on_vertex_left_iterator;  

   typedef typename domain_traits<DomainT>::template edge_on_vertex_direction_left_iterator<1>::type  edge_on_vertex_left_iterator;  
   typedef info_iter<edge_on_vertex_left_iterator,  elem_functor<vertex_type>, merge_element> vertex_edge_left;


   typedef typename domain_traits<DomainT>::template edge_on_vertex_direction_iterator<1>::type  edge_on_vertex_x_iterator;  
   typedef info_iter<edge_on_vertex_x_iterator,  elem_functor<vertex_type>, merge_element> vertex_edge_x;

   typedef typename domain_traits<DomainT>::template edge_on_vertex_direction_iterator<2>::type  edge_on_vertex_y_iterator;  
   typedef info_iter<edge_on_vertex_y_iterator,  elem_functor<vertex_type>, merge_element> vertex_edge_y;

   typedef typename domain_traits<DomainT>::template edge_on_vertex_direction_iterator<3>::type  edge_on_vertex_z_iterator;  
   typedef info_iter<edge_on_vertex_z_iterator,  elem_functor<vertex_type>, merge_element> vertex_edge_z;

};

  // the lambda iterator provides a full de-coupling between the
  // topology-independent Iterator Tags (listed above) and the
  // topology dependent information iterators. This indirection step
  // allows us to formulate iterations completely independent from the
  // underlying data structure. 

  template <typename DomainT, typename Tag> struct lambda_iterator {};
  
  template <typename DomainT> struct lambda_iterator<DomainT, segment_vertex> 
  { typedef typename lambda_traits<DomainT>::segment_vertex type;};

  template <typename DomainT> struct lambda_iterator<DomainT, segment_edge> 
  { typedef typename lambda_traits<DomainT>::segment_edge type;};

  template <typename DomainT> struct lambda_iterator<DomainT, segment_facet> 
  { typedef typename lambda_traits<DomainT>::segment_facet type;};

  template <typename DomainT> struct lambda_iterator<DomainT, segment_cell> 
  { typedef typename lambda_traits<DomainT>::segment_cell type;};


  template <typename DomainT> struct lambda_iterator<DomainT, vertex_edge> 
  { typedef typename lambda_traits<DomainT>::vertex_edge type;};

  template <typename DomainT> struct lambda_iterator<DomainT, vertex_facet> 
  { typedef typename lambda_traits<DomainT>::vertex_facet type;};

  template <typename DomainT> struct lambda_iterator<DomainT, vertex_cell> 
  { typedef typename lambda_traits<DomainT>::vertex_cell type;};


  template <typename DomainT> struct lambda_iterator<DomainT, edge_vertex> 
  { typedef typename lambda_traits<DomainT>::edge_vertex type;};

  template <typename DomainT> struct lambda_iterator<DomainT, edge_facet> 
  { typedef typename lambda_traits<DomainT>::edge_facet type;};

  template <typename DomainT> struct lambda_iterator<DomainT, edge_cell> 
  { typedef typename lambda_traits<DomainT>::edge_cell type;};


  template <typename DomainT> struct lambda_iterator<DomainT, facet_vertex> 
  { typedef typename lambda_traits<DomainT>::facet_vertex type;};

  template <typename DomainT> struct lambda_iterator<DomainT, facet_edge> 
  { typedef typename lambda_traits<DomainT>::facet_edge type;};

  template <typename DomainT> struct lambda_iterator<DomainT, facet_cell> 
  { typedef typename lambda_traits<DomainT>::facet_cell type;};


  template <typename DomainT> struct lambda_iterator<DomainT, cell_vertex> 
  { typedef typename lambda_traits<DomainT>::cell_vertex type;};

  template <typename DomainT> struct lambda_iterator<DomainT, cell_edge> 
  { typedef typename lambda_traits<DomainT>::cell_edge type;};

  template <typename DomainT> struct lambda_iterator<DomainT, cell_facet> 
  { typedef typename lambda_traits<DomainT>::cell_facet type;};



  template <typename DomainT> struct lambda_iterator<DomainT, vertex_edge_left> 
  { typedef typename structured_lambda_traits<DomainT>::vertex_edge_left type;};


  template <typename DomainT> struct lambda_iterator<DomainT, vertex_edge_x> 
  { typedef typename structured_lambda_traits<DomainT>::vertex_edge_x type;};

   template <typename DomainT> struct lambda_iterator<DomainT, vertex_edge_y> 
  { typedef typename structured_lambda_traits<DomainT>::vertex_edge_y type;};

  template <typename DomainT> struct lambda_iterator<DomainT, vertex_edge_z> 
  { typedef typename structured_lambda_traits<DomainT>::vertex_edge_z type;};

}
#endif
