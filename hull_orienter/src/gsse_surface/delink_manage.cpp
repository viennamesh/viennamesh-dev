/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **
     mesh generation and adapation: 2D/3D

     Copyright (c) 2003-2008 Rene Heinzl                     rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha              philipp@gsse.at
     Copyright (c) 2007-2008 Franz Stimpfl                  franz@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */
//
//   (c)  2003, Peter Fleischmann (delink)
//
#include "delink.hpp"



void insert_triangle(tri_pt tri_p) 
{
  struct tri_listdef* new_lp;

  // OPTIONAL COLLINEAR TEST
  if(!calc_collinear(tri_p->dppp[0], tri_p->dppp[1], tri_p->dppp[2])) 
  {
    msg_warn("+WARNING+: inserting a collinear triangle \n");
  }

  new_lp = (struct tri_listdef*) my_malloc(sizeof(struct tri_listdef));

  new_lp->tri_p = tri_p;     
  new_lp->ntri_lp = tri_p->dppp[0]->ntri_lp; 
  tri_p->dppp[0]->ntri_lp = new_lp;     


  new_lp = (struct tri_listdef*) my_malloc(sizeof(struct tri_listdef));

  new_lp->tri_p = tri_p;
  new_lp->ntri_lp = tri_p->dppp[1]->ntri_lp; 
  tri_p->dppp[1]->ntri_lp = new_lp;


  new_lp = (struct tri_listdef*) my_malloc(sizeof(struct tri_listdef));
  new_lp->tri_p = tri_p;
  new_lp->ntri_lp = tri_p->dppp[2]->ntri_lp;
  tri_p->dppp[2]->ntri_lp = new_lp;
}




int remove_triangle(tri_pt remtri_p) 
{
  int te;
  int i;
  int j, k, l;
  tri_pt tri_p;
  tet_pt tet_p;
  ppt* basedppp;
  struct tri_listdef *lp;
  struct tri_listdef *lp1;
  
  // remove the attached tetrahedra
  //
  te = 0;

  tet_p = remtri_p->ftet_p;
  if(tet_p != null) 
  {
     // frontside
     //
    basedppp = tet_p->basetri_p->dppp;
    te++;
    if(tet_p->basetri_p == remtri_p) 
    {
       //  first triangle: dp1 dp2 dp4
      i = exist_tri(basedppp[0], basedppp[1], tet_p->dpp4, &tri_p);
      if(i==1) 
      {
         // i=1: unmerge backside
         tri_p->ctag &= (CTG_BITMAX-4);
         tri_p->btet_p = null;   
      }
      else if(i==-1) 
      {
         //  i=-1: unmerge frontside
	tri_p->ctag &= (CTG_BITMAX-3);
	tri_p->ftet_p = null;   
      }
  
      //  second triangle: dp2 dp3 dp4
      i = exist_tri(basedppp[1], basedppp[2], tet_p->dpp4, &tri_p);
      if(i==1) 
      {
	/* i=1: unmerge backside                                             */
	tri_p->ctag &= (CTG_BITMAX-4);
	tri_p->btet_p = null;   
      }
      else if(i==-1) {
	/* i=-1: unmerge frontside                                           */
	tri_p->ctag &= (CTG_BITMAX-3);
	tri_p->ftet_p = null;   
      }
  
      /** third triangle: dp3 dp1 dp4 ****************************************/
      i = exist_tri(basedppp[2], basedppp[0], tet_p->dpp4, &tri_p);
      if(i==1) {
	/* i=1: unmerge backside                                             */
	tri_p->ctag &= (CTG_BITMAX-4);
	tri_p->btet_p = null;   
      }
      else if(i==-1) {
	/* i=-1: unmerge frontside                                           */
	tri_p->ctag &= (CTG_BITMAX-3);
	tri_p->ftet_p = null;   
      }

    }
    else {
      /* dpp4 belongs to the triangle                                        */
      /* Updating two triangles and the base triangle                        */

      /* determine index of dpp4 in remtri_p                                 */
      j = 0;
      while(j<3 && remtri_p->dppp[j]!=tet_p->dpp4) j++;

      j = (j+1)%3;
      k = 0;

      while(k<3 && basedppp[k]!=remtri_p->dppp[j]) k++;
      
// unmerge base triangle
      tet_p->basetri_p->ctag &= (CTG_BITMAX-3);
      tet_p->basetri_p->ftet_p = null;

      /** first triangle: dp1 dp2 dp4 ****************************************/
      l = (k+1)%3;
      i = exist_tri(basedppp[k], basedppp[l], tet_p->dpp4, &tri_p);
      if(i==1) {
	/* i=1: unmerge backside                                             */
	tri_p->ctag &= (CTG_BITMAX-4);
	tri_p->btet_p = null;   
      }
      else if(i==-1) {
	/* i=-1: unmerge frontside                                           */
	tri_p->ctag &= (CTG_BITMAX-3);
	tri_p->ftet_p = null;   
      }
  
      /** second triangle: dp2 dp3 dp4 ***************************************/
      i = exist_tri(basedppp[l], basedppp[(l+1)%3], tet_p->dpp4, &tri_p);
      if(i==1) {
	/* i=1: unmerge backside                                             */
	tri_p->ctag &= (CTG_BITMAX-4);
	tri_p->btet_p = null;   
      }
      else if(i==-1) {
	/* i=-1: unmerge frontside                                           */
	tri_p->ctag &= (CTG_BITMAX-3);
	tri_p->ftet_p = null;   
      }

    }
    /* finished with ftet_p                                                  */
    
  }


  tet_p = remtri_p->btet_p;
  if(tet_p != null) {
    /** BACKSIDE *************************************************************/
    basedppp = tet_p->basetri_p->dppp;
    te++;
    /* dpp4 must belong to the triangle                                      */
    /* Updating two triangles and the base triangle                          */

    /* determine index of dpp4 in remtri_p                                   */
    j = 0;
    while(j<3 && remtri_p->dppp[j]!=tet_p->dpp4) j++;

    j = (j+2)%3;
    k = 0;
    while(k<3 && basedppp[k]!=remtri_p->dppp[j]) k++;
      
    /* unmerge base triangle                                                 */
    tet_p->basetri_p->ctag &= (CTG_BITMAX-3);
    tet_p->basetri_p->ftet_p = null;

    /** first triangle: dp1 dp2 dp4 ******************************************/
    l = (k+1)%3;
    i = exist_tri(basedppp[k], basedppp[l], tet_p->dpp4, &tri_p);
    if(i==1) {
      /* i=1: unmerge backside                                               */
      tri_p->ctag &= (CTG_BITMAX-4);
      tri_p->btet_p = null;   
    }
    else if(i==-1) {
      /* i=-1: unmerge frontside                                             */
      tri_p->ctag &= (CTG_BITMAX-3);
      tri_p->ftet_p = null;   
    }
  
    /** second triangle: dp2 dp3 dp4 *****************************************/
    i = exist_tri(basedppp[l], basedppp[(l+1)%3], tet_p->dpp4, &tri_p);
    if(i==1) {
      /* i=1: unmerge backside                                               */
      tri_p->ctag &= (CTG_BITMAX-4);
      tri_p->btet_p = null;   
    }
    else if(i==-1) {
      /* i=-1: unmerge frontside                                             */
      tri_p->ctag &= (CTG_BITMAX-3);
      tri_p->ftet_p = null;   
    }

    /* finished with btet_p                                                  */
    
  }

  
  /** SECOND: Remove the triangle list elements ******************************/

  for(j=0 ; j<3 ; j++) {
    lp = remtri_p->dppp[j]->ntri_lp;
    if(lp->tri_p != remtri_p) {
      
      while( (lp->ntri_lp->tri_p) != remtri_p ) lp = lp->ntri_lp;
      
      lp1 = lp->ntri_lp; 
      
      /* can be null                                                         */
      lp->ntri_lp = lp1->ntri_lp; 
      
    }
    else {
      lp1 = lp;
      remtri_p->dppp[j]->ntri_lp = lp1->ntri_lp;
    }
    /* lp1 points to the to be removed list element                          */
    /* The remtri_p should not be derived from the list element when this    */
    /* function is called (e.g. lp->tri_p as argument to remove_triangle()), */
    /* because the list element will be freed.                               */
    free(lp1);
  }


  return te;

}


// delete a triangle without connected tetrahedra 
//
void sm_remove_triangle(tri_pt remtri_p) 
{
  int j;
  struct tri_listdef *lp;
  struct tri_listdef *lp1;
  
  for(j=0 ; j<3 ; j++) {
    lp = remtri_p->dppp[j]->ntri_lp;
    if(lp->tri_p != remtri_p) {
      
      while( (lp->ntri_lp->tri_p) != remtri_p ) lp = lp->ntri_lp;
      
      lp1 = lp->ntri_lp; 
      
      /* can be null                                                         */
      lp->ntri_lp = lp1->ntri_lp; 
      
    }
    else {
      lp1 = lp;
      remtri_p->dppp[j]->ntri_lp = lp1->ntri_lp;
    }
    /* lp1 points to the to be removed list element                          */
    /* The remtri_p should not be derived from the list element when this    */
    /* function is called (e.g. lp->tri_p as argument to remove_triangle()), */
    /* because the list element will be freed.                               */
    free(lp1);
  }

}



