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
//   (c)  2003, Peter Fleischmann


#define ON 1
#define OFF 0

typedef struct mesh_object_t_def mesh_object_t;
typedef mesh_object_t* mesh_p_t;

extern mesh_p_t global_init();
extern void global_reset(mesh_p_t);



/** debugging ****************************************************************/
#define ONLINE_VISUALIZATION       OFF
#define CONTOUR_FILE_OUTPUT        OFF
#define ERROR_FILE_OUTPUT          ON


/** verbosity ****************************************************************/
/* Warnings do not invalidate the mesh but are a sign that undesired things  */
/* happen. Errors occur when files cannot be read or written or might        */
/* indicate that the output is inconsistent. Fatals result in an exit().     */
/* Debug information produces text output for almost every step.             */
/* Infos are useful text outputs about the current status.                   */

#ifdef DEBUG
#define VERBOSE_WARNINGS           ON
#define VERBOSE_ERRORS             ON
#define VERBOSE_FATALS             ON
#define VERBOSE_DEBUG              ON
#define VERBOSE_INFOS              ON
#endif

/** select level of influence ************************************************/
/*    0      stop asking annoying questions                                  */
/*           (except when functions for MANUAL inputs are called on purpose.)*/
/*           File input and of course the library API function including     */
/*           DLKMakeMesh() are automatic.                                    */
/*    1      full influence                                                  */
#define INTERACTIVE_LEVEL          0


/** select version of algorithm **********************************************/
/*    0      no specials (obsolete)                                          */
/*    1      special 1 only (obsolete)                                       */
/*    2      special 1 and special 2  (required for general point clouds)    */
#define VERSION                    2


/** perform an extra check on the input for points which intersect edges *****/
/* and split such edges                                                      */
#define LINESPLIT                  OFF


/** PIF output (requires PIF libraries) **************************************/
#define PIF                        OFF


#define null NULL      
#define dfmt "%+#-24.17g"
#define LONGEST_CHARIN    80
#ifndef PI
#define PI 3.141592653589793238462643383279502884197169399375105820974944592308
#endif

/* extra bounding box cushion for outside points from pcgen                  */
/* and excluding upper octree bounds as well as minimum buf if span is zero  */
#define BBCUSHIONPERCENT 0.08  
#define BBZEROBUF 0.00001  

/* the minimum square length of vector n                                     */
/* used in nonconvex_triangulate, find n to be valid for a general polygon   */
/* used in sm_locate to follow an edge                                       */
/* and debugging safety check of collinear tris in insert_triangle           */
/* and for collinear tri tests                                               */
#define MINN2_EPS          (3 * DBL_EPSILON)


/** pdraw linkage for online visualization ***********************************/
#if ONLINE_VISUALIZATION == ON
#define graphical_debug(A,B) debug_start(A,B)
#else
#define graphical_debug(A,B) /* debug_start(A,B)                             */
#endif

/** verbose levels ***********************************************************/
#if VERBOSE_INFOS == ON
#define msg_info(F) {printf(F);}		       
#define msg_info1(F,F1) {printf(F,F1);}		       
#define msg_info2(F,F1,F2) {printf(F,F1,F2);}		       
#define msg_info3(F,F1,F2,F3) {printf(F,F1,F2,F3);}		       
#else 
#define msg_info(F) /*{printf(F);}	                                     */
#define msg_info1(F,F1) /*{printf(F,F1);}		                     */
#define msg_info2(F,F1,F2) /*{printf(F,F1,F2);}		                     */
#define msg_info3(F,F1,F2,F3) /*{printf(F,F1,F2,F3);}		             */
#endif

#if VERBOSE_DEBUG == ON
#define msg_dinfo(F) {printf(F);}		       
#define msg_dinfo1(F,F1) {printf(F,F1);}		       
#define msg_dinfo2(F,F1,F2) {printf(F,F1,F2);}		       
#define msg_dinfo3(F,F1,F2,F3) {printf(F,F1,F2,F3);}		       
#else
#define msg_dinfo(F) /*{printf(F);}		                             */
#define msg_dinfo1(F,F1) /*{printf(F,F1);}		                     */
#define msg_dinfo2(F,F1,F2) /*{printf(F,F1,F2);}		             */
#define msg_dinfo3(F,F1,F2,F3) /*{printf(F,F1,F2,F3);}		             */
#endif

#if VERBOSE_WARNINGS == ON
#define msg_warn(F) {printf(F);}
#define msg_warn1(F,F1) {printf(F,F1);}
#define msg_warn3(F,F1,F2,F3) {printf(F,F1,F2,F3);} 
#else 
// #define msg_warn(F) {printf(F);}
// #define msg_warn1(F,F1) {printf(F,F1);}
// #define msg_warn3(F,F1,F2,F3) {printf(F,F1,F2,F3);} 
#define msg_warn(F) /*{printf(F);}                                           */
#define msg_warn1(F,F1) /*{printf(F,F1);}                                    */
#define msg_warn3(F,F1,F2,F3) /*{printf(F,F1,F2,F3);}                        */
#endif

#if VERBOSE_ERRORS == ON
#define msg_error(F) {printf(F);}		       
#define msg_error1(F,F1) {printf(F,F1);}		       
#else
#define msg_error(F) /*{printf(F);}		                             */
#define msg_error1(F,F1) /*{printf(F,F1);}		                     */
#endif

#if VERBOSE_FATALS == ON
#define msg_fatal(F) {printf(F);}		       
#define msg_fatal1(F,F1) {printf(F,F1);}		       
#else
#define msg_fatal(F) /*{printf(F);}		                             */
#define msg_fatal1(F,F1) /*{printf(F,F1);}		                     */
#endif

#define msg_interactive(F) {printf(F);}
#define msg_interactive1(F,F1) {printf(F,F1);}

   
/** macro for checking the bounding box **************************************/
#define inside_bb(_oct, _dpp) ( (_dpp)->x >= (_oct).bbminimum.x &&            \
                                (_dpp)->y >= (_oct).bbminimum.y &&            \
                                (_dpp)->z >= (_oct).bbminimum.z &&            \
                                (_dpp)->x <  (_oct).bbmaximum.x &&            \
                                (_dpp)->y <  (_oct).bbmaximum.y &&            \
                                (_dpp)->z <  (_oct).bbmaximum.z )


/** general initialization values ********************************************/
#define D_AR 50              /* default array length                         */

#define init_point_index -1  /* must be a non valid index                    */
                             /* initializes point_index in struct pointtype  */
                             /* temporarily used vor various indices         */
                             /* e.g. function relax()                        */

/* init_material_index must be a non valid index (negative)                  */
/* (only positive material markers during input are allowed)                 */
/* init_material_index-1, init_material_index-2 will be used to store the    */
/* opposite_ind in sliver tets !                                             */
/* opposite_ind must be positive, so substracting it from the negative       */
/* init_material_index yields a still negative index.                        */
/* This is important to keep the indices of slivers invalid. They must be    */
/* detected e.g. during insert_advancing_tetrahedron and rcheck !!!          */
#define init_material_index -1   


