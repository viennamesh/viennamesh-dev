/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

 Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
 Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
 Copyright (c)      2007 Franz Stimpfl              franz@gsse.at

 Use, modification and distribution is subject to the Boost Software
 License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

// *** system includes
//
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cmath>

// ** BOOST includes
//
#include<boost/lexical_cast.hpp>

// *** GSSE includes
//
#include "gsse/domain.hpp"



// ###################################################################################################
//
// GSSE hin reader 
//
// ###################################################################################################
//
//
template <typename Domain>
int convert_hin_2_gsse(const std::string& filename, Domain& domain)
{

  typedef typename gsse::domain_traits<Domain>::vertex_handle 		vertex_handle;
  typedef typename gsse::domain_traits<Domain>::cell_iterator		cell_iterator;
  typedef typename gsse::domain_traits<Domain>::vertex_on_cell_iterator	vertex_on_cell_iterator;
  typedef typename gsse::domain_traits<Domain>::vertex_iterator		vertex_iterator;
  typedef typename gsse::domain_traits<Domain>::segment_iterator		segment_iterator;
  typedef typename gsse::domain_traits<Domain>::segment_t   		segment_t;
  typedef typename gsse::domain_traits<Domain>::point_t			point_t;
  typedef typename gsse::domain_traits<Domain>::storage_type	storage_type;
  typedef typename gsse::domain_traits<Domain>::storage_type	storage_type;
  typedef typename gsse::domain_traits<Domain>::segment_t		segment_t;
  typedef typename gsse::domain_traits<Domain>::cell_2_vertex_mapping	cell_2_vertex_mapping;
  
  typedef typename gsse::domain_traits<Domain>::global_point_iterator	global_point_iterator;
  
  std::cout << " ====================== " << std::endl;
  std::cout << " IMPORTER::  hin -> GSSE" << std::endl;
  std::cout << "  Trying filename: " << filename << std::endl;
  std::cout << " ====================== " << std::endl;
  std::cout << std::endl;
  
 // ----------------------------
  
  std::ifstream file_in;
  file_in.open( filename.c_str() );
    
  std::vector<segment_t> temp_segments;
  
  // ----------------------------
  //
  // geometry part 
  //
  // ----------------------------
  std::string temp; 
  long number_of_points;
  
  // [FS][TODO] .. parse the input "Vertices[12345]{"
  //
//   file_in >> temp;
  
//   std::string::size_type pos1 = temp.rfind("[")+1;
//   std::string::size_type pos2 = temp.size()-2;
//   std::string temp2 = temp.substr(pos1, pos2-pos1);
  
//   number_of_points = boost::lexical_cast<long>(temp2);

  file_in >> number_of_points;

  std::cout << "[FS] .. num of points: " << number_of_points << std::endl;
  
  for (long counter = 0; counter < number_of_points; ++counter)
    {
      double cx,cy,cz;
      file_in >> cx;
      file_in >> cy;
      file_in >> cz;
//       std::cout << "cx: " << cx << std::endl;
//       std::cout << "cy: " << cy << std::endl;
//       std::cout << "cz: " << cz << std::endl;
      domain.fast_point_insert(point_t(cx, cy,cz));     
    }
  
  
  // ----------------------------
  //
  // topology part 
  //
  // ----------------------------

  std::cout << "[FS] .. reading topological information" << std::endl;
  
  std::string tempstring;
  file_in >> tempstring >> tempstring >> tempstring >> tempstring;
  
  long number_of_segments;
  file_in >> number_of_segments; //  number of points 

  std::cout << "[FS] .. number of segments: " << number_of_segments << std::endl;

  // /////////////////////////////////////////////////////////////////////////
  //   next we read in the segment information 
  //   -> topological information is stored into the segment
  // /////////////////////////////////////////////////////////////////////////
  
  
  temp_segments.resize(number_of_segments*2);
  
  int ind1, ind2, ind3, ind4;
  
  int surface_nr;
  
  while (1)
    {
      if (	file_in >> surface_nr 
		>> ind1 >> ind2 >> ind3 >> ind4
		)
	{
// 	  std::cout << "ind1: "<< ind1 << "/" << ind2 << "/" << ind3 << "/" << ind4 << std::endl;
	  
	  
	  // ################# Repository Part #######################
	  temp_segments[surface_nr].add_cell_2(cell_2_vertex_mapping(  
								     ind1,
								     ind2,
								     ind3,
								     ind4
								     ) );
	  // ################# Repository Part #######################
	}
      else
	break;
    }

  std::cout << "[FS] .. after read in of topological information" << std::endl;

  long seg_counter = 0;
  for (unsigned int i =0; i < temp_segments.size(); ++i)
    {
      if (temp_segments[i].cell_size() > 0)
	{
	  std::stringstream segname;
	  segname << "segment" << seg_counter++;
	  temp_segments[i].set_name(segname.str());
	      domain.add_segment(temp_segments[i]);
	}	  
      
    }
  
  
  file_in.close();

  std::cout << "[FS] .. after close file" << std::endl;
  
  return 0;
}