int remove_point(ppt remdpp, int* te_p) 
{
  int tr;
  int te;
  int i;
  int j, k, l;
  tri_pt tri_p;
  tri_pt remtri_p;
  tet_pt tet_p;
  ppt* basedppp;
  struct tri_listdef *lp;
  struct tri_listdef *lp1;
  struct tri_listdef *lp2;
  
  if(remdpp->ntri_lp == null) {
    *te_p = 0;
    return 0;
  }

  tr = 0;
  te = 0;
  lp = remdpp->ntri_lp;
  do {
    remtri_p = lp->tri_p;

    /** FIRST: Remove the attached tetrahedra ********************************/
    tet_p = remtri_p->ftet_p;
    if(tet_p != null) {
      /** FRONTSIDE **********************************************************/
      basedppp = tet_p->basetri_p->dppp;
      te++;
      if(tet_p->basetri_p == remtri_p) {
	/* dpp4 does not belong to the triangle                              */
	/* Updating three triangles                                          */
	/* do not unmerge base triangle                                      */
	
	/** first triangle: dp1 dp2 dp4 **************************************/
	i = exist_tri(basedppp[0], basedppp[1], tet_p->dpp4, &tri_p);
	if(i==1) {
	  /* i=1: unmerge backside                                           */
	  tri_p->ctag &= (CTG_BITMAX-4);
	  tri_p->btet_p = null;   
	}
	else if(i==-1) {
	  /* i=-1: unmerge frontside                                         */
	  tri_p->ctag &= (CTG_BITMAX-3);
	  tri_p->ftet_p = null;   
	}
	
	/** second triangle: dp2 dp3 dp4 *************************************/
	i = exist_tri(basedppp[1], basedppp[2], tet_p->dpp4, &tri_p);
	if(i==1) {
	  /* i=1: unmerge backside                                           */
	  tri_p->ctag &= (CTG_BITMAX-4);
	  tri_p->btet_p = null;   
	}
	else if(i==-1) {
	  /* i=-1: unmerge frontside                                         */
	  tri_p->ctag &= (CTG_BITMAX-3);
	  tri_p->ftet_p = null;   
	}
	
	/** third triangle: dp3 dp1 dp4 **************************************/
	i = exist_tri(basedppp[2], basedppp[0], tet_p->dpp4, &tri_p);
	if(i==1) {
	  /* i=1: unmerge backside                                           */
	  tri_p->ctag &= (CTG_BITMAX-4);
	  tri_p->btet_p = null;   
	}
	else if(i==-1) {
	  /* i=-1: unmerge frontside                                         */
	  tri_p->ctag &= (CTG_BITMAX-3);
	  tri_p->ftet_p = null;   
	}
	
      }
      else {
	/* dpp4 belongs to the triangle                                      */
	/* Updating two triangles and the base triangle                      */
	
	/* determine index of dpp4 in remtri_p                               */
	j = 0;
	while(j<3 && remtri_p->dppp[j]!=tet_p->dpp4) j++;
	
	j = (j+1)%3;
	k = 0;
	
	while(k<3 && basedppp[k]!=remtri_p->dppp[j]) k++;
	
	/* unmerge base triangle                                             */
	tet_p->basetri_p->ctag &= (CTG_BITMAX-3);
	tet_p->basetri_p->ftet_p = null;
	
	/** first triangle: dp1 dp2 dp4 **************************************/
	l = (k+1)%3;
	i = exist_tri(basedppp[k], basedppp[l], tet_p->dpp4, &tri_p);
	if(i==1) {
	  /* i=1: unmerge backside                                           */
	  tri_p->ctag &= (CTG_BITMAX-4);
	  tri_p->btet_p = null;   
	}
	else if(i==-1) {
	  /* i=-1: unmerge frontside                                         */
	  tri_p->ctag &= (CTG_BITMAX-3);
	  tri_p->ftet_p = null;   
	}
	
	/** second triangle: dp2 dp3 dp4 *************************************/
	i = exist_tri(basedppp[l], basedppp[(l+1)%3], tet_p->dpp4, &tri_p);
	if(i==1) {
	  /* i=1: unmerge backside                                           */
	  tri_p->ctag &= (CTG_BITMAX-4);
	  tri_p->btet_p = null;   
	}
	else if(i==-1) {
	  /* i=-1: unmerge frontside                                         */
	  tri_p->ctag &= (CTG_BITMAX-3);
	  tri_p->ftet_p = null;   
	}
	
      }
      /* finished with ftet_p                                                */
      
    }
    
    tet_p = remtri_p->btet_p;
    if(tet_p != null) {
      /** BACKSIDE ***********************************************************/
      basedppp = tet_p->basetri_p->dppp;
      te++;
      /* dpp4 must belong to the triangle                                    */
      /* Updating two triangles and the base triangle                        */
      
      /* determine index of dpp4 in remtri_p                                 */
      j = 0;
      while(j<3 && remtri_p->dppp[j]!=tet_p->dpp4) j++;
      
      j = (j+2)%3;
      k = 0;
      while(k<3 && basedppp[k]!=remtri_p->dppp[j]) k++;
      
// unmerge base triangle
      tet_p->basetri_p->ctag &= (CTG_BITMAX-3);
      tet_p->basetri_p->ftet_p = null;
      
      /** first triangle: dp1 dp2 dp4 ****************************************/
      l = (k+1)%3;
      i = exist_tri(basedppp[k], basedppp[l], tet_p->dpp4, &tri_p);
      if(i==1) {
	/* i=1: unmerge backside                                             */
	tri_p->ctag &= (CTG_BITMAX-4);
	tri_p->btet_p = null;   
      }
      else if(i==-1) {
	/* i=-1: unmerge frontside                                           */
	tri_p->ctag &= (CTG_BITMAX-3);
	tri_p->ftet_p = null;   
      }
      
      /** second triangle: dp2 dp3 dp4 ***************************************/
      i = exist_tri(basedppp[l], basedppp[(l+1)%3], tet_p->dpp4, &tri_p);
      if(i==1) {
	/* i=1: unmerge backside                                             */
	tri_p->ctag &= (CTG_BITMAX-4);
	tri_p->btet_p = null;   
      }
      else if(i==-1) {
	/* i=-1: unmerge frontside                                           */
	tri_p->ctag &= (CTG_BITMAX-3);
	tri_p->ftet_p = null;   
      }
      
      /* finished with btet_p                                                */
      
    }
    
    /** SECOND: Remove the triangle ******************************************/
    tr++;
    
    /* determine index of the given point in remtri_p                        */
    j = 0;
    while(j<3 && remtri_p->dppp[j]!=remdpp) j++;
    
    /* only take list elements out of the two other points                   */
    for(k=1 ; k<3 ; k++) {
      l = (j+k)%3;
      lp1 = remtri_p->dppp[l]->ntri_lp;

      if(lp1->tri_p != remtri_p) {
	
	while( (lp1->ntri_lp->tri_p) != remtri_p ) lp1 = lp1->ntri_lp;
	
	lp2 = lp1->ntri_lp; 
	
	/* can be null                                                       */
	lp1->ntri_lp = lp2->ntri_lp; 
	
      }
      else {
	lp2 = lp1;
	remtri_p->dppp[l]->ntri_lp = lp2->ntri_lp;
      }
      /* lp2 points to the to be removed list element                        */
      free(lp2);
    }
    

    /* get next connected triangle of remdpp                                 */
    lp = lp->ntri_lp;

  } while (lp != null);      
	  
  /* finished with all connected triangles and its tetrahedra                */
  /* The removed point contains the list elements of the removed triangles   */
  
  *te_p = te;

  return tr;

}
  


/*****************************************************************************/
/*                                                                           */
/*  sm_remove_point()                                                        */
/*                                                                           */
/*****************************************************************************/
/* Cuts a point out of the datastructure                                     */
/* Thereby, also removing connected triangles (not tetrahedra)               */
/*                                                                           */
/* The removed elements are not erased and are still accessible via the      */
/* list elements of the point (triangle pointers)                            */
/*                                                                           */
/* Is equivalent to a call to sm_remove_triangle() for each connected        */
/* triangle. However, one list element needs to be preserved so the          */
/* function sm_remove_triangle() cannot directly be used.                    */
/*                                                                           */
/* Returns the number of removed triangles                                   */
/*                                                                           */
/* The removal of a point in this sense is independent of the octree         */
/* The point can still be inserted into an octree                            */
/* Only when the point needs to be erased then it needs to be removed from   */
/* the octree                                                                */
/*                                                                           */
int sm_remove_point(ppt remdpp) {
  int tr;
  int j, k, l;
  tri_pt remtri_p;
  struct tri_listdef *lp;
  struct tri_listdef *lp1;
  struct tri_listdef *lp2;
  
  if(remdpp->ntri_lp == null) {
    return 0;
  }

  tr = 0;
  lp = remdpp->ntri_lp;
  do {
    remtri_p = lp->tri_p;
    tr++;
    
    /* determine index of the given point in remtri_p                        */
    j = 0;
    while(j<3 && remtri_p->dppp[j]!=remdpp) j++;
    
    /* only take list elements out of the two other points                   */
    for(k=1 ; k<3 ; k++) {
      l = (j+k)%3;
      lp1 = remtri_p->dppp[l]->ntri_lp;

      if(lp1->tri_p != remtri_p) {
	
	while( (lp1->ntri_lp->tri_p) != remtri_p ) lp1 = lp1->ntri_lp;
	
	lp2 = lp1->ntri_lp; 
	
	/* can be null                                                       */
	lp1->ntri_lp = lp2->ntri_lp; 
	
      }
      else {
	lp2 = lp1;
	remtri_p->dppp[l]->ntri_lp = lp2->ntri_lp;
      }
      /* lp2 points to the to be removed list element                        */
      free(lp2);
    }
    

    /* get next connected triangle of remdpp                                 */
    lp = lp->ntri_lp;

  } while (lp != null);      
	  
  /* finished with all connected triangles                                   */
  /* The removed point contains the list elements of the removed triangles   */
  

  return tr;

}







