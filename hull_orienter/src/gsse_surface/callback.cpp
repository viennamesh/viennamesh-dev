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

/** For function cb_linesplit() **********************************************/
/* used to determine if a point is close enough to a line to be split        */
/* (at the moment not sine but rather absolute distance)                     */
#define LS_EPS     0.0000001






/*****************************************************************************/
/*                                                                           */
/*  cb_printout()                                                            */
/*                                                                           */
/*****************************************************************************/
/* printout nothing else                                                     */
void cb_printout(ppt dpp, void* client, mesh_p_t nup) {
  int* countp = (int*) client;

  msg_info("Point x y z : ");
  msg_info1(dfmt, dpp->x);
  msg_info1(dfmt, dpp->y);
  msg_info1(dfmt, dpp->z);
  msg_info("\n");

  (*countp)++;

}


/*****************************************************************************/
/*                                                                           */
/*  cb_dump()                                                                */
/*                                                                           */
/*****************************************************************************/
/* save points to file                                                       */
void cb_dump(ppt dpp, void* vp, mesh_p_t nup) {
  struct dump_clientdef *dc_p = (struct dump_clientdef*) vp;
  
  //put_point(dc_p->fp, dpp);
  
  dc_p->count++;
  
}


/*****************************************************************************/
/*                                                                           */
/*  cb_dumps()                                                               */
/*                                                                           */
/*****************************************************************************/
/* save only points to file that are unconnected                             */
void cb_dumps(ppt dpp, void* vp, mesh_p_t nup) {
  struct dump_clientdef *dc_p = (struct dump_clientdef*) vp;
  
  if(dpp->ntri_lp!=null) return;
  
  //put_point(dc_p->fp, dpp);
  
  dc_p->count++;
  
}


/*****************************************************************************/
/*                                                                           */
/*  cb_reset_index()                                                         */
/*                                                                           */
/*****************************************************************************/
void cb_reset_index(ppt dpp, void* vp, mesh_p_t nup) {

  dpp->point_index = init_point_index;

}


/*****************************************************************************/
/*                                                                           */
/*  cb_rem_p2sl()                                                            */
/*                                                                           */
/*****************************************************************************/
/* Removes a found point from the octree into an array of point-pointers:    */
/* slist with initialized n, where n==max is allowed !                       */
/* (That means for this slist the expansion check always has to occur first) */
/* dpp->point_index is not initialized !                                     */
/* The octree_counter is updated by ext_del                                  */
/* (number of all points = octree_counter + number of points in slist)       */
/* Also removes points from the octree if they are part of a triangle !      */
int cb_rem_p2sl(ppt dpp, void* vp, mesh_p_t nup) {
  struct arraydef* sl_p = (struct arraydef*) vp;

  if(sl_p->n==sl_p->max) expand_ar(sl_p, D_AR); 

  ((ppt*)sl_p->vp)[sl_p->n] = dpp;
  sl_p->n++;

  return 1;

}



/*****************************************************************************/
/*                                                                           */
/*  cb_copy_p2sl()                                                           */
/*                                                                           */
/*****************************************************************************/
/* copies a found point from the octree into an array of point-pointers:     */
/* (actually its pointer)                                                    */
/* slist with initialized n, where n==max is allowed !                       */
/* (That means for this slist the expansion check always has to occur first) */
/* dpp->point_index is not initialized !                                     */
void cb_copy_p2sl(ppt dpp, void* vp, mesh_p_t nup) {
  struct arraydef* sl_p = (struct arraydef*) vp;

  if(sl_p->n==sl_p->max) expand_ar(sl_p, D_AR); 
  
  ((ppt*)sl_p->vp)[sl_p->n] = dpp;
  
  sl_p->n++;
  
}




/*****************************************************************************/
/*                                                                           */
/*  cb_copy_init_p2sl()                                                      */
/*                                                                           */
/*****************************************************************************/
/* copies a found point from the octree into an array of point-pointers:     */
/* (actually its pointer)                                                    */
/* slist with initialized n, where n==max is allowed !                       */
/* (That means for this slist the expansion check always has to occur first) */
/* dpp->point_index is initialized !                                         */
void cb_copy_init_p2sl(ppt dpp, void* vp, mesh_p_t nup) {
  struct arraydef* sl_p = (struct arraydef*) vp;

  if(sl_p->n==sl_p->max) expand_ar(sl_p, D_AR); 
  
  ((ppt*)sl_p->vp)[sl_p->n] = dpp;
  
  dpp->point_index = sl_p->n;

  sl_p->n++;

}




/*****************************************************************************/
/*                                                                           */
/*  cb_copy_init_ep2sl()                                                     */
/*                                                                           */
/*****************************************************************************/
/* copies a found and unconnected point from the octree into an array of     */
/* point-pointers:  (actually its pointer)                                   */
/* slist with initialized n, where n==max is allowed !                       */
/* (That means for this slist the expansion check always has to occur first) */
/* dpp->point_index is initialized !                                         */
void cb_copy_init_ep2sl(ppt dpp, void* vp, mesh_p_t nup) {
  struct arraydef* sl_p = (struct arraydef*) vp;

  if(dpp->ntri_lp!=null) return;

  if(sl_p->n==sl_p->max) expand_ar(sl_p, D_AR);

  ((ppt*)sl_p->vp)[sl_p->n] = dpp;

  dpp->point_index = sl_p->n;
  
  sl_p->n++;
  
}

