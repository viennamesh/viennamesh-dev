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

// *** system includes
//
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unistd.h>


// ** BOOST includes
//
#include<boost/lexical_cast.hpp>
#include<boost/thread/thread.hpp>
#include<boost/thread/mutex.hpp>
#include<boost/bind.hpp>
#include<boost/timer.hpp>

// *** GSSE includes
//
#include "gsse_meshing_includes.hpp"
#include "gsse_vgmodeler.hpp"
#include "../include/hin.hpp"

#include "../include/threadpool.hpp"

//#define DEBUGORIENTATION
   
// --------------------------------------



// #########################################################
// #########################################################

template <typename Domain>
int write_intel_hin(Domain& domain, const std::string& filename)
{

  typedef typename gsse::domain_traits<Domain>::vertex_handle vertex_handle;
  typedef typename gsse::domain_traits<Domain>::cell_iterator cell_iterator;
  typedef typename gsse::domain_traits<Domain>::vertex_on_cell_iterator vertex_on_cell_iterator;
  typedef typename gsse::domain_traits<Domain>::vertex_iterator vertex_iterator;
  typedef typename gsse::domain_traits<Domain>::segment_iterator segment_iterator;
  typedef typename gsse::domain_traits<Domain>::segment_t   segment_t;
  typedef typename gsse::domain_traits<Domain>::point_t point_t;
  typedef typename gsse::domain_traits<Domain>::segment_t        segment_t;
  typedef typename gsse::domain_traits<Domain>::cell_2_vertex_mapping cell_2_vertex_mapping;

  typedef typename gsse::domain_traits<Domain>::global_point_iterator   global_point_iterator;

  // ----------------------------

  const int dimension = gsse::domain_traits<Domain>::dimension_tag::dim;
  std::ofstream            file_out;
    
  BOOST_STATIC_ASSERT(dimension >= 2);
  BOOST_STATIC_ASSERT(dimension <= 3);

    
  file_out.open( filename.c_str() );
    
    
  file_out << domain.point_size() << std::endl;
  file_out.setf(std::ios::right, std::ios::adjustfield);



  // *** get all geometrical points
  global_point_iterator  gpi, gpi_end;
  gpi = domain.point_begin();
  gpi_end =domain.point_end(); 

  // *** output all points to the wss file
  for (; gpi != gpi_end; ++gpi)
    {
      //std::cout << *pl_it << std::endl;
      file_out << std::setprecision(16) << std::setiosflags(std::ios::fixed) <<  std::setiosflags(std::ios::showpos)  << (*gpi).second << std::endl;

    }
  // *** end of the POINTS section
  file_out.unsetf(std::ios::showpos);

    
  // *** begin of the SEGMENTS
    
  segment_iterator seg_iter;
    
  file_out << "# Number of segments " << std::endl;
  file_out << domain.segment_size() << std::endl; 
    
  long seg_counter = 0;
  // *** begin of the Topology (GRID)
    
  for (seg_iter = domain.segment_begin(); seg_iter != domain.segment_end(); seg_iter++)
    {
      cell_iterator cit;

      // *** iterate over all cells
      for (cit = (*seg_iter).cell_begin(); cit != (*seg_iter).cell_end(); cit++)
{
  vertex_on_cell_iterator vocit(*cit);

  file_out << seg_counter;

  std::map<vertex_handle, long>  testmap;

  // *** iterate over all interior vertices
  while (vocit.valid())
    {
      file_out << "  " << (*vocit).handle();
      testmap[ (*vocit).handle() ] ++;
      vocit++;
    }
  file_out << std::endl;

  if (testmap.size() != 4)
    {
      std::cout << "!!!!! not consistent tetrahedron!!!! .. STOP converting .. " << std::endl;
      return -1;
    }

}

      ++seg_counter;
    }
  file_out.close();

  return 0;
}


// #########################################################
// #########################################################

int create_surface_mesh(const std::string& filename_in, gsse::domain_32t& domain, int meshing_type=0)
{
  gsse_surface_interface   gsse_surface;

//  std::cout << "## Reading HIN file: " << filename_in  << std::endl; 
  gsse_surface.readGeometry(filename_in);

//  std::cout << "## Converting file" << std::endl;
  gsse_surface.triangulateSurface();
      
  //std::cout << "## Finalizing surface " << std::endl;
  gsse_surface.extractSurface(domain);

  return 0;
}


// #########################################################
// #########################################################
//
// SEQUENTIAL VOLUME MESHING
//
// #########################################################
// #########################################################

