/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_GEOMETRY_ALGORITHMS_PREDICATESJRS
#define GML_GEOMETRY_ALGORITHMS_PREDICATESJRS

/*
   This file contains the fast, non-robust predicates from
   Jonathan Richard Shewchuk.
   http://www.cs.cmu.edu/afs/cs/project/quake/public/code/predicates.c

*/
namespace gml
{
namespace algorithms
{
//
// ===================================================================================
//
template < typename MetricalSpace >
typename gml::result_of::val< MetricalSpace >::type
jrs_orient2dfast ( MetricalSpace const& pa,
                   MetricalSpace const& pb,
                   MetricalSpace const& pc )
{
   typename gml::result_of::val< MetricalSpace >::type 
      acx, bcx, acy, bcy;

   acx = pa[0] - pc[0];
   bcx = pb[0] - pc[0];
   acy = pa[1] - pc[1];
   bcy = pb[1] - pc[1];
  
   return acx * bcy - acy * bcx;
}
//
// ===================================================================================
//
template < typename MetricalSpace, typename MetricalSpace2 >
typename gml::result_of::val< MetricalSpace >::type
jrs_orient3dfast( MetricalSpace const& pa,
                  MetricalSpace const& pb,
                  MetricalSpace2 const& pc,
                  MetricalSpace const& pd )
{
   typename gml::result_of::val< MetricalSpace >::type 
   adx, bdx, cdx,
   ady, bdy, cdy,
   adz, bdz, cdz;

   adx = pa[0] - pd[0];
   bdx = pb[0] - pd[0];
   cdx = pc[0] - pd[0];
   ady = pa[1] - pd[1];
   bdy = pb[1] - pd[1];
   cdy = pc[1] - pd[1];
   adz = pa[2] - pd[2];
   bdz = pb[2] - pd[2];
   cdz = pc[2] - pd[2];

   return adx * (bdy * cdz - bdz * cdy)
        + bdx * (cdy * adz - cdz * ady)
        + cdx * (ady * bdz - adz * bdy);
}
//
// ===================================================================================
//
template < typename MetricalSpace >
typename gml::result_of::val< MetricalSpace >::type
jrs_incirclefast( MetricalSpace const& pa,
                  MetricalSpace const& pb,
                  MetricalSpace const& pc,
                  MetricalSpace const& pd )
{
   typename gml::result_of::val< MetricalSpace >::type 
      adx, ady, bdx, bdy, cdx, cdy,
      abdet, bcdet, cadet,
      alift, blift, clift;

   adx = pa[0] - pd[0];
   ady = pa[1] - pd[1];
   bdx = pb[0] - pd[0];
   bdy = pb[1] - pd[1];
   cdx = pc[0] - pd[0];
   cdy = pc[1] - pd[1];

   abdet = adx * bdy - bdx * ady;
   bcdet = bdx * cdy - cdx * bdy;
   cadet = cdx * ady - adx * cdy;
   alift = adx * adx + ady * ady;
   blift = bdx * bdx + bdy * bdy;
   clift = cdx * cdx + cdy * cdy;

   return alift * bcdet + blift * cadet + clift * abdet;
}

//
// ===================================================================================
//
template < typename MetricalSpace >
typename gml::result_of::val< MetricalSpace >::type
jrs_inspherefast( MetricalSpace const& pa,
                  MetricalSpace const& pb,
                  MetricalSpace const& pc,
                  MetricalSpace const& pd,
                  MetricalSpace const& pe )
{
   typename gml::result_of::val< MetricalSpace >::type 
      aex, bex, cex, dex,
      aey, bey, cey, dey,
      aez, bez, cez, dez,
      alift, blift, clift, dlift,
      ab, bc, cd, da, ac, bd,
      abc, bcd, cda, dab;

   aex = pa[0] - pe[0];
   bex = pb[0] - pe[0];
   cex = pc[0] - pe[0];
   dex = pd[0] - pe[0];
   aey = pa[1] - pe[1];
   bey = pb[1] - pe[1];
   cey = pc[1] - pe[1];
   dey = pd[1] - pe[1];
   aez = pa[2] - pe[2];
   bez = pb[2] - pe[2];
   cez = pc[2] - pe[2];
   dez = pd[2] - pe[2];

   ab = aex * bey - bex * aey;
   bc = bex * cey - cex * bey;
   cd = cex * dey - dex * cey;
   da = dex * aey - aex * dey;

   ac = aex * cey - cex * aey;
   bd = bex * dey - dex * bey;   

   abc = aez * bc - bez * ac + cez * ab;
   bcd = bez * cd - cez * bd + dez * bc;
   cda = cez * da + dez * ac + aez * cd;
   dab = dez * ab + aez * bd + bez * da;

   alift = aex * aex + aey * aey + aez * aez;
   blift = bex * bex + bey * bey + bez * bez;
   clift = cex * cex + cey * cey + cez * cez;
   dlift = dex * dex + dey * dey + dez * dez;

   return (dlift * abc - clift * dab) + (blift * cda - alift * bcd);
}

} // end namespace: algorithms
//
// ===================================================================================
//
} // end namespace: gml
#endif