/*****************************************************************************/
/*                                                                           */
/*  cb_copy_ep2sl()                                                          */
/*                                                                           */
/*****************************************************************************/
/* copies a found and unconnected point from the octree into an array of     */
/* point-pointers:  (actually its pointer)                                   */
/* slist with initialized n, where n==max is allowed !                       */
/* (That means for this slist the expansion check always has to occur first) */
void cb_copy_ep2sl(ppt dpp, void* vp, mesh_p_t nup) {
  struct arraydef* sl_p = (struct arraydef*) vp;

  if(dpp->ntri_lp!=null) return;

  if(sl_p->n==sl_p->max) expand_ar(sl_p, D_AR);

  ((ppt*)sl_p->vp)[sl_p->n] = dpp;

  sl_p->n++;
  
}


/*****************************************************************************/
/*                                                                           */
/*  cb_erase_point()                                                         */
/*                                                                           */
/*****************************************************************************/
/* must be executed from ext_del which updates the tree structure            */
/* returns 1 if point is deleted                                             */
/* returns 0 if not                                                          */
/* uses erase_point for freeing of the memory                                */
/* does not check whether or not the point is unconnected :                  */
/* generally, elements must be first removed from the data structure before  */
/* they can be erased                                                        */
int cb_erase_point(ppt dpp, void* vp, mesh_p_t nup) {

  msg_dinfo("Erasing Point x y z : ");
  msg_dinfo1(dfmt, dpp->x);
  msg_dinfo1(dfmt, dpp->y);
  msg_dinfo1(dfmt, dpp->z);
  msg_dinfo("\n");

  erase_point(dpp);

  return 1;

}




/*****************************************************************************/
/*                                                                           */
/*  cb_simple()                                                              */
/*                                                                           */
/*****************************************************************************/
/* minimum version of algorithm:                                             */
/* check if a point within the search region is inside of the best sphere    */
/* and on the correct side and update bestdpp, lambda, and M                 */
/* When the search is finished, the structure lambda will contain the point  */
/* with which to create the tetrahedron                                      */
/* The sphere test is important, because comparing lambda's directly can     */
/* lead to numerical problems when the point is close to the triangle plane  */
/* and outside the sphere !                                                  */
/* Needs triangle parameters and lambda parameters (all client data)         */
/* rlist.n must be 0 due to the here unnecessary rlist check in check_side() */
/*                                                                           */
/* Instead of skipping a point in cases with too small tet-volumes,          */
/* could check if point is unconnected and erase the point ?                 */
/*                                                                           */
void cb_simple(ppt dpp, void* client, mesh_p_t mesh_p) {
  struct client_datadef* cli = (struct client_datadef*)(client);
  struct vectordef vc;

  /* testing if on the correct side and within the sphere (R2! not R)        */
  if(!check_side(mesh_p, cli, dpp)) return;

  /* now R2 must be less or equal than the best R2                           */
  /* Therefore, the point must be better, being inside the best sphere       */
  /* reduce lambda and calculate real R2 (R2 which corresponds to the        */
  /* distance between the point and the new M, instead of the old M)         */

  /* calculate and update lambda and M in client data                        */
  if(!calc_l_M(mesh_p, dpp, &cli->tri, &(cli->lambda.M), &(cli->lambda.lam))) {
    /* The tetrahedron has a too small volume                                */
    /* skip this point without updating M, R2, and lambda                    */
    /* (M, lambda were not changed)                                          */
    return;
  }

  /* update R2 (with new M) and bestdpp                                      */
  /* better using P4 than any other                                          */
  /* R is not updated to spare sqrt()                                        */
  /* So, the available R is the radius of the corresponding search region,   */
  /* not the current best sphere during octree traversal                     */
  vc.x = dpp->x - cli->lambda.M.x;      
  vc.y = dpp->y - cli->lambda.M.y;  
  vc.z = dpp->z - cli->lambda.M.z;      

  cli->lambda.R2 = vc.x*vc.x + vc.y*vc.y + vc.z*vc.z;

  cli->lambda.bestdpp = dpp;  

  cli->counter++;

}



