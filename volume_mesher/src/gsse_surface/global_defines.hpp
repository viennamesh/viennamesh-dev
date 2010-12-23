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

/* default octree resolution                                                 */
/* (from this value the octree depth will be derived which is allowed using  */
/*  long signed integers depending on the bounding box)                      */
#define dOCTRES               1e-12

/* default bounding-box for double-coordinates                               */
/* (minima are included, maxima are exclusive bounds)                        */
#define dXMAX               100.0 
#define dYMAX               100.0 
#define dZMAX               100.0 
#define dXMIN              -100.0 
#define dYMIN              -100.0 
#define dZMIN              -100.0 
	    
/* The following parameter defaults can be arbitrary expressions using some  */
/* of these identifiers:                                                     */
/*   spanx      x-width of bounding box                                      */
/*   spany      y-width of bounding box                                      */
/*   spanz      z-width of bounding box                                      */
/*   stepx      size of smallest octree node                                 */
/*   stepy      size of smallest octree node                                 */
/*   stepz      size of smallest octree node                                 */
/* These identifiers will either be computed from the default bounding box   */
/* size, or from the bounding box size being read from the file parms.dlk,   */
/* or from later bounding box size changes due to file load operations       */
/*                                                                           */
/* EXAMPLE:         dMINL_PAR  (stepx + stepy + stepz)                       */

/* The values lambda_start and lambda_max have a major impact on the         */
/* tetrahedralization. They are derived from the bounding box and usually    */
/* they will not need to be modified.                                        */
/* lambda_max must be much greater than the bounding box because it limits   */
/* the maximal circumsphere of an element. It should be proven that no holes */
/* can emerge within the tetrahedralization due to elements which are missing*/
/* because their circumsphere is larger. Firstly this gap would be filled    */
/* approaching from a different side, secondly such large circumspheres can  */
/* only be possible near the boundary for boundary elements where the 2ndsph */
/* check was applied. Therefore the 2ndsph test will be linked to lambda_max */
/* as well!                                                                  */
#define dlambda_start    (stepx + stepy + stepz)
#define dlambda_max      (spanx + spany + spanz) 
        
/* These parameters affect the surface preprocessing                         */
#define dMINL_PAR             0.0   
#define	dSDEL_PAR             0.0    
#define	dMINSNAP_PAR          (stepx + stepy + stepz)    
#define	dSPROJ_PAR            ((spanx + spany + spanz)/100.0) 
#define	dMAXSNAP_PAR          (spanx + spany + spanz) 
#define	dMINFLIPANG_PAR     176.0
#define	dMAXAREA_PAR          0.0 
#define	dMINANG_PAR           0.0

/* for relaxation                                                            */
#define	dRELAXR_PAR           0
#define	dRELAXC_PAR           0

/* flags                                                                     */
#define dOCTAEDER             0
#define dALIGNEDPTS           0
#define dREPHOLTHR            0
#define dEQUATORSPH           0
#define dFINDMATERIALS        0

/* only affects reading from file (read_geom and read_svgeom) at the moment! */
/* (not in API or in hlev)                                                   */
#define dFLIP_GNORMALS        0

/* 0 is very dangerous, because almost disables rotation case,               */
/* 1 skips the initial glines refinement which seems to make things WORSE!?  */
/* 2 is the original which still works best                                  */
#define dREFINE_GLINES        2


/** Write for each material one VTK output file for more flexibility *********/
/* (This allows different visualization modes for each material. If set to 1 */
/* only one file is created with all the material information and all        */
/* materials will have the same visualization mode. If set to 2 the single as*/
/* well as the material files are written)                                   */
#define dVTKONEFILE           1







// ====================================

/** default epsilons unless otherwise defined in file parms.dlk **************/
/* The defaults can be arbitrary expressions using some of these identifiers */
/*   spanx      x-width of bounding box                                      */
/*   spany      y-width of bounding box                                      */
/*   spanz      z-width of bounding box                                      */
/*   stepx      size of smallest octree node                                 */
/*   stepy      size of smallest octree node                                 */
/*   stepz      size of smallest octree node                                 */
/* These identifiers will either be computed from the default bounding box   */
/* size, or from the bounding box size being read from the file parms.dlk,   */
/* or from later bounding box size changes due to file load operations       */
/*                                                                           */
/* EXAMPLE:          dL_BEPS  (DBL_EPSILON * 10^4 * spanx)                   */

// [RH] dN_EPS from 1e-14 -> 1e-12

#define dpN_EPS           0.0 
#define dN_EPS            1e-12         
#define dR_EPS            1e-12                    		

#define dL_BEPS           1e-9     
#define dL_SEPS           1e-12   
#define dM_BEPS           1e-12             

#define dVOL_EPS          0.0
#define dWSBREF_EPS       DBL_EPSILON




