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


// for parser / input file
//
#define START_VERTICES  "Vertices"
#define NUM_STARTVERT    8
#define START_POLYGONS  "Polygons"
#define NUM_STARTPOLY    8
#define START_SEGMENTS  "Polyhedra"
#define NUM_STARTSGMTS   9
#define START_SEGMENT   "Polyhedron"
#define NUM_STARTSGMT    10
#define START_MATERIALS  "Material"
#define NUM_STARTMATS   8
#define START_PARAMETERS  "Parameter"
#define NUM_STARTPARS   9
#define START_FACEMARKERS  "Facemarkers"
#define NUM_STARTFMS   11


// input parser  with the following grammer
//
/* SYNTAX:  *   entries without spaces or newlines that exist or not exist   */
/* 	    +   entries without spaces or newlines which must exist          */
/* 	  * * * denotes any number of entries that exist or not exist        */
/*          _   spaces or newlines that may or may not exist                 */
/*              (no blank inbetween or blanks inbetween)                     */
/*          ||  denotes logical or                                           */
/* ------------------------------------------------------------------------- */
/*  * * *                                                                    */
/*  KEYWORD_VERTICES[number]_+                                               */
/*  x y z ....								     */
/* 									     */
/*  * * * 								     */
/*  KEYWORD_POLYGONS[number]_+						     */
/*  +[number]_+								     */
/*  v1 v2 v3 ...							     */
/*  +									     */
/*  +[number]_+								     */
/*  v1 v2 v3 ...							     */
/*  +									     */
/* 									     */
/*  * * * 								     */
/*  KEYWORD_FACEMARKERS[number]_+					     */
/*  +[number]_+								     */
/*  fm p1 p2 p3 ...							     */
/*  +									     */
/*  +[number]_+								     */
/*  fm p1 p2 p3 ...							     */
/*  +									     */
/* 									     */
/*  * * *								     */
/*  KEYWORD_SEGMENTS[number]*						     */
/* 									     */
/*  * * *								     */
/*  KEYWORD_SEGMENT[number]_+						     */
/*  p1 p2 p3 p4 ...							     */
/*  * * *								     */
/*  KEYWORD_SEGMENT[number]_+						     */
/*  p1 p2 p3 p4 ...							     */
/*  * * *								     */
/*  KEYWORD_MATERIALS[number]_+						     */
/*  m1 m2 m3 m4 ...							     */
/*  * * *								     */
/* ------------------------------------------------------------------------- */
/*									     */
int read_geom(mesh_p_t mesh_p, FILE* fp) 
{
   int i, j, k, l, m, h;
   int flipflag;
   int number;
   int elcount;
   int noseglist;
   int ctag;
   int stflag;
   int index;
   double minx;
   double maxx;
   double miny;
   double maxy;
   double minz;
   double maxz;
   double epsx;
   double epsy;
   double epsz;
   char charin[LONGEST_CHARIN];
   char *charpart;
   ppt dpp;
   ppt dppout;
   tri_pt starttri_p;
   tri_pt begintri_p;
   tri_pt tri_p2;

   struct arraydef vertex_list;
   int polygon_count;
   struct polygondef* polygon_p;
   int polyhedra_count;
   struct segmentdef* segment_p;
   int z;
   tri_pt tri_p;
   struct pointtype dpmin;
   struct pointtype dpmax;
   struct ls_clientdef ls_client;
   int nfms;
   int fm;

   noseglist = 0;     // flag for segment list
   array_init(&ls_client.ls_list, 20, sizeof(ppt)); 


   do 
   {
      if(fscanf(fp, "%s", charin)==EOF) 
      {
         msg_info("Premature EOF ? \n");
         free(ls_client.ls_list.vp);
         return 0;
      }
   } while( strncmp(charin, START_VERTICES, NUM_STARTVERT) != 0 );
   charpart = strtok(charin, "[");
   charpart = strtok(NULL, "]");
   number = atoi(charpart);
   charpart = strtok(NULL, "}");
   if(charpart == NULL) int ret = fscanf(fp, "%s", charin);

   array_init(&vertex_list, number, sizeof(ppt));
   vertex_list.n = number;
   msg_info1("Reading %i vertices...\n", vertex_list.n); 
  
   for(i=0 ; i<vertex_list.n ; i++) 
   {
      create_point(dpp);
      ((ppt*)vertex_list.vp)[i] = dpp;

      int ret = fscanf(fp, "%lf %lf %lf", &(dpp->x), &(dpp->y), &(dpp->z));

      if(i==0) 
      {
         minx = dpp->x; 
         maxx = dpp->x;
         miny = dpp->y; 
         maxy = dpp->y;
         minz = dpp->z; 
         maxz = dpp->z;

         continue;
      }

      if(dpp->x > maxx) maxx = dpp->x;
      else if(dpp->x < minx) minx = dpp->x;

      if(dpp->y > maxy) maxy = dpp->y;
      else if(dpp->y < miny) miny = dpp->y;

      if(dpp->z > maxz) maxz = dpp->z;
      else if(dpp->z < minz) minz = dpp->z;
   }

   epsx = (minx==maxx)? (BBZEROBUF) : ((maxx - minx) * BBCUSHIONPERCENT);
   epsy = (miny==maxy)? (BBZEROBUF) : ((maxy - miny) * BBCUSHIONPERCENT);
   epsz = (minz==maxz)? (BBZEROBUF) : ((maxz - minz) * BBCUSHIONPERCENT);
   maxx += epsx;
   minx -= epsx;
   maxy += epsy;
   miny -= epsy;
   maxz += epsz;
   minz -= epsz;

   // octree 
   //
   if(mesh_p->global_octree.octree_counter>0) 
   {
      if(minx < mesh_p->global_octree.bbminimum.x || 
	 maxx > mesh_p->global_octree.bbmaximum.x ||
	 miny < mesh_p->global_octree.bbminimum.y || 
	 maxy > mesh_p->global_octree.bbmaximum.y ||
	 minz < mesh_p->global_octree.bbminimum.z || 
	 maxz > mesh_p->global_octree.bbmaximum.z  ) 
      {
         msg_error("+ERROR+: Cannot load data exceeding current "); 
         msg_error("octree bounding box\n");
      
         for(i=0 ; i<vertex_list.n ; i++) 
         {
            erase_point(((ppt*)vertex_list.vp)[i]);
         }
         free(vertex_list.vp);
         free(ls_client.ls_list.vp);
         return 0;
      }
   } else 
   { 
      mesh_p->global_octree.bbminimum.x = minx;
      mesh_p->global_octree.bbmaximum.x = maxx;
      mesh_p->global_octree.bbminimum.y = miny;
      mesh_p->global_octree.bbmaximum.y = maxy;
      mesh_p->global_octree.bbminimum.z = minz;
      mesh_p->global_octree.bbmaximum.z = maxz;

      octree_init(&mesh_p->global_octree);
   }

   do 
   {
      if(fscanf(fp, "%s", charin)==EOF) 
      {
         msg_dinfo("No Parameter section found in file\n");
         break;
      }
   } while( strncmp(charin, START_PARAMETERS, NUM_STARTPARS) != 0 );

   if(strncmp(charin, START_PARAMETERS, NUM_STARTPARS) == 0) 
   {
      if(strcmp(charin, START_PARAMETERS)==0) int ret = fscanf(fp, "%s", charin);
      msg_info("Reading the Parameter section \n");
      printf("Reading the Parameter section \n");

      param_init(mesh_p, fp);
   }

   rewind(fp);

   for(i=0 ; i<vertex_list.n ; i++) 
   {
      dppout = oct_insert_point(((ppt*)vertex_list.vp)[i], &mesh_p->global_octree);
      if(dppout!=null) 
      {
         msg_dinfo("Point not inserted into octree, ");
         msg_dinfo("because same or similar point was found. \n");
         erase_point(((ppt*)vertex_list.vp)[i]);

         ((ppt*)vertex_list.vp)[i] = dppout;    //using the found octree point
      } 
   }

   // read in polygons
   //
   do 
   {
      if(fscanf(fp, "%s", charin)==EOF) 
      {
         msg_info("Premature EOF ? \n");
         free(vertex_list.vp);
         free(ls_client.ls_list.vp);
         return 0;
      }
   } while( strncmp(charin, START_POLYGONS, NUM_STARTPOLY) != 0 );
   charpart = strtok(charin, "[");
   charpart = strtok(NULL, "]");
   polygon_count = atoi(charpart);
   charpart = strtok(NULL, "}");
   if(charpart == NULL) int ret = fscanf(fp, "%s", charin);

   polygon_p = (struct polygondef*) my_malloc(polygon_count * sizeof(struct polygondef));   // allocate memory for polygon array

   for(i=0 ; i<polygon_count ; i++) 
   {
      int ret = fscanf(fp, "%s", charin);
      charpart = strtok(charin, "[");
      charpart = strtok(NULL, "]");
      k = atoi(charpart);
      charpart = strtok(NULL, "}");
      if(charpart == NULL) int ret = fscanf(fp, "%s", charin);
    
      array_init(&(polygon_p[i].vts), k, sizeof(ppt));
      polygon_p[i].vts.n = k;

      for(j=0 ; j<k ; j++) 
      {
         int ret = fscanf(fp, "%i", &h);
         ((ppt*)(polygon_p[i].vts.vp))[j] = ((ppt*)vertex_list.vp)[h];
      }

      ret = fscanf(fp, "%s", charin);

      k--;
      if(((ppt*)(polygon_p[i].vts.vp))[k] != ((ppt*)(polygon_p[i].vts.vp))[0]) 
      {
         k++;
      }

      polygon_p[i].vts.n = k;
      polygon_p[i].ctag = CTG_BLOCK;
      polygon_p[i].first_tri_p = null;
      polygon_p[i].facemark = init_facemark;
      polygon_p[i].number_of_indices = 0;  

      if(k<3) 
      { 
         msg_info1("%ith polygon has less than two points !\n", i);
         for(j=0 ; j<(i+1) ; j++) 
         {
            free(polygon_p[j].vts.vp);
         }
         free(polygon_p);
         free(vertex_list.vp);
         free(ls_client.ls_list.vp);
         return 0;
      }
   }
  
   // segment list
   //
   do 
   {
      if(fscanf(fp, "%s", charin)==EOF) 
      {
         msg_info("No segment list found, creating segment containing all \n");
         msg_info("the  polygons \n");
         noseglist = 1;
         rewind(fp);
         break;
      }
   } while( strncmp(charin, START_SEGMENTS, NUM_STARTSGMTS) !=0 );

   if(noseglist) 
   {
      polyhedra_count = 1;
   } else 
   {
      charpart = strtok(charin, "[");
      charpart = strtok(NULL, "]");
      polyhedra_count = atoi(charpart);
      if(polyhedra_count <= 0) 
      {
         polyhedra_count = 1;
         msg_info("Segment list empty, creating segment containing all \n");
         msg_info("the  polygons \n");
         noseglist = 1;
      } else 
      {
         msg_info1("There are %i segments.\n", polyhedra_count); 
      }
   }

   segment_p = (struct segmentdef*) my_malloc(polyhedra_count * sizeof(struct segmentdef));

  
   // [MOD] .. additional material information
   //
   for(l=0 ; l<polyhedra_count ; l++) 
   {
      if(noseglist) 
      {
         number = polygon_count;
      } else 
      {
         // number of polygons
         //
         do 
         {
            if(fscanf(fp, "%s", charin)==EOF) 
            {
               msg_info("Premature EOF ? \n");
               for(i=0 ; i<l ; i++) 
               {
                  free(segment_p[i].pgs_p);
               }
               free(segment_p);
               for(j=0 ; j<polygon_count ; j++) 
               {
                  free(polygon_p[j].vts.vp);
               }
               free(polygon_p);
               free(vertex_list.vp);
               free(ls_client.ls_list.vp);
               return 0;
            }
         } while( strncmp(charin, START_SEGMENT, NUM_STARTSGMT) != 0 );
         charpart = strtok(charin, "[");
         charpart = strtok(NULL, "]");
         number = atoi(charpart);
         charpart = strtok(NULL, "}");
         if(charpart == NULL) int ret = fscanf(fp, "%s", charin);
         msg_info2("Segment %i contains %i polygons\n", l, number); 
      }

      segment_p[l].n = number;
      segment_p[l].pgs_p = (int*) my_malloc(number * sizeof(int));

      // loop over all polygons of the l-th segment
      //
      for(m=0 ; m<number ; m++) 
      { 
         if(noseglist) 
            i = m;
         else 
            int ret = fscanf(fp, "%i", &i);

         segment_p[l].pgs_p[m] = i;
         if(i < 0) 
            i = ~i;

         if (polygon_p[i].number_of_indices == 0)    // additional material information
         {
            polygon_p[i].number_of_indices=1;
            polygon_p[i].material[0]=l;
            polygon_p[i].material[1]=-2;   // material is going to be incremented.. 
         }
         else if (polygon_p[i].number_of_indices == 1)
         {
            polygon_p[i].number_of_indices=2;
            polygon_p[i].material[1]=polygon_p[i].material[0];
            polygon_p[i].material[0]=l;

            if ( polygon_p[i].material[1] == polygon_p[i].material[0])
            {
               polygon_p[i].number_of_indices=1;
               polygon_p[i].material[1]=-2;
            }
         }
         else if (polygon_p[i].number_of_indices == 2)
         {
            polygon_p[i].number_of_indices=3;
            polygon_p[i].material[1]=-1;
            polygon_p[i].material[0]=-1;
         }


         if(polygon_p[i].ctag == CTG_BLOCK) 
         {
            polygon_p[i].ctag = CTG_BFACE + CTG_SEGBFACE;
         }
         else if(polygon_p[i].ctag == (CTG_BFACE + CTG_SEGBFACE)) 
         {
            polygon_p[i].ctag = CTG_IFACE + CTG_SEGIFACE;
         }
         else if( polygon_p[i].ctag == (CTG_IFACE + CTG_SEGIFACE) ) 
         {
            msg_warn("+WARNING+: Polygon is referenced by more than ");
            msg_warn("two segments\n");
         }
      }
   }    

   // all segments are read in
   //
   // now process all segments
   //
   elcount = 0;
   begintri_p = mesh_p->lasttri_p; 

   for(l=0 ; l<polyhedra_count ; l++) 
   {
      stflag = 0;
      if(mesh_p->FLIP_GNORMALS) 
         flipflag = 1;
      else 
         flipflag = 0;


      number = segment_p[l].n;
      for(m=0 ; m<number ; m++) 
      { 
         i = segment_p[l].pgs_p[m];
      
         if(i < 0) i = ~i;
         k = polygon_p[i].vts.n;
         ctag = polygon_p[i].ctag;
      
         // [MOD] .. a polygon is going to be blocked here..
         //
         if(ctag & CTG_BLOCK) 
         {
            continue; 
         }
      
         if( (flipflag==1 && segment_p[l].pgs_p[m]>=0) || 
             (flipflag==0 && segment_p[l].pgs_p[m]<0) ) 
         {
            // switch the orientation of ith polygon (mth in lth segment)     
            //
            for(j=0 ; j<(k/2.0) ; j++) 
            {
               dpp = ((ppt*)(polygon_p[i].vts.vp))[j];

               ((ppt*)(polygon_p[i].vts.vp))[j] = 
                  ((ppt*)(polygon_p[i].vts.vp))[k-j-1];

               ((ppt*)(polygon_p[i].vts.vp))[k-j-1] = dpp;

            }
            if(polygon_p[i].facemark!=init_facemark && 
               polygon_p[i].facemark!=~init_facemark) 
            {
               polygon_p[i].facemark = ~polygon_p[i].facemark;
            }
         }
         starttri_p = mesh_p->lasttri_p; 
         polygon_p[i].first_tri_p = starttri_p;


         // NEW nonconvex_triangulate with material information
         //
         elcount += nonconvex_triangulate2(&(polygon_p[i].vts),  ctag,  polygon_p[i].facemark, &mesh_p->lasttri_p,
                                           polygon_p[i].material[0]+1, polygon_p[i].material[1]+1);  
         polygon_p[i].ctag = CTG_BLOCK; 

         polygon_p[i].last_tri_p = mesh_p->lasttri_p;
         if(stflag!=2) 
         {
            if(ctag==(CTG_BFACE + CTG_SEGBFACE)) 
            {
               stflag = 2; 
               mesh_p->lasttri_p->ctag |= CTG_SEED;
               if(mesh_p->seed_list.n == mesh_p->seed_list.max) 
               {
                  expand_ar(&mesh_p->seed_list, D_AR+polyhedra_count);
               }
               ((struct seed_def*) (mesh_p->seed_list.vp))[mesh_p->seed_list.n].seedtri_p      =   mesh_p->lasttri_p; 
               ((struct seed_def*) (mesh_p->seed_list.vp))[mesh_p->seed_list.n].material_index = l;
               mesh_p->seed_list.n++;

               polygon_p[i].ctag |= CTG_SEED; 

            } else 
            { 
               // interface / segment interface
               //
               stflag = 1; 
               index = i; 
            }
         } 
      } 

      if(stflag!=2) 
      { 
         if(stflag==1) 
         {
            msg_dinfo1("Segment %i contains no boundary faces \n", l);
            mesh_p->lasttri_p->ctag |= CTG_SEED;
            if(mesh_p->seed_list.n == mesh_p->seed_list.max) 
            {
               expand_ar(&mesh_p->seed_list, D_AR+polyhedra_count);
            }
            ((struct seed_def*)
             (mesh_p->seed_list.vp))[mesh_p->seed_list.n].seedtri_p = 
               mesh_p->lasttri_p; 
            ((struct seed_def*)
             (mesh_p->seed_list.vp))[mesh_p->seed_list.n].material_index = l;
            mesh_p->seed_list.n++;
	
            polygon_p[index].ctag |= CTG_SEED; 

         } else 
         { 
            msg_dinfo1("Segment %i contains no new triangles \n", l);
            for(m=0 ; m<number ; m++) 
            {
               i = segment_p[l].pgs_p[m];
               if(i < 0) i = ~i;
               if(! (polygon_p[i].ctag&CTG_SEED) ) 
               { 
                  /* avoid other starttris                                         */
                  polygon_p[i].last_tri_p->ctag |= CTG_SEED;
                  /* Manage seed_list and set material index                       */
                  if(mesh_p->seed_list.n == mesh_p->seed_list.max) 
                  {
                     expand_ar(&mesh_p->seed_list, D_AR+polyhedra_count);
                  }
                  ((struct seed_def*)
                   (mesh_p->seed_list.vp))[mesh_p->seed_list.n].seedtri_p = 
                     polygon_p[i].last_tri_p; 
                  ((struct seed_def*)
                   (mesh_p->seed_list.vp))[mesh_p->seed_list.n].material_index = l;
                  mesh_p->seed_list.n++;
                  polygon_p[i].ctag |= CTG_SEED;

                  /* if(flipflag==0) { FLIP in ANYCASE ASSUMING correct interface  */

                  /* flip all triangles of polygon containing starttri           */
                  /* when not interactive default for flipflag==0 should be      */
                  /* to *flip* the interface tri which already exists.           */
                  starttri_p = polygon_p[i].first_tri_p; 
                  do 
                  { 
                     /* there must be at least one triangle in a polygon          */
                     starttri_p = starttri_p->ntri_p;
                     dppout = starttri_p->dppp[0];
                     starttri_p->dppp[0] = starttri_p->dppp[1];
                     starttri_p->dppp[1] = dppout;
                     /* update facemark                                           */
                     if(starttri_p->facemark!=init_facemark &&
                        starttri_p->facemark!=~init_facemark) 
                     {
                        starttri_p->facemark = ~starttri_p->facemark;
                     }
                  } while(starttri_p!=polygon_p[i].last_tri_p);
                  /* only one starttri, no more polygons                           */
                  break;
               }
               /* next polygon                                                    */
            }
            /* check if a seed was found among all the polygons from this segment*/
            if(m==number) 
            {
               msg_warn1("+WARNING:+ Cannot create a seed for segment %i !\n", l);
               msg_warn("           This segment is bounded exclusively by other seeds!\n");
            }
         }
      }

      /* NEXT SEGMENT                                                          */
   } 


   /** PROCESS EXTRA MATERIAL INFORMATION *************************************/

   do 
   {
      if(fscanf(fp, "%s", charin)==EOF) 
      {
         msg_dinfo("No extra material information found \n");
         rewind(fp);
         break;
      }
   } while( strncmp(charin, START_MATERIALS, NUM_STARTMATS) != 0 );

   if(strncmp(charin, START_MATERIALS, NUM_STARTMATS) == 0) 
   {
      charpart = strtok(charin, "[");
      charpart = strtok(NULL, "]");
      number = atoi(charpart);
      charpart = strtok(NULL, "}");
      if(charpart == NULL) int ret = fscanf(fp, "%s", charin);
    
      if(number!=polyhedra_count) 
      {
         msg_info("Not reading the material info (different number) \n");
      }
      else 
      {
         msg_info("Reading the material info ... \n");
         for(l=0 ; l<polyhedra_count ; l++) 
         {
            int ret = fscanf(fp, "%i", &j);
            if(j>=0) 
            {
               ((struct seed_def*)(mesh_p->seed_list.vp))[l].material_index = j;
            }
            else 
            {
               ((struct seed_def*)(mesh_p->seed_list.vp))[l].material_index = j;

               dppout = 
                  ((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->dppp[0];
               ((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->dppp[0] = 
                  ((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->dppp[1];
               ((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->dppp[1] = 
                  dppout;

               /* update facemark                                                 */
               if(((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->facemark != init_facemark &&
                  ((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->facemark != ~init_facemark) 
               {
                  ((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->facemark=
                     ~((struct seed_def*)(mesh_p->seed_list.vp))[l].seedtri_p->facemark;
               }

            }
         }
      }
   }

   // freeing allocated memory
   for(l=0 ; l<polyhedra_count ; l++) 
   {
      free(segment_p[l].pgs_p);
   }
   free(segment_p);
   for(j=0 ; j<polygon_count ; j++) 
   {
      free(polygon_p[j].vts.vp);
   }
   free(polygon_p);
   free(vertex_list.vp);
   free(ls_client.ls_list.vp);

  
   /** COMPLETE DATASTRUCTURE FROM BEGINTRI_P ON ******************************/
   j = 0;
   m = 0; /* flip toggle                                                      */
   tri_p = begintri_p->ntri_p; 
   while(tri_p!=null) 
   {
      //    printf(" .. hierwrap.c:: complete data structure \n");
      /* just to make sure check if it exists already!                         */
      /* If yes, then could change ctag or do flipping for the findmaterials   */
      /* functionality!                                                        */
      if(exist_tri(tri_p->dppp[0], 
		   tri_p->dppp[1], 
		   tri_p->dppp[2],
		   &tri_p2) != 0) 
      {

         /* count double existing tris                                          */
         j++;
      
         /* CASE both seeds                                                     */
         if( (tri_p2->ctag&CTG_SEED) && (tri_p->ctag&CTG_SEED) ) 
         {
            /* both triangles were referenced by segment info and both are seeds */
            /* erase the uninserted (tri_p) also from the seedlist               */
            for(i=0 ; i < mesh_p->seed_list.n ; i++) 
            {
               if( ((struct seed_def*)(mesh_p->seed_list.vp))[i].seedtri_p==tri_p) 
               {
                  break;
               }
            }
            /* must be found !! (i<seed_list.n)                                  */
            mesh_p->seed_list.n--;
            ((struct seed_def*)(mesh_p->seed_list.vp))[i].seedtri_p = 
               ((struct seed_def*)(mesh_p->seed_list.vp))[mesh_p->seed_list.n].seedtri_p;
            ((struct seed_def*)(mesh_p->seed_list.vp))[i].material_index = 
               ((struct seed_def*)(mesh_p->seed_list.vp))[mesh_p->seed_list.n].material_index;
            msg_warn("+WARNING+: Erasing a seed triangle due to triangles which exist twice! \n");
	
            /* could have been BFACE and needs to be changed to IFACE            */
            /* could cause troubles when polys are not just triangles..          */
            /* or for flag checks when cocircular points..                       */
            /* or when just simply wrong that double tris.. and not interface    */
            tri_p2->ctag = CTG_SEED + CTG_IFACE + CTG_SEGIFACE;

            /* do not need to remove tri_p because is not yet inserted           */
            tri_p2 = tri_p; /* the one to be erased                              */
            tri_p = tri_p->ntri_p; /* can be null                                */
            erase_triangle(tri_p2, mesh_p->lasttri_p);
            elcount--;

            /* continue with next tri (tri_p)                                    */
            continue;
         }

         /* CASE one of them is a seed                                          */
         if(tri_p2->ctag&CTG_SEED) 
         {
            /* the inserted tri is a seed                                        */
	
            /* could have been BFACE and needs to be changed to IFACE            */
            /* could cause troubles when polys are not just triangles..          */
            /* or for flag checks when cocircular points..                       */
            /* or when just simply wrong that double tris.. and not interface    */
            tri_p2->ctag = CTG_SEED + CTG_IFACE + CTG_SEGIFACE;

            /* do not need to remove tri_p because is not yet inserted           */
            tri_p2 = tri_p; /* the one to be erased                              */
            tri_p = tri_p->ntri_p; /* can be null                                */
            erase_triangle(tri_p2, mesh_p->lasttri_p);
            elcount--;

            /* continue with next tri (tri_p)                                    */
            continue;
         }
         if(tri_p->ctag&CTG_SEED) 
         {
            /* the uninserted tri is a seed                                      */
	
            /* could have been BFACE and needs to be changed to IFACE            */
            /* could cause troubles when polys are not just triangles..          */
            /* or for flag checks when cocircular points..                       */
            /* or when just simply wrong that double tris.. and not interface    */
            tri_p->ctag = CTG_SEED + CTG_IFACE + CTG_SEGIFACE;

            /* need to remove tri_p2 because was inserted                        */
            sm_remove_triangle(tri_p2);
            erase_triangle(tri_p2, mesh_p->lasttri_p);
            elcount--;

            /* insert the seed                                                   */
            insert_triangle(tri_p);
            tri_p = tri_p->ntri_p; 

            /* continue with next tri (tri_p)                                    */
            continue;
         }

         /* CASE none of them is a seed and the inserted can always win         */
         /* alternately flip the inserted to make sure findmaterials works      */
         /* (necessary for segments which are surrounded by interfaces)         */

         /* could have been BFACE and needs to be changed to IFACE              */
         /* could cause troubles when polys are not just triangles..            */
         /* or for flag checks when cocircular points..                         */
         /* or when just simply wrong that double tris.. and not interface      */
         tri_p2->ctag = CTG_IFACE + CTG_SEGIFACE;

         /* alternate flip                                                      */
         if(m) 
         {
            dpp = tri_p2->dppp[0];
            tri_p2->dppp[0] = tri_p2->dppp[1];
            tri_p2->dppp[1] = dpp;
            /* update facemark                                                   */
            if(tri_p2->facemark!=init_facemark && 
               tri_p2->facemark!=~init_facemark) 
            {
               tri_p2->facemark = ~tri_p2->facemark;
            }
         }
         m = (m+1)%2;

         /* do not need to remove tri_p because is not yet inserted             */
         tri_p2 = tri_p; /* the one to be erased                                */
         tri_p = tri_p->ntri_p; /* can be null                                  */
         erase_triangle(tri_p2, mesh_p->lasttri_p);
         elcount--;

         /* continue with next tri (tri_p)                                      */
         continue;

      }
      /* unique triangle                                                       */
      insert_triangle(tri_p);
      tri_p = tri_p->ntri_p;
   }

   mesh_p->gltr += elcount;
  
   if(j) 
   {
      msg_info1("%i triangles have been erased (multiple existence) \n", j);
   }
   msg_info1("%i triangles have been inserted \n", elcount);
   msg_info1("%i triangles\n", mesh_p->gltr);
   msg_info1("%i points in global octree \n", 
             mesh_p->global_octree.octree_counter);
  
   return elcount;

}
 
