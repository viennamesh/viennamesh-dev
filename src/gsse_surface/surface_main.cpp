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


// ===================================================================================


// recursion swap protection
//
#define FLIPREC_STAGE1   100
#define FLIPREC_PAD      50

static struct edge_infodef f_ei;            // edge info needed to swap triangles
static struct surf_clientdef f_sclient;     // static variable for sm_refflip/process_tris

int surface_recursive_swap(mesh_p_t mesh_p, tri_pt stacktri_p, tri_pt flip_tri_p) 
{
   int i;
   tri_pt oldatri_p;

   // check delaunay property / swap list
   //
   f_sclient.tri.tri_p = stacktri_p;
   i = check_del_tri(mesh_p, &f_sclient);

   if(i==1)  {   return 1; }   // already delaunay triangle
   if(i==2)  {   return 0; }   // treatment of a collinear triangle 

   if(f_sclient.flip_list.n == 0) return 0;    // non-delaunay triangle

   // edge loop
   //
   for(i=0 ; i<3 ; i++)  
   {
      if(investigate_edge(stacktri_p, i, &f_sclient.tri.n, &f_sclient.flip_list, &f_ei) == 1) 
      {
         // edge could be swapped (only one adjacent triangle)
         //
         if(f_ei.l_idx != -1) 
         {     
            // adjacent triangle has third point in common
            //
            if(f_ei.atri_p == flip_tri_p) 
            { 
               // do not flip the same triangle again
               //
               if(f_sclient.s1overall == 1) return 1;

            }
            else 
            {
               // check if it is a structural edge which cannot be flipped        
               //
               if(f_ei.ccos<=mesh_p->COSP_PAR && 
                  (stacktri_p->facemark==f_ei.atri_p->facemark ||
                   stacktri_p->facemark==~f_ei.atri_p->facemark) ) 
               {

                  if(sm_flip_tris(stacktri_p, f_ei.atri_p, ((i+2)%3), f_ei.a_idx)) 
                  {
                     // flipped triangles
                     //

                     //  recursion .. start
                     //
                     oldatri_p = f_ei.atri_p;
                     mesh_p->fliprecdepth++;
                     msg_dinfo1("%i ", mesh_p->fliprecdepth);
	      
                     if(mesh_p->fliprecdepth >= FLIPREC_STAGE1) 
                     {
                        if(mesh_p->fliprecdepth == FLIPREC_STAGE1) 
                        {
                           // first try
                           //
                           mesh_p->fliprectri_p = stacktri_p;
                           mesh_p->fliprecstate = 0;
                        }
                        if(mesh_p->fliprecdepth >= (FLIPREC_STAGE1 + FLIPREC_PAD)) 
                        {
                           if(mesh_p->fliprectri_p == stacktri_p && !mesh_p->fliprecstate) 
                           {
                              mesh_p->fliprecstate = mesh_p->fliprecdepth;
                           }
                           if(mesh_p->fliprecstate &&
                              mesh_p->fliprecdepth >= 
                              (mesh_p->fliprecstate + FLIPREC_PAD)) 
                           {
                              if(mesh_p->fliprectri_p == stacktri_p) 
                              {
                                 // break the endless swap loop
                                 //
                                 mesh_p->fliprec_endless = 1;
                                 i = 0;
                              }
                           }
		  
                           if(mesh_p->fliprecdepth > (mesh_p->gltr)*2) 
                           {
                              // endless swap loop 
                              //   end it here
                              //
                              mesh_p->fliprec_endless = 1;
                              i = 0;
                           }

                        }
                     }

                     //  to determine endless break
                     //
                     if(!mesh_p->fliprec_endless) 
                     {
                        i = surface_recursive_swap(mesh_p, oldatri_p, stacktri_p);
                     }

                     if(!mesh_p->fliprec_endless) 
                     {
                        i &= surface_recursive_swap(mesh_p, stacktri_p, oldatri_p);
                     }

                     mesh_p->fliprecdepth--;

                     if(mesh_p->fliprecdepth == 0) 
                     {
                        mesh_p->fliprec_endless = 0;
                     }
                     return i;
	      
                  }
               }
            }
         }
      }
   }
   // no more edges.. no delaunay triangle and no swapping
   //
   return 0;
  
}


// ===================================================================================


int surface_refinement(mesh_p_t mesh_p, tri_pt top_tri_p, int fm) 
{
   tri_pt stri_p;

   char c[4];
   FILE* fp;

   struct arraydef pclass_pool;

   int i;
   int pdeleted;
   int pinserted;

   int decicount;
   ppt decidpp;
   int deci;
   int threshold;
   int result;
  
   // init ..
   //
   pdeleted = mesh_p->global_octree.octree_counter;
   pinserted = 0;

   array_init(&pclass_pool, mesh_p->gltr+1, sizeof(struct point_classdef));

// reset *all* the point indices just in case                              */
//   ext_find(&mesh_p->global_octree.bbminimum, 
// 	   &mesh_p->global_octree.bbmaximum, 
//            cb_reset_index,
// 	   null, 
// 	   &mesh_p->global_octree,
// 	   null);



   // detect structural edges
   // [IMPORTANT]    ultra fast option  for gsse::vgmodeler
   //
   detect_glines(mesh_p, &pclass_pool, top_tri_p, fm, 0);

   // process structural edges
   //
   process_glines(mesh_p, &pclass_pool);


   // first swap iteration
   //
   for(stri_p=top_tri_p->ntri_p ;  stri_p!=null ;   stri_p=stri_p->ntri_p) 
   {
      if(fm != ~init_facemark) 
      {
         if(stri_p->facemark != fm && stri_p->facemark != ~fm) continue;
      } 
      surface_recursive_swap(mesh_p, stri_p, null);
   } 


   // refinement    [RH] [!!] .. here .. I have deleted a part .. may be important?? not right now
   //
   for(threshold=0 ; threshold<12 ; threshold++) 
   {
      result = 0;
      process_tris(mesh_p, &pclass_pool,  top_tri_p, fm,  &result);
      if(!result) break;
   }

   mesh_p->leaktris.n = 0;
   for(stri_p=top_tri_p->ntri_p ;  stri_p!=null ;  stri_p=stri_p->ntri_p) 
   {
      if(fm != ~init_facemark) 
      {
         if(stri_p->facemark != fm && stri_p->facemark != ~fm) continue;
      } 
      if(!surface_recursive_swap(mesh_p, stri_p, null)) 
      {
         stri_p->ctag |= CTG_SPECIAL;
         if(mesh_p->leaktris.n == mesh_p->leaktris.max) 
            expand_ar(&mesh_p->leaktris, D_AR);
         ((tri_pt*)(mesh_p->leaktris.vp))[mesh_p->leaktris.n] = stri_p;
         mesh_p->leaktris.n++;
      }
      else stri_p->ctag &= (CTG_BITMAX - CTG_SPECIAL);
   }


   // finished 
   //
   for(i=0 ; i<pclass_pool.n ; i++) 
   {
      free_pclass( ((pcl_pt)pclass_pool.vp)[i] );
   }
   free(pclass_pool.vp);


   if(mesh_p->leaktris.n > 0) 
   {
      if(top_tri_p!=&mesh_p->grid.unified_top_tri || fm!=~init_facemark) 
      {
         msg_info("Non-Delaunay elements left on this surface patch for now\n");
      }
      else 
      {
         msg_warn("+WARNING+: Non-Delaunay surface elements are left (leaks)\n");
      }
   }

   return 0;

}


// ===================================================================================


// detect structural edges
//
// used:  COSP_PAR, H_EPS
//

#define PTG_HOLE    1                 /* point part of an edge with only 1 atri */
#define H_EPS      (10*DBL_EPSILON)

