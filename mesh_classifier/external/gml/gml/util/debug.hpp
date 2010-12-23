/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2005-2009 Philipp Schwaha                      philipp@gsse.at

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_UTIL_DEBUG
#define GML_UTIL_DEBUG
//
// ===================================================================================
//
// *** SYSTEM includes
//
#include <iostream>
#include <cxxabi.h>
//
// *** BOOST includes
//
#include <boost/fusion/include/for_each.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/static_assert.hpp>

//
// ===================================================================================
//
namespace gml 
{
   // [JW] TODO implement a generic debug, which offers the same interface for different application cases.
   //  implement a new dump method for dumping the capabilities and set-able properties for mesher objects ..




   // -------------------------------------------------------------
   // [JW] the type_to_string-function has been extracted from gsse debug_meta source code. 
   // I was in need for a few tweaks and the memory management has been improved (thx to valgrind) 
   // .. however, cheers to philipp!
   //
   template<typename T>
   std::string
   type_to_string()     
   {
      std::size_t  len = 0;
      int          stat = 0;
      
      char *demangled = abi::__cxa_demangle(typeid(T).name(), NULL, &len, &stat);
            
      std::string type_string("");
               
      if(stat ==  0) // all went fine ..
      { 
         type_string = boost::lexical_cast< std::string >( demangled );
         if(demangled)                    free(demangled);           
         return type_string; 
      }
      if(stat == -1) 
         std::cout << "gml::util::debug::type_to_string: memory allocation failure!" << std::endl;
      
      if(stat == -2)
         std::cout << "gml::util::debug::type_to_string: the requested name is not valid under the C++ ABI mangling rules!" << std::endl;
      
      if(stat == -3)
         std::cout << "gml::util::debug::type_to_string: one of the arguments is invalid!" << std::endl;
      return type_string;
   }
   // -------------------------------------------------------------   
   template<typename T>
   void dumptype()     
   {
      std::cout << "\033[1;32m" << type_to_string<T>() << "\033[0m"  << std::endl;
   }  

   template<typename T>
   void dumptype(T & t)  
   {
      gml::dumptype<T>();
   }
   
   template<typename T>
   void dumptype(T const& t)  
   {
      gml::dumptype<T>();
   }  
   // -------------------------------------------------------------
   struct dump_sequ_functor
   {
      template < typename Functor >
      void operator()(Functor func) const
      {
         std::string type_string = gml::type_to_string<Functor>();

         std::string::size_type loc = type_string.find("<", 0);
         if( loc != std::string::npos )   type_string.erase( loc );  // remove the template types ..         

         std::cout << "   Functor: \033[1;32m" << type_string << "\033[0m" << std::endl;
         std::cout << "       result-type: " << gml::type_to_string< typename Functor::result_type >() << std::endl;
         std::cout << std::endl;
      }
   };
   template<typename Sequence>
   void dumpqueue(Sequence & sequ)
   {
      BOOST_MPL_ASSERT(( boost::fusion::traits::is_sequence< Sequence > ));   
      std::cout << std::endl;
      std::cout << "----------------------------------------------------------------------" << std::endl;      
      std::cout << "# FUNCTOR SEQUENCE " << std::endl;      
      std::cout << std::endl;      
      boost::fusion::for_each(sequ, dump_sequ_functor());  
      std::cout << "----------------------------------------------------------------------" << std::endl;      
      std::cout << std::endl;
   }   
   template<typename Sequence>
   void dumpqueue(Sequence const& sequ)
   {
      BOOST_MPL_ASSERT(( boost::fusion::traits::is_sequence< Sequence > ));   
      Sequence _sequ = sequ;
      dumpqueue(_sequ);  
   }   
   // -------------------------------------------------------------   
} // end namespace: gml
#endif
















