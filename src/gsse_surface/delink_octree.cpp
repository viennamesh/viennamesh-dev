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
//
#include "delink.hpp"

/** macros to compute integer-coordinates from double-coordinates ************/
/* cast is important for rounding,  Floor(double-coordinate) is necessary ?  */
#define i_coord_x(O, I) ( (long int) ( ((I)-((O)->bbminimum.x)) /             \
				       (O)->step_size_x           ) )
#define i_coord_y(O, I) ( (long int) ( ((I)-((O)->bbminimum.y)) /             \
				       (O)->step_size_y           ) )
#define i_coord_z(O, I) ( (long int) ( ((I)-((O)->bbminimum.z)) /             \
				       (O)->step_size_z           ) )


/** macro exclusively used in function o_delpoints() *************************/
/* Examines if an octree node has to be shifted up within the octree tree-   */
/* hierarchy after a point deletion                                          */
/* sets flag to either -1 or index                                           */
#define check_tree_state {                                                    \
   flag2 = 0;                                                                 \
   flag  = 0;                                                                 \
   for(i=0 ; i<8 ; i++) {                                                     \
     if( nodep->tag&(1<<i) ) {                                                \
       flag = i;                                                              \
       flag2++;                                                               \
     }                                                                        \
     else {                                                                   \
       if(nodep->u[i].child!=null) break;                                     \
     }                                                                        \
   }                                                                          \
   if(i<8) flag = -1;                                                         \
   else {                                                                     \
     if(flag2 > 1) flag = -1;                                                 \
   }                                                                          \
} 				   
 

/** Static variables *********************************************************/
static ppt o_dppmin, o_dppmax;
static void* o_client;
static void (*o_callbackf) (ppt, void*, mesh_p_t);
static  int (*o_callbackd) (ppt, void*, mesh_p_t);
static mesh_p_t o_anon_p; 
static struct octree_def* o_octreep; 

static struct nodedef* o_newnodep; 
static ppt o_dpp;
static int o_index2;
/* o_k can only be used in o_findpoints, not in o_delpoints                  */
static int o_k;  


/** Internal functions *******************************************************/
static int  o_delpoints (ippt, ippt, int, struct nodedef*);
static void o_findpoints(ippt, ippt, int, struct nodedef*);



/*****************************************************************************/
/*                                                                           */
/*  oct_insert_point()                                                       */
/*                                                                           */
/*****************************************************************************/
/* can be used for any octree, not just the global octree                    */
/* The octree_counter is updated !                                           */
/*                                                                           */
/* The POINT must be inside of the bounding box. It is not checked !         */
/* (coordinates can be equal to coord_min's ,  but have to be less than      */
/* coord_max's !).  Always checking would sometimes be redundant. Also, the  */
/* action when a point is out of the bounding box depends on the calling     */
/* function.                                                                 */
/*                                                                           */
/* The address of a found point is returned, if insertion is impossible.     */
/*                                                                           */
ppt oct_insert_point(ppt dpp, struct octree_def* octreep) {
  struct nodedef* newnodep;
  struct nodedef* nodep;
  int index, index2, depth;
  ppt newdpp;
  long int kpower;
  int i; 
  struct ipointtype ip, newip;

  /* limits are not checked                                                  */
  ip.x = i_coord_x(octreep, dpp->x); 
  ip.y = i_coord_y(octreep, dpp->y);
  ip.z = i_coord_z(octreep, dpp->z);

  nodep = &octreep->topnode;
  newnodep = nodep;
  depth = 0;
   
  do {
    nodep = newnodep;
    depth++;
    kpower = 1l<< (octreep->maxdepth - depth);
    index = (ip.x&kpower) / kpower + 
            ((ip.y&kpower) / kpower) * 2 + 
	    ((ip.z&kpower) / kpower) * 4 ;

    /* may contain pointadr !                                                */
    newnodep = nodep->u[index].child;   

  } while( ((nodep->tag&(1<<index))==0) && (newnodep!=null) );

  /* if empty insert point, no new node                                      */
  if(newnodep==null) {  
    /* implies that the tag is 0                                             */
    nodep->u[index].pointadr = dpp;
    nodep->tag |= 1<<index;
    /* update octree point counter                                           */
    octreep->octree_counter++;
    return null;
  }                      

  /* in fact, now newnodep must contain a point                              */
  newdpp = nodep->u[index].pointadr;
  newip.x = i_coord_x(octreep, newdpp->x); 
  newip.y = i_coord_y(octreep, newdpp->y);
  newip.z = i_coord_z(octreep, newdpp->z);

  /* do point comparison !!                                                  */
  if(newip.x==ip.x &&
     newip.y==ip.y &&
     newip.z==ip.z ) return newdpp; 

  /* now it is clear that the maximal depth of the octree is sufficient to   */
  /* store both points.                                                      */
  /* update octree point counter                                             */
  octreep->octree_counter++;
  
  /* toggle tag, becomes 0, other bits not changed                           */
  nodep->tag ^= 1<<index;   
  /* depth and index have well defined values here                           */
  do {

    /* Create and initialize new node                                        */
    newnodep = (struct nodedef*) my_malloc(sizeof(struct nodedef));
    nodep->u[index].child = newnodep;
    for(i=0 ; i<8 ; newnodep->u[i++].child = null);   
    newnodep->tag = 0;
    /* newnodep->mother = nodep;                                             */
    /* newnodep->n_depth = depth;                                            */
    
    depth++;
    /* will check maxdepth to safely avoid endless loop in the case of       */
    /* points outside of the bounding box due to inexact numerics            */
    if(depth > octreep->maxdepth) {
      msg_fatal("+FATAL+: Points not within the bounding box have sneaked "); 
      msg_fatal("into the octree\n");
      exit(0);
    }
    kpower = 1l<< (octreep->maxdepth - depth);
    index = (ip.x&kpower) / kpower + 
            ((ip.y&kpower) / kpower) * 2 + 
	    ((ip.z&kpower) / kpower) * 4 ;
    index2 = (newip.x&kpower) / kpower + 
             ((newip.y&kpower) / kpower) * 2 + 
	     ((newip.z&kpower) / kpower) * 4 ;
    
    nodep = newnodep;

  } while(index==index2); 
  
  nodep->u[index].pointadr = dpp;
  /* toggle tag, becomes 1, other bits not changed                           */
  nodep->tag ^= 1<<index;
  
  nodep->u[index2].pointadr = newdpp;
  /* toggle tag, becomes 1, other bits not changed                           */
  nodep->tag ^= 1<<index2;

  return(null);  
  
}



