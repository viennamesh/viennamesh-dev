/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_QUANTITY_SPARSE_HH_ID
#define GSSE_QUANTITY_SPARSE_HH_ID

// *** system includes
//
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

// *** BOOST includes
//
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/counting_iterator.hpp>

// *** GSSE includes
//
#include "gsse/math/matrix.hpp"
#include "gsse/iterator.hpp"
#include "gsse/exceptions.hpp"
#include "gsse/quantity_traits.hpp"
 

namespace gsse {

///////////////////////////////////////////////////////////////////////////
/// @brief GSSE quantity storage system
///
/// This system can access the stored quantities with two different mechanisms
///   1) return all points from a quantity
///   2) return all quantities from a point
///
/// To handle this feature the quantities are stored within a 2D memory
/// array. 
/// For an easy usage a pseudo associative initialisation is implemented.
///
/// The first quantity name access generates a corresponding column. All
/// afterward accesses are sorted correctly inside this memory array.
///
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
/// @brief GSSE Quantity storage system
///
/// Developed for sparse systems ..
///
///////////////////////////////////////////////////////////////////////////


template<typename HandleType, typename KeyType, typename StorageType>
class gsse_quantity_sparse
{

public:
  typedef StorageType  pattern_type;          // [A1]
  typedef StorageType  storage_type;           // [1]
  typedef HandleType       object_handle_type;    // [3]
  typedef KeyType          quan_key_type;         // [4]

   //typedef double           numeric_type;       // [2]

private:

  // mapping of all internal names
   typedef std::map<quan_key_type, storage_type>           internal_name_t;

  // set of all handles
   typedef std::set<object_handle_type>                   internal_handle_t;

  // map quan_on_handle[object_handle_type][quan_key_type] = Data_PatternType
   typedef std::map<object_handle_type, internal_name_t > quan_on_handle_t;

  // stores on which name handles exist 
   typedef std::map<quan_key_type,  internal_handle_t>    handle_on_quan_t;


public:

   typedef typename std::vector<quan_key_type>::const_iterator key_iterator;     // C [12]
   
   key_iterator key_begin() const {return quantity_names.begin();}               // C [13]
   key_iterator key_end()   const {return quantity_names.end();}                 // C [14]   

  typedef typename quan_on_handle_t::const_iterator pre_handle_iterator;               
  
  template <typename PairType>
  struct member_var_first
  {
    typedef typename PairType::first_type result_type;
    
    typename PairType::first_type&
    operator()(PairType const& p) const 
    {
      return p.first;
    }
 };
  
  typedef member_var_first<typename pre_handle_iterator::value_type> first_functor;
  typedef boost::transform_iterator <first_functor, pre_handle_iterator, object_handle_type> 
  internal_handle_iterator;   // [15]
  
   internal_handle_iterator internal_handle_begin() 
      const {return internal_handle_iterator(quan_on_handle.begin());}       // [16]
   internal_handle_iterator internal_handle_end()   
      const {return internal_handle_iterator(quan_on_handle.end());}         // [17]
  
   typedef gsse_iterator<internal_handle_iterator> handle_iterator;

   handle_iterator handle_begin() const 
      {return handle_iterator(internal_handle_begin(), internal_handle_begin(), internal_handle_end());}
   
   handle_iterator handle_end() const 
      {return handle_iterator(internal_handle_end(), internal_handle_begin(), internal_handle_end());}
   

  // default constructor works fine here [9]
  
public:
  //   typedef typename internal_handle_t::iterator           quantity_entry_iterator_column;
  //   typedef typename internal_name_t::iterator             pre_quantity_entry_iterator;
  //
  //
  //

  /// @brief adds a quantity on an object
  ///
  /// @param  object_index           the unqiue id from an object (i.e. vertex, edge, cell, ..)
  /// @param  quan_name              the name of a quantity
  /// @param  value                  should be stored within the quantity memory array
  ///
  /// @throws nothing
  /// @see    getIndexFromQuanName
  ///

