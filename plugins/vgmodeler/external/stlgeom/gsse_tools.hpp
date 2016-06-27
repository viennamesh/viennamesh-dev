using namespace boost::phoenix;
using namespace boost::phoenix::arg_names;
using namespace gsse::access_specifier; 

template<typename T>
void fs_test(T& bla)
{
   std::cout << " .. test " << std::endl;
}


template<typename FindContainerT, typename SearchContainerT>
int find_edge_in_triangle(FindContainerT& find, SearchContainerT& search)
{
   for(long i = 0; i < 3; ++i)
   {
      int count = 0;
#ifdef DEBUGALL
      std::cout << "..search["<<i<<"]: " << search[i] << std::endl;
#endif

      for(long j = 0; j < 2; ++j)
      {
#ifdef DEBUGALL
         std::cout << "..find["<<j<<"]: " << find[j] << std::endl;
#endif
         if(search[(i+j)%3] == find[j])
         {
            count++;
         }
      }

      if(count == 2)
         return 1;
   }
   
   return 0;
}

template<typename EdgeVectorT, typename IntVectorT, typename HolesVectorT, typename TotalEdgesT>
int find_next_edge(EdgeVectorT& holes, long& start, long& current, IntVectorT& used, HolesVectorT& found_holes, TotalEdgesT& edges, long& face)
{
#ifdef DEBUGALL
   std::cout << ".. find_next_edge: start: " << start << std::endl;
#endif

  int giveup = 0;

//   if(current == holes.size())
//   {
//      std::cout << "..setting current to 0" << std::endl;
//      current = 0;
//   }
  
  typename EdgeVectorT::iterator eit;
  long i = 0;
  for(eit = holes.begin(); eit != holes.end(); ++eit, ++i)
  {
//      typename TotalEdgesT::value_type::data_type temp_edge;
     std::vector<long> current_edge, temp_edge;

     temp_edge.push_back(edges[(*eit).first][0]);
     temp_edge.push_back(edges[(*eit).first][1]);

     int orientation = 0;
     
//      if((*(*found_holes[face].end()).begin()).second == 0)
     if(holes[current] == 0)
     {
        current_edge.push_back(edges[current][0]);
        current_edge.push_back(edges[current][1]);
     }
     else
     {
        current_edge.push_back(edges[current][1]);
        current_edge.push_back(edges[current][0]);
        orientation = 1;
     }

#ifdef DEBUGALL
     std::cout << "..edge: " << (*eit).first << "(" << temp_edge[0] << " / " << temp_edge[1] << ")"
               << " :: curr: " << current << "(" << edges[current][0] << " / " << edges[current][1] << ")"
               << " :: i: " << i << " :: used: " << used[(*eit).first] << std::endl;
#endif
  
//      if( ( (edges[(*eit).first][0] == current_edge[0]) || (edges[(*eit).first][1] == current_edge[1])) && 
//          (used[i] == -1) && ((*eit).first != current) )

     if( (temp_edge[0] == current_edge[1]) && (used[(*eit).first] == -1) && ((*eit).first != current) )
     {
#ifdef DEBUGALL
        std::cout << "..found1: " << (*eit).first << std::endl;
#endif

        used[(*eit).first] = start;
	
        EdgeVectorT test;
        test[(*eit).first] = 0;
        found_holes[face].push_back(test);
        current = (*eit).first;
        (*eit).second = 0;
        
        if( ((*eit).first == start) || (giveup == 10) )
           return 1;
        else	    
           find_next_edge(holes, start, current, used, found_holes, edges, face);
     }
//      else if(((temp_edge[0] == edges[current][0]) || (edges[(*eit).first][1] == edges[current][1])) 
//              && 
//              (used[i] == -1) && ((*eit).first != current) )
     else if( (temp_edge[1] == current_edge[1]) && (used[(*eit).first] == -1) && ((*eit).first != current) )
     {
#ifdef DEBUGALL
        std::cout << "..found2: " << (*eit).first << std::endl;
#endif
        used[(*eit).first] = start;
     
//         typename EdgeVectorT::value_type edge;
//         edge[0] = (*eit)[1];
//         edge[1] = (*eit)[0];

        EdgeVectorT test;
        test[(*eit).first] = 1;
        found_holes[face].push_back(test);
     
//         holes[i][0] = edge[0];
//         holes[i][1] = edge[1];
     
        current = (*eit).first;
        (*eit).second = 1;
     
        if( ((*eit).first == start) || (giveup == 10) )
           return 1;
        else	    
           find_next_edge(holes, start, current, used, found_holes, edges, face);	  
     }
  }
  return 0;
}