template<typename Domain32T, typename VertexVectorT, typename Domain3T>
int create_volume_mesh_sequential(Domain32T& domain_in, VertexVectorT additional_vertices, Domain3T& domain_out, double maxh = 0.05)
{
#ifdef DEBUG
  std::cout << ".. starting sequential meshing .. " << std::endl;
#endif
  
  typedef typename gsse::domain_traits<Domain32T>::segment_iterator    segment_iterator;
  typedef typename gsse::domain_traits<Domain32T>::cell_iterator    cell_iterator;
  typedef typename gsse::domain_traits<Domain32T>::vertex_on_cell_iterator  vertex_on_cell_iterator;
  typedef typename gsse::domain_traits<Domain32T>::point_t    point_t;
  typedef typename gsse::domain_traits<Domain32T>::vertex_handle            vertex_handle;
  
  typedef typename boost::array<long, 3>                cell_vertex_mapping;
  typedef typename std::vector<cell_vertex_mapping>     segment_vector;
  typedef typename std::map<point_t, long>              vertex_map;
  typedef typename vertex_map::iterator                 vertex_map_it;
  typedef typename std::map<vertex_handle, point_t>     vertex_map_reverse;
  
//   mycout = new ofstream ("/dev/null");
//   myerr = new ofstream ("/dev/null");
//   testout = new ofstream ("/dev/null");

  vertex_map vertex_domain_mapping;
  long point_count = 0;
  long threadID = 0, nr_of_threads = 1;
  int seg_counter = 0;

  segment_iterator seg_iter = domain_in.segment_begin();


//   for(;  threadID < nr_of_threads;  threadID++)  
  for( ; seg_iter != domain_in.segment_end(); ++seg_iter, ++threadID)
    {             
      vertex_map segment_vertices;
      long local_point_count = 1;
      long normal_counter = 1;

      Mesh mesh;  

      std::cout << " .. meshing subdomain: " << threadID << std::endl;   

//       mesh.AddFaceDescriptor (FaceDescriptor (1, 1, 0, 1));

      std::map<cell_vertex_mapping, long> element_map;

      // FS TEST
      //      
#ifdef DEBUG
      std::ofstream      file_out;
      std::string temp_file = "fs_segment_" + boost::lexical_cast<std::string>(threadID) +".stl";
      file_out.open(temp_file.c_str());
      file_out << "solid " << (*seg_iter).get_segment_name() << std::endl;
#endif

      cell_iterator cit;                     
      for (cit = (*seg_iter).cell_begin(); cit != (*seg_iter).cell_end(); ++cit, normal_counter++)
{      
  vertex_on_cell_iterator vocit(*cit);
  int localcounter = 0;
  cell_vertex_mapping cell;

  // FS TEST
  //
#ifdef DEBUG
  file_out << "   facet normal 0.0 0.0 0.0" << std::endl 
   << "     outer loop  " << std::endl;
#endif


  while (vocit.valid())
    {
      // store all vertex handles, which are used by the segment
      //
      point_t new_point = domain_in.get_point((*vocit));
      vertex_map_it sv_it = segment_vertices.find(new_point);

      // FS TEST
      //
#ifdef DEBUG
      file_out << "      vertex " << new_point << std::endl;
#endif

            
      if(sv_it == segment_vertices.end())          // point is not in the map
{
  Point3d p;
  double scale = 1;

  // add vertex to mesh
  if(std::abs(new_point[0]) <= 1e-15)
    p.X() = 0;
  else
    p.X() = new_point[0] * scale;
  
  if(std::abs(new_point[1]) <= 1e-15)
    p.Y() = 0;
  else
    p.Y() = new_point[1] * scale;    
  
  if(std::abs(new_point[2]) <= 1e-15)
    p.Z() = 0;
  else
    p.Z() = new_point[2] * scale;

#ifdef DEBUG    
  std::cout << " test.. 1"
    << p.X() << "/" 
    << p.Y() << "/" 
    << p.Z() << std::endl << std::endl;
#endif

   //std::cout << "adding point: " << p << std::endl;

  mesh.AddPoint(p);

#ifdef DEBUG
  std::cout << "..num points in mesh: " << mesh.GetNP() << std::endl;
#endif

  // for adding the additional vertices
  //
//   domain.retrieve_quantity(*vit, "vertex_quantity", quan_values );
//   if(quan_values == 1)
//     {
//       mesh.AddLockedPoint(mesh.GetNP());
//     }


  segment_vertices[new_point] = mesh.GetNP();
   cell[localcounter] = mesh.GetNP();
  
#ifdef DEBUG
  std::cout << ".. point added: " << mesh.GetNP() << "# " 
    << p.X() << "/" 
    << p.Y() << "/" 
    << p.Z() << std::endl;
#endif
}
      else     // point is already in the map - use this handle
{
#ifdef DEBUG
  std::cout << ".. point found: " << (*sv_it).second << "# " << (*sv_it).first << std::endl;
#endif
  cell[localcounter] = (*sv_it).second;
}

      vocit++;
      localcounter++;
     
    }

  // FS TEST
  //
#ifdef DEBUG
  file_out << "     endloop " << std::endl;
  file_out << "   endfacet " << std::endl;
#endif

  
  Element2d el;
//   el.SetIndex(normal_counter+1);
  el.SetIndex(1);
  el.PNum(1) = cell[0];
  el.PNum(2) = cell[1];
  el.PNum(3) = cell[2];   

#ifdef DEBUG
  std::cout << ".. add element: " << cell[0] << " " << cell[1] << " " << cell[2] << std::endl;
#endif

  std::map<cell_vertex_mapping, long>::iterator em_it = element_map.find(cell);
  if(em_it == element_map.end())
    {
      element_map[cell] = normal_counter;

      // [FS] .. old version
      //
//       mesh.AddFaceDescriptor(FaceDescriptor(normal_counter+1,1,0,0));

      // [FS] .. because we only have one segment to mesh, the materials can be hardcoded .. i guess ;-)
      //
      mesh.AddFaceDescriptor(FaceDescriptor(threadID+1,1,0,0));
      //std::cout << "adding constraint: " << el << std::endl;
      mesh.AddSurfaceElement(el);  

    }
        
}

      // FS TEST
      //
#ifdef DEBUG
      file_out << "endsolid" << std::endl;   
      file_out.close();
#endif
   

          
#ifdef DEBUG
      std::cout << " .. after for .. num points in domain: " << domain_in.point_size() << std::endl;   
      std::cout << " .. after for .. num points in mesh  : " << mesh.GetNP() << " :: num elem: " << mesh.GetNSE()  << std::endl;   
#endif

      typedef typename VertexVectorT::iterator vvit_t;
      vvit_t vvit;
      for(vvit = additional_vertices.begin(); vvit != additional_vertices.end(); ++vvit)
{  
  Point3d p;
  double scale = 1;
  
  // add vertex to mesh
  if(std::abs((*vvit).first[0]) <= 1e-15)
    p.X() = 0;
  else
    p.X() = (*vvit).first[0] * scale;
  
  if(std::abs((*vvit).first[1]) <= 1e-15)
    p.Y() = 0;
  else
    p.Y() = (*vvit).first[1] * scale;    
  
  if(std::abs((*vvit).first[2]) <= 1e-15)
    p.Z() = 0;
  else
    p.Z() = (*vvit).first[2] * scale;
  
#ifdef DEBUG
  std::cout << " test.. 1"
    << p.X() << "/" 
    << p.Y() << "/" 
    << p.Z() << std::endl << std::endl;
#endif
  std::cout << "ADDING ADDITIONAL POINTS" << std::endl;
  mesh.AddPoint(p);
  
  mesh.AddLockedPoint(mesh.GetNP());     
}

#ifdef DEBUG     
      std::cout << " .. after for .. num points in domain: " << domain_in.point_size() << std::endl;   
      std::cout << " .. after for .. num points in mesh  : " << mesh.GetNP() << " :: num elem: " << mesh.GetNSE()  << std::endl;   
#endif
      
      
//       mesh.ClearFaceDescriptors();
    
      Point3d pmin, pmax;
      mesh.GetBox (pmin, pmax);
    
      MeshingParameters mp;
      mp.maxh = maxh;

#ifdef DEBUG
      std::cout << "## GSSE .. num points: " << mesh.GetNP() << std::endl;       
      std::cout << "## GSSE .. num elements: " << mesh.GetNE() << std::endl << std::endl;
      std::cout << "## Calculating local feature size from input.. " << std::endl;
#endif

      //mesh.CalcLocalHFromPointDistances();
      mesh.CalcLocalH(threadID);

#ifdef DEBUG
      std::cout << "## After calculating LFS .. before volume meshing" << std::endl;
#endif

      MeshVolume (mp, mesh, threadID);


      // RemoveIllegalElements (mesh);
      // OptimizeVolume (mp, mesh);

#ifdef DEBUG
      std::cout << "## GSSE: saving temp volume file .." << std::endl;
      std::string filename = "temp_gsse_" + boost::lexical_cast<std::string>(threadID) + ".vol";
      std::cout << ".. saving file: " << filename << std::endl; 
      mesh.Save(filename);
#endif

      // ####################################################
      // ####################################################
      // ##
      // ##  DATA BACK TO GSSE 
      // ##
      // ####################################################
      // ####################################################
      //
      // now we need to get the data back to the parent thread
      // maybe this can be done by filling the data into a seperate domain
      //       
      typedef typename gsse::domain_traits<Domain3T>::segment_t             segment_3u_t;
      typedef typename gsse::domain_traits<Domain3T>::cell_2_vertex_mapping       cell_2_vertex_mapping_2;

      //     domain_3u_t domain_out;        
      std::string temp_string;
      long local_dimension;
      long number_of_elements;
    
      local_dimension     = mesh.GetDimension();
      number_of_elements  = mesh.GetNE();  
      //   std::cout << "## GSSE .. number of elements: " << number_of_elements << std::endl;
    
      std::vector<segment_3u_t> temp_segments;

      // ----------------------------
      //
      // geometry part 
      //
      // ----------------------------
      long number_of_points;
      number_of_points = mesh.GetNP();   
      std::map<long, long> mesh_domain_mapping; 

#ifdef DEBUG
      std::cout << "point_count before for : " << point_count << std::endl;
#endif

      for (PointIndex pi = PointIndex::BASE; pi < number_of_points+PointIndex::BASE; pi++)
{
   std::cout << "direct point output: " << mesh[pi] << std::endl;
  // check if a point is already included in the domain
  //
  point_t new_point = point_t(mesh[pi].X(), mesh[pi].Y(), mesh[pi].Z());
  vertex_map_it vdm_it = vertex_domain_mapping.find(new_point);

#ifdef DEBUG
  std::cout << ".. point index: " << pi << " :: point: " << new_point << std::endl;
#endif

  if(vdm_it == vertex_domain_mapping.end())          // point is not in the map
    { //std::cout << "point is new: " << new_point << std::endl;
      //::cout << "  mapping: " << new_point << " -- " << point_count << std::endl;
      vertex_domain_mapping[new_point] = point_count;
      //std::cout << "  mapping: " << pi << " -- " << point_count << std::endl;
      mesh_domain_mapping[pi] = point_count;
      
      domain_out.fast_point_insert(new_point);     
      point_count++;
    }
  else                                       // point is already in the map - use this handle
    {
      //std::cout << "point alread added: " << new_point << std::endl;
      //std::cout << "  mapping: " << pi << " -- " << (*vdm_it).second << std::endl;
      mesh_domain_mapping[pi] = (*vdm_it).second;    
    }

}

#ifdef DEBUG
      std::cout << "point_count after for : " << point_count << std::endl;        
#endif
    
      // /////////////////////////////////////////////////////////////////////////
      //   next we read in the segment information 
      //   -> topological information is stored into the segment
      // /////////////////////////////////////////////////////////////////////////
    
//       int ind1, ind2, ind3, ind4;
      int temp_element;
      unsigned int surface_nr;
    
      // process volume elements
      //
      for (ElementIndex ei = 0; ei < number_of_elements; ei++)
{
  // file_in >> surface_nr 
  // >> temp_element 
  // >> ind1 >> ind2 >> ind3  >> ind4;

  surface_nr = mesh[ei].GetIndex();
  temp_element = mesh[ei].GetNP();

  Element el = mesh[ei];

  // ind1 = el[0]-1;
  // ind2 = el[1]-1;
  // ind3 = el[2]-1;
  // ind4 = el[3]-1;

//   std::cout << " old cell: " 
//     << el[0]-1 << "#"
//     << el[1]-1 << "#"
//     << el[2]-1 << "#"
//     << el[3]-1 << "\t\t"
//   file1 << mesh[el[0]].X() << " "
//     << mesh[el[0]].Y() << " "
//     << mesh[el[0]].Z() << "#"
//     << mesh[el[1]].X() << " "
//     << mesh[el[1]].Y() << " "
//     << mesh[el[1]].Z() << "#"
//     << mesh[el[2]].X() << " "
//     << mesh[el[2]].Y() << " "
//     << mesh[el[2]].Z() << "#"
//     << mesh[el[3]].X() << " "
//     << mesh[el[3]].Y() << " "
//     << mesh[el[3]].Z() << " "
//     << std::endl;

//   std::cout << " new cell: " 
//     << mesh_domain_mapping[el[0]] << "#"
//     << mesh_domain_mapping[el[1]] << "#"
//     << mesh_domain_mapping[el[2]] << "#"
//     << mesh_domain_mapping[el[3]] << "\t\t"

//   file2 << domain_out.global_point_list[mesh_domain_mapping[el[0]]] << "#"
//     << domain_out.global_point_list[mesh_domain_mapping[el[1]]] << "#"
//     << domain_out.global_point_list[mesh_domain_mapping[el[2]]] << "#"
//     << domain_out.global_point_list[mesh_domain_mapping[el[3]]] << " "
//     << std::endl;

  if (surface_nr+1 > temp_segments.size() )
    {
      temp_segments.resize(surface_nr+2);
    }
        
//   temp_segments[surface_nr].add_cell_2(cell_2_vertex_mapping_2( ind1, ind2, ind3, ind4 ) );
   //std::cout << "mapping cell" << std::endl;
             std::cout << "direct cell output: " << el << std::endl;
   //std::cout << mesh_domain_mapping[el[0]] << " " << mesh_domain_mapping[el[1]] << " " << 
   //   mesh_domain_mapping[el[2]] << " " << mesh_domain_mapping[el[3]] << std::endl;
      
  temp_segments[surface_nr].add_cell_2(cell_2_vertex_mapping_2( mesh_domain_mapping[el[0]],
mesh_domain_mapping[el[1]],
mesh_domain_mapping[el[2]],
mesh_domain_mapping[el[3]]) );
}
    

      for (unsigned long i =0; i < temp_segments.size(); ++i)
{
   if (temp_segments[i].cell_size() > 0)
     {
      std::stringstream segname;
      segname << "segment_" << seg_counter++;
      temp_segments[i].set_name(segname.str());
      domain_out.add_segment(temp_segments[i]);
     }  
}
            
    }


#ifdef DEBUG  
  std::cout << "..after merge .. domain point size: " << domain_out.point_size() << std::endl;
  std::cout << "..after merge .. domain segment size: " << domain_out.segment_size() << std::endl;
#endif  
 
  //
  // ############################################
}