void detect_glines(mesh_p_t mesh_p,  struct arraydef* pclspool_p, tri_pt top_tri_p, int fm, int and_ctag) 
{
   tri_pt stri_p;
   struct edge_infodef edgi;
   pcl_pt pcl_p;
   int i;
   int e;
   int hflag;
   int sflag;
   ppt dpp1;
   ppt dpp2;
   struct vectordef vn;
   struct vectordef va;
   struct vectordef vb;
   double h;


   // reset of the point class
   //
   for(i=0 ; i<pclspool_p->n ; i++) 
   {
      free_pclass( ((pcl_pt)(pclspool_p->vp))[i] );
   }
   pclspool_p->n = 0;

   // reseting the ctg_edge bits
   //
   for(stri_p=top_tri_p->ntri_p ; stri_p!=null ; stri_p=stri_p->ntri_p) 
   {
      stri_p->ctag &= CTG_BITMAX - (CTG_EDGE0 + CTG_EDGE1 + CTG_EDGE2);
   }

   // [!!] BUG ?? 
   //
   for(stri_p=top_tri_p->ntri_p ; stri_p!=null ; stri_p=stri_p->ntri_p) 
   {
      if(fm != ~init_facemark) {
         if(stri_p->facemark != fm && stri_p->facemark != ~fm) continue;
      } 
      if(and_ctag >0) {
         if( (stri_p->ctag&and_ctag)==0) continue;
      }    

      if( (stri_p->ctag&(CTG_EDGE0 + CTG_EDGE1 + CTG_EDGE2)) == (CTG_EDGE0 + CTG_EDGE1 + CTG_EDGE2) ) 
      {
         stri_p->ctag &= CTG_BITMAX - (CTG_EDGE0 + CTG_EDGE1 + CTG_EDGE2);  // reset the CTG_EDGE bit
         continue;
      }
    
      va.x = stri_p->dppp[1]->x - stri_p->dppp[0]->x;
      va.y = stri_p->dppp[1]->y - stri_p->dppp[0]->y;
      va.z = stri_p->dppp[1]->z - stri_p->dppp[0]->z;

      vb.x = stri_p->dppp[2]->x - stri_p->dppp[0]->x;
      vb.y = stri_p->dppp[2]->y - stri_p->dppp[0]->y;
      vb.z = stri_p->dppp[2]->z - stri_p->dppp[0]->z;

      vn.x = va.y * vb.z - va.z * vb.y;
      vn.y = va.z * vb.x - va.x * vb.z;
      vn.z = va.x * vb.y - va.y * vb.x;

      h = sqrt(vn.x*vn.x + vn.y*vn.y + vn.z*vn.z);
      if(h<=H_EPS) 
      {
         if(stri_p->ftet_p==null && stri_p->btet_p==null) 
         {
            msg_warn("+WARNING+: Detected a collinear triangle \n");
            msg_warn("+WARNING+: Skipping coll. triangle in detect glines. \n");
            continue;
         }
         // modify result.. will be a stored edge
         //
         h = 2*H_EPS;
      }
      h = 1.0 / h;
      vn.x *= h;
      vn.y *= h;
      vn.z *= h;

      // check edges
      //
      for(e=0 ; e<3 ; e++) 
      {
         if( (stri_p->ctag & (CTG_EDGE0<<e)) )   // already checked here .. reset CTG_EDGE bits
         {
            stri_p->ctag &= CTG_BITMAX - (CTG_EDGE0<<e);
            continue;
         }
      
         dpp1 = stri_p->dppp[e];
         dpp2 = stri_p->dppp[(e+1)%3];
         sflag = 0;
         hflag = 0;
         i = investigate_edge(stri_p, e, &vn, null, &edgi);

         // zero adjacent triangles
         //
         if(i == 0)    //structural edge
         {
            sflag = 1;
            hflag = PTG_HOLE;
         }

         // one adjacent triangle
         else if(i == 1) 
         {
            if(edgi.ccos>mesh_p->COSP_PAR ||  
               (edgi.atri_p->facemark!=stri_p->facemark &&  edgi.atri_p->facemark!=~stri_p->facemark) ||    
               stri_p->ftet_p!=null || stri_p->btet_p!=null )   //check for structural edges
            {
               sflag = 1;    // detect colinear adjacent triangles
            }
            edgi.atri_p->ctag |= ( CTG_EDGE0 << ((edgi.a_idx+1)%3) );   //update adjacent triangles
         }
         // more adjacent triangles
         //
         else 
         {
            sflag = 1;
            if(dpp1->point_index != init_point_index)    
            {
               pcl_p = ((pcl_pt) pclspool_p->vp) + dpp1->point_index;
               for(i=0 ; i<pcl_p->pri_gline.n ; i++) 
               {
                  if( ((ppt*)(pcl_p->pri_gline.vp))[i] == dpp2 ) 
                  {
                     sflag = 0;   // point exists.. 
                     break;
                  }
               }
               if(!sflag) continue; // next edge
               for(i=0 ; i<pcl_p->sec_gline.n ; i++) 
               {
                  if( ((ppt*)(pcl_p->sec_gline.vp))[i] == dpp2 ) 
                  {
                     sflag = 0;   // point exists
                     break;
                  }
               }
            }
         }

         if(sflag)  // update point class 
         {

            // first point
            //
            if(dpp1->point_index == init_point_index) 
            {
               if(pclspool_p->n == pclspool_p->max) 
               {
                  expand_ar(pclspool_p, D_AR);
               }
               init_pclass( (((pcl_pt)pclspool_p->vp)[pclspool_p->n]),  dpp1, pclspool_p->n);
               pclspool_p->n++;

            }

            pcl_p = ((pcl_pt) pclspool_p->vp) + dpp1->point_index;
            pcl_p->ptype |= hflag;
            if(pcl_p->pri_gline.n == pcl_p->pri_gline.max) 
            {
               expand_ar(&pcl_p->pri_gline, 2);
            }
            ((ppt*)(pcl_p->pri_gline.vp))[pcl_p->pri_gline.n] = dpp2;
            pcl_p->pri_gline.n++;


            // second point
            //
            if(dpp2->point_index == init_point_index) 
            {
               if(pclspool_p->n == pclspool_p->max) {
                  expand_ar(pclspool_p, D_AR);
               }
               init_pclass( (((pcl_pt)pclspool_p->vp)[pclspool_p->n]),
                            dpp2, pclspool_p->n);
               pclspool_p->n++;

            }

            pcl_p = ((pcl_pt) pclspool_p->vp) + dpp2->point_index;
            pcl_p->ptype |= hflag;
            if(pcl_p->sec_gline.n == pcl_p->sec_gline.max) 
            {
               expand_ar(&pcl_p->sec_gline, 2);
            }
            ((ppt*)(pcl_p->sec_gline.vp))[pcl_p->sec_gline.n] = dpp1;
            pcl_p->sec_gline.n++;
         }
      }
   }
}


