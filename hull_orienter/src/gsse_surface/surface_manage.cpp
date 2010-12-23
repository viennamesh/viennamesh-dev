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
#include "global_defines.hpp"


mesh_p_t global_init() 
{
   mesh_p_t mesh_p;

   mesh_p =  (mesh_p_t) my_malloc(sizeof(mesh_object_t));

   mesh_p->grid.mattop_tri_p = null;
   mesh_p->grid.mattop_tet_p = null;

   mesh_p->grid.matn = 0;
   mesh_p->grid.material_map.vp = null; 

   mesh_p->grid.unified_top_tet.basetri_p = null;            
   mesh_p->grid.unified_top_tet.dpp4 = null;                 
   mesh_p->grid.unified_top_tet.ntet_p = null;               
   mesh_p->grid.unified_top_tet.ptet_p = null;               
   mesh_p->grid.unified_top_tet.mat_ind = init_material_index;

   mesh_p->grid.unified_top_tri.ftet_p = null;
   mesh_p->grid.unified_top_tri.btet_p = null;
   mesh_p->grid.unified_top_tri.dppp[0] = null;
   mesh_p->grid.unified_top_tri.dppp[1] = null;
   mesh_p->grid.unified_top_tri.dppp[2] = null;
   mesh_p->grid.unified_top_tri.ntri_p = null;
   mesh_p->grid.unified_top_tri.ptri_p = null;
   mesh_p->grid.unified_top_tri.ctag = 0;

   // pointers to the last elements
   //
   mesh_p->lasttri_p = &mesh_p->grid.unified_top_tri;
   mesh_p->lasttet_p = &mesh_p->grid.unified_top_tet;

   // global data
   //
   //  overall triangle and tetrahedra counters
   //
   mesh_p->gltr = 0;
   mesh_p->glte = 0;
   mesh_p->glsl = 0;

   //  error counters
   //
   mesh_p->error_h = 0;
   mesh_p->error_v = 0;
   mesh_p->error_r = 0;

   // swap  recursion control
   //
   mesh_p->fliprecdepth = 0;
   mesh_p->fliprecstate = 0;
   mesh_p->fliprec_endless = 0;
   mesh_p->fliprectri_p = null;

   // 
   mesh_p->fph = null;
   mesh_p->fpv = null;


   // order matters... 
   //

   boundingbox_init(&mesh_p->global_octree);
   octree_init     (&mesh_p->global_octree);

   // api functions
   //
   mesh_p->PD.findmaterials = -1;
   mesh_p->PD.sdel_par = -1.0;
   mesh_p->PD.minflipang_par = -1.0;
   mesh_p->PD.maxarea_par = -1.0;
   mesh_p->PD.minsnap_par = -1.0;

   // epsilons
   //
   param_init(mesh_p, null);

   // surface initialization
   //
   surf_init();

   array_init(&mesh_p->leaktris, D_AR, sizeof(tri_pt));
   array_init(&mesh_p->seed_list, D_AR, sizeof(struct seed_def));
   array_init(&mesh_p->pc_gen.box_list, D_AR, sizeof(struct box_def));
   array_init(&mesh_p->pc_gen.bnx_list, D_AR, sizeof(struct bnx_def));
   array_init(&mesh_p->pc_gen.bnd_list, D_AR, sizeof(struct bnd_def));
   array_init(&mesh_p->pc_gen.srf_list, D_AR, sizeof(struct srf_def));
   mesh_p->pc_gen.hom = 0;
   mesh_p->pc_gen.inpsnap = 0;

   return mesh_p;

}



void global_quit(mesh_p_t mesh_p) {
  
   mesh_p->fph = null;
   mesh_p->fpv = null;

   free_mesh(mesh_p);     
   free_surfmod();
   free(mesh_p->leaktris.vp);
   free(mesh_p->seed_list.vp);
   free(mesh_p->pc_gen.box_list.vp);
   free(mesh_p->pc_gen.bnx_list.vp);
   free(mesh_p->pc_gen.bnd_list.vp);
   free(mesh_p->pc_gen.srf_list.vp);
  
   free(mesh_p);
}