/*****************************************************************************/
/*                                                                           */
/*  oct_exchange_point()                                                     */
/*                                                                           */
/*****************************************************************************/
/* can be used for any octree, not just the global octree                    */
/* The octree_counter is updated !                                           */
/*                                                                           */
/* The POINT must be inside of the bounding box. It is not checked !         */
/*                                                                           */
/* Same as oct_insert_point() except that the point will be inserted even    */
/* when another point is found. They will be exchanged.                      */
/*                                                                           */
/* Returns address of point that has been taken out.                         */
/*                                                                           */
/* Will not check whether removed point belongs to any triangle !            */
/*                                                                           */
ppt oct_exchange_point(ppt dpp, struct octree_def* octreep) {
  struct nodedef* newnodep;
  struct nodedef* nodep;
  int index, index2, depth;
  ppt newdpp;
  long int kpower;
  int i; 
  struct ipointtype ip, newip;
  
  /* limits are not checked                                                  */
  ip.x = i_coord_x(octreep, dpp->x); 
  ip.y = i_coord_y(octreep, dpp->y);
  ip.z = i_coord_z(octreep, dpp->z);
  
  nodep = &octreep->topnode;
  newnodep = nodep;
  depth = 0;
   
  do {
    nodep = newnodep;
    depth++;
    kpower = 1l<< (octreep->maxdepth - depth);
    index = (ip.x&kpower) / kpower + 
            ((ip.y&kpower) / kpower) * 2 + 
	    ((ip.z&kpower) / kpower) * 4 ;

    /* may contain pointadr !                                                */
    newnodep = nodep->u[index].child;   

  } while( ((nodep->tag&(1<<index))==0) && (newnodep!=null) );

  /* if empty insert point, no new node                                      */
  if(newnodep==null) {  
    /* implies that the tag is 0                                             */
    nodep->u[index].pointadr = dpp;
    nodep->tag |= 1<<index;
    /* update octree point counter                                           */
    octreep->octree_counter++;
    return null;
  }                      

  /* in fact, now newnodep must contain a point                              */
  newdpp = nodep->u[index].pointadr;
  newip.x = i_coord_x(octreep, newdpp->x); 
  newip.y = i_coord_y(octreep, newdpp->y);
  newip.z = i_coord_z(octreep, newdpp->z);

  /* do point comparison !!                                                  */
  if(newip.x==ip.x &&
     newip.y==ip.y &&
     newip.z==ip.z ) {
    /* exchange points                                                       */
    nodep->u[index].pointadr = dpp;
    return newdpp;
  } 

  /* now it is clear that the maximal depth of the octree is sufficient to   */
  /* store both points.                                                      */
  /* update octree point counter                                             */
  octreep->octree_counter++;
  
  /* toggle tag, becomes 0, other bits not changed                           */
  nodep->tag ^= 1<<index;   
  /* depth and index have well defined values here                           */
  do {

    /* Create and initialize new node                                        */
    newnodep = (struct nodedef*) my_malloc(sizeof(struct nodedef));
    nodep->u[index].child = newnodep;
    for(i=0 ; i<8 ; newnodep->u[i++].child = null);   
    newnodep->tag = 0;
    /* newnodep->mother = nodep;                                             */
    /* newnodep->n_depth = depth;                                            */
    
    depth++;
    /* will check maxdepth to safely avoid endless loop in the case of       */
    /* points outside of the bounding box due to inexact numerics            */
    if(depth > octreep->maxdepth) {
      msg_fatal("+FATAL+: Points not within the bounding box have sneaked "); 
      msg_fatal("into the octree\n");
      exit(0);
    }
    kpower = 1l<< (octreep->maxdepth - depth);
    index = (ip.x&kpower) / kpower + 
            ((ip.y&kpower) / kpower) * 2 + 
	    ((ip.z&kpower) / kpower) * 4 ;
    index2 = (newip.x&kpower) / kpower + 
             ((newip.y&kpower) / kpower) * 2 + 
	     ((newip.z&kpower) / kpower) * 4 ;
    
    nodep = newnodep;

  } while(index==index2); 
  
  nodep->u[index].pointadr = dpp;
  /* toggle tag, becomes 1, other bits not changed                           */
  nodep->tag ^= 1<<index;
  
  nodep->u[index2].pointadr = newdpp;
  /* toggle tag, becomes 1, other bits not changed                           */
  nodep->tag ^= 1<<index2;

  return(null);  
  
}