/* facemarks store in their sign an orientation for the marked triangles.    */
/* SO negative facemarks are allowed and generated e.g. in read_geom() when  */
/* the polygon index is negative or in the DLK_HI when the facemark is given.*/
/* BY default when checked for flipping the negativ counterpart will count as*/
/* the same facemark and allow flipping. OUTPUT will always give positive    */
/* facemarks (the application might not even know that some facemarks where  */
/* ~ed due to negative polygon indices)!! Also when writing to file all both */
/* positive and negative will be written together.                           */
/* IMPORTANT: To store 'unmarked' triangles which do not have a specific     */
/* facemark simply use 0 or -1!! SO 0/-1 is RESERVED for unmarked triangles. */
/* So flipping works within unmarked tris as well as not between unmarked and*/
/* marked tris. When in fact facemarks should be disregarded (e.g. in main   */
/* interactive mode for writing) '-1' can be used as flag to *disregard* the */
/* facemark information. (if fm==~init_facemark no comparison) NOW if one    */
/* desires all unmarked tris to be chosen simply use '0' !! (==init_facemark */
/* and will equal the '0' facemarks)                                         */
/* CONCLUSIO: all facemarks are possible... 0/-1 is used for new triangles   */
/* and in this sense unmarked. 0/-1 can be given but will be mixed with other*/
/* triangles that are created..                                              */
/* The feature of distinguishing ALL from UNMARKED (-1 vs. 0) is so far used */
/* in save_tris() and surfmod.c but could be used in pc_gen as well for the  */
/* bnd, bnx. No where else should be "if(fm==~init_facemark)" because in fact*/
/* it is just a regular marker! (It will never make sense to allow flipping  */
/* between unmarked and any marked tri!)                                     */
/* Exception: during input for unmarked it makes sense to *not* correct the  */
/* facemark according to the polygon index sign, because they should all have*/
/* the same sign, due to the correction of the polygons itself anyway!       */
/* Then can actually use 0/-1 for bnd, bnx as well! (otherwise not!)         */
/* So for the umarked *and* 'i' case (no bnd fm given) the facemarks will be */
/* 0 and therefore the orientation correct if the input polys were correct!  */
/* To choose the positive orientation with a bnd ('a') use fm 0!             */
/* ALSO ~init_facemark never exists? because will not be adjusted and no     */
/* where else changed not even when reading a facemark section from file.    */
/* Only way to have ~init_facemark is when explicitly given in api or as fm! */
#define init_facemark 0     



/** macro to map a material index to an array index **************************/
/* Maps the given material index to an index which allows access to the      */
/* corresponding material list in the grid structure.                        */
/* If the material index is invalid, the result is 'init_material_index'     */
#define get_mapped_list_ind(_grid, _mat_ind)                                  \
   ( ((_mat_ind)>=0 && (_mat_ind)<(_grid).material_map.max) ?                 \
     (((int*)((_grid).material_map.vp))[(_mat_ind)]) : init_material_index  )


/** macro to access an array index given a material index ********************/
/* The material index must be within range !                                 */
/* Results in the stored list index without testing or is lvalue to set.     */
#define mapped_list_ind(_grid, _mat_ind)                                      \
   (  ((int*)((_grid).material_map.vp))[(_mat_ind)]  )


/** macro for first initialization of a point_class **************************/
/* Initializes gline arrays and sets default values                          */
/* also sets the index in the point                                          */
#define init_pclass(_pcl, _dpp, _index) {                                     \
   array_init(&((_pcl).pri_gline), 2, sizeof(ppt)); 			      \
   array_init(&((_pcl).sec_gline), 2, sizeof(ppt)); 			      \
   (_pcl).dpp = _dpp;                            			      \
   (_pcl).ptype = 0;                             			      \
   (_pcl).dist = 0.0;                            			      \
   (_dpp)->point_index = _index; }

/** macro for freeing of a point_class ***************************************/
/* Frees gline arrays and resets point_index                                 */
#define free_pclass(_pcl) {                                                   \
   free((_pcl).pri_gline.vp); 			                              \
   free((_pcl).sec_gline.vp);                      			      \
   (_pcl).dpp->point_index = init_point_index; }


/** macro to change the position of a triangle in a double linked list *******/
/* The first argument specifies the triangle which should be re-positioned.  */
/* The second argument specifies the destination. The triangle will be       */
/* re-positioned to the location right after the destination.                */
/*                                                                           */
/* CAUTION: this is a macro and not a function !! Thus, the arguments are    */
/* not values that are copied into local automatic variables, instead the    */
/* variables of the caller are directly used ! Therefore, it is not allowed  */
/* to use pointers as arguments that are dependent on the position of the    */
/* triangle in the double linked list. Since the position will be changed    */
/* the values of such pointers would loose their correct meaning. So,        */
/* any argument that contains ntri_p or ptri_p is prohibited !               */
/* For instance    position_triangle(tri_p->ntri_p, lasttri_p)               */
/*                 position_triangle(tri_p, lasttri_p->ptri_p)               */
/* is not allowed, and would produce garbage.                                */
/* Also, the value of srctri_p and destri_p may not be the same !!           */
/* (This would not make sense anyhow and needs to be checked by the caller)  */
/* Also, the empty top element of a list cannot be the first argument,       */
/* since re-positioning the top element would not make sense.                */
/*                                                                           */
/* Valid calls to position_triangle are for instance:                        */
/*                 position_triangle(tri_p, lasttri_p)                       */
/*                 position_triangle(tri_p, toptri_p)                        */
/*                 position_triangle(lasttri_p, toptri_p)                    */
/*                 position_triangle(tri_p1, tri_p2)                         */
/*                    (tri_p1 and tri_p2 can be neighbors)                   */
/*                                                                           */
/* Of course it does not matter if triangles are re-positioned to other lists*/
/*                                                                           */
/* The caller must take care of any pointers referencing re-positioned       */
/* triangles. Only the caller knows which pointers exist and which need to   */
/* be maintained.                                                            */
/* For instance:                                                             */
/*  In case of the unified triangle list, the LASTTRI_P must be updated.     */
/*  During a LOOP, the current triangle pointer should be stored.            */
/*  For tetrahedrization the STACKTRI_P must be updated.                     */
/*  Generally, order specific pointers need to be memorized/updated.         */
/*  Triangle specific pointers will usually not need to be maintained.       */
/*  (e.g. seedtri_p and all the back references from points and tets)        */
/*                                                                           */
#define position_triangle(_srctri_p, _destri_p) {                             \
  /* Take out at source location                                            */\
  (_srctri_p)->ptri_p->ntri_p = (_srctri_p)->ntri_p; /* might be null       */\
  if((_srctri_p)->ntri_p != null) {                                           \
    (_srctri_p)->ntri_p->ptri_p = (_srctri_p)->ptri_p;                        \
  }                                                                           \
  /* Insert at destination location                                         */\
  (_srctri_p)->ntri_p = (_destri_p)->ntri_p; /* might be null               */\
  if((_srctri_p)->ntri_p != null) {                                           \
    (_srctri_p)->ntri_p->ptri_p = (_srctri_p);                                \
  }                                                                           \
  (_destri_p)->ntri_p = (_srctri_p);                                          \
  (_srctri_p)->ptri_p = (_destri_p);}
  