/*****************************************************************************/
/*                                                                           */
/*  sm_ref_triedge()                                                         */
/*                                                                           */
/*****************************************************************************/
/* Refine an edge to which no tetrahedra are connected                       */
/* Multiple lines are allowed (more than one adjacent triangle per edge)     */
/* The given point dpp can be located anywhere on the edge which is given by */
/* the index m of the opposite point.                                        */
/*                                                                           */
/* If the point is connected, the existing lps will be preserved. However,   */
/* there are no additonal checks to avoid double elements !                  */
/* If the point is unconnected, ntri_lp will not be initialized. It must be  */
/* null already.                                                             */
/*                                                                           */
/* insert_triangle() must add the new list element at the TOP !              */
/* May not copy 'CTG_SEED' bit, but otherwise ctags and orientation remain   */
/* unchanged.                                                                */
/* Return number of created and inserted triangles (can be more than one)    */
/* does not insert point into octree                                         */
/* will not use remove functions because more efficient to modify and create */
/* only one new triangle per refined old triangle                            */
/* Also, for stacktri_p uses as e.g. in surface_refinement() it is necessary that the  */
/* stacktri_p(==reftri_p) is still at the same position in the list !        */
/*                                                                           */
/* The function sm_ref_triedge() is required to modify the triangles in such */
/* a way that dpp2 is cut out and all newly created triangles include dpp2.  */
/* OTHERWISE the function ref_triedge() will not work !                      */
/* That also means that dpp1, dpp2 must be derived in the same way in both   */
/* functions (definition of m and modulo operation)                          */
/*                                                                           */
int sm_ref_triedge(tri_pt reftri_p, int m, ppt dpp, tri_pt* lasttri_pp) {
  int i;
  int tr;
  ppt dpp1;
  ppt dpp2;
  struct tri_listdef *lp;
  struct tri_listdef *lp1; 


  /* initialize number of new triangles                                      */
  tr = 0;

  /* has to be in correct order (orientation)                                */
  dpp1 = reftri_p->dppp[(m+1)%3]; 
  dpp2 = reftri_p->dppp[i=((m+2)%3)];


  /** Inserting triangles with same ctag and orientation ! *******************/

  /* FIRST: change *reftri_p (take out dpp2)                                 */
  lp = dpp2->ntri_lp;
  if(lp->tri_p != reftri_p) {
    
    while( (lp->ntri_lp->tri_p)!= reftri_p ) lp = lp->ntri_lp;
    
    lp1 = lp->ntri_lp; 

    /* can be null                                                           */
    lp->ntri_lp = lp1->ntri_lp; 

  }
  else {
    lp1 = lp;
    dpp2->ntri_lp = lp1->ntri_lp;
  }

  /* preserve other list elements for connected points                       */
  lp1->ntri_lp = dpp->ntri_lp;  
  dpp->ntri_lp = lp1;


  /* change dpp2 (index i) in *reftri_p                                      */
  reftri_p->dppp[i] = dpp;

  /* OPTIONAL COLLINEAR TEST should be disabled as long as no special return */
  if(!calc_collinear(reftri_p->dppp[0], reftri_p->dppp[1], reftri_p->dppp[2])){
    msg_warn("+WARNING+: triedge refinement makes a collinear triangle \n");
  }
  
  
  /* creating and inserting triangle, counting it                            */
  tr++;
  // [RH] change 1
  //
  //printf("[RH] .. triangle insert .. style 1.1 \n");
  //printf("#!#2 refine tri mat1: %ld, mat2: %ld \n", reftri_p->material[0], reftri_p->material[1]);
  //printf("%lf %lf %lf\n", reftri_p->dppp[0]->x,  reftri_p->dppp[0]->y, reftri_p->dppp[0]->z);
  //printf("%lf %lf %lf\n", reftri_p->dppp[1]->x,  reftri_p->dppp[1]->y, reftri_p->dppp[1]->z);
  //printf("%lf %lf %lf\n", reftri_p->dppp[2]->x,  reftri_p->dppp[2]->y, reftri_p->dppp[2]->z);

  create_triangle_rh(reftri_p,
		     reftri_p->dppp[m], 
		     dpp,
		     dpp2, 
		     reftri_p->ctag&(CTG_BITMAX-CTG_SEED), 
		     reftri_p->facemark, 
		     *lasttri_pp);
  insert_triangle(*lasttri_pp);
  
  /* SECOND: Searching for all adj. triangles (not limited to one)           */
  /* Look in list of dpp2, then taking dpp2 (lp) out is easy.                */
  /* TOP list el. can be skipped, is recently added triangle.                */
  /* i           will now be the index of dpp1 instead of dpp2               */
  /* dpp1, dpp2  will not have correct orientation anymore  !                */

  /* cannot be null                                                          */
  lp = dpp2->ntri_lp; 

  while( (lp1=lp->ntri_lp) !=null) {

    i = 0;

    while (i<3 && lp1->tri_p->dppp[i]!=dpp1) i++;

    if(i!=3) { 
      /* atri found                                                          */
      if( lp1->tri_p->dppp[(m=(i+1)%3)] == dpp2 ) {  
	/* m index of dpp2                                                   */

	/* can be null                                                       */
	lp->ntri_lp = lp1->ntri_lp; 

	lp1->ntri_lp = dpp->ntri_lp;
	dpp->ntri_lp = lp1;
	lp1->tri_p->dppp[m] = dpp;

	/* creating and inserting triangle, counting it                      */
	tr++;
	//printf("#!#3 refine tri mat1: %ld, mat2: %ld \n", (*lasttri_pp)->material[0], (*lasttri_pp)->material[1]);
	// [RH] changed
	//
	create_triangle_rh(lp1->tri_p,
			   lp1->tri_p->dppp[(m+1)%3], 
			   dpp, 
			   dpp2,
			   lp1->tri_p->ctag&(CTG_BITMAX-CTG_SEED), 
			   lp1->tri_p->facemark, 
			   *lasttri_pp);
	insert_triangle(*lasttri_pp);
      }
      else {
	/* m index of other point                                            */

	/* can be null                                                       */
	lp->ntri_lp = lp1->ntri_lp; 

	lp1->ntri_lp = dpp->ntri_lp;
	dpp->ntri_lp = lp1;

	/* (m+1)%3 must be index of dpp2                                     */
	lp1->tri_p->dppp[(m+1)%3] = dpp; 

	/* creating and inserting triangle, counting it                      */
	tr++;
	//printf("#!#4 refine tri mat1: %ld, mat2: %ld \n", (*lasttri_pp)->material[0], (*lasttri_pp)->material[1]);
	// [RH] changed
	//
	create_triangle_rh(lp1->tri_p,
			   lp1->tri_p->dppp[m], 
			   dpp2,
			   dpp,
			   lp1->tri_p->ctag&(CTG_BITMAX-CTG_SEED),
			   lp1->tri_p->facemark,
			   *lasttri_pp);
	insert_triangle(*lasttri_pp);
      }
    }
    else { 
      /* only increment lp if lp1 was not taken out                          */
      /* not an atri                                                         */

      /* cannot be null                                                      */
      lp = lp1; 
    }
  }

  return tr; 

}