// ###################################################################################################
//
// GSSE into intel format method
//
// ###################################################################################################
//
//
template <typename Domain>
int convert_gsse_2_hin(Domain& domain, const std::string& filename)
{

	typedef typename gsse::domain_traits<Domain>::vertex_handle 		vertex_handle;
	typedef typename gsse::domain_traits<Domain>::cell_iterator		cell_iterator;
	typedef typename gsse::domain_traits<Domain>::vertex_on_cell_iterator	vertex_on_cell_iterator;
	typedef typename gsse::domain_traits<Domain>::vertex_iterator		vertex_iterator;
	typedef typename gsse::domain_traits<Domain>::segment_iterator		segment_iterator;
	typedef typename gsse::domain_traits<Domain>::segment_t   		segment_t;
	typedef typename gsse::domain_traits<Domain>::point_t			point_t;
	typedef typename gsse::domain_traits<Domain>::storage_type	storage_type;
	typedef typename gsse::domain_traits<Domain>::segment_t		segment_t;
	typedef typename gsse::domain_traits<Domain>::cell_2_vertex_mapping	cell_2_vertex_mapping;

	typedef typename gsse::domain_traits<Domain>::global_point_iterator	global_point_iterator;

// 	std::cout << " ====================== " << std::endl;
// 	std::cout << " EXPORTER::  GSSE -> Intel format" << std::endl;
// 	std::cout << "  Trying filename: " << filename << std::endl;
// 	std::cout << " ====================== " << std::endl;
// 	std::cout << std::endl;


 // ----------------------------

      const unsigned int dimension = gsse::domain_traits<Domain>::dimension_tag::dim;
      std::ofstream            file_out;
    
      BOOST_STATIC_ASSERT(dimension >= 2);
      BOOST_STATIC_ASSERT(dimension <= 3);

    
      file_out.open( filename.c_str() );
    
    
      file_out << domain.point_size() << std::endl;
      file_out.setf(std::ios::right, std::ios::adjustfield);



      // *** get all geometrical points
      global_point_iterator  gpi, gpi_end;
	gpi = 		domain.point_begin();
	gpi_end =	domain.point_end(); 

      // *** output all points to the wss file
      for (; gpi != gpi_end; ++gpi)
      {
         //std::cout << *pl_it << std::endl;
	file_out << std::setprecision(16) << std::setiosflags(std::ios::fixed) <<  std::setiosflags(std::ios::showpos)  << (*gpi).second << std::endl;

      }
      // *** end of the POINTS section
      file_out.unsetf(std::ios::showpos);





    
      // *** begin of the SEGMENTS
    
      segment_iterator 	seg_iter;
    
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
// 		std::cout << "!!!!! not consistent tetrahedron!!!! .. STOP converting .. " << std::endl;
		return -1;
	      }

         }

	 ++seg_counter;
      }
      file_out.close();

  return 0;
}



// ###################################################################################################
//
// convert hin file into gsse
//
// ###################################################################################################
//
template <typename DomainT>
int hin_2_gsse(const std::string& filename_in, DomainT& domain)
{
  std::cout << " ==========   " << " using hin to gsse converter " << std::endl;

  convert_hin_2_gsse(filename_in, domain);

//   domain.write_file(filename_out);
//   domain.write_dx_file("dataexplorer_out.dx");

  return 0;
}


// ###################################################################################################
//
// convert gsse file into hin
//
// ###################################################################################################
//
template <typename DomainT>
int gsse_2_hin(DomainT& domain, const std::string& filename_out)
{
  std::cout << " ==========   " << " using gsse to hin converter " << std::endl;
    
  convert_gsse_2_hin(domain, filename_out);

  return 0;
}
