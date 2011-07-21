/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_QUANTITY_HH_ID
#define GSSE_QUANTITY_HH_ID

// *** BOOST includes
// 
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/counting_iterator.hpp>

// *** system includes 
//
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

// *** GSSE includes
// 
#include "gsse/math/matrix.hpp"
#include "gsse/iterator.hpp"
#include "gsse/exceptions.hpp"
#include "gsse/quantity_traits.hpp"


   // ============================================================================================================
   //
   //                           GSSE implements the concept of a fiber bundle / sheaf
   //                      detailed information can be found at http://www.reneheinzl.at/phd/
   //
   //  The GSSE::quantity represents the fiber space
   //    *) a so-called section   represents  a given quantity with unique name distributed over the base space
   //    *) a so-called fiber     represents  all quantities over a single element of the base space
   //
   //
   // ============================================================================================================


namespace gsse 
{

  // 
  // @brief GSSE Quantity storage system
  //
  // This system can access the stored quantities with two different mechanisms
  //   1) return all points from a quantity
  //   2) return all quantities from a point
  //
  // To handle this feature the quantities are stored within a 2D memory
  // array. 
  // For an easy usage a pseudo associative initialisation is implemented.
  //
  // The first quantity name access generates a corresponding column. All
  // afterward accesses are sorted correctly inside this memory array.
  //
  //////////////////////////////////////////////////////////////////////////

   template<typename HandleType, typename KeyType, typename StorageType>
   class gsse_quantity
   {
      typedef std::vector<std::vector<StorageType> >          entry_t;  // internal name is OK

      // perhaps we can avoid this 
      //typedef typename matrix_traits<StorageType>::numeric_t  numeric_t;
      //

      typedef HandleType                                        object_index_t;

   public:

    typedef StorageType                                     pattern_type;       // C [A1]
    typedef pattern_type                                        storage_type;        // C [1]

      // and avoid this
      //typedef numeric_t                                           numeric_type;       // C [2]
      //

    typedef object_index_t                                      object_handle_type; // C [3]
    typedef KeyType                                             quan_key_type;      // C [4]

    // C concept requirements [5] .. [8] are fulfilled in the external PTS
    // C of the quantity_traits.

    // Construction

    gsse_quantity() // C [9]
    {
      entry = entry_t(0, std::vector<storage_type>(0));
    }

    // T these methods are not necessary for the concept and only for
    // T convenience. However these methods have to be tested in the
    // T module test !!!

    gsse_quantity(const unsigned int& size1) 
    {
      entry = entry_t(size1, std::vector<storage_type>(1));
    }

    gsse_quantity(const unsigned int& size1, const unsigned int& size2) 
    {
      entry = entry_t(size1, std::vector<storage_type>(size2));
    }
  
    // end of Construction 
  
  public:
    
    // Inserting 