/*****************************************************************************/
/*                                                                           */
/*  sm_ref_edge()                                                            */
/*                                                                           */
/*****************************************************************************/
/* Refine an edge to which no tetrahedra are connected                       */
/* Multiple lines are allowed (more than one adjacent triangle per edge)     */
/* The given point dpp can be located anywhere on the edge                   */
/* Same as function sm_ref_triedge except the edge is given by two points    */
/*                                                                           */
/* If the point is connected, the existing lps will be preserved. However,   */
/* there are no additonal checks to avoid double elements !                  */
/* If the point is unconnected, ntri_lp will not be initialized. It must be  */
/* null already.                                                             */
/*                                                                           */
/* insert_triangle() must add the new list element at the TOP !              */
/* May not copy 'CTG_SEED' bit, but otherwise ctags and orientation remain   */
/* unchanged.                                                                */
/* Return number of created and inserted triangles (can be zero !)           */
/* does not insert point into octree                                         */
/* will not use remove functions because more efficient to modify and create */
/* only one new triangle per refined old triangle                            */
/* Also, for stacktri_p uses as e.g. in surface_refinement() it is necessary that the  */
/* refined triangles are still at the same position in the list !            */
/*                                                                           */
int sm_ref_edge(ppt dpp1, ppt dpp2, ppt dpp, tri_pt* lasttri_pp) {
  int i;
  int tr;
  int m;
  struct tri_listdef **lpp;
  struct tri_listdef *lp1; 


  /* initialize number of new triangles                                      */
  tr = 0;

  /** Inserting triangles with same ctag and orientation ! *******************/

  /* Searching for all adjacent triangles (not limited to one)               */
  /* Look in list of dpp2, then taking dpp2 (lp) out is easy.                */
  /* TOP list element is not skipped.                                        */
  /* i           will now be the index of dpp1 instead of dpp2               */
  /* dpp1, dpp2  will not have correct orientation anymore  !                */

  lpp = &dpp2->ntri_lp; 

  while( (lp1=(*lpp)) != null ) {

    i = 0;

    while (i<3 && lp1->tri_p->dppp[i]!=dpp1) i++;

    if(i!=3) { 
      /* atri found                                                          */
      if( lp1->tri_p->dppp[(m=(i+1)%3)] == dpp2 ) {  
	/* m index of dpp2                                                   */

	/* can be null                                                       */
	*lpp = lp1->ntri_lp; 

	lp1->ntri_lp = dpp->ntri_lp;
	dpp->ntri_lp = lp1;
	lp1->tri_p->dppp[m] = dpp;

	/* OPTIONAL COLLINEAR TEST                                           */
	if(!calc_collinear(lp1->tri_p->dppp[0], 
			   lp1->tri_p->dppp[1], 
			   lp1->tri_p->dppp[2])) {
	  msg_warn("+WARNING+: edge refinement makes a collinear triangle \n");
	}

	/* creating and inserting triangle, counting it                      */
	tr++;
	//printf("[RH] .. triangle insert .. style smrefedge 1.1 \n");
	//printf("# refine tri mat1: %ld, mat2: %ld \n", lp1->tri_p->material[0], lp1->tri_p->material[1]);
	//printf("%lf %lf %lf\n", lp1->tri_p->dppp[0]->x,  lp1->tri_p->dppp[0]->y, lp1->tri_p->dppp[0]->z);
	//printf("%lf %lf %lf\n", lp1->tri_p->dppp[1]->x,  lp1->tri_p->dppp[1]->y, lp1->tri_p->dppp[1]->z);
	//printf("%lf %lf %lf\n", lp1->tri_p->dppp[2]->x,  lp1->tri_p->dppp[2]->y, lp1->tri_p->dppp[2]->z);

	// [RH] changed
	//
	create_triangle_rh(lp1->tri_p,
			   lp1->tri_p->dppp[(m+1)%3], 
			   dpp, 
			   dpp2,
			   lp1->tri_p->ctag&(CTG_BITMAX-CTG_SEED),
			   lp1->tri_p->facemark,
			   *lasttri_pp);
	insert_triangle(*lasttri_pp);
      }
      else {
	/* m index of other point                                            */

	/* can be null                                                       */
	*lpp = lp1->ntri_lp; 

	lp1->ntri_lp = dpp->ntri_lp;
	dpp->ntri_lp = lp1;

	/* (m+1)%3 must be index of dpp2                                     */
	lp1->tri_p->dppp[(m+1)%3] = dpp; 

	/* OPTIONAL COLLINEAR TEST                                           */
	if(!calc_collinear(lp1->tri_p->dppp[0], 
			   lp1->tri_p->dppp[1], 
			   lp1->tri_p->dppp[2])) {
	  msg_warn("+WARNING+: edge refinement makes a collinear triangle \n");
	}

	/* creating and inserting triangle, counting it                      */
	//printf("[RH] .. triangle insert .. style smrefedge 2.1 \n");
	//printf("# refine tri mat1: %ld, mat2: %ld \n", lp1->tri_p->material[0], lp1->tri_p->material[1]);
	//printf("%lf %lf %lf\n", lp1->tri_p->dppp[0]->x,  lp1->tri_p->dppp[0]->y, lp1->tri_p->dppp[0]->z);
	//printf("%lf %lf %lf\n", lp1->tri_p->dppp[1]->x,  lp1->tri_p->dppp[1]->y, lp1->tri_p->dppp[1]->z);
	//printf("%lf %lf %lf\n", lp1->tri_p->dppp[2]->x,  lp1->tri_p->dppp[2]->y, lp1->tri_p->dppp[2]->z);

	tr++;
	// [RH] changed
	//
	create_triangle_rh(lp1->tri_p,
			   lp1->tri_p->dppp[m],
			   dpp2, 
			   dpp,
			   lp1->tri_p->ctag&(CTG_BITMAX-CTG_SEED),
			   lp1->tri_p->facemark,
			   *lasttri_pp);
	insert_triangle(*lasttri_pp);
      }
    }
    else { 
      /* only increment lp if lp1 was not taken out                          */
      /* not an atri                                                         */

      /* cannot be null                                                      */
      lpp = &((*lpp)->ntri_lp);
    }
  }

  return tr; 

}
    



/*****************************************************************************/
/*                                                                           */
/*  sm_ref_tri()                                                             */
/*                                                                           */
/*****************************************************************************/
/* Refine a triangle without connected tetrahedra with an inside point       */
/* The given point dpp can be located anywhere inside of the triangle        */
/* and even *outside* of the plane spanned by the triangle !                 */
/*                                                                           */
/* If the point is connected, the existing lps will be preserved. However,   */
/* there are no additonal checks to avoid double elements !                  */
/* If the point is unconnected, ntri_lp will not be initialized. It must be  */
/* null already.                                                             */
/*                                                                           */
/* May not copy 'CTG_SEED' bit, but otherwise ctags and orientation remain   */
/* unchanged.                                                                */
/* Return number of created and inserted triangles (is always 2)             */
/* does not insert point into octree                                         */
/* will not use remove functions because more efficient to modify and create */
/* only two new triangles                                                    */
/* Also, for stacktri_p uses as e.g. in surface_refinement() it is necessary that the  */
/* stacktri_p(==reftri_p) is still at the same position in the list !        */
/*                                                                           */
int sm_ref_tri(tri_pt reftri_p, ppt dpp, tri_pt* lasttri_pp) {
  ppt dpp2;
  struct tri_listdef *lp;
  struct tri_listdef *lp1; 


  dpp2 = reftri_p->dppp[1];


  /** Inserting triangles with same ctag and orientation ! *******************/

  /* FIRST: change *reftri_p (take out dpp2)                                 */
  lp = dpp2->ntri_lp;
  if(lp->tri_p != reftri_p) {

    while( (lp->ntri_lp->tri_p)!= reftri_p ) lp = lp->ntri_lp;
    lp1 = lp->ntri_lp; 

    /* can be null                                                           */
    lp->ntri_lp = lp1->ntri_lp; 

  }
  else {
    lp1 = lp;
    dpp2->ntri_lp = lp1->ntri_lp;
  }

  /* preserve other list elements for connected points                       */
  lp1->ntri_lp = dpp->ntri_lp;  
  dpp->ntri_lp = lp1;

  /* change dpp2 (index 1) in *reftri_p                                      */
  reftri_p->dppp[1] = dpp;

  /* OPTIONAL COLLINEAR TEST should be disabled as long as no special return */
  if(!calc_collinear(reftri_p->dppp[0], reftri_p->dppp[1], reftri_p->dppp[2])){
    msg_warn("+WARNING+: tri refinement makes a collinear triangle \n");
  }
  

  /** Insert other two triangles *********************************************/

  /* creating and inserting triangle                                         */

  // [RH] change 2
  //
  // printf("[RH] .. triangle insert .. style 3.1 \n");
  create_triangle_rh(reftri_p,
		     reftri_p->dppp[0],
		     dpp2,
		     dpp, 
		     reftri_p->ctag&(CTG_BITMAX-CTG_SEED),
		     reftri_p->facemark,
		     *lasttri_pp);
  insert_triangle(*lasttri_pp);

  /* creating and inserting triangle                                         */

  // [RH] change 2
  //
  //  printf("[RH] .. triangle insert .. style 3.2 \n");
  create_triangle_rh(reftri_p,
		     dpp2, 
		     reftri_p->dppp[2], 
		     dpp, 
		     reftri_p->ctag&(CTG_BITMAX-CTG_SEED),
		     reftri_p->facemark,
		     *lasttri_pp);
  insert_triangle(*lasttri_pp);
  
  
  return 2; 

}









