/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at   

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
      
#ifndef GMI_UTILS_METASELECTION
#define GMI_UTILS_METASELECTION   


namespace gsse {

// ===============================================================

template< typename NumericT >
struct Triangle;

template< int DIM, typename NumericT >
struct Cgal;

template< typename NumericT >
struct Tetgen;

// ===============================================================
struct tag_triangle   {};
struct tag_cgal         {};
struct tag_tetgen       {};
// ===============================================================
namespace result_of {
// ===============================================================
template < int      DIM, 
           typename MesherID,
           typename Numeric >
struct mesher_impl { };
// ===============================================================
template < typename Numeric >
struct mesher_impl < 2, gsse::tag_triangle, Numeric >
{
   typedef gsse::Triangle<Numeric>    type;
};
// ===============================================================
template < typename Numeric >
struct mesher_impl < 2, gsse::tag_cgal, Numeric >
{
   typedef gsse::Cgal< 2, Numeric >     type;
};
// ===============================================================
template < typename Numeric >
struct mesher_impl < 3, gsse::tag_cgal, Numeric >
{
   typedef gsse::Cgal< 3, Numeric >     type;
};
// ===============================================================
template < typename  MesherID, typename Numeric >
struct mesher_impl < 3, MesherID, Numeric >
{
   typedef gsse::Tetgen<Numeric>        type;
};
// ===============================================================
template < int       DIM, 
           typename  MesherID  = void,
           typename  Numeric   = double>
struct mesher
{
   typedef typename gsse::result_of::mesher_impl< 
      DIM, MesherID, Numeric 
   >::type  type;
};
// ===============================================================
} // end namespace: result_of
} // end namespace: gsse

#endif
