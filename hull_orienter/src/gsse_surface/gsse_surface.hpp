/* =======================================================================

                     ---------------------------------------
                     ViennaMesh - The Vienna Meshing Library
                     ---------------------------------------
                            
   maintainers:    
               Josef Weinbub                      weinbub@iue.tuwien.ac.at
               Johann Cervenka                   cervenka@iue.tuwien.ac.at

   developers:
               Franz Stimpfl, Rene Heinzl, Philipp Schwaha

   license:    
               LGPL, see file LICENSE in the ViennaMesh base directory

   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at

   ======================================================================= */

#ifndef GSSESURFACE_HH_ID
#define GSSESURFACE_HH_ID

// *** system includes
//
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

// *** BOOST includes
//
#include <boost/lexical_cast.hpp>


// *** GSSE overall meshing includes
//
#include "gsse_meshing_includes.hpp"

// *** GSSE surface dl includes
//
#include "surface_includes.hpp"

// -----------------



class gsse_surface_interface
{
   
   mesh_p_t  dlk;
   
   typedef std::vector<gsse::point_t>	point_tv;
   point_tv		phv;
   

public:
   gsse_surface_interface()
      {
         dlk              = global_init();
         phv.clear();	
      }
   ~gsse_surface_interface()
      {
         global_reset(dlk);
      }
   
   
  int  readGeometry(const std::string& fname)
  {
    FILE* fp = fopen(fname.c_str(),"r");
    
    int cnt;
    cnt = read_geom(dlk, fp);
    
    fclose(fp);
    
    return cnt;
  }
  
  
  void triangulateSurface()
  {
    surface_refinement(dlk, &dlk->grid.unified_top_tri, 0);
  }

  
  void extractSurface(gsse::domain_32t& domain)
  {
    tri_pt tri_p;
    
    std::vector<gsse::gsse_triangle>          triangle_container;
    std::vector<gsse::point_t>                point_vector;
    std::map<gsse::point_t, long>             point_container;
    std::map<gsse::point_t, long>::iterator   pc_it;
    
    std::vector<long>                         local_segment_cell_counter;
    
    long max_mat_index = -1;
    
    
    if(&dlk->grid.unified_top_tri == null) return;
    
    // #### extra code for implementing the new GSSE cell counter mechanism 
    //  we have to know the global number of triangles 
    // 
    tri_p = dlk->grid.unified_top_tri.ntri_p;
    long global_cc = 0;
    while(tri_p!=null)   
      { 
 	if(!(tri_p->ctag & 1)) 
	  {
	    ++global_cc;
	  }        
	tri_p = tri_p->ntri_p; 
      } 
    local_segment_cell_counter.resize(global_cc, 0);
    
//     std::cout << "..global_cc: " << global_cc << std::endl;
    
    //
    // =================
    
    tri_p = dlk->grid.unified_top_tri.ntri_p;
    
    while(tri_p!=null) 
      {
       if(!(tri_p->ctag & 1))    // check this tag .. all valid triangles ??
	 {
	   long mat_info[2]={-1,-1};       // use this information only locally
	   bool replicate_triangles = false;	   
	   
	   if (tri_p->material[1] != -1)   // here we have an interface triangle
	     {
	       // gsse material information for interface triangle
	       //
	       mat_info[0] = tri_p->material[1];
	       mat_info[1] = tri_p->material[0]; 
	       
	       replicate_triangles =  true;

// 	       std::cout << "..replicate this triangle: " << std::endl;
// 	       std::cout << "..point0: " << tri_p->dppp[0]->x << " / " << tri_p->dppp[0]->y << " / " << tri_p->dppp[0]->z << std::endl;
// 	       std::cout << "..point1: " << tri_p->dppp[1]->x << " / " << tri_p->dppp[1]->y << " / " << tri_p->dppp[1]->z << std::endl;
// 	       std::cout << "..point2: " << tri_p->dppp[2]->x << " / " << tri_p->dppp[2]->y << " / " << tri_p->dppp[2]->z << std::endl;
	     }
	   else
	     {
	       // gsse material information .. just one material 
	       //
	       mat_info[0] = tri_p->material[0];

// 	       std::cout << "..point0: " << tri_p->dppp[0]->x << " / " << tri_p->dppp[0]->y << " / " << tri_p->dppp[0]->z << std::endl;
// 	       std::cout << "..point1: " << tri_p->dppp[1]->x << " / " << tri_p->dppp[1]->y << " / " << tri_p->dppp[1]->z << std::endl;
// 	       std::cout << "..point2: " << tri_p->dppp[2]->x << " / " << tri_p->dppp[2]->y << " / " << tri_p->dppp[2]->z << std::endl;
	     }
	   
	   gsse::gsse_triangle ttri(1);
	   
	   // ########################
	   //
	   ttri.material = mat_info[0];    // here we set the normal triangle material info
	   ++local_segment_cell_counter[mat_info[0]] ;   // increment the number of cells in this material / segment number
// 	   std::cout << ".. mat1: " << mat_info[0] << std::endl;
// 	   std::cout << ".. local_segment_cell_counter in mat1: " << local_segment_cell_counter[mat_info[0]] << std::endl;
	   
	   //
	   // ########################
	   

         if (mat_info[0] > max_mat_index)   //just for the domain.add_segment() part  .. search the maximum mat number
	    max_mat_index = mat_info[0];
	  
         gsse::gsse_triangle ttri_2(1);
         if (replicate_triangles)
         {
            ttri.mat_index[0] = mat_info[0];
            ttri.mat_index[1] = mat_info[1];
	      
            ttri_2.material = mat_info[1];
            ttri_2.mat_index[1] = mat_info[0];
            ttri_2.mat_index[0] = mat_info[1];

	    ++local_segment_cell_counter[mat_info[1]] ;   // increment the number of cells in this material / segment number
// 	    std::cout << ".. mat2: " << mat_info[1] << std::endl;
// 	    std::cout << ".. local_segment_cell_counter in mat2: " << local_segment_cell_counter[mat_info[1]] << std::endl;
         }
         else
         {
            ttri.mat_index[0] = -1;
            ttri.mat_index[1] = -1;
         }
	  
         for (long i = 0; i < 3; i++)
         {	  	      
            long tempindex;
            gsse::point_t tempp(tri_p->dppp[i]->x, tri_p->dppp[i]->y, tri_p->dppp[i]->z);
	      	      	

            // #####   check if the point was read in before
            //
            pc_it = point_container.find(tempp);
            if ( pc_it == point_container.end()) // point was not inserted before
            {
               point_vector.push_back(tempp);
               tempindex = point_vector.size() -1;
               point_container[tempp] = tempindex;
            }
            else    // point found
            {
               tempindex = (*pc_it).second;
            }
	      
	      
            // assign the point indices
            //
            ttri.pindexarray[i] = tempindex;
	      
            if (replicate_triangles)
            {
               ttri_2.pindexarray[i] = tempindex;
            }	      	
         }	  
	      
         // store the triangles
         //
         triangle_container.push_back(ttri);
         if (replicate_triangles)
         {
            std::swap (ttri_2.pindexarray[0], ttri_2.pindexarray[1]) ; //change the orientation for the replicated triangle
            triangle_container.push_back(ttri_2);
         }	  
	 
// 	 if(replicate_triangles)
// 	   {
// 	     std::cout << "tri1: ";
// 	     for (long i = 0; i < 3; i++)
// 	       {
// 		 std::cout << " " << ttri.pindexarray[i];
// 	       }
// 	     std::cout << std::endl;

// 	     std::cout << "tri2: ";
// 	     for (long i = 0; i < 3; i++)
// 	       {
// 		 std::cout << " " << ttri_2.pindexarray[i];
// 	       }
// 	     std::cout << std::endl;
	     
// 	   }
	  	 	  
      }
      tri_p = tri_p->ntri_p;
   } // while

   // ------------------
   // 
   // GSSE geometry:3d  topology:2d domain generation
   //
   typedef  gsse::domain_traits<gsse::domain_32t>::segment_iterator	    segment_iterator;
   typedef  gsse::domain_traits<gsse::domain_32t>::cell_iterator	    cell_iterator;
   typedef  gsse::domain_traits<gsse::domain_32t>::segment_t	            segment_t;
   typedef  gsse::domain_traits<gsse::domain_32t>::point_t		    point_t;
   typedef  gsse::domain_traits<gsse::domain_32t>::cell_handle              cell_handle;
   typedef  gsse::domain_traits<gsse::domain_32t>::global_point_iterator    global_point_iterator;
   typedef  gsse::domain_traits<gsse::domain_32t>::cell_2_vertex_mapping    cell_2_vertex_mapping;
   typedef  gsse::domain_traits<gsse::domain_32t>::storage_type             storage_type;
  
  
   for (size_t j = 0; j < point_vector.size(); ++j)
   {      
      std::vector<double> coord;
      for (unsigned int dim = 0; dim < gsse::domain_32t::dimension_geometry; ++dim)
      {
         coord.push_back(point_vector[j][dim]);
      }
      point_t tempp( coord[0], coord[1], coord[2] );
      domain.fast_point_insert(tempp);    
   }


   // add approp. segment information
   //
//    segment_iterator segit = domain.add_segment();

//    std::cout << "..max_mat_index: " << max_mat_index << std::endl;

   

   // here we translate the local segment cell counter information to a
   //   a global cell counter .. to access the global offset globally
   // 
   std::vector<long> global_cell_counter (local_segment_cell_counter.size(), 0);
   long global_cell_counter_var = 0;
   for (size_t index = 0; index < local_segment_cell_counter.size(); ++index)
     {
       global_cell_counter_var += local_segment_cell_counter[index];
       global_cell_counter[index] = global_cell_counter_var;
     }


   for (long i = 0; i < max_mat_index; ++i)
     {
       segment_iterator segit = domain.add_segment();
       (*segit).set_cell_index_offset( global_cell_counter[i] );

//        std::cout << "i: "<< i << "\t global cell counter: "<< global_cell_counter[i] << std::endl;
       (*segit).add_cell_quantity("interface");
       (*segit).add_cell_quantity("mat1");
       (*segit).add_cell_quantity("mat2");

     }

   for (size_t j = 0; j < triangle_container.size(); ++j)
   {
      // navigate to the correct segment
      // 
      segment_iterator segit = domain.segment_begin();
      cell_handle ch_offset = 0;

      // ommit the first offset due to the offset = 0 for the first segment
      //
      for (long i = 1; i <= triangle_container[j].material-1; ++i)   //material starts with 1.. !!
	{
	  ++segit;        // !!! be carefull.. the iterator is already on the first segment with in a domain
	                  // ## here we step to the second segment.. 
	  ch_offset =  global_cell_counter[i]; // because we do not need the current offset, but the offset of the following 
	}
      
      cell_handle ch = (*segit).add_cell_2(cell_2_vertex_mapping( triangle_container[j][0],
								  triangle_container[j][2],
								  triangle_container[j][1]));
      ch += ch_offset;

      if (triangle_container[j].mat_index[0] != -1)
      {
// 	std::cout << "########### ch: " << ch << std::endl;
         domain.store_quantity_direct(ch, "interface", storage_type(1,1,1.0));
         domain.store_quantity_direct(ch, "mat1", storage_type(1,1,triangle_container[j].mat_index[0]));
         domain.store_quantity_direct(ch, "mat2", storage_type(1,1,triangle_container[j].mat_index[1]));
// 	 std::cout << "store: 1.0 :: mat1: " << triangle_container[j].mat_index[0] << " :: " << triangle_container[j].mat_index[1] << std::endl;
      }
      else
      {
// 	std::cout << "########### ch: " << ch << std::endl;
         domain.store_quantity_direct(ch, "interface", storage_type(1,1,0.0));
         domain.store_quantity_direct(ch, "mat1", storage_type(1,1,-1.0));
         domain.store_quantity_direct(ch, "mat2", storage_type(1,1,-1.0));
      }
       
	  
      domain.add_vertex(triangle_container[j][0], segit);
      domain.add_vertex(triangle_container[j][1], segit);
      domain.add_vertex(triangle_container[j][2], segit);


   }
//    std::cout << " cell quan: " << std::endl;
//    domain.dump_quantities_on_cell() ; 
//    domain.write_file("testme.gau32");


}
   

};


#endif	// GSSESURFACE_HH_ID