/** macro to find the point of a backside connected tetrahedron **************/
/* To find the point of the connected tet, only the basetri of the tet       */
/* has to be checked, because it must contain the point since it cannot be   */
/* the same triangle as _tri_p.                                              */
/* Can reduce the number of ifs by relying on the orientation and the        */
/* consistency of the topology (independently of negative volumes!).         */
/* (It is a given fact that the tet is connected to btet_p of _tri_p and     */
/*  to the ftet_p of its basetri!)                                           */
/*                                                                           */
/* Will not check if btet_p != null !!!                                      */
/* Thus, will always set _dpp                                                */
/*                                                                           */
#define find_backconnected_tetpoint(_tri_p, _dpp) {                           \
  if((_tri_p)->btet_p->basetri_p->dppp[0] == (_tri_p)->dppp[0]) {	      \
    /* point 0 of btet-basetri is triangle point 0                          */\
    if((_tri_p)->btet_p->basetri_p->dppp[1] == (_tri_p)->dppp[1]) {           \
      _dpp = (_tri_p)->btet_p->basetri_p->dppp[2];		      	      \
    }								      	      \
    else {								      \
      _dpp = (_tri_p)->btet_p->basetri_p->dppp[1];			      \
    }								      	      \
  }								      	      \
  else if((_tri_p)->btet_p->basetri_p->dppp[0] == (_tri_p)->dppp[1]) {        \
    /* point 0 of btet-basetri is triangle point 1                          */\
    if((_tri_p)->btet_p->basetri_p->dppp[1] == (_tri_p)->dppp[2]) {           \
      _dpp = (_tri_p)->btet_p->basetri_p->dppp[2];			      \
    }								      	      \
    else {								      \
      _dpp = (_tri_p)->btet_p->basetri_p->dppp[1];			      \
    }								      	      \
  }                                                                           \
  else if((_tri_p)->btet_p->basetri_p->dppp[0] == (_tri_p)->dppp[2]) {        \
    /* point 0 of btet-basetri is triangle point 2                          */\
    if((_tri_p)->btet_p->basetri_p->dppp[1] == (_tri_p)->dppp[0]) {           \
      _dpp = (_tri_p)->btet_p->basetri_p->dppp[2];			      \
    }								      	      \
    else {								      \
      _dpp = (_tri_p)->btet_p->basetri_p->dppp[1];			      \
    }								      	      \
  }								      	      \
  else {								      \
    _dpp = (_tri_p)->btet_p->basetri_p->dppp[0];			      \
  }								      	      \
}


  
/** macro to find the point of a frontside connected tetrahedron *************/
/* Can reduce the number of ifs by relying on the orientation and the        */
/* consistency of the topology (independently of negative volumes!).         */
/* (It is a given fact that the tet is connected to ftet_p of _tri_p and     */
/*  to the ftet_p of its basetri!)                                           */
/*                                                                           */
/* Will not check if ftet_p != null !!!                                      */
/* Thus, will always set _dpp                                                */
/*                                                                           */
#define find_frontconnected_tetpoint(_tri_p, _dpp) {                          \
  if((_tri_p)->ftet_p->basetri_p == (_tri_p)) {				      \
    _dpp = (_tri_p)->ftet_p->dpp4;					      \
  }									      \
  else {								      \
    if((_tri_p)->ftet_p->basetri_p->dppp[0] == (_tri_p)->dppp[0]) {	      \
      /* point 0 of ftet-basetri is triangle point 0                        */\
      if((_tri_p)->ftet_p->basetri_p->dppp[1] == (_tri_p)->dppp[2]) {         \
	_dpp = (_tri_p)->ftet_p->basetri_p->dppp[2];		      	      \
      }								      	      \
      else {								      \
	_dpp = (_tri_p)->ftet_p->basetri_p->dppp[1];			      \
      }								      	      \
    }								      	      \
    else if((_tri_p)->ftet_p->basetri_p->dppp[0] == (_tri_p)->dppp[1]) {      \
      /* point 0 of ftet-basetri is triangle point 1                        */\
      if((_tri_p)->ftet_p->basetri_p->dppp[1] == (_tri_p)->dppp[0]) {         \
	_dpp = (_tri_p)->ftet_p->basetri_p->dppp[2];			      \
      }								      	      \
      else {								      \
	_dpp = (_tri_p)->ftet_p->basetri_p->dppp[1];			      \
      }								      	      \
    }                                                                  	      \
    else if((_tri_p)->ftet_p->basetri_p->dppp[0] == (_tri_p)->dppp[2]) {      \
      /* point 0 of ftet-basetri is triangle point 2                        */\
      if((_tri_p)->ftet_p->basetri_p->dppp[1] == (_tri_p)->dppp[1]) {         \
	_dpp = (_tri_p)->ftet_p->basetri_p->dppp[2];			      \
      }								      	      \
      else {								      \
	_dpp = (_tri_p)->ftet_p->basetri_p->dppp[1];			      \
      }								      	      \
    }								      	      \
    else {								      \
      _dpp = (_tri_p)->ftet_p->basetri_p->dppp[0];			      \
    }								      	      \
  }                                                                           \
}

  

/** CTAGNAMES for triangles **************************************************/
/*'CTG_IFACE', 'CTG_BFACE', 'CTG_NEW' are contained in  4 bits defining      */
/*  the connectivity of a triangle. Important during tetrahedrization.	     */
/*  Through these 4 bits correct or double merges are determined, and it     */
/*  corresponds exactly with the existence of connected tetrahedra.          */
/*  Only if connected tetrahedra exist, is it possible to derive from their  */
/*  indices between which materials an interface triangle is located or to   */
/*  which material a triangle forms a boundary. That way, the interface can  */
/*  have any orientation and it does not require additionally stored info.   */
/*  Thus, interface/boundary triangles are sorted by examining the connected */
/*  tetrahedra and their indices, instead of checking 'CTG_SEGIFACE'.        */
/*  However, if additional memory would be spent (e.g. 2 integers describing */
/*  front and back segment index), it would become possible to detect/repair */
/*  unmeshed segments or derive interface triangles with no tetrahedra conn. */
/* When a triangle is not yet processed (bit0==0) then bit1 (value 2) must be*/
/*  0 as well! The triangle cannot have an unprocessed 'connection' on the   */
/*  frontside! This will be exploited in rcheck()                            */
/*  (In other words bit1 should not be undefined when bit0==0!)              */
/*                                                                           */
/*'CTG_SEGIFACE' is independent from the tetrahedrization process            */
/*  (used during file-load and output to distinguish real segment interfaces */
/*  from double connected triangles)                                         */
/*  Provides extra information to e.g. detect cases where a single input     */
/*  polyhedron (segment) is in fact formed by more than one independent      */
/*  physical polyhedra. In such cases only one of the several 'blocks' would */
/*  be meshed, because only one starttri ('CTG_SEED') is defined per segment.*/
/*  Then, triangles with 'CTG_SEGIFACE' have too few tetrahedra connected.   */
/*  More generally, if several segments share the same material index,       */
/*  interfaces between them can only be detected by checking 'CTG_SEGIFACE'  */
/*                                                                           */
/* (segment refers to an input polyhedron with exactly one starttri, whereas */
/*  material refers to a 'named' index set for each segment)                 */
/*                                                                           */
/*'CTG_SEGBFACE' is independent from the tetrahedrization process            */
/*  (set during file-load and input to distinguish real boundary triangles   */
/*   from declared input boundary triangles)                                 */
/*  When by mistake an input boundary triangle becomes an interface triangle */
/*  and local regridding is performed, delink must know which triangles to   */
/*  respect and which can be removed.                                        */
/*                                                                           */
/*After the tetrahedrization triangles might be 'blocked' or 'marked'.	     */
/*                                                                           */
/*'CTG_SEED' characterizes a *single* starttri per input segment.	     */
/*  If several starttris are required (e.g. for multiple independent blocks  */
/*  with same material), the segment must be subdivided in the input.        */
/*  The material index can be set to the same value, leading to a once again */
/*  unified output mesh.                                                     */
/*                                                                           */
/*Ctag copying occurs during refinement and in read_geom(). All ctags except */
/*  'CTG_SEED' can be copied (multiplied) correctly. Thus, when refining     */
/*  a triangle in surface_refinement() the 'CTG_SEED' bit should not be copied ! 	     */
/*  The other bits in fact must be copied during refinement !                */
/*  (segment interface, input boundary, ..)                                  */
/*                                                                           */
/*There should not be any direct assignments to ctags or direct comparisons  */
/*  to values. (Only during file-load and during adding of a new triangle)   */
/*                                                                           */
/*'CTG_EDGE' is used to characterize edges of adjacent triangles that have   */
/*  already been checked whether or not they form a structural edge.         */
/*  Alternatively, it can be set to avoid checking of specific edges.        */
/*                                                                           */
/*CTG_BITMAX is used to delete certain bits.				     */
/*                                                                           */