// process structural edges
//
// used param: MINL_PAR
//
int process_glines(mesh_p_t mesh_p, struct arraydef* pclspool_p) {
   ppt dpp1;
   ppt dpp2;
   ppt commondpp;
   ppt otherdpp;
   ppt ref_dpp;
   int pri_index;
   int sec_index;
   ppt distdpp;
   double d;
   int i;
   int l;
   int j;
   int flag_inserted;
   int flag_hitlimit;
   int rep;
   int spts;
   pcl_pt pcl_p;
   pcl_pt pcl2_p;
   pcl_pt distpcl_p;
   struct vectordef va;
   struct vectordef ve;
   struct vectordef vR;
   double e;
   double x;
   double a;
   double halfMINL_PAR;
   double cacheMINL_PAR;

   ppt dppout;

   // process all structural edges
   //   .. possible MINL_PAR criterion
   //
   halfMINL_PAR = 0.5 * mesh_p->MINL_PAR;
   cacheMINL_PAR = mesh_p->MINL_PAR;
   spts = 0;
   for(rep=0 ; ; rep++) 
   {
      flag_inserted = 0;
      flag_hitlimit = 0;

      for(i=0 ; i<pclspool_p->n ; i++) 
      {
         pcl_p = ((pcl_pt)pclspool_p->vp) + i;
         dpp1 = pcl_p->dpp;

         for(l=0 ; l < pcl_p->pri_gline.n ; l++) 
         {
            dpp2 = ((ppt*)(pcl_p->pri_gline.vp))[l];
	    
	    // [FS] .. checks edge for delaunay property - returns closest point
            distdpp = check_del_edge(mesh_p, dpp1, dpp2, &d);  // structural edge: dpp1, dpp2
	    
            // check for smallest sphere criterion
            //
            if(distdpp == null) continue;  // next edge

	    // [FS]
#ifdef DEBUG
	    printf("FS .. dpp1: %f :: %f :: %f \n", dpp1->x, dpp1->y, dpp1->z);
	    printf("FS .. dpp2: %f :: %f :: %f \n", dpp2->x, dpp2->y, dpp2->z);
	    printf("FS .. distpoint: %f :: %f :: %f \n", distdpp->x, distdpp->y, distdpp->z);
#endif
            // check for minimum edge length (MINL_PAR)
            //
            ve.x = dpp2->x - dpp1->x;
            ve.y = dpp2->y - dpp1->y;
            ve.z = dpp2->z - dpp1->z;
            e = sqrt(ve.x*ve.x + ve.y*ve.y + ve.z*ve.z);
            if(e < (2*cacheMINL_PAR))    // no refinement possible.. short edge
            {
               flag_hitlimit = 1;
               continue;
            }

            // here, the type of refinement (see http://www.iue.tuwien.ac.at/phd/fleischmann
            //  .. projection
            //  .. rotation
            //
            commondpp = null;
            otherdpp = null;
            pri_index = init_point_index;
            sec_index = init_point_index;

            if(distdpp->point_index != init_point_index) 
            {
               
               distpcl_p = ((pcl_pt)pclspool_p->vp) + distdpp->point_index;
               /* Locate any of dpp1 or dpp2 in glines of distdpp                 */

               /* searching for dpp1                                              */
               for(j=0 ; j<distpcl_p->pri_gline.n ; j++) {
                  if( ((ppt*)(distpcl_p->pri_gline.vp))[j] == dpp1 ) {
                     /* found dpp1 in primary list                                  */
                     commondpp = dpp1;
                     otherdpp = dpp2;
                     pri_index = j;
                     break;
                  }
               }
               if(commondpp == null) {
                  for(j=0 ; j<distpcl_p->sec_gline.n ; j++) {
                     if( ((ppt*)(distpcl_p->sec_gline.vp))[j] == dpp1 ) {
                        /* found dpp1 in secondary list                              */
                        commondpp = dpp1;
                        otherdpp = dpp2;
                        sec_index = j;
                        break;
                     }
                  }
               }
               /* searching for dpp2                                              */
               for(j=0 ; j<distpcl_p->pri_gline.n ; j++) {
                  if( ((ppt*)(distpcl_p->pri_gline.vp))[j] == dpp2 ) {
                     if(commondpp == null)    // no dpp1/dpp2 -> rotation
                     {
		       // [FS]
#ifdef DEBUG
		       printf("FS .. in search for dpp2..");
#endif

                        commondpp = dpp2;
                        otherdpp = dpp1;
                        pri_index = j;
                     }
                     else    // -> projection
                     {
                        otherdpp = null;
                     }
                     break;
                  }
               }
               
               // four possible states:
               //    common == null, other == null:  look for dpp2 in secondary
               //    common == dpp1, other == null:  found both -> PROJECTION       
               //    common == dpp1, other != null:  look for dpp2 in secondary
               //    common == dpp2, other != null:  found only dpp2 -> ROTATION    
               //
               if(commondpp == null && otherdpp == null) 
               {
                  for(j=0 ; j<distpcl_p->sec_gline.n ; j++) 
                  {
                     if( ((ppt*)(distpcl_p->sec_gline.vp))[j] == dpp2 )   // no dpp1/dpp2 -> rotation
                     { 
                        commondpp = dpp2;
                        otherdpp = dpp1;
                        sec_index = j;
                        break;
                     }
                  }
               }
               else if(commondpp == dpp1 && otherdpp != null) 
               {
                  for(j=0 ; j<distpcl_p->sec_gline.n ; j++) 
                  {
                     if( ((ppt*)(distpcl_p->sec_gline.vp))[j] == dpp2 )    // both points -> projection
                     {
                        otherdpp = null;
                        break;
                     }
                  }
               }
            }
            // four possible states:
            //    common == null, other == null:  none             -> PROJECTION       
            //    common == dpp1, other == null:  found both       -> PROJECTION       
            //    common == dpp1, other != null:  only dpp1        -> ROTATION
            //    common == dpp2, other != null:  only dpp2         -> ROTATION    
            //
            // .. refinement point calculation
            //
            create_point(ref_dpp);

#ifdef DEBUG
	    printf("\n");
	    printf("FS .. new point before projection/rotation: %f :: %f :: %f \n", ref_dpp->x, ref_dpp->y, ref_dpp->z);
#endif
            if(otherdpp == null)    // projection
            {
#ifdef DEBUG
	      printf("FS ..in projection ");
#endif

               va.x = distdpp->x - dpp1->x;
               va.y = distdpp->y - dpp1->y;
               va.z = distdpp->z - dpp1->z;
               x = (va.x * ve.x + va.y * ve.y + va.z * ve.z) / e;
	  
               // PR CASE I
               if(x>=cacheMINL_PAR && x<=(e - cacheMINL_PAR)) 
               { 
                  x = x/e;
                  ref_dpp->x = dpp1->x + x*ve.x;
                  ref_dpp->y = dpp1->y + x*ve.y;
                  ref_dpp->z = dpp1->z + x*ve.z;
               }
               // P CASE IIa  
               else if(x<cacheMINL_PAR) 
               {
                  x = cacheMINL_PAR / e;
                  ref_dpp->x = dpp1->x + x*ve.x;
                  ref_dpp->y = dpp1->y + x*ve.y;
                  ref_dpp->z = dpp1->z + x*ve.z;

                  if(d <= halfMINL_PAR) 
                  {
                     vR.x = distdpp->x - 0.5 * (ref_dpp->x + dpp1->x);
                     vR.y = distdpp->y - 0.5 * (ref_dpp->y + dpp1->y);
                     vR.z = distdpp->z - 0.5 * (ref_dpp->z + dpp1->z);
                     if(sqrt(vR.x*vR.x + vR.y*vR.y + vR.z*vR.z) <= halfMINL_PAR) 
                     {
                        erase_point(ref_dpp);
                        flag_hitlimit = 1;
                        msg_dinfo("No refinement (sphere test failed)\n");
                        continue; 
                     }
                  }		
               }
               // P CASE IIb
               else 
               {
                  x = (e - cacheMINL_PAR) / e;
                  ref_dpp->x = dpp1->x + x*ve.x;
                  ref_dpp->y = dpp1->y + x*ve.y;
                  ref_dpp->z = dpp1->z + x*ve.z;

                  if(d <= halfMINL_PAR) 
                  {
                     vR.x = distdpp->x - 0.5 * (ref_dpp->x + dpp2->x);
                     vR.y = distdpp->y - 0.5 * (ref_dpp->y + dpp2->y);
                     vR.z = distdpp->z - 0.5 * (ref_dpp->z + dpp2->z);
                     if(sqrt(vR.x*vR.x + vR.y*vR.y + vR.z*vR.z) <= halfMINL_PAR) 
                     {
                        erase_point(ref_dpp);
                        flag_hitlimit = 1;
                        msg_dinfo("No refinement (sphere test failed)\n");
                        continue; 
                     }
                  }		
               }
            }
            else    // ROTATION
            {
	      // [FS]
#ifdef DEBUG
	      printf("FS .. rotation \n");
#endif
               va.x = distdpp->x - commondpp->x;
               va.y = distdpp->y - commondpp->y;
               va.z = distdpp->z - commondpp->z;
               ve.x = otherdpp->x - commondpp->x;
               ve.y = otherdpp->y - commondpp->y;
               ve.z = otherdpp->z - commondpp->z;
	  
               x = sqrt(va.x*va.x + va.y*va.y + va.z*va.z);

#ifdef DEBUG	       
	       printf("FS .. commondpp: %f :: %f :: %f \n", commondpp->x, commondpp->y, commondpp->z);
	       printf("FS .. otherdpp : %f :: %f :: %f \n", otherdpp->x, otherdpp->y, otherdpp->z);
	       printf("FS .. e - cacheMINL_PAR: %f \n", e - cacheMINL_PAR);
#endif

               // R CASE I
               if(x>=cacheMINL_PAR && x<=(e - cacheMINL_PAR)) 
               { 
#ifdef DEBUG
		 printf("FS .. case 1 \n");
		  printf("FS .. e: %f \n", e);
		  printf("FS .. x before x/e: %f \n", x);
#endif
                  x = x/e;

#ifdef DEBUG
		  printf("FS .. x after x/e : %f \n", x);
#endif
		  
		  // [FS] .. TEMP FIX
		  //
		  if(x > 0.9)
		    {
		      ref_dpp->x = otherdpp->x;
		      ref_dpp->y = otherdpp->y;
		      ref_dpp->z = otherdpp->z;		      
		    }
		  else
		    {
		      ref_dpp->x = commondpp->x + x*ve.x;
		      ref_dpp->y = commondpp->y + x*ve.y;
		      ref_dpp->z = commondpp->z + x*ve.z;
		    }
               }
               // R CASE II
               else if(x<cacheMINL_PAR) 
               {
#ifdef DEBUG
		 printf("FS .. case 2 \n");
#endif

                  x = cacheMINL_PAR / e;
                  ref_dpp->x = commondpp->x + x*ve.x;
                  ref_dpp->y = commondpp->y + x*ve.y;
                  ref_dpp->z = commondpp->z + x*ve.z;
               
                  if(d <= halfMINL_PAR) 
                  {
                     vR.x = distdpp->x - 0.5 * (ref_dpp->x + commondpp->x);
                     vR.y = distdpp->y - 0.5 * (ref_dpp->y + commondpp->y);
                     vR.z = distdpp->z - 0.5 * (ref_dpp->z + commondpp->z);
                     if(sqrt(vR.x*vR.x + vR.y*vR.y + vR.z*vR.z) <= halfMINL_PAR) 
                     {
                        erase_point(ref_dpp);
                        flag_hitlimit = 1;
                        msg_dinfo("No refinement (sphere test failed)\n");
                        continue; 
                     }
                  }		
               }

               // R CASE III
               else if(x > 2*cacheMINL_PAR) 
               {
#ifdef DEBUG
		 printf("FS .. case 3 \n");
#endif

                  a = 0.5 * (e - x + cacheMINL_PAR);
                  x = (x - cacheMINL_PAR) / e;
                  ref_dpp->x = commondpp->x + x*ve.x;
                  ref_dpp->y = commondpp->y + x*ve.y;
                  ref_dpp->z = commondpp->z + x*ve.z;

                  if(d <= a) 
                  {
                     vR.x = distdpp->x - 0.5 * (ref_dpp->x + otherdpp->x);
                     vR.y = distdpp->y - 0.5 * (ref_dpp->y + otherdpp->y);
                     vR.z = distdpp->z - 0.5 * (ref_dpp->z + otherdpp->z);
                     if(sqrt(vR.x*vR.x + vR.y*vR.y + vR.z*vR.z) <= a) 
                     {
                        erase_point(ref_dpp);
                        flag_hitlimit = 1;
                        msg_dinfo("No refinement (sphere test failed)\n");
                        continue; 
                     }
                  }		
               }
               // R CASE IV
               else 
               {
#ifdef DEBUG
		 printf("FS .. case 4 \n");
#endif

                  a = x;
                  x = (e - cacheMINL_PAR) / e;
                  ref_dpp->x = commondpp->x + x*ve.x;
                  ref_dpp->y = commondpp->y + x*ve.y;
                  ref_dpp->z = commondpp->z + x*ve.z;

                  if(d <= halfMINL_PAR) 
                  {
                     vR.x = distdpp->x - 0.5 * (ref_dpp->x + otherdpp->x);
                     vR.y = distdpp->y - 0.5 * (ref_dpp->y + otherdpp->y);
                     vR.z = distdpp->z - 0.5 * (ref_dpp->z + otherdpp->z);
                     if(sqrt(vR.x*vR.x + vR.y*vR.y + vR.z*vR.z) <= halfMINL_PAR) 
                     {
                        erase_point(ref_dpp);
                        flag_hitlimit = 1;
                        msg_dinfo("No refinement (sphere test failed)\n");
                        continue; 
                     }
                  }		

                  vR.x = ref_dpp->x - 0.5 * (distdpp->x + commondpp->x);
                  vR.y = ref_dpp->y - 0.5 * (distdpp->y + commondpp->y);
                  vR.z = ref_dpp->z - 0.5 * (distdpp->z + commondpp->z);
                  if(sqrt(vR.x*vR.x + vR.y*vR.y + vR.z*vR.z) <= 0.5*a) 
                  {
                     erase_point(ref_dpp);
                     flag_hitlimit = 1;
                     msg_dinfo("No refinement (sphere test failed)\n");
                     continue; 
                  }
               }
            }
            // new point is calculated and inserted
            //
	    // [FS]
#ifdef DEBUG
	    printf("FS .. new point after projection/rotation: %f :: %f :: %f \n", ref_dpp->x, ref_dpp->y, ref_dpp->z);
#endif
	    
            if((dppout=oct_insert_point(ref_dpp, &mesh_p->global_octree))!=null) 
            {
               if(dppout->ntri_lp==null) 
               {
                  dppout->x = ref_dpp->x;
                  dppout->y = ref_dpp->y;
                  dppout->z = ref_dpp->z;

                  erase_point(ref_dpp);
                  ref_dpp = dppout;
               }
               else 
               {
                  msg_warn("+WARNING+: Not enough OCTRES for S-Edge refinement\n");
                  erase_point(ref_dpp);
                  flag_hitlimit = 1;
                  continue; 
               }
            }
            else    // check numerics
            {
               snap_clear(mesh_p, mesh_p->MINSNAP_PAR, ref_dpp);
            }
            spts++;
            msg_dinfo1("This is the %i. S-Edge refinement point\n", spts);
            flag_inserted = 1;
	
            mesh_p->gltr += sm_ref_edge(dpp1, dpp2, ref_dpp, &mesh_p->lasttri_p); 

            if(pclspool_p->n == pclspool_p->max) 
            {
               expand_ar(pclspool_p, mesh_p->gltr);
               pcl_p = ((pcl_pt)pclspool_p->vp) + i;
            }
            init_pclass((((pcl_pt)pclspool_p->vp)[pclspool_p->n]), ref_dpp, pclspool_p->n);
            pclspool_p->n++;
            j = 0;
            pcl2_p = ((pcl_pt)pclspool_p->vp) + dpp2->point_index;
            while( ((ppt*)(pcl2_p->sec_gline.vp))[j] != dpp1 &&
                   j < pcl2_p->sec_gline.n ) j++;
            if(j == pcl2_p->sec_gline.n) 
            {
               msg_fatal("+FATAL+: The point classes are not consistent \n");
               exit(0);
            }

            // update
            //
            ((ppt*)(pcl_p->pri_gline.vp))[l] = ref_dpp;
            ((ppt*)(pcl2_p->sec_gline.vp))[j] = ref_dpp;
            pcl2_p = ((pcl_pt)pclspool_p->vp) + ref_dpp->point_index;
            ((ppt*)(pcl2_p->pri_gline.vp))[0] = dpp2;
            ((ppt*)(pcl2_p->sec_gline.vp))[0] = dpp1;
            pcl2_p->pri_gline.n++;
            pcl2_p->sec_gline.n++;

            l--;
         }
      }

      if(flag_inserted) 
      {
         msg_dinfo1("REPETITION NUMBER                   %i\n\n", rep+1);
         continue;
      }

      if(flag_hitlimit) 
      {
         msg_warn("+WARNING+: Finished with gline processing but reached bounds\n");
      }
      else 
      {
         msg_info("Successfully processed all glines !\n");
      }

      msg_info1("There were %i refinement points \n", spts);

      break;

   }

   return(spts);

}

