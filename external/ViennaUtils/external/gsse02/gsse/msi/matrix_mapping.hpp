/* ============================================================================
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2004-2008 Philipp Schwaha                      philipp@gsse.at
   Copyright (c) 2005-2006 Michael Spevak
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_MATRIX_MAPPING_HH
#define GSSE_MATRIX_MAPPING_HH

// *** system includes   
#include <string>
#include <iostream>
//#include <vector>
#include <map>

// *** BOOST includes
// *** GSSE includes   
//#include "gsse/exceptions.hpp"


namespace gsse
{

// matrix-mapping connects a gsse::matrix-solver-interface <-> cell/quan complex
// it consists of two mappings between 
//  an entry type 
//  an index type. 
//
template<typename EntryT, typename FunctorType>
class matrix_mapping
{
   typedef EntryT                entry_t;

  // Mapping: entries -> indices
   std::map<entry_t, long>       entries;

  // Mapping: indices -> entries
   std::map<long, entry_t>       inv_entries;

  // The function object which is responsible for a matrix line, only
  // used for writing back quantities to the GSSE. The function object
  // has a set_value as well as an update_value method with the entry
  // type as argument.
   std::map<long, FunctorType*>  inv_functor;

  // the size of the mapping
   int length;
  
public:
  
   matrix_mapping() {length = 0;}
   
   void dump()
   {
      for(typename std::map<entry_t, long>::iterator iter = entries.begin(); iter != entries.end(); ++iter)
      {
         std::cout << (*iter).first << "   " << (*iter).second << std::endl;
      }
   }


   // insert a new entry into the mapping using the funcion object func
   // as writing back type
   // 
   void add(entry_t const& v, FunctorType* func)
   {
      // [RH][BUGFIX] .. 20080203 .. for domain quantities
      //  	each mapping has to be unique
      //
      if (entries.find(v) == entries.end())
      {
         inv_entries[length] = v;
         entries[v] = length;
         inv_functor[length] = func;
         ++length;
      }
   }

   // returns the index of an entry
   //
   long operator()(entry_t const& v) const
   {
      typename std::map<entry_t, long>::const_iterator iter; 
      
      iter = entries.find(v);
      
      if (iter == entries.end()) 
      {
         // [RH][TODO]
//         throw gsse::search_not_successfull("matrix_mapping, operator() with argument: entry_t");
      }
      return (*iter).second;
   }
  

   // returns the corresponding entry of an index
   //
   entry_t operator()(long l) 
   {
      return inv_entries[l];
   }

   // returns the function object which is responsible for rewriting
   // the values from the l-th value. 
   //
   FunctorType& get_functor(long l) 
   {
      return *inv_functor[l];
   }


   // convenient method: sets the value of the l-th solution variable
   // to n
   //
   template <typename NumericT>
   void set_value(long l, NumericT n) 
   {
      get_functor(l).set_value(inv_entries[l], n);
   }

   // convenient method: updates the value of the l-th solution variable
   // to n
   //
   template <typename NumericT>
   void update_value(long l, NumericT n) 
   {
//      std::cout << "## l: " << l << "   n: " << n << std::endl;
      get_functor(l).update_value(inv_entries[l], n);
   }
  
   std::string get_key(long l)
   {
      return get_functor(l).get_key();
   }
   
   long size() const {return length;}
};


  // This is a high performance implementation which can only be used
  // if the following requirements are met:
  
  // 1. Only one quantity and one segment
  // 2. The solution handle indices are dense beginning with zero
  // 3. There is only one function object responsible for rewriting
  // 4. The size of the matrix is known at construction time
  // 5. The elements passed have a handle member function returning
  // something convertible to long

// template<typename EntryT, typename FunctorType>
// class high_performance_matrix_mapping
// {
//   typedef EntryT                entry_t;
//   std::map<long, entry_t>       inv_entries;
//   std::vector<long>             entries;
//   FunctorType*                  func;           // Only one function object
//   int length;
  
// public:
  
//   // No default construction, the size is necessary
//   high_performance_matrix_mapping(long size) { entries.resize(size); length = 0;}
  
//   void dump()
//   {
//     for(typename std::map<entry_t, long>::iterator iter = entries.begin(); iter != entries.end(); ++iter)
//       {
// 	std::cout << (*iter).first << "   " << (*iter).second << std::endl;
//       }
//   }

//   // Insert
  
//   void add(entry_t const& v, FunctorType* f)
//   {
// //   if (std::find(entries.begin(), entries.end(), v.handle() ) == entries.end())
//    {
//     inv_entries[length] = v;
//     entries[v.handle()] = length;
//     func = f;
//     ++length;
//    }
//   }


//   // returns the index of an entry  
//   long operator()(entry_t const& v) const
//   {
//     typename std::map<entry_t, long>::const_iterator iter;   

//     return entries[v.handle()];
//   }
  
//   // returns the entry of an index
//    entry_t operator()(long l) 
//    {
//       return inv_entries[l];
//    }

//   // returns the function object
//    FunctorType& get_functor(long l) 
//    {
//       return *func;
//    }

//   // convenient method: sets the value of the l-th solution variable
//   // to n

//    template <typename NumericT>
//    void set_value(long l, NumericT n) 
//    {
//      (*func).set_value(inv_entries[l], n);
//    }

//   // convenient method: updates the value of the l-th solution variable
//   // to n

//    template <typename NumericT>
//    void update_value(long l, NumericT n) 
//    {
//      (*func).update_value(inv_entries[l], n);
//    }
  
//   // returns the size of the total data structure
//    long size() const {return length;}  
// };



} // end of namespace gsse;

#endif