    void insert(const object_handle_type& object_index, 
		const quan_key_type& quan_name, 
		const storage_type& value)  // [10]
    {
      unsigned int index_quan;
      index_quan = get_index_from_quan_name(quan_name) ;

      // check if object index is valid

      if (object_index >= static_cast<object_handle_type>(entry.size()))
	{
	  entry.resize(object_index+1,std::vector<storage_type>(index_quan+1) );
	}

      // check if the index from the quantity is valid

      if (index_quan >= entry[object_index].size())
	{
	  for (unsigned int i =0; i < entry.size(); ++i)
	    {
	      entry[i].resize(index_quan+1);
	    }
	}
             
      // here all indices are accessible

      entry[object_index][index_quan] = value;
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

    void insert(const quan_key_type& quan_name, storage_type value)   // C [11]
    {
      int index_quan;
      index_quan = get_index_from_quan_name(quan_name) ;

      // std::cout << "debug.. index quan: " << index_quan << std::endl;

      typename entry_t::iterator  eit(entry.begin());
      typename entry_t::iterator  eit_end(entry.end());

      for (; eit != eit_end; ++eit)
	{
	  // std::cout << "value .. " << value << std::endl;

	  if ( static_cast<int>((*eit).size()) <= index_quan)
	    (*eit).resize(index_quan+1);

	  // here all indices are accessible
	  (*eit)[index_quan] = value;
	}
    }

    // end of insertion

    /// @brief direct access to quantities
    ///
    /// Direct mechanism for accessing the stored data pattern types.
    /// Here also the quan names are checked.. if the quantity does not exist, one is created
    /// Here, the ranges are checked. Use this method, if you are not sure if the indices are valid.
    ///
    /// @param  quan_name         the name of a quantity
    /// @param  value            should be stored within the quantity memory array
    /// 

    storage_type& retrieve_quantity(const object_handle_type&  object_index,
				   const quan_key_type&       quan_name)  
    {
      size_t quan_index;
      quan_index = get_index_from_quan_name_const(quan_name);

      if (object_index >= static_cast<object_handle_type>(entry.size()))  
	{
	  throw invalid_object_index(object_index);
	}

      // check if the index from the quantity is valid
      //
      if (quan_index >= entry[object_index].size())
	{
	  for (unsigned int i =0; i < entry.size(); ++i)
	    {
	      entry[i].resize(quan_index+1);
	    }
	}

      return entry[object_index][quan_index];
    }

    /// @brief direct access to quantities
    ///
    /// Direct mechanism for accessing the stored data pattern types.
    /// Here, the ranges are checked. Use this method, if you are not sure if the indices are valid.
    ///
    /// @param  quan_name         the name of a quantity
    /// @param  value            should be stored within the quantity memory array
    /// 
    /// @throws                  invalid_quantity_name
    /// @throws                  invalid_object_index
    ///
    storage_type& retrieve_quantity_notautomatic(const object_handle_type&  object_index,
						const quan_key_type&       quan_name)  
    {
      int quan_index;
      quan_index = get_index_from_quan_name_const(quan_name);
      if (quan_index < 0)  
	{
	  throw invalid_quantity_name(quan_name);
	}
      if (object_index >= static_cast<object_handle_type>(entry.size()))  
	{
	  throw invalid_object_index(object_index);
	}
      return entry[object_index][quan_index];
    }


    storage_type const& retrieve_quantity(const object_handle_type&  object_index, 
					  const quan_key_type&     quan_name) const // C [22]
    {
      int quan_index;
      quan_index = get_index_from_quan_name_const(quan_name);
      if (quan_index < 0)  
	{
	  throw invalid_quantity_name(quan_name);
	}
      if (static_cast<int>(object_index) >= static_cast<int>(entry.size())) 
	{
	  throw invalid_object_index(object_index);
	}
      return entry[object_index][quan_index];
    }

    /// @brief direct access 
    ///
    /// BE CAREFUL.. this operator does not check the validity ..
    /// only for high speed operations
    ///

    storage_type& operator()(const object_handle_type& object_index, 
			    const quan_key_type&      quan_name)                        // C [23]
    {
      return entry[object_index][get_index_from_quan_name_const(quan_name)];
    }

    storage_type const& operator()(const object_handle_type& object_index,
			    const quan_key_type&      quan_name) const                  // C [24]
    {
      return entry[object_index][get_index_from_quan_name_const(quan_name)];
    }

    unsigned int key_size()    const { return quantity_names.size(); }                  // C [18]
    unsigned int object_size() const { return entry.size(); }                           // C [19]

    friend std::ostream& operator<<(std::ostream& ostr, gsse_quantity const&  mt)       // C [25]
    {
      ostr << "#### gsse quantity" << std::endl;
      for (unsigned int i=0; i < mt.entry.size(); i++)
	{
	  ostr << "##### object id: " << i << std::endl;
	  for (unsigned int j=0; j < mt.entry[i].size(); j++)
	    {
	      // ###
	      // if (mt.quantity_names.size() > 0)
	      // std::cout <<"###### quan: " << mt.quantity_names[j] << " j = " << j <<  std::endl;
	      // ###
	       
	      ostr << mt.entry[i][j];
	    }
	  ostr << std::endl;
	}
      return ostr;
    }

  private:
    entry_t                     entry;
    std::vector<quan_key_type>  quantity_names;

  public:
  
     typedef typename std::vector<quan_key_type>::const_iterator internal_key_iterator;    
     
     internal_key_iterator internal_key_begin() const {return quantity_names.begin();}     
     internal_key_iterator internal_key_end()   const {return quantity_names.end();}       

     typedef gsse_iterator<internal_key_iterator> key_iterator;

     key_iterator key_begin() const 
        {return key_iterator(internal_key_begin(), internal_key_begin(), internal_key_end());}
     
     key_iterator key_end() const 
        {return key_iterator(internal_key_end(), internal_key_begin(), internal_key_end());}


    // In this case the object iterator is a simple counting iterator
    // which can be taken from the boost libraries!

    typedef boost::counting_iterator<unsigned int> internal_handle_iterator;                                  

    internal_handle_iterator internal_handle_begin() const { return internal_handle_iterator(0); }            
    internal_handle_iterator internal_handle_end()   const { return internal_handle_iterator(entry.size()); } 


     typedef gsse_iterator<internal_handle_iterator> handle_iterator;

     handle_iterator handle_begin() const 
        {return handle_iterator(internal_handle_begin(), internal_handle_begin(), internal_handle_end());}
     
     handle_iterator handle_end() const 
        {return handle_iterator(internal_handle_end(), internal_handle_begin(), internal_handle_end());}



  private:

    // these methods are private and do not need to be tested explicitly
    // but they only have to work together with the other functions.

    int get_index_from_quan_name(const quan_key_type& name) 
    {
       internal_key_iterator iter;

      iter = std::find(quantity_names.begin(), quantity_names.end(), name);

      if (iter != quantity_names.end())
	{
	  return iter - quantity_names.begin();
	}

//       bool found(false);
//       for (unsigned int i = 0 ; i < quantity_names.size(); i++)
// 	{
// 	  if (quantity_names[i] == name)
// 	    {
// 	      found=true;
// 	      return i;
// 	    }
// 	}

      // the name was not found  !!!
      // here we create the associative resize feature from std::map for our container
      
      quantity_names.push_back(name);
      return (quantity_names.size()-1);
    }
    
    // This method returns the key index from the key name
    
    int get_index_from_quan_name_const(const quan_key_type& name) const
    {
       internal_key_iterator iter;

      iter = std::find(quantity_names.begin(), quantity_names.end(), name);

      if (iter != quantity_names.end())
	{
	  return iter - quantity_names.begin();
	}

//       bool found(false);
//       for (unsigned int i = 0 ; i < quantity_names.size(); i++)
// 	{
// 	  if (quantity_names[i] == name)
// 	    {
// 	      found=true;
// 	      return i;
// 	    }
// 	}
      // the name was not found  !!!
      return -1;
    }
  };



template <typename HandleType, typename KeyType, typename StorageType>
struct quantity_traits< gsse_quantity<HandleType, KeyType, StorageType> >
{
   typedef gsse_quantity<HandleType, KeyType, StorageType> quantity_type;

   typedef typename quantity_type::pattern_type          pattern_type;        
   typedef typename quantity_type::storage_type          storage_type;       
   typedef typename quantity_type::object_handle_type    object_handle_type;  
   typedef typename quantity_type::quan_key_type         quan_key_type ;      
   typedef typename quantity_type::key_iterator          key_iterator;        
   typedef typename quantity_type::handle_iterator       handle_iterator;     
};

}

#endif
