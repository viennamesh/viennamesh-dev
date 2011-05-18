/* ============================================================================
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_VORONOI_HH_ID
#define GSSE_VORONOI_HH_ID

// *** BOOST includes
#include <boost/array.hpp>

// *** GSSE includes
#include "gsse/math/power.hpp"
#include "gsse/math/determinant.hpp"
#include "gsse/geometry/metric_object.hpp" 
#include "gsse/geometry/metric_operations.hpp" 
#include "gsse/geometry/orient.hpp" 




// [RH][TODO] !!!!

// #define DEBUG_VORONOI
// #define DEBUGGING_LEVEL 1


namespace gsse
{
  
/*

  // structured voronoi information
   //
  template<typename DomainT>
  struct voronoi_calculation_cuboid
  {
     typedef typename domain_traits<DomainT>::segment_t               segment;
     typedef typename domain_traits<DomainT>::vertex_type             vertex;
     typedef typename domain_traits<DomainT>::edge_type               edge;
     typedef typename domain_traits<DomainT>::facet_type              facet;
     typedef typename domain_traits<DomainT>::cell_type               cell;
     
     typedef typename domain_traits<DomainT>::edge_on_cell_iterator   edge_on_cell_iterator;
     typedef typename domain_traits<DomainT>::vertex_on_cell_iterator vertex_on_cell_iterator;
     typedef typename domain_traits<DomainT>::cell_iterator           cell_iterator;
     typedef typename domain_traits<DomainT>::vertex_iterator         vertex_iterator;
     typedef typename domain_traits<DomainT>::segment_iterator        segment_iterator;
     
     typedef typename domain_traits<DomainT>::point_t                 point_t;
     typedef typename domain_traits<DomainT>::storage_type            storage_type;
     typedef typename domain_traits<DomainT>::dimension_tag           dimension_tag;



    void operator()(DomainT& domain)
    {
       // [RH][TODO] .. place this in the domain or somewhere else
       //

      // quantity accessors
      //
      std::string cell_volume_key("cell_volume");
      std::string edge_area_key("edge_area");
      std::string edge_len_key("edge_length");
      std::string vertex_volume_key("vertex_volume");      



//      using boost::phoenix::_1;
      using namespace boost::phoenix;
      using namespace boost::phoenix::local_names;

      BOOST_STATIC_ASSERT(( boost::is_same<typename domain_traits<DomainT>::topology_tag, structured_topology_tag>::value ));

      static const int dim = dimension_tag::dim;
      static const double dim_frac = double(power<2, dim>::value);
      static const double dim_frac_1 = double(power<2, dim-1>::value);
      
 
      typename domain_traits<DomainT>::segment_iterator seg_it;
    
      // coordinate accessors
      //
      typedef typename gsse::coord_access_t<DomainT>::type      coord_access_t;                     
      coord_access_t coord     = coord_access(domain);


      
      typedef typename gsse::scalar_quan_access_t<DomainT>::type                           quan_t;           
      typedef typename gsse::domain_traits<DomainT>::edge_on_vertex_iterator               edge_on_vertex;
      typedef typename gsse::domain_traits<DomainT>::vertex_on_edge_iterator               vertex_on_edge;
      typedef typename gsse::domain_traits<DomainT>::cell_on_edge_iterator                 cell_on_edge;
      typedef typename gsse::domain_traits<DomainT>::edge_on_cell_iterator                 edge_on_cell;
      
      
      quan_t   vol_vertex  = gsse::scalar_quan_access(domain,vertex_volume_key);     							
      quan_t   area        = gsse::scalar_quan_access(domain,edge_area_key);       
      quan_t   dist        = gsse::scalar_quan_access(domain,edge_len_key);    	         
      quan_t   vol_cell    = gsse::scalar_quan_access(domain,cell_volume_key);     							
      
      for(seg_it = domain.segment_begin(); seg_it != domain.segment_end(); ++seg_it)
      {
         domain.init_vertex_quantity(*seg_it, vertex_volume_key, storage_type(1, 1, 0.0));
         domain.init_edge_quantity  (*seg_it, edge_len_key,      storage_type(1, 1, 1.0));    // FV uses an A / d .. set d -> 1.0
         domain.init_edge_quantity  (*seg_it, edge_area_key,     storage_type(1, 1, 0.0));
         domain.init_cell_quantity  (*seg_it, cell_volume_key,   storage_type(1, 1, 0.0));
      }

//[RH][TODO] rewrite the next statements to  GSSE::FP
//
      for(seg_it = domain.segment_begin(); seg_it != domain.segment_end(); ++seg_it)
      {
           point_t range;
                      
           for (cell_iterator cit = (*seg_it).cell_begin(); cit.valid(); ++cit)
           {
              // calculate cell volume
              //
  	      boost::array<long,dim> indices = (*cit).topo_anchor().get_cell_index(*cit);
              
              vertex lower = (*cit).topo_anchor().get_index_vertex(indices);                   // calculate the vertex with biggest and smallest index
              std::for_each(indices.begin(), indices.end(), ++ boost::phoenix::arg_names::_1);
              vertex up = (*cit).topo_anchor().get_index_vertex(indices);
              range = domain.get_point(lower) - domain.get_point(up);                          // get the range
              
              double value_cell_volume = 1.0;                                                  // calculate the volume of the cell
              std::for_each(range.begin(), range.end(), ref(value_cell_volume) *= boost::phoenix::arg_names::_1);
              
              value_cell_volume = std::abs(value_cell_volume);
              
              cell cl(*cit);
              ( vol_cell = value_cell_volume )(cl);
              traverse<vertex>()   [  vol_vertex += value_cell_volume / val(dim_frac)   ](cl);

           }
          
      }

      traverse<segment>()
      [
          traverse<edge>()   
          [  
             dist = Norm (sum<vertex>(point_t(), _e)   [  coord  *  orient(boost::phoenix::arg_names::_1,_e) ] ),
             area = sum<cell>(0.0, _e)
             [
                vol_cell   /   val(dim_frac_1)
             ] / dist
          ]
      ] (domain);

    }
  };
   



// ##########################################################################################
//
// ======================================== voronoi 2D ======================================
//
// ##########################################################################################

template <typename DomainT>
int calculate_voronoi_2d(DomainT & domain)
{
   typedef typename gsse::domain_traits<DomainT>::segment_t               segment;
   typedef typename gsse::domain_traits<DomainT>::vertex_type             vertex;
   typedef typename gsse::domain_traits<DomainT>::edge_type               edge;
   typedef typename gsse::domain_traits<DomainT>::facet_type              facet;
   typedef typename gsse::domain_traits<DomainT>::cell_type               cell;
   
   typedef typename gsse::domain_traits<DomainT>::vertex_on_cell_iterator  vertex_on_cell_iterator_t;
   typedef typename gsse::domain_traits<DomainT>::edge_on_cell_iterator    edge_on_cell_iterator_t;
   typedef typename gsse::domain_traits<DomainT>::vertex_iterator          vertex_iterator_t;
   typedef typename gsse::domain_traits<DomainT>::edge_iterator            edge_iterator_t;
   typedef typename gsse::domain_traits<DomainT>::point_t                  point2d_t;
   typedef typename gsse::domain_traits<DomainT>::vertex_type              vertex_t;
   typedef typename gsse::domain_traits<DomainT>::edge_type                edge_type;
   typedef typename gsse::domain_traits<DomainT>::storage_type             storage_type;
   typename gsse::domain_traits<DomainT>::segment_iterator                 seg_it;

   typedef double numeric_t;

   // for pseudo 3d calculation of the ex product
   //
   typedef gsse::generic_point<numeric_t, 3> point3d_t;

   static const long dimension = DomainT::dimension_geometry;

   std::string edge_area_key("edge_area");
   std::string edge_len_key("edge_length");
   std::string vertex_volume_key("vertex_volume");      
   

   for (seg_it = domain.segment_begin(); seg_it != domain.segment_end(); ++seg_it)
   {
      domain.init_vertex_quantity(*seg_it, vertex_volume_key, storage_type(1, 1, 0.0));
      domain.init_edge_quantity  (*seg_it, edge_len_key,      storage_type(1, 1, 1.0));    // FV uses an A / d .. set d -> 1.0
      domain.init_edge_quantity  (*seg_it, edge_area_key,     storage_type(1, 1, 0.0));
   }

//       // ===============================================
//       //
//       // initialize voronoi quantities
//       //
//       (*seg_it).add_vertex_quantity(vertex_volume_key);

//       for (vertex_iterator_t v_it = (*seg_it).vertex_begin();  v_it != (*seg_it).vertex_end(); ++v_it)
//       {
//          storage_type  dpt(1,1,0.0);
//          domain.store_quantity(*v_it, vertex_volume_key, dpt);
//       }
//       (*seg_it).add_edge_quantity(edge_len_key);
//       (*seg_it).add_edge_quantity(edge_area_key);
//       for (typename gsse::domain_traits<DomainT>::cell_iterator c_it = (*seg_it).cell_begin(); c_it != (*seg_it).cell_end(); ++c_it)
//       {
//          for (edge_on_cell_iterator_t eoc_it(*c_it); eoc_it.valid(); ++eoc_it)
//          {
// //             std::cout << "edgeit: " << *eoc_it << std::endl;

//             domain.store_quantity(*eoc_it, edge_len_key,  storage_type(1, 1, 0.0));
//             domain.store_quantity(*eoc_it, edge_area_key, storage_type(1, 1, 0.0));
//          }
//       }
//    }

   
   for (seg_it = domain.segment_begin(); seg_it != domain.segment_end(); ++seg_it)
   {
      // ===============================================
      //
      // calculate voronoi couplings and assign the values to the corresponding topological object
      //
      for (typename gsse::domain_traits<DomainT>::cell_iterator c_it = (*seg_it).cell_begin(); c_it != (*seg_it).cell_end(); ++c_it)
      {
#ifdef DEBUG_VORONOI
         std::cout << "### next cell:" << *c_it << std::endl;
#endif

	 // transfer gsse objects/information into arrays
	 //
	 boost::array<point3d_t,  dimension+1>   geompoints;
	 boost::array<vertex_t, dimension+1>     topopoints;


	 long index = 0;
	 for (vertex_on_cell_iterator_t voc_it(*c_it); voc_it.valid(); ++voc_it, ++index)
	 {
		 point2d_t point_temp  = domain.get_point( *voc_it );
		 geompoints[index]     = point3d_t(point_temp[0], point_temp[1], 0.0);
		 topopoints[index]     = (*voc_it);
	 }

	 point3d_t point_cc = gsse::circumcenter_triangle2D(geompoints[0], geompoints[1], geompoints[2]);
	 point3d_t point_bc = gsse::barycenter(geompoints.begin(), geompoints.end());

#ifdef DEBUG_VORONOI
	 std::cout << " ## circumcenter: " << point_cc << std::endl;
	 std::cout << "p0 g: " << geompoints[0] << "\ttopo: " << topopoints[0] << std::endl;
	 std::cout << "p1 g: " << geompoints[1] << "\ttopo: " << topopoints[1] << std::endl;
	 std::cout << "p2 g: " << geompoints[2] << "\ttopo: " << topopoints[2] << std::endl;
#endif

	 // -----------------------------
	 //
	 // transfer 2d simplex into local vector system
	 //
	 point_cc -= geompoints[0];
	 point_bc -= geompoints[0];
	 point3d_t  vec01 = geompoints[1] - geompoints[0];
	 point3d_t  vec02 = geompoints[2] - geompoints[0];
	 point3d_t  vec12 = geompoints[2] - geompoints[1];


#ifdef DEBUG_VORONOI
    // [JW] deactivated due to compiler error ..
	 //short or = orient2D_triangle(geompoints[0], geompoints[1], geompoints[2],  point_cc);
	 //std::cout << "circumcenter inside (0: inside, 1: on boundary, 2: outside): " << or << std::endl;
	 std::cout << "## bc : " << point_bc << std::endl;
	 std::cout << "## cc : " << point_cc << std::endl;
	 std::cout << "## vec01: " << vec01 << std::endl;
	 std::cout << "## vec02: " << vec02 << std::endl;
	 std::cout << "## vec12: " << vec12 << std::endl;
#endif

	 // create topological local (temporary) edge types for quantity storage
	 //
	 edge_type e1( (*seg_it).retrieve_topology(), topopoints[0].handle(),topopoints[1].handle());
	 edge_type e2( (*seg_it).retrieve_topology(), topopoints[0].handle(),topopoints[2].handle());
	 edge_type e3( (*seg_it).retrieve_topology(), topopoints[1].handle(),topopoints[2].handle());

	//
	// ##### actual voronoi calculation  #######
	//
	 // calculate the necessary vectors to the CC
	 //
	 point3d_t  edgearea_01 = point_cc  - vec01/2;	
	 point3d_t  edgearea_02 = point_cc  - vec02/2;	
	 point3d_t  edgearea_12 = vec12 / 2 + geompoints[1] - geompoints[0] - point_cc;

	 // calculate the necessary vectors to the BC
	 //
	 point3d_t  p2bc_01 = point_bc  - vec01/2;	
	 point3d_t  p2bc_02 = point_bc  - vec02/2;	
	 point3d_t  p2bc_12 = vec12 / 2 + geompoints[1] - geompoints[0]  - point_bc;

	 // check the signs for each of the calculated vectors to the CC
	 //
	 double sign01 = 1.0;
	 double sign02 = 1.0;
	 double sign12 = 1.0;
	 if (gsse::in(edgearea_01, p2bc_01) < 0.0)
		 sign01 *= -1;
	 if (gsse::in(edgearea_02, p2bc_02) < 0.0)
		 sign02 *= -1;
	 if (gsse::in(edgearea_12, p2bc_12) < 0.0)
		 sign12 *= -1;

	 double e1_length = gsse::length(vec01);
	 double e2_length = gsse::length(vec02);
	 double e3_length = gsse::length(vec12);

	 double e1_area = sign01 * gsse::length(edgearea_01);
	 double e2_area = sign02 * gsse::length(edgearea_02);
	 double e3_area = sign12 * gsse::length(edgearea_12);

	 //   GSSE quantity assignments
	 //
	 (domain)(e1, edge_len_key  )(0,0) = e1_length;
	 (domain)(e2, edge_len_key  )(0,0) = e2_length;
	 (domain)(e3, edge_len_key  )(0,0) = e3_length;

	 (domain)(e1, edge_area_key )(0,0) += e1_area;
	 (domain)(e2, edge_area_key )(0,0) += e2_area;
	 (domain)(e3, edge_area_key )(0,0) += e3_area;

	 (domain)(topopoints[0],vertex_volume_key )(0,0) += e1_area * e1_length / 4.;
	 (domain)(topopoints[1],vertex_volume_key )(0,0) += e1_area * e1_length / 4.;

	 (domain)(topopoints[0],vertex_volume_key )(0,0) += e2_area * e2_length / 4.;
	 (domain)(topopoints[2],vertex_volume_key )(0,0) += e2_area * e2_length / 4.;

	 (domain)(topopoints[1],vertex_volume_key )(0,0) += e3_area * e3_length / 4.;
	 (domain)(topopoints[2],vertex_volume_key )(0,0) += e3_area * e3_length / 4.;

#ifdef DEBUG_VORONOI
	 std::cout << "####### local area 01: " << e1_area << std::endl;
	 std::cout << "####### local area 02: " << e2_area << std::endl;
	 std::cout << "####### local area 12: " << e3_area << std::endl;
	 std::cout << "## cc : " << point_cc << std::endl;
	 std::cout << "## e01: "<< edgearea_01 << std::endl;
	 std::cout << "## e02: "<< edgearea_02 << std::endl;
	 std::cout << "## e12: "<< edgearea_12 << std::endl;
	 std::cout << " ### e12 .. vec12/2: " << vec12/2 << "    vec12 / 2 + p1: "<< vec12/2 +geompoints[1] - geompoints[0]<< std::endl;



         std::cout << "  ## E1: " << domain(e1, edge_area_key )(0,0) << std::endl;
         std::cout << "  ## E2: " << domain(e2, edge_area_key )(0,0) << std::endl;
         std::cout << "  ## E3: " << domain(e3, edge_area_key )(0,0) << std::endl;
         std::cout << "  #### e1: " << e1 << std::endl;
         std::cout << "  #### e2: " << e2 << std::endl;
         std::cout << "  #### e3: " << e3 << std::endl;


	 std::cout << "   volu p0: " <<  domain(topopoints[0],vertex_volume_key )(0,0)  << std::endl;
	 std::cout << "   volu p1: " <<  domain(topopoints[1],vertex_volume_key )(0,0)  << std::endl;
	 std::cout << "   volu p2: " <<  domain(topopoints[2],vertex_volume_key )(0,0)  << std::endl;
#endif   
      }
   }

   typedef typename gsse::scalar_quan_access_t<DomainT>::type                           quan_t;           
   typedef typename gsse::domain_traits<DomainT>::edge_on_vertex_iterator               edge_on_vertex;

   quan_t   vol         = gsse::scalar_quan_access(domain,vertex_volume_key);     							
   quan_t   area        = gsse::scalar_quan_access(domain,edge_area_key);       
   quan_t   dist        = gsse::scalar_quan_access(domain,edge_len_key);    	         


      traverse<segment>()
      [
          traverse<edge>()   
          [
           (   if_(dist < 0.0)
              [
                 dist *= -1.0
              ],
              if_(dist == 0.0)
              [
                 dist = 1.0
              ],
              if_(area < 0.0)
              [
                 area *= -1.0
              ],
              if_(area < 1e-10)
              [
                 area = 0.0
              ])
          ]
      ] (domain);



  return 0;
}




// ##########################################################################################
//
// ======================================== voronoi 3D ======================================
//
// ##########################################################################################


// [RH][TODO] [P10] ..rewrite this voronoi as well 
//

struct coupling_c
{
	double areadivdistance;
	double distance;

	coupling_c() : areadivdistance(0.0), distance(0.0) {}
	coupling_c &operator+=(const coupling_c &c) 
	{ 
		areadivdistance+=c.areadivdistance; 
		distance=c.distance;
		return *this;
	}
};


// solve simple linear equation system
//      l * vec(a)  + m * vec(b) = vec(c), where the solution is in one plane
//
template<typename GenericVector>
void solve_leq(double &l, const GenericVector &a,  
               double &m, const GenericVector &b, 
                          const GenericVector &c)
{
	GenericVector axb=gsse::ex(a,b);

	// solve l*a+m*b+n*(axb)=c, 
        //   n should be zero
        //
	double det,detl,detm;
	det  = gsse::square_length(axb);     	                // det  = a*(b x (axb))
	detl = gsse::in(axb, gsse::ex(c,b));    		// detl = c*(b x (axb))
	detm = gsse::in(axb, gsse::ex(a,c));    		// detm = a*(c x (axb))

	l=detl/det;
	m=detm/det;
}


// generate the coupling of the part of the vectors
// points[0] in direction points[1]
//
template<typename GenericVector, long DIM>
void compute_couplings(const GenericVector v[DIM], 
                       const GenericVector mid, 
                       coupling_c &coupling)
{
	double l,m=1;
        
	const GenericVector axb = gsse::ex(v[0],v[1]);     
	const GenericVector an  = gsse::ex(axb, v[0]);     // vector normal to a, in plane axb

//         std::cout << "## axb: " << axb << std::endl;
//         std::cout << "## an:  " << an << std::endl;

//  [RH][TODO] .. Versuch, anstatt 
// 	a/2 + l an + m axb = mid
// zu lsen, 3 Gleichungen, 2 unbekannte l,m
// 	n a + l an + m axb = mid
// 3 Gleichungen, 3 Unbekannte, n sollte mit 0.5 herauskommen
// 


	solve_leq(l,an,  m,axb,
                  mid-v[0]/2.);   // how to come from a/2 to mid

        double local_length;
        // -----
        //
	// l has the correct sign
	// {A/d} = l |(axb) x a|/|a| = l |axb| |a|/|a|
        //
        // -----
        if (DIM == 1)
        {
           local_length = 1. / gsse::length(v[0]);
        }
        else if (DIM == 2)
        {
           local_length = l * gsse::length(axb) / 2.;
        }
        else if (DIM ==3)
        {
           if (gsse::in(axb,v[2] )< 0.)        	  // axb and c in different directions, 
              m=-m;                               // correct sign of m
           local_length = l * m * gsse::square_length(axb) / 2.;
        }

	coupling.areadivdistance = local_length;
        coupling.distance        = gsse::length(v[0]);
}




// Calculate the couplings around point[0]
//
template<typename GenericVector>
void compute_voronoi_simplex_3d(const boost::array<GenericVector, 4>& points, 
                                const GenericVector &mid, 
                                boost::array<coupling_c, 3>&  couplings)
{
//    std::string edge_area_key("edge_area");
//    std::string edge_len_key("edge_length");
//    std::string vertex_volume_key("vertex_volume");      


   GenericVector v[3],mid0,dummy;


   // transfer to p0 coordinate system
   //
   for (int i=1; i<4; i++)
      v[i-1]=points[i]-points[0];
   mid0=mid-points[0];
   

   // calculate coupling of p0 --> p1
   //
   compute_couplings<GenericVector, 3>(v,mid0,couplings[0]);
   

   // calculate coupling of p0 --> p2
   //
   dummy=v[0]; 
   v[0]=v[1]; 
   v[1]=dummy;
   compute_couplings<GenericVector, 3>(v,mid0,couplings[1]);
   

   // calculate coupling of p0 --> p3
   //
   dummy=v[0]; 
   v[0]=v[1]; 
   v[1]=v[2]; 
   v[2]=dummy;
   compute_couplings<GenericVector, 3>(v,mid0,couplings[2]);


}


// [RH][TODO][P10] .. remove the domain.. it is only required due to the data type extraction
//
template <typename DomainT, typename SegmentIterator, typename TopoPointType, typename GenericVector>
int permutate_vertices_3d(DomainT& domain,
                          SegmentIterator& seg_it, 
                          const boost::array<GenericVector, 4>& geompoints, 
                          const boost::array<TopoPointType, 4>& topopoints, 
                          const GenericVector&  point_cc )
{
   typedef typename gsse::domain_traits<DomainT>::edge_type                edge_type;
   typedef typename gsse::domain_traits<DomainT>::storage_type        storage_type;

   boost::array<coupling_c, 3> coupling;


   std::string edge_area_key("edge_area");
   std::string edge_len_key("edge_length");
   std::string vertex_volume_key("vertex_volume");      
   
   {
       compute_voronoi_simplex_3d(geompoints, point_cc, coupling);
      
      // -----------------------------
      //
      // p0 --> p1
      //
      domain(topopoints[0],vertex_volume_key )(0,0) += coupling[0].areadivdistance * coupling[0].distance*coupling[0].distance / 6.;
      domain(topopoints[1],vertex_volume_key )(0,0) += coupling[0].areadivdistance * coupling[0].distance*coupling[0].distance / 6.;
      
      
      // -----------------------------
      //
      // p0 --> p2
      //
      domain(topopoints[0],vertex_volume_key )(0,0) += coupling[1].areadivdistance * coupling[1].distance*coupling[1].distance / 6.;
      domain(topopoints[2],vertex_volume_key )(0,0) += coupling[1].areadivdistance * coupling[1].distance*coupling[1].distance / 6.;
      
      
      // -----------------------------
      //
      // p0 --> p3
      //
      domain(topopoints[0],vertex_volume_key )(0,0) += coupling[2].areadivdistance * coupling[2].distance*coupling[2].distance / 6.;
      domain(topopoints[3],vertex_volume_key )(0,0) += coupling[2].areadivdistance * coupling[2].distance*coupling[2].distance / 6.;
      
      edge_type e1( (*seg_it).retrieve_topology(), topopoints[0].handle(),topopoints[1].handle());
      edge_type e2( (*seg_it).retrieve_topology(), topopoints[0].handle(),topopoints[2].handle());
      edge_type e3( (*seg_it).retrieve_topology(), topopoints[0].handle(),topopoints[3].handle());
      
      domain.store_quantity(e1, edge_area_key, storage_type (1,1, coupling[0].areadivdistance));
      domain.store_quantity(e1, edge_len_key,  storage_type (1,1, coupling[0].distance));
      domain.store_quantity(e2, edge_area_key, storage_type (1,1, coupling[1].areadivdistance));
      domain.store_quantity(e2, edge_len_key,  storage_type (1,1, coupling[1].distance));
      domain.store_quantity(e3, edge_area_key, storage_type (1,1, coupling[2].areadivdistance));
      domain.store_quantity(e3, edge_len_key,  storage_type (1,1, coupling[2].distance));

#ifdef DEBUG_VORONOI
      std::cout << "..p0: " << topopoints[0] << std::endl;
      std::cout << "..p1: " << topopoints[1] << std::endl;
      std::cout << "..p2: " << topopoints[2] << std::endl;
      std::cout << "..p3: " << topopoints[3] << std::endl;

       std::cout << "## coupling 1 a: " << coupling[0].areadivdistance << std::endl;
       std::cout << "## coupling 1 d: " << coupling[0].distance << std::endl;
       std::cout << "## coupling 2 a: " << coupling[1].areadivdistance << std::endl;
       std::cout << "## coupling 2 d: " << coupling[1].distance << std::endl;
       std::cout << "## coupling 3 a: " << coupling[2].areadivdistance << std::endl;
       std::cout << "## coupling 3 d: " << coupling[2].distance << std::endl;
         std::cout << std::endl;
#endif

   }
  return 0;
}






template <typename DomainT>
int calculate_voronoi_3d(DomainT & domain)
{
   typedef typename gsse::domain_traits<DomainT>::vertex_on_cell_iterator  vertex_on_cell_iterator_t;
   typedef typename gsse::domain_traits<DomainT>::edge_on_cell_iterator    edge_on_cell_iterator_t;
   typedef typename gsse::domain_traits<DomainT>::vertex_iterator          vertex_iterator_t;
   typedef typename gsse::domain_traits<DomainT>::edge_iterator            edge_iterator_t;
   typedef typename gsse::domain_traits<DomainT>::point_t                  point_t;
   typedef typename gsse::domain_traits<DomainT>::vertex_type              vertex_t;
   typedef typename gsse::domain_traits<DomainT>::edge_type                edge_type;
   typedef typename gsse::domain_traits<DomainT>::storage_type             storage_type;

   typedef typename gsse::domain_traits<DomainT>::segment_type             segment;
   typedef typename gsse::domain_traits<DomainT>::vertex_type              vertex;
   typedef typename gsse::domain_traits<DomainT>::edge_type                edge;
   typedef typename gsse::domain_traits<DomainT>::facet_type               facet;
   typedef typename gsse::domain_traits<DomainT>::cell_type                cell;

   typename gsse::domain_traits<DomainT>::segment_iterator                 seg_it;

   static const long dimension = DomainT::dimension_geometry;

   std::string edge_area_key("edge_area");
   std::string edge_len_key("edge_length");
   std::string vertex_volume_key("vertex_volume");      
   
#ifdef DEBUG_VORONOI
   std::cout << "GSSE::VORONOI_3D::initializing voronoi quantities .." << std::endl; 
#endif
   for (seg_it = domain.segment_begin(); seg_it != domain.segment_end(); ++seg_it)
   {
	(*seg_it).add_vertex_quantity(vertex_volume_key);
	(*seg_it).add_edge_quantity(edge_len_key);
	(*seg_it).add_edge_quantity(edge_area_key);
      // ===============================================
      //
      // initialize voronoi quantities
      //
      for (vertex_iterator_t v_it = (*seg_it).vertex_begin();
           v_it != (*seg_it).vertex_end(); ++v_it)
      {
         storage_type  dpt(1,1,0.0);

         domain.store_quantity(*v_it, vertex_volume_key, dpt);
      }
      for (typename gsse::domain_traits<DomainT>::cell_iterator c_it = (*seg_it).cell_begin();
           c_it != (*seg_it).cell_end(); ++c_it)
      {
         for (edge_on_cell_iterator_t eoc_it(*c_it); eoc_it.valid(); ++eoc_it)
         {
            domain.store_quantity(*eoc_it, edge_len_key,  storage_type(1, 1, 0.0));
            domain.store_quantity(*eoc_it, edge_area_key, storage_type(1, 1, 0.0));
         }
      }
   }
   
#ifdef DEBUG_VORONOI
   std::cout << "GSSE::VORONOI_3D::start voronoi computation .." << std::endl; 
#endif
   for (seg_it = domain.segment_begin(); seg_it != domain.segment_end(); ++seg_it)
   {
#ifdef OUTPUT
      std::cout << "\tGSSE::VORONOI_3D::start of seg: " << (*seg_it).get_name()  << std::endl; 
#endif
      // ===============================================
      //
      // calculate voronoi couplings and assign the values to the corresponding topological object
      //
	long counter=0;	
      for (typename gsse::domain_traits<DomainT>::cell_iterator c_it = (*seg_it).cell_begin();
           c_it != (*seg_it).cell_end(); ++c_it, ++counter)
      {
         boost::array<point_t,  dimension+1>   geompoints, geompoints_real;
         boost::array<vertex_t, dimension+1>   topopoints, topopoints_real;
#ifdef OUTPUT
	if (counter % 1000 == 0)
		std::cout << " ## gsse voronoi:: caculated percentage: " << 1.0 * counter / (*seg_it).cell_size() * 100. << std::endl;
#endif
 	long index = 0;
         for (vertex_on_cell_iterator_t voc_it(*c_it); voc_it.valid(); ++voc_it, ++index)
         {
            geompoints[index] = domain.get_point( *voc_it);
            topopoints[index] = (*voc_it);
         }
         point_t point_cc = circumcenter_simplex(geompoints[0], geompoints[1], geompoints[2], geompoints[3]);

#ifdef DEBUG_VORONOI
         std::cout << "p0 g: " << geompoints[0] << std::endl;
         std::cout << "p0 y: " << topopoints[0] << std::endl;
         std::cout << "p1 g: " << geompoints[1] << std::endl;
         std::cout << "p1 y: " << topopoints[1] << std::endl;
         std::cout << "p2 g: " << geompoints[2] << std::endl;
         std::cout << "p2 y: " << topopoints[2] << std::endl;
         std::cout << "p3 g: " << geompoints[3] << std::endl;
         std::cout << "p3 y: " << topopoints[3] << std::endl;
#endif

         geompoints_real[0] = geompoints[0];
         geompoints_real[1] = geompoints[1];
         geompoints_real[2] = geompoints[2];
         geompoints_real[3] = geompoints[3];

         topopoints_real[0] = topopoints[0];
         topopoints_real[1] = topopoints[1];
         topopoints_real[2] = topopoints[2];
         topopoints_real[3] = topopoints[3];
          permutate_vertices_3d(domain, seg_it, geompoints_real,  topopoints_real, point_cc);


         geompoints_real[0] = geompoints[1];
         geompoints_real[1] = geompoints[0];
         geompoints_real[2] = geompoints[3];
         geompoints_real[3] = geompoints[2];

         topopoints_real[0] = topopoints[1];
         topopoints_real[1] = topopoints[0];
         topopoints_real[2] = topopoints[3];
         topopoints_real[3] = topopoints[2];
          permutate_vertices_3d(domain, seg_it, geompoints_real,  topopoints_real, point_cc);

         geompoints_real[0] = geompoints[2];
         geompoints_real[1] = geompoints[0];
         geompoints_real[2] = geompoints[1];
         geompoints_real[3] = geompoints[3];

         topopoints_real[0] = topopoints[2];
         topopoints_real[1] = topopoints[0];
         topopoints_real[2] = topopoints[1];
         topopoints_real[3] = topopoints[3];
          permutate_vertices_3d(domain, seg_it, geompoints_real,  topopoints_real, point_cc);


         geompoints_real[0] = geompoints[3];
         geompoints_real[1] = geompoints[0];
         geompoints_real[2] = geompoints[2];
         geompoints_real[3] = geompoints[1];

         topopoints_real[0] = topopoints[3];
         topopoints_real[1] = topopoints[0];
         topopoints_real[2] = topopoints[2];
         topopoints_real[3] = topopoints[1];
          permutate_vertices_3d(domain, seg_it, geompoints_real,  topopoints_real, point_cc);
      }
#ifdef DEBUG_VORONOI
      std::cout << "\tGSSE::VORONOI_3D::end of seg: " << (*seg_it).get_name()  << std::endl; 
#endif
   }


   typedef typename gsse::scalar_quan_access_t<DomainT>::type                           quan_t;           
   typedef typename gsse::domain_traits<DomainT>::edge_on_vertex_iterator               edge_on_vertex;
   typedef typename gsse::domain_traits<DomainT>::edge_on_cell_iterator                 edge_on_cell;

   quan_t   vol         = gsse::scalar_quan_access(domain,vertex_volume_key);     							
   quan_t   area        = gsse::scalar_quan_access(domain,edge_area_key);       
   quan_t   dist        = gsse::scalar_quan_access(domain,edge_len_key);    	         


    traverse<segment>()
    [
        traverse<vertex>()
        [
            if_(vol < 0.0) [ vol *= -1.0 ]
        ],
    	traverse<cell>()
	[
            traverse<edge>()
            [
              if_(dist < 0.0)    [ dist *= -1.0 ],
              if_(dist == 0.0)   [ dist = 1.0   ],
              if_(area < 0.0)    [ area *= -1.0 ],
              if_(area == 0.0)   [ area = 1.0   ]  
            ] 
	]
     ](domain);


  return 0;
}




// =============================================================================
//
// function object wrapper for automatic voronoi deducation within GSSE::domain
//


  template<typename DomainT>
  struct voronoi_calculation_simplex_2d
  {
    void operator()(DomainT& domain)
    {
       calculate_voronoi_2d<DomainT>(domain);
    }

  };

  template<typename DomainT>
  struct voronoi_calculation_simplex_3d
  {
    void operator()(DomainT& domain)
    {
       calculate_voronoi_3d<DomainT>(domain);
    }

  };


*/
   
} // *** end of namespace gsse  


#undef DEBUGGING_LEVEL
#endif 
