#define DEBUGORIENTATION 1
#define DEBUGNEARESTMATCH 1

template<typename DomainT, typename SegmentT, typename CellIterator, typename EdgeVectorT>
bool orientation_recursive(DomainT& domain,  SegmentT& segment,  CellIterator& cit, EdgeVectorT& non_manifold_edges)
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


   // the cell, which are adjacent to the given cell iterator cit, are oriented
   //
   for (edge_on_cell_iterator eocit(*cit); eocit.valid(); ++eocit)
   {
#ifdef DEBUGORIENTATION
      std::cout << ".. eocit: " << (*eocit).handle1() << " " << (*eocit).handle2() << std::endl;
#endif
      
      int do_it = 1;

      typename EdgeVectorT::iterator eit;
      for(eit = non_manifold_edges.begin(); eit != non_manifold_edges.end(); ++eit)
      {
         if( ((*eit)[0] == (*eocit).handle1() && (*eit)[1] == (*eocit).handle2()) ||
             ((*eit)[1] == (*eocit).handle1() && (*eit)[0] == (*eocit).handle2()) )
         {
            do_it = 0;

            domain.store_quantity(*cit, "orientation", storage_type(1,1,-2.0));  // -2.0 -> triangle adjacent to NON-MANIFOLD EDGE

#ifdef DEBUGORIENTATION
            std::cout << ".. do not orient triangle: " << (*cit) << std::endl;
#endif
         }
      }

      for (cell_on_edge_iterator coeit(*eocit); coeit.valid(); ++coeit)
      {
         if ((*coeit) == (*cit))
	    continue;
         if ( (domain(*coeit, "orientation")(0,0) != 0.0) || (domain(*coeit, "orientation")(0,0) == -2.0) )
	    continue;

         bool is_oriented_consistently = 
	    gsse::check_oriented_neighbor(segment.retrieve_topology().get_cell( (*coeit).handle() ), 
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

         orientation_recursive(domain, segment, coeit, non_manifold_edges);
      }
   }
}


template<typename Domain, typename EdgeVectorT>
void check_and_repair_orientation(Domain& domain, EdgeVectorT& non_manifold_edges)
{
   typedef typename gsse::domain_traits<Domain>::cell_iterator		  cell_iterator;
   typedef typename gsse::domain_traits<Domain>::segment_iterator	  segment_iterator;
   typedef typename gsse::domain_traits<Domain>::storage_type             storage_type;
   typedef typename gsse::domain_traits<Domain>::vertex_on_cell_iterator  vertex_on_cell_iterator;

   std::cout << " ====================== " << std::endl;
   std::cout << " check and repair  orientation.." << std::endl;
   std::cout << " ====================== " << std::endl;
   std::cout << std::endl;

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

     
      // [TODO] include the intersection test here ... but only for the first segment 
      //        rest will be derived from this segment
      //

     
     
      domain.store_quantity(*cit, "orientation", storage_type(1,1,1.0));  // 1.0 -> oriented consistenly
   }

   for (seg_iter = domain.segment_begin(); seg_iter != domain.segment_end(); ++seg_iter)
   {
      cell_iterator cit = (*seg_iter).cell_begin(); 
      orientation_recursive(domain, *seg_iter, cit, non_manifold_edges);
   }	
}


