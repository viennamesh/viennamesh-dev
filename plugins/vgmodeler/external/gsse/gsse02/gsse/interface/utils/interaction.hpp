/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at   

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GMI_UTILS_INTERACTION
#define GMI_UTILS_INTERACTION   


//
// ===============================================================
//
namespace gmi {

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
} // end namespace: gmi

#endif
