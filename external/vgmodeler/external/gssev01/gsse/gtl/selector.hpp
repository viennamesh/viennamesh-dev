/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_DATATYPE_MAPS
#define GSSE_DATATYPE_MAPS

// ** system includes
//
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <functional>

// ** BOOST includes
//
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/at.hpp>
#include <boost/utility/enable_if.hpp>


// ** GSSE includes
//
#include "gsse/domain.hpp"

#include "gsse/debug_meta.hpp"   // [RH][TODO][temporary]


namespace gsse {

// have to implement an iterator wrapper
//
// this iterator wrapper should not access the values 
//  instead it should only return the given access mechanism
//  e.g., vector ->  index
//  e.g., map    ->  key


// can only be used if the index type = long
//
template<typename IndexType, typename NumericT>
struct stl_vector
{
  typedef IndexType                          index_type;
  typedef IndexType                          value_type;   // [RH][TODO].. value type not the best choice (traverse.hpp)
  typedef NumericT                           storage_type;
 
  struct iterator_inside
  {
  	typedef long   value_type;
	iterator_inside(stl_vector<IndexType, NumericT>* container, value_type status):container(container),status(status){}
        
	bool        operator==(iterator_inside& other)   { return status == other.status;}
	bool        operator!=(iterator_inside& other)   { return !(status == other.status);}
	void        operator++()   { ++ status; }
        value_type  operator*()    { return status;}
    private:
     stl_vector<IndexType,NumericT>*  container;
     value_type                       status;
  };
  typedef iterator_inside                   iterator;

  iterator begin() { return iterator_inside(this, 0); }
  iterator end()   { return iterator_inside(this,container.size()); }

  stl_vector(IndexType size=0, NumericT startvalue=0)
  {
	container = std::vector<NumericT>(size, startvalue);
  }
  
  void insert(IndexType index, NumericT val)
  {
  	if ( static_cast<size_t>(index+1) >= container.size())
	  container.resize(index+1);

	container[index]=val;
  }
  NumericT& operator[](IndexType index)
  {
	return container[index];
  }
  NumericT& operator()(IndexType index)  //   to be compatible with other lambda mechanisms
  {
	return container[index];
  }

   size_t size() 
      {
         return container.size();
      }


 friend std::ostream& operator<<(std::ostream& ostr, stl_vector cr)
 {    
 	long index = 0;
	for (typename stl_vector::iterator c_it = cr.container.begin(); c_it != cr.container.end(); ++c_it, ++index)
	{
		ostr << index << " / " << *c_it << std::endl;
	}
	return ostr;
 }



protected:
  std::vector<NumericT>       container;
};


template<typename IndexType, typename NumericT>
struct stl_set
{
  typedef IndexType                          index_type;
  typedef IndexType                          value_type;   // [RH][TODO].. value type not the best choice (traverse.hpp)
  typedef NumericT                           storage_type;
 
  struct iterator_inside
  {
  	typedef long   value_type;
	iterator_inside(stl_set<IndexType, NumericT>* container, value_type status):container(container),status(status){}
        
	bool        operator==(iterator_inside& other)   { return status == other.status;}
	bool        operator!=(iterator_inside& other)   { return !(status == other.status);}
	void        operator++()   { ++ status; }
        value_type  operator*()    { return status;}
    private:
     stl_set<IndexType,NumericT>*  container;
     value_type                       status;
  };
  typedef iterator_inside                   iterator;

  iterator begin() { return iterator_inside(this, 0); }
  iterator end()   { return iterator_inside(this,container.size()); }

   size_t size() 
      {
         return container.size();
      }
   
   void insert(IndexType index, NumericT val)
      {
         container.insert(val);
      }
   
//   NumericT& operator[](IndexType index)
//   {
// 	return container[index];
//   }
//   NumericT& operator()(IndexType index)  //   to be compatible with other lambda mechanisms
//   {
// 	return container[index];
//   }


 friend std::ostream& operator<<(std::ostream& ostr, stl_set cr)
 {    
 	long index = 0;
	for (typename stl_set::iterator c_it = cr.container.begin(); c_it != cr.container.end(); ++c_it, ++index)
	{
//		ostr << index << " / " << *c_it << std::endl;
	}
	return ostr;
 }

protected:
  std::set<NumericT>       container;
};



template<typename IndexType, typename NumericT>
struct stl_map
{
  typedef IndexType                          index_type;
  typedef IndexType                          value_type;
  typedef NumericT                           storage_type;

// [RH][TODO] .. extend the map's iterator inside to a real map iterator
//
  struct iterator_inside
  {
  	typedef long   value_type;
	iterator_inside(stl_map<IndexType, NumericT>* container, value_type status):container(container),status(status){}
        
