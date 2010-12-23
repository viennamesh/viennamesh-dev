/* =======================================================================

                     ---------------------------------------
                     ViennaMesh - The Vienna Meshing Library
                     ---------------------------------------
                            
   maintainers:    
               Josef Weinbub                      weinbub@iue.tuwien.ac.at
               Johann Cervenka                   cervenka@iue.tuwien.ac.at

   developers:
               Franz Stimpfl, Rene Heinzl, Philipp Schwaha, Peter Fleischmann

   license:    
               LGPL, see file LICENSE in the ViennaMesh base directory

   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at

   ======================================================================= */

#include "delink.hpp"



/** used for point-inside-triangle tests *************************************/

/* determines if a point is considered to be outside of a triangle           */
/* Relative epsilon: fraction of an edge length                              */
/* Should be very small or zero.                                             */
#define EDGO_EPS     100.0 * DBL_EPSILON   

/* determines if a point is considered to be inside or on an edge of a tri.  */
/* Relative epsilon: fraction of an edge length                              */
/* Can be zero, but should be larger to avoid refinement very close to edges */
#define EDGI_EPS     FLT_EPSILON



/** used for line-triangle/plane intersection ********************************/

/* determines if a line and a plane are parallel (cosine of an angle)        */
#define ITL_EPS      FLT_EPSILON



/** stupid parameters for bad relaxation never used **************************/
/* someday replace with laplace or whatever relaxation without all that fuz. */
#define GD1_PAR           (10.0)
#define GD8_PAR           (0.0)



/*****************************************************************************/
/*                                                                           */
/*  calc_region()                                                            */
/*                                                                           */
/*****************************************************************************/
/* Calculates dpmin, dpmax for a given triangle and lambda0.                 */
/* Initializes first M, first R2 and R.                                      */
/* (needs access to structure client_data)                                   */
/*                                                                           */
/* USED EPSILON:                                                             */
/*  R_EPS                                                                    */
/*                                                                           */
/* It is important to increase the region slightly by R_EPS:                 */
/*  - Generally, if a point is located near the border of the sphere *and*   */
/*    near the border of the search-cube, it will be necessary to compare    */
/*    its lambda with the lambda of another point which may be located well  */
/*    inside of the search-cube (and also close to the border of the sphere).*/
/*    Therefore, both points should be found.                                */
/*    However, this is not such an issue due to the relative epsilon region  */
/*    (SER): If a point is located too close to the border of the sphere     */
/*    defining the search-cube, the search region will be increased anyhow.  */
/*  - Thus, the remaining issue is the following:                            */
/*    If the search-cube is not increased because a single point was found   */
/*    and it is well inside the sphere, it will be taken as the fourth point.*/
/*    Depending on whether or not there exists a rlist, different actions are*/
/*    taken. Thus, if the base-circle (rlist) is the aequator of the search- */
/*    sphere some points belonging to the rlist might have been missed !     */
/*    It cannot happen that points belonging to the plist should actually    */
/*    belong to the rlist, but it could happen that the rlist is too small ! */
/*                                                                           */
void calc_region(mesh_p_t mesh_p,
		 ppt dppmin, 
		 ppt dppmax, 
		 struct client_datadef* cli) {

  struct vectordef va;
  double R;

  cli->lambda.M.x = cli->tri.H.x + cli->lambda.lam * cli->tri.n.x;
  cli->lambda.M.y = cli->tri.H.y + cli->lambda.lam * cli->tri.n.y;
  cli->lambda.M.z = cli->tri.H.z + cli->lambda.lam * cli->tri.n.z;

  /* vector M->P1                                                            */
  va.x = cli->tri.tri_p->dppp[0]->x - cli->lambda.M.x;       
  va.y = cli->tri.tri_p->dppp[0]->y - cli->lambda.M.y;
  va.z = cli->tri.tri_p->dppp[0]->z - cli->lambda.M.z;       

  /* square of radius                                                        */
  cli->lambda.R2 = va.x*va.x + va.y*va.y + va.z*va.z;  

  /* radius                                                                  */
  R = sqrt(cli->lambda.R2);                      

  /* next time increase lambda by this amount                                */
  cli->lambda.R = R;                

  /* better with an additional epsilon !!                                    */
  /* (for the region not the radius !)                                       */
  R += mesh_p->R_EPS;                      

  dppmin->x = cli->lambda.M.x - R;     
  dppmin->y = cli->lambda.M.y - R;
  dppmin->z = cli->lambda.M.z - R;
  dppmax->x = cli->lambda.M.x + R;  
  dppmax->y = cli->lambda.M.y + R;
  dppmax->z = cli->lambda.M.z + R;
  
  
}




/*****************************************************************************/
/*                                                                           */
/*  calc_n_H()                                                               */
/*                                                                           */
/*****************************************************************************/
/* Calculates circumcenter 'H' and normal vector 'n' of a triangle.          */
/* Stores all important data for later calculations in triangle_param        */
/* structure. (normalized n, not normalized n ...)                           */
/*                                                                           */
/* Caution: Some functions rely on va: 0->1 vb: 0->2 !                       */
/*                                                                           */
/* Returns 0 if the area of the triangle is too small                        */
/*                                                                           */
int calc_n_H(struct triangle_paramdef* tpp) {
  double DD;
  double invnu2;
  double h;
  struct vectordef P1H;
  /* shorthand                                                               */
  ppt dpp1 = tpp->tri_p->dppp[0]; 
  ppt dpp2 = tpp->tri_p->dppp[1];
  ppt dpp3 = tpp->tri_p->dppp[2];


  /** Initialize all necessary parameters for future calculations (lambda) ***/

  /* vector a                                                                */
  tpp->va.x = dpp2->x - dpp1->x; 
  tpp->va.y = dpp2->y - dpp1->y;
  tpp->va.z = dpp2->z - dpp1->z;

  /* vector b                                                                */
  tpp->vb.x = dpp3->x - dpp1->x; 
  tpp->vb.y = dpp3->y - dpp1->y;
  tpp->vb.z = dpp3->z - dpp1->z;

  tpp->A = -tpp->va.x * (dpp1->x + dpp2->x) 
           -tpp->va.y * (dpp1->y + dpp2->y) 
           -tpp->va.z * (dpp1->z + dpp2->z);
  tpp->B = -tpp->vb.x * (dpp1->x + dpp3->x) 
           -tpp->vb.y * (dpp1->y + dpp3->y) 
           -tpp->vb.z * (dpp1->z + dpp3->z);

  /* vector nu does not have length 1.0                                      */
  tpp->nu.x = (tpp->va.y * tpp->vb.z - tpp->vb.y * tpp->va.z); 
  tpp->nu.y = (tpp->va.z * tpp->vb.x - tpp->vb.z * tpp->va.x);
  tpp->nu.z = (tpp->va.x * tpp->vb.y - tpp->vb.x * tpp->va.y);

  /* collinear test could use an epsilon instead of zero                     */
  h = tpp->nu.x*tpp->nu.x + tpp->nu.y*tpp->nu.y + tpp->nu.z*tpp->nu.z; 
  if(h == 0.0) {
    msg_warn("+WARNING+: Detected a collinear triangle \n");
    /* Setting any ctags? definitely not 'blocked' ! neither req. nor good   */
    /* tpp->tri_p->ctag |= ;                                                 */
    return 0;
  }

  invnu2 = 1.0 / h;

  h = sqrt(invnu2);

  /* normalized vector n                                                     */
  tpp->n.x = h * tpp->nu.x;               
  tpp->n.y = h * tpp->nu.y;
  tpp->n.z = h * tpp->nu.z;


  /** Calculation of H (circumcenter of triangle) ****************************/

  /* nu . vx1 * 2                                                            */
  DD = 2 * (tpp->nu.x * dpp1->x +               
	    tpp->nu.y * dpp1->y +             
	    tpp->nu.z * dpp1->z);
  
  h = 0.5 * invnu2;
  tpp->H.x = h * (tpp->A * (tpp->nu.y*tpp->vb.z - tpp->vb.y*tpp->nu.z) + 
	          tpp->B * (tpp->va.y*tpp->nu.z - tpp->nu.y*tpp->va.z) +
	          DD * tpp->nu.x);
  tpp->H.y = h * (tpp->A * (tpp->vb.x*tpp->nu.z - tpp->nu.x*tpp->vb.z) + 
	          tpp->B * (tpp->va.z*tpp->nu.x - tpp->nu.z*tpp->va.x) +
	          DD * tpp->nu.y);
  tpp->H.z = h * (tpp->A * (tpp->vb.y*tpp->nu.x - tpp->nu.y*tpp->vb.x) + 
	          tpp->B * (tpp->va.x*tpp->nu.y - tpp->nu.x*tpp->va.y) +
	          DD * tpp->nu.z);

  /* P1H, lp1h                                                               */
  P1H.x = (tpp->H.x - dpp1->x);
  P1H.y = (tpp->H.y - dpp1->y);
  P1H.z = (tpp->H.z - dpp1->z);
  tpp->lp1h = sqrt(P1H.x*P1H.x + P1H.y*P1H.y + P1H.z*P1H.z);

  return 1;

}





