/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at    

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_GENERATION_UTILS_METASELECTION
#define GML_GENERATION_UTILS_METASELECTION

//
// ===============================================================
//
// *** GML includes
//
//#include "../interfaces.hpp"
// ===============================================================
//
namespace gml {

// ===============================================================

template< typename NumericT >
struct Triangle2D;

template< int DIM, typename NumericT >
struct Cgal;

template< typename NumericT >
struct Tetgen;

// ===============================================================
struct tag_triangle2d   {};
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
template < typename  MesherID, typename Numeric >
struct mesher_impl < 2, MesherID, Numeric >
{
   typedef gml::Triangle2D<Numeric>    type;
};
// ===============================================================
template < typename Numeric >
struct mesher_impl < 2, gml::tag_cgal, Numeric >
{
   typedef gml::Cgal< 2, Numeric >     type;
};
// ===============================================================
template < typename Numeric >
struct mesher_impl < 3, gml::tag_cgal, Numeric >
{
   typedef gml::Cgal< 3, Numeric >     type;
};
// ===============================================================
template < typename  MesherID, typename Numeric >
struct mesher_impl < 3, MesherID, Numeric >
{
   typedef gml::Tetgen<Numeric>        type;
};
// ===============================================================
template < int       DIM, 
           typename  MesherID  = void,
           typename  Numeric   = double>
struct mesher
{
   typedef typename gml::result_of::mesher_impl< 
      DIM, MesherID, Numeric 
   >::type  type;
};
// ===============================================================
} // end namespace: result_of
} // end namespace: gml

#endif