/*****************************************************************************/
/*                                                                           */
/*  ext_find()                                                               */
/*                                                                           */
/*****************************************************************************/
/* can be used for any octree with different BBs, not just the global octree */
/*                                                                           */
/* Due to the static variables it is *not* possible to call ext_find()       */
/* from within a callback during an ext_find operation on the same or a      */
/* different octree. This can be achieved by keeping a BACKUP of the static  */
/* variables within the callback function ! (better to avoid static vars)    */
/*                                                                           */
/* The argument anon_p is not necessary for the octree functions. It will    */
/* not be used. Especially, only the octree pointer is used to reference the */
/* correct octree and its parameters.                                        */
/* The argument is only passed to the callback function as call data.        */
/* Could actually make it a void pointer. (It is such global information,    */
/* that it would not make sense to actually put this pointer into the client */
/* data, where it would logically belong. Hence the slightly incorrect name  */
/* 'call data')                                                              */
/*                                                                           */
void ext_find(ppt dppmi, 
	      ppt dppma,
	      void (*call)(ppt, void*, mesh_p_t),
	      void* cli,
	      struct octree_def* octreep,
	      mesh_p_t anon_p) {    

  struct ipointtype ipmin, ipmax;
  
  /* initialize static variables                                             */
  o_callbackf = call; 
  o_client = cli; 
  o_anon_p = anon_p;
  o_dppmin = dppmi;
  o_dppmax = dppma; 
  o_octreep = octreep;

  /** correct the given search region to fit inside the bounding box *********/
  /* however, it will not be checked if the given search minima are greater  */
  /* than the given maxima or if the search region is entirely outside the   */
  /* bounding box. Such cases are not bound to happen, because part of the   */
  /* search region always lies within the bounding box. If such a case       */
  /* occurs, it leads to wrong integer coordinates and thus unnecessary tree */
  /* traversals, but the overall result will still be correct due to the     */
  /* comparison with the double coordinates.                                 */
  if(o_dppmin->x < (octreep->bbminimum.x)) ipmin.x = 0; 
  else ipmin.x = i_coord_x(octreep, o_dppmin->x);
  
  if(o_dppmin->y < (octreep->bbminimum.y)) ipmin.y = 0; 
  else ipmin.y = i_coord_y(octreep, o_dppmin->y);
  
  if(o_dppmin->z < (octreep->bbminimum.z)) ipmin.z = 0; 
  else ipmin.z = i_coord_z(octreep, o_dppmin->z);

  if(o_dppmax->x >= (octreep->bbmaximum.x)) ipmax.x = octreep->maxnodes - 1;
  else ipmax.x = i_coord_x(octreep, o_dppmax->x);
  
  if(o_dppmax->y >= (octreep->bbmaximum.y)) ipmax.y = octreep->maxnodes - 1;  
  else ipmax.y = i_coord_y(octreep, o_dppmax->y);
  
  if(o_dppmax->z >= (octreep->bbmaximum.z)) ipmax.z = octreep->maxnodes - 1;
  else ipmax.z = i_coord_z(octreep, o_dppmax->z);
  
  o_findpoints(&ipmin, &ipmax, 0, &(octreep->topnode));  
  
}



/*****************************************************************************/
/*                                                                           */
/*  ext_del()                                                                */
/*                                                                           */
/*****************************************************************************/
/* can be used for any octree with different BBs, not just the global octree */
/*                                                                           */
/* Due to the static variables it is *not* possible to call ext_del()        */
/* from within a callback during an ext_del operation on the same or a       */
/* different octree. This can be achieved by keeping a BACKUP of the static  */
/* variables within the callback function ! (better to avoid static vars)    */
/*                                                                           */
/* The callback must return whether or not a point-entry should be deleted.  */
/* The octree_counter is updated right after each callback (good so that     */
/* in the next callback the octree counter is correct)                       */
/*                                                                           */
/* The argument anon_p is not necessary for the octree functions. It will    */
/* not be used. Especially, only the octree pointer is used to reference the */
/* correct octree and its parameters.                                        */
/* The argument is only passed to the callback function as call data.        */
/* Could actually make it a void pointer. (It is such global information,    */
/* that it would not make sense to actually put this pointer into the client */
/* data, where it would logically belong. Hence the slightly incorrect name  */
/* 'call data')                                                              */
/*                                                                           */
void ext_del(ppt dppmi, 
	     ppt dppma,
	     int (*call)(ppt, void*, mesh_p_t),
	     void* cli,
	     struct octree_def* octreep,
	     mesh_p_t anon_p) {    

  struct ipointtype ipmin, ipmax;
  
  /* initialize static variables                                             */
  o_callbackd = call; 
  o_client = cli; 
  o_anon_p = anon_p;
  o_dppmin = dppmi; 
  o_dppmax = dppma; 
  o_octreep = octreep;
  
  /** correct the given search region to fit inside the bounding box *********/
  /* however, it will not be checked if the given search minima are greater  */
  /* than the given maxima or if the search region is entirely outside the   */
  /* bounding box. Such cases are not bound to happen, because part of the   */
  /* search region always lies within the bounding box. If such a case       */
  /* occurs, it leads to wrong integer coordinates and thus unnecessary tree */
  /* traversals, but the overall result will still be correct due to the     */
  /* comparison with the double coordinates.                                 */
  if(o_dppmin->x < (octreep->bbminimum.x)) ipmin.x = 0; 
  else ipmin.x = i_coord_x(octreep, o_dppmin->x);

  if(o_dppmin->y < (octreep->bbminimum.y)) ipmin.y = 0; 
  else ipmin.y = i_coord_y(octreep, o_dppmin->y);

  if(o_dppmin->z < (octreep->bbminimum.z)) ipmin.z = 0; 
  else ipmin.z = i_coord_z(octreep, o_dppmin->z);

  if(o_dppmax->x >= (octreep->bbmaximum.x)) ipmax.x = octreep->maxnodes - 1;
  else ipmax.x = i_coord_x(octreep, o_dppmax->x);

  if(o_dppmax->y >= (octreep->bbmaximum.y)) ipmax.y = octreep->maxnodes - 1;  
  else ipmax.y = i_coord_y(octreep, o_dppmax->y);

  if(o_dppmax->z >= (octreep->bbmaximum.z)) ipmax.z = octreep->maxnodes - 1;
  else ipmax.z = i_coord_z(octreep, o_dppmax->z);

  /* only in the topnode is a single point allowed                           */
  /* so the first call to o_delpoints does not have to worry about shifting  */
  o_delpoints(&ipmin, &ipmax, 0, &(octreep->topnode)); 
  
}






/*****************************************************************************/
/*                                                                           */
/*  o_findpoints()                                                           */
/*                                                                           */
/*****************************************************************************/
/* requires correct initialized static variables o_dppmin, o_dppmax          */
/* dpmin, dpmax, ipmin, ipmax:                                               */
/* ---------------------------                                               */
/* (dp,ip are referenced by dpp,ipp)                                         */
/* (ipp is given as argument and dpp as external variable)                   */
/*  - Every coord. of dpmin, ipmin must be =< to corresponding               */
/*    coord. of dpmax, ipmax by definition  				     */
/*    If dpmin=dpmax, then point=dpmin=dpmax is searched for.		     */
/*  - dpmin, dpmax can be outside of the bounding-box !			     */
/*    ipmin, ipmax have to be within the limits for int.-coord.		     */
/*    (ipmin, ipmax do not necessarily correspond to dpmin, dpmax)	     */
/*    (because of recursion dp *and* ip necessary)			     */
/*  - either 2 additional arguments for o_dppmin, o_dppmax or		     */
/*    static variables. (Every recursion of function needs		     */
/*    the same dpmin,dpmax)						     */
/*                                                                           */
/* callback: gets from caller client data, finds object                      */
/*           calls with object, client data and additonal call data          */
/* independent of type of octree! (can be any octree, not just global octree)*/