/*****************************************************************************/
/*                                                                           */
/*  sm_flip_tris()                                                           */
/*                                                                           */
/*****************************************************************************/
/* Flip two triangles to which no tetrahedra are connected                   */
/* The adjacent edge to be swapped is given by the indices of the two        */
/* opposite points of each triangle                                          */
/* (first index belongs to first triangle, second to second triangle)        */
/* The triangles do not have to lie in the same plane                        */
/* It is checked if the flipped triangles exist already                      */
/* The two triangles *must* be adjacent !                                    */
/* The edge to be flipped may not be a multiple edge ! (not checked)         */
/* will not use remove functions and does not need lasttri_p                 */
/* (more efficient to modify the two triangles and independent of the list)  */
/* (tri_list elements are assumed to carry only tri_p info)                  */
/* ctags and orientation are left unchanged                                  */
/* ditto for facemark                                                        */
/* If the data structure is not consistent e.g. points do not have pointers  */
/* to connected triangles, a neverending loop may evolve.                    */
/*                                                                           */
/* Important: The triangles must in conjunction be CONVEX !                  */
/* otherwise the flipped triangles overlap !                                 */
/* If point 2 is inside of circum-circle1 or vice versa then tris are convex */
/*                                                                           */
/* Return 0 if flipped triangles exist already and no flip possible          */
/*     or if numerically not convex and flipping would create collinear tris */
/*                                                                           */
/* Flip convention (necessary for some functions like e.g. rcheck)           */
/* The edge is swapped in the direction of the orientation of the first      */
/* triangle. That also means that dppA will belong soley to tri_p1 and dppB  */
/* soley to tri_p2.                                                          */
/*                                                                           */
/* Could check to make sure that no triangles are ever flipped with different*/
/* facemarks, but not necessary because will be checked when angle is checked*/
int sm_flip_tris(tri_pt tri_p1, tri_pt tri_p2, int index1, int index2) {
  struct tri_listdef *lp;
  struct tri_listdef *lp1;
  ppt dppA;
  ppt dppB;
  int i;
  int k;
  tri_pt dummy;

  k = (index1+2)%3;
  dppB = tri_p1->dppp[k];
  i = (index2+1)%3;

  if(tri_p2->dppp[i]==dppB) {
    i = (index2+2)%3;
    dppA = tri_p2->dppp[i];
  }
  else {
    dppA = tri_p2->dppp[i];
  }
  
  /* Could happen that flipped triangles exist already                       */
  if(exist_tri(tri_p1->dppp[index1], tri_p2->dppp[index2], dppA, &dummy) !=0) {
    return 0;
  }
  if(exist_tri(tri_p1->dppp[index1], tri_p2->dppp[index2], dppB, &dummy) !=0) {
    return 0;
  }
  /* could limit flipping and return 0 as if tris would be existing. However */
  /* can be quite dangerous under finite numerics to disable flipping even   */
  /* though it would not be disabled for coll tris but rather for making     */
  /* coll tris?                                                              */
  if(!calc_collinear(tri_p1->dppp[index1], tri_p2->dppp[index2], dppA)) {
    msg_warn("+WARNING+: Flip makes a collinear triangle \n");
    /* return 0; */
  }
  if(!calc_collinear(tri_p1->dppp[index1], tri_p2->dppp[index2], dppB)) {
    msg_warn("+WARNING+: Flip makes a collinear triangle \n");
    /* return 0; */
  }
  
  /* CHANGE DATASTRUCTURE, FLIP !                                            */
  /* overwrite dppB in tri_p1                                                */
  tri_p1->dppp[k] = tri_p2->dppp[index2];
  /* overwrite dppA in tri_p2                                                */
  tri_p2->dppp[i] = tri_p1->dppp[index1];


  /* TAKING FIRST triangle out of list of point B                            */
  lp = dppB->ntri_lp;
  if(lp->tri_p != tri_p1) {
    
    while( (lp->ntri_lp->tri_p)!=tri_p1 ) lp = lp->ntri_lp;
    /* contains the FIRST triangle                                           */
    lp1 = lp->ntri_lp; 

    /* can be null                                                           */
    lp->ntri_lp = lp1->ntri_lp; 

  }
  else {
    lp1 = lp;
    dppB->ntri_lp = lp1->ntri_lp;
  }
  /* inserting first triangle into list of point 2                           */
  lp1->ntri_lp = tri_p2->dppp[index2]->ntri_lp;
  tri_p2->dppp[index2]->ntri_lp = lp1;
  

  /* TAKING SECOND triangle out of list of point A                           */
  lp = dppA->ntri_lp;
  if(lp->tri_p != tri_p2) {

    while( (lp->ntri_lp->tri_p)!=tri_p2 ) lp = lp->ntri_lp;
    /* contains the SECOND triangle                                          */
    lp1 = lp->ntri_lp; 

    /* can be null                                                           */
    lp->ntri_lp = lp1->ntri_lp; 

  }
  else {
    lp1 = lp;
    dppA->ntri_lp = lp1->ntri_lp;
  }
  /* inserting second triangle into list of point 1                          */
  lp1->ntri_lp = tri_p1->dppp[index1]->ntri_lp;
  tri_p1->dppp[index1]->ntri_lp = lp1;
  


  return 1;

}



/* the maximal number of adjacent triangles to follow                        */
#define MAXREC_LOCATE      100