#define CTG_BFACE 0                /* boundary                               */
#define CTG_IFACE 8                /* double connected triangle              */
#define CTG_NEW 12

#define CTG_MARK 16                /* for special2 (Bit 5)                   */
#define CTG_SPECIAL 32             /* currently used for non Delaunauy leaks */
#define CTG_BLOCK 64               /* skipped by any pop_tri function        */
                                   /* (block when intersecting I-I or leaks) */
                                   /* (cases where to avoid errors)          */

/* All input triangles must be specially characterized !                     */
#define CTG_SEGBFACE 8192          /* declared boundary input triangle       */
#define CTG_SEGIFACE 128           /* declared interface between segments    */
#define CTG_SEED 256               /* only one triangle per segment          */

#define CTG_EDGE0 512              /* CTG_EDGE0<<1 == CTG_EDGE1 !!!          */
#define CTG_EDGE1 1024
#define CTG_EDGE2 2048

#define CTG_AFSKIP 4096            /* suppress advancing of the front        */
                                   /* e.g. for better LSPB entry point (n>4) */
                                   /* or intersection test for leaks         */
                                   /* (blocked+skipped vs. only skipped which*/
                                   /*  should not form a gap at the end)     */

 
#define CTG_BITMAX SHRT_MAX        /* ctag will usually have 15 bits         */






// ================================================================

struct ipointtype 
{
  long int x,y,z;
};
typedef struct ipointtype* ippt;

struct pointtype 
{
  double x,y,z;
  struct tri_listdef* ntri_lp;
  long int point_index;
};
typedef struct pointtype* ppt;


struct vectordef 
{
  double x,y,z;
};

struct matrixdef 
{       
  double a11, a12, a13; 
  double a21, a22, a23;    
  double a31, a32, a33;	 
};


struct arraydef 
{
  void *vp;           /* pointer to array                                    */
  long int n;         /* how many entries                                    */
  long int max;       /* current array size                                  */
  long int itemsize;  /* size of array item                                  */
};



// octree data types
//
struct nodedef 
{
  union {
    struct nodedef* child;
    ppt pointadr;           
  } u[8];
  short int tag;      /* bit == 1 if point, 0 if childnode or empty          */
  /* struct nodedef* mother;                                                 */
  /* short int n_depth;                                                      */
};
struct octree_def {
  struct nodedef topnode;
  long int octree_counter;
  /* General Bounding Box and Resolution Information                         */
  struct pointtype bbminimum;
  struct pointtype bbmaximum;  
  double step_size_x;
  double step_size_y;
  double step_size_z;
  long int maxnodes;
  int maxdepth;
  double octres;
};


/** TETRAHEDRON **************************************************************/
struct tetraederdef {
  struct triangledef* basetri_p;
  ppt dpp4;
  struct tetraederdef* ntet_p; /* pointer to next tetraeder                  */
  struct tetraederdef* ptet_p; /* pointer to previous tetraeder              */
  int mat_ind;                 /* material index                             */
};
typedef struct tetraederdef* tet_pt;   

/** TRIANGLE *****************************************************************/
struct triangledef {
  ppt dppp[3];
  struct triangledef* ntri_p;  /* pointer to next triangle                   */
  struct triangledef* ptri_p;  /* pointer to previous triangle               */
  short int ctag;              /* ctagnames see below                        */
  struct tetraederdef* ftet_p; /* backreference to front connected tet       */
  struct tetraederdef* btet_p; /* backreference to back connected tet        */
  int facemark;                /* later added index for each triangle        */
                               /* Less optimal because requires extra int for*/
                               /* all internal triangles as well. Also as it */
                               /* is now implemented the facemark is checked */
                               /* unfortunately after the angle is computed, */
                               /* while checking before could save that angle*/
                               /* computation                                */

  // [RH] additional material information ..
  //
  int material[2];             /* material information */
  int rh_facemark;              
};                                     
typedef struct triangledef* tri_pt;   

/** TRIANGLE LISTELEMENT *****************************************************/
/* should require minimal memory space, because of overhead in datastructure */
struct tri_listdef {             
  tri_pt tri_p;             
  struct tri_listdef* ntri_lp; /* pointer to next list element containing    */
                               /* pointer to triangle                        */
};                                       


/** POLYGONS, POLYHEDRA (for hierarchical input) *****************************/
/* A polygon needs to know where its triangles are in the triangle list      */
struct polygondef {
  struct arraydef vts;         /* need an array to be compatible with the    */
  int ctag;                    /* function nonconvex_triangulate()           */
  tri_pt first_tri_p;
  tri_pt last_tri_p;
  int facemark;

  // [RH] additional index information ..
  //
  int number_of_indices;          /* how often is this polygon references..*/
  int material[2];
};

/* A segment will contain an integer array of the polygon indices            */
struct segmentdef {
  int *pgs_p;              
  int n;
};


/** POINT CLASSIFICATION AND STRUCTURAL EDGES ********************************/
/* is linked to a point with structural edges via the point_index            */
struct point_classdef {
  ppt dpp;
  short int ptype;             /* classification and flags                   */
  double dist;                 /* current minimum edge length                */
  struct arraydef pri_gline;   /* structural edges (primary geometry lines)  */
  struct arraydef sec_gline;   /* structural edges (secondary geometry lines)*/
};
typedef struct point_classdef* pcl_pt;

/* holds information on the adjacent triangle for a given triangle and edge  */
struct edge_infodef {
  tri_pt atri_p;               /* pointer to one adjacent triangle           */
  double ccos;                 /* cosine of angle (or -2.0 if not calculated)*/
  int a_idx;                   /* index of 3rd point in adjacent triangle    */
  int l_idx;                   /* index of 3rd point in given slist (or -1)  */
}; 
  

/** CLIENT DATA structures ***************************************************/
/* client data in the context of callback functions executed by              */
/* octree search functions                                                   */
struct triangle_paramdef{
  struct vectordef H;  /* circumcenter                                       */
  struct vectordef n;  /* normalized normal vector                           */
  struct vectordef nu; /* unnormalized vector n                              */
  struct vectordef va;
  struct vectordef vb;   
  tri_pt tri_p;
  double A;
  double B;
  double lp1h;         /* length of distance between point1 and H            */
};
struct lambdadef {                       
  struct vectordef M;
  double R2;           /* square of R                                        */
  double R;            /* R (not always set when R2 is set)                  */
  double lam;          /* best lambda                                        */
  ppt bestdpp;                     
  struct arraydef rlist;
  struct arraydef plist;
  double lmin;         /* minimum of lambdas                                 */
  double lmax;         /* maximum of lambdas                                 */
  int n;               /* number of points after 1. pass                     */
  int opposite_ind;    /* index of point in triangle opposite to rlist point */
  double opposite_d;   /* normal distance of last rlist point                */
};
struct client_datadef {
  struct triangle_paramdef tri;
  struct lambdadef lambda;
  int counter;         /* general purpose counter                            */
};