// #########################################################
// #########################################################
//
// PARALLEL VOLUME MESHING
//
// #########################################################
// #########################################################

template<typename SegmentCellsT, typename SegmentVerticesT, typename SegmentVerticesRevT, typename PointT>
void meshing(const long threadID, SegmentCellsT segment_cells, SegmentVerticesT segment_vertices, SegmentVerticesRevT segment_vertices_rev)
{
#ifdef DEBUG
  std::cout << ".. in meshing thread: " << threadID << std::endl;
#endif

    
//   mycout = new ofstream ("/dev/null");
//   myerr = new ofstream ("/dev/null");
//   testout = new ofstream ("/dev/null");    

  typedef typename SegmentCellsT::iterator          segment_cells_it;
  typedef typename SegmentCellsT::value_type        cell_vertex_mapping;
  typedef typename cell_vertex_mapping::iterator    cell_vertex_mapping_it;
  typedef typename cell_vertex_mapping::value_type  vertex_handle;
  typedef typename SegmentVerticesT::iterator       segment_vertices_it;

  typedef typename std::map<PointT, long>           vertex_map;
  typedef typename vertex_map::iterator             vertex_map_it;

  typedef typename std::map<vertex_handle, long>    mesh_vertex_mapping;
    

  Mesh mesh;
  mesh.AddFaceDescriptor (FaceDescriptor (1, 1, 0, 1));
    
  mesh_vertex_mapping new_vertex_mapping;
    
  // convert domain segments to volume elements
  //
  long segcounter = threadID;

#ifdef DEBUG
  std::cout << " ..  in function_object .. number of cells : " << segment_cells.size() << std::endl;    
  std::cout << " ..  in function_object .. number of vertices : " << segment_vertices.size() << std::endl;
#endif

  segment_vertices_it sv_it;
  long count = 1 ;
  long normal_counter = 1;
  typedef typename std::map<cell_vertex_mapping, long> element_map_t;
  typedef typename element_map_t::iterator             element_map_t_it;
  element_map_t element_map;

#ifdef DEBUG    
  std::cout << " .. number of points in mesh: " << mesh.GetNP() << std::endl;
#endif

  vertex_map new_segment_vertices;
    
  segment_cells_it sc_it;
  for (sc_it = segment_cells.begin(); sc_it != segment_cells.end(); ++sc_it, normal_counter++)
    {      
      int localcounter = 0;
      cell_vertex_mapping cell;

      cell_vertex_mapping_it cvmit;
      for(cvmit = (*sc_it).begin(); cvmit != (*sc_it).end(); ++cvmit)
{  
  PointT new_point = segment_vertices_rev[(*cvmit)];  

  vertex_map_it sv_it = new_segment_vertices.find(new_point);
  
  if(sv_it == new_segment_vertices.end())          // point is not in the map
    {
      Point3d p;
      double scale = 1.0;

#ifdef DEBUG
//       std::cout << " checkpoint add 2: " << (*cvmit) << " :: " << new_point << std::endl;
#endif
      
      if(std::abs(new_point[0]) <= 1e-15)
p.X() = 0;
      else
p.X() = new_point[0] * scale;
      
      if(std::abs(new_point[1]) <= 1e-15)
p.Y() = 0;
      else
p.Y() = new_point[1] * scale;    
      
      if(std::abs(new_point[2]) <= 1e-15)
p.Z() = 0;
      else
p.Z() = new_point[2] * scale;

#ifdef DEBUG        
//       std::cout << ".. mesh point to add: " << p.X() << "/" << p.Y() << "/" << p.Z() << std::endl;
#endif
      
      // gsse volume mesh
      //
      mesh.AddPoint(p);
      
      new_segment_vertices[new_point] = mesh.GetNP();
      
      cell[localcounter] = mesh.GetNP();
    }
  else
    {
#ifdef DEBUG
//       std::cout << ".. point found: " << (*sv_it).second << "# " << (*sv_it).first << std::endl;
#endif
      cell[localcounter] = (*sv_it).second;
    }
  
  localcounter++;
}

      Element2d el;
      // [FS] .. old
      //       el.SetIndex(localindex + 1);
      el.SetIndex(1);
      el.PNum(1) = cell[0];
      el.PNum(2) = cell[1];
      el.PNum(3) = cell[2]; 

#ifdef DEBUG     
//       std::cout << "..el: " << cell[0] << " " << cell[1] << " " << cell[2] << std::endl;
#endif
      
      element_map_t_it em_it = element_map.find(cell);
      if(em_it == element_map.end())
{
  element_map[cell] = normal_counter;
        
  // gsse ng add surface elements
  //
  // mesh.AddFaceDescriptor(FaceDescriptor(localindex+1,domainin, domainout,0));
  
  // because we only have one segment to mesh, this can be hardcoded .. i guess ;-)
  //
  // [FS] .. old
  //       mesh.AddFaceDescriptor(FaceDescriptor(localindex+1,1, 0,0));
  mesh.AddFaceDescriptor(FaceDescriptor(0,1,0,0));
  mesh.AddSurfaceElement(el);  
}
    }      
    
#ifdef DEBUG
  std::cout << " .. after for .. num points: " << mesh.GetNP() << " :: num elem: " << mesh.GetNSE()  << std::endl;   
#endif
      
  //mesh.ClearFaceDescriptors();
    
  Point3d pmin, pmax;
  mesh.GetBox (pmin, pmax);
    
  MeshingParameters mp;
  mp.maxh = 1e6;

#ifdef DEBUG    
  std::cout << "## GSSE .. num points: " << mesh.GetNP() << std::endl;       
  std::cout << "## GSSE .. num elements: " << mesh.GetNE() << std::endl << std::endl;    
  std::cout << "## Calculating local feature size from input.. " << std::endl;
#endif

//   mesh.CalcLocalHFromPointDistances();
  mesh.CalcLocalH(threadID);

#ifdef DEBUG
  std::cout << "## GSSE .. starting volume meshing..id: " << threadID << std::endl;
#endif

  MeshVolume (mp, mesh, threadID);


  // RemoveIllegalElements (mesh);
  // OptimizeVolume (mp, mesh);

  //   std::cout << "## GSSE: saving temp volume file .." << std::endl;
#ifdef DEBUG
  std::string filename = "temp_gsse_" + boost::lexical_cast<std::string>(threadID) + ".vol";
  std::cout << ".. saving file: " << filename << ".vol" << std::endl; 
//   mesh.Save(filename);
#endif

  // now we need to get the data back to the parent thread
  //
  // maybe this can be done by filling the data into a seperate domain
    

}