// process the triangles
//
//  USED EPSILON:
//    MINN2_EPS
//    H_EPS	
//
// USED PARAMETERS:
//   SDEL_PAR
//   MAXCOS_PAR
//   MAXAR2_PAR
//   MINL_PAR	
//
int process_tris(mesh_p_t mesh_p, struct arraydef* pclspool_p, tri_pt top_tri_p, int fm, int* ret_p) 
{
  tri_pt stri_p;
   int rep;
   int pts;
   int i;
   int ind;
   int flag_inserted;
   int flag_hitlimit;
   int flag_doglines;
   struct vectordef vdist;
   ppt ref_dpp;
   ppt distdpp;
   tri_pt foundtri_p;
   struct vectordef vH;
   ppt dpp1, dpp2;
   struct tri_listdef* lp;
   struct arraydef newtris;
   char c[4];
   int flag_poverride;
   int flag_steiner;
   int flag_quality;
   int thr;
   double d;

   ppt dppout;

   array_init(&newtris, D_AR, sizeof(tri_pt));

   pts = 0;
   thr = 0; 
   *ret_p = 0;   // do not repeate process glines
   for(rep=0 ; ; rep++) 
   {

      flag_inserted = 0;
      flag_doglines = 0;
      flag_hitlimit = 0;
    
      for(stri_p=top_tri_p->ntri_p ; stri_p!=null ; stri_p=stri_p->ntri_p) 
      {
         //  check the triangle for flipping
         //

         if(surface_recursive_swap(mesh_p, stri_p, null)) 
         {
            // DELAUNAY
            //
            stri_p->ctag &= (CTG_BITMAX - CTG_SPECIAL);
	
            if(mesh_p->MAXAR2_PAR != 0.0 || mesh_p->MAXCOS_PAR != 1.0) 
            {
               if(check_quality_tri(mesh_p, &f_sclient.tri)) continue;
               else flag_quality = 1;
            }
            else continue;
         }	
         else {
            // NOT DELAUNAY
            // 
	   ppt p0, p1, p2;
	   p0 = stri_p->dppp[0];
	   p1 = stri_p->dppp[1];
	   p2 = stri_p->dppp[2];

#ifdef DEBUG
	   printf("FS .. not delaunay point0: %f :: %f :: %f ", p0->x, p1->y, p2->z);
	   printf("FS .. not delaunay point1: %f :: %f :: %f ", p0->x, p1->y, p2->z);
	   printf("FS .. not delaunay point2: %f :: %f :: %f ", p0->x, p1->y, p2->z);
#endif

            stri_p->ctag |= CTG_SPECIAL;
            flag_quality = 0;
         }

         if(f_sclient.tri.tri_p!=stri_p || f_sclient.validforthistri==0) 
         {
            msg_warn("+WARNING+: Collinear triangle invalidates data? \n");
            continue;
         }

         distdpp = null;
         flag_poverride = 0;
      
         if(!flag_quality)    // check of point can be deleted
         {
            if(f_sclient.cneps_list.n != 0) 
            {
               distdpp = ((ppt*)f_sclient.cneps_list.vp)[0];
               flag_poverride = 1;
            }	
            else if(f_sclient.uneps_list.n != 0 &&  (mesh_p->SDEL_PAR < mesh_p->N_EPS)) 
            {
               // not delete points
               distdpp = ((ppt*)f_sclient.uneps_list.vp)[0];
               flag_poverride = 1;
            }
            else 
            {
               if(f_sclient.Fbestdpp==null && f_sclient.Bbestdpp==null) 
               {
                  for(i=0 ; i<f_sclient.uneps_list.n ; i++) 
                  {
                     ext_del(((ppt*)f_sclient.uneps_list.vp)[i],   
                             ((ppt*)f_sclient.uneps_list.vp)[i],          
                             cb_erase_point, 
                             null,
                             &mesh_p->global_octree,
                             null);   
                  }                                                          
                  stri_p = stri_p->ptri_p;
                  continue;
               }

               // CAUTION: originally d can be negative for backside! (now always>0)
               if(f_sclient.Fbestdpp!=null && f_sclient.Bbestdpp!=null) 
               {
                  // select the approprioate point
                  if(f_sclient.Flam <= -(f_sclient.Blam) ) 
                  {
                     distdpp = f_sclient.Fbestdpp;
                     if(f_sclient.Fbestd <= mesh_p->SPROJ_PAR) flag_poverride = 1;
                  }
                  else 
                  {
                     distdpp = f_sclient.Bbestdpp;
                     if(f_sclient.Bbestd <= mesh_p->SPROJ_PAR) flag_poverride = 1;
                  }
               }
               else 
               {
                  if(f_sclient.Fbestdpp != null) 
                  {
                     distdpp = f_sclient.Fbestdpp;
                     if(f_sclient.Fbestd <= mesh_p->SPROJ_PAR) flag_poverride = 1;
                  }
                  else 
                  {
                     distdpp = f_sclient.Bbestdpp;
                     if(f_sclient.Bbestd <= mesh_p->SPROJ_PAR) flag_poverride = 1;
                  }
               }
               if(distdpp->ntri_lp == null) 
               {
                  for(i=0 ; i<f_sclient.dpar_list.n ; i++) 
                  {
                     if( ((ppt*)f_sclient.dpar_list.vp)[i] == distdpp ) 
                     {
                        break;
                     }
                  }
                  if(i<f_sclient.dpar_list.n) 
                  {
                     for(i=0 ; i<f_sclient.dpar_list.n ; i++) 
                     {
                        ext_del(((ppt*)f_sclient.dpar_list.vp)[i],   
                                ((ppt*)f_sclient.dpar_list.vp)[i],          
                                cb_erase_point, 
                                null,
                                &mesh_p->global_octree,
                                null);   
                     }                                                          
                     stri_p = stri_p->ptri_p;
                     continue;
                  }
               }
            }
            vdist.x = distdpp->x;
            vdist.y = distdpp->y;
            vdist.z = distdpp->z;
         }

         // [!!] complex area.. check this again
         //
         flag_steiner = 0;
         if((f_sclient.s1overall>thr && flag_poverride==0) || flag_quality) 
         {
            vdist.x = f_sclient.tri.H.x;
            vdist.y = f_sclient.tri.H.y;
            vdist.z = f_sclient.tri.H.z;
            flag_steiner = 1;
         }

         if(f_sclient.tri.lp1h < mesh_p->MINL_PAR) 
         {
            msg_dinfo("Triangle too small to be refined even more \n");
            if(!flag_quality) 
               flag_hitlimit = 1;  // non delaunay triangle cannot be refined

            continue;
         }

         // possible correction : real point or location       
         //
         if(flag_steiner) 
         {
            foundtri_p = sm_locate_tri(mesh_p,
                                       pclspool_p,
                                       f_sclient.tri.tri_p,
                                       &vdist,
                                       &f_sclient.tri.n,
                                       &ind, 
                                       null);
         }
         else 
         {
            foundtri_p = sm_locate_tri(mesh_p,
                                       pclspool_p,
                                       f_sclient.tri.tri_p,
                                       &vdist, 
                                       &f_sclient.tri.n,
                                       &ind, 
                                       distdpp);
         }	
         // ind:
         //  0-2     non-flippable edge
         //  3       original triangle contains point
         //  4       adjacent triangle has been found
         //  5       recursion depth exceeded        
         //  6       encountered distdpp in flipable plane

         if(ind==6) 
         {
            msg_dinfo("In sm_locate() found distdpp itself in flipable plane!\n");
            msg_dinfo("Trying steiner point instead \n");
            vdist.x = f_sclient.tri.H.x;
            vdist.y = f_sclient.tri.H.y;
            vdist.z = f_sclient.tri.H.z;
            flag_steiner = 1;
            foundtri_p = sm_locate_tri(mesh_p,
                                       pclspool_p,
                                       f_sclient.tri.tri_p,
                                       &vdist, 
                                       &f_sclient.tri.n,
                                       &ind, 
                                       null);
         }

      
         create_point(ref_dpp);

         if(ind<3)   // non swappable edge
         {
            msg_dinfo("Projection lies outside of an S-Edge \n");
            if(!flag_quality) 
            { 
               erase_point(ref_dpp);
               flag_doglines++;
               continue;
            }
            else 
            {
               dpp1 = foundtri_p->dppp[(ind+1)%3];
               dpp2 = foundtri_p->dppp[(ind+2)%3];
               vH.x = dpp2->x - dpp1->x;
               vH.y = dpp2->y - dpp1->y;
               vH.z = dpp2->z - dpp1->z;
               ref_dpp->x = dpp1->x + 0.5*vH.x;
               ref_dpp->y = dpp1->y + 0.5*vH.y;
               ref_dpp->z = dpp1->z + 0.5*vH.z;
            }
         }
         else if(ind==3) 
         {
            // point was inside the original triangle
            //
            ind = intersect_tri_line(foundtri_p, &f_sclient.tri.n,  &vdist,  ref_dpp,  f_sclient.tri.lp1h);
	
         }
         else if(ind==4) 
         {
            // adjacent triangle
            //
            ind = intersect_tri_line(foundtri_p, &f_sclient.tri.n, &vdist,  ref_dpp,   f_sclient.tri.lp1h);

         }
         else 
         {
            // recursion depth .. maximum reached..
            //
            flag_doglines++;
            erase_point(ref_dpp);
            continue;
         }

         // insert the newly calculated ref point
         //
         if(ind == 4) 
         {
            flag_doglines++;
            erase_point(ref_dpp);
            continue;
         }
	
         // snap test ..
         //
         if(flag_poverride) 
         {
            if(!snap_test(mesh_p, mesh_p->MINSNAP_PAR, ref_dpp)) 
            {
               vdist.x = f_sclient.tri.H.x;
               vdist.y = f_sclient.tri.H.y;
               vdist.z = f_sclient.tri.H.z;
               flag_steiner = 1;
               foundtri_p = sm_locate_tri(mesh_p,  pclspool_p, f_sclient.tri.tri_p,  &vdist, &f_sclient.tri.n, &ind,  null);
               if(ind<3) 
               {
                  // non swappable edge
                  //
                  erase_point(ref_dpp);
                  flag_doglines++;
                  continue;
               }
               else if(ind==3) 
               {
                  ind = intersect_tri_line(foundtri_p,  &f_sclient.tri.n,  &vdist,  ref_dpp,  f_sclient.tri.lp1h);
               }
               else if(ind==4) 
               {
                  ind = intersect_tri_line(foundtri_p, &f_sclient.tri.n,  &vdist,   ref_dpp,  f_sclient.tri.lp1h);
               }
               else 
               {
                  flag_doglines++;
                  erase_point(ref_dpp);
                  continue;
               }
               if(ind == 4) 
               {
                  flag_doglines++;
                  erase_point(ref_dpp);
                  continue;
               }
            }
         }

         // set indices from intersect_tri_line
         //
         if((dppout=oct_insert_point(ref_dpp, &mesh_p->global_octree)) != null) 
         {
            if(dppout->ntri_lp==null) 
            {
               dppout->x = ref_dpp->x;
               dppout->y = ref_dpp->y;
               dppout->z = ref_dpp->z;

               erase_point(ref_dpp);
               ref_dpp = dppout;
            }
            else   // connected point .. 
            {
               msg_warn3("+WARNING+: Not enough OCTRES for refinement at %f %f %f\n", ref_dpp->x, ref_dpp->y, ref_dpp->z);
               if(!flag_quality) flag_hitlimit = 1;
               erase_point(ref_dpp);
               continue;
            }	
         }
         else   //check numerics
         {
            snap_clear(mesh_p, mesh_p->MINSNAP_PAR, ref_dpp);
         }
         pts++;
         flag_inserted++;

         // actual refinement
         //
         if(ind==3) 
         {
            // [MOD] .. here a triangle is refined..
            //   refine and update triangle counter
            //
            mesh_p->gltr += sm_ref_tri(foundtri_p, ref_dpp, &mesh_p->lasttri_p); 
         } else if(ind<3) {

            // [MOD] .. here a triangle is refined..
            //   edge refinement
            mesh_p->gltr += sm_ref_triedge(foundtri_p, ind,  ref_dpp,  &mesh_p->lasttri_p); 
         }

         // local swapping / repair
         //
         newtris.n = 0;
         lp = ref_dpp->ntri_lp;
         while(lp != null) 
         {
            if(newtris.n == newtris.max) expand_ar(&newtris, D_AR);
            ((tri_pt*)newtris.vp)[newtris.n] = lp->tri_p;
            newtris.n++;
            lp = lp->ntri_lp;
         }
         for(i=0 ; i<newtris.n ; i++) 
         {
            surface_recursive_swap(mesh_p, ((tri_pt*)newtris.vp)[i], null);
         }
      
         stri_p = stri_p->ptri_p; 
      } 



      if(mesh_p->REFINE_GLINES == 2)     // basic version
      {
         if(flag_doglines>0) 
         {
            *ret_p = 1;
            break;
         }

         if(flag_inserted>0) 
         {
            if(rep<5) 
               continue;
            *ret_p = 1;
            break;
         }
      }
      else   // new version with balance
      {  
         if(flag_doglines>flag_inserted) 
         {
            *ret_p = 1;
            break;
         }
         else if(flag_inserted>0) 
         {
            if(rep<5) 
               continue; 
            *ret_p = 1;
            break;
         }	
      }

      if(flag_hitlimit) 
      {
         msg_warn("+WARNING+: Finished with surface processing but reached bounds\n");
         *ret_p = 0;
         break;
      }
      else 
      {
         msg_info("Successfully processed all surface triangles !\n");
         /* reset just in case                                                  */
         *ret_p = 0;
         break;
      }

   }

   free(newtris.vp);
   msg_info1("There were %i refinement points \n", pts);

   return(pts);

}