template<typename FindContainerT, typename SearchContainerT>
int find_edge_in_triangle(FindContainerT& find, SearchContainerT& search)
{
   for(long i = 0; i < 3; ++i)
   {
      int count = 0;
#ifdef DEBUGALLTOOLS
      std::cout << "..search["<<i<<"]: " << search[i] << std::endl;
#endif

      for(long j = 0; j < 2; ++j)
      {
#ifdef DEBUGALLTOOLS
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

// ##############################################################################
//
// find nearest match
//
// ##############################################################################

template<typename DomainT, typename CellIterator, typename EdgeT, typename TargetT, typename ValueT>
long nearest_match_recursion(DomainT& domain, CellIterator& start_cell, EdgeT& non_manifold_edge, TargetT& target,
                             ValueT recursion_counter, ValueT& recursion_max_depth)
{
   typedef typename gsse::domain_traits<DomainT>::edge_on_cell_iterator    edge_on_cell_iterator;
   typedef typename gsse::domain_traits<DomainT>::cell_on_edge_iterator    cell_on_edge_iterator;
   typedef typename gsse::domain_traits<DomainT>::vertex_on_cell_iterator  vertex_on_cell_iterator;
   typedef typename gsse::domain_traits<DomainT>::storage_type             storage_type;

#ifdef DEBUGNEARESTMATCH
   std::cout << ".. in recursion ..  depth: " << recursion_counter << std::endl;
#endif

   // [INFO] stop condition for recursion
   //
   if(recursion_counter >= recursion_max_depth)
      return -1;          
   
   recursion_counter++;
   domain.store_quantity(*start_cell, "nearest_match", storage_type(1,1,1.0));  // 1.0 -> already visited

   // [INFO] find the target vertex in the current cell
   //        
   vertex_on_cell_iterator vocit(*start_cell);
   typename TargetT::iterator tit;

#ifdef DEBUGNEARESTMATCH
   std::cout << "cell vertices: ";
#endif
   while (vocit.valid())
   {
#ifdef DEBUGNEARESTMATCH
      std::cout << (*vocit).handle() << " ";
#endif      
      for(tit = target.begin(); tit != target.end(); ++tit)
      {
         if((*tit) == (*vocit).handle())
         {
            std::cout << ".. route to vertex: " << (*tit) << " found! " << std::endl;
            return (*tit);
         }
      }
      vocit++;
   }
#ifdef DEBUGNEARESTMATCH
   std::cout << std::endl;
#endif

   for (edge_on_cell_iterator eocit(*start_cell); eocit.valid(); ++eocit)
   {
      // [INFO] do not use the manifold edge
      //
      if( (non_manifold_edge[0] == (*eocit).handle1() && non_manifold_edge[1] == (*eocit).handle2()) ||
          (non_manifold_edge[1] == (*eocit).handle1() && non_manifold_edge[0] == (*eocit).handle2()) )
      {         
#ifdef DEBUGNEARESTMATCH
         std::cout << ".. nm edge .. " << std::endl;
#endif
      }
      else
      {
#ifdef DEBUGNEARESTMATCH
         std::cout << ".. in else .. " << std::endl;
#endif         
         for(cell_on_edge_iterator coeit(*eocit); coeit.valid(); ++coeit)
         {            
            if(domain(*coeit, "nearest_match")(0,0) == 1.0) // only visit new triangles
               continue;

#ifdef DEBUGNEARESTMATCH
            std::cout << "..before calling next level .. " << std::endl;
#endif
            long result = nearest_match_recursion(domain, coeit, non_manifold_edge, target, recursion_counter, recursion_max_depth);

            if(result != -1)
               return result;

#ifdef DEBUGNEARESTMATCH
            std::cout << "..after calling next level .. " << std::endl;
#endif
         }
      }  	
   }

   return -1;
}



template<typename Domain, typename SegmentT, typename CellT, typename EdgeT, typename TargetT, typename ValueT>
long find_nearest_match(Domain& domain, SegmentT& segment, CellT& start_cell, EdgeT& non_manifold_edge, TargetT& target, 
                        ValueT& recursion_max_depth)
{
   typedef typename gsse::domain_traits<Domain>::storage_type      storage_type;
   typedef typename gsse::domain_traits<Domain>::cell_iterator	   cell_iterator;

   std::cout << " ====================== " << std::endl;
   std::cout << " find nearest match.." << std::endl;
   std::cout << " ====================== " << std::endl;
   std::cout << std::endl;

   ValueT recursion_counter = 0;
   
   (*segment).add_cell_quantity("nearest_match");
   
   for (cell_iterator cit = (*segment).cell_begin(); cit != (*segment).cell_end(); cit++)
      domain.store_quantity((*cit), "nearest_match", storage_type(1,1,0.0));  // 0.0 -> not yet visited
      
   return(nearest_match_recursion(domain, start_cell, non_manifold_edge, target, recursion_counter, recursion_max_depth));
}


template<typename VertexCellIteratorT, typename CellT, typename EdgeOnCellIteratorT>
long get_base_vertex(CellT& cell, EdgeOnCellIteratorT& edge)
{
   long vertex;
//    point_t base_point;
   long base_point_handle;
   long pc = 0;

   VertexCellIteratorT vocit(*cell);                           
   std::cout << "cell vertices: ";
                        
   while (vocit.valid())
   {
      if((*vocit).handle() != (*edge).handle1() && (*vocit).handle() != (*edge).handle2())
      {
//          base_point = domain.get_point(*vocit);
         base_point_handle = (*vocit).handle();
         std::cout << ".. base point: " << (*vocit).handle() << std::endl;
         vertex = pc;
      }
      std::cout << (*vocit) << " ";
      vocit++;
      pc++;
   }
   std::cout << std::endl;

   return vertex;
}


template<typename SegmentIteratorT, typename CellIteratorT, typename CellHandleT>
bool compare_triangles(SegmentIteratorT& segit1, CellIteratorT handle1, SegmentIteratorT& segit2, CellHandleT& handle2)
{
#ifdef DEBUGCOMPARE
   std::cout << ".. first trig: " 
             << (*segit1).retrieve_topology().get_cell(handle1)[0] << " "
             << (*segit1).retrieve_topology().get_cell(handle1)[1] << " "
             << (*segit1).retrieve_topology().get_cell(handle1)[2] << " "
             << std::endl;
#endif

   std::vector<long> triangle1;
   
   triangle1.push_back((*segit1).retrieve_topology().get_cell(handle1)[0]);
   triangle1.push_back((*segit1).retrieve_topology().get_cell(handle1)[1]);
   triangle1.push_back((*segit1).retrieve_topology().get_cell(handle1)[2]);

   sort(triangle1.begin(), triangle1.end());                                                    

#ifdef DEBUGCOMPARE
   std::cout << ".. second trig: " 
             << (*segit2).retrieve_topology().get_cell(handle2)[0] << " "
             << (*segit2).retrieve_topology().get_cell(handle2)[1] << " "
             << (*segit2).retrieve_topology().get_cell(handle2)[2] << " "
             << std::endl;
#endif
       
   std::vector<long> triangle2;

   triangle2.push_back((*segit2).retrieve_topology().get_cell(handle2)[0]);
   triangle2.push_back((*segit2).retrieve_topology().get_cell(handle2)[1]);
   triangle2.push_back((*segit2).retrieve_topology().get_cell(handle2)[2]);
   
   sort(triangle2.begin(), triangle2.end());
   
   if(triangle1 == triangle2)
      return true;
   else
      return false;
}


template<typename DomainT, typename SegmentT, typename CellIterator>
bool check_orientation_recursive(DomainT& domain,  SegmentT& segment,  CellIterator& cit)
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

	  check_orientation_recursive(domain, segment, coeit);
	}
    }
}