void global_reset(mesh_p_t mesh_p) 
{
   free_mesh(mesh_p);     

   mesh_p->grid.mattop_tri_p = null;
   mesh_p->grid.mattop_tet_p = null;

   mesh_p->grid.matn = 0;
   mesh_p->grid.material_map.vp = null; 
  
   mesh_p->error_h = 0;
   mesh_p->error_v = 0;
   mesh_p->error_r = 0;
   mesh_p->fliprecdepth = 0;
   mesh_p->fliprecstate = 0;
   mesh_p->fliprec_endless = 0;
   mesh_p->fliprectri_p = null;

   mesh_p->PD.findmaterials = -1;
   mesh_p->PD.sdel_par = -1.0;
   mesh_p->PD.minflipang_par = -1.0;
   mesh_p->PD.maxarea_par = -1.0;
   mesh_p->PD.minsnap_par = -1.0;

   mesh_p->leaktris.n = 0;
   mesh_p->seed_list.n = 0;
  
   mesh_p->pc_gen.box_list.n = 0;
   mesh_p->pc_gen.bnx_list.n = 0;
   mesh_p->pc_gen.bnd_list.n = 0;
   mesh_p->pc_gen.srf_list.n = 0;
   mesh_p->pc_gen.hom = 0;
   mesh_p->pc_gen.inpsnap = 0;

}


void boundingbox_init(struct octree_def* oct_p) 
{
   FILE* fp;
   double valind;
   char charin[LONGEST_CHARIN];
   int j;

   char* named[7 + 1] = {
      "XMIN", 
      "XMAX",
      "YMIN", 
      "YMAX",
      "ZMIN", 
      "ZMAX",
      "OCTRES",
      "end" 
   }; 
  
   double* pointerd[7];

   pointerd[0] = &oct_p->bbminimum.x; 
   pointerd[1] = &oct_p->bbmaximum.x;
   pointerd[2] = &oct_p->bbminimum.y; 
   pointerd[3] = &oct_p->bbmaximum.y;
   pointerd[4] = &oct_p->bbminimum.z; 
   pointerd[5] = &oct_p->bbmaximum.z;
   pointerd[6] = &oct_p->octres;
  

   oct_p->bbminimum.x = dXMIN;
   oct_p->bbmaximum.x = dXMAX;
   oct_p->bbminimum.y = dYMIN;
   oct_p->bbmaximum.y = dYMAX;
   oct_p->bbminimum.z = dZMIN;
   oct_p->bbmaximum.z = dZMAX;
   oct_p->octres = dOCTRES;
}



void octree_init(struct octree_def* oct_p) 
{
   int exp;
   int maxexp;
  
   oct_p->topnode.u[0].child = null;
   oct_p->topnode.u[1].child = null;
   oct_p->topnode.u[2].child = null;
   oct_p->topnode.u[3].child = null;
   oct_p->topnode.u[4].child = null;
   oct_p->topnode.u[5].child = null;
   oct_p->topnode.u[6].child = null;
   oct_p->topnode.u[7].child = null;
   oct_p->topnode.tag = 0;
   oct_p->octree_counter = 0;

   if(oct_p->octres == 0.0) 
   {
      msg_dinfo("invalid zero octree resolution is changed \n");
      oct_p->octres = 0.000001;
   }

   frexp(LONG_MAX, &maxexp);
   if(maxexp <16 ) 
   {
      frexp(INT_MAX, &maxexp);
   }
   if(maxexp < 16) 
   {
      maxexp = 16;
   }

   frexp((oct_p->bbmaximum.x-oct_p->bbminimum.x)/oct_p->octres, &exp);
   if(exp < 15) exp = 15;

   // maximum depth is machine dependent
   //
   if(exp >= maxexp) exp = maxexp-1;
   oct_p->maxdepth = exp;
   oct_p->maxnodes = (1l<<exp);

   if(oct_p->maxnodes <= 0) 
   {
      exp = 15;
      oct_p->maxdepth = exp;
      oct_p->maxnodes = (1l<<exp);
   }
  
   oct_p->step_size_x =(oct_p->bbmaximum.x-oct_p->bbminimum.x)/oct_p->maxnodes; 
   oct_p->step_size_y =(oct_p->bbmaximum.y-oct_p->bbminimum.y)/oct_p->maxnodes; 
   oct_p->step_size_z =(oct_p->bbmaximum.z-oct_p->bbminimum.z)/oct_p->maxnodes;
}




#define NUMBER_d    18     // total number of double parameters/epsilons
#define NUMBER_i    10     // total number of integer parameters