struct ls_clientdef {
  struct arraydef ls_list;
  ppt dpp1;
  ppt dpp2;
  struct vectordef ve;
  double l_edge;
};

struct dump_clientdef {
  long int count;
  FILE* fp;
};

struct pcvldef {
  struct vectordef v;
  ppt dppout;
  int bbout;
  double uvmin_dist;
  double wmin_dist;
};

struct relax_clientdef {
  ppt dppX;
  struct vectordef vF;
  struct matrixdef mA;
  int counter;
};

struct surf_clientdef {
  struct triangle_paramdef tri;
  struct arraydef cneps_list;   /* dist. conn. points close to the surface   */
  struct arraydef uneps_list;   /* dist. unconn. points close to the surface */
  struct arraydef dpar_list;    /* dist. unconn. p.: d<SDEL_PAR and not uneps*/
  struct arraydef flip_list;    /* all dist. conn. points                    */
  int s1unconn;                 /* number of unconn. dist. p. in smallest sp.*/
  int s1overall;                /* number of dist. points in smallest sphere */
  int s2overall;                /* number of overall dist. p. in 2nd sphere  */

  /* frontside:                                                              */
  /* 'best' dist. point (conn. or unconn.) not in cneps_list or uneps_list   */
  ppt Fbestdpp;
  double Flam;
  struct vectordef F_M;
  double F_R2;
  double Fbestd;

  /* backside:                                                               */
  /* 'best' dist. point (conn. or unconn.) not in cneps_list or uneps_list   */
  ppt Bbestdpp;
  double Blam;
  struct vectordef B_M;
  double Bbestd;
  double B_R2;

  /* is uptodate? was set? or collinear tri?                                 */
  int validforthistri;
};
  
struct edge_clientdef {
  ppt dpp1;
  ppt dpp2;
  struct vectordef vM;
  struct vectordef ve;
  double inv2R;
  double R;
  double d;
  ppt founddpp;
  int counter;
};

/** GRID STRUCTURE ***********************************************************/
/* The main purpose is to hold the several tetrahedra lists and boundary and */
/* interface triangle lists.                                                 */
/* It also holds the unified tet list for all segments and the unified       */
/* triangle list during the meshing process.                                 */
/* The top element will always be empty (lastt**_p has to be initialized!)   */
/* This structure will be initialized after the meshing process to           */
/* separate the materials and generate the boundary and interface grids      */
struct griddef {
  tet_pt mattop_tet_p; /* pointer to array of top_tets (materials)           */
  tri_pt mattop_tri_p; /* pointer to array of top_tris (material interfaces) */
  int matn;            /* number of different materials                      */
  struct arraydef material_map; /* maps a material index to an array index   */
  struct tetraederdef unified_top_tet;
  struct triangledef unified_top_tri;
};

/** SEGMENT/SEED *************************************************************/
/* Holds all necessary information about a segment.                          */
struct seed_def {
  tri_pt seedtri_p;
  int material_index;                     /* material index for this segment */
};

/* optional extra name information                                           */
struct Name_def {
  int material_index;                     /* material index for this segment */
  char segment_name[LONGEST_CHARIN];      
  char material_name[LONGEST_CHARIN];
};


/** PCGEN STRUCTURE **********************************************************/
/* The main purpose is to hold all the info regarding point cloud generation */
/* via the directives from the input file.                                   */
struct pc_gendef {
  struct arraydef box_list;
  struct arraydef bnd_list;
  struct arraydef bnx_list;
  struct arraydef srf_list;
  int hom;
  int inpsnap;
};
struct srf_def {
  int fm;
  double maxarea_par;
  double minl_par;
  double sdel_par;
  double sproj_par;
  int equatorsph;
};
struct box_def {
  struct vectordef vp;
  struct vectordef vu;
  struct vectordef vv;
  struct vectordef vw;
  struct vectordef vspacing;
  double spacing_factor;
  struct arraydef mindistlist;
  struct arraydef pointlist;
};
struct bnd_def {
  int fm;
  struct vectordef vd;
  double depth;
  double spacing;
  double spacing_factor;
  struct arraydef mindistlist;
  struct arraydef pointlist;
};
struct bnx_def {
  int fm;
  int usefill;
  double depth;
  struct vectordef vw;
  struct vectordef vspacing;
  double spacing_factor;
};


/** MESH OBJECT **************************************************************/
/* It holds all 'global' variables                                           */
/* The related typedefs are also used by the application and therefore are   */
/* in dlk_api.h                                                              */
struct mesh_object_t_def {

  /* global point bucket octree containing topnode                           */
  /* (topnode will not be empty)                                             */
  struct octree_def global_octree;

  /* triangle and tetrahedra lists                                           */
  struct griddef grid;

  /* pcgen information                                                       */
  struct pc_gendef pc_gen;

  /* array for pointers to each seed triangle of a segment                   */
  struct arraydef seed_list;

  /* pointers to end of triangle and tetrahedra list                         */
  tri_pt lasttri_p;
  tet_pt lasttet_p;
  
  /* overall tetrahedra and triangle counters                                */
  long int gltr;
  long int glte;

  /* temporary counter for slivers only used during tetrahedrization and in  */
  /* the optimization loop (adaptation and sliver removal)                   */
  /* only counts slivers with negative mat_ind (d>0 OR d<=0; NOT d>0 && I-I) */
  int glsl;
  
  /* array for leak triangles (should be replaced with second octree)        */
  struct arraydef leaktris;


  /* PARAMETERS and EPSILONS                                                 */
  /* (The number of parameters and epsilons is defined in init.c)            */
  
  /* lambda_max, lambda_start for the search regions                         */
  double lambda_max;
  double lambda_start;      
  
  /* epsilons                                                                */
  double                pN_EPS; 
  double                 N_EPS;         
  double                 R_EPS;                    	     
  double                 L_BEPS;     
  double                 L_SEPS;   
  double                 M_BEPS;             
  double               VOL_EPS;
  double            WSBREF_EPS;
  
  /* parameters                                                              */
  double              MINL_PAR;
  double              SDEL_PAR;
  double           MINSNAP_PAR;
  double             SPROJ_PAR;
  double           MAXSNAP_PAR;
  double              COSP_PAR;
  double            MAXAR2_PAR;
  double            MAXCOS_PAR;
  int               RELAXR_PAR;
  int               RELAXC_PAR;
  int                 OCTAEDER;
  int               ALIGNEDPTS;
  int                REPHOLTHR;
  int               VTKONEFILE;
  int               EQUATORSPH;
  int            FINDMATERIALS;
  int            FLIP_GNORMALS;
  int            REFINE_GLINES;


  /* error file output                                                       */
  FILE *fph;
  FILE *fpv;
  int error_h;
  int error_v;
  
  /* counter for connect errors                                              */
  int error_r;

  /* counter for merge errors in insert_advancing_tetrahedron()              */
  int error_m;

  /* counter for merge warnings in insert_advancing_tetrahedron()            */
  int warning_m;

  /* counter for flip recursions using surface_recursive_swap() and triangle pointer     */
  int fliprecdepth;
  tri_pt fliprectri_p;
  int fliprecstate;
  int fliprec_endless;

  /** DELINK API VARIABLES ***************************************************/

  /* Variables for _PD_                                                      */
  struct {
    
    /* api default for findmaterials or negative when not used by api        */
    int findmaterials;

    /* api default for sdel_par or negative when not used by api             */
    double sdel_par;

    /* api default for minflipang_par or negative when not used by api       */
    double minflipang_par;

    /* api default for maxarea_par or negative when not used by api          */
    /* CAUTION: when 0.0 then any area is ok equivalent to no bound!         */
    double maxarea_par;