/*****************************************************************************/
/*                                                                           */
/*  sm_locate_tri()                                                          */
/*                                                                           */
/*****************************************************************************/
/* Locates for a given Delaunay triangle and its circumcenter H the          */
/* (adjacent) triangle which contains the projection of H. Makes only sense  */
/* for Delaunay triangles and almost planar surfaces. Adjacent in this       */
/* context means that the triangle is connected to the original triangle via */
/* other triangles along flippable edges.				     */
/* The searching procedure recurses along these flippable edges.	     */
/* If an edge is a triple-line or otherwise not flippable the search         */
/* terminates and no triangle will be found. However, the address of the     */
/* last triangle and the index of the point opposite of its non-flippable    */
/* edge will be returned. (will be needed !)                                 */
/* Recursion always follows only one edge per triangle. This is only         */
/* sufficient and makes only sense for Delaunay triangles.                   */
/*                                                                           */
/* Generally works best if vH is within sphere of tri (steiner, or distdpp)  */
/* (then only one edge to follow most likely, and no weird cases)            */
/*                                                                           */
/* Uses structural edge information!                                         */
/*                                                                           */
/* For general (e.g. non Delaunay) triangles and other points the search     */
/* can be interpreted as following: Project all triangles and the given      */
/* point into the plane of the given triangle. Then, find the triangle that  */
/* contains the projected point by recursing and following flippable         */
/* edges (one per passed triangle). It will not look for the                 */
/* best edge to follow if there are more than one, and it will not check if  */
/* it keeps passing the same triangle again and again. It only checks a      */
/* maximal number of passed triangles (MAXREC_LOCATE)                        */
/* If the minimum flip angle is smaller than 90                              */
/* degrees, and it is recursed along such an edge, the orientation of the    */
/* projected triangle flips ! The normal vectors for the edges actually      */
/* point inside of the projected triangle, and therefore correctly lead to   */
/* further recursions ! It should not happen that after a sharper than 90    */
/* degree angle the first triangle is detected to falsely contain the        */
/* projected point. Only with angles close to 90 degrees and zero area       */
/* projected triangles it may happen due to numerics that the detected       */
/* triangle is not really holding the projected point, but rather containing */
/* the projection vector vnp in its plane.                                   */
/*                                                                           */
/* Further extension: Allow specification of vnp (used to be tri.n)          */
/* This is necessary to correct the projection direction! (travel classes)   */
/*                                                                           */
/* ARGUMENTS:                                                                */
/* tpp is triangle_param structure holding n and tri_p.                      */
/* vH is the point of which the projection is searched (steiner or distdpp)  */
/* vnp is the given projection direction (used for correcting/averaging n)   */
/*       (better if given normalized!? AND MUST have correct orientation!!)  */
/* distdpp can be null and is only used for safety check to possibly ret. 6  */
/*                                                                           */
/* Return pointer to the found triangle					     */
/* Return  3 if the triangle which was found is the original triangle.       */
/* Return  4 if the triangle which was found is an adjacent triangle.        */
/* Return  0-2 if the recursion stopped due to a non-flippable edge.         */
/* Return  5 if the recursion depth was exceeded. (and null)                 */
/* Return  6 if distdpp is encountered among the flippable tris              */
/*  (happens only if called for 'bad' distdpp, e.g. cnepslist when tri       */
/*   is due to OTHER points not Delaunay)                                    */
/*									     */
/*									     */
/*									     */
/* USED EPSILONS:							     */
/*  MINN2_EPS								     */
/*									     */
/*                                                                           */
tri_pt sm_locate_tri(mesh_p_t mesh_p,
		     struct arraydef* pclspool_p,
		     tri_pt tri_p, 
		     struct vectordef* vH_p, 
		     struct vectordef* vnp_p,
		     int* index_p,
		     ppt distdpp) {

  struct vectordef vnewn;
  struct vectordef va;
  struct vectordef ve;
  int i;
  int d;
  int i0;
  int i1;
  struct tri_listdef* lp;
  ppt dpp0;
  ppt dpp1;
  ppt newdpp;
  pcl_pt pclasspool;

  /* even if H is outside of the bounding box it is necessary to find        */
  /* the last triangle ! (therefore no bounding box checks)                  */

  /* vnp_p usually is tri.n (normalized! orientation!)                       */
  /* vnp must be correctly oriented !                                        */
  /* Will not be changed here, so as argument caller can give &tri.n !       */

  pclasspool = (pcl_pt) pclspool_p->vp;
  
  /* First check 3 edges for first triangle                                  */
  for(i=0 ; i<3 ; i++) {
    /* check for every edge on which side H lies                             */
    dpp0 = tri_p->dppp[i];
    dpp1 = tri_p->dppp[(i+1)%3];
    if(dpp0==distdpp || dpp1==distdpp) {
      /* return because H was derived from a point within the flipable plane */
      (*index_p) = 6;
      return null;
    }
    ve.x = dpp1->x - dpp0->x;    
    ve.y = dpp1->y - dpp0->y;
    ve.z = dpp1->z - dpp0->z;

    /* ve x vnp                                                              */
    vnewn.x = (ve.y * vnp_p->z - vnp_p->y * ve.z);   
    vnewn.y = (ve.z * vnp_p->x - vnp_p->z * ve.x);
    vnewn.z = (ve.x * vnp_p->y - vnp_p->x * ve.y);

    va.x = vH_p->x - dpp0->x;    
    va.y = vH_p->y - dpp0->y;
    va.z = vH_p->z - dpp0->z;

    if( (va.x*vnewn.x + va.y*vnewn.y + va.z*vnewn.z) >= 0.0 ) {
      /* better to include zero !                                            */
      break;
    }
  }

  if(i==3) {
    /* H lies inside of the triangle                                         */
    (*index_p) = 3;
    return tri_p;
  }
  
  /* initialize index_p !                                                    */
  (*index_p) = (i+2)%3; 
  d = 0;
  do {  
    /* dpp0 dpp1 have been checked for distdpp !!!                           */
    /* one of it as newdpp                                                   */

    /* follow edge ve (points dpp0 -> dpp1), also using uptodate va, tri_p!  */
    
    /* Check if flippable edge                                               */
    /* BE CAREFUL not to rely on pclasses for non-structural points!         */
    if(dpp0->point_index != init_point_index &&
       dpp1->point_index != init_point_index) {
      /* maybe gline                                                         */
      i=0;
      while(i<pclasspool[dpp0->point_index].pri_gline.n &&
	    ((ppt*)(pclasspool[dpp0->point_index].pri_gline.vp))[i]!=dpp1) i++;
      if(i<pclasspool[dpp0->point_index].pri_gline.n) {
	/* ve is a gline (found dpp1 in pri list of dpp0)                    */
	/* (*index_p) is already set to the correct index !                  */
	return tri_p;
      }
      i=0;
      while(i<pclasspool[dpp0->point_index].sec_gline.n &&
	    ((ppt*)(pclasspool[dpp0->point_index].sec_gline.vp))[i]!=dpp1) i++;
      if(i<pclasspool[dpp0->point_index].sec_gline.n) {
	/* ve is a gline (found dpp1 in sec list of dpp0)                    */
	/* (*index_p) is already set to the correct index !                  */
	return tri_p;
      }
    }
    /* either one not a structural point or anyway ve not a gline            */

    /* Find the adjacent triangle                                            */
    /* do not reset tri_p, is still last triangle!                           */
    lp = dpp0->ntri_lp;
    while(lp!=null) {
      /* excluding triangle itself                                           */
      if(lp->tri_p != tri_p) {
	i = 0;
	while (i<3 && lp->tri_p->dppp[i]!=dpp1) i++;
	if(i!=3) { 
	  /* found adjacent triangle                                         */
	  /* CAN now only be one, because must be flipable!                  */
	  /* (topology does not change during surface refinement!)           */

	  /* UPDATE TRI_P ALREADY HERE!                                      */
	  tri_p = lp->tri_p;

	  /* index of point dpp1 in new triangle                             */
	  i1 = i; 
	  i = (i+1)%3;
	  if(tri_p->dppp[i]==dpp0) {
	    /* index of point dpp0 in new triangle                           */
	    i0 = i; 
	    newdpp = tri_p->dppp[(i+1)%3];
	  }
	  else {
	    /* index of point dpp0 in new triangle                           */
	    i0 = (i+1)%3; 
	    newdpp = tri_p->dppp[i];
	  }
	  /* stop looking for more atris                                     */
	  break;
	}
      }
      lp = lp->ntri_lp;
    }

    if(lp==null) {
      msg_warn("+WARNING+: Inconsistent pclass_pool? No adjacent triangle found for a flipable edge? \n"); 
      /* (*index_p) is already set to the correct index !                    */
      /* tri_p is still the last triangle or the original one                */
      return tri_p;
    } 

#if ONLINE_VISUALIZATION == ON
    msg_dinfo("Missing triangle is currently investigated in sm_locate\n");
    if(tri_p->ctag&CTG_BLOCK) {
      graphical_debug(&mesh_p->grid.unified_top_tri, 1);
    }
    else {
      tri_p->ctag |= CTG_BLOCK;
      graphical_debug(&mesh_p->grid.unified_top_tri, 1);
      tri_p->ctag &= (CTG_BITMAX - CTG_BLOCK);
    }
#endif

    /* check if the new point in the flipable plane is the distdpp           */
    if(newdpp==distdpp) {
      /* return because H was derived from a point within the flipable plane */
      (*index_p) = 6;
      return null;
    }

    /* CAREFUL tri_p is now the new triangle!                                */

    /* CHECK EDGE dpp0 -> newdpp (using va)                                  */
    (*index_p) = i1;
    ve.x = newdpp->x - dpp0->x;
    ve.y = newdpp->y - dpp0->y;
    ve.z = newdpp->z - dpp0->z;
    
    /* ve x vnp                                                              */
    /* can now be *zero*                                                     */
    vnewn.x = (ve.y * vnp_p->z - vnp_p->y * ve.z);   
    vnewn.y = (ve.z * vnp_p->x - vnp_p->z * ve.x);   
    vnewn.z = (ve.x * vnp_p->y - vnp_p->x * ve.y);
    
    if( (vnewn.x*vnewn.x+vnewn.y*vnewn.y+vnewn.z*vnewn.z) > MINN2_EPS ) {  
      if( (va.x*vnewn.x + va.y*vnewn.y + va.z*vnewn.z) >= 0.0 ) {
	/* better to include zero !                                          */
	/* found NEXT EDGE to follow                                         */
	/* Initialize: va, dpp0, ve, tri_p, and dpp1                         */
	/* (if not yet done for recursion)                                   */
	dpp1 = newdpp;
	d++;
	continue;
      }
    }

    /* CHECK EDGE newdpp -> dpp1                                             */
    (*index_p) = i0;
    ve.x = dpp1->x - newdpp->x;
    ve.y = dpp1->y - newdpp->y;
    ve.z = dpp1->z - newdpp->z;

    /* ve x vnp                                                              */
    /* can now be *zero*                                                     */
    vnewn.x = (ve.y * vnp_p->z - vnp_p->y * ve.z);   
    vnewn.y = (ve.z * vnp_p->x - vnp_p->z * ve.x);  
    vnewn.z = (ve.x * vnp_p->y - vnp_p->x * ve.y);

    va.x = vH_p->x - newdpp->x;    
    va.y = vH_p->y - newdpp->y;
    va.z = vH_p->z - newdpp->z;

    if( (vnewn.x*vnewn.x+vnewn.y*vnewn.y+vnewn.z*vnewn.z) > MINN2_EPS ) {  
      if( (va.x*vnewn.x + va.y*vnewn.y + va.z*vnewn.z) >= 0.0 ) {
	/* better to include zero !                                          */
	/* found NEXT EDGE to follow                                         */
	/* Initialize: dpp1, ve, tri_p, dpp0, and va                         */
	/* (if not yet done for recursion)                                   */
	dpp0 = newdpp;
	d++;
	continue;
      }
    }

    /* could not find edge to follow                                         */
    /* either located the right triangle or nonsense due to extremely        */
    /* nonplanar surface                                                     */

    /* found triangle other than the original triangle                       */
    (*index_p) = 4; 
    return tri_p;
    
  } while(d < MAXREC_LOCATE);

  /* kept finding edges to follow and exceeded maximum recursion depth       */
  /* must be due to nonplanar nonsense or more than MAXREC_LOCATE triangles  */
  /* within one circle around a Delaunay triangle                            */

  msg_dinfo("Exceeded maximal recursion depth in sm_locate_tri() \n");
  (*index_p) = 5;
  
  return null;
  
}




/*****************************************************************************/
/*                                                                           */
/*  exist_tri()                                                              */
/*                                                                           */
/*****************************************************************************/
/* Find an inserted triangle and return its pointer and relative orientation */
/* (depends on order of the three given points)                              */
/* returns 1 if the found triangle has the same orientation                  */
/*        -1                               mirrored orientation              */
/*         0 if no triangle was found                                        */
/* Ignores ctags !!                                                          */
/*                                                                           */
int exist_tri(ppt dpp1, ppt dpp2, ppt dpp3, tri_pt* pp) {
  struct tri_listdef* lp;
  int i;

  lp = dpp3->ntri_lp;
  while(lp!=null) {
    i = 0;
    while(i<3 && lp->tri_p->dppp[i]!=dpp1) i++;

    if(i!=3) {                                   
      if( lp->tri_p->dppp[(i+2)%3] == dpp2 ) {
	(*pp) = lp->tri_p;
	return -1;
      }  
      if( lp->tri_p->dppp[(i+1)%3] == dpp2 ) {
	(*pp) = lp->tri_p; 
	return 1;
      }  
    }
    /* not the same, next triangle                                           */
    lp = lp->ntri_lp;
  }
  
  return 0;
  
}