/*****************************************************************************/
/*                                                                           */
/*  cb_pass1()                                                               */
/*                                                                           */
/*****************************************************************************/
/* First pass yielding RLIST and setting lmin and lmax required for making   */
/* the PLIST                                                                 */
/* updates lam and bestdpp                                                   */
/*                                                                           */
/* Some additional checks are necessary to ensure a numerically independent  */
/* consistent rlist/plist. For the sliver extension to the algorithm it is   */
/* required that during special1() points with d>0 but with slivers are      */
/* guaranteed to be in the rlist. Also, the index 'opposite_ind' is needed.  */
/* (The crucial issue is in fact to have points in the rlist to avoid        */
/*  them in the plist or vice versa)                                         */
/*                                                                           */
/* USED EPSILONS:                                                            */
/*  N_EPS                                                                    */
/*  R_EPS                                                                    */
/*  L_BEPS                                                                   */
/*  pN_EPS: usually set to 0.0, limits height of tetraeders                  */
/*          value greater than 0.0 may cause holes !!                        */
/*                                                                           */
/*                                                                           */
/* Instead of skipping a point in cases with too small tet-volumes,          */
/* could check if point is unconnected and erase the point ?                 */
/*                                                                           */
void cb_pass1(ppt dpp, void* client, mesh_p_t mesh_p) {
  struct client_datadef* cli = (struct client_datadef*)(client);
  struct vectordef va;
  double d;
  double a;
  double lam;
  int i;

  /* FIRST: check whether point belongs to base triangle                     */
  if(cli->tri.tri_p->dppp[0]==dpp ||
     cli->tri.tri_p->dppp[1]==dpp ||
     cli->tri.tri_p->dppp[2]==dpp   ) return;

  /* Also: check whether point is already in rlist due to macro              */
  /* complete_rlist_check to avoid double points in the rlist.               */
  if(cli->lambda.rlist.n > 0) {
    if( ((ppt*)cli->lambda.rlist.vp)[0] == dpp ) {
      /* points are not encountered twice but this one has been already      */
      /* inserted before the octree traversal as the first point in the list */
      return;
    }
  }

  /* Calculate distance                                                      */
  /* safer to use dppp[0] instead of H which would be needed later anyhow    */
  va.x = dpp->x - cli->tri.tri_p->dppp[0]->x;
  va.y = dpp->y - cli->tri.tri_p->dppp[0]->y;
  va.z = dpp->z - cli->tri.tri_p->dppp[0]->z;
  d = (va.x*cli->tri.n.x + va.y*cli->tri.n.y + va.z*cli->tri.n.z);

  /* SECOND: look for points on negative side of the triangle                */
  if(d < -(mesh_p->N_EPS)) return;

  /* Radius vector                                                           */
  va.x = dpp->x - cli->tri.H.x;
  va.y = dpp->y - cli->tri.H.y;
  va.z = dpp->z - cli->tri.H.z;

  /* THIRD: look for points on base-circle (RLIST)                           */
  a = sqrt(va.x*va.x + va.y*va.y + va.z*va.z);
  if( ( a <= (cli->tri.lp1h + (mesh_p->R_EPS)) ) &&
      ( a >= (cli->tri.lp1h - (mesh_p->R_EPS)) ) && 
      ( d <= (mesh_p->N_EPS) ) ) { 

    /* Extension for slivers: Extract opposite_ind while making sure that    */
    /* the current point makes sense in the rlist.                           */
    i = inside_tri_test(&cli->tri, dpp);

    if(i>=7 && i<=9) {
      /* Most likely case where point is outside of an edge (one!)           */
      /* Thats how it is supposed to be. Sliver exists if needed to resolve. */
      /* Set opposite_ind to identify the diagonal edge and memorize d.      */
      cli->lambda.opposite_ind = i - 7;
      cli->lambda.opposite_d = d;

      /* add to rlist and return                                             */
      ((ppt*)cli->lambda.rlist.vp)[cli->lambda.rlist.n] = dpp;
      cli->lambda.rlist.n++;
      if(cli->lambda.rlist.n == cli->lambda.rlist.max) 
	expand_ar(&cli->lambda.rlist, D_AR);
      return; 
    }
    /* NOW, if i>=4 :                                                        */
    /* Point outside of a vertex (outside of two edges)                      */
    /* Point should not be in rlist, numerical nonsense.                     */
    /* Point might not even be considered in the plist.                      */
    /* Avoiding it in the plist by returning now might not be good though.   */
    /* (For large N_EPS it might make sense to consider it for the plist)    */
    /* OR, i<4 :                                                             */
    /* Point is inside the triangle or on an edge.                           */
    /* Point should not be in rlist, numerical nonsense.                     */
    /* Point will probably be inserted into the plist.                       */
  }     
  
  /* FOURTH: points only on positive side                                    */
  if(d <= (mesh_p->pN_EPS)) return;

  /* FIFTH: *need* to exclude points close to the plane and outside of the   */
  /* base-circle! (lambda could be infinitely negative)                      */
  /* vector M->P (M from calc_region!)                                       */
  va.x = dpp->x - cli->lambda.M.x;     
  va.y = dpp->y - cli->lambda.M.y;     
  va.z = dpp->z - cli->lambda.M.z; 
  if ((va.x*va.x + va.y*va.y + va.z*va.z) > cli->lambda.R2) {
    /* point outside sphere                                                  */
    return;
  }   

  /* NOW: all points need to be calculated to set lmin, lmax                 */
  /* count them in cli->counter                                              */
  if(!calc_l(mesh_p, dpp, &cli->tri, &lam)) {
    /* The tetrahedron has a too small volume                                */
    /* skip this point                                                       */
    return;
  }

  if(lam > cli->lambda.lmax) return;
  if(lam < cli->lambda.lmin) { 

    cli->lambda.lmin = lam - (mesh_p->L_BEPS);
    cli->lambda.lmax = lam + (mesh_p->L_BEPS);
    cli->lambda.bestdpp = dpp;
    cli->lambda.lam = lam;

    /* 0 only if non found !                                                 */
    cli->lambda.n = 1; 

    return;

  }

  /* lambda is within the limits of lmin, lmax :                             */
  if(cli->lambda.lmin > lam - (mesh_p->L_BEPS)) {
    cli->lambda.lmin = lam - (mesh_p->L_BEPS);
    cli->lambda.lam = lam;
    cli->lambda.bestdpp = dpp;
  }
  if( cli->lambda.lmax < (lam + (mesh_p->L_BEPS)) ) 
    cli->lambda.lmax = lam + (mesh_p->L_BEPS);

  /* count how many points exist in this relative epsilon region             */
  cli->lambda.n++;
 
}