/*****************************************************************************/
/*                                                                           */
/*  calc_collinear()                                                         */
/*                                                                           */
/*****************************************************************************/
/* Calculates if a triangle is collinear                                     */
/* The order of the input points is not relevant.                            */
/* returns 0 if the area of the triangle is too small                        */
/*                                                                           */
/* Used so far at all times where triangles could be generated during        */
/* surface preprocessing (insert tri, refinement functions which reuse       */
/* reftri_p, flip tris, however not completely at tet-based refinement even  */
/* though these functions use sm_ refinement partly)                         */
/* HOWEVER only used for warnings so far even for flipping !!                */
/* (too dangerous to limit flipping under numerics)                          */
/* so no real changes so far !!                                              */
/*                                                                           */
/* To disable return 1 here                                                  */
/*                                                                           */
int calc_collinear(ppt dpp1, ppt dpp2, ppt dpp3) {
  struct vectordef va;
  struct vectordef vb;
  struct vectordef nu;
  
  va.x = dpp2->x - dpp1->x; 
  va.y = dpp2->y - dpp1->y;
  va.z = dpp2->z - dpp1->z;
  vb.x = dpp3->x - dpp1->x; 
  vb.y = dpp3->y - dpp1->y;
  vb.z = dpp3->z - dpp1->z;

  nu.x = (va.y * vb.z - vb.y * va.z);
  nu.y = (va.z * vb.x - vb.z * va.x);
  nu.z = (va.x * vb.y - vb.x * va.y);

  /* collinear test could use an epsilon instead of zero (MINN2_EPS !!)      */
  if((nu.x*nu.x + nu.y*nu.y + nu.z*nu.z) <= 0.0) {
    msg_dinfo("Tested for and found a collinear triangle \n");
    return 0;
  }

  return 1;

}





/*****************************************************************************/
/*                                                                           */
/*  calc_H_T()                                                               */
/*                                                                           */
/*****************************************************************************/
/* Calculates circumcenter H and the vector T:                               */
/* T = dH  | if length of vector DPP2-DPP1 is varied !                       */
/* Therefore, the order of input points is important.                        */
/* returns 0 if the area of the triangle is too small                        */
int calc_H_T(ppt dpp1, ppt dpp2, ppt dpp3, 
	     struct vectordef *dppH, struct vectordef *dppT) {
  double DD;
  double h;
  double A;
  double B;
  struct vectordef va;
  struct vectordef vb;
  struct vectordef nu;

  /* length of va varies                                                     */
  va.x = dpp2->x - dpp1->x; 
  va.y = dpp2->y - dpp1->y;
  va.z = dpp2->z - dpp1->z;
  vb.x = dpp3->x - dpp1->x; 
  vb.y = dpp3->y - dpp1->y;
  vb.z = dpp3->z - dpp1->z;

  A = -va.x * (dpp1->x + dpp2->x) -
       va.y * (dpp1->y + dpp2->y) -
       va.z * (dpp1->z + dpp2->z);
  B = -vb.x * (dpp1->x + dpp3->x) -
       vb.y * (dpp1->y + dpp3->y) -
       vb.z * (dpp1->z + dpp3->z);

  nu.x = (va.y * vb.z - vb.y * va.z);
  nu.y = (va.z * vb.x - vb.z * va.x);
  nu.z = (va.x * vb.y - vb.x * va.y);

  /* collinear test could use an epsilon instead of zero                     */
  h = nu.x*nu.x + nu.y*nu.y + nu.z*nu.z; 
  if(h == 0.0) {
    msg_warn("+WARNING+: Detected a collinear triangle \n");
    return 0;
  }

  h = 0.5 / h; 

  DD = 2 * (nu.x * dpp1->x + nu.y * dpp1->y + nu.z * dpp1->z);

  dppH->x = h * (A * (nu.y*vb.z - vb.y*nu.z) + 
		 B * (va.y*nu.z - nu.y*va.z) + 
		 DD * nu.x);
  dppH->y = h * (A * (vb.x*nu.z - nu.x*vb.z) + 
		 B * (va.z*nu.x - nu.z*va.x) + 
		 DD * nu.y);
  dppH->z = h * (A * (vb.y*nu.x - nu.y*vb.x) + 
		 B * (va.x*nu.y - nu.x*va.y) + 
		 DD * nu.z);

  /* calculate T                                                             */
  h *= (va.x*va.x + va.y*va.y + va.z*va.z);
  dppT->x = h * (nu.y * vb.z - vb.y * nu.z);
  dppT->y = h * (nu.z * vb.x - vb.z * nu.x);
  dppT->z = h * (nu.x * vb.y - vb.x * nu.y);
	    
  return 1;

}




/*****************************************************************************/
/*                                                                           */
/*  calc_l_M()                                                               */
/*                                                                           */
/*****************************************************************************/
/* Calculates circumcenter M of the sphere defined by the triangle and P4.   */
/* calculates lambda                                                         */
/* returns 0 if the volume of the tetrahedron defined by the triangle and P4 */
/* is too small (M and lambda remain unchanged)                              */
/* Usually, the determinant is positive because the considered points are on */
/* the frontside of a triangle. However, points on the backside are allowed  */
/* resulting in a mirrored lambda: For a negative determinant the closer the */
/* point to the triangle (on the backside), the greater the value of lambda !*/
/*                                                                           */
/* USED EPSILON:                                                             */
/*  VOL_EPS           (can be 0.0)                                           */
/*                                                                           */
int calc_l_M(mesh_p_t mesh_p, 
	     ppt dpp4, 
	     struct triangle_paramdef* tpp, 
	     struct vectordef* vM_p,
	     double* lambdap) {

  struct vectordef vc;
  double det;
  double C;
  ppt dpp3 = tpp->tri_p->dppp[2];
  
  /* Vector vx4 - vx3                                                        */
  vc.x = dpp4->x - dpp3->x; 
  vc.y = dpp4->y - dpp3->y;
  vc.z = dpp4->z - dpp3->z;

  C = -vc.x * (dpp4->x + dpp3->x) 
      -vc.y * (dpp4->y + dpp3->y) 
      -vc.z * (dpp4->z + dpp3->z);

  /* determinant is volume of prism                                          */
  det = tpp->nu.x * vc.x + tpp->nu.y * vc.y + tpp->nu.z * vc.z;
  if(det<=mesh_p->VOL_EPS && det>=(-mesh_p->VOL_EPS)) {
    /* not a warning because it depends on the caller of this function       */
    msg_dinfo("Volume of tetrahedron is too small to compute lambda \n");
    return 0;
  }

  det = 1.0 / (2*det);

  /* M is circumcenter of tetrahedron                                        */
  vM_p->x = det * (tpp->A * (tpp->vb.z*vc.y - vc.z*tpp->vb.y) + 
		   tpp->B * (tpp->va.y*vc.z - vc.y*tpp->va.z) -
		   C * tpp->nu.x);
  vM_p->y = det * (tpp->A * (tpp->vb.x*vc.z - vc.x*tpp->vb.z) + 
		   tpp->B * (tpp->va.z*vc.x - vc.z*tpp->va.x) -
		   C * tpp->nu.y);
  vM_p->z = det * (tpp->A * (tpp->vb.y*vc.x - vc.y*tpp->vb.x) + 
		   tpp->B * (tpp->va.x*vc.y - vc.x*tpp->va.y) -
		   C * tpp->nu.z);

  *lambdap = (vM_p->x - dpp3->x) * tpp->n.x +
             (vM_p->y - dpp3->y) * tpp->n.y +
	     (vM_p->z - dpp3->z) * tpp->n.z;

  return 1;

}