void param_init(mesh_p_t mesh_p, FILE* fpinput) 
{
   FILE* fp;
   double valind;
   int valini;
   char charin[LONGEST_CHARIN];
   int j;

   static double MINFLIPANG_PAR;
   static double MAXAREA_PAR;
   static double MINANG_PAR;

   double spanx;
   double spany;
   double spanz;
   double stepx;
   double stepy;
   double stepz;

   /* array for names and pointers                                            */
   char*      named[NUMBER_d+1] = {
      "lambda_start",
      "lambda_max",

      "pN_EPS",
      "N_EPS",  
      "R_EPS",
      "L_BEPS",
      "L_SEPS",
      "M_BEPS",
      "VOL_EPS",
      "WSBREF_EPS",
			      
      "MINL_PAR",
      "SDEL_PAR",
      "MINSNAP_PAR",
      "SPROJ_PAR",
      "MAXSNAP_PAR",
      "MINFLIPANG_PAR",
      "MAXAREA_PAR",
      "MINANG_PAR",

      "end"
   };        
  
   double* pointerd[NUMBER_d];

   char* namei[NUMBER_i+1] = {
      "RELAXR_PAR",
      "RELAXC_PAR",
      "OCTAEDER",
      "ALIGNEDPTS",
      "REPHOLTHR",
      "VTKONEFILE",
      "EQUATORSPH",
      "FINDMATERIALS",
      "FLIP_GNORMALS",
      "REFINE_GLINES",
      "end"
   };

   int* pointeri[NUMBER_i];
  

   pointerd[0]  =   &mesh_p->lambda_start;
   pointerd[1]  =   &mesh_p->lambda_max;

   pointerd[2]  =   &mesh_p->pN_EPS;
   pointerd[3]  =   &mesh_p->N_EPS;  
   pointerd[4]  =   &mesh_p->R_EPS;
   pointerd[5]  =   &mesh_p->L_BEPS;
   pointerd[6]  =   &mesh_p->L_SEPS;
   pointerd[7]  =   &mesh_p->M_BEPS;
   pointerd[8]  =   &mesh_p->VOL_EPS;
   pointerd[9]  =   &mesh_p->WSBREF_EPS;

   pointerd[10] =   &mesh_p->MINL_PAR;
   pointerd[11] =   &mesh_p->SDEL_PAR;
   pointerd[12] =   &mesh_p->MINSNAP_PAR;
   pointerd[13] =   &mesh_p->SPROJ_PAR;
   pointerd[14] =   &mesh_p->MAXSNAP_PAR;
   pointerd[15] =   &MINFLIPANG_PAR;
   pointerd[16] =   &MAXAREA_PAR;
   pointerd[17] =   &MINANG_PAR;

   pointeri[0] =  &mesh_p->RELAXR_PAR; 
   pointeri[1] =  &mesh_p->RELAXC_PAR; 
   pointeri[2] =  &mesh_p->OCTAEDER; 
   pointeri[3] =  &mesh_p->ALIGNEDPTS; 
   pointeri[4] =  &mesh_p->REPHOLTHR; 
   pointeri[5] =  &mesh_p->VTKONEFILE; 
   pointeri[6] =  &mesh_p->EQUATORSPH; 
   pointeri[7] =  &mesh_p->FINDMATERIALS; 
   pointeri[8] =  &mesh_p->FLIP_GNORMALS; 
   pointeri[9] =  &mesh_p->REFINE_GLINES; 


   //  initialize identifiers before the macros
   spanx = mesh_p->global_octree.bbmaximum.x - 
      mesh_p->global_octree.bbminimum.x;
   spany = mesh_p->global_octree.bbmaximum.y - 
      mesh_p->global_octree.bbminimum.y;
   spanz = mesh_p->global_octree.bbmaximum.z - 
      mesh_p->global_octree.bbminimum.z;
   stepx = mesh_p->global_octree.step_size_x;
   stepy = mesh_p->global_octree.step_size_y;
   stepz = mesh_p->global_octree.step_size_z;


   //  set to the default values
   mesh_p->lambda_start =      dlambda_start;
   mesh_p->lambda_max =        dlambda_max;
  
   mesh_p->pN_EPS =            dpN_EPS; 
   mesh_p->N_EPS =             dN_EPS;         
   mesh_p->R_EPS =             dR_EPS;
   mesh_p->L_BEPS =            dL_BEPS;     
   mesh_p->L_SEPS =            dL_SEPS;   
   mesh_p->M_BEPS =            dM_BEPS;             
   mesh_p->VOL_EPS =           dVOL_EPS;
   mesh_p->WSBREF_EPS =        dWSBREF_EPS;
  
   mesh_p->SDEL_PAR =          dSDEL_PAR;
   mesh_p->MINSNAP_PAR =       dMINSNAP_PAR;
   mesh_p->SPROJ_PAR =         dSPROJ_PAR;
   mesh_p->MINL_PAR =          dMINL_PAR;
   mesh_p->MAXSNAP_PAR =       dMAXSNAP_PAR;
   MINFLIPANG_PAR =    dMINFLIPANG_PAR;
   MAXAREA_PAR =       dMAXAREA_PAR;
   MINANG_PAR =        dMINANG_PAR;

   mesh_p->RELAXR_PAR =        dRELAXR_PAR;
   mesh_p->RELAXC_PAR =        dRELAXC_PAR;
   mesh_p->OCTAEDER =          dOCTAEDER;
   mesh_p->ALIGNEDPTS =        dALIGNEDPTS;
   mesh_p->REPHOLTHR =         dREPHOLTHR;
   mesh_p->VTKONEFILE =        dVTKONEFILE;
   mesh_p->EQUATORSPH =        dEQUATORSPH;
   mesh_p->FINDMATERIALS =     dFINDMATERIALS;
   mesh_p->FLIP_GNORMALS =     dFLIP_GNORMALS;
   mesh_p->REFINE_GLINES =     dREFINE_GLINES;
    

   //  override the default values with the api defaults
   //
   if(mesh_p->PD.findmaterials >= 0)          
      mesh_p->FINDMATERIALS =  mesh_p->PD.findmaterials;
   if(mesh_p->PD.sdel_par >= 0.0)          
      mesh_p->SDEL_PAR =       mesh_p->PD.sdel_par;
   if(mesh_p->PD.minflipang_par >= 0.0) 
      MINFLIPANG_PAR =         mesh_p->PD.minflipang_par;
   if(mesh_p->PD.maxarea_par >= 0.0) 
      MAXAREA_PAR =            mesh_p->PD.maxarea_par;
   if(mesh_p->PD.minsnap_par >= 0.0) 
      mesh_p->MINSNAP_PAR =    mesh_p->PD.minsnap_par;

   mesh_p->COSP_PAR = cos(MINFLIPANG_PAR * 2.0 * PI/360.0);
   mesh_p->MAXAR2_PAR = MAXAREA_PAR * MAXAREA_PAR;
   mesh_p->MAXCOS_PAR = cos(MINANG_PAR * 2.0 * PI/360.0);
}