template<typename Domain>
void check_and_repair_orientation_new(Domain& domain)
{
  typedef typename gsse::domain_traits<Domain>::cell_iterator		  cell_iterator;
  typedef typename gsse::domain_traits<Domain>::segment_iterator	  segment_iterator;
  typedef typename gsse::domain_traits<Domain>::storage_type             storage_type;
  typedef typename gsse::domain_traits<Domain>::vertex_on_cell_iterator  vertex_on_cell_iterator;

  std::cout << " ====================== " << std::endl;
  std::cout << " check  orientation OLD .." << std::endl;
  std::cout << " ====================== " << std::endl;
  std::cout << std::endl;

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
  // 	     << " " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1]
  // 	     << " " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[2] << std::endl;

  //    long temp =    (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[0];
  //    (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[0] = (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1];
  //    (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1] = temp;

  //    std::cout << "cell: " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[0] 
  // 	     << " " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[1]
  // 	     << " " << (*seg_iter).retrieve_topology().get_cell( (*cit).handle() )[2] << std::endl;

  for (seg_iter = domain.segment_begin(); seg_iter != domain.segment_end(); ++seg_iter)
    {
      cell_iterator cit = (*seg_iter).cell_begin(); 
      //        domain.store_quantity(*cit, "orientation", storage_type(1,1,1.0));  // 1.0 -> oriented consistenly

      check_orientation_recursive(domain, *seg_iter, cit);
    }	
}