/*****************************************************************************/
/*                                                                           */
/*  calc_l()                                                                 */
/*                                                                           */
/*****************************************************************************/
/* Calculates lambda for the sphere defined by the triangle and P4           */
/* returns 0 if the volume of the tetrahedron is too small (lam is unchanged)*/
/* Usually, the determinant is positive because the considered points are on */
/* the frontside of a triangle. However, points on the backside are allowed  */
/* resulting in a mirrored lambda: For a negative determinant the closer the */
/* point to the triangle (on the backside), the greater the value of lambda !*/
/*                                                                           */
/* USED EPSILON:                                                             */
/*  VOL_EPS           (can be 0.0)                                           */
/*                                                                           */
int calc_l(mesh_p_t mesh_p, 
	   ppt dpp4, 
	   struct triangle_paramdef* tpp, 
	   double* lambdap) {

  struct vectordef vc;
  struct vectordef M;
  double det;
  double C;
  ppt dpp3 = tpp->tri_p->dppp[2];  

  /* Vector vx4 - vx3                                                        */
  vc.x = dpp4->x - dpp3->x; 
  vc.y = dpp4->y - dpp3->y;
  vc.z = dpp4->z - dpp3->z;

  C = -vc.x * (dpp4->x + dpp3->x) - 
       vc.y * (dpp4->y + dpp3->y) - 
       vc.z * (dpp4->z + dpp3->z);

  /* determinant is volume of prism                                          */
  det = tpp->nu.x * vc.x + tpp->nu.y * vc.y + tpp->nu.z * vc.z;
  if(det<=mesh_p->VOL_EPS && det>=(-mesh_p->VOL_EPS)) {
    /* not a warning because it depends on the caller of this function       */
    msg_dinfo("Volume of tetrahedron is too small to compute lambda \n");
    return 0;
  }

  det = 1.0 / (2*det);

  /* M is circumcenter of tetrahedron                                        */
  M.x = det * (tpp->A * (tpp->vb.z*vc.y - vc.z*tpp->vb.y) + 
	       tpp->B * (tpp->va.y*vc.z - vc.y*tpp->va.z) -
	       C * tpp->nu.x);
  M.y = det * (tpp->A * (tpp->vb.x*vc.z - vc.x*tpp->vb.z) + 
	       tpp->B * (tpp->va.z*vc.x - vc.z*tpp->va.x) -
	       C * tpp->nu.y);
  M.z = det * (tpp->A * (tpp->vb.y*vc.x - vc.y*tpp->vb.x) + 
	       tpp->B * (tpp->va.x*vc.y - vc.x*tpp->va.y) -
	       C * tpp->nu.z);

  *lambdap = (M.x - dpp3->x) * tpp->n.x +
             (M.y - dpp3->y) * tpp->n.y +
             (M.z - dpp3->z) * tpp->n.z;

  return 1;

}



/*****************************************************************************/
/*                                                                           */
/*  check_side()                                                             */
/*                                                                           */
/*****************************************************************************/
/* Determines whether or not a point can be considered as a fourth point     */
/* to form a tetrahedron with the base triangle.                             */
/* Thereby, mainly checking if the point does not belong to the RLIST and    */
/* is inside of the sphere defined as in function calc_region()              */
/* (requires initialized M and R2 in lambda structure)                       */
/* (may not use R ! )                                                        */
/* Mostly used to make the PLIST                                             */
/* returns 1 if on the correct side of the given triangle and inside of the  */
/* sphere.                                                                   */
/*                                                                           */
/* USED EPSILONS:                                                            */
/*  pN_EPS: usually set to 0.0, limits height of tetraeders                  */
/*          value greater than 0.0 may cause holes !!                        */
/*                                                                           */
int check_side(mesh_p_t mesh_p, struct client_datadef* cli, ppt dpp) {
  struct vectordef va;
  int i;
  double d;

  /* FIRST: check whether point belongs to base triangle                     */
  if(cli->tri.tri_p->dppp[0]==dpp ||
     cli->tri.tri_p->dppp[1]==dpp ||
     cli->tri.tri_p->dppp[2]==dpp   ) return 0;
  
  /* SECOND: look for points in rlist                                        */
  for(i=0 ; i < cli->lambda.rlist.n ; i++) 
    if( ((ppt*)cli->lambda.rlist.vp)[i] == dpp ) return 0;

  /* Calculate distance using dppp[0]                                        */
  va.x = dpp->x - cli->tri.tri_p->dppp[0]->x;
  va.y = dpp->y - cli->tri.tri_p->dppp[0]->y;
  va.z = dpp->z - cli->tri.tri_p->dppp[0]->z;
  d = (va.x*cli->tri.n.x + va.y*cli->tri.n.y + va.z*cli->tri.n.z);

  /* THIRD: points only on positive side                                     */
  if(d <= (mesh_p->pN_EPS)) return 0;

  /* FOURTH: *need* to exclude points close to the plane and outside of the  */
  /* base-circle! (lambda could be infinitely negative)                      */
  /* vector M->P (M from calc_region!)                                       */
  va.x = dpp->x - cli->lambda.M.x; 
  va.y = dpp->y - cli->lambda.M.y; 
  va.z = dpp->z - cli->lambda.M.z; 
  if ((va.x*va.x + va.y*va.y + va.z*va.z) > cli->lambda.R2) {
    /* point outside sphere                                                  */
    return 0; 
  }

  /* NOW: point on right side                                                */

  return 1;

}



/*****************************************************************************/
/*                                                                           */
/*  move_point()                                                             */
/*                                                                           */
/*****************************************************************************/
/* moves a single point without any restrictions                             */
/* may apply Newton-damping coefficient                                      */
/* trivial function but need function pointer (*)                            */
/*                                                                           */
/* first, point should be removed from the octree                            */
/*                                                                           */
/* Arg mesh_p is needed to have the same prototype as rest_move_point() for  */
/* relaxation functions. It might be useful for e.g. coefficient parameters  */
/*                                                                           */
void move_point(mesh_p_t mesh_p, ppt dpp, struct vectordef* vdx_p) {
  
  dpp->x += vdx_p->x;
  dpp->y += vdx_p->y;
  dpp->z += vdx_p->z;

}