template<typename MeshT, typename FullSpaceT, typename MaterialT>
void convert_mesh_to_gsse(MeshT& mesh, FullSpaceT& full_space, MaterialT& material_chart_mapping)
{
  typedef gsse::property_domain<FullSpaceT>  GSSE_Domain;
  
  typedef typename gsse::result_of::property_CoordPosT<GSSE_Domain>::type              CoordPosT;
  typedef typename gsse::result_of::property_CellT<GSSE_Domain>::type                  CellT;
  
  typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type SpaceTopologySegmentsT;
  typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<GSSE_Domain>::type SpaceQuantitySegmentsT;
  typedef typename gsse::result_of::property_FBPosT<GSSE_Domain>::type                 FBPosT;
  
  typedef typename gsse::result_of::property_QuanStorage_0form<GSSE_Domain>::type      Storage_0form;
  
  static const long DIMG = gsse::result_of::property_DIMG<GSSE_Domain>::value;   
  static const long DIMT = gsse::result_of::property_DIMT<GSSE_Domain>::value;   

  SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(full_space);
  SpaceQuantitySegmentsT& segments_quantity = gsse::at_dim<AQ>(full_space);    
  FBPosT&                 geometry          = gsse::at_dim<AP>(full_space);


  typedef typename MaterialT::mapped_type map_value_type;
  

  // [INFO] save the mesh vertices in the space
  //
  vgmnetgen::PointIndex pi; 
  long vi = 0;
  for(pi = vgmnetgen::PointIndex::BASE; pi < mesh.GetNP()+vgmnetgen::PointIndex::BASE; pi++, vi++)
    {
      CoordPosT vertex;
      vertex[0] = mesh[pi](0);      
      vertex[1] = mesh[pi](1);      
      vertex[2] = mesh[pi](2);      
      
      gsse::at(vi)(geometry) = vertex;
    }      

#ifdef DEBUGALL
  std::cout << ".. size: " << gsse::size(geometry) << std::endl;
  std::cout << ".. number surface ele: " << mesh.GetNSE() << std::endl;
#endif

  typename MaterialT::iterator mcit;
  for(mcit = material_chart_mapping.begin(); mcit != material_chart_mapping.end(); ++mcit)
  {    
     typename map_value_type::iterator cit;     
     for(cit = (*mcit).second.begin(); cit != (*mcit).second.end(); ++cit)
     {
        vgmnetgen::SurfaceElementIndex si;    
        for(si = 0; si < mesh.GetNSE(); si++)
        {
#ifdef DEBUGALL
           std::cout << ".. si: " << si << std::endl;
#endif

           if(mesh.GetFaceDescriptor(mesh[si].GetIndex()).SurfNr() == (*cit).first)
           {
              vgmnetgen::Element2d sel = mesh[si];

#ifdef DEBUGALL              
              std::cout << ".. writing element " << (*cit).first+1 << " : " << sel << std::endl;
#endif
              
              if((*cit).second)             
                 sel.Invert();
              
              CellT cell;
              for (int j = 0; j < sel.GetNP(); j++)
              {
                 cell[j] = sel[j]-1;
              }
              
              gsse::pushback(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at((*mcit).first)(segments_topology)))) = cell;
              gsse::at_index(gsse::at((*mcit).first)(segments_topology)) = "segment_"+boost::lexical_cast<std::string>((*mcit).first);
              gsse::at_index(gsse::at((*mcit).first)(segments_quantity)) = "segment_"+boost::lexical_cast<std::string>((*mcit).first);
           }
        }
     }
  }
  