    /* api default for minsnap_par or negative when not used by api          */
    double minsnap_par;

  } PD;

  /* Status of _HI_                                                          */
  struct {
    
    /* point list                                                            */
    struct arraydef vertex_list;

    /* face list                                                             */
    struct arraydef polygon_list;
    
    /* segment list                                                          */
    struct arraydef segment_list;

    /* bounding box evaluation                                               */
    double minx;
    double maxx;
    double miny;
    double maxy;
    double minz;
    double maxz;

    /* for linesplit                                                         */
    struct ls_clientdef ls_client;

    /* general status variables                                              */
    int elcount;
    tri_pt begintri_p;

  } HI;

  /* Status of _HO_                                                          */
  struct {

    /* current material (for which point indices are set)                    */
    int mat_ind;

    /* current point                                                         */
    int next_pindex;
    int p_lindex;
    tet_pt curr_tet_p;
    int done_tindex;

    /* current tetrahedron                                                   */
    int t_lindex;
    tet_pt done_tet_p;

    /* current triangle                                                      */
    int orientation;
    tri_pt done_tri_p;
    tri_pt lowerdiagtri_p;

  } HO;


};







// ========================================================================


extern mesh_p_t global_init(); 
extern void global_quit(mesh_p_t); 
extern void global_reset(mesh_p_t); 



/* set default bounding box values, or read them from file parms.dlk         */
extern void boundingbox_init(struct octree_def*);

/* set octree constants (depends on bb, octres), initialize topnode+counter  */
extern void octree_init(struct octree_def*);

/* set default parameters and epsilons (depending on bb, octree constants)   */
/* or read them from file parms.dlk                                          */
extern void param_init(mesh_p_t, FILE*);

/* set the point indices for a continous pointlist                           */
extern int init_pointlist(tet_pt, struct octree_def*, int*);

/* Determine the number of different materials and allocate the material map.*/
/* Allocate memory for grid structure after the number of materials is known.*/
extern void init_grid(struct griddef*, struct arraydef*);

/* generate the lists per material, check the triangle ctags, and statistics */
/* should only be called once and calls itself init_grid()                   */
extern int sort_mesh(mesh_p_t);       

/* frees the entire grid efficiently without using the remove functions      */
/* also frees the allocated memory from init_grid()                          */
/* A 'free_grid()' does not make sense, hence the difference in name.        */
extern void free_mesh(mesh_p_t); 

/* memory management                                                         */
extern void* my_malloc(size_t);


/** ARRAYS *******************************************************************/

/* allocates array                                                           */
extern int array_init(struct arraydef*, int, long int); 

/* increase array dynamically                                                */
extern int expand_ar(struct arraydef*, int); 


/** OCTREE *******************************************************************/

/* returns found point if not inserted, and updates octree_counters          */
extern ppt oct_insert_point(ppt, struct octree_def*);  

/* returns found point if exchange, and updates octree_counters              */
extern ppt oct_exchange_point(ppt, struct octree_def*);

/* octree search, arguments contain callback, client data                    */
extern void ext_find(ppt, 
		     ppt, 
		     void (*)(ppt, void*, mesh_p_t),
		     void*, 
		     struct octree_def*,
		     mesh_p_t); 

/* octree search, arguments contain callback, client data, and allows delete */
extern void ext_del(ppt, 
		    ppt, 
		    int (*)(ppt, void*, mesh_p_t),
		    void*, 
		    struct octree_def*,
		    mesh_p_t);


/** CALLBACKS ****************************************************************/
/* executed by octree search functions for the found points                  */

/* prints out and counts points                                              */
extern void cb_printout(ppt, void*, mesh_p_t);   

/* file dump of points                                                       */
extern void cb_dump(ppt, void*, mesh_p_t);       

/* file dump of unconnected points                                           */
extern void cb_dumps(ppt, void*, mesh_p_t);       

/* resetting index to init_point_index                                       */
extern void cb_reset_index(ppt, void*, mesh_p_t);

/* point reference in octree is removed to array                             */
extern int cb_rem_p2sl(ppt, void*, mesh_p_t);      

/* point reference in octree is copied to array                              */
extern void cb_copy_p2sl(ppt, void*, mesh_p_t); 

/* point reference in octree is copied to array                              */
/* point_index is initialized with array index                               */
extern void cb_copy_init_p2sl(ppt, void*, mesh_p_t);  

/* point reference to unconnected point in octree is copied to array         */
/* point_index is initialized with array index                               */
extern void cb_copy_init_ep2sl(ppt, void*, mesh_p_t);          

/* point reference to unconnected point in octree is copied to array         */
extern void cb_copy_ep2sl(ppt, void*, mesh_p_t);          

/* point is entirely erased                                                  */
extern int cb_erase_point(ppt, void*, mesh_p_t);     

/* tetrahedrization callback for old version without specials                */
extern void cb_simple(ppt, void*, mesh_p_t);      

/* 1st pass tetrahedrization callback                                        */
extern void cb_pass1(ppt, void*, mesh_p_t);      

/* 2nd pass tetrahedrization callback                                        */
extern void cb_pass2(ppt, void*, mesh_p_t);       

/* smallest sphere test for a triangle                                       */
extern void cb_smsph(ppt, void*, mesh_p_t);

/* second sphere test                                                        */
extern void cb_2ndsph(ppt, void*, mesh_p_t);

/* smallest sphere test for an edge                                          */
extern int cb_smsph_edge(ppt, void*, mesh_p_t);

/* point relaxation                                                          */
extern void cb_relax(ppt, void*, mesh_p_t);
            
/* finds and sorts points on a line                                          */
extern void cb_linesplit(ppt, void*, mesh_p_t); 



/** HIGH LEVEL INPUT/OUTPUT **************************************************/
/* all functions update the global counters (gltr, glte, octree_counter)     */

/* loading points from file                                                  */
extern int load_points(mesh_p_t, FILE*, struct octree_def*);         

/* randomly generating points with integer coordinates                       */
extern int random_ip(int, struct octree_def*);   

/* randomly generating points with double coordinates                        */
extern int random_dp(int, struct octree_def*);  

/* manually entering points                                                  */
extern int enter_dp(int, struct octree_def*);      

/* saving all points to file                                                 */
extern int save_points(FILE*, struct octree_def*);                            

/* saving all unconnected points to file                                     */
extern int save_spoints(FILE*, struct octree_def*);                           

/* frontend for loading triangles from file                                  */
extern int load_tri(mesh_p_t, FILE*);
     
/* saving triangles with specified masked ctag and facemark to file          */
extern int save_tri(FILE*, tri_pt, int, int, int, ppt, ppt);     

/* read triangles from stdin or file                                         */
extern int input_tri(mesh_p_t, FILE*, int); 

/* saving all triangles from a tri_list                                      */
extern int save_tri_list(FILE*, struct tri_listdef*, ppt, ppt);   

/* saving all points from slist                                              */
extern void save_slist(FILE*, struct arraydef*, ppt, ppt);   

/* saving all point_classes                                                  */
extern void save_pclass(FILE*, struct arraydef*, ppt, ppt);   

/* write PIF tetrahedra grid                                                 */
extern int grid2pif(tet_pt, char*, struct octree_def*);

/* load HIERARCHICAL geometry specification                                  */
extern int read_geom(mesh_p_t, FILE*);

/* load SV geometry specification                                            */
extern int read_svgeom(mesh_p_t, FILE*);

/* save HIERARCHICAL mesh                                                    */
extern int write_meshes(mesh_p_t, FILE*, int);