/*****************************************************************************/
/*                                                                           */
/*  rest_move_point()                                                        */
/*                                                                           */
/*****************************************************************************/
/* Checks the topography to determine how much a point can be moved without  */
/* resulting in inconsistent connected elements                              */
/* may apply Newton-damping coefficient                                      */
/* not yet implemented                                                       */
/*                                                                           */
/* USED PARAMETERS:                                                          */
/*  GD1_PAR                                                                  */
/*  GD8_PAR                                                                  */
/*                                                                           */
/* first, point should be removed from the octree                            */
/*                                                                           */
void rest_move_point(mesh_p_t mesh_p, ppt dpp, struct vectordef* vdx_p) {
  double max;

  max = vdx_p->x*vdx_p->x + vdx_p->y*vdx_p->y + vdx_p->z*vdx_p->z;

  if(max > GD8_PAR) {
    dpp->x += vdx_p->x * sqrt(GD8_PAR / max) ;
    dpp->y += vdx_p->y * sqrt(GD8_PAR / max) ;
    dpp->z += vdx_p->z * sqrt(GD8_PAR / max) ;
  } 
  else {
    dpp->x += vdx_p->x / GD1_PAR ;
    dpp->y += vdx_p->y / GD1_PAR ;
    dpp->z += vdx_p->z / GD1_PAR ;
  }
}



/*****************************************************************************/
/*                                                                           */
/*  oct_move_points()                                                        */
/*                                                                           */
/*****************************************************************************/
/* Translates a set of points along a given vector.                          */
/* Takes care about the octree: First removing the points to an slist, and   */
/* then reinserting the moved points                                         */
/* Does not check for topographical inconsistencies even though it moves     */
/* points that might be connected to elements.                               */
/*                                                                           */
/* Returns the number of moved points.                                       */
/* If reinsertion is impossible than ALL points will remain unchanged.       */
/*                                                                           */
int oct_move_points(struct octree_def* oct_p, 
		    ppt dppmin, 
		    ppt dppmax, 
		    struct vectordef* v_p) {

  struct arraydef point_list;
  struct arraydef rempoint_list;
  int i, j;

  array_init(&point_list, 3, sizeof(ppt));
  array_init(&rempoint_list, 1, sizeof(ppt)); /* n=max for this array allowed*/


  /* points are removed from the octree and the octree_counter is updated    */
  ext_del(dppmin, 
	  dppmax, 
	  cb_rem_p2sl, 
	  &point_list, 
	  oct_p,
	  null);
  if(point_list.n == 0) {
    msg_info("No point was found to be moved \n");
    free(point_list.vp);
    free(rempoint_list.vp);
    return 0;
  }

  for(i=0 ; i<point_list.n ; i++) {
    ((ppt*)point_list.vp)[i]->x += v_p->x;
    ((ppt*)point_list.vp)[i]->y += v_p->y;
    ((ppt*)point_list.vp)[i]->z += v_p->z;
  
    if( !inside_bb((*oct_p), ((ppt*)point_list.vp)[i]) ) {
      /* point has been moved out of the bounding box                        */
      msg_info("point was moved out of BB ! (inserting original points) \n");
      break;
    }

    /* reinserting moved point                                               */
    if( oct_insert_point(((ppt*)point_list.vp)[i], oct_p) != null) {
      msg_info("Cannot insert moved point because of other point, ");
      msg_info("inserting original points \n");
      break;
    }

    /* moved point has sucessfully been inserted                             */
  }
  
  if(i < point_list.n) {
    /* there were difficulties, restore all original points                  */

    /* first remove all already inserted points                              */
    for(j=0 ; j<i ; j++) {
      rempoint_list.n = 0;
      ext_del(((ppt*)point_list.vp)[j],
	      ((ppt*)point_list.vp)[j],
	      cb_rem_p2sl, 
	      &rempoint_list, 
	      oct_p,
	      null);
      if(rempoint_list.n == 0) {
	msg_fatal("+FATAL+: Could not relocate inserted point in octree\n");
	exit(0);
      }
      /* do not need to keep the pointer to the point (exists in point_list) */
    }

    /* Now all points are removed from the octree                            */
    /* restore the coordinates                                               */
    for(j=0 ; j<=i ; j++) {
      ((ppt*)point_list.vp)[j]->x -= v_p->x;
      ((ppt*)point_list.vp)[j]->y -= v_p->y;
      ((ppt*)point_list.vp)[j]->z -= v_p->z;
    }
    
    /* Now all points can be reinserted at their original positions          */
    for(i=0 ; i<point_list.n ; i++) {
      if( oct_insert_point(((ppt*)point_list.vp)[i], oct_p) != null) {
	msg_fatal("+FATAL+: Numerical problems with reinserting original point\n");
	exit(0);
      }
    }
    
    free(point_list.vp);
    free(rempoint_list.vp);
    
    return 0;

  } 

  /* successfully inserted all moved points                                  */
  free(point_list.vp);
  free(rempoint_list.vp);

  return(i);

}    