int array_init(struct arraydef* ar_p, int initsize, long int itemsize) 
{
   ar_p->vp = my_malloc(initsize * itemsize);
   ar_p->max = initsize;
   ar_p->n = 0;
   ar_p->itemsize = itemsize;

   return 0;
}


int expand_ar(struct arraydef* ar_p, int plus) 
{
   void *vp;
  
   /* msg_dinfo1("Expanding array ! (itemsize=%i) \n", ar_p->itemsize); */  
   vp = realloc(ar_p->vp, (ar_p->max+plus) * ar_p->itemsize);

   if(vp==null) {
      msg_fatal("+FATAL+: OUT OF MEMORY \n");
      exit(0);
   }

   ar_p->vp = vp;
   ar_p->max += plus;

   return 0;

}



// reset all point indices first
//
int init_pointlist(tet_pt toptet_p, struct octree_def* oct_p, int* tetn) 
{
   tet_pt tet_p;
   int pn;
   int tn;
   int i;

   // reset all point incices
   //
   ext_find(&oct_p->bbminimum, &oct_p->bbmaximum,  cb_reset_index,  null, oct_p, null);
  
   pn = 0;
   tn = 0;
   for(tet_p=toptet_p->ntet_p ; tet_p!=null ; tet_p=tet_p->ntet_p) 
   {
      for(i=0 ; i<3 ; i++) 
      {
         if(tet_p->basetri_p->dppp[i]->point_index == init_point_index) 
         {
            tet_p->basetri_p->dppp[i]->point_index = pn;
            pn++;
         }
      }
      if(tet_p->dpp4->point_index == init_point_index) 
      {
         tet_p->dpp4->point_index = pn;
         pn++;
      }
      tn++;
   }

   // tn:   number of tetrahedra
   // pn:   different points
   // pn-1: max. point index
   *tetn = tn;

   return pn;
}