/* save HIERARCHICAL mesh vtk format                                         */
extern int write_vtk(mesh_p_t, FILE*, int);





/*****************************************************************************/
/** FUNCTIONS FOR DATASTRUCTURE MANAGEMENT ***********************************/
/*****************************************************************************/

/* DEFINITION: The datastructure in the following context contains the       */
/*   topological information whereas the octree holds topographical info     */
/* RULE: In a consistent datastructure all references and elements have to   */
/*   be valid. All forward references have to be set and cannot be null, all */
/*   backward references can either be set or null.                          */
/* DEFINITION:                                                               */
/*   create -- allocates, lists, and initializes an element                  */
/*   insert -- inserts an element into the consistent datastructure          */
/*             thereby mainly setting the back references to the element     */
/*   remove -- removes an element from the datastr. keeping it consistent    */
/*             thereby removing higher order elements and setting its        */
/*             back references in lower order elements to null               */
/*   erase  -- freeing elements after removing                               */
/* NOTE:                                                                     */
/*   sm_* implies a surface only as opposed to a volume                      */
/* NOTE:                                                                     */
/*   the consistency is independent of a point being in or out of the octree */
/* NOTE:                                                                     */
/*   some modification functions use their own optimized manipulations       */
/*   instead of a sequence of remove's and insert's                          */
/* NOTE:                                                                     */
/*   elements must be removed before erasing them                            */
/* NOTE:                                                                     */
/*   points must also be removed from the octree before erasing them         */
/*   remove_point() only removes from the datastructure                      */
/* NOTE:                                                                     */
/*   triangle and tetrahedra counters are not updated in these functions,    */
/*   not even in refinement functions                                        */
/*   (The triangle and tetrahedra counters are updated in the high level I/O */
/*    functions and during the volume decomposition and surface treatment)   */
/* NOTE:                                                                     */
/*   refinement functions create and insert triangles and tetrahedra,        */
/*   but do not create or oct_insert points                                  */
/* NOTE:                                                                     */
/*   remove functions do not erase the higher order elements, instead        */
/*   preserve the information of the cut out elements. An additional erase   */
/*   loop is necessary.                                                      */
/* NOTE:                                                                     */
/*   All pointers which could possibly reference an erased (not just removed)*/
/*   triangle or tetrahedron have to be updated in time. This becomes e.g.   */
/*   non trivial and important for a call to erase_cutoutpoint() when for    */
/*   instance the stacktri_p references one of the erased triangles.         */
/*   Only the pointers lasttri_p and lasttet_p are updated automatically.    */
/*   E.g. there should not be something like a stacktet_p during             */
/*   tetrahedrization, because some Slivers might be erased.                 */
/* NOTE:                                                                     */
/*   Modification functions (sm_flip, refine...) should not use erase and    */
/*   create, but rather really only modify ! Otherwise, same problems when   */
/*   pointers exist which reference the erased elements. (relevant in e.g.   */
/*   surface_recursive_swap and so on..)                                                 */
/*                                                                           */
/* NOTE FOR THE EXTENSION TO NEGATIVE VOLUME SLIVERS:                        */
/* Independently of positive or negative volumes of a sliver, the basetri    */
/* of a tet must always have its tet connected to the frontside (ftet_p).    */
/* Also, the orientation must always be consistent with the ftet_p and       */
/* btet_p. So, during insertion of a tet or while merging, depending on the  */
/* orientation of a triangle the correct back reference to the tet (ftet_p,  */
/* or btet_p) must be updated. This is non-trivial for e.g. special1() and   */
/* negative volume slivers. (A check becomes necessary to avoid such         */
/* wrong merges, because once the connectivity is 'the other way around' due */
/* to negative volumes, it must always be 'the other way around' to still be */
/* consistent!)                                                              */
/* Independently of the topography two overlapping triangles t1,t2 with the  */
/* same orientation may have a single tet connected either to the backside   */
/* of t1 and the frontside of t2 or vice versa. Assume t1,t2 to be           */
/* oriented upwards. Then, assume that t1 is topographically 'above' t2.     */
/* Normally with a positive volume tet inbetween, the tet will be attached to*/
/* the backside of t1 (below an upward oriented triangle means backside) and */
/* and to the frontside of t2. A topological correct but negative volume tet */
/* inbetween t1,t2 would in such a case be attached to the frontside of t1   */
/* and the backside of the (below!) t2, without violating the orientations or*/
/* order of indices and point relations ('the other way around').            */
/*                                                                           */
 

/** POINTS *******************************************************************/

/* creating a point                                                          */
#define create_point(_dpp) {                                                  \
   (_dpp) = (ppt) my_malloc(sizeof(struct pointtype));                        \
   (_dpp)->ntri_lp = null;                                                    \
   (_dpp)->point_index = init_point_index;} 

/* inserting a point is in this context meaningless                          */

/* removing a point, checking for tris and tets                              */
extern int remove_point(ppt, int*);   

/* removing a point, checking for tris                                       */
extern int sm_remove_point(ppt);

/* erase a point with connected and removed triangles and tetrahedra         */
/* (After the removal of a point the connected elements are also removed but */
/* not yet erased to keep the information of the cut out part.)              */
void erase_cutoutpoint(ppt, tri_pt*, tet_pt*);

/* erasing a point is trivial, when invoked ntri_lp already equals null      */
#define erase_point(_dpp) {                                                   \
   free(_dpp);}                       


/** TRIANGLES ****************************************************************/

/* creating a triangle and initializing the ctag                             */
/* has to link the new triangle to the end of the list, because some         */
/* functions rely on it e.g. read_geom()                                     */
#define create_triangle(_dpp1, _dpp2, _dpp3, _ctag, _mark, _p) {              \
   (_p)->ntri_p = (tri_pt) my_malloc(sizeof(struct triangledef));             \
   (_p)->ntri_p->ptri_p = (_p);                                               \
   (_p) = (_p)->ntri_p;                                                       \
   (_p)->ntri_p = null;                                                       \
   (_p)->dppp[0] = _dpp1;                                                     \
   (_p)->dppp[1] = _dpp2;                                                     \
   (_p)->dppp[2] = _dpp3;                                                     \
   (_p)->ftet_p = null;                                                       \
   (_p)->btet_p = null;                                                       \
   (_p)->facemark = _mark;                                                    \
   (_p)->ctag = _ctag;                                                        \
   (_p)->material[0] = (_p)->ptri_p->material[0];                             \
   (_p)->material[1] = (_p)->ptri_p->material[1];                             }

// [RH] .. to create a triangle with the same material as the base triangle
//
#define create_triangle_rh(_op, _dpp1, _dpp2, _dpp3, _ctag, _mark, _p) {	\
   (_p)->ntri_p = (tri_pt) my_malloc(sizeof(struct triangledef));             \
   (_p)->ntri_p->ptri_p = (_p);                                               \
   (_p) = (_p)->ntri_p;                                                       \
   (_p)->ntri_p = null;                                                       \
   (_p)->dppp[0] = _dpp1;                                                     \
   (_p)->dppp[1] = _dpp2;                                                     \
   (_p)->dppp[2] = _dpp3;                                                     \
   (_p)->ftet_p = null;                                                       \
   (_p)->btet_p = null;                                                       \
   (_p)->facemark = _mark;                                                    \
   (_p)->ctag = _ctag;                                                        \
   (_p)->material[0] = (_op)->material[0];                             \
   (_p)->material[1] = (_op)->material[1];                             }

/* inserting a triangle                                                      */
extern void insert_triangle(tri_pt);

