/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at    

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_GENERATION_UTILS_INTERACTION
#define GML_GENERATION_UTILS_INTERACTION


//
// ===============================================================
//
namespace gml {

// ===============================================================
// ===============================================================
// ===============================================================
template<typename Object>
void start(Object const& object) 
{ 
   object.start(); 
}
template<typename Object>
void start(Object      & object) 
{ 
   object.start(); 
}
// ===============================================================
// ===============================================================
// ===============================================================
template<typename Object>
void release(Object const& object) 
{ 
   object.release(); 
}
template<typename Object>
void release(Object      & object) 
{ 
   object.release(); 
}
// ===============================================================
// ===============================================================
// ===============================================================

// ===============================================================
// ===============================================================
// ===============================================================
} // end namespace: gml

#endif