  void insert(const object_handle_type& object_index, 
	      const quan_key_type&      quan_name, 
	      const storage_type&        value )                // [10]
  {
//      std::cout << "## debug quan.hpp.. insert method.. "<< std::endl;
//      std::cout << " ### object index: " << object_index  << std::endl;
//      std::cout << " ### quan name:    " << quan_name << std::endl;
//      std::cout << " ### value:        " << value << std::endl;

    quan_on_handle[object_index][quan_name] = value;
    handle_on_quan[quan_name].insert(object_index);

    if (std::find(quantity_names.begin(), quantity_names.end(), quan_name) == quantity_names.end())
      {
	quantity_names.push_back(quan_name);
	
      }
  }
   
   /// @brief adds a quantity on all objects
   ///
   /// If the quantity does not exist, the getIndexFromQuanName creates a corresponding column
   /// 
   ///
   /// @param  quan_name              the name of a quantity
   /// @param  value                  should be stored within the quantity memory array
   ///
   /// @returns: true, if the add was successfull
   ///           false, if the quantity was already inserted
   /// @throws nothing
   /// @see    getIndexFromQuanName
   ///

   void insert(const std::string& quan_name, storage_type value)  // [11]
   {
      typename quan_on_handle_t::iterator  qohit(quan_on_handle.begin());
      typename quan_on_handle_t::iterator  qohit_end(quan_on_handle.end());

      for (; qohit != qohit_end; ++qohit)
      {
         ((*qohit).second)[quan_name] = value;
         handle_on_quan[quan_name].insert( (*qohit).first  );
      }
   }


   /// @brief direct access to quantities
   ///
   /// Direct mechanism for accessing the stored data pattern types.
   /// Here, the ranges are checked. Use this method, if you are not sure if the indices are valid.
   ///
   /// @param  quan_name         the name of a quantity
   /// @param  value            should be stored within the quantity memory array
   /// 
   /// @throws                  
   /// @throws                  
   ///
   storage_type& retrieve_quantity(const object_handle_type&  object_index, 
				  const quan_key_type&       quan_name)  // [21]
   {
      typename quan_on_handle_t::iterator  qon_it ( quan_on_handle.find(object_index) );
      if ( qon_it != quan_on_handle.end())
      {   // first key found
             
//          std::cout << "first key found: " << quan_name  <<  std::endl;

         typename internal_name_t::iterator  in_it ( (*qon_it).second.find(quan_name));
         if ( in_it !=  (*qon_it).second.end())
         {  
            //  second key found
            //
	   return (*in_it).second;
         }
         else 
         {
	 
	   insert(object_index, quan_name, storage_type());   // default initializiation of storage_type

	   return quan_on_handle[object_index][quan_name];
	   //throw invalid_quantity_name(quan_name);
         }
      }
      else
      {
         throw invalid_object_index(object_index);
      }
   }

   /// @brief direct access to quantities
   ///
   /// Direct mechanism for accessing the stored data pattern types.
   /// Here, the ranges are checked. Use this method, if you are not sure if the indices are valid.
   ///
   /// @param  quan_name         the name of a quantity
   /// @param  value            should be stored within the quantity memory array
   /// 
   /// @throws                  
   /// @throws                  
   ///
   storage_type& retrieve_quantity_notautomatic(const object_handle_type&  object_index, 
					       const quan_key_type&       quan_name) 
   {
      typename quan_on_handle_t::iterator  qon_it ( quan_on_handle.find(object_index) );
      if ( qon_it != quan_on_handle.end())
      {   // first key found
             
         typename internal_name_t::iterator  in_it ( (*qon_it).second.find(quan_name));
         if ( in_it !=  (*qon_it).second.end())
         {  //second key found
	   return (*in_it).second;
         }
         else 
         {
	   throw invalid_quantity_name(quan_name);
         }
      }
      else
      {
	  throw invalid_object_index(object_index);
      }
   }