/*****************************************************************************/
/*                                                                           */
/*  intersect_tri_line()                                                     */
/*                                                                           */
/*****************************************************************************/
/* Calculates the intersection I of a triangle (tri_p) with a line given by  */
/* a normalized vector (va_p, |va|=1), a middle point (vH_p), and half the   */
/* line length (maxl)                                                        */
/* This also means that the projection of the middle point onto the triangle */
/* along the given vector with a maximum distance of half the line length    */
/* is searched for. (is e.g. needed for steiner point insertion on surface)  */
/*                                                                           */
/* Return values:                                                            */
/* If the intersection lies on an edge, the index of the point opposite of   */
/* the edge will be returned.                                                */
/* If the intersection lies inside of the triangle 3 will be returned.       */
/* If there is no intersection 4 will be returned.                           */
/* No distinction is made as to where the point lies outside                 */
/*                                                                           */
/* The epsilons can be used to slightly modify the location of the           */
/* intersection point. For refinement where the resulting triangles are not  */
/* subject to additional quality tests, it makes sense to move an            */
/* intersection point which is very close to an edge on to the edge.         */
/* EDGO_EPS should always be very small.                                     */
/* EDGI_EPS may be larger to collapse intersection points with an edge.      */
/*                                                                           */
/* USED EPSILONS:                                                            */
/*  EDGO_EPS     can be zero                                                 */
/*  EDGI_EPS     can be zero                                                 */
/*  ITL_EPS      to avoid intersection between almost parallel line/plane    */
/*                                                                           */
int intersect_tri_line(tri_pt tri_p, 
		       struct vectordef* va_p, 
		       struct vectordef* vH_p, 
		       ppt dppI,
		       double maxl) {

  struct vectordef vn;
  struct vectordef va;
  struct vectordef vb;
  double t;
  double a;
  double b;
  double h1;
  double h2;


  va.x = tri_p->dppp[1]->x - tri_p->dppp[0]->x; 
  va.y = tri_p->dppp[1]->y - tri_p->dppp[0]->y; 
  va.z = tri_p->dppp[1]->z - tri_p->dppp[0]->z; 
  vb.x = tri_p->dppp[2]->x - tri_p->dppp[0]->x; 
  vb.y = tri_p->dppp[2]->y - tri_p->dppp[0]->y; 
  vb.z = tri_p->dppp[2]->z - tri_p->dppp[0]->z; 

  /* va x vb                                                                 */
  vn.x = (va.y * vb.z - vb.y * va.z);   
  vn.y = (va.z * vb.x - vb.z * va.x);   
  vn.z = (va.x * vb.y - vb.x * va.y);
  a = (vn.x * va_p->x + vn.y * va_p->y + vn.z * va_p->z);

  /* NORMALIZE n for test if parallel !                                      */
  /* asssume va_p to already be normalized                                   */
  b  = vn.x * vn.x;
  h1 = vn.y * vn.y;
  h2 = vn.z * vn.z;
  t = a / sqrt(b + h1 + h2);

  /* now t is the cosine of the angle between n and va_p                     */
  if( t<=ITL_EPS  &&  t>=-(ITL_EPS) ) {
    /* almost parallel, will not make sense to compute intersection          */
    msg_dinfo("No intersection because triangle and line are parallel \n");
    return 4;
  }

  t = ((tri_p->dppp[0]->x - vH_p->x) * vn.x + 
       (tri_p->dppp[0]->y - vH_p->y) * vn.y + 
       (tri_p->dppp[0]->z - vH_p->z) * vn.z) / a;

  /* testing maxl                                                            */
  if(t <= -maxl || t >= maxl) {
    msg_dinfo("No intersection because line does not reach the triangle \n");
    return 4;
  }

  /* calculate projected point                                               */
  dppI->x = vH_p->x  +  t * va_p->x;
  dppI->y = vH_p->y  +  t * va_p->y;
  dppI->z = vH_p->z  +  t * va_p->z;
  

  /* should also test if triangle is collinear, but can do that for the case */
  /* where b == h1 == h2 (== 0.0)  (comparison '>=' is important not '>')    */
  /* will pretend that the point is simply not inside                        */

  /* check if inside of the triangle                                         */
  if(b >= h1) {
    if(b >= h2) { 
      /* only use y and z                                                    */

      /* collinear test only here                                            */
      if(vn.x == 0.0) {
	/* Setting any ctags? definitely not blocked! neither req. nor good  */
	/* tri_p->ctag |= ;                                                  */
	msg_warn("+WARNING+: Detected a collinear triangle \n");
	return 4;
      }

      h1 = dppI->y - tri_p->dppp[0]->y;
      h2 = dppI->z - tri_p->dppp[0]->z;
      t = 1.0 / vn.x;
      a = t * (h1 * vb.z - vb.y * h2);
      b = t * (va.y * h2 - h1 * va.z);
    }
    else { 
      /* only use x and y                                                    */
      h1 = dppI->x - tri_p->dppp[0]->x;
      h2 = dppI->y - tri_p->dppp[0]->y;
      t = 1.0 / vn.z;
      a = t * (h1 * vb.y - vb.x * h2);
      b = t * (va.x * h2 - h1 * va.y);
    }
  } else if(h1 >= h2) { 
    /* only use x and z                                                      */
    h1 = dppI->x - tri_p->dppp[0]->x;
    h2 = dppI->z - tri_p->dppp[0]->z;
    t = -1.0 / vn.y;
    a = t * (h1 * vb.z - vb.x * h2);
    b = t * (va.x * h2 - h1 * va.z);
  } else {
    /* only use x and y                                                      */
    h1 = dppI->x - tri_p->dppp[0]->x;
    h2 = dppI->y - tri_p->dppp[0]->y;
    t = 1.0 / vn.z;
    a = t * (h1 * vb.y - vb.x * h2);
    b = t * (va.x * h2 - h1 * va.y);
  }

  /* careful epsilons can be zero                                            */
  if( a < -(EDGO_EPS)  ||  b < -(EDGO_EPS)  ||  (a+b) > (1.0+(EDGO_EPS)) ) {
    msg_dinfo("Intersection point of line lies outside of the triangle \n");
    return 4;
  }
  

  /* Change close to edge points so that excactly on the edge                */
  /* otherwise dangerous: same problems as if point was moved                */
  if(a<=(EDGI_EPS)) {
    dppI->x = tri_p->dppp[0]->x + b*vb.x;
    dppI->y = tri_p->dppp[0]->y + b*vb.y;
    dppI->z = tri_p->dppp[0]->z + b*vb.z;
    return 1;
  }
  
  if(b<=(EDGI_EPS)) {
    dppI->x = tri_p->dppp[0]->x + a*va.x;
    dppI->y = tri_p->dppp[0]->y + a*va.y;
    dppI->z = tri_p->dppp[0]->z + a*va.z;
    return 2;
  }

  if((a+b)>=(1.0-(EDGI_EPS))) {
    h1 = 0.5 * (1 + a - b);
    h2 = 0.5 * (1 + b - a);
    dppI->x = tri_p->dppp[0]->x + h1*va.x + h2*vb.x;
    dppI->y = tri_p->dppp[0]->y + h1*va.y + h2*vb.y;
    dppI->z = tri_p->dppp[0]->z + h1*va.z + h2*vb.z;
    return 0;
  }
  
  return 3;
  
}




/*****************************************************************************/
/*                                                                           */
/*  intersect_plane_line()                                                   */
/*                                                                           */
/*****************************************************************************/
/* Calculates the intersection I of a plane with a line given by two points. */
/* The plane can be given by any three points.                               */
/*                                                                           */
/* Return values:                                                            */
/*   1    intersection is I                                                  */
/*   0    no intersection (line is parallel or does not reach the plane or   */
/*        plane is not well defined)                                         */
/*                                                                           */
/* USED EPSILONS:                                                            */
/*  ITL_EPS      to avoid intersection between almost parallel line/plane    */
/*                                                                           */
int intersect_plane_line(ppt dppP0, 
			 ppt dppP1, 
			 ppt dppP2, 
			 ppt dppL0, 
			 ppt dppL1, 
			 ppt dppI) {

  struct vectordef vn;
  struct vectordef va;
  struct vectordef vb;
  double t;
  double a;
  double h1;

  va.x = dppP1->x - dppP0->x; 
  va.y = dppP1->y - dppP0->y; 
  va.z = dppP1->z - dppP0->z; 
  vb.x = dppP2->x - dppP0->x; 
  vb.y = dppP2->y - dppP0->y; 
  vb.z = dppP2->z - dppP0->z; 

  /* va x vb                                                                 */
  vn.x = (va.y * vb.z - vb.y * va.z);   
  vn.y = (va.z * vb.x - vb.z * va.x);   
  vn.z = (va.x * vb.y - vb.x * va.y);

  h1 = sqrt(vn.x*vn.x + vn.y*vn.y + vn.z*vn.z);
  if(h1 == 0.0) {
    /* plane is not well defined                                             */
    return 0;
  }
  h1 = 1.0/h1;
  vn.x *= h1;
  vn.y *= h1;
  vn.z *= h1;

  /* now vn is normalized n vector for the plane                             */

  va.x = dppL1->x - dppL0->x; 
  va.y = dppL1->y - dppL0->y; 
  va.z = dppL1->z - dppL0->z; 
  
  a = sqrt(va.x*va.x + va.y*va.y + va.z*va.z);
  if(a == 0.0) {
    /* line is not well defined                                              */
    return 0;
  }
  h1 = 1.0/a;
  va.x *= h1;
  va.y *= h1;
  va.z *= h1;

  /* now va is the normalized line vector and a is the length of the line    */
  
  t = vn.x*va.x + vn.y*va.y + vn.z*va.z;

  /* now t is the cosine of the angle between n and va (can be negative)     */

  if( t<=ITL_EPS  &&  t>=-(ITL_EPS) ) {
    /* almost parallel, will not make sense to compute intersection          */
    msg_dinfo("No intersection because plane and line are parallel \n");
    return 0;
  }

  t = ((dppP0->x - dppL0->x) * vn.x + 
       (dppP0->y - dppL0->y) * vn.y + 
       (dppP0->z - dppL0->z) * vn.z) / t;

  /* now t is the distance along va of the point I to the point L0           */

  /* testing length                                                          */
  if(t <= 0.0 || t >= a) {
    /* Usually need a clear intersection, I==L0/L1 is not usable             */
    msg_dinfo("Line does not reach the plane or merely touches it !\n");
    return 0;
  }

  /* calculate intersection point                                            */
  dppI->x = dppL0->x  +  t * va.x;
  dppI->y = dppL0->y  +  t * va.y;
  dppI->z = dppL0->z  +  t * va.z;
  
  return 1;
  
}