/* removing a triangle, checking for tets, and deleting tri_lists            */
extern int remove_triangle(tri_pt);

/* removing a triangle, and deleting tri_lists                               */
extern void sm_remove_triangle(tri_pt);

/* erase a triangle with connected and removed tetrahedra                    */
/* (After the removal of a triangle the connected tetrahedra are also        */
/* removed but not yet erased to keep the information of the cut out part.)  */
void erase_cutouttriangle(tri_pt, tri_pt*, tet_pt*);

/* erasing a triangle, requiring double linked lists                         */
/* cannot be called for top elements which are not created (automatic)       */
/* has to update *lasttri_pp                                                 */
/* _tri_p and _lasttri_p may not be the same pointer ! (same address is ok)  */
#define erase_triangle(_tri_p, _lasttri_p) {                                  \
   (_tri_p)->ptri_p->ntri_p = (_tri_p)->ntri_p;                               \
   if((_tri_p)->ntri_p != null) (_tri_p)->ntri_p->ptri_p = (_tri_p)->ptri_p;  \
   else (_lasttri_p) = (_tri_p)->ptri_p;                                      \
   free(_tri_p);}
   



/** REFINEMENT ***************************************************************/

/* refines an edge, returns the number of created and inserted tris          */
extern int sm_ref_triedge(tri_pt, int, ppt, tri_pt*);

/* refines an edge, returns the number of created and inserted tris          */
extern int sm_ref_edge(ppt, ppt, ppt, tri_pt*);

/* refines an edge, returns the number of created and inserted tets, tris    */
extern int ref_triedge(tri_pt, int, ppt, tri_pt*, tet_pt*, int*);
     
/* refines an edge, returns the number of created and inserted tets, tris    */
extern int ref_edge(ppt, ppt, ppt, tri_pt*, tet_pt*, int*);
     
/* refines a triangle, returns the number of created and inserted tris       */
extern int sm_ref_tri(tri_pt, ppt, tri_pt*);

/* refines a triangle, returns the number of created and inserted tets, tris */
extern int ref_tri(tri_pt, ppt, tri_pt*, tet_pt*, int*);

/* refines a tetrahedron, returns number of created and inserted tris        */
extern int ref_tet(tet_pt, ppt, tri_pt*, tet_pt*);


/** OTHER DS STUFF ***********************************************************/

/* edge-swap two adjacent triangles                                          */
extern int sm_flip_tris(tri_pt, tri_pt, int, int);

/* finds an inserted triangle within a flippable surface area containing the */
/* projection in a given direction of a given point                          */
extern tri_pt sm_locate_tri(mesh_p_t,
			    struct arraydef*,
			    tri_pt, 
			    struct vectordef*, 
			    struct vectordef*, 
			    int*,
			    ppt); 

/* finds an inserted triangle, returns its orientation                       */
extern int exist_tri(ppt, ppt, ppt, tri_pt*); 

/* finds an inserted triangle, returns its orientation and                   */
/* the index of a third point                                                */
extern int exist_tri_getidx(ppt, ppt, ppt, tri_pt*, int*);

/* finds a mergeable adjacent inserted triangle with third point in plist,   */
/* returning the plist index of the point                                    */
extern int exist_mrgadjtri(mesh_p_t, ppt, ppt, struct client_datadef*); 

/* looks for adjacent triangles possibly with a 3rd point out of an slist    */
/* and calculates the angle between the two faces                            */
extern int investigate_edge(tri_pt, 
			    int, 
			    struct vectordef*, 
			    struct arraydef*,
			    struct edge_infodef*);

/*****************************************************************************/



/** GEOMETRIC CALCULATIONS ***************************************************/
/* calculates search region for a given lambda                               */
extern void calc_region(mesh_p_t, ppt, ppt, struct client_datadef*);

/* initializes all parameters in struct triangle_paramdef                    */
extern int calc_n_H(struct triangle_paramdef*);        

/* calculates if a triangle is collinear                                     */
extern int calc_collinear(ppt, ppt, ppt);

/* calculates the circumcenter of three points and vector T                  */
extern int calc_H_T(ppt, ppt, ppt, struct vectordef*, struct vectordef*);

/* calculates lambda and the circumcenter M for a given triangle and a point */
extern int calc_l_M(mesh_p_t, 
		    ppt, 
		    struct triangle_paramdef*, 
		    struct vectordef*, 
		    double*);

/* calculates lambda for a given triangle and a given point                  */
extern int calc_l(mesh_p_t, ppt, struct triangle_paramdef*, double*);

/* returns if point is not in rlist and on the right side to build a tet     */
extern int check_side(mesh_p_t, struct client_datadef*, ppt);    

/* translate a point not inserted in an octree, trivial but need pointer (*) */
extern void move_point(mesh_p_t, ppt, struct vectordef*);
                               
/* translate a point not inserted in an octree, applying constraints         */
extern void rest_move_point(mesh_p_t, ppt, struct vectordef*);

/* translate points which are already inserted in an octree                  */
extern int oct_move_points(struct octree_def*, ppt, ppt, struct vectordef*);

/* calculates intersection of triangle with a line given by a normalized     */
/* vector, a middle point, and half the line length                          */
extern int intersect_tri_line(tri_pt, 
			      struct vectordef*,
			      struct vectordef*, 
			      ppt, 
			      double);

/* calculates the intersection of an infinite plane with a line. The line is */
/* given by two points and the plane is given by any three points.           */
extern int intersect_plane_line(ppt, ppt, ppt, ppt, ppt, ppt); 

/* calculates if a point lies inside a triangle                              */
extern int inside_tri_test(struct triangle_paramdef*, ppt);

/* checks the Delaunay property of a triangle and generates lists of the     */
/* various types of disturbing points                                        */
extern int check_del_tri(mesh_p_t, struct surf_clientdef*);

/* checks the smallest sphere property of an edge                            */
extern ppt check_del_edge(mesh_p_t, ppt, ppt, double*);

/* calculates area and angle criteria, returns if triangle needs refinement  */
extern int check_quality_tri(mesh_p_t, struct triangle_paramdef*);

/* intersection test of a given tetrahedron with all leaks                   */
int intersection_test(mesh_p_t, tet_pt);




/** SURFACE MODELING *********************************************************/

/* initialize static arrays used during surface modeling                     */
extern void surf_init();
/* free static arrays used during surface modeling                           */
extern void free_surfmod();
/* recursive edge swapping and Delaunay triangle check                       */
extern int surface_recursive_swap(mesh_p_t, tri_pt, tri_pt);
/* entire surface treatment, prior to tetrahedrization                       */
extern int surface_refinement(mesh_p_t, tri_pt, int);
/* detect structural edges                                                   */
extern void detect_glines(mesh_p_t, struct arraydef*, tri_pt, int, int);
/* refine structural edges                                                   */
extern int process_glines(mesh_p_t, struct arraydef*);
/* refine surface triangles                                                  */
extern int process_tris(mesh_p_t, struct arraydef*, tri_pt, int, int*);

/* triangulate a general nonconvex polygon without holes and insert tris     */
int nonconvex_triangulate2(struct arraydef* poly_p, int ctag, int facemark, tri_pt* lasttri_pp, long mat1, long mat2);

 

/** POINT GENERATION *********************************************************/
/* snap points with a given snap distance array (background mesh)            */
extern int snap_points(mesh_p_t, struct arraydef*, struct arraydef*);

/* snap in this way that not point itself but all surrounding pts are erased */
extern void snap_clear(mesh_p_t, double, ppt);

/* test whether uninserted point would be too close to other points          */
extern int snap_test(mesh_p_t, double, ppt);