//   vgmnetgen::SurfaceElementIndex si;    
//   for(si = 0; si < mesh.GetNSE(); si++)
//   {     
//      typename MaterialT::iterator fit;
//      for(fit = materials.begin(); fit != materials.end(); ++fit)
//      {
//         std::cout << "..face: " << (*fit).first << std::endl;

//         vgmnetgen::Element2d sel = mesh[si];
      
//         if(mesh.GetFaceDescriptor(mesh[si].GetIndex()).SurfNr() == (*fit).first)
//         {
//            std::map<int,int>::iterator mit;  
//            int mat_count = 0;
//            for(mit = (*fit).second.begin(); mit != (*fit).second.end(); ++mit, ++mat_count)
//            {              
//               std::cout << "..index: " << si << " :: "
//                         << "..sel: " << sel << std::endl;

//               std::cout << ".. mat: " << (*mit).first << std::endl;

//               // [INFO][TODO] this should be calculated according to the orientation of the segment
//               //        not just inverted from the previous segment
//               //
//               if(mat_count> 0)             
//                  sel.Invert();
              
//               CellT cell;
//               for (int j = 0; j < sel.GetNP(); j++)
//               {
//                  cell[j] = sel[j]-1;
//               }
//               gsse::pushback(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at((*mit).first)(segments_topology)))) = cell;
//               gsse::at_index(gsse::at((*mit).first)(segments_topology)) = "segment_"+boost::lexical_cast<std::string>((*fit).first);
//               gsse::at_index(gsse::at((*mit).first)(segments_quantity)) = "segment_"+boost::lexical_cast<std::string>((*fit).first);
//            }
//         }
            
//      } 

// //      long cnt_cl = 0;
// //      gsse::traverse<AC>()
// //         [
// //            gsse::traverse<AT_cl>()
// //            [
// //               std::cout << ref(cnt_cl) << val(" \t")
// //               ,
// //               ++ref(cnt_cl)
// //               ,
// //               gsse::traverse()
// //               [
// //                  std::cout << _1 << val("  ")
// //                  ]
// //               , std::cout << val("") << std::endl
// //               ]
// //            ](full_space);

// //      std::cout << ".. cell size: " 
// //                << gsse::size(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(mat)(segments_topology))))
// //                << std::endl;
//   }
}



template <int GeomDIM, int TopoDIM, int QuanDIM>
struct SpaceGenerator
{
  typedef gsse::cell_simplex                         CellType;
  typedef double                                     StorageTPosition;
  typedef double                                     StorageTQuantity;
  typedef gsse::array<StorageTPosition, GeomDIM>     CoordPosT;
  
  typedef typename gsse::cellcomplex::create_index_dense::template apply<TopoDIM, CellType>::type      CellComplex;
    
  typedef gsse::fiberbundle::collection<
    QuanDIM, gsse::cell_nothing
    , boost::fusion::pair<gsse::access_specifier::AT_vx, gsse::fiberbundle::dforms::FB_0form>
    , boost::fusion::pair<gsse::access_specifier::AT_ee, gsse::fiberbundle::dforms::FB_0form>
    , boost::fusion::pair<gsse::access_specifier::AT_cl, gsse::fiberbundle::dforms::FB_0form>
  > QuanComplex;



typedef typename gsse::container::create< CoordPosT >::type  FBPosT;  
typedef typename gsse::container::create_index_dense< CellComplex, std::vector, std::string >::type   SpaceTopologySegmentsT;
typedef typename gsse::container::create_index_dense< QuanComplex, std::vector, std::string >::type   SpaceQuantitySegmentsT;


typedef boost::fusion::map<
  boost::fusion::pair<gsse::access_specifier::AP, FBPosT>
  , boost::fusion::pair<gsse::access_specifier::AC, SpaceTopologySegmentsT>
  , boost::fusion::pair<gsse::access_specifier::AQ, SpaceQuantitySegmentsT>
> FullSpace;   
};