/*****************************************************************************/
/*                                                                           */
/*  exist_tri_getidx()                                                       */
/*                                                                           */
/*****************************************************************************/
/* Find an inserted triangle and return its pointer and relative orientation */
/* as well as the index of the third given point                             */
/* (depends on order of the three given points)                              */
/* returns 1 if the found triangle has the same orientation                  */
/*        -1                               mirrored orientation              */
/*         0 if no triangle was found                                        */
/* Ignores ctags !!                                                          */
/*                                                                           */
int exist_tri_getidx(ppt dpp1, ppt dpp2, ppt dpp3, tri_pt* pp, int* indexp) {
  struct tri_listdef* lp;
  int i;
  
  lp = dpp3->ntri_lp;
  while(lp!=null) {
    i = 0;
    while(i<3 && lp->tri_p->dppp[i]!=dpp1) i++;
    
    if(i!=3) {                                   
      if( lp->tri_p->dppp[(i+2)%3] == dpp2 ) {
	(*pp) = lp->tri_p;
	(*indexp) = (i+1)%3;
	return -1;
      }  
      if( lp->tri_p->dppp[(i+1)%3] == dpp2 ) {
	(*pp) = lp->tri_p; 
	(*indexp) = (i+2)%3;
	return 1;
      }  
    }
    /* not the same, next triangle                                           */
    lp = lp->ntri_lp;
  }

  return 0;

}



/*****************************************************************************/
/*                                                                           */
/*  exist_mrgadjtri()                                                        */
/*                                                                           */
/*****************************************************************************/
/* Looks for a triangle adjacent to the base triangle stored in structure    */
/* triangle_paramdef with which merging to create a tetrahedron makes sense. */
/* Checks lambda only for those points out of the plist.                     */
/* Therefore, all client data is needed.                                     */
/* Requires correct order of the two given points specifying the edge        */
/*                                                                           */
/* Must look for *all* such triangles and not only first to minimize the     */
/* angle (maximize the signed cosine) because under WSBREF two atris may     */
/* exist which have similar lambda and which would make a merge possible,    */
/* but lead to leaking without leaks                                         */
/*                                                                           */
/* returns -1   if no triangle was found                                     */
/* returns  k  the index of the point in the plist                           */
/*                                                                           */
/* Checks the ctags if a merge makes sense :                                 */
/* Looks only for a triangle adjacent to the given edge AND                  */
/* with orientation -1 and ctag&3!=3   or                                    */
/* with orientation  1 and ctag&4==0                    AND                  */
/* fulfilled L_SEPS                                     AND                  */
/* optimal angle                                                             */
/* (also finds backside-boundary faces which will produce a warning when     */
/* merged)                                                                   */
/* (does not check bit 'blocked' !)                                          */
/*									     */
/* If lambda calculation is not possible, will follow the same philosophy:   */
/* pretend that lambda is too big and do not consider a tetrahedron	     */
/* However, it is a more serious fault, because of the connected triangle !  */
/* Not treated as an error (dinfo) because VOL_EPS could be > 0.0	     */
/*									     */
/* Generally, does not exploit the possibility to detect leaks by checking   */
/* atris with points *not* in plist which might have better angles.          */
/* Such a case would clearly lead to an inconsistent topology but cannot be  */
/* avoided because double merges may occur. So it is better to *ALWAYS*      */
/* fulfill L_SEPS and follow the rule hierarchy. Then the worst thing that   */
/* can happen is a missed atri and leaking. This case is very unlikely       */
/* because such a triangle should not survive surface_refinement(). Such triangles     */
/* however DO survive surface_refinement() when cospherical points are involved.       */
/* So checking all atris with points of the plist should be within this      */
/* algorithm sufficient *and* necessary.                                     */
/*									     */
/* USED EPSILONS:							     */
/*  L_SEPS								     */
/*									     */
/* There may not be any call for expanding an array because of realloc and   */
/* the address stored in dppp !                                              */
/*									     */
int exist_mrgadjtri(mesh_p_t mesh_p, 
		    ppt dpp1, 
		    ppt dpp2, 
		    struct client_datadef* cli) {
  struct tri_listdef* lp;
  int i;
  int k;
  int i1;
  int i2;
  ppt* dppp = (ppt*) cli->lambda.plist.vp;
  double lam;
  struct vectordef ve, vh, vb;
  double h;
  double ccos;
  int best_k;

  /* two points must be in the correct order of the triangle                 */
  ve.x = dpp2->x - dpp1->x;
  ve.y = dpp2->y - dpp1->y;
  ve.z = dpp2->z - dpp1->z;

  /* maximize cosine of angle between triangles (use 'inconsistent' normals) */
  ccos = -2.0;
  best_k = -1;
  
  lp = dpp2->ntri_lp;
  while(lp!=null) {
    i = 0;
    while (i<3 && lp->tri_p->dppp[i]!=dpp1) i++;
    
    if(i!=3) {      
      i1 = (i+1)%3;
      i2 = (i+2)%3;
      for(k=0 ; k<cli->lambda.plist.n ; k++) {
	if(lp->tri_p->dppp[i2]==dppp[k]) { 
	  /* i  ... dpp1                                                     */
	  /* i1 ... dpp2                                                     */
	  /* i2 ... 3rd/k                                                    */
	  /* 'inconsistent' orientation                                      */
	  /* triangle identified, correct one ?                              */
	  if( (lp->tri_p->ctag&4) == 0 ) {
	    if(!calc_l(mesh_p, dppp[k], &cli->tri, &lam)) {
	      /* two adjacent triangles with 3. point of plist are coplanar ?*/
	      msg_dinfo("Found a coplanar adjacent triangle with third ");
	      msg_dinfo("point in a plist ??? \n");
	    }
	    else if( lam <= (cli->lambda.lam + (mesh_p->L_SEPS)) ) {
	      /* L_SEPS is okay so compare/memorize angle to find corr. tri! */
	      /* (instead of first only.. which leads to 0leaks + leaking!)  */
	      vb.x = dppp[k]->x - dpp1->x;  		      
	      vb.y = dppp[k]->y - dpp1->y;  		      
	      vb.z = dppp[k]->z - dpp1->z;  		      
	  
	      /* ve x vb                                                     */
	      vh.x = ve.y * vb.z - vb.y * ve.z;
	      vh.y = ve.z * vb.x - vb.z * ve.x;
	      vh.z = ve.x * vb.y - vb.x * ve.y;
	      h = sqrt(vh.x*vh.x + vh.y*vh.y + vh.z*vh.z); 	      
   									      
	      if(h != 0) {
		lam=(cli->tri.n.x*vh.x+cli->tri.n.y*vh.y+cli->tri.n.z*vh.z)/h;
		if(lam > ccos) {
		  /* maximize cosine                                         */
		  ccos = lam;
		  best_k = k;
		}
	      }
	      else {
		/* found collinear adjacent triangle                         */
		msg_dinfo("found coll. triangle during edge investigation \n");
		/* treat as if volume is 0 and lambda calculation impossible */
		/* (not a good atri even though lam okay, now angle not???   */
		/*  do not use it...rather find another one or use bestdpp?) */
	      }
	    }
	  }
	  /* POSSIBILITIES for this atri:                                    */
	  /* - ctag bad, lambda calc. imp., lambda value bad, or coll. tri   */
	  /* - atri ok but already one found with better ccos                */
	  /* - no better atri yet or first... best_k is set                  */
	  /* next triangle ! no more plist comparison                        */
	  break;
	}
	if(lp->tri_p->dppp[i1]==dppp[k]) { 
	  /* i  ... dpp1                                                     */
	  /* i1 ... 3rd/k                                                    */
	  /* i2 ... dpp2                                                     */
	  /* 'consistent' orientation                                        */
	  /* triangle identified, correct one ?                              */
	  if( (lp->tri_p->ctag&3) != 3 ) {
	    if(!calc_l(mesh_p, dppp[k], &cli->tri, &lam)) {
	      /* two adjacent triangles with 3. point of plist are coplanar ?*/
	      msg_dinfo("Found a coplanar adjacent triangle with third ");
	      msg_dinfo("point in a plist ??? \n");
	    }
	    else if( lam <= (cli->lambda.lam + (mesh_p->L_SEPS)) ) {
	      /* L_SEPS is okay so compare/memorize angle to find corr. tri! */
	      /* (instead of first only.. which leads to 0leaks + leaking!)  */
	      vb.x = dppp[k]->x - dpp1->x;  		      
	      vb.y = dppp[k]->y - dpp1->y;  		      
	      vb.z = dppp[k]->z - dpp1->z;  		      
	  
	      /* ve x vb                                                     */
	      vh.x = ve.y * vb.z - vb.y * ve.z;
	      vh.y = ve.z * vb.x - vb.z * ve.x;
	      vh.z = ve.x * vb.y - vb.x * ve.y;
	      h = sqrt(vh.x*vh.x + vh.y*vh.y + vh.z*vh.z); 	      
   									      
	      if(h != 0) {
		lam=(cli->tri.n.x*vh.x+cli->tri.n.y*vh.y+cli->tri.n.z*vh.z)/h;
		if(lam > ccos) {
		  /* maximize cosine                                         */
		  ccos = lam;
		  best_k = k;
		}
	      }
	      else {
		/* found collinear adjacent triangle                         */
		msg_dinfo("found coll. triangle during edge investigation \n");
		/* treat as if volume is 0 and lambda calculation impossible */
		/* (not a good atri even though lam okay, now angle not???   */
		/*  do not use it...rather find another one or use bestdpp?) */
	      }
	    }
	  }  
	  /* POSSIBILITIES for this atri:                                    */
	  /* - ctag bad, lambda calc. imp., lambda value bad, or coll. tri   */
	  /* - atri ok but already one found with better ccos                */
	  /* - no better atri yet or first... best_k is set                  */
	  /* next triangle ! no more plist comparison                        */
	  break; 
	}  
	/* For this atri the 3rd point has not yet been found in the plist   */
	/* next plist point                                                  */
      }
    }
    /* next triangle                                                         */
    lp = lp->ntri_lp;
  }
  /* no more atris to find                                                   */
  /* searched all, and picked best in case there was any                     */
  /* must go by angle, because it can be different for the same lambda values*/
  /* and it is the real topology test                                        */
  
  return(best_k); 
  
}