template<typename DomainT>
int create_volume_mesh_parallel(DomainT& domain, const std::string& filename_in)
{
#ifdef DEBUG
  std::cout << ".. starting parallel meshing .. " << std::endl;
#endif
  
  typedef typename gsse::domain_traits<DomainT>::segment_iterator  segment_iterator;
  typedef typename gsse::domain_traits<DomainT>::cell_iterator  cell_iterator;
  typedef typename gsse::domain_traits<DomainT>::vertex_on_cell_iterator  vertex_on_cell_iterator;
  typedef typename gsse::domain_traits<DomainT>::point_t  point_t;
  typedef typename gsse::domain_traits<DomainT>::vertex_handle            vertex_handle;
  
  // ############################################
  //
  // for every segment in the domain a new thread is spawned
  //
  typedef typename boost::array<long, 3>                cell_vertex_mapping;
  typedef typename std::vector<cell_vertex_mapping>     segment_vector;
  typedef typename std::map<point_t, long>              vertex_map;
  typedef typename vertex_map::iterator                 vertex_map_it;
  typedef typename std::map<long, point_t>              vertex_map_reverse;
  
  long threadID = 0, nr_of_threads = 2;
//   boost::thread_group threads;

  // [FS] .. test 
//   boost::threadpool::pool tp;
//   tp.size_controller().resize(2);

  segment_iterator seg_iter = domain.segment_begin();

//   for(;  threadID < nr_of_threads;  threadID++)  
  for( ; seg_iter != domain.segment_end(); ++seg_iter, ++threadID)  
    {       
      segment_vector segment_cells;
      vertex_map segment_vertices;
      vertex_map_reverse segment_vertices_rev;

      cell_iterator cit;                     
      for (cit = (*seg_iter).cell_begin(); cit != (*seg_iter).cell_end(); ++cit)
{      
  vertex_on_cell_iterator vocit(*cit);
  int localcounter = 0;
  cell_vertex_mapping cell;

  while (vocit.valid())
    {
      // store all vertex and handles in a seperate datastructure,
      // which is passed on to the meshing thread
      //
      point_t new_point = domain.get_point((*vocit));
      vertex_map_it sv_it = segment_vertices.find(new_point);
            
      if(sv_it == segment_vertices.end())          // point is not in the map
{
  // point not found in the map - add the new point
  //
//     std::cout << " new point: " << (*vocit).handle() << "/" << new_point << std::endl;
  cell[localcounter] = (*vocit).handle();
  segment_vertices[new_point] = (*vocit).handle();
  segment_vertices_rev[(*vocit).handle()] = new_point;
}
      else                                       
{
  // point is already in the map - use this handle
  //
//    std::cout << " point found: " << (*vocit).handle() << "/" << new_point << std::endl;
  cell[localcounter] =(*sv_it).second;
}

      vocit++;
      localcounter++;     
    }   
  segment_cells.push_back(cell); 
}

#ifdef DEBUG      
      std::cout << " .. number of cells: " << segment_cells.size() << std::endl;
      std::cout << " .. number of vertices: " << segment_vertices.size() << std::endl;
#endif
      
      // partition is done -> start the thread
      //
      
      // OLD
      //
      // boost::thread mThread(boost::bind(&meshing<segment_vector, vertex_map, vertex_map_reverse>, threadID, segment_cells, segment_vertices, segment_vertices_rev ));




      // NEW
      //
      // threads.create_thread(boost::bind(meshing<segment_vector, vertex_map, vertex_map_reverse, point_t>, threadID, segment_cells, segment_vertices, segment_vertices_rev ));

//       schedule(tp, boost::bind(meshing<segment_vector, vertex_map, vertex_map_reverse, point_t>, threadID, segment_cells, segment_vertices, segment_vertices_rev ));

//       meshing<segment_vector, vertex_map, vertex_map_reverse, point_t>(threadID, segment_cells, segment_vertices, segment_vertices_rev);
      
//       seg_iter++;
    }

  // wait for all thread to finish
  //
//   tp.wait();
  
//   sleep(10);

//   threads.join_all();

  std::cout << "## GSSE .. leaving parallel meshing .." << std::endl;
  //
  // ############################################
}