template <typename FullSpace>
void initialize_topology(FullSpace& full_space, std::ostream& stream = std::cout)
{
   typedef typename gsse::result_of::at_dim<FullSpace, AC>::type                 SpaceTopologySegmentsT;
   typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type  CellComplex;
   typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_cl>::type        CellT;      
   typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_vx>::type        VertexT;     

   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(full_space);

   //dump_type<VertexT>();

   long segment_size = segments_topology.size();
   // --------------------------------------------
   for(long si = 0; si < segment_size; ++si)
     {   
      //
      // vertex_con_t --> to impose uniqueness on the segment vertices
      //
      typedef std::map<long, bool>      vertex_con_t;
      //
      // vertex_map_t --> to bridge between the dense vertex container elements
      //                  and the sparse real vertex indices
      //
      typedef std::map<size_t, size_t>    vertex_map_t;
      vertex_con_t      vertex_con;   
      vertex_map_t      vertex_map;      

      size_t cellsize = gsse::at_dim<AT_cl>(gsse::at_fiber(segments_topology[ si ])).size();

      //
      // SETUP UNIQUE VERTEX CONTAINER
      //
      for (size_t ci = 0 ; ci < cellsize; ++ci)
      {
         CellT cell = gsse::at(ci) (gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)))	);
      
         for (size_t voci = 0; voci < cell.size(); ++voci)    
         {
            if (!vertex_con[ cell[voci] ])
            	vertex_con[ cell[voci] ] = true;
         }           
      }
      //
      // RESIZE VERTEX CONTAINER OF THE ACTUAL SEGMENT ACCORDING TO THE UNIQUE VERTEX CONTAINER
      //      
      gsse::resize(vertex_con.size())(gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology))));
      //
      // SETUP TRANSFER UNIQUE VERTEX DATASET TO THE SEGMENT VERTEX CONTAINER
      //        
      size_t   vindex = 0;
      for(vertex_con_t::iterator viter = vertex_con.begin(); viter != vertex_con.end(); ++viter)
      {
         gsse::at_index(gsse::at(vindex)(gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(si)(segments_topology))))) = (*viter).first; 
         //
         // setup index space link: 
         //
         //       key:     real vertex index
         //       value:   base space index
         //        
         vertex_map[(*viter).first] = vindex;         
         vindex++;
      }
      //
      // SETUP CELL ON VERTEX RELATION
      //
      for (size_t ci = 0 ; ci < cellsize; ++ci)
      {
         CellT cell = gsse::at(ci) (gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)))	);      
         for (size_t voci = 0; voci < cell.size(); ++voci)    
         {
            gsse::pushback // store on the fiber (vector<long>)
            ( 
               gsse::at_fiber // access the fiber of this vertex
               (
                  //
                  // use index space link to derive the vertex container position(aka base space index) 
                  // for a given real vertex index out of the sparse vertex index space
                  //                |
                  //                v
                  gsse::at(vertex_map[gsse::at(voci)(cell)]) //  access the vertex container element, a certain vertex
                  (gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(si)(segments_topology)) ) ) // <- vertex container
               ) 
            ) = ci;   
         } 


	 // [TODO] calculate the normal vector of this cell and store it in FB
	 //
	 
      }
   }
   //
   // CREATE EDGES
   //
   gsse::create_edges(full_space);
   