	bool        operator==(iterator_inside& other)   { return status == other.status;}
	bool        operator!=(iterator_inside& other)   { return !(status == other.status);}
	void        operator++()   { ++ status; }
        value_type  operator*()    { return status;}
    private:
     stl_map<IndexType,NumericT>*  container;
     value_type                    status;
  };
  typedef iterator_inside                   iterator;

  iterator begin() { return iterator_inside(this, 0); }
  iterator end()   { return iterator_inside(this,container.size()); }

  stl_map(IndexType size=0, NumericT startvalue=0)
  {
  }

   size_t size() 
      {
         return container.size();
      }
  

  void insert(IndexType index, NumericT val)
  {
	container[index]=val;
  }
  NumericT& operator[](IndexType index)
  {
	return container[index];
  }
  NumericT& operator()(IndexType index)
  {
	return container[index];
  }



 friend std::ostream& operator<<(std::ostream& ostr, stl_map cr)
 {
	for (typename stl_map::iterator c_it = cr.container.begin(); c_it != cr.container.end(); ++c_it)
	{
//		ostr << (*c_it).first << " / " << (*c_it).second << std::endl;
	}
	return ostr;
 }

protected:
  std::map<IndexType, NumericT>                             container;
};



// =========================================================================================================
// 
// gsse domain:: preparation data types for  lambda style
//

// 20080115:: new non-intrusive / intrusive (gsse) container type deducation
//   can be easily extended to all types of container structures (non-intrusively) .. 
//
   template <typename ContainerInformation>
   struct is_container 
   {
      enum { value = false };  
   };
   
   template <>
   struct is_container<gsse_container>
   {
      enum { value = true };  
   };

   // [RH] .. required for the gsse::traversal and access mechanism
   //   .. due to the partial specializatio rule
   //      we have to use the full container information
   //
   template <typename ContainerInformation, typename CTInformation1, typename CTInformation2>
   struct is_container_stl 	
   {
      enum { value = false };  
   };
   template <typename CTInformation1, typename CTInformation2>
   struct is_container_stl< stl_vector<CTInformation1, CTInformation2>, CTInformation1, CTInformation2  > 	
   {
      enum { value = true };  
   };
   template <typename CTInformation1, typename CTInformation2>
   struct is_container_stl< stl_set<CTInformation1, CTInformation2>, CTInformation1, CTInformation2  > 	
   {
      enum { value = true };  
   };
   template <typename CTInformation1, typename CTInformation2>
   struct is_container_stl< stl_map<CTInformation1, CTInformation2>, CTInformation1, CTInformation2  > 	
   {
      enum { value = true };  
   };






// ================================================================
// ================================================================

   template<typename OriginT>
   struct extra_traversal_information
   {
   typedef boost::mpl::map<
      boost::mpl::pair<boost::mpl::pair<typename OriginT::vertex_type, typename OriginT::edge_type >,    typename OriginT::vertex_on_edge_iterator  >,
      boost::mpl::pair<boost::mpl::pair<typename OriginT::vertex_type, typename OriginT::facet_type >,   typename OriginT::vertex_on_facet_iterator >,
      boost::mpl::pair<boost::mpl::pair<typename OriginT::vertex_type, typename OriginT::cell_type >,    typename OriginT::vertex_on_cell_iterator  >,

      boost::mpl::pair<boost::mpl::pair<typename OriginT::edge_type,   typename OriginT::vertex_type >,  typename OriginT::edge_on_vertex_iterator  >,
      boost::mpl::pair<boost::mpl::pair<typename OriginT::edge_type,   typename OriginT::facet_type >,   typename OriginT::edge_on_facet_iterator >,
      boost::mpl::pair<boost::mpl::pair<typename OriginT::edge_type,   typename OriginT::cell_type >,    typename OriginT::edge_on_cell_iterator >,
 
      boost::mpl::pair<boost::mpl::pair<typename OriginT::facet_type,  typename OriginT::vertex_type >,  typename OriginT::facet_on_vertex_iterator  >,
      boost::mpl::pair<boost::mpl::pair<typename OriginT::facet_type,  typename OriginT::edge_type >,    typename OriginT::facet_on_edge_iterator >,
      boost::mpl::pair<boost::mpl::pair<typename OriginT::facet_type,  typename OriginT::cell_type >,    typename OriginT::facet_on_cell_iterator >,

      boost::mpl::pair<boost::mpl::pair<typename OriginT::cell_type,   typename OriginT::vertex_type >,  typename OriginT::cell_on_vertex_iterator  >,
      boost::mpl::pair<boost::mpl::pair<typename OriginT::cell_type,   typename OriginT::edge_type >,    typename OriginT::cell_on_edge_iterator  >,
      boost::mpl::pair<boost::mpl::pair<typename OriginT::cell_type,   typename OriginT::facet_type >,   typename OriginT::cell_on_facet_iterator  >