/*****************************************************************************/
/*                                                                           */
/*  inside_tri_test()                                                        */
/*                                                                           */
/*****************************************************************************/
/* Checks whether a given point X lying in the same plane as the triangle    */
/* is inside or outside of the triangle                                      */
/* Requires the triangle parameter structure and will assume the triangle    */
/* to be not collinear !                                                     */
/* If it would be collinear the triangle structure would not have been       */
/* initialized by e.g. calc_n_H() !                                          */
/*                                                                           */
/* For safety will project the point into the plane to avoid false results   */
/*                                                                           */
/* The point will not be modified !                                          */
/* However, if it lies close to an edge (EDGI_EPS) the index of the opposite */
/* triangle vertex will be returned.                                         */
/* The value of EDGI_EPS will usually not matter, because points inside of   */
/* a triangle but interpreted to be on an edge are in any case considered to */
/* belong to the triangle.                                                   */
/* The value of EDGO_EPS should be small, because points on an edge or very  */
/* close to an edge from the outside are considered to belong to the triangle*/
/* whereas points further away definitely do not belong to the triangle.     */
/*                                                                           */
/* RETURN VALUES:                                                            */
/* Depend on the calculation of va, vb in the triangle parameter structure ! */
/* Should be calculated by e.g. calc_n_H() and therefore va: 0->1 vb: 0->2   */
/* The indices which are returned here are relative to va, vb                */
/* (Index 0 means the common point of va and vb.                             */
/*  Index 1 means the point soley belonging to va.                           */
/*  Index 2 means the point soley belonging to vb.                           */
/*  However, if va or vb have the wrong orientation the results are wrong !) */
/*                                                                           */
/* If the intersection lies on an edge, the index of the point opposite of   */
/* the edge will be returned.                                                */
/* If the intersection lies inside of the triangle 3 will be returned.       */
/* If there is no intersection:                                              */
/*    4      outside triangle vertex 0                                       */
/*    5      outside triangle vertex 1                                       */
/*    6      outside triangle vertex 2                                       */
/*    7      outside edge with vertices 1,2                                  */
/*    8      outside edge with vertices 2,0                                  */
/*    9      outside edge with vertices 0,1                                  */
/*                                                                           */
/* USED EPSILONS:                                                            */
/*  EDGO_EPS     can be zero                                                 */
/*  EDGI_EPS     can be zero                                                 */
/*                                                                           */
int inside_tri_test(struct triangle_paramdef* tpp, ppt dppX) {
  struct vectordef vX;
  double t;
  double a;
  double b;
  double h1;
  double h2;
  ppt dpp0;
  
  dpp0 = tpp->tri_p->dppp[0];

  t = ( (dpp0->x - dppX->x) * tpp->n.x + 
        (dpp0->y - dppX->y) * tpp->n.y + 
        (dpp0->z - dppX->z) * tpp->n.z );

  /* calculate projected point                                               */
  vX.x = dppX->x  +  t * tpp->n.x;
  vX.y = dppX->y  +  t * tpp->n.y;
  vX.z = dppX->z  +  t * tpp->n.z;
  
  /* determine best plane for two dimensional inside test                    */
  b  = tpp->nu.x * tpp->nu.x;
  h1 = tpp->nu.y * tpp->nu.y;
  h2 = tpp->nu.z * tpp->nu.z;

  if(b >= h1) {
    if(b >= h2) { 
      /* only use y and z                                                    */
      h1 = vX.y - dpp0->y;
      h2 = vX.z - dpp0->z;
      /* Cannot be collinear and nu.x is largest positive                    */
      t = 1.0 / tpp->nu.x;
      a = t * (h1 * tpp->vb.z - tpp->vb.y * h2);
      b = t * (tpp->va.y * h2 - h1 * tpp->va.z);
    }
    else { 
      /* only use x and y                                                    */
      h1 = vX.x - dpp0->x;
      h2 = vX.y - dpp0->y;
      t = 1.0 / tpp->nu.z;
      a = t * (h1 * tpp->vb.y - tpp->vb.x * h2);
      b = t * (tpp->va.x * h2 - h1 * tpp->va.y);
    }
  } else if(h1 >= h2) { 
    /* only use x and z                                                      */
    h1 = vX.x - dpp0->x;
    h2 = vX.z - dpp0->z;
    t = -1.0 / tpp->nu.y;
    a = t * (h1 * tpp->vb.z - tpp->vb.x * h2);
    b = t * (tpp->va.x * h2 - h1 * tpp->va.z);
  } else { 
    /* only use x and y                                                      */
    h1 = vX.x - dpp0->x;
    h2 = vX.y - dpp0->y;
    t = 1.0 / tpp->nu.z;
    a = t * (h1 * tpp->vb.y - tpp->vb.x * h2);
    b = t * (tpp->va.x * h2 - h1 * tpp->va.y);
  }


  /* CAUTION: Choice of epsilons (EDGI or EDGO) and equality (==) matters !! */
  /* Especially because epsilons could be zero as well !                     */
  /* Outside the triangle the meaning is inverse: points on a prolonged edge */
  /* (a or b equals zero) are to be considered outside of a vertex instead of*/
  /* outside of an edge !                                                    */
  /* Cannot skip the 3 decision-sub-regions for each initial if-case because */
  /* the already tested regions do not exactly match the other regions to be */
  /* tested (<, <= !!). Therefore it only seems that there are some if-cases */
  /* redundant !                                                             */

  if(a < -(EDGO_EPS)) { 
    if(b <= (EDGO_EPS)) {
      /* a<0  AND  b<=0 : outside vertex 0                                   */
      return 4;
    }
    if( (a+b) < (1.0-(EDGO_EPS)) ) { 
      /* a<0  AND  b ok : outside edge 2,0                                   */
      return 8;
    }

    /* a<0 AND a+b>=1 : outside vertex 2                                     */
    return 6;
  }

  if(b < -(EDGO_EPS)) { 
    if(a <= (EDGO_EPS)) {
      /* a<=0  AND  b<0 : outside vertex 0                                   */
      return 4;
    }
    if( (a+b) < (1.0-(EDGO_EPS)) ) { 
      /* a ok  AND  b<0 : outside edge 0,1                                   */
      return 9;
    }

    /* b<0 AND a+b>=1 : outside vertex 1                                     */
    return 5;
  }

  if( (a+b) > (1.0+(EDGO_EPS)) ) { 
    if(a <= (EDGO_EPS)) {
      /* a<=0 AND a+b>1 : outside vertex 2                                   */
      return 6;
    }
    if(b <= (EDGO_EPS)) { 
      /* b<=0 AND a+b>1 : outside vertex 1                                   */
      return 5;
    }

    /* a,b >0 AND a+b>1 : outside edge 1,2                                   */
    return 7;
  }


  /* Now the point must be inside the triangle or on an edge                 */
  if(a<=(EDGI_EPS)) {
    return 1;
  }

  if(b<=(EDGI_EPS)) {
    return 2;
  }

  if((a+b)>=(1.0-(EDGI_EPS))) {
    return 0;
  }

  return 3;

}