/*****************************************************************************/
/*                                                                           */
/*  cb_pass2()                                                               */
/*                                                                           */
/*****************************************************************************/
/* Second pass yielding plist requiring initialized lmin and lmax            */
/* Count number of points which are too close to lmax                        */
/* They would increase lmax making a 3rd pass necessary.                     */
/* Thus if 0 such points were counted, the plist contains all points !       */
/*                                                                           */
/* USED EPSILONS:							     */
/*  L_BEPS								     */
/*  DBL_EPSILON								     */
/*  pN_EPS: usually set to 0.0, limits height of tetraeders		     */
/*          value greater than 0.0 may cause holes !!			     */
/*									     */
/*                                                                           */
/* Instead of skipping a point in cases with too small tet-volumes,          */
/* could check if point is unconnected and erase the point ?                 */
/*                                                                           */
void cb_pass2(ppt dpp, void* client, mesh_p_t mesh_p) {
  struct client_datadef* cli = (struct client_datadef*)(client);
  double lam;

  /* check whether point is already found point bestdpp                      */
  if(cli->lambda.bestdpp == dpp) return;

  /* testing if on the correct side, not in rlist, and within the sphere     */
  if(!check_side(mesh_p, cli, dpp)) return;

  /* NOW: all points need to be calculated                                   */
  /* count only those where lmax is not valid                                */
  if(!calc_l(mesh_p, dpp, &cli->tri, &lam)) {
    /* The tetrahedron has a too small volume                                */
    /* skip this point                                                       */
    return;
  }
  
  if(lam >= cli->lambda.lmax) return; /* strictly greater *not* equal??      */
  if( (lam + (mesh_p->L_BEPS) - DBL_EPSILON) > cli->lambda.lmax) {
    /* add to plist and count                                                */
    cli->counter++;
    ((ppt*)cli->lambda.plist.vp)[cli->lambda.plist.n] = dpp;
    cli->lambda.plist.n++;
    if(cli->lambda.plist.n == cli->lambda.plist.max) 
      expand_ar(&cli->lambda.plist, D_AR);
    return; 
  }

  /* possible Test: check if lam is really not better than bestdpp           */
  /* if(lam < cli->lambda.lam) {                                             */
  /*   msg_dinfo("found better point in 2nd pass: ");                        */
  /*   msg_dinfo1(dfmt, lam - cli->lambda.lam);                              */
  /*   msg_dinfo("\n");                                                      */
  /* }                                                                       */
  /* can be added to plist without changing bestdpp                          */


  /* now lam is correctly within the limits, add to plist and don't count    */
  /* (and not bestdpp !!!)                                                   */
  ((ppt*)cli->lambda.plist.vp)[cli->lambda.plist.n] = dpp;
  cli->lambda.plist.n++;
  if(cli->lambda.plist.n == cli->lambda.plist.max) 
    expand_ar(&cli->lambda.plist, D_AR);


}





