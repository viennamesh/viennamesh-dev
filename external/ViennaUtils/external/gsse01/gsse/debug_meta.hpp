#ifndef DEBUG_META_HH
#define DEBUG_META_HH
#include "cxxabi.h"
#include <iostream>
//
//#include <cstddef> 
//#include <cstdlib>
//#include <typeinfo>
//
//


template<typename T>
void dump_type()
{
   char*       ptr = NULL;
   std::size_t len;
   int         stat;
   std::cout << abi::__cxa_demangle(typeid(T).name(),ptr,&len,&stat) << std::endl << std::endl;
}


#endif // DEBUG_META_HH