// #########################################################
// #########################################################

// [RH][TODO] 
//   use the facet iterator to be consistent in all dimensions
//
template<typename DomainT, typename SegmentT, typename CellIterator>
bool orientation_recursive(DomainT& domain,  SegmentT& segment,  CellIterator& cit)
{
  typedef typename gsse::domain_traits<DomainT>::edge_on_cell_iterator    edge_on_cell_iterator;
  typedef typename gsse::domain_traits<DomainT>::cell_on_edge_iterator    cell_on_edge_iterator;
  typedef typename gsse::domain_traits<DomainT>::storage_type             storage_type;


#ifdef DEBUGORIENTATION
  std::cout << " check  orientation recursion at cell: " << *cit << std::endl;
#endif

  long number_of_not_oriented_cells = 0;
  for (edge_on_cell_iterator eocit(*cit); eocit.valid(); ++eocit)
    {
      for (cell_on_edge_iterator coeit(*eocit); coeit.valid(); ++coeit)
{
  if (domain(*coeit, "orientation")(0,0) == 0.0)
    ++number_of_not_oriented_cells;
}
    }

#ifdef DEBUGORIENTATION
  std::cout << " number of not oriented cells: " << number_of_not_oriented_cells << std::endl;
#endif


  // stop condition for recursion
  //
  if (number_of_not_oriented_cells == 0)
    return true;


  //   domain.write_file("gsse_orientation_recursive_out.gau32");


  // here we have to orient the adjacent cells to the given cell iterator cit
  //
  for (edge_on_cell_iterator eocit(*cit); eocit.valid(); ++eocit)
    {
      for (cell_on_edge_iterator coeit(*eocit); coeit.valid(); ++coeit)
{

  if ((*coeit) == (*cit))
    continue;
  if (domain(*coeit, "orientation")(0,0) != 0.0)
    continue;

  bool is_oriented_consistently = 
    gsse::check_oriented_neighbor(   segment.retrieve_topology().get_cell( (*coeit).handle() ), 
     segment.retrieve_topology().get_cell( (*cit).handle() )  );
   
  if (is_oriented_consistently)
    {
      domain.store_quantity(*coeit, "orientation", storage_type(1,1,1.0));  // 1.0 -> oriented consistenly
    }
  else
    {
      bool wrong_oriented_neighbor = 
gsse::check_wrong_oriented_neighbor( segment.retrieve_topology().get_cell( (*coeit).handle() ), 
     segment.retrieve_topology().get_cell( (*cit).handle() ) ) ;
      if (wrong_oriented_neighbor)
{
  domain.store_quantity(*coeit, "orientation", storage_type(1,1,2.0));  // 2.0 -> NOT oriented consistenly

#ifdef DEBUGORIENTATION  
  std::cout << "..wrong oriented neighbor: " << (*coeit) << std::endl;
#endif
   
  // try to change this error immediatly
  //
  long temp = segment.retrieve_topology().get_cell( (*coeit).handle() )[1];
  segment.retrieve_topology().get_cell((*coeit).handle())[1] = segment.retrieve_topology().get_cell((*coeit).handle())[2];
  segment.retrieve_topology().get_cell((*coeit).handle())[2] = temp;

  domain.store_quantity(*coeit, "orientation", storage_type(1,1,1.0));  // 1.0 -> oriented consistenly
}
      else
domain.store_quantity(*coeit, "orientation", storage_type(1,1,-1.0));  // possible error !!!!

    }

}

    }
   

  // all adjacent cells are consistently oriented
  //
  // step into the next recursion level
  //
  for (edge_on_cell_iterator eocit(*cit); eocit.valid(); ++eocit)
    {
      for (cell_on_edge_iterator coeit(*eocit); coeit.valid(); ++coeit)
{
  if ((*coeit) == (*cit))
    continue;

  orientation_recursive(domain, segment, coeit);
}
    }
}