/*****************************************************************************/
/*                                                                           */
/*  cb_smsph()                                                               */
/*                                                                           */
/*****************************************************************************/
/* Checks the smallest sphere criterion for a triangle and generates slists  */
/* for the various disturbing point types.                                   */
/* Needs the triangle parameters and initialized bestdpp                     */
/* Only makes sense if the search region is derived from the smallest sphere */
/*                                                                           */
/* USED EPSILONS:                                                            */
/*  WSBREF_EPS  could be negative or positive for strict sense bref          */
/*  N_EPS                                                                    */
/*                                                                           */
/* Do NOT use a parameter instead of N_EPS because it affects whether or not */
/* a 2ndsphere test is considered to be numerically sensible!                */
/*                                                                           */
/* USED PARAMETERS:                                                          */
/*  SDEL_PAR                                                                 */
/*                                                                           */
/* The sign of Blam is not corrected, thus before comparing it to Flam it    */
/* has to be reversed                                                        */
/*                                                                           */
/* Results:                                                                  */
/*  points in cneps_list or uneps_list  -> NOT DELAUNAY                      */
/*  no points in neps_lists and only 1 bestdpp -> MAYBE DELAUNAY (2nd sphere)*/
/*  2 bestdpps -> NOT DELAUNAY                                               */
/*                                                                           */
/*  Independently, there are the flip_list holding points with which a flip  */
/*  might make sense, and the dpar_list of points that could be deleted.     */
/*                                                                           */
/* Need to make sure that no points of the dpar_list are also in the         */
/* uneps_list, otherwise the deletion of the dpar_list would be difficult.   */
/* Therefore, SDEL_PAR can only influence deletion of pts for SDEL_PAR>N_EPS */
/* For SDEL_PAR<N_EPS it only makes sense to *not* delete the points, thereby*/
/* interpreting SDEL_PAR as zero, otherwise the uneps_list would never have  */
/* to be generated.                                                          */
/*                                                                           */
/* points from the dpar_list should not right away be deleted, a flip or an  */
/* anyhow necessary refinement point might resolve the situation             */
/* A check whether or not a point is not only inside the circumcircle but    */
/* also inside the triangle can avoid the deletion of far away points.       */
/* However, such a check should not occur within this function and might not */
/* be necessary if the triangle is first refined or flipped.                 */
/*                                                                           */
void cb_smsph(ppt dpp, void* vp, mesh_p_t mesh_p) {
  struct surf_clientdef* cli = (struct surf_clientdef*) vp;
  struct vectordef va;
  double R;
  double d;
  
  /* FIRST: check whether point belongs to base triangle                     */
  if(cli->tri.tri_p->dppp[0]==dpp ||
     cli->tri.tri_p->dppp[1]==dpp ||
     cli->tri.tri_p->dppp[2]==dpp   ) return;
  
  /* SECOND: check whether point is inside of smallest sphere                */
  /* (do not use M !)                                                        */
  va.x = dpp->x - cli->tri.H.x;  
  va.y = dpp->y - cli->tri.H.y;
  va.z = dpp->z - cli->tri.H.z;
  R = sqrt(va.x*va.x + va.y*va.y + va.z*va.z); 

  /* NOT + -> - !!                                                           */
  if ( R > (cli->tri.lp1h - (mesh_p->WSBREF_EPS)) ) {
    /* point outside smallest sphere                                         */
    return;
  }  

  /* POINT IS INSIDE OF SMALLEST SPHERE                                      */

  cli->s1overall++;

  if(dpp->ntri_lp!=null) {
    /* connected point                                                       */
    /* add to flip_list                                                      */
    ((ppt*)cli->flip_list.vp)[cli->flip_list.n] = dpp;
    cli->flip_list.n++;
    if(cli->flip_list.n==cli->flip_list.max) expand_ar(&cli->flip_list, D_AR);
  }
  else {
    /* unconnected point                                                     */
    cli->s1unconn++;
  }

  /* THIRD: check normal distance                                            */
  va.x = dpp->x - cli->tri.tri_p->dppp[0]->x;
  va.y = dpp->y - cli->tri.tri_p->dppp[0]->y;
  va.z = dpp->z - cli->tri.tri_p->dppp[0]->z;
  d = (va.x*cli->tri.n.x + va.y*cli->tri.n.y + va.z*cli->tri.n.z);
  
  /* FOURTH: check if too close to the surface                               */
  /* important to include the bounds in the case where N_EPS == zero !       */
  if(d >= -(mesh_p->N_EPS) && d <= (mesh_p->N_EPS)) { 
    if(dpp->ntri_lp!=null) {
      /* connected point                                                     */
      /* add to cneps_list                                                   */
      ((ppt*)cli->cneps_list.vp)[cli->cneps_list.n] = dpp;
      cli->cneps_list.n++;
      if(cli->cneps_list.n==cli->cneps_list.max) 
	expand_ar(&cli->cneps_list, D_AR);
    }
    else {
      /* unconnected point                                                   */
      /* add to uneps_list                                                   */
      ((ppt*)cli->uneps_list.vp)[cli->uneps_list.n] = dpp;
      cli->uneps_list.n++;
      if(cli->uneps_list.n==cli->uneps_list.max) 
	expand_ar(&cli->uneps_list, D_AR);
    }
    /* For points close to the surface no lambda will be calculated          */
    /* Also, such points are not inserted into the dpar_list                 */
    return;
  }

  /* FIFTH: check if point could be deleted (dpar_list)                      */
  /* If SDEL_PAR<N_EPS then dpar_list is always empty and the deletion of    */
  /* unconnected points from the uneps_list depends on other criteria.       */
  if(d>-(mesh_p->SDEL_PAR) && d<(mesh_p->SDEL_PAR) && dpp->ntri_lp==null) { 
    /* add unconnected point to dpar_list                                    */
    ((ppt*)cli->dpar_list.vp)[cli->dpar_list.n] = dpp;
    cli->dpar_list.n++;
    if(cli->dpar_list.n==cli->dpar_list.max) expand_ar(&cli->dpar_list, D_AR);
  }

  /* SIXTH: determine for each side the 'best' point                         */
  if(d > 0.0) {
    /* frontside                                                             */
    if(cli->Fbestdpp!=null) {
      va.x = dpp->x - cli->F_M.x;      
      va.y = dpp->y - cli->F_M.y;  
      va.z = dpp->z - cli->F_M.z;      

      if((va.x*va.x+va.y*va.y+va.z*va.z) >= cli->F_R2) {
	/* point is worse or equal                                           */
	return;
      }
    }
    /* point is 'better' than last point or is first point                   */
    if( !calc_l_M(mesh_p, dpp, &cli->tri, &(cli->F_M), &(cli->Flam)) ) {
      /* The tetrahedron has a too small volume                              */
      /* This case should never happen! It was the purpose of the neps_list  */
      /* to avoid such a case ! Therefore, exit!                             */
      /* (may not update bestdpp without updating M, R2, and lam !!)         */
      msg_fatal("+FATAL+: VOL_EPS and N_EPS are not compatible\n");
      msg_fatal("During examination of the Delaunay property of a triangle ");
      msg_fatal("a point was encountered which is not close to the surface ");
      msg_fatal("and yet no lambda value could be calculated !\n");
      exit(0);
    }
    /* update R2 (with new M) and bestdpp                                    */
    va.x = dpp->x - cli->F_M.x;      
    va.y = dpp->y - cli->F_M.y;  
    va.z = dpp->z - cli->F_M.z;      
    
    cli->F_R2 = va.x*va.x + va.y*va.y + va.z*va.z;
    
    cli->Fbestdpp = dpp;
    cli->Fbestd = d;

  }
  else {
    /* backside                                                              */
    if(cli->Bbestdpp!=null) {
      va.x = dpp->x - cli->B_M.x;      
      va.y = dpp->y - cli->B_M.y;  
      va.z = dpp->z - cli->B_M.z;      
      
      if((va.x*va.x+va.y*va.y+va.z*va.z) >= cli->B_R2) {
	/* point is worse or equal                                           */
	return;
      }
    }
    /* point is 'better' than last point or is first point                   */
    if( !calc_l_M(mesh_p, dpp, &cli->tri, &(cli->B_M), &(cli->Blam)) ) {
      /* The tetrahedron has a too small volume                              */
      /* This case should never happen! It was the purpose of the neps_list  */
      /* to avoid such a case ! Therefore, exit!                             */
      /* (may not update bestdpp without updating M, R2, and lam !!)         */
      msg_fatal("+FATAL+: VOL_EPS and N_EPS are not compatible\n");
      msg_fatal("During examination of the Delaunay property of a triangle ");
      msg_fatal("a point was encountered which is not close to the surface ");
      msg_fatal("and yet no lambda value could be calculated !\n");
      exit(0);
    }
    /* update R2 (with new M) and bestdpp                                    */
    va.x = dpp->x - cli->B_M.x;      
    va.y = dpp->y - cli->B_M.y;  
    va.z = dpp->z - cli->B_M.z;      
    
    cli->B_R2 = va.x*va.x + va.y*va.y + va.z*va.z;
    
    cli->Bbestdpp = dpp;
    /* for backside correct d to be positive distance! needed in surface_refinement()! */
    cli->Bbestd = -d;

  }

  /* all lists and counters updated, no more checks                          */

} 