#ifdef DEBUGALL
   for (size_t si = 0; si < segment_size; ++si)
   {  
      stream << "segment-number " << si << std::endl;
      stream << "  vertexsize " << gsse::size(gsse::at_dim<AT_vx>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) << std::endl;
      //      stream << "  edgesize " << gsse::size(gsse::at_dim<AT_ee>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) << std::endl;
      stream << "  cellsize " << gsse::size(gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) << std::endl;
         
      std::cout << "\tsegment #: " << si << std::endl;
      std::cout << "\t\tvertexsize: \033[1;32m"
         << gsse::size(gsse::at_dim<AT_vx>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) 
         << "\033[0m" << std::endl;
      std::cout << "\t\tcellsize: \033[1;32m"  
         << gsse::size(gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) 
         << "\033[0m" << std::endl;
//       std::cout << "\t\tedgesize: \033[1;32m"  
//          << gsse::size(gsse::at_dim<AT_ee>( gsse::at_fiber((gsse::at(si) (segments_topology))) )) 
//          << "\033[0m" << std::endl;
   }
#endif
}



template<typename FullSpace>
void write_space(FullSpace& full_space, std::ostream& output = std::cout)
{
  typedef typename gsse::result_of::at_dim<FullSpace, AC>::type                 SpaceTopologySegmentsT;   
  typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type  CellComplex;  
  typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_cl>::type        CellT;  
  static const long TOPODIM = CellComplex::DIM_cc;
  SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(full_space);   	
  
  
  // [TODO] get geometric dimension from FullSpace
  //
  long gsse_geom_dim = 3;
  
  // geometric information
  //
  output << gsse_geom_dim << std::endl;
  output << gsse::size(gsse::at_dim<AP>(full_space)) << std::endl;
  output.setf( std::ios::right,std::ios::adjustfield);
  output << std::setprecision(12) << std::setiosflags(std::ios::fixed);
  
  gsse::traverse<AP>()
    [
     gsse::traverse()
     [
      output  << gsse::acc << val("  ")
     ]
     , output << val("") << std::endl
    ](full_space);

  
  // print the segment size
  //
  output << gsse::size(gsse::at_dim<AC>(full_space)) << std::endl;
  
  for (size_t cnt_seg = 0; cnt_seg < gsse::size(gsse::at_dim<AC>(full_space)); ++cnt_seg)
    {      
      size_t cellsize   = gsse::size(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at_dim<AC>(full_space)[cnt_seg])));
      size_t vertexsize = gsse::size(gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at_dim<AC>(full_space)[cnt_seg])));
      
      // segment name   
      //
      std::string segname = gsse::at_index(gsse::at(cnt_seg)(segments_topology));

      if(segname == "") 
	output << "Default-Segment-"+boost::lexical_cast<std::string>(cnt_seg) << std::endl;
      else
	output << segname << std::endl;

      // print size of cells and vertices
      //
      output << cellsize << std::endl;   
      output << vertexsize << std::endl;

      for (size_t ci = 0 ; ci < cellsize; ++ci)
	{
	  CellT cell = gsse::at(ci) (gsse::at_dim<AT_cl>(gsse::at_fiber(segments_topology[cnt_seg])));
	  output << ci;
	  
	  for (size_t voci = 0; voci < gsse::size(cell); ++voci)
	    {
	      output << " " << cell[voci];
	    }         
	  output << std::endl;         
	}
      
#ifdef DEBUG_GAU
      std::cout << ".. finished reading segments topology.." << std::endl;
#endif      
      
//       long cnt_cl = 0;
//       gsse::traverse<AC>()
//       [
//          gsse::traverse<AT_cl>()
//          [
//             output << ref(cnt_cl) << val(" \t")
//             ,
//             ++ref(cnt_cl)
//             ,
//             gsse::traverse()
//             [
// 	     output << _1 << val("  ")
//             ]
//             , output << val("") << std::endl
//          ]
//       ](full_space);
      
      
//       output << "Quantity: " << "vertex " << 
// 	gsse::size(gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at_dim<AQ>(full_space)[cnt_seg]))) << std::endl;

           
//       gsse::traverse<AQ>()
// 	[
//          gsse::traverse<AT_vx>()
//          [
// 	  output << val("") << gsse::acc0 << std::endl   // ## quantity name
// 	  ,
// 	  output << gsse::asize1 << std::endl
// 	  ,
// 	  gsse::traverse()
// 	  [
// 	   output << gsse::acc << val(" ")
// 	   ,
// 	   gsse::traverse() // for multiple values
// 	   [
// 	    output << boost::phoenix::arg_names::_1
// 	   ]
// 	   ,
// 	   output << val("") << std::endl
//           ]
// 	 ]
// 	](full_space);
     

