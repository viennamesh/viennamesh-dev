/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at   

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GMI_UTILS_ACCESS
#define GMI_UTILS_ACCESS

namespace gmi {

template<typename Object>
typename Object::ResultPointContainer &
geometry(Object const& object) 
{ 
   return object.geometry(); 
}
template<typename Object>
typename Object::ResultPointContainer &
geometry(Object      & object) 
{ 
   return object.geometry(); 
}
// ===============================================================
// ===============================================================
// ===============================================================
template<typename Object>
typename Object::ResultCellContainer &
topology(Object const& object) 
{ 
   return object.topology(); 
}
template<typename Object>
typename Object::ResultCellContainer &
topology(Object      & object) 
{ 
   return object.topology(); 
}

} // end namespace: gmi

#endif