/*****************************************************************************/
/*                                                                           */
/*  cb_2ndsph()                                                              */
/*                                                                           */
/*****************************************************************************/
/* Check the second sphere criterion for a triangle                          */
/* Excluding points on that side of the single bestdpp                       */
/* (This is necessary because of the use of WSBREF_EPS for lambda)           */
/* Only makes sense if no points are close to the surface and the bestdpp    */
/* really has the best lambda for that side and no bestdpp exists for the    */
/* other side.                                                               */
/*                                                                           */
/* Requires initialized triangle parameters, M, R2, bestdpp, and             */
/* counter s2overall.                                                        */
/* (need bestdpp to determine which side !)                                  */
/* Will not change value of M and R2                                         */
/*                                                                           */
/* Results:                                                                  */
/*  Depending on the search region (WSBREF_EPS) the triangle fulfills the    */
/*  Delaunay criterion in a strict or wide sense if no points are found.     */
/*  If a point is found, it will have a worse lambda, because it was not     */
/*  inside of the first sphere (smallest)                                    */
/*                                                                           */
/*  Counts the number of disturbing points (of *one* side) in s2overall      */
/*  Does not generate a list of disturbing points (not the smallest sphere)  */
/*                                                                           */
/* PERFORMANCE issues: For a bestdpp close to the surface and yet not in the */
/*  neps_list the 2nd sphere might become extremely large. If no points are  */
/*  found the performance is okay. If many points are found, and no info is  */
/*  required about them it is much faster to abort the octree traversal      */
/*  as soon as one disturbing point is encountered ! Otherwise the effort    */
/*  becomes O(n^2)                                                           */
/*                                                                           */
void cb_2ndsph(ppt dpp, void* vp, mesh_p_t nup) {
  struct surf_clientdef* cli = (struct surf_clientdef*) vp;
  struct vectordef va;
  double d;

  /* FIRST: check whether point belongs to base triangle                     */
  if(cli->tri.tri_p->dppp[0]==dpp ||
     cli->tri.tri_p->dppp[1]==dpp ||
     cli->tri.tri_p->dppp[2]==dpp   ) return;

  /* DETERMINE which side to check                                           */
  if(cli->Bbestdpp != null) {
    /* FRONTSIDE                                                             */

    /* SECOND: check if the found point is bestdpp                           */
    if(cli->Bbestdpp == dpp) return;

    /* THIRD: check frontside                                                */
    va.x = dpp->x - cli->tri.tri_p->dppp[0]->x;
    va.y = dpp->y - cli->tri.tri_p->dppp[0]->y;
    va.z = dpp->z - cli->tri.tri_p->dppp[0]->z;
    d = (va.x*cli->tri.n.x + va.y*cli->tri.n.y + va.z*cli->tri.n.z);
    if(d < 0.0) return;

    /* FOURTH: check whether or not point is inside of the 2nd sphere        */
    va.x = dpp->x - cli->B_M.x;     
    va.y = dpp->y - cli->B_M.y;     
    va.z = dpp->z - cli->B_M.z; 
    if ((va.x*va.x + va.y*va.y + va.z*va.z) > cli->B_R2) {
      /* point outside sphere                                                */
      return;
    }   

  }
  else {
    /* BACKSIDE                                                              */

    /* SECOND: check if the found point is bestdpp                           */
    if(cli->Fbestdpp == dpp) return;

    /* THIRD: check backside                                                 */
    va.x = dpp->x - cli->tri.tri_p->dppp[0]->x;
    va.y = dpp->y - cli->tri.tri_p->dppp[0]->y;
    va.z = dpp->z - cli->tri.tri_p->dppp[0]->z;
    d = (va.x*cli->tri.n.x + va.y*cli->tri.n.y + va.z*cli->tri.n.z);
    if(d > 0.0) return;

    /* FOURTH: check whether or not point is inside of the 2nd sphere        */
    va.x = dpp->x - cli->F_M.x;     
    va.y = dpp->y - cli->F_M.y;     
    va.z = dpp->z - cli->F_M.z; 
    if ((va.x*va.x + va.y*va.y + va.z*va.z) > cli->F_R2) {
      /* point outside sphere                                                */
      return;
    }   

  }


  /* point is disturbing                                                     */
  cli->s2overall++;

}