//  Allocates array of top_tets for grid structure and initializes pointers  
//
void init_grid(struct griddef* grid_p, struct arraydef* seedlist_p) 
{
   int i;
   int highest;

   // material map has to be generated first
   //

   //  determine highest material index
   //
   highest = 0;
   for(i=0 ; i < seedlist_p->n ; i++) 
   {
      if( ((struct seed_def*)(seedlist_p->vp))[i].material_index > highest ) 
      {
         highest = ((struct seed_def*)(seedlist_p->vp))[i].material_index;
      }
   }

   // initialize material map
   //
   array_init(&grid_p->material_map, highest + 1, sizeof(int));

   // use the invalid init_material_index to mark non existent array elements 
   //  
   for(i=0 ; i < grid_p->material_map.max ; i++) 
   {
      ((int*)(grid_p->material_map.vp))[i] = init_material_index;
   }
   grid_p->material_map.n = grid_p->material_map.max;

   // generate the mapped array indices
   //
   grid_p->matn = 0;
   for(i=0 ; i < seedlist_p->n ; i++) 
   {
      if(mapped_list_ind((*grid_p),  ((struct seed_def*)(seedlist_p->vp))[i].material_index) == init_material_index) 
      {
         // new material
         //
         mapped_list_ind((*grid_p), ((struct seed_def*)(seedlist_p->vp))[i].material_index) = grid_p->matn;
         grid_p->matn++;

      }
   }
   // number of materials is known
   //
   // allocate memory
   //
   grid_p->mattop_tet_p = (tet_pt) my_malloc(grid_p->matn *    sizeof(struct tetraederdef));
   grid_p->mattop_tri_p = (tri_pt) my_malloc(grid_p->matn * grid_p->matn *  sizeof(struct triangledef));

   for(i=0 ; i<grid_p->matn ; i++) 
   {
      grid_p->mattop_tet_p[i].basetri_p = null;
      grid_p->mattop_tet_p[i].dpp4 = null;
      grid_p->mattop_tet_p[i].ntet_p = null;
      grid_p->mattop_tet_p[i].ptet_p = null;
      grid_p->mattop_tet_p[i].mat_ind = init_material_index;
   }

   for(i=0 ; i < (grid_p->matn*grid_p->matn) ; i++) 
   {
      grid_p->mattop_tri_p[i].ftet_p = null;
      grid_p->mattop_tri_p[i].btet_p = null;
      grid_p->mattop_tri_p[i].dppp[0] = null;
      grid_p->mattop_tri_p[i].dppp[1] = null;
      grid_p->mattop_tri_p[i].dppp[2] = null;
      grid_p->mattop_tri_p[i].ntri_p = null;
      grid_p->mattop_tri_p[i].ptri_p = null;
      grid_p->mattop_tri_p[i].ctag = 0;
   }


}




/* Used by function free_mesh                                                */
static int cb_free_points(ppt, void*, mesh_p_t);

void free_mesh(mesh_p_t mesh_p) 
{
   tri_pt top_tri_p;
   tet_pt top_tet_p;
   void* lastdummy_p;
   tri_pt ntri_p;
   tet_pt ntet_p;
   int i;


   // 2-simplex
   //
   top_tri_p = &mesh_p->grid.unified_top_tri;
   ntri_p = top_tri_p->ntri_p;

   // initialize top element
   //
   top_tri_p->ntri_p = null; 

   while(ntri_p != null) 
   {
      top_tri_p = ntri_p;     
      ntri_p = ntri_p->ntri_p;
      erase_triangle(top_tri_p, mesh_p->lasttri_p);
      mesh_p->gltr--;
   }

   //  boundary and interface lists
   //
   if(mesh_p->grid.mattop_tri_p != null) 
   {
      for(i=0 ; i < (mesh_p->grid.matn*mesh_p->grid.matn) ; i++) 
      {
         top_tri_p = mesh_p->grid.mattop_tri_p + i;
         ntri_p = top_tri_p->ntri_p; 
    
         top_tri_p->ntri_p = null; 
      
         while(ntri_p != null) 
         {
            top_tri_p = ntri_p;     
            ntri_p = ntri_p->ntri_p;
            erase_triangle(top_tri_p, lastdummy_p);
            mesh_p->gltr--;
         }
      }

      free(mesh_p->grid.mattop_tri_p);
      mesh_p->grid.mattop_tri_p = null;
   }


   // 0-simplex
   //
   ext_del(&mesh_p->global_octree.bbminimum, &mesh_p->global_octree.bbmaximum, cb_free_points, null, &mesh_p->global_octree, null);


   // materials
   //
   if(mesh_p->grid.material_map.vp != null) 
   {
      free(mesh_p->grid.material_map.vp);
      mesh_p->grid.matn = 0;
      mesh_p->grid.material_map.vp = null; 
   }


}

void* my_malloc(size_t SIZE) 
{
   void* vp;

   vp = malloc(SIZE);
   if(vp==null) 
   {
      msg_fatal("+FATAL+: OUT OF MEMORY \n");
      exit(0);
   }

   return vp;

}


// erases points including the attached triangle lists  
//
static int cb_free_points(ppt dpp, void* vp, mesh_p_t nup) 
{
   struct tri_listdef* ntri_lp;
   struct tri_listdef* h_lp;

   ntri_lp = dpp->ntri_lp;
   while(ntri_lp != null) {
      h_lp = ntri_lp;
      ntri_lp = ntri_lp->ntri_lp;
      free(h_lp);
   }
  
   erase_point(dpp);

   return 1;
}