template<typename Domain>
void check_and_repair_orientation_new(Domain& domain)
{
  typedef typename gsse::domain_traits<Domain>::cell_iterator  cell_iterator;
  typedef typename gsse::domain_traits<Domain>::segment_iterator  segment_iterator;
  typedef typename gsse::domain_traits<Domain>::storage_type             storage_type;
  typedef typename gsse::domain_traits<Domain>::vertex_on_cell_iterator  vertex_on_cell_iterator;

  for (segment_iterator seg_iter = domain.segment_begin(); seg_iter != domain.segment_end(); ++seg_iter)
    {
      (*seg_iter).add_cell_quantity("orientation");
       
      for (cell_iterator cit = (*seg_iter).cell_begin(); cit != (*seg_iter).cell_end(); cit++)
      {
         domain.store_quantity_direct((*cit).handle(), "orientation", storage_type(1,1,0.0));  // 0.0 -> not yet oriented 
      }
    }

  // show the orientation of the first cell of each segment
  //
#ifdef DEBUGORIENTATION
  std::cout << "..orientation of the first cell of each segment:" << std::endl;
#endif

  segment_iterator seg_iter = domain.segment_begin();
  for (seg_iter = domain.segment_begin(); seg_iter != domain.segment_end(); ++seg_iter)
    {
#ifdef DEBUGORIENTATION
      std::cout << "segment: " << (*seg_iter) << std::endl;
#endif
      cell_iterator cit = (*seg_iter).cell_begin(); 

      vertex_on_cell_iterator vocit(*cit);

#ifdef DEBUGORIENTATION
      std::cout << "cell vertices: ";
      while (vocit.valid())
{
  std::cout << (*vocit) << " ";
  vocit++;
}
      std::cout << std::endl;

      std::cout << "cell: " << (*cit) << std::endl;
#endif

      domain.store_quantity(*cit, "orientation", storage_type(1,1,1.0));  // 1.0 -> oriented consistenly
    }

  // [FS] .. change the orientation of the second segment
  //
  //    seg_iter = domain.segment_begin();
  //    seg_iter++;
  //    cell_iterator cit = (*seg_iter).cell_begin(); 
  //    std::cout << "cell: " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[0] 
  //      << " " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1]
  //      << " " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[2] << std::endl;

  //    long temp =    (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[0];
  //    (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[0] = (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1];
  //    (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1] = temp;

  //    std::cout << "cell: " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[0] 
  //      << " " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1]
  //      << " " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[2] << std::endl;

  for (seg_iter = domain.segment_begin(); seg_iter != domain.segment_end(); ++seg_iter)
    {
      cell_iterator cit = (*seg_iter).cell_begin(); 
      //        domain.store_quantity(*cit, "orientation", storage_type(1,1,1.0));  // 1.0 -> oriented consistenly

      orientation_recursive(domain, *seg_iter, cit);
    }
}

