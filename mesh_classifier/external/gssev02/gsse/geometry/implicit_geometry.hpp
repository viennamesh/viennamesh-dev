/* ============================================================================
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_IMPLICIT_GEOMETRY_HH_ID
#define GSSE_IMPLICIT_GEOMETRY_HH_ID

// *** GSSE includes
//

namespace gsse
{

template <typename TopologyT,typename PointT>
class implicit_geometry
{
public:

   typedef implicit_geometry<TopologyT, PointT>                      self;
   typedef PointT                                                       point_t;
   typedef typename topology_traits<TopologyT>::vertex_handle           vertex_handle;           
   
   static long const dimension = topology_traits<TopologyT>::dimension_tag::dim;
   
private:
   typedef typename topology_traits<TopologyT>::vertex_type             vertex_type;
   typedef typename PointT::numeric_t                                   numeric_t;
   


public:
   implicit_geometry() {}

   implicit_geometry(long dimension)
   {
      dimension_ticks.resize(dimension, std::vector<numeric_t>()) ;
   }

   implicit_geometry(const std::vector<std::vector<numeric_t> >& list_dim )
   {
      dimension_ticks = list_dim;
   }
  
public:

   // insert one value afterwards, the container is resorted
   //
   void insert_dim_tick(const unsigned long& dim, const numeric_t& value)
   {
      if (dim <= 0)
         throw dimension_problem("Structured domain:: insert dim tick with dimension <= 0");

//       std::cout << "dim: " << dim <<   " value: " << value << std::endl;
      if (dim >= dimension_ticks.size())
          dimension_ticks.resize(dim);

      dimension_ticks[dim-1].push_back(value);
      std::sort (dimension_ticks[dim-1].begin(), dimension_ticks[dim-1].end());
      typename std::vector<numeric_t>::iterator new_end = 
         std::unique(dimension_ticks[dim-1].begin(), dimension_ticks[dim-1].end() ) ;
      dimension_ticks[dim-1].erase(new_end, dimension_ticks[dim-1].end());

//       std::cout << "dim: " << dim <<   " size: " <<dimension_ticks[dim-1].size()  << std::endl;
   }

   // insert more than one value, afterwards, the container is resorted
   //

   void insert_dim_ticks(const std::vector<numeric_t>& ticksnew, const unsigned long& dim)
   {
      std::copy (ticksnew.begin(), ticksnew.end(), std::back_inserter(dimension_ticks[dim-1]));
      std::sort (dimension_ticks[dim-1].begin(), dimension_ticks[dim-1].end());
   }
  
   long size()
   {
      long thesize(1);
      for (long i= 1; i <= dimension; ++i)
      {
         thesize*= get_dim_n(i);
//          std::cout << "dim: " << i << "   values: " << thesize << std::endl;
      }
      return thesize;
   }

   long get_dim_n(const long& dim)
   {
      return dimension_ticks[dim-1].size();
   }

   void print_dim_n(const long& dim) 
   { 
      std::copy (dimension_ticks[dim-1].begin(),dimension_ticks[dim-1].end(), 
                 std::ostream_iterator<numeric_t>(std::cout," ")); 
   }

   std::vector<numeric_t>& get_dim_n_coordinates(const long& dim)
   {
     return dimension_ticks[dim];
   }



   void scale_coord(std::vector<numeric_t> coord_scaler)
   {
      for (size_t index1= 0; index1 < dimension_ticks.size(); ++index1)
      {
         for (size_t index2= 0; index2 < dimension_ticks[index1].size(); ++index2)
         {
            dimension_ticks[index1][index2] *= coord_scaler[index1];
            
         }
      }
   }



  // access the generic point corresponding to a vertex
  //
  point_t operator[](const vertex_type& ve) const

  {     
    long countdim(1);
    point_t local_point;
    for (unsigned long i= 0; i < dimension_ticks.size(); i++, countdim++)
      {
	numeric_t coord = dimension_ticks[i][ve.topo_anchor().get_vertex_dim_n_index(ve, countdim )];
	local_point[i] = coord;
      }
    return local_point;
  }
  


private:
   std::vector<std::vector<numeric_t> > dimension_ticks;
};

}
#endif