/*****************************************************************************/
/*                                                                           */
/*  check_del_tri()                                                          */
/*                                                                           */
/*****************************************************************************/
/* Checks a triangle for its Delaunay property and generates the slists      */
/* of the various types of disturbing points                                 */
/* (dpar points are not deleted from here, but rather the dpar_list is made) */
/*                                                                           */
/* Requires an initialized cli->tri.tri_p                                    */
/* and allocated arrays in the client structure                              */
/*                                                                           */
/* USED EPSILON:                                                             */
/*  WSBREF_EPS                                                               */
/*                                                                           */
/* Returns:                                                                  */
/*         0    triangle is not Delaunay                                     */
/*         1    triangle is Delaunay                                         */
/*         2    triangle is collinear and no calculations were possible      */
/*                                                                           */
/* (This is a *real* Delaunay criterion, so if it is not Delaunay there is   */
/*  really no sphere regardless of size existent which would be empty !)     */
/*                                                                           */
/* First smallest sphere test than possibly second sphere test.              */
/* However, the point lists are generated according to the smallest sphere.  */
/* That makes sense because the second sphere might be extremely large and   */
/* the two sides of the triangle would not be treated equally.               */
/* Also, further decisions should be based on the vicinity and thus the      */
/* smallest sphere.                                                          */
/*                                                                           */
/* Unconn. points which are close to the triangle (SDEL_PAR) are stored in   */
/* the uneps_list and dpar_list.                                             */
/* Their removal may lead to strange and unmotivated point deletions when the*/
/* triangle has extremely bad shape. Testing if the points not only lie      */
/* close to the plane and within the sphere but also inside of the triangle  */
/* can resolve this.                                                         */
/* Especially, points might be unnecessarily deleted before a flipping step. */
/*                                                                           */
/* If it is Delaunay the lists might still not be empty (2nd sphere test     */
/* went okay but in smallest sphere points might exist)                      */
/* M, R2, and lambda might be slightly distorted due to the 2nd sphere test  */
/* Requires that the sign of the calculated lambda of the backside remains   */
/* original (uncorrected) !                                                  */
/*                                                                           */
int check_del_tri(mesh_p_t mesh_p, struct surf_clientdef* cli) {
  struct pointtype dpmin;
  struct pointtype dpmax;
  double R;
  
  /* Initialize triangle parameters                                          */
  /* need also n-vector for minimum distance and lp1h                        */
  if(!calc_n_H(&cli->tri)) {
    /* allow for skipping of the triangle in case of degenerate polygon      */
    /* triangulations where flipping might resolve the collinearity          */
    cli->validforthistri = 0;
    return 2;
  }

  /* Initialize counters, lists, and bestdpp                                 */
  cli->validforthistri = 1;
  cli->Fbestdpp = null;
  cli->Bbestdpp = null;
  cli->cneps_list.n = 0;
  cli->uneps_list.n = 0;
  cli->flip_list.n = 0;
  cli->dpar_list.n = 0;
  cli->s1unconn = 0;
  cli->s1overall = 0;
  cli->s2overall = 0;

  /* Will not add epsilon to search region as in calc_region()               */
  /* (only WSBREF_EPS to make region smaller !)                              */
  dpmin.x = cli->tri.H.x - cli->tri.lp1h + mesh_p->WSBREF_EPS;  
  dpmin.y = cli->tri.H.y - cli->tri.lp1h + mesh_p->WSBREF_EPS;
  dpmin.z = cli->tri.H.z - cli->tri.lp1h + mesh_p->WSBREF_EPS;
  dpmax.x = cli->tri.H.x + cli->tri.lp1h - mesh_p->WSBREF_EPS;  
  dpmax.y = cli->tri.H.y + cli->tri.lp1h - mesh_p->WSBREF_EPS;
  dpmax.z = cli->tri.H.z + cli->tri.lp1h - mesh_p->WSBREF_EPS;

  /* search smallest sphere and generate lists of various dist. points       */
  ext_find(&dpmin, 
	   &dpmax, 
	   cb_smsph,
	   cli, 
	   &mesh_p->global_octree,
	   mesh_p);

  if(cli->s1overall == 0) {
    /* DELAUNAY, no second sphere test                                       */
    msg_dinfo("Smallest sphere contains 0 points \n"); 
    return 1;
  }

  /* There are at least some points inside of the smallest sphere            */

  if(cli->cneps_list.n>0 || cli->uneps_list.n>0) {
    /* NOT DELAUNAY, no second sphere test                                   */
    msg_dinfo1("Smallest sphere contains %i points (neps)\n", cli->s1overall);
    return 0;
  }

  /* There are no points close to the surface                                */
  /* (there must be at least one bestdpp)                                    */

  if(cli->Fbestdpp != null && cli->Bbestdpp != null) {
    /* NOT DELAUNAY, no second sphere test                                   */
    /* Comparison of lambda of each side happens outside this function       */
    msg_dinfo1("Smallest sphere contains %i points (F+B)\n", cli->s1overall); 
    return 0;
  }

  /* There is exactly *one* bestdpp and no points close to the surface, thus */
  /* the 2nd sphere test becomes relevant !                                  */

  /* R2, lam, M is for both F+B initialized (if bestdpp!=null)               */
  /* (will for the 2nd sphere test according to WSBREF slightly be modified) */

  if(mesh_p->EQUATORSPH) { 
    msg_dinfo("2nd sphere test avoided due to equatorsph \n"); 
    return 0;
  }

  /* Initialize M and R2 for 2nd sphere test and calculate search region     */
  if(cli->Bbestdpp != null) {

    /* CAUTION ONLY ALLOW if it fits to lambda_max! otherwise unconn. tris!  */
    /* (generally for any triangle from which to create this boundary element*/
    /* a lambda between |Blam| and B_R can be required where B_R > |Blam|)   */
    if(sqrt(cli->B_R2) >= mesh_p->lambda_max) {
      msg_dinfo("2nd sphere test avoided due to lambda_max \n"); 
      return 0;
    }

    /* for searching the frontside decrease lambda                           */
    cli->Blam -= mesh_p->WSBREF_EPS;
    cli->B_M.x = cli->tri.H.x + cli->Blam * cli->tri.n.x;
    cli->B_M.y = cli->tri.H.y + cli->Blam * cli->tri.n.y;
    cli->B_M.z = cli->tri.H.z + cli->Blam * cli->tri.n.z;
    dpmin.x = cli->tri.tri_p->dppp[0]->x - cli->B_M.x;       
    dpmin.y = cli->tri.tri_p->dppp[0]->y - cli->B_M.y;
    dpmin.z = cli->tri.tri_p->dppp[0]->z - cli->B_M.z;       
    cli->B_R2 = dpmin.x*dpmin.x + dpmin.y*dpmin.y + dpmin.z*dpmin.z;  
    R = sqrt(cli->B_R2);

    dpmin.x = cli->B_M.x - R;     
    dpmin.y = cli->B_M.y - R;
    dpmin.z = cli->B_M.z - R;
    dpmax.x = cli->B_M.x + R;  
    dpmax.y = cli->B_M.y + R;
    dpmax.z = cli->B_M.z + R;
  }
  else {

    /* CAUTION ONLY ALLOW if it fits to lambda_max! otherwise unconn. tris!  */
    /* (generally for any triangle from which to create this boundary element*/
    /* a lambda between |Flam| and F_R can be required where F_R > |Flam|)   */
    if(sqrt(cli->F_R2) >= mesh_p->lambda_max) {
      msg_dinfo("2nd sphere test avoided due to lambda_max \n"); 
      return 0;
    }

    /* for searching the backside increase lambda                            */
    cli->Flam += mesh_p->WSBREF_EPS;
    cli->F_M.x = cli->tri.H.x + cli->Flam * cli->tri.n.x;
    cli->F_M.y = cli->tri.H.y + cli->Flam * cli->tri.n.y;
    cli->F_M.z = cli->tri.H.z + cli->Flam * cli->tri.n.z;
    dpmin.x = cli->tri.tri_p->dppp[0]->x - cli->F_M.x;       
    dpmin.y = cli->tri.tri_p->dppp[0]->y - cli->F_M.y;
    dpmin.z = cli->tri.tri_p->dppp[0]->z - cli->F_M.z;       
    cli->F_R2 = dpmin.x*dpmin.x + dpmin.y*dpmin.y + dpmin.z*dpmin.z;  
    R = sqrt(cli->F_R2);

    dpmin.x = cli->F_M.x - R;     
    dpmin.y = cli->F_M.y - R;
    dpmin.z = cli->F_M.z - R;
    dpmax.x = cli->F_M.x + R;  
    dpmax.y = cli->F_M.y + R;
    dpmax.z = cli->F_M.z + R;
  }
    
  /* Now search the second sphere                                            */
  ext_find(&dpmin, 
	   &dpmax,
	   cb_2ndsph,
	   cli, 
	   &mesh_p->global_octree,
	   null);

  if(cli->s2overall>0) { 
    /* NOT DELAUNAY                                                          */
    msg_dinfo1("Smallest sphere contains %i, ", cli->s1overall); 
    msg_dinfo1("Second sphere %i points \n", cli->s2overall); 
    return 0;
  }

  /* The second sphere is empty: DELAUNAY                                    */
  /* XXXXXXXXX wird nicht geflippt wegen 2ndsph !?? besser immer eqsph? BUG? */
  msg_dinfo1("Smallest sphere contains %i, ", cli->s1overall); 
  msg_dinfo("Second sphere 0 points \n"); 
  return 1;

}


