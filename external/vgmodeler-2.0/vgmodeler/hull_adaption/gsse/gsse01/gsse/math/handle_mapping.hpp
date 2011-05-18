/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#include <map>

#ifndef GSSE_MAPPING_HH
#define GSSE_MAPPING_HH

namespace gsse
{

  // A handle mapping is a data structure which makes a connection
  // between a matrix interface and the GSSE. It consists of two
  // mappings between an entry type and a long index type. 

template<typename EntryT, typename FunctorType>
class handle_mapping
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

  // the size of the matrix
   int length;
  
public:
  
   handle_mapping() {length = 0;}
  

   void dump()
   {
      for(typename std::map<entry_t, long>::iterator iter = entries.begin(); iter != entries.end(); ++iter)
      {
         std::cout << (*iter).first << "   " << (*iter).second << std::endl;
      }
   }


  // insert a new entry into the mapping using the funcion object func
  // as writing back type

   void add(entry_t const& v, FunctorType* func)
   {
      inv_entries[length] = v;
      entries[v] = length;
      inv_functor[length] = func;
      ++length;
   }

  // returns the index of an entry
  
   long operator()(entry_t const& v) const
   {
      typename std::map<entry_t, long>::const_iterator iter; 

      iter = entries.find(v);

      if (iter == entries.end()) 
      {
         throw gsse::search_not_successfull("handle_mapping, operator() with argument: entry_t");
      }

      return (*iter).second;
   }
  

  // returns the corresponding entry of an index

   entry_t operator()(long l) 
   {
      return inv_entries[l];
   }

  // returns the function object which is responsible for rewriting
  // the values from the l-th value. 

   FunctorType& get_functor(long l) 
   {
      return *inv_functor[l];
   }


  // convenient method: sets the value of the l-th solution variable
  // to n

   template <typename NumericT>
   void set_value(long l, NumericT n) 
   {
      get_functor(l).set_value(inv_entries[l], n);
   }

  // convenient method: updates the value of the l-th solution variable
  // to n

   template <typename NumericT>
   void update_value(long l, NumericT n) 
   {
      get_functor(l).update_value(inv_entries[l], n);
   }
  

  // returns the size of the total data structure
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

template<typename EntryT, typename FunctorType>
class high_performance_handle_mapping
{
  typedef EntryT                entry_t;
  std::map<long, entry_t>       inv_entries;
  std::vector<long>             entries;
  FunctorType*                  func;           // Only one function object
  int length;
  
public:
  
  // No default construction, the size is necessary
  high_performance_handle_mapping(long size) { entries.resize(size); length = 0;}
  
  void dump()
  {
    for(typename std::map<entry_t, long>::iterator iter = entries.begin(); iter != entries.end(); ++iter)
      {
	std::cout << (*iter).first << "   " << (*iter).second << std::endl;
      }
  }

  // Insert
  
  void add(entry_t const& v, FunctorType* f)
  {
    inv_entries[length] = v;
    entries[v.handle()] = length;
    // entries.at(v) = length;       //  for debugging reasons
    func = f;
    ++length;
  }


  // returns the index of an entry  
  long operator()(entry_t const& v) const
  {
    typename std::map<entry_t, long>::const_iterator iter;   

    return entries[v.handle()];
  }
  
  // returns the entry of an index
   entry_t operator()(long l) 
   {
      return inv_entries[l];
   }

  // returns the function object
   FunctorType& get_functor(long l) 
   {
      return *func;
   }

  // convenient method: sets the value of the l-th solution variable
  // to n

   template <typename NumericT>
   void set_value(long l, NumericT n) 
   {
     (*func).set_value(inv_entries[l], n);
   }

  // convenient method: updates the value of the l-th solution variable
  // to n

   template <typename NumericT>
   void update_value(long l, NumericT n) 
   {
     (*func).update_value(inv_entries[l], n);
   }
  
  // returns the size of the total data structure
   long size() const {return length;}  
};



} // end of namespace gsse;

#endif