static void o_findpoints(ippt ippmin, 
			 ippt ippmax, 
			 int depth,
			 struct nodedef* nodep) {
  
  int index1, i;
  long int kpower;
  struct ipointtype min, max; 

  
  o_newnodep = nodep;
  /* depth = nodep->n_depth;   then argument depth unnecessary               */

  do {
    nodep = o_newnodep;
    depth++;
    kpower = 1l<< (o_octreep->maxdepth - depth);
    index1 = (ippmin->x&kpower) / kpower + 
             ((ippmin->y&kpower) / kpower) * 2 + 
	     ((ippmin->z&kpower) / kpower) * 4 ;
    o_index2 = (ippmax->x&kpower) / kpower + 
               ((ippmax->y&kpower) / kpower) * 2 + 
               ((ippmax->z&kpower) / kpower) * 4 ;

    /* may contain pointadr !                                                */
    o_newnodep = nodep->u[index1].child;   

  } while( ( (nodep->tag&(1<<index1)) == 0 ) && 
	   ( o_newnodep != null )            && 
	   ( index1 == o_index2 ) );
  
  
  /** first guess: index1 == o_index2 ****************************************/
  /* just one node relevant                                                  */
  if(index1==o_index2) {
    
    if(o_newnodep==null) {
      /* empty node                                                          */
      return; 
    }
    
    /* node contains one point                                               */
    o_dpp = nodep->u[index1].pointadr; 
    if(o_dpp->x<o_dppmin->x || o_dpp->y<o_dppmin->y || o_dpp->z<o_dppmin->z ||
       o_dpp->x>o_dppmax->x || o_dpp->y>o_dppmax->y || o_dpp->z>o_dppmax->z) {
      /* point outside                                                       */
      return;                    
    }

    /* found single point. No further processing !!                          */
    (*o_callbackf)(o_dpp, o_client, o_anon_p);
    
    return;
  }                      
  

  /* index1 != o_index2: calculating affected subnodes (index1 < o_index2 !) */
  /* 1. Eight subnodes  (1 case)                                             */
  /* 2. Four  subnodes  (3 cases)                                            */
  /* 3. Two   subnodes  (3 cases)                                            */

  
  /** 1. Eight subnodes: 1 possible case *************************************/
  if(o_index2-index1==7) {
    for(i=0 ; i<8 ; i++) {
      /* subnode i                                                           */
      if(nodep->u[i].child!=null) {
	if((nodep->tag&(1<<i))==0) { 
	  /* subnode contains child, recursion                               */
	  min.x = (i&1)? ippmax->x &~(kpower-1) : ippmin->x;
	  min.y = (i&2)? ippmax->y &~(kpower-1) : ippmin->y;
	  min.z = (i&4)? ippmax->z &~(kpower-1) : ippmin->z;
	  max.x = (i&1)? ippmax->x : ippmin->x |(kpower-1);
	  max.y = (i&2)? ippmax->y : ippmin->y |(kpower-1);
	  max.z = (i&4)? ippmax->z : ippmin->z |(kpower-1);
	  o_findpoints(&min, &max, depth, nodep->u[i].child);
	}
	else {
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[i].pointadr;  			
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x &&
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) {
	    /* point is inside !                                             */
	    (*o_callbackf)(o_dpp, o_client, o_anon_p);
	  }           
	}
      } 	
    }
    /* all subnodes processed, finished                                      */
    return; 
  }



  /** 2. Four subnodes: 3 possible cases *************************************/
  
  /** case 1: in xz-plane ****************************************************/
  if(o_index2-index1==5) {  
    min.y = ippmin->y;
    max.y = ippmax->y;
    for(i=0 ; i<4 ; i++) {
      /* subnode k                                                           */
      o_k = index1 + (i&1) + (i&2)*2;  
      if(nodep->u[o_k].child!=null) {
	if((nodep->tag&(1<<o_k))==0) {  
	  /* subnode contains child, recursion                               */
	  min.x = (o_k&1)? ippmax->x &~(kpower-1) : ippmin->x;
	  min.z = (o_k&4)? ippmax->z &~(kpower-1) : ippmin->z;
	  max.x = (o_k&1)? ippmax->x : ippmin->x |(kpower-1);
	  max.z = (o_k&4)? ippmax->z : ippmin->z |(kpower-1);
	  o_findpoints(&min, &max, depth, nodep->u[o_k].child);
	}
	else {
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[o_k].pointadr;
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) { 
	    /* point is inside !                                             */
	    (*o_callbackf)(o_dpp, o_client, o_anon_p);
	  }   
	}
      } 	
    }
    /* all subnodes processed, finished                                      */
    return; 
  }

  
  /** case 2: in yz-plane ****************************************************/
  if(o_index2-index1==6) {  
    min.x = ippmin->x;
    max.x = ippmax->x;
    for(i=0 ; i<4 ; i++) {
      /* subnode k                                                           */
      o_k = index1 + i*2;  
      if(nodep->u[o_k].child!=null) {
	if((nodep->tag&(1<<o_k))==0) {  
	  /* subnode contains child, recursion                               */
	  min.y = (o_k&2)? ippmax->y &~(kpower-1) : ippmin->y;
	  min.z = (o_k&4)? ippmax->z &~(kpower-1) : ippmin->z;
	  max.y = (o_k&2)? ippmax->y : ippmin->y |(kpower-1);
	  max.z = (o_k&4)? ippmax->z : ippmin->z |(kpower-1);
	  o_findpoints(&min, &max, depth, nodep->u[o_k].child);
	}
	else {
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[o_k].pointadr;
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) {
	    /* point is inside !                                             */
	    (*o_callbackf)(o_dpp, o_client, o_anon_p);
	  }           
	}
      } 	
    }
    /* all subnodes processed, finished                                      */
    return; 
  }
  

  /** case 3: in xy-plane ****************************************************/
  if(o_index2-index1==3) {  
    min.z = ippmin->z;
    max.z = ippmax->z;
    for(i=0 ; i<4 ; i++) {
      /* subnode k                                                           */
      o_k = index1 + i;  
      if(nodep->u[o_k].child!=null) {
	if((nodep->tag&(1<<o_k))==0) {  
	  /* subnode contains child, recursion                               */
	  min.x = (o_k&1)? ippmax->x &~(kpower-1) : ippmin->x;
	  min.y = (o_k&2)? ippmax->y &~(kpower-1) : ippmin->y;
	  max.x = (o_k&1)? ippmax->x : ippmin->x |(kpower-1);
	  max.y = (o_k&2)? ippmax->y : ippmin->y |(kpower-1);
	  o_findpoints(&min, &max, depth, nodep->u[o_k].child);
	}
	else {
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[o_k].pointadr;
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) { 
	    /* point is inside !                                             */
	    (*o_callbackf)(o_dpp, o_client, o_anon_p);
	  }  
	}
      } 	
    }
    /* all subnodes processed, finished                                      */
    return;
  }
  


  /** 3. Two subnodes: 3 possible cases **************************************/

  /** case 1: in x-direction *************************************************/
  if(o_index2-index1==1) {  
    min.y = ippmin->y;
    min.z = ippmin->z;
    max.y = ippmax->y;
    max.z = ippmax->z;
    for(i=0 ; i<2 ; i++) {
      /* subnode k                                                           */
      o_k = index1 + i;  
      if(nodep->u[o_k].child!=null) {
	if((nodep->tag&(1<<o_k))==0) {  
	  /* subnode contains child, recursion                               */
	  min.x = (o_k&1)? ippmax->x &~(kpower-1) : ippmin->x;
	  max.x = (o_k&1)? ippmax->x : ippmin->x |(kpower-1);
	  o_findpoints(&min, &max, depth, nodep->u[o_k].child);
	}
	else {
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[o_k].pointadr;
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) { 
	    /* point is inside !                                             */
	    (*o_callbackf)(o_dpp, o_client, o_anon_p); 
	  } 
	}
      } 	
    }
    /* all subnodes processed, finished                                      */
    return; 
  }
  

  /** case 2: in y-direction *************************************************/
  if(o_index2-index1==2) { 
    min.x = ippmin->x;
    min.z = ippmin->z;
    max.x = ippmax->x;
    max.z = ippmax->z;
    for(i=0 ; i<2 ; i++) {
      /* subnode k                                                           */
      o_k = index1 + i*2;  
      if(nodep->u[o_k].child!=null) {
	if((nodep->tag&(1<<o_k))==0) {  
	  /* subnode contains child, recursion                               */
	  min.y = (o_k&2)? ippmax->y &~(kpower-1) : ippmin->y;
	  max.y = (o_k&2)? ippmax->y : ippmin->y |(kpower-1);
	  o_findpoints(&min, &max, depth, nodep->u[o_k].child);
	}
	else { 
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[o_k].pointadr;
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) { 
	    /* point is inside !                                             */
	    (*o_callbackf)(o_dpp, o_client, o_anon_p);
	  }  
	}
      } 	
    }
    /* all subnodes processed, finished                                      */
    return; 
  }
  
  
  /** case 3: in z-direction *************************************************/
  if(o_index2-index1==4) {  
    min.x = ippmin->x;
    min.y = ippmin->y;
    max.x = ippmax->x;
    max.y = ippmax->y;
    for(i=0 ; i<2 ; i++) {
      /* subnode k                                                           */
      o_k = index1 + i*4;  
      if(nodep->u[o_k].child!=null) {
	if((nodep->tag&(1<<o_k))==0) {  
	  /* subnode contains child, recursion                               */
	  min.z = (o_k&4)? ippmax->z &~(kpower-1) : ippmin->z; 
	  max.z = (o_k&4)? ippmax->z : ippmin->z |(kpower-1);
	  o_findpoints(&min, &max, depth, nodep->u[o_k].child);
	}
	else {
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[o_k].pointadr;
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) { 
	    /* point is inside !                                             */
	    (*o_callbackf)(o_dpp, o_client, o_anon_p);
	  }
	}
      } 	
    }
    /* all subnodes processed, finished                                      */
    return; 
  }
  
  

  /* all possible values for o_index2-index1 have been dealt with,           */
  /* hence, function must have returned by now !!                            */
}






