



template <typename DatastructureT>
mesh_kernel<viennamesh::tag::tetgen>::result_type 
mesh_kernel<viennamesh::tag::tetgen>::operator()(DatastructureT& data) // default meshing
{
   return (*this)(data, boost::fusion::make_map<tag::criteria>(tag::constrained_delaunay()));
}   

template<typename DatastructureT, typename ParametersMapT>
mesh_kernel<viennamesh::tag::tetgen>::result_type 
mesh_kernel<viennamesh::tag::tetgen>::operator()(DatastructureT& data, ParametersMapT const& paras )
{
   // redirect to reference implementation 
   ParametersMapT paras_new(paras);
   return (*this)(data, paras_new);
}

template<typename DatastructureT, typename ParametersMapT>
mesh_kernel<viennamesh::tag::tetgen>::result_type 
mesh_kernel<viennamesh::tag::tetgen>::operator()(DatastructureT& data, ParametersMapT & paras )
{
   this->init();      

   typedef typename DatastructureT::segment_iterator  vmesh_segment_iterator;
   typedef typename DatastructureT::cell_type         vmesh_cell_type;
   typedef typename DatastructureT::cell_iterator     vmesh_cell_iterator;   

   //this->setOptions(paras);


   size_t segment_size = data.segment_size();
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing segments" << std::endl;
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - detected segments: " << segment_size << std::endl;
#endif  
   if( segment_size > 1 )
   {
     typedef std::vector<point_type>    region_points_type;
     region_points_type region_points;
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - dealing with multi-segment input" << std::endl;
   #endif      
      std::size_t seg_cnt = 0;

      for(vmesh_segment_iterator seg_iter = data.segment_begin();
         seg_iter != data.segment_end(); seg_iter++)
      {
         point_type pnt;
         typename DatastructureT::cell_complex_wrapper_type segment = *seg_iter;
         this->find_point_in_segment(data, segment, pnt, seg_cnt);
         
         region_points.push_back(pnt);
      #ifdef MESH_KERNEL_DEBUG
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - computed point in segment " 
            << seg_cnt << " : " << pnt[0] << " " << pnt[1] << " " << pnt[2] << std::endl;
      #endif
         seg_cnt++;
      }
      for(typename region_points_type::iterator iter = region_points.begin();
          iter != region_points.end(); iter++)
      {
         this->addRegion(*iter);
      }
   }
#ifdef MESH_KERNEL_DEBUG
   else
   {
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - dealing with single-segment input" << std::endl;
   }   
#endif              



   // traverse and add the geometry information
   // aka the set of points
   //
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing geometry" << std::endl;
#endif
   typedef typename DatastructureT::geometry_iterator geometry_iterator;
   for(geometry_iterator iter = data.geometry_begin();
       iter != data.geometry_end(); iter++)
   {
   #ifdef MESH_KERNEL_DEBUG_FULL
       std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding point " << 
          std::distance(data.geometry_begin(), iter) << " : " << *iter << std::endl;
   #endif   
      this->addPoint(*iter);
   }
   
   // traverse and add the topology information
   // aka the set of boundary constraints
   //
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing constraints" << std::endl;
#endif
   //size_t si = 0;

   typedef std::vector<std::size_t>           cell_copy_type;
   std::map<cell_copy_type, bool>        cell_uniquer;      
#ifdef MESH_KERNEL_DEBUG_FULL
   size_t cell_cnt = 0;
#endif
   for(vmesh_segment_iterator seg_iter = data.segment_begin();
      seg_iter != data.segment_end(); seg_iter++)
   {  
      for(vmesh_cell_iterator cit = (*seg_iter).cell_begin();
          cit != (*seg_iter).cell_end(); cit++)
      {  
         vmesh_cell_type cell = *cit;

         // make sure a cell hasn't been already added ..
         // therefore we copy the original cell, and sort it ascendingly
         // due to the ascending order, we can compare it with other cells
         cell_copy_type cell_copy(cell.size());

         for(size_t i = 0; i < cell.size(); i++)  
            cell_copy[i] = cell[i];

         std::sort(cell_copy.begin(), cell_copy.end());
         
         if(!cell_uniquer[cell_copy])
         {
         #ifdef MESH_KERNEL_DEBUG_FULL
            std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding constraint   " << 
               cell_cnt << " : ";
            for(size_t i = 0; i < cell.size(); i++)  
               std::cout << cell[i] << " ";
            std::cout << std::endl;
            cell_cnt++;
         #endif               
            cell_uniquer[cell_copy] = true;
            
            this->addConstraint(cell);
         }
      #ifdef MESH_KERNEL_DEBUG_FULL
         else
         { 
            std::cout << "## MeshKernel::"+mesh_kernel_id+" - skipping constraint " << 
               cell_cnt << " : ";
            for(size_t i = 0; i < cell.size(); i++)  
               std::cout << cell[i] << " ";
            std::cout << std::endl;
         }
      #endif

      }
   }            

   domain_ptr_type domain(new domain_type);               

   this->do_meshing(domain);

   this->clear();   
   
   return domain;

}