// ===================================================================================


static void t_rectriang2(ppt*, int, long , long );  // [MOD]additional material
static tri_pt* t_lasttri_pp;
static int t_elcount;
static int t_ctag;
static int t_facemark;
static struct vectordef t_vn;


// triangulates a possibly nonconvex polygon without holes                   
//
// USED EPSILON:							     */
//   MINN2_EPS								     */
//   H_EPS								     */
// [MOD].. additional material information
//
int nonconvex_triangulate2(struct arraydef* poly_p, int ctag, int facemark, tri_pt* lasttri_pp, long mat1, long mat2) 
{
   int i;
   int k;
   struct vectordef va;
   struct vectordef vb;
   ppt* dppp;
   
   // minimal number of points -> three
   //
   k = poly_p->n; 

   t_lasttri_pp = lasttri_pp;
   t_elcount = 0;
   t_ctag = ctag;
   t_facemark = facemark;

   dppp = (ppt*) poly_p->vp;

   // find valid normal vector (not at 2 collinear edges)
   //
   va.x = dppp[1]->x - dppp[0]->x;
   va.y = dppp[1]->y - dppp[0]->y;
   va.z = dppp[1]->z - dppp[0]->z;
   for(i=k-1 ; i>1 ; i--) {
      vb.x = dppp[i]->x - dppp[0]->x;
      vb.y = dppp[i]->y - dppp[0]->y;
      vb.z = dppp[i]->z - dppp[0]->z;

      // colinear check -> va x vb
      //
      t_vn.x = (va.y * vb.z - vb.y * va.z);   
      t_vn.y = (va.z * vb.x - vb.z * va.x);
      t_vn.z = (va.x * vb.y - vb.x * va.y);
      if(t_vn.x*t_vn.x + t_vn.y*t_vn.y + t_vn.z*t_vn.z <= MINN2_EPS) continue;
      break;
   }
  
   t_rectriang2(dppp, k, mat1, mat2);


   return t_elcount;

}