/*****************************************************************************/
/*                                                                           */
/*  o_delpoints()                                                            */
/*                                                                           */
/*****************************************************************************/
/* requires correct initialized static variables o_dppmin, o_dppmax          */
/* dpmin, dpmax, ipmin, ipmax:                                               */
/* ---------------------------                                               */
/* (dp,ip are referenced by dpp,ipp)                                         */
/* (ipp is given as argument and dpp as external variable)                   */
/*  - Every coord. of dpmin, ipmin must be =< to corresponding               */
/*    coord. of dpmax, ipmax by definition  				     */
/*    If dpmin=dpmax, then point=dpmin=dpmax is searched for.		     */
/*  - dpmin, dpmax can be outside of the bounding-box !			     */
/*    ipmin, ipmax have to be within the limits for int.-coord.		     */
/*    (ipmin, ipmax do not necessarily correspond to dpmin, dpmax)	     */
/*    (because of recursion dp *and* ip necessary)			     */
/*  - either 2 additional arguments for o_dppmin, o_dppmax or		     */
/*    static variables. (Every recursion of function needs		     */
/*    the same dpmin,dpmax)						     */
/*                                                                           */
/* callback: gets from caller client data, finds object                      */
/*           calls with object, client data and additonal call data          */
/* callback function returns 1 if point-entry should be deleted              */
/* or it returns 0 if not                                                    */
/* callback frees allocated memory (erases) point itself !!                  */
/* (advantage: may actually not erase point, but only octree-entry !)        */
/*                                                                           */
/* independent of type of octree! (can be any octree, not just global octree)*/
/*                                                                           */
/* Because of the necessary tree-checks, the initial while-loop is not       */
/* possible. Uses macro check_tree_state                                     */