//       output << "Quantity: " << "edge " << 
//          gsse::at_dim<AT_ee>(gsse::at_fiber(gsse::at_dim<AQ>(full_space)[cnt_seg])).size() << std::endl;

	
//       gsse::traverse<AQ>()
//       [
//          gsse::traverse<AT_ee>()
//          [
//             output << val("") << gsse::acc << std::endl
//             ,
//             output << gsse::asize1 << std::endl
//             ,
//             gsse::traverse()
//             [
//                output << gsse::acc << val(" ")
//                ,
//                gsse::traverse()
//                [
//                   output << boost::phoenix::arg_names::_1
//                ]
//                ,
//                output << val("") << std::endl
//            ]
//        ]
//     ](full_space);

   
//       output << "Quantity: " << "facet " << 
//          gsse::at_dim<AT_f0>(gsse::at_fiber(gsse::at_dim<AQ>(full_space)[cnt_seg])).size() << std::endl;


//       gsse::traverse<AQ>()
//       [
//          gsse::traverse<AT_f0>()
//          [
//             output << val("") << gsse::acc << std::endl
//             ,
//             output << gsse::asize1 << std::endl
//             ,
//             gsse::traverse()
//             [
//                output << gsse::acc << val(" ")
//                ,
//                gsse::traverse()
//                [
//                   output << boost::phoenix::arg_names::_1
//                ]
//                ,
//                output << val("") << std::endl
//             ]
//         ]
//      ](full_space);


//        output << "Quantity: " << "cell " << 
// 	 gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at_dim<AQ>(full_space)[cnt_seg])).size() << std::endl;
	 
//       gsse::traverse<AQ>()
//       [
//          gsse::traverse<AT_cl>()
//          [
//             output << val("") << gsse::acc << std::endl
//             ,
//             output << gsse::asize1 << std::endl
//             ,
//             gsse::traverse()
//             [
//                output << gsse::acc << val(" ")
//                ,
//                gsse::traverse()
//                [
//                   output << boost::phoenix::arg_names::_1
//                ]
//                ,
//                output << val("") << std::endl
//            ]
//         ]
//      ](full_space);
       

#ifdef DEBUG_GAU
      std::cout << ".. finished reading quantities .." << std::endl;
#endif      
   }
}


// template<typename EdgeVectorT, typename IntVectorT, typename HolesVectorT, typename TotalEdgesT>
// int find_next_edge1(EdgeVectorT& holes, long& start, long& current, IntVectorT& used, HolesVectorT& found_holes, TotalEdgesT& edges, long& face)
// {
//    std::cout << ".. find_next_edge: start: " << start << std::endl;
   
//    typename EdgeVectorT::iterator eit;
//    long i = 0;
//    for(eit = holes.begin(); eit != holes.end(); ++eit, ++i)
//    {
//       std::cout << "..edge: " << (*eit).first << " :: curr: " << current << "(" << edges[current][0] << " / " << edges[current][1] << ")"
//                 <<  " :: i: " << i << " :: used: " << used[i] << std::endl;
      
//       if(((edges[(*eit).first][0] == edges[current][1]) || (edges[(*eit).first][1] == edges[current][0])) && 
//          (used[i] == -1) && ((*eit).first != current) )
//       {
//          std::cout << "..found1: " << (*eit).first << std::endl;
         
//          used[i] = start;
         
//          EdgeVectorT test;
//          test[(*eit).first] = (*eit).second;
//          found_holes[face].push_back(test);
//          current = (*eit).first;
         
//          if( ((*eit).first == start) || (giveup == 10) )
//             return 1;
//          else	    
//             find_next_edge(holes, start, current, used, found_holes, edges, face);
//       }

      
//    }
// }