/*****************************************************************************/
/*                                                                           */
/*  investigate_edge()                                                       */
/*                                                                           */
/*****************************************************************************/
/* Looks for adjacent triangles to a given triangle edge.                    */
/* The edge is given by the index of its first point !!                      */
/* (differs from other functions)                                            */
/* Determines necessary information to detect structural edges               */
/* (used for edge swapping and building of the extended data structure       */
/*  during surface modeling)                                                 */
/*                                                                           */
/* Requires the triangle and an index specifying the edge.                   */
/* The *optionally* given normal vector (vn_p may be null) has to be         */
/* normalized and consistent with the orientation of the given triangle      */
/* For the *optionally* given slist it will be checked if an adjacent        */
/* triangle contains one of its points. (sl_p may be null !)                 */
/* It is necessary to include the slist handling in this function, because   */
/* of the search policy:                                                     */
/* Generally, the search for adjacent triangles terminates as soon as more   */
/* than one triangle is found (no lists of all adjacent triangles will be    */
/* generated). However, triangles with a 3rd point out of the given slist    */
/* are preferred and thus the search will be continued as long as only       */
/* triangles were found which do not hold a 3rd point out of the slist.      */
/* This is only of limited use, because e.g. for flipping and the flip_list  */
/* an atri with the point of the slist will be returned in any case if there */
/* is only one atri. Or it will not be needed anyhow because flipping is not */
/* possible if there were more atris.                                        */
/*                                                                           */
/* Returns:                                                                  */
/*   2       There were more than 1 atris                                    */
/*   1       There was only 1 atri                                           */
/*   0       There were no atris                                             */
/*   atri_p  pointer to one atri if possible with 3rd point out of slist     */
/*   ccos    cosine of angle between invtri and atri_p corrected with orient.*/
/*   a_idx   index of the 3rd point in the atri                              */
/*   l_idx   index of the 3rd point in the slist (-1 if none)                */
/*                                                                           */
/* (atri_p, ccos, and l_idx are always initialized whereas a_idx is only     */
/*  valid when atri_p != null !)                                             */
/*                                                                           */
/* If no slist is given or no atri exists with a 3rd point in it, l_idx will */
/*   be -1                                                                   */
/* If no normal vector was given (vn_p=null) no angles will be computed, and */
/*   ccos will be -2.0                                                       */
/* This can be important when no angle information is needed, or when the    */
/* checked triangle is nearly collinear and no normal vector exists while    */
/* edge swapping is still possible !                                         */
/* Therefore, the returned value is -2.0 which always allows flipping.       */
/* If the encountered adjacent triangle is collinear and its normal vector   */
/* equals zero, flipping is again possible because ccos will be set to -2.0  */
/* as well.                                                                  */
/*                                                                           */
/* ALSO SUPPORTS connected TETS (e.g. used for detect_glines)                */
/*                                                                           */
int investigate_edge(tri_pt invtri_p, 
		     int edge, 
		     struct vectordef* vn_p, 
		     struct arraydef* sl_p,
		     struct edge_infodef* ei_p) {
  
  int i;
  int index;
  int flag;
  double h;
  struct tri_listdef *lp;               
  ppt dpp0;
  ppt dpp1;
  ppt dpp3rd;
  struct vectordef ve, vb, vh;

  /* has to be in the correct order and consistent with vn_p                 */
  dpp0 = invtri_p->dppp[edge];
  dpp1 = invtri_p->dppp[(edge+1)%3];
  
  ve.x = dpp1->x - dpp0->x;
  ve.y = dpp1->y - dpp0->y;
  ve.z = dpp1->z - dpp0->z;

  ei_p->atri_p = null;
  ei_p->l_idx  = -1;
  ei_p->ccos   = -2.0;

  flag = 0;
  lp = dpp0->ntri_lp;
  while(lp!=null) {

    i = 0;
    while(i<3 && lp->tri_p->dppp[i] != dpp1) i++;

    /* excluding triangle itself                                             */
    if(i!=3 && lp->tri_p!=invtri_p) { 

      /** PROCESSING found adjacent triangle *********************************/

      index = (i+2)%3;
      if(lp->tri_p->dppp[index]==dpp0) {
	index = (i+1)%3;
      } 
      dpp3rd = lp->tri_p->dppp[index];

      if(ei_p->atri_p==null) { 
	/* FIRST adjacent triangle                                           */
	flag = 1;
	ei_p->atri_p = lp->tri_p; 
	ei_p->a_idx = index;

	/* checking slist                                                    */
	if(sl_p != null) {
	  i = 0;
	  while(i < sl_p->n  &&  dpp3rd != ((ppt*)sl_p->vp)[i]) i++;
	  if(i<sl_p->n) {
	    ei_p->l_idx = i;
	  } 
	}

	/* calculate angle between faces (not between normal vectors)        */
	if(vn_p!=null) {
	  vb.x = dpp3rd->x - dpp0->x;  		      
	  vb.y = dpp3rd->y - dpp0->y;  		      
	  vb.z = dpp3rd->z - dpp0->z;  		      
	  
	  /* ve x vb                                                         */
	  vh.x = ve.y * vb.z - vb.y * ve.z;  				     
	  vh.y = ve.z * vb.x - vb.z * ve.x;  				 
	  vh.z = ve.x * vb.y - vb.x * ve.y;  		            
	  h = sqrt(vh.x*vh.x + vh.y*vh.y + vh.z*vh.z); 	      
   									      
	  if(h != 0) {
	    ei_p->ccos = (vn_p->x*vh.x + vn_p->y*vh.y + vn_p->z*vh.z) / h;
	  }
	  else {
	    /* found collinear adjacent triangle                             */
	    msg_dinfo("found coll. triangle during edge investigation \n");
	  }
	}
      }
      else {     
	/* ANOTHER adjacent triangle found                                   */
	flag = 2;
	if(ei_p->l_idx != -1) {
	  /* more than 1 atri and already found one with 3rd point in slist  */
	  /* (a_idx not destroyed by index and ccos already computed)        */
	  return 2;
	}
	
	/* checking slist                                                    */
	if(sl_p == null) {
	  /* more than 1 atri and already found one with a_idx and ccos and  */
	  /* will never find one with 3rd point out of an empty slist        */
	  return 2;
	}
	i = 0;
	while(i < sl_p->n  &&  dpp3rd != ((ppt*)sl_p->vp)[i]) i++;
	if(i<sl_p->n) {
	  /* found one with 3rd point out of slist,                          */
	  /* update a_idx, l_idx, ccos, and atri_p                           */
	  ei_p->atri_p = lp->tri_p; 
	  ei_p->a_idx = index;
	  ei_p->l_idx = i;

	  /* calculate angle between faces (not between normal vectors)      */
	  if(vn_p!=null) {
	    vb.x = dpp3rd->x - dpp0->x;  		      
	    vb.y = dpp3rd->y - dpp0->y;  		      
	    vb.z = dpp3rd->z - dpp0->z;  		      
	    
	    /* ve x vb                                                       */
	    vh.x = ve.y * vb.z - vb.y * ve.z;  				     
	    vh.y = ve.z * vb.x - vb.z * ve.x;  				 
	    vh.z = ve.x * vb.y - vb.x * ve.y;  		            
	    h = sqrt(vh.x*vh.x + vh.y*vh.y + vh.z*vh.z); 	      
	    
	    if(h != 0) {
	      ei_p->ccos = (vn_p->x*vh.x + vn_p->y*vh.y + vn_p->z*vh.z) / h;
	    }
	    else {
	      /* found collinear adjacent triangle                           */
	      msg_dinfo("found coll. triangle during edge investigation \n");
	    }
	  }
	  return 2;
	}
	/* no triangle has been found till now with 3rd point out of slist,  */
	/* only 'normal' atris of which the first of all is stored in atri_p */
	/* with corresponding a_idx, ccos                                    */
	
      } /* end of another found atri                                         */
      
      /* Continue, because just first found or another normal one            */
      
    } /* end of processing found triangle                                    */
    
    /* next triangle                                                         */
    lp = lp->ntri_lp;
  }
  
  return flag;

} 