static int o_delpoints(ippt ippmin, 
		       ippt ippmax, 
		       int depth, 
		       struct nodedef* nodep) {

  int index1, i, flag, flag2, k;
  long int kpower;
  struct ipointtype min, max; 

  /* depth = nodep->n_depth;   then argument depth unnecessary               */
  depth++;

  kpower = 1l<< (o_octreep->maxdepth - depth);
  index1 = (ippmin->x&kpower) / kpower + 
           ((ippmin->y&kpower) / kpower) * 2 + 
	   ((ippmin->z&kpower) / kpower) * 4 ;
  o_index2 = (ippmax->x&kpower) / kpower + 
             ((ippmax->y&kpower) / kpower) * 2 + 
	     ((ippmax->z&kpower) / kpower) * 4 ;
  
  /* INITIALIZE flag for all cases                                           */
  flag2 = 1;	
  flag = 0; 
  

  /** first guess: index1 == o_index2 ****************************************/
  /* just one node relevant                                                  */
  if(o_index2==index1) { 
    /* ONE subnode                                                           */

    if(nodep->u[index1].child==null) return(-1);

    if((nodep->tag&(1<<index1))==0) {  
      /* subnode contains child, recursion                                   */
      if( (o_index2=o_delpoints(ippmin, ippmax, depth, nodep->u[index1].child))
	  == -1 ) return(-1);
      
      /* upmove -> check_tree_state                                          */
      if(nodep->u[index1].child->tag) { 
	/* move up point                                                     */
	o_dpp = nodep->u[index1].child->u[o_index2].pointadr;
	free(nodep->u[index1].child);
	nodep->u[index1].pointadr = o_dpp;
	nodep->tag |= 1<<index1;
      }
      else { 
	/* move up empty node                                                */
	free(nodep->u[index1].child);
	nodep->u[index1].child = null;
      }
      check_tree_state;
      return flag;
    }
    
    /* subnode contains point, callback                                      */
    o_dpp = nodep->u[index1].pointadr;
    if(o_dpp->x<o_dppmin->x || o_dpp->y<o_dppmin->y || o_dpp->z<o_dppmin->z ||
       o_dpp->x>o_dppmax->x || o_dpp->y>o_dppmax->y || o_dpp->z>o_dppmax->z) {
      /* point is outside !                                                  */
      return(-1);
    }                    
    /* point is inside !                                                     */
    if( ((*o_callbackd)(o_dpp, o_client, o_anon_p))==0 ) return(-1);
    o_octreep->octree_counter--;
    nodep->u[index1].child = null; /* Important: CHILD !                     */
    nodep->tag ^= (1<<index1); /* toggle tag, bit=0                          */
    
    /* set flag                                                              */
    check_tree_state;
    /* flag contains -1 or index                                             */
    return flag; 
  }


  /* index1 != o_index2: calculating affected subnodes (index1 < o_index2 !) */
  /* 1. Eight subnodes  (1 case)                                             */
  /* 2. Four  subnodes  (3 cases)                                            */
  /* 3. Two   subnodes  (3 cases)                                            */


  /** 1. Eight subnodes: 1 possible case *************************************/
  if(o_index2-index1==7) {
    for(i=0 ; i<8 ; i++) {
      /* subnode i                                                           */
      if(nodep->u[i].child!=null) {
	if((nodep->tag&(1<<i))==0) { 
	  /* subnode contains child, recursion                               */
	  min.x = (i&1)? ippmax->x &~(kpower-1) : ippmin->x;
	  min.y = (i&2)? ippmax->y &~(kpower-1) : ippmin->y;
	  min.z = (i&4)? ippmax->z &~(kpower-1) : ippmin->z;
	  max.x = (i&1)? ippmax->x : ippmin->x |(kpower-1);
	  max.y = (i&2)? ippmax->y : ippmin->y |(kpower-1);
	  max.z = (i&4)? ippmax->z : ippmin->z |(kpower-1);
	  if( (o_index2 = o_delpoints(&min, &max, depth, nodep->u[i].child))
	     !=-1 ) {
	    /* upmove -> check_tree_state                                    */
	    flag = 1; 
	    if(nodep->u[i].child->tag) { 
	      /* move up point                                               */
	      o_dpp = nodep->u[i].child->u[o_index2].pointadr;
	      free(nodep->u[i].child);
	      nodep->u[i].pointadr = o_dpp;
	      nodep->tag |= 1<<i;
	    }
	    else { 
	      /* move up empty node                                          */
	      free(nodep->u[i].child);
	      nodep->u[i].child = null;
	    }
	  }
	  else flag2 = 0; /* no check when at least one child                */
	}
	else {    
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[i].pointadr;  			
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) { 
	    /* point is inside !                                             */
	    if( (*o_callbackd)(o_dpp, o_client, o_anon_p) ) {
	      o_octreep->octree_counter--;
	      flag = 1;
	      nodep->u[i].child = null; /* Important: CHILD !                */
	      nodep->tag ^= (1<<i); /* toggle tag, bit=0                     */
	    }
	  }
	}
      } 	
    }
    if( (flag&flag2) ) { 
      /* changes ocurred and no child on the way                             */
      /* set flag                                                            */
      check_tree_state;
      /* flag contains -1 or index                                           */
      return flag; 
    }
    return(-1);
  }

  

  /** 2. Four subnodes: 3 possible cases *************************************/

  /** case 1: in xz-plane ****************************************************/
  if(o_index2-index1==5) {  
    min.y = ippmin->y;
    max.y = ippmax->y;
    for(i=0 ; i<4 ; i++) {
      /* subnode k                                                           */
      k = index1 + (i&1) + (i&2)*2;  
      if(nodep->u[k].child!=null) {
	if((nodep->tag&(1<<k))==0) { 
	  /* subnode contains child, recursion                               */
	  min.x = (k&1)? ippmax->x &~(kpower-1) : ippmin->x;
	  min.z = (k&4)? ippmax->z &~(kpower-1) : ippmin->z;
	  max.x = (k&1)? ippmax->x : ippmin->x |(kpower-1);
	  max.z = (k&4)? ippmax->z : ippmin->z |(kpower-1);
	  if( (o_index2 = o_delpoints(&min, &max, depth, nodep->u[k].child))
	      !=-1 ) {
	    /* upmove -> check_tree_state                                    */
	    flag = 1; 
	    if(nodep->u[k].child->tag) {
	      /* move up point                                               */
	      o_dpp = nodep->u[k].child->u[o_index2].pointadr;
	      free(nodep->u[k].child);
	      nodep->u[k].pointadr = o_dpp;
	      nodep->tag |= 1<<k;
	    }
	    else {
	      /* move up empty node                                          */
	      free(nodep->u[k].child);
	      nodep->u[k].child = null;
	    }
	  }
	  else flag2 = 0; /* no check when at least one child                */
	}
	else { 
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[k].pointadr;
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) { 
	    /* point is inside !                                             */
	    if( (*o_callbackd)(o_dpp, o_client, o_anon_p) ) {
	      o_octreep->octree_counter--;
	      flag = 1;
	      nodep->u[k].child = null; /* Important: CHILD !                */
	      nodep->tag ^= (1<<k); /* toggle tag, bit=0                     */
	    }
	  }
	}
      } 	
    }
    if( (flag&flag2) ) { 
      /* changes ocurred and no child on the way                             */
      /* set flag                                                            */
      check_tree_state;
      /* flag contains -1 or index                                           */
      return flag; 
    }
    return(-1);
  }
  

  /** case 2: in yz-plane ****************************************************/
  if(o_index2-index1==6) {  
    min.x = ippmin->x;
    max.x = ippmax->x;
    for(i=0 ; i<4 ; i++) {
      /* subnode k                                                           */
      k = index1 + i*2;  
      if(nodep->u[k].child!=null) {
	if((nodep->tag&(1<<k))==0) { 
	  /* subnode contains child, recursion                               */
	  min.y = (k&2)? ippmax->y &~(kpower-1) : ippmin->y;
	  min.z = (k&4)? ippmax->z &~(kpower-1) : ippmin->z;
	  max.y = (k&2)? ippmax->y : ippmin->y |(kpower-1);
	  max.z = (k&4)? ippmax->z : ippmin->z |(kpower-1);
	  if( (o_index2 = o_delpoints(&min, &max, depth, nodep->u[k].child))
	      !=-1 ) {
	    /* upmove -> check_tree_state                                    */
	    flag = 1; 
	    if(nodep->u[k].child->tag) { 
	      /* move up point                                               */
	      o_dpp = nodep->u[k].child->u[o_index2].pointadr;
	      free(nodep->u[k].child);
	      nodep->u[k].pointadr = o_dpp;
	      nodep->tag |= 1<<k;
	    }
	    else {
	      /* move up empty node                                          */
	      free(nodep->u[k].child);
	      nodep->u[k].child = null;
	    }
	  }
	  else flag2 = 0; /* no check when at least one child                */
	}
	else {  
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[k].pointadr;
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) { 
	    /* point is inside !                                             */
	    if( (*o_callbackd)(o_dpp, o_client, o_anon_p) ) {
	      o_octreep->octree_counter--;
	      flag = 1;
	      nodep->u[k].child = null; /* Important: CHILD !                */
	      nodep->tag ^= (1<<k); /* toggle tag, bit=0                     */
	    }
	  }
	}
      } 	
    }
    if( (flag&flag2) ) { 
      /* changes ocurred and no child on the way                             */
      /* set flag                                                            */
      check_tree_state;
      /* flag contains -1 or index                                           */
      return flag; 
    }
    return(-1);
  }
  

  /** case 3: in xy-plane ****************************************************/
  if(o_index2-index1==3) {  
    min.z = ippmin->z;
    max.z = ippmax->z;
    for(i=0 ; i<4 ; i++) {
      /* subnode k                                                           */
      k = index1 + i;  
      if(nodep->u[k].child!=null) {
	if((nodep->tag&(1<<k))==0) {
	  /* subnode contains child, recursion                               */
	  min.x = (k&1)? ippmax->x &~(kpower-1) : ippmin->x;
	  min.y = (k&2)? ippmax->y &~(kpower-1) : ippmin->y;
	  max.x = (k&1)? ippmax->x : ippmin->x |(kpower-1);
	  max.y = (k&2)? ippmax->y : ippmin->y |(kpower-1);
	  if( (o_index2 = o_delpoints(&min, &max, depth, nodep->u[k].child))
	      !=-1 ) {
	    /* upmove -> check_tree_state                                    */
	    flag = 1; 
	    if(nodep->u[k].child->tag) {
	      /* move up point                                               */
	      o_dpp = nodep->u[k].child->u[o_index2].pointadr;
	      free(nodep->u[k].child);
	      nodep->u[k].pointadr = o_dpp;
	      nodep->tag |= 1<<k;
	    }
	    else { 
	      /* move up empty node                                          */
	      free(nodep->u[k].child);
	      nodep->u[k].child = null;
	    }
	  }
	  else flag2 = 0; /* no check when at least one child                */
	}
	else {                   
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[k].pointadr;
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) { 
	    /* point is inside !                                             */
	    if( (*o_callbackd)(o_dpp, o_client, o_anon_p) ) {
	      o_octreep->octree_counter--;
	      flag = 1;
	      nodep->u[k].child = null; /* Important: CHILD !                */
	      nodep->tag ^= (1<<k); /* toggle tag, bit=0                     */
	    }
	  }
	}
      } 	
    }
    if( (flag&flag2) ) { 
      /* changes ocurred and no child on the way                             */
      /* set flag                                                            */
      check_tree_state;
      /* flag contains -1 or index                                           */
      return flag; 
    }
    return(-1);
  }
  


  /** 3. Two subnodes: 3 possible cases **************************************/

  /** case 1: in x-direction *************************************************/
  if(o_index2-index1==1) {  
    min.y = ippmin->y;
    min.z = ippmin->z;
    max.y = ippmax->y;
    max.z = ippmax->z;
    for(i=0 ; i<2 ; i++) {
      /* subnode k                                                           */
      k = index1 + i;  
      if(nodep->u[k].child!=null) {
	if((nodep->tag&(1<<k))==0) {
	  /* subnode contains child, recursion                               */
	  min.x = (k&1)? ippmax->x &~(kpower-1) : ippmin->x;
	  max.x = (k&1)? ippmax->x : ippmin->x |(kpower-1);
	  if( (o_index2 = o_delpoints(&min, &max, depth, nodep->u[k].child))
	      !=-1 ) {
	    /* upmove -> check_tree_state                                    */
	    flag = 1; 
	    if(nodep->u[k].child->tag) {
	      /* move up point                                               */
	      o_dpp = nodep->u[k].child->u[o_index2].pointadr;
	      free(nodep->u[k].child);
	      nodep->u[k].pointadr = o_dpp;
	      nodep->tag |= 1<<k;
	    }
	    else {
	      /* move up empty node                                          */
	      free(nodep->u[k].child);
	      nodep->u[k].child = null;
	    }
	  }
	  else flag2 = 0; /* no check when at least one child                */
	}
	else {  
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[k].pointadr;
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) { 
	    /* point is inside !                                             */
	    if( (*o_callbackd)(o_dpp, o_client, o_anon_p) ) {
	      o_octreep->octree_counter--;
	      flag = 1;
	      nodep->u[k].child = null; /* Important: CHILD !                */
	      nodep->tag ^= (1<<k); /* toggle tag, bit=0                     */
	    }
	  }
	}
      } 	
    }
    if( (flag&flag2) ) { 
      /* changes ocurred and no child on the way                             */
      /* set flag                                                            */
      check_tree_state; 
      /* flag contains -1 or index                                           */
      return flag; 
    }
    return(-1);
  }
  

  /** case 2: in y-direction *************************************************/
  if(o_index2-index1==2) {  
    min.x = ippmin->x;
    min.z = ippmin->z;
    max.x = ippmax->x;
    max.z = ippmax->z;
    for(i=0 ; i<2 ; i++) {
      /* subnode k                                                           */
      k = index1 + i*2;  
      if(nodep->u[k].child!=null) {
	if((nodep->tag&(1<<k))==0) {  
	  /* subnode contains child, recursion                               */
	  min.y = (k&2)? ippmax->y &~(kpower-1) : ippmin->y;
	  max.y = (k&2)? ippmax->y : ippmin->y |(kpower-1);
	  if( (o_index2 = o_delpoints(&min, &max, depth, nodep->u[k].child))
	     !=-1 ) {
	    /* upmove -> check_tree_state                                    */
	    flag = 1; 
	    if(nodep->u[k].child->tag) { 
	      /* move up point                                               */
	      o_dpp = nodep->u[k].child->u[o_index2].pointadr;
	      free(nodep->u[k].child);
	      nodep->u[k].pointadr = o_dpp;
	      nodep->tag |= 1<<k;
	    }
	    else {
	      /* move up empty node                                          */
	      free(nodep->u[k].child);
	      nodep->u[k].child = null;
	    }
	  }
	  else flag2 = 0; /* no check when at least one child                */
	}
	else { 
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[k].pointadr;
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) { 
	    /* point is inside !                                             */
	    if( (*o_callbackd)(o_dpp, o_client, o_anon_p) ) {
	      o_octreep->octree_counter--;
	      flag = 1;
	      nodep->u[k].child = null; /* Important: CHILD !                */
	      nodep->tag ^= (1<<k); /* toggle tag, bit=0                     */
	    }
	  }
	}
      } 	
    }
    if( (flag&flag2) ) { 
      /* changes ocurred and no child on the way                             */
      /* set flag                                                            */
      check_tree_state; 
      /* flag contains -1 or index                                           */
      return flag;
    }
    return(-1);
  }
  

  /** case 3: in z-direction *************************************************/
  if(o_index2-index1==4) {  
    min.x = ippmin->x;
    min.y = ippmin->y;
    max.x = ippmax->x;
    max.y = ippmax->y;
    for(i=0 ; i<2 ; i++) {
      /* subnode k                                                           */
      k = index1 + i*4;  
      if(nodep->u[k].child!=null) {
	if((nodep->tag&(1<<k))==0) {  
	  /* subnode contains child, recursion                               */
	  min.z = (k&4)? ippmax->z &~(kpower-1) : ippmin->z; 
	  max.z = (k&4)? ippmax->z : ippmin->z |(kpower-1);
	  if( (o_index2 = o_delpoints(&min, &max, depth, nodep->u[k].child))
	      !=-1 ) {
	    /* upmove -> check_tree_state                                    */
	    flag = 1; 
	    if(nodep->u[k].child->tag) { 
	      /* move up point                                               */
	      o_dpp = nodep->u[k].child->u[o_index2].pointadr;
	      free(nodep->u[k].child);
	      nodep->u[k].pointadr = o_dpp;
	      nodep->tag |= 1<<k;
	    }
	    else { 
	      /* move up empty node                                          */
	      free(nodep->u[k].child);
	      nodep->u[k].child = null;
	    }
	  }
	  else flag2 = 0; /* no check when at least one child                */
	}
	else {
	  /* subnode contains point, callback                                */
	  o_dpp = nodep->u[k].pointadr;
	  if(o_dpp->x >= o_dppmin->x && 
	     o_dpp->y >= o_dppmin->y && 
	     o_dpp->z >= o_dppmin->z &&
	     o_dpp->x <= o_dppmax->x && 
	     o_dpp->y <= o_dppmax->y && 
	     o_dpp->z <= o_dppmax->z) { 
	    /* point is inside !                                             */
	    if( (*o_callbackd)(o_dpp, o_client, o_anon_p) ) {
	      o_octreep->octree_counter--;
	      flag = 1;
	      nodep->u[k].child = null; /* Important: CHILD !                */
	      nodep->tag ^= (1<<k); /* toggle tag, bit=0                     */
	    }
	  }
	}
      } 	
    }
    if( (flag&flag2) ) { 
      /* changes ocurred and no child on the way                             */
      /* set flag                                                            */
      check_tree_state;
      /* flag contains -1 or index                                           */
      return flag; 
    }
    return(-1);
  }
  

  
  /* all possible values for o_index2-index1 have been dealt with,           */
  /* hence, function must have returned by now !!                            */
}