// ===================================================================================


// [MOD] additional material information
//
static void t_rectriang2(ppt* dppp, int k, long mat1, long mat2) 
{
   int i, j, l, h;
   int ll;
   int bad;
   int found;
   int corr;
   int lastsidebad;
   struct vectordef vn;
   struct vectordef vl;
   struct vectordef va;
   struct vectordef vn2;
   double doti;
   double dotj;

   struct arraydef poly1;
   struct arraydef poly2;


   // triangle
   //
   if(k==3) 
   {
      t_elcount++;

      create_triangle(dppp[0], dppp[1], dppp[2], t_ctag, t_facemark, *t_lasttri_pp); 
      (*t_lasttri_pp)->material[0] = mat1;
      (*t_lasttri_pp)->material[1] = mat2;

      return;
   }

   //  splitline  ?
   //
   found = 0;
   for(i=0 ; i<(k-2) ; i++) 
   {
      for(j=(i+2) ; j<k ; j++) 
      {

         if(i==0 && j==(k-1)) continue;

         vl.x = dppp[j]->x - dppp[i]->x;
         vl.y = dppp[j]->y - dppp[i]->y;
         vl.z = dppp[j]->z - dppp[i]->z;

         vn.x = (vl.y * t_vn.z - t_vn.y * vl.z);  
         vn.y = (vl.z * t_vn.x - t_vn.z * vl.x);
         vn.z = (vl.x * t_vn.y - t_vn.x * vl.y);

         bad = 0;
         ll = j+1; 
         l  = ll%k;
         va.x = dppp[l]->x - dppp[i]->x;
         va.y = dppp[l]->y - dppp[i]->y;
         va.z = dppp[l]->z - dppp[i]->z;
         if( (doti=va.x * vn.x + va.y * vn.y + va.z * vn.z) > H_EPS ) corr = -1;
         else if(doti < -(H_EPS)) corr = 1;
         else 
         {
            continue;
         } 
         for(ll=j+2 ; ll<k+i ; ll++) 
         {
            l = ll%k;

            va.x = dppp[l]->x - dppp[i]->x;
            va.y = dppp[l]->y - dppp[i]->y;
            va.z = dppp[l]->z - dppp[i]->z;

            if( (va.x * vn.x + va.y * vn.y + va.z * vn.z) * corr >= -(H_EPS) ) 
            {
               bad = 1;
               break; 
            }
         }
         if(bad) continue;

         for(l=i+1 ; l<j ; l++) 
         {
            va.x = dppp[l]->x - dppp[i]->x;
            va.y = dppp[l]->y - dppp[i]->y;
            va.z = dppp[l]->z - dppp[i]->z;

            if( (va.x * vn.x + va.y * vn.y + va.z * vn.z) * corr <= H_EPS ) 
            {
               bad = 1;
               break; 
            }
         }
         if(bad) continue;

         found = 1;
         break;
      }

      if(found) break;
   }

   // finite splitline
   //
   if(!found)    //could not find splitline
   {
      for(i=0 ; i<(k-2) ; i++) 
      {
         for(j=(i+2) ; j<k ; j++) 
         {
            if(i==0 && j==(k-1)) continue;

            vl.x = dppp[j]->x - dppp[i]->x;
            vl.y = dppp[j]->y - dppp[i]->y;
            vl.z = dppp[j]->z - dppp[i]->z;

            vn.x = (vl.y * t_vn.z - t_vn.y * vl.z);  
            vn.y = (vl.z * t_vn.x - t_vn.z * vl.x);
            vn.z = (vl.x * t_vn.y - t_vn.x * vl.y);

            bad = 0;
            ll = j+1; 
            l  = ll%k;
            va.x = dppp[l]->x - dppp[i]->x;
            va.y = dppp[l]->y - dppp[i]->y;
            va.z = dppp[l]->z - dppp[i]->z;

            if( (doti=va.x * vn.x + va.y * vn.y + va.z * vn.z) > H_EPS ) corr = -1;
            else if(doti < -(H_EPS)) corr = 1;
            else bad = 1;

            if(!bad) 
            {
               for(ll=j+2 ; ll<k+i ; ll++) 
               {
                  l = ll%k;
                  va.x = dppp[l]->x - dppp[i]->x;
                  va.y = dppp[l]->y - dppp[i]->y;
                  va.z = dppp[l]->z - dppp[i]->z;
                  if( (va.x * vn.x + va.y * vn.y + va.z * vn.z) * corr >= -(H_EPS)) 
                  {
                     bad = 1;
                     break; 
                  }
               }
            }
            if(bad) 
            {
               l = i+1; 
               va.x = dppp[l]->x - dppp[i]->x;
               va.y = dppp[l]->y - dppp[i]->y;
               va.z = dppp[l]->z - dppp[i]->z;
               if( (doti=va.x * vn.x + va.y * vn.y + va.z * vn.z) < -(H_EPS) ) 
               {
                  corr = -1;
               }
               else if(doti > H_EPS) corr = 1;
               else {  continue;    }   //avoid this splitline
               
               bad = 0;
               for(l=i+2 ; l<j ; l++) 
               {
                  va.x = dppp[l]->x - dppp[i]->x;
                  va.y = dppp[l]->y - dppp[i]->y;
                  va.z = dppp[l]->z - dppp[i]->z;

                  if( (va.x * vn.x + va.y * vn.y + va.z * vn.z) * corr <= H_EPS ) 
                  {
                     bad = 1;   // both sides are bad
                     break;
                  }
               }
               if(bad) 
                  continue; 
               bad = 1; 
            }

            // found splitline with valid point
            //  bad==1:   i...j side is not bad                                  
            //  bad==0:   j...i side is not bad
            //
            if(bad) 
            { 
               bad = 0; 
               ll = j+1; 
               l  = ll%k;
               va.x = dppp[l]->x - dppp[i]->x;
               va.y = dppp[l]->y - dppp[i]->y;
               va.z = dppp[l]->z - dppp[i]->z;
               if( (va.x * vn.x + va.y * vn.y + va.z * vn.z) * corr >= -(H_EPS) ) 
               {
                  lastsidebad = 1;
                  bad++; 
                  vl.x = dppp[l]->x - dppp[j]->x;
                  vl.y = dppp[l]->y - dppp[j]->y;
                  vl.z = dppp[l]->z - dppp[j]->z;

                  vn2.x = (vl.y*t_vn.z - t_vn.y*vl.z);   
                  vn2.y = (vl.z*t_vn.x - t_vn.z*vl.x);
                  vn2.z = (vl.x*t_vn.y - t_vn.x*vl.y);

                  va.x = dppp[j-1]->x - dppp[j]->x; 
                  va.y = dppp[j-1]->y - dppp[j]->y;
                  va.z = dppp[j-1]->z - dppp[j]->z;
                  if( (va.x * vn2.x + va.y * vn2.y + va.z * vn2.z) * corr <= 0.0 ) {
                     continue;
                  }
               }
               else 
               {
                  lastsidebad = 0;
               }
               for(ll=j+2 ; ll<k+i ; ll++) 
               {
                  l = ll%k;
                  va.x = dppp[l]->x - dppp[i]->x;
                  va.y = dppp[l]->y - dppp[i]->y;
                  va.z = dppp[l]->z - dppp[i]->z;
                  if(lastsidebad) 
                  {
                     if( (va.x * vn.x + va.y * vn.y + va.z * vn.z) * corr <= H_EPS ) 
                     {
                        lastsidebad = 0; 
                        h = (ll-1)%k; 

                        vl.x = dppp[l]->x - dppp[h]->x;
                        vl.y = dppp[l]->y - dppp[h]->y;
                        vl.z = dppp[l]->z - dppp[h]->z;

                        vn2.x = (vl.y * t_vn.z - t_vn.y * vl.z);   
                        vn2.y = (vl.z * t_vn.x - t_vn.z * vl.x);
                        vn2.z = (vl.x * t_vn.y - t_vn.x * vl.y);

                        va.x = dppp[j]->x - dppp[h]->x; 
                        va.y = dppp[j]->y - dppp[h]->y;
                        va.z = dppp[j]->z - dppp[h]->z;

                        dotj = (va.x * vn2.x + va.y * vn2.y + va.z * vn2.z) * corr;

                        va.x = dppp[i]->x - dppp[h]->x; 
                        va.y = dppp[i]->y - dppp[h]->y;
                        va.z = dppp[i]->z - dppp[h]->z;

                        doti = (va.x * vn2.x + va.y * vn2.y + va.z * vn2.z) * corr;

                        if(doti*dotj <= 0.0)    // side detection
                        {
                           bad = 1; 
                           break;
                        }
                        if(doti<0.0)    // same sign and are not too small
                        {
                           bad++;
                        }
                     }
                  }
                  else 
                  {
                     if((va.x * vn.x + va.y * vn.y + va.z * vn.z) * corr >= -(H_EPS))
                     {
                        lastsidebad = 1;    // wrong side

                        h = (ll-1)%k; 

                        vl.x = dppp[l]->x - dppp[h]->x;
                        vl.y = dppp[l]->y - dppp[h]->y;
                        vl.z = dppp[l]->z - dppp[h]->z;

                        vn2.x = (vl.y * t_vn.z - t_vn.y * vl.z);   
                        vn2.y = (vl.z * t_vn.x - t_vn.z * vl.x);
                        vn2.z = (vl.x * t_vn.y - t_vn.x * vl.y);

                        va.x = dppp[j]->x - dppp[h]->x; 
                        va.y = dppp[j]->y - dppp[h]->y;
                        va.z = dppp[j]->z - dppp[h]->z;

                        dotj = (va.x * vn2.x + va.y * vn2.y + va.z * vn2.z) * corr;

                        va.x = dppp[i]->x - dppp[h]->x; 
                        va.y = dppp[i]->y - dppp[h]->y;
                        va.z = dppp[i]->z - dppp[h]->z;

                        doti = (va.x * vn2.x + va.y * vn2.y + va.z * vn2.z) * corr;

                        if(doti*dotj <= 0.0) 
                        {
                           bad = 1; 
                           break;
                        }
                        if(doti>0.0) 
                        {
                           bad++;
                        }
                     }
                  }
               } 

               if(bad%2)    // next splitlien
               {
                  continue;
               }  

               // found valid splitline
               //
               found = 1;
               break; 
            } 
            else    // check other side
            {
               bad = 0; 
               l = i+1; 
               va.x = dppp[l]->x - dppp[i]->x;
               va.y = dppp[l]->y - dppp[i]->y;
               va.z = dppp[l]->z - dppp[i]->z;

               if( (va.x * vn.x + va.y * vn.y + va.z * vn.z) * corr <= H_EPS )
               {
                  lastsidebad = 1;
                  bad++; 

                  vl.x = dppp[l]->x - dppp[i]->x;
                  vl.y = dppp[l]->y - dppp[i]->y;
                  vl.z = dppp[l]->z - dppp[i]->z;
	    
                  vn2.x = (vl.y * t_vn.z - t_vn.y * vl.z);   
                  vn2.y = (vl.z * t_vn.x - t_vn.z * vl.x);
                  vn2.z = (vl.x * t_vn.y - t_vn.x * vl.y);
                  h = i-1; 
                  if(i==0) h = k-1;

                  va.x = dppp[h]->x - dppp[i]->x; 
                  va.y = dppp[h]->y - dppp[i]->y;
                  va.z = dppp[h]->z - dppp[i]->z;

                  if( (va.x * vn2.x + va.y * vn2.y + va.z * vn2.z) * corr <= 0.0) 
                  {
                     continue;
                  }
               }
               else 
               {
                  lastsidebad = 0;
               }
               for(l=i+2 ; l<j ; l++) 
               { 
                  va.x = dppp[l]->x - dppp[i]->x;
                  va.y = dppp[l]->y - dppp[i]->y;
                  va.z = dppp[l]->z - dppp[i]->z;
                  if(lastsidebad) {
                     if((va.x * vn.x + va.y * vn.y + va.z * vn.z) * corr >= -(H_EPS))
                     {
                        lastsidebad = 0;   // correct side

                        vl.x = dppp[l]->x - dppp[l-1]->x; 
                        vl.y = dppp[l]->y - dppp[l-1]->y;
                        vl.z = dppp[l]->z - dppp[l-1]->z;
		
                        vn2.x = (vl.y * t_vn.z - t_vn.y * vl.z);   
                        vn2.y = (vl.z * t_vn.x - t_vn.z * vl.x);
                        vn2.z = (vl.x * t_vn.y - t_vn.x * vl.y);

                        va.x = dppp[j]->x - dppp[l]->x; 
                        va.y = dppp[j]->y - dppp[l]->y;
                        va.z = dppp[j]->z - dppp[l]->z;

                        dotj = (va.x * vn2.x + va.y * vn2.y + va.z * vn2.z) * corr;

                        va.x = dppp[i]->x - dppp[l]->x; 
                        va.y = dppp[i]->y - dppp[l]->y;
                        va.z = dppp[i]->z - dppp[l]->z;

                        doti = (va.x * vn2.x + va.y * vn2.y + va.z * vn2.z) * corr;

                        if(doti*dotj <= 0.0) 
                        {
                           bad = 1; 
                           break;
                        }
                        if(doti<0.0) 
                        {
                           bad++;
                        }
                     }

                  }
                  else 
                  { 
                     if( (va.x * vn.x + va.y * vn.y + va.z * vn.z) * corr <= H_EPS ) 
                     {
                        lastsidebad = 1; 

                        vl.x = dppp[l]->x - dppp[l-1]->x; 
                        vl.y = dppp[l]->y - dppp[l-1]->y;
                        vl.z = dppp[l]->z - dppp[l-1]->z;

                        vn2.x = (vl.y * t_vn.z - t_vn.y * vl.z);   
                        vn2.y = (vl.z * t_vn.x - t_vn.z * vl.x);
                        vn2.z = (vl.x * t_vn.y - t_vn.x * vl.y);

                        va.x = dppp[j]->x - dppp[l]->x; 
                        va.y = dppp[j]->y - dppp[l]->y;
                        va.z = dppp[j]->z - dppp[l]->z;

                        dotj = (va.x * vn2.x + va.y * vn2.y + va.z * vn2.z) * corr;

                        va.x = dppp[i]->x - dppp[l]->x; 
                        va.y = dppp[i]->y - dppp[l]->y;
                        va.z = dppp[i]->z - dppp[l]->z;

                        doti = (va.x * vn2.x + va.y * vn2.y + va.z * vn2.z) * corr;

                        if(doti*dotj <= 0.0) 
                        {
                           bad = 1; 
                           break;
                        }
                        if(doti>0.0) 
                        {
                           bad++;
                        }
                     }
                  }
               } 
               if(bad%2)    //next splitline
               {
                  continue;  
               }
	  
               // found valid splitline
               //
               found = 1;
               break; 
            }  
         }
         if(found) 
         {
            break; 
         }
      } 
   }  
  
   // valid splitline  -> recursion
   //
   array_init(&poly2, i+k-j+1, sizeof(ppt));
   bad = 0;
   for(ll=j ; ll<=i+k ; ll++) 
   {
      l = ll%k;
      ((ppt*)poly2.vp)[bad] = dppp[l];
      bad++;
   }
   poly2.n = bad;

   array_init(&poly1, j-i+1, sizeof(ppt));
   found = 0;
   for(l=i ; l<=j ; l++) 
   {
      ((ppt*)poly1.vp)[found] = dppp[l];
      found++;
   }
   poly1.n = found;


   // recursion start
   //
   t_rectriang2((ppt*)poly1.vp, found, mat1, mat2);
   free(poly1.vp);

   t_rectriang2((ppt*)poly2.vp, bad, mat1, mat2);
   free(poly2.vp);
  
   return;

}

  
// ===================================================================================
// ===================================================================================



