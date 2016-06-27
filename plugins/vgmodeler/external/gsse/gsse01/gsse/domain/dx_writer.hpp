/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef DX_WRITER_HH_ID
#define DX_WRITER_HH_ID

namespace gsse
{
   ///////////////////////////////////////////////////////////////////////////
   //
   //   DX writer facets
   //
   ///////////////////////////////////////////////////////////////////////////

   template <typename DomainT>
   void write_dx_file_edge(DomainT & domain, 
                           const std::string& filename, 
                           std::string quan_name="")
   {
         
      std::ofstream            file_out;
      std::string tempfilename(filename);

      file_out.open(tempfilename.c_str());
            
      // ===============
      // geometry section
      //


      typedef typename domain_traits<DomainT>::edge_type edge_type;
      typedef typename domain_traits<DomainT>::vertex_type vertex_type;
      typedef typename domain_traits<DomainT>::segment_iterator segment_iterator;
      typedef typename domain_traits<DomainT>::edge_iterator edge_iterator;
      typedef typename domain_traits<DomainT>::vertex_on_edge_iterator vertex_on_edge_iterator;


      segment_iterator segit;
      std::vector<std::pair<edge_type, segment_iterator> > edges;
      typename std::vector<std::pair<edge_type, segment_iterator > >::iterator iter;


      for(segit = domain.segment_begin(); segit != domain.segment_end(); ++segit)
      {
         for (edge_iterator eit = (*segit).edge_begin(); eit.valid(); ++eit)
         {
            edges.push_back(make_pair(*eit, segit));
         }
      }

      file_out << "object \"position list\" class array type float rank 1 shape "
               << domain_traits<DomainT>::dimension_tag::dim 
               << " items " << edges.size() * 2 << " data follows"
               << std::endl;

      unsigned long cnt = 0;

      for (iter = edges.begin(); iter != edges.end(); ++iter, ++cnt)
      {
         vertex_on_edge_iterator voeit((*iter).first);         
         file_out <<  domain.get_point(*voeit)  << "  \t\t\t # " << cnt << std::endl;
         ++voeit;
         ++cnt;
         file_out <<  domain.get_point(*voeit)  << "  \t\t\t # " << cnt << std::endl;
      }
      file_out << std::endl;
                        
      // ===============
      // topology section
      //

      // Print the edge list. 
      //

      file_out << "object \"edge list\" class array type int rank 0 items " 
               << edges.size() * 2  << " data follows " << std::endl;

      for (cnt = 0; cnt < edges.size() * 2; ++cnt)
      {
         file_out << "  " << cnt << "  ";
         if (cnt % 2) file_out << std::endl;
      }

      file_out << "attribute \"ref\" string \"positions\" "<< std::endl;

      
      // Print the loop list
      //
      
      
      file_out << "object \"loop list\" class array type int rank 0 items "
              << edges.size() << " data follows " << std::endl;

      for (cnt = 0; cnt < edges.size(); ++cnt)
      {
         file_out << "  " << cnt * 2 << "  " << std::endl;
      }

      file_out << "attribute \"ref\" string \"edges\"" << std::endl;



      // Print the face list
      //

      file_out << "object \"face list\" class array type int rank 0 items "
              << edges.size() << " data follows " << std::endl;

      for (cnt = 0; cnt < edges.size(); ++cnt)
      {
         file_out << "  " << cnt << "  " << std::endl;
      }

      file_out << "attribute \"ref\" string \"loops\"" << std::endl;

      // 
      // Quantity section
      // 

      file_out << "object \"data\" class array type float rank 0 items "
               << edges.size() * 2 << " data follows" << std::endl;


      for (iter = edges.begin(); iter != edges.end(); ++iter, ++cnt)
      {
         file_out << (*((*iter).second))((*iter).first,quan_name)(0, 0) << std::endl;
         file_out << (*((*iter).second))((*iter).first,quan_name)(0, 0) << std::endl;
      }
      
      file_out << "attribute \"dep\" string \"positions\"" << std::endl;

      // 
      // Conclusion
      // 

      file_out << "object \"map\" class field" << std::endl;
      file_out << "  component \"positions\"  \"position list\"" << std::endl;
      file_out << "  component \"edges\"  \"edge list\"" << std::endl;
      file_out << "  component \"loops\"  \"loop list\"" << std::endl;
      file_out << "  component \"faces\"  \"face list\"" << std::endl;
      file_out << "  component \"data\"   \"data\""      << std::endl;  
      file_out << "end " << std::endl;

      file_out.close();

   }
}


#endif