/*****************************************************************************/
/*                                                                           */
/*  check_del_edge()                                                         */
/*                                                                           */
/*****************************************************************************/
/* Checks an edge for its Delaunay property and returns the closest point    */
/* (no matter whether connected, unconnected or structural and no matter     */
/*  how many disturbing points)                                              */
/* Deletes unconnected and close points right away (SDEL_PAR) !!             */
/* Will not use an epsilon for wide sense checking, because it does not make */
/* sense for an edge !                                                       */
/* (HOWEVER: should allow right angles ? (THALES-CIRCLE !!)                  */
/*                                                                           */
/* Returns:                                                                  */
/*                       null:  edge is Delaunay                             */
/*   pointer to closest point:  smallest sphere criterion not fulfilled      */
/*                          d:  normal distance                              */
/*                                                                           */
ppt check_del_edge(mesh_p_t mesh_p, ppt dpp1, ppt dpp2, double* d_p) {
  struct pointtype dpmin;
  struct pointtype dpmax;
  struct edge_clientdef eclient;

  eclient.d = 0.0;
  eclient.counter = 0;
  eclient.founddpp = null;
  eclient.dpp1 = dpp1;
  eclient.dpp2 = dpp2;
  eclient.ve.x = dpp2->x - dpp1->x;
  eclient.ve.y = dpp2->y - dpp1->y;
  eclient.ve.z = dpp2->z - dpp1->z;
  eclient.vM.x = 0.5 * (dpp1->x + dpp2->x); 
  eclient.vM.y = 0.5 * (dpp1->y + dpp2->y); 
  eclient.vM.z = 0.5 * (dpp1->z + dpp2->z); 
  eclient.R = sqrt(eclient.ve.x * eclient.ve.x +
		   eclient.ve.y * eclient.ve.y +
		   eclient.ve.z * eclient.ve.z);
  eclient.inv2R = 1.0 / eclient.R;
  eclient.R = 0.5 * eclient.R;

  dpmin.x = eclient.vM.x - eclient.R;
  dpmin.y = eclient.vM.y - eclient.R;
  dpmin.z = eclient.vM.z - eclient.R;
  dpmax.x = eclient.vM.x + eclient.R;
  dpmax.y = eclient.vM.y + eclient.R;
  dpmax.z = eclient.vM.z + eclient.R;
 
  ext_del(&dpmin,
	  &dpmax,
	  cb_smsph_edge, 
	  &eclient,
	  &mesh_p->global_octree,
	  mesh_p);

  msg_dinfo1("Smallest sphere for EDGE contains %i points\n", eclient.counter);
  
  *d_p = eclient.d;

  return(eclient.founddpp);

}



/*****************************************************************************/
/*                                                                           */
/*  check_quality_tri()                                                      */
/*                                                                           */
/*****************************************************************************/
/* checks whether a triangle fulfills additional quality criteria            */
/*                                                                           */
/* area not too large, smallest angle not too small ...                      */
/*                                                                           */
/* Return values:                                                            */
/*    0     failed quality test                                              */
/*    1     passed quality test                                              */
/*                                                                           */
/* will be using   va, vb, and nu   of structure triangle_paramdef  !        */
/*                                                                           */
/* USED PARAMETERS:                                                          */
/*  MAXAR2_PAR                                                               */
/*  MAXCOS_PAR                                                               */
/*                                                                           */
/* CAUTION by default if MAXAR2_PAR == 0.0 then do not check it !!!!!!!      */
int check_quality_tri(mesh_p_t mesh_p, struct triangle_paramdef* tpp) {
  double a2;
  double b2;
  double c2;
  double cos0;
  double cos1;
  double cos2;
  struct vectordef vc;
  

  if(mesh_p->MAXAR2_PAR != 0.0) {
    if( (tpp->nu.x*tpp->nu.x + tpp->nu.y*tpp->nu.y + tpp->nu.z*tpp->nu.z) 
	> mesh_p->MAXAR2_PAR) {
      /* area too large                                                      */
      return 0;
    }
  }

  /* thus independent of how va and vb were computed                         */
  vc.x = tpp->va.x - tpp->vb.x; 
  vc.y = tpp->va.y - tpp->vb.y;
  vc.z = tpp->va.z - tpp->vb.z;

  a2 = tpp->va.x*tpp->va.x + tpp->va.y*tpp->va.y + tpp->va.z*tpp->va.z;
  b2 = tpp->vb.x*tpp->vb.x + tpp->vb.y*tpp->vb.y + tpp->vb.z*tpp->vb.z;
  c2 = vc.x*vc.x + vc.y*vc.y + vc.z*vc.z;

  cos0 = (a2+b2-c2) / (2*sqrt(a2*b2));
  cos1 = (a2+c2-b2) / (2*sqrt(a2*c2));
  cos2 = (c2+b2-a2) / (2*sqrt(c2*b2));

  if(cos0 > mesh_p->MAXCOS_PAR || 
     cos1 > mesh_p->MAXCOS_PAR || 
     cos2 > mesh_p->MAXCOS_PAR) {
    /* too small angle                                                       */
    return 0;
  }

  return 1;
  
}



/*****************************************************************************/
/*                                                                           */
/*  intersection_test()                                                      */
/*                                                                           */
/*****************************************************************************/
/* Tests if an advancing tetrahedron intersects a leak.                      */
/* The leaks should be handled more efficiently, e.g. in a second octree.    */
/*                                                                           */
/* See issues commented in function insert_advancing_tetrahedron() as to     */
/* where this test should be performed.                                      */
/*                                                                           */
/* Returns 1 in case of intersection.                                        */
/*                                                                           */
int intersection_test(mesh_p_t mesh_p, tet_pt tet_p) {
  int i;
  struct vectordef va;
  struct vectordef vP;
  double a;
  double l;
  struct pointtype dp;
  int edge;

  if(mesh_p->leaktris.n <= 0) return 0;

  for(edge=0 ; edge<3 ; edge++) {
    vP.x = 0.5 * (tet_p->basetri_p->dppp[edge]->x + tet_p->dpp4->x);
    vP.y = 0.5 * (tet_p->basetri_p->dppp[edge]->y + tet_p->dpp4->y);
    vP.z = 0.5 * (tet_p->basetri_p->dppp[edge]->z + tet_p->dpp4->z);
  
    va.x = tet_p->basetri_p->dppp[edge]->x - tet_p->dpp4->x;
    va.y = tet_p->basetri_p->dppp[edge]->y - tet_p->dpp4->y;
    va.z = tet_p->basetri_p->dppp[edge]->z - tet_p->dpp4->z;
    
    a = sqrt(va.x*va.x + va.y*va.y + va.z*va.z);
    l = 0.5 * a;

    a = 1.0/a;
    va.x *= a;
    va.y *= a;
    va.z *= a;
    
    /* should do it with a second octree !!                                  */
    for(i=0 ; i<mesh_p->leaktris.n ; i++) {

      /* va must be normalized !                                             */
      if( intersect_tri_line(((tri_pt*)(mesh_p->leaktris.vp))[i], 
			     &va, 
			     &vP, 
			     &dp, 
			     l)
       != 4) {
	/* There is an intersection                                          */
	return 1;
      }
      
    }
  }

  /* no intersection                                                         */

  return 0;
  
}

  