/*****************************************************************************/
/*                                                                           */
/*  cb_smsph_edge()                                                          */
/*                                                                           */
/*****************************************************************************/
/* Checks the smallest sphere criterion for an edge                          */
/* Needs initialized vM, R, dpp1, dpp2, ve, inv2R, founddpp (==null !!), and */
/* counter !                                                                 */
/*                                                                           */
/* USED PARAMETERS:                                                          */
/*  SDEL_PAR                                                                 */
/*                                                                           */
/* Results:                                                                  */
/*   determines closest disturbing point (founddpp with distance d)          */
/*   deletes unconnected points which are too close (SDEL_PAR)               */
/*                                                                           */
int cb_smsph_edge(ppt dpp, void* vp, mesh_p_t mesh_p) {
  struct edge_clientdef* cli = (struct edge_clientdef*) vp;
  struct vectordef va;
  struct vectordef vn;
  double d;
  
  /* FIRST: check whether point belongs to the edge                          */
  if(cli->dpp1==dpp || cli->dpp2==dpp) return 0;
  
  /* SECOND: check whether point is inside of smallest sphere                */
  va.x = dpp->x - cli->vM.x;  
  va.y = dpp->y - cli->vM.y;
  va.z = dpp->z - cli->vM.z;

  if ( sqrt(va.x*va.x + va.y*va.y + va.z*va.z) >= cli->R) {
    /* point outside smallest sphere                                         */
    return 0;
  }  

  /* POINT IS INSIDE OF SMALLEST SPHERE                                      */

  /* THIRD: check normal distance                                            */
  va.x = dpp->x - cli->dpp1->x;
  va.y = dpp->y - cli->dpp1->y;
  va.z = dpp->z - cli->dpp1->z;
  /* va x ve                                                                 */
  vn.x = va.y*cli->ve.z - va.z*cli->ve.y;
  vn.y = va.z*cli->ve.x - va.x*cli->ve.z;
  vn.z = va.x*cli->ve.y - va.y*cli->ve.x;

  /* cannot be negative                                                      */
  d = cli->inv2R * sqrt(vn.x*vn.x + vn.y*vn.y + vn.z*vn.z);
  
  if(d < (mesh_p->SDEL_PAR) && dpp->ntri_lp==null) { 
    /* delete unconnected point                                              */
    msg_dinfo("Erasing Point x y z : ");
    msg_dinfo1(dfmt, dpp->x);
    msg_dinfo1(dfmt, dpp->y);
    msg_dinfo1(dfmt, dpp->z);
    msg_dinfo("\n");
    erase_point(dpp);
    return 1;
  }

  cli->counter++;

  if(cli->founddpp == null) {
    cli->d = d;
    cli->founddpp = dpp;
  }
  else if(d < cli->d) {
    cli->d = d;
    cli->founddpp = dpp;
  }

  return 0;

} 

  