struct snapclientdef 
{
   ppt dpp;
   double snapd;
   int eraseflag;
};

struct snaptestclientdef 
{
   int unconpts;
   int conpts;
};

struct afmindistdef 
{
   ppt dpp;
   double shortestdist;
   int fm;
};


void cb_snap_point(ppt dpp, void* vp, mesh_p_t mesh_p) 
{
   struct snapclientdef* scl_p = (struct snapclientdef*) vp;
  
   if(dpp==scl_p->dpp || scl_p->eraseflag) return;
  
   // different point found
   //
   if( sqrt((dpp->x-scl_p->dpp->x)*(dpp->x-scl_p->dpp->x) +
            (dpp->y-scl_p->dpp->y)*(dpp->y-scl_p->dpp->y) +
            (dpp->z-scl_p->dpp->z)*(dpp->z-scl_p->dpp->z)) <= scl_p->snapd) 
   {
      scl_p->eraseflag = 1;   //snap flag
   }

}

int cb_snap_clear(ppt dpp, void* vp, mesh_p_t mesh_p) 
{
   ppt dpporg = (ppt) vp;
  
   if(dpp==dpporg || dpp->ntri_lp!=null) return 0;
   erase_point(dpp);

   return 1;
}

void cb_snap_test(ppt dpp, void* vp, mesh_p_t mesh_p) 
{
   struct snaptestclientdef* scl_p = (struct snaptestclientdef*) vp;

   if(dpp->ntri_lp!=null) 
      scl_p->conpts++;
   else 
      scl_p->unconpts++;
}


