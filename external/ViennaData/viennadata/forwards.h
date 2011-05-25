/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */


#ifndef VIENNADATA_FORWARDS_H
#define VIENNADATA_FORWARDS_H

#include <map>
#include <vector>
#include <list>
#include <set>
#include <iostream>
#include <algorithm>
#include <memory>

/** @file viennadata/forwards.h
    @brief This file provides the forward declarations for the main types used within ViennaData
*/

/**
 @mainpage Source Code Documentation for ViennaData 1.0.0

 This is the source code documentation of ViennaData. Detailed information about the functions available in ViennaData can be found here.
 
 For a general overview over the types and functionality provided by ViennaData, please refer to the file doc/viennadata.pdf

*/


namespace viennadata
{
  
    ////////////////////// tags ////////////////////////

    //element identification:
    /** @brief A tag class that indicates that object addresses should be used as identification */
    struct pointer_based_id {};      //use object address as ID
    /** @brief A tag class that indicates that the object provides an ID mechanism (must be supplied by overloading object_identifier appropriately)*/
    struct object_provided_id {};   //object provides some ID with values in 0...N


    //storage policy tag:
    /** @brief A tag class for storing data in a dense manner, i.e. most objects of the same type carry the data for a particular key/value pair */
    struct dense_data_tag {};      //for a property that is accessed on every element *and* is required to be fast
    /** @brief A tag class for storing data in a sparse manner, i.e. only a few objects of the same type carry the data for a particular key/value pair */
    struct sparse_data_tag {};     //properties that will be accessed on some elements only. Default policy.

    
    /** @brief A tag class for specifying that both the key type and the key object are used for data access */
    struct full_key_dispatch_tag {};
    /** @brief A tag class for specifying that ONLY the key type is used for data access */
    struct type_key_dispatch_tag {};  //key dispatch based on type only


    
    ////////////////////// other stuff ////////////////////////
    /** @brief The main data storage facility
     * 
     * @tparam KeyType       Type of the key
     * @tparam DataType     Type of the data
     * @tparam ObjectType    Type of the object the data is associated with
     */
    template <typename KeyType,
              typename DataType,
              typename ObjectType>
    class data_container;
    
    /** @brief A tag class that is used to represent all types of either a particular key type, or a particular value type */
    struct all {};   //denotes all types
    
    /** @brief A proxy class used internally to copy data from one object to another */
    template <typename ObjectType>
    class key_value_copy_construction;
    
    /** @brief A proxy class used internally to copy data from one object to another */
    template <typename ObjectType>
    class key_value_copy_construction_wrapper;
  
    /** @brief A helper class to raise compile time errors */
    template <typename T>
    struct error_indicator {};
  
    
  
} //namespace


#endif