   /// @brief direct access to quantities
   ///
   /// Direct mechanism for accessing the stored data pattern types.
   /// Here, the ranges are checked. Use this method, if you are not sure if the indices are valid.
   ///
   /// @param  quan_name         the name of a quantity
   /// @param  value            should be stored within the quantity memory array
   /// 
   /// @throws                  
   /// @throws                  
   ///
   const storage_type& retrieve_quantity(const object_handle_type&  object_index, 
					const quan_key_type&        quan_name)   const // [21]
  {
      typename quan_on_handle_t::const_iterator  qon_it ( quan_on_handle.find(object_index) );
      if ( qon_it != quan_on_handle.end())
	{   // first key found
	  
	  typename internal_name_t::const_iterator  in_it ( (*qon_it).second.find(quan_name));
	  if ( in_it !=  (*qon_it).second.end())
	    {  //second key found
	      return (*in_it).second;
	    }
	  else 
	    {
	      throw invalid_quantity_name(quan_name);
	    }
	}
      else
	{
	  throw invalid_object_index(object_index);
 	}
   }
  
  
   /// @brief direct access 
   ///
   /// BE CAREFUL.. this operator does not check the validity ..
   /// only for high speed operations
   ///
   ///  !!! possible segmentation fault !!!!!
   ///
   storage_type& operator()(const object_handle_type& object_index, 
                            const std::string&        quan_name)   // [23]
  {
//      std::cout << " quantity access 2..: " << object_index << " / " << quan_name << std::endl;

//      if (quan_on_handle.find(object_index) == quan_on_handle.end())
//         {
//            std::cout << "no quan on this object .. " << std::endl;
//         }
//      else
//            std::cout << "!! quan on this object .. " << std::endl;
     
   
    return ( (* (*quan_on_handle.find(object_index)).second.find(quan_name)).second);
      //        |          |                               |                     |
      //        |             the first key -> result:row                        |
      //        |                                                                |
      //        |     second search -> result: element                           |
      //
   }

  const storage_type& operator()(const object_handle_type& object_index,
				const std::string&    quan_name) const // [23a]
   {
      return ( (* (*quan_on_handle.find(object_index)).second.find(quan_name)).second);
   }


  unsigned int key_size() const {return quantity_names.size(); }     // [18]
  unsigned int object_size() const { return quan_on_handle.size(); } // [19]

 
  friend std::ostream& operator<<(std::ostream& ostr, gsse_quantity_sparse const&  mt)  // [24]
  {
    typename quan_on_handle_t::const_iterator qoh_it, qoh_it_end;
    typename handle_on_quan_t::const_iterator hoq_it, hoq_it_end;
    
    typename internal_name_t::const_iterator   in_it, in_it_end;
    typename internal_handle_t::const_iterator ins_it, ins_it_end;    
    
    qoh_it     = mt.quan_on_handle.begin();
    qoh_it_end = mt.quan_on_handle.end();
    
    while(qoh_it != qoh_it_end)
      {
	std::cout << "index: " << (*qoh_it).first << "  size: " << (*qoh_it).second.size() << std::endl;
	
	in_it     = (*qoh_it).second.begin();
	in_it_end = (*qoh_it).second.end();
        
	while(in_it != in_it_end)
	  {
            std::cout << "    name: " << (*in_it).first << "  dt: " << (*in_it).second << std::endl;
            ++in_it;
	  }
	
	++qoh_it;
      }


      ///////////////

         hoq_it     = mt.handle_on_quan.begin();
         hoq_it_end = mt.handle_on_quan.end();

         while(hoq_it != hoq_it_end)
         {
            std::cout << "index: " << (*hoq_it).first << "  size: " << (*hoq_it).second.size() << "  content: " ;
            
            ins_it     = (*hoq_it).second.begin();
            ins_it_end = (*hoq_it).second.end();

            while( ins_it != ins_it_end)
            {
               std::cout << *ins_it << "  ";
               ++ins_it;
            }
            std::cout << std::endl;

            ++hoq_it;
         }

         return ostr;
   }

private:
  long size_from_name(const std::string& quan_name) { return handle_on_quan[quan_name].size(); }

private:
   quan_on_handle_t           quan_on_handle;
   handle_on_quan_t           handle_on_quan;
   std::vector<quan_key_type>    quantity_names;
};


template <typename HandleType, typename KeyType, typename StorageType>
struct quantity_traits< gsse_quantity_sparse<HandleType, KeyType, StorageType> >
{
   typedef gsse_quantity_sparse<HandleType, KeyType, StorageType> quantity_type;

   typedef typename quantity_type::pattern_type         pattern_type;
   typedef typename quantity_type::storage_type         storage_type; 
   typedef typename quantity_type::object_handle_type   object_handle_type;
   typedef typename quantity_type::quan_key_type        quan_key_type ;    
   typedef typename quantity_type::key_iterator         key_iterator;      
   typedef typename quantity_type::handle_iterator      handle_iterator;   
};


}

#endif