// template<typename DomainT, typename PointT>
// void create_debug_patch(DomainT& domain, int seg_num, PointT& lower_point, PointT& upper_point)
// {
//   typedef typename gsse::domain_traits<DomainT>::cell_iterator  cell_iterator;
//   typedef typename gsse::domain_traits<DomainT>::cell_handle              cell_handle;
//   typedef typename gsse::domain_traits<DomainT>::cell_2_vertex_mapping    cell_2_vertex_mapping;
//   typedef typename gsse::domain_traits<DomainT>::segment_iterator  segment_iterator;
//   typedef typename gsse::domain_traits<DomainT>::storage_type             storage_type;
//   typedef typename gsse::domain_traits<DomainT>::vertex_on_cell_iterator  vertex_on_cell_iterator;
//   typedef typename gsse::domain_traits<DomainT>::vertex_handle            vertex_handle;
  
//   std::cout << " ==========================" << std::endl;
//   std::cout << " create visual debug code.." << std::endl;
//   std::cout << " ==========================" << std::endl;
//   std::cout << std::endl;

//   DomainT patch;
//   segment_iterator segit = patch.add_segment();

//   std::map<PointT, vertex_handle>           point_map;
//   typename std::map<PointT, vertex_handle>::iterator cm_it;
//   boost::array<vertex_handle, 3>            element_container;
//   long element_counter = 0;


//   segment_iterator seg_iter = domain.segment_begin();
//   for(int i=0; i<seg_num ;i++)
//     seg_iter++;
  
// //   for( ; seg_iter != domain.segment_end(); ++seg_iter)
//     {      
//       for(cell_iterator cit = (*seg_iter).cell_begin(); cit != (*seg_iter).cell_end(); ++cit)
// {  
//   int element_index = 0;
//   vertex_on_cell_iterator vocit(*cit);   
//   while(vocit.valid())
//     {
//       PointT point = domain.get_point( *vocit );
      
//       if( (point[0] >= lower_point[0]) && (point[0] <= upper_point[0]) &&
//   (point[1] >= lower_point[1]) && (point[1] <= upper_point[1]) &&
//   (point[2] >= lower_point[2]) && (point[2] <= upper_point[2]) )  
// {  
//   cm_it = point_map.find(point);
  
//   if(cm_it == point_map.end())
//     {
//       patch.fast_point_insert(point);
      
//        element_container[element_index] = element_counter;
//       point_map[point] = element_counter;
//       element_counter++;
      
// #ifdef DEBUG
// //       std::cout << " adding point to patch: " << point << std::endl;
// #endif
//     }
//   else
//     {
// #ifdef DEBUG
// //       std::cout << " point found: " << (*cm_it).first << "/" << (*cm_it).second << std::endl;
// #endif
//       element_container[element_index] = (*cm_it).second;
//     }  
  
//   element_index++;
// }
      
//       ++vocit;
//     }

//   if(element_index == 3)
//     {
//       cell_handle ch = (*segit).add_cell_2(cell_2_vertex_mapping(element_container[0], 
//  element_container[1], 
//  element_container[2]  ) );
//     }    
// }      
//     }

// #ifdef DEBUG
//   std::cout << ".. added " << patch.point_size() << " to patch" << std::endl;
// #endif
//   patch.write_file("fs_patch.gau32");
// }