/*****************************************************************************/
/*                                                                           */
/*  cb_relax()                                                               */
/*                                                                           */
/*****************************************************************************/
/* Calls for each found point the function grid_dens2fi(X,Xi) which adds fi  */
/* and fi' to the rclient structure.                                         */
/* The point X to be relaxed is not allowed to be in the octree.             */
void cb_relax(ppt dppXi, void* vp, mesh_p_t mesh_p) {
  struct relax_clientdef* rcli = (struct relax_clientdef*) vp;

  rcli->counter++;
  //  grid_dens2fi(mesh_p, rcli->dppX, dppXi, &(rcli->vF), &(rcli->mA));
  
}




/*****************************************************************************/
/*                                                                           */
/*  cb_linesplit()                                                           */
/*                                                                           */
/*****************************************************************************/
/* Check whether or not a point lies on an edge given by dpp1, dpp2, ve, and */
/* the length in structure ls_client. If it does, add the point to a sorted  */
/* slist.                                                                    */
/* The sorting is done less optimal (having to shift part of the list)       */
/* The point-edge intersection test could be improved with better numerics ? */
/*                                                                           */
/* USED EPSILONS:                                                            */
/*  LS_EPS  The minimal normal distance of a point to the line so that the   */
/*          line is not split                                                */
/* In this sense it is an absolute epsilon instead of a normalized one       */
/* Divide the measure by the length of vector a to get the sine of the angle */
/* This can be compared to a size-independent epsilon.                       */
/* The cosine is already known, why compute the sine ? better numerics       */
/* because of the angle being close to 0 degrees                             */
/*                                                                           */
void cb_linesplit(ppt dpp, void* vp, mesh_p_t nup) {
  struct ls_clientdef* cl_p = (struct ls_clientdef*) vp;
  struct vectordef va;
  struct vectordef ex;
  double proj;
  double proj2;
  int i;
  int j;

  if(dpp==cl_p->dpp1 || dpp==cl_p->dpp2) return;

  /* first check if point is on the line                                     */
  va.x = dpp->x - cl_p->dpp1->x;
  va.y = dpp->y - cl_p->dpp1->y;
  va.z = dpp->z - cl_p->dpp1->z;
  proj = (va.x*cl_p->ve.x + va.y*cl_p->ve.y + va.z*cl_p->ve.z) / cl_p->l_edge;
  if(proj<0.0 || proj>cl_p->l_edge) {
    /* This is actually an error if the search region is limited to the      */
    /* minima and maxima of the given edge. Treat point as not intersecting. */
    msg_dinfo("Numerical problems during linesplit ? \n");
    return;
  }

  /* va x ve                                                                 */
  ex.x = (va.y*cl_p->ve.z - cl_p->ve.y*va.z);   
  ex.y = (va.z*cl_p->ve.x - cl_p->ve.z*va.x);
  ex.z = (va.x*cl_p->ve.y - cl_p->ve.x*va.y);
  if( sqrt(ex.x*ex.x + ex.y*ex.y + ex.z*ex.z)/cl_p->l_edge > LS_EPS ) 
    return;
  
  /* insert point into sorted slist, expanding the slist if necessary        */
  for(i=0 ; i < cl_p->ls_list.n ; i++) {
    va.x = ((ppt*)cl_p->ls_list.vp)[i]->x - cl_p->dpp1->x;
    va.y = ((ppt*)cl_p->ls_list.vp)[i]->y - cl_p->dpp1->y;
    va.z = ((ppt*)cl_p->ls_list.vp)[i]->z - cl_p->dpp1->z;
    proj2 = (va.x*cl_p->ve.x + va.y*cl_p->ve.y + va.z*cl_p->ve.z) / 
            cl_p->l_edge;
    if(proj2==proj) {
      msg_fatal("+FATAL+: Two points too close during linesplit\n");
      exit(0);
    }
    if(proj2>proj) break;
  }
  /* point belongs inbetween i-1 and i                                       */
  for(j=cl_p->ls_list.n ; j>i ; j--) { 
    /* move up rest of list                                                  */
    ((ppt*)cl_p->ls_list.vp)[j] = ((ppt*)cl_p->ls_list.vp)[j-1];
  }
  ((ppt*)cl_p->ls_list.vp)[i] = dpp;
  cl_p->ls_list.n++;
  if(cl_p->ls_list.n==cl_p->ls_list.max) expand_ar(&cl_p->ls_list, 10);

  return;

}



