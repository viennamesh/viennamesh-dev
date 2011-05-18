/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2005-2010 Philipp Schwaha                      philipp@gsse.at
   Copyright (c) 2005-2010 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_DEBUG_META_HH
#define GSSE_DEBUG_META_HH

// *** SYSTEM includes
//
#include <iostream>

#ifndef __clang__
#include "cxxabi.h"
#endif

// *** BOOST includes
//
#include <boost/fusion/include/for_each.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/static_assert.hpp>

// [RH][TODO]

namespace gsse 
{


   template<typename T>
   std::string
   type_to_string()     
   {
      int          stat = 0;
      
   #ifndef __clang__
      std::size_t  len = 0;
      char *demangled = abi::__cxa_demangle(typeid(T).name(), NULL, &len, &stat);
   #else
      const char *demangled = "currently not available with this compiler";
   #endif
            
      std::string type_string("");
               
      if(stat ==  0) // all went fine ..
      { 
         type_string = boost::lexical_cast< std::string >( demangled );
      #ifndef __clang__
         if(demangled)                   
            free(demangled);           
      #endif
         return type_string; 
      }
      if(stat == -1) 
         std::cout << "gsse::util::debug::type_to_string: memory allocation failure!" << std::endl;
      
      if(stat == -2)
         std::cout << "gsse::util::debug::type_to_string: the requested name is not valid under the C++ ABI mangling rules!" << std::endl;
      
      if(stat == -3)
         std::cout << "gsse::util::debug::type_to_string: one of the arguments is invalid!" << std::endl;
      return type_string;
   }
   // -------------------------------------------------------------   
   template<typename T>
   void dump_type()     
   {
      std::cout << "\033[1;32m" << type_to_string<T>() << "\033[0m"  << std::endl;
   }  

   template<typename T>
   void dump_type(T & t)  
   {
      gsse::dump_type<T>();
   }
   
   template<typename T>
   void dump_type(T const& t)  
   {
      gsse::dump_type<T>();
   }  
   // -------------------------------------------------------------
   struct dump_sequ_functor
   {
      template < typename Functor >
      void operator()(Functor func) const
      {
         std::string type_string = gsse::type_to_string<Functor>();

         std::string::size_type loc = type_string.find("<", 0);
         if( loc != std::string::npos )   type_string.erase( loc );  // remove the template types ..         

         std::cout << "   Functor: \033[1;32m" << type_string << "\033[0m" << std::endl;
         std::cout << "       result-type: " << gsse::type_to_string< typename Functor::result_type >() << std::endl;
         std::cout << std::endl;
      }
   };
   template<typename Sequence>
   void dump_queue(Sequence & sequ)
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


} // namespace gsse
#endif // GSSE_DEBUG_META_HH