   > traversal_repository;

  };




template<typename BaseType, typename TraversalObjectT>
struct default_functor_type
{
   typedef TraversalObjectT                        iterator;
   typedef typename TraversalObjectT::value_type   value_type;
   
   default_functor_type(const BaseType& base) : traversal_obj(base) 
      { 
//          dump_type<TraversalObjectT>();
//          dump_type<BaseType>();
      }

   iterator begin() const { return traversal_obj;}
   iterator end()         { return traversal_obj.end(); }
protected:
   TraversalObjectT traversal_obj;
};




// ### for stl containers
//
template<typename ContainerT>
struct traverse_stl_vertex
{
   typedef typename ContainerT::iterator     iterator;
   typedef typename ContainerT::value_type   value_type;
   
   traverse_stl_vertex(ContainerT& container) : container(container) {}
   
   iterator begin() { return container.begin();}
   iterator end()   { return container.end(); }
protected:
   ContainerT& container;
};


// #### simple meta function for selecting
//  a deduced traversal object
//  e.g, vertex_on_cell_iterator
//       by specifying just vertex, cell
// 
template<typename DomainType, typename TraversalObjectDeduced, typename TraversalObject>
struct get_traversal_object
{
  typedef  typename  boost::mpl::at<typename gsse::extra_traversal_information<DomainType>::traversal_repository, 
                                    boost::mpl::pair<TraversalObjectDeduced, TraversalObject > >::type  type; 
//    example                                                vertex             cell

};

// ===========================================================================================================================
// ############                                       DEDUCED TRAVERSAL                                          #############
// ############                              for non-containers, e.g., vertex on cell                            #############
// ===========================================================================================================================
//
//    this meta-function is used to select between intrinsic / deduced traversal mechanisms
//     ..example::   cell              vertex_on_cell
//
template<typename TraversalObject, typename TraversalSubType, typename EvalType=void>
struct init_functor_switch
{

   //               example                                          cell :: us_topo     vertex          cell     -> vertex_on_cell
   //             
   typedef typename gsse::get_traversal_object<typename TraversalObject::base_type,TraversalSubType, TraversalObject>::type  new_type;

   typedef default_functor_type<TraversalObject, new_type> type;

};

// [RH] newly introduced to fullfill [PS] naming scheme
//
template<typename TraversalObject, typename TraversalSubType>
struct deduced_traversal
{

   //               example                                          cell :: us_topo     vertex          cell     -> vertex_on_cell
   //             
   typedef typename gsse::get_traversal_object<typename TraversalObject::base_type,TraversalSubType, TraversalObject>::type  iterator;
   typedef typename iterator::value_type   value_type;
   
   deduced_traversal(const TraversalObject& base) : traversal_obj(base) 
      { 
//          dump_type<TraversalObjectT>();
//          dump_type<BaseType>();
      }

   iterator begin() const { return traversal_obj;}
   iterator end()         { return traversal_obj.end(); }
protected:
   iterator traversal_obj;


};




// ===========================================================================================================================
// ############                                    INTRINSIC TRAVERSAL                                           #############
// ############                                       for containers                                             #############
// ===========================================================================================================================
//
//     ...   stl::container, bgl::container, gral::container, gsse::container
//     e.g., where a indirection have to be used to access the nested object      

//
//     ### GSSE container  e.g., where a indirection have to be used to access the nested object      
//
template<typename TraversalObject, typename TraversalSubType>
struct init_functor_switch<TraversalObject, TraversalSubType, 
    typename boost::enable_if<is_container<typename TraversalObject::gsse_type> >::type >
{
   typedef typename boost::mpl::at<typename TraversalObject::traversal_repository, boost::mpl::pair<TraversalObject, TraversalSubType> >::type type;

};


// [RH] convenience meta-function for [PS]
//
template<typename TraversalObject, typename TraversalSubType>
struct intrinsic_traversal 
{
   typedef typename boost::mpl::at<typename TraversalObject::traversal_repository, boost::mpl::pair<TraversalObject, TraversalSubType> >::type type;

};




//     ### stl container
//
template<typename TraversalObject, typename TraversalSubType>
struct init_functor_switch<TraversalObject, TraversalSubType, 
     typename boost::enable_if<is_container_stl< TraversalObject, typename TraversalObject::index_type, typename TraversalObject::storage_type > >::type >
{
   typedef traverse_stl_vertex<TraversalObject> type;
};



} // namespace gsse



#endif 