void cb_setafmindist(ppt dpp, void* vp, mesh_p_t mesh_p) 
{
   struct afmindistdef* cl_p = (struct afmindistdef*) vp;
   struct tri_listdef* lp;
   struct vectordef ve;
   double length;
  
   // disregard points not from the  surface
   //
   if(dpp==cl_p->dpp || dpp->ntri_lp==null) 
      return;
  
   for(lp=dpp->ntri_lp ; lp!=null ; lp=lp->ntri_lp) 
   {
      if(lp->tri_p->facemark == cl_p->fm ||  lp->tri_p->facemark == ~cl_p->fm) 
         break;
   }
   if(lp == null) 
   {
      return;
   }

   ve.x = dpp->x - cl_p->dpp->x;
   ve.y = dpp->y - cl_p->dpp->y;
   ve.z = dpp->z - cl_p->dpp->z;
   length = sqrt(ve.x*ve.x + ve.y*ve.y + ve.z*ve.z);
   if(length < cl_p->shortestdist) cl_p->shortestdist = length;

   return;

}





void snap_clear(mesh_p_t mesh_p, double snapd,  ppt dpp) 
{
   struct pointtype dpmin;
   struct pointtype dpmax;

   dpmin.x = dpp->x - snapd;
   dpmax.x = dpp->x + snapd;
   dpmin.y = dpp->y - snapd;
   dpmax.y = dpp->y + snapd;
   dpmin.z = dpp->z - snapd;
   dpmax.z = dpp->z + snapd;
    
   ext_del(&dpmin, 
           &dpmax, 
           cb_snap_clear,
           dpp, 
           &mesh_p->global_octree,
           mesh_p);

}


int snap_test(mesh_p_t mesh_p, double snapd,  ppt dpp) 
{
   struct pointtype dpmin;
   struct pointtype dpmax;
   struct snaptestclientdef cl;

   dpmin.x = dpp->x - snapd;
   dpmax.x = dpp->x + snapd;
   dpmin.y = dpp->y - snapd;
   dpmax.y = dpp->y + snapd;
   dpmin.z = dpp->z - snapd;
   dpmax.z = dpp->z + snapd;
    
   cl.conpts = 0;
   cl.unconpts = 0;

   ext_find(&dpmin,  &dpmax, cb_snap_test,  &cl,  &mesh_p->global_octree,  mesh_p);

   if(cl.conpts == 0) 
      return 1;
   else return 0;


}

int snap_points(mesh_p_t mesh_p, struct arraydef* pointlistp, struct arraydef* mindistlistp) 
{
   int s;
   int i;
   ppt* dppp;
   struct pointtype dpmin;
   struct pointtype dpmax;
   struct snapclientdef snapclient;
   double snapd;

   dppp = (ppt*)pointlistp->vp;
   s = 0;

   snapd = mesh_p->MINSNAP_PAR;
  
   for(i=0 ; i<pointlistp->n ; i++) 
   {
      if(mindistlistp!=null && i<mindistlistp->n) 
      {
         snapd = ((double*)mindistlistp->vp)[i];
      }
      dpmin.x = dppp[i]->x - snapd;
      dpmax.x = dppp[i]->x + snapd;
      dpmin.y = dppp[i]->y - snapd;
      dpmax.y = dppp[i]->y + snapd;
      dpmin.z = dppp[i]->z - snapd;
      dpmax.z = dppp[i]->z + snapd;
    
      snapclient.dpp = dppp[i];
      snapclient.eraseflag = 0;
      snapclient.snapd = snapd;
      ext_find(&dpmin, &dpmax, cb_snap_point, &snapclient,  &mesh_p->global_octree, mesh_p);

      if(snapclient.eraseflag && dppp[i]->ntri_lp==null) {
         msg_dinfo("Snapping Point \n");
         ext_del(dppp[i],  dppp[i], cb_erase_point, null,   &mesh_p->global_octree,    null);   
         s++;
      }
   }
   return s;
}



void surf_init() 
{

   array_init(&f_sclient.cneps_list, D_AR, sizeof(ppt)); 
   array_init(&f_sclient.uneps_list, D_AR, sizeof(ppt));
   array_init(&f_sclient.dpar_list, D_AR, sizeof(ppt));
   array_init(&f_sclient.flip_list, D_AR, sizeof(ppt));
}


void free_surfmod() 
{
   free(f_sclient.uneps_list.vp);
   free(f_sclient.cneps_list.vp);
   free(f_sclient.dpar_list.vp);
   free(f_sclient.flip_list.vp);
}