static void printHeader()
{
  std::cout << "  " << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "-- ViennaMesh: VGModeler Volume Mesher 1.0,  2010                --" << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "--  developed by:                                                --" << std::endl;
  std::cout << "--    Franz Stimpfl, Rene Heinzl, Philipp Schwaha                --" << std::endl;
  std::cout << "--  maintained by:                                               --" << std::endl;
  std::cout << "--    Josef Weinbub, Johann Cervenka                             --" << std::endl;
  std::cout << "--  Institute for Microelectronics, TU Wien, Austria             --" << std::endl;
  std::cout << "--  http://www.iue.tuwien.ac.at                                  --" << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "  " << std::endl;
}


extern int convert_dfise(const std::string& filename_in, const std::string& filename_out);

// *********************************
//
int main(int argc, char** argv)
{
   boost::timer t;
  printHeader();

#ifdef RELEASE
  if (argc < 3)
    {
      std::cout << "Usage: " <<  argv[0] << " input_file.{bnd|hin|gau32} output_file.gau3" << std::endl;
      return -1;
    } 
#else
  if (argc < 5)
    {
      std::cout << argv[0] << " fileinput fileoutput <additional points off/on - 0/1> <parallel meshing off/on - 0/1> <orientation test off/on - 0/1> <hin file for additional points>"<< std::endl;
      std::cout << "EXAMPLE: " <<  argv[0] << " input/levelset_orig.hin levelset_orig.gau3 0 0 0" << std::endl;
      return -1;
    } 
#endif

  std::string filename_in(argv[1]);
  std::string filename_out(argv[2]);
  std::vector<std::string> orient_changer;
  gsse::domain_32t  domain_input;
  gsse::domain_3t   domain_output;

  try
    {
      std::string::size_type pos = filename_in.rfind(".")+1;
      std::string filename_in_extension = filename_in.substr(pos, filename_in.size());

#ifdef DEBUG
      std::cout << " .. using file extension: " << filename_in_extension << std::endl;
#endif

      typedef gsse::domain_traits<gsse::domain_32t>::point_t  point_t;
      std::map<point_t,long> additional_vertices;
     
      // #############################################################################
      // SURFACE MESHING
      //
      t.restart();
      if (filename_in_extension == "bnd")
      {
         std::cout << "## Converting BND file: " << filename_in << std::endl;
         convert_dfise(filename_in, "temp.hin");

         create_surface_mesh("temp.hin", domain_input);    
         int ret = system("rm temp.hin");

         domain_input.write_file("surface.gau32");
      }
      else if (filename_in_extension == "hin")      
      { 
         std::cout << "## Converting HIN file: " << filename_in  << std::endl; 

         create_surface_mesh(filename_in, domain_input);    
         domain_input.write_file("surface.gau32");
      }
      else if (filename_in_extension == "gau32")      
      {  
         domain_input.read_file(filename_in, false);
      }
      std::cout << "  reading mesh::exec-time: " << t.elapsed() << std::endl;
      // #############################################################################
      // ORIENT CHECKER
      // 
      // [FS] .. run the checking test several times to see if the triangles are really oriented the right way
      //    
//      int check_orientation = 0;
//      try
//      {
//         check_orientation = boost::lexical_cast<int>(argv[3]);
//      }
//      catch(boost::bad_lexical_cast &)
//      {
//        check_orientation = 0;
//      }

// 
//      if(check_orientation)
//      {
//         std::cout << "## checking orientation .. " << std::endl;

//         int failure = 1;
//         int max_count = 0;
//         while(failure && max_count < 10)
//         {
//            failure = 0;

//            check_and_repair_orientation_new(domain_input);

//            typedef gsse::domain_traits<gsse::domain_32t>::cell_iterator  cell_iterator;
//            typedef gsse::domain_traits<gsse::domain_32t>::segment_iterator  segment_iterator;
//            typedef gsse::domain_traits<gsse::domain_32t>::storage_type             storage_type;

//            for (segment_iterator seg_iter = domain_input.segment_begin(); seg_iter != domain_input.segment_end(); ++seg_iter)
//            { 
//               for (cell_iterator cit = (*seg_iter).cell_begin(); cit != (*seg_iter).cell_end(); ++cit)
//               {
//                  storage_type value(1,1,0.0);
//                  domain_input.retrieve_quantity_direct((*cit).handle(), "orientation", value);
//                  if(value(0,0) != 1.0)
//                  {

//                     long temp =    (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1];
//                     (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1] = (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[2];
//                     (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[2] = temp;

//                     failure = 1;
//                  }
//               }
//            }
//            max_count++;
//         } // while
//      }
     
       // #############################################################################
       // VOLUME MESHING
       //
       std::cout << "## Starting volume meshing" << std::endl;
     
       // this argument defines whether to mesh in parallel or not
       //
// #ifndef RELEASE
//       if(boost::lexical_cast<int>(argv[4]))
//  create_volume_mesh_parallel(domain_input, filename_out);
//       else
// #endif
      t.restart();
      create_volume_mesh_sequential(domain_input, additional_vertices, domain_output, atof(argv[3]));
      std::cout << "  volume meshing::exec-time: " << t.elapsed() << std::endl;

       std::cout << "## Saving final mesh: " << filename_out << std::endl;

// #ifdef RELEASE
//       // output for INTEL
//       //
//       convert_gsse_2_hin(domain_output, filename_out);
// #else
       // output the GSSE format
       //
      t.restart();
       domain_output.write_file(filename_out);
      std::cout << "  writing domain::exec-time: " << t.elapsed() << std::endl;       
// #endif
      

    }
  catch (std::exception& ex)
    { 
      std::cout << ex.what() << std::endl;
    }
//   catch(...)
//     {
//       std::cout << "ERROR: some other error happend .." << std::endl;
//     }
  
  return 0;
}








