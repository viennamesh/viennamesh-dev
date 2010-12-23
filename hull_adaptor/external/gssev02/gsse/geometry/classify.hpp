/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at     
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at          
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at                     

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GSSE_ALGORITHMS_CLASSIFY
#define GSSE_ALGORITHMS_CLASSIFY
//
// ==================================================================
//
// *** GSSE includes
//
#include "gsse/util.hpp"
#include "gsse/geometry/functors/cell_quality.hpp"
#include "gsse/geometry/functors/metric_quantity.hpp"
#include "latex_histogram.hpp"
#include "detail/classify_3d.hpp"
#include "detail/keys.hpp"
//
// *** GSSE includes
//
#include <gsse/datastructure/domain.hpp>
#include <gsse/algorithm/io.hpp>
//
// *** SYSTEM includes
//
#include <set>
//
// ==================================================================
//
//#define DEBUG_CLASSIFY
//#define DEBUG_CLASSIFY_FULL
//
// ==================================================================
//
namespace gsse {
//
// ==================================================================
//
namespace detail {
//
// ==================================================================
//
template < int DIM >
struct classify { };
// ------------------------------------------------------------------
template < >
struct classify < 2 >
{
   template < typename Domain >
   static void
   compute( Domain& domain )
   {
      using namespace gsse::access_specifier; 
      namespace phoenix = boost::phoenix; 
      using namespace phoenix::local_names;   
      
      typedef gsse::property_domain<Domain>           DomainProperty;
      
      typedef typename gsse::result_of::property_SpaceTopologySegmentsT<DomainProperty>::type   SpaceTopologySegments;
      typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<DomainProperty>::type   SpaceQuantitySegments;  
      typedef typename gsse::result_of::property_FBPosT<DomainProperty>::type                   FBPos;    

      typedef typename gsse::result_of::at_dim_index<SpaceTopologySegments>::type               CellComplex;     
      typedef typename gsse::result_of::at_dim_index<SpaceQuantitySegments>::type               QuanComplex;         
      typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_cl>::type                    Cell;
      typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_ee>::type                    Edge;
      typedef typename gsse::result_of::at_dim<CellComplex, AT_cl>::type                        CellContainer;
      typedef typename gsse::result_of::at_dim<QuanComplex, AT_cl>::type                        CellQuantityContainerSet;
      typedef typename gsse::result_of::at_dim_fiber<CellQuantityContainerSet>::type            CellQuantityContainer;
      
      SpaceTopologySegments&  segments_topology = gsse::at_dim< AC >(domain); 
      SpaceQuantitySegments&  segments_quantity = gsse::at_dim< AQ >(domain);  
      FBPos&                  geometry          = gsse::at_dim< AP >(domain);        
      //
      // declare and define the gsse generic functors
      //
      typedef gsse::boundary< 2, 0 >                                          TopologyOperationQuality;
      typedef gsse::property_data< TopologyOperationQuality, CellContainer >   PropertyDataQuality;
      typedef typename gsse::cell_quality< PropertyDataQuality >::type         Quality;
      Quality qual;

   #ifdef DEBUG_CLASSIFY
      std::cout << "using quality: "; gsse::dumptype< Quality >();      
   #endif           
      // ----
      typedef gsse::boundary< 2, 1 >                                          TopologyOperationMetric;
      typedef gsse::property_data< TopologyOperationMetric, CellContainer >    PropertyDataMetric;
      typedef typename gsse::metric_quantity< PropertyDataMetric >::type       CellLengths;
      // [JW]TODO we need to derive it from the property ?! 
      // however, right now, there is no coupling of the types!
      typedef typename gsse::result_of::metric_quantity< double >::type        MetricQuantityResult;                     
      CellLengths cell_lengths;      
      // ----
      typedef gsse::boundary< 1, 0 >                                          TopologyOperationEdge;
      typedef gsse::property_data< TopologyOperationEdge, CellContainer >      PropertyDataEdge;
      typedef typename gsse::metric_quantity< PropertyDataEdge >::type         Edgeength;
      Edgeength edge_length;      

      const double PI  = 3.14159265;
      const double PI2 = 6.2831853;
      
      using namespace gsse::detail::keys;
      
      std::map<std::string, int>    histogram;         
      
      typedef std::vector< double >   Angles;
      Angles angles_out;      

   #ifdef DEBUG_CLASSIFY
      std::cout << "initializing .." << std::endl;
   #endif
      for( int si = 0; si < gsse::size( segments_topology ); si++ )
      {
         CellContainer& cell_cont = 
            gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)));      
      
         CellQuantityContainer& cell_quality_quan_cont = 
            gsse::at(key_quality)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));      

         CellQuantityContainer& cell_round_quan_cont = 
            gsse::at(key_round)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_dagger_quan_cont = 
            gsse::at(key_dagger)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_blade_quan_cont = 
            gsse::at(key_blade)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         for( long ci = 0; ci < gsse::size( cell_cont ); ci++ )
         {
            gsse::at(0)( gsse::at( ci )( cell_quality_quan_cont ) )  = 0.0;
            gsse::at(0)( gsse::at( ci )( cell_round_quan_cont ) )    = 0.0;
            gsse::at(0)( gsse::at( ci )( cell_dagger_quan_cont ) )   = 0.0;
            gsse::at(0)( gsse::at( ci )( cell_blade_quan_cont ) )    = 0.0;
         }
      }
   #ifdef DEBUG_CLASSIFY
      std::cout << "classifying .." << std::endl;
   #endif
      for( int si = 0; si < gsse::size( segments_topology ); si++ )
      {
      #ifdef DEBUG_CLASSIFY
         std::cout << "  segment: " << si << std::endl;
      #endif
         CellContainer& cell_cont = 
            gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)));
            
         CellQuantityContainer& cell_quality_quan_cont = 
            gsse::at(key_quality)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));        

         CellQuantityContainer& cell_round_quan_cont = 
            gsse::at(key_round)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));     

         CellQuantityContainer& cell_dagger_quan_cont = 
            gsse::at(key_dagger)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));     

         CellQuantityContainer& cell_blade_quan_cont = 
            gsse::at(key_blade)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));                                       

         for( long ci = 0; ci < gsse::size( cell_cont ); ci++ )
         {
            double round_value  = 0;
            double dagger_value = 0;
            double blade_value  = 0;
            double min_angle    = 0;
            double max_angle    = 0;
         
            // 
            // compute the lengths of the edges of the cell
            //         
            MetricQuantityResult cell_length_values = cell_lengths(geometry, cell_cont[ci]);
         
            //
            // sort the lengths
            //
            std::sort( cell_length_values.begin(), cell_length_values.end() );
         
            // 
            // compute the cell quality
            //
            double quality = gsse::at(0)(qual(geometry, cell_cont[ci]));
            
            //
            // store the quality as cell quantity on the domain
            //
            gsse::at(0)( gsse::at( ci )( cell_quality_quan_cont ) ) = quality;
         
            //
            // compute the "roundness" degree in percent
            //
            //   brief: the ratio of the smallest and the largest edge 
            //
            round_value = 100.0 * ( cell_length_values[0] / cell_length_values[2] );
            
            //
            // store the roundness as cell quantity on the domain
            //
            gsse::at(0)( gsse::at( ci )( cell_round_quan_cont ) ) = round_value;  
            
            //
            // if it is a considerable one - register it at the histogram
            //                     
            if( round_value  > 50.0 ) histogram[key_round]++;     

            //
            // compute the cell angles
            //
            Cell& cell = cell_cont[ci];
            
            Edge a( cell[1], cell[2] );
            Edge b( cell[0], cell[2] );
            Edge c( cell[0], cell[1] );                         

            std::set< double > angles;
               
            double a_len = gsse::at(0)(edge_length( geometry, a ));
            double b_len = gsse::at(0)(edge_length( geometry, b ));
            double c_len = gsse::at(0)(edge_length( geometry, c ));

            double alpha = acos( (a_len*a_len + b_len*b_len - c_len*c_len)/(2*a_len*b_len) );
            double beta  = acos( (c_len*c_len + b_len*b_len - a_len*a_len)/(2*c_len*b_len) );
            double gamma = PI - alpha - beta;

            if( ( alpha > PI2 ) || ( alpha < 0 ) ) 
            {
               std::cout << "ERROR - ALPHA ANGLE IS WEIRD: " << alpha << " rad" << std::endl; 
               std::cout << "shutting down .. " << std::endl;
               exit(-1);
            }
            if( ( beta > PI2 ) || ( beta < 0 ) ) 
            {
               std::cout << "ERROR - BETA ANGLE IS WEIRD: " << beta << " rad" << std::endl; 
               std::cout << "shutting down .. " << std::endl;
               exit(-1);
            }
            if( ( gamma > PI2 ) || ( gamma < 0 ) ) 
            {
               std::cout << "ERROR - GAMMA ANGLE IS WEIRD: " << gamma << " rad" << std::endl; 
               std::cout << "shutting down .. " << std::endl;
               exit(-1);
            }
                                    
            //
            // sort the angles and convert to degree
            //
            angles.insert( (alpha * 360.0)/PI2 );
            angles.insert( (beta  * 360.0)/PI2 );
            angles.insert( (gamma * 360.0)/PI2 );
            
            gsse::for_each( angles, phoenix::push_back( phoenix::ref( angles_out ), _1 ) );               

            min_angle = *(angles.begin());
            typename std::set< double >::iterator last = angles.end();
            last--;
            max_angle = *(last);      
            
            //
            // if the smallest edge is considerable smaller than the maximum
            // we've found a dagger
            //
            if( cell_length_values[0] < (0.5*cell_length_values[2]) )
            {
               //
               // compute the "daggerness" degree in percent
               //
               //   brief: by shifting of 1, we can invert the behavior
               //          if the triangle would be optimal, the ratio would yield 1
               //          as the edge lengths are equal. the fix subtraction from 1
               //          would result in a dagger value of zero. 
               //            
               dagger_value = 100.0 * (1.0 - cell_length_values[0] / cell_length_values[1]);

               //
               // store the daggerness as cell quantity on the domain
               //
               gsse::at(0)( gsse::at( ci )( cell_dagger_quan_cont ) ) = dagger_value;  
               
               //
               // if it is a considerable one - register it at the histogram
               //                        
               if( dagger_value > 50.0 ) histogram[key_dagger]++;                   
            }
            //
            // otherwise we've found a blade, as there is no small edge
            //
            else
            {
               //
               // compute the "bladeness" degree in percent
               //
               //   brief: (1- smallest_angle/largest angle)*100
               //
               blade_value = 100.0 * ( 1.0 - min_angle / max_angle );
               
               //
               // store the bladeness as cell quantity on the domain
               //
               gsse::at(0)( gsse::at( ci )( cell_blade_quan_cont ) ) = blade_value;  
   
               //
               // if it is a considerable one - register it at the histogram
               //           
               if( blade_value  > 50.0 ) histogram[key_blade]++;                    
            }
 

         #ifdef DEBUG_CLASSIFY_FULL
            std::cout << "----------------------------------------------------" << std::endl;
            std::cout << "    cell: " << ci << std::endl;
            std::cout << "      quality: " << quality << std::endl;
            std::cout << "      round:   " << round_value << " %" << std::endl;
            std::cout << "      dagger:  " << dagger_value << " %" << std::endl;
            std::cout << "      blade:   " << blade_value << " %" << std::endl;
            std::cout << "      angles - min: " << min_angle << " deg" << std::endl;
            std::cout << "               max: " << max_angle << " deg" << std::endl;            
            std::cout << "----------------------------------------------------" << std::endl;
            std::cout << std::endl;
         #endif            
         }  // end cell traversal

      }  // end segment traversal
   
      {
         std::ofstream  stream; 
         std::string file("latex.txt");
         stream.open(file.c_str());   
         gsse::writeLatex<2>::run(histogram, stream);
         stream.close();         
      }
      {
         std::ofstream  stream; 
         std::string file("angles.txt");
         stream.open(file.c_str());
         gsse::algorithm::print_1d( angles_out, stream );
         stream.close();
      }      
   }
};
// ------------------------------------------------------------------
template < >
struct classify < 3 >
{
   template < typename Domain >
   static void
   compute( Domain& domain )
   {
      using namespace gsse::access_specifier; 
      
      typedef gsse::property_domain<Domain>           DomainProperty;
      
      typedef typename gsse::result_of::property_SpaceTopologySegmentsT<DomainProperty>::type   SpaceTopologySegments;
      typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<DomainProperty>::type   SpaceQuantitySegments;  
      typedef typename gsse::result_of::property_FBPosT<DomainProperty>::type                   FBPos;    

      typedef typename gsse::result_of::at_dim_index<SpaceTopologySegments>::type               CellComplex;     
      typedef typename gsse::result_of::at_dim_index<SpaceQuantitySegments>::type               QuanComplex;         
      typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_cl>::type                    Cell;
      typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_ee>::type                    Edge;
      typedef typename gsse::result_of::at_dim<CellComplex, AT_cl>::type                        CellContainer;
      typedef typename gsse::result_of::at_dim<QuanComplex, AT_cl>::type                        CellQuantityContainerSet;
      typedef typename gsse::result_of::at_dim_fiber<CellQuantityContainerSet>::type            CellQuantityContainer;
      
      SpaceTopologySegments&  segments_topology = gsse::at_dim< AC >(domain); 
      SpaceQuantitySegments&  segments_quantity = gsse::at_dim< AQ >(domain);  
      FBPos&                  geometry          = gsse::at_dim< AP >(domain);           

      static const long DIMT = gsse::result_of::property_DIMT<DomainProperty>::value;

      typedef gsse::boundary< DIMT, 0 >                                          TopologyOperationQuality;
      typedef gsse::property_data< TopologyOperationQuality, CellContainer >   PropertyDataQuality;
      typedef typename gsse::cell_quality< PropertyDataQuality >::type         Quality;
      Quality qual;     

   #ifdef DEBUG_CLASSIFY
      std::cout << "using quality: "; gsse::dumptype< Quality >();      
   #endif     
      typedef std::vector< double >   Angles;
      Angles angles;
     
      typedef std::map<std::string, int> Histogram;
      Histogram histogram;
      typedef gsse::external::SimplexClassification<double> SimplexCl;   
      
      using namespace gsse::detail::keys;

   #ifdef DEBUG_CLASSIFY
      std::cout << "initializing .." << std::endl;
   #endif

      histogram[key_round] = 0;
      histogram[key_wedge] = 0;
      histogram[key_slat] = 0;
      histogram[key_needle] = 0;
      histogram[key_spindle] = 0;
      histogram[key_spade] = 0;
      histogram[key_cap] = 0;
      histogram[key_sliver] = 0;            

      for( int si = 0; si < gsse::size( segments_topology ); si++ )
      {
         CellQuantityContainer& cell_quality_quan_cont = 
            gsse::at(key_quality)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));      

         CellQuantityContainer& cell_round_quan_cont = 
            gsse::at(key_round)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_wedge_quan_cont = 
            gsse::at(key_wedge)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_slat_quan_cont = 
            gsse::at(key_slat)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_needle_quan_cont = 
            gsse::at(key_needle)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_spindle_quan_cont = 
            gsse::at(key_spindle)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_spade_quan_cont = 
            gsse::at(key_spade)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_cap_quan_cont = 
            gsse::at(key_cap)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_sliver_quan_cont = 
            gsse::at(key_sliver)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         for( long ci = 0; ci < gsse::size( cell_quality_quan_cont ); ci++ )
         {
            gsse::at(0)( gsse::at( ci )( cell_quality_quan_cont ) ) = 0.0;
            gsse::at(0)( gsse::at( ci )( cell_round_quan_cont ) )   = 0.0;
            gsse::at(0)( gsse::at( ci )( cell_wedge_quan_cont ) )   = 0.0;
            gsse::at(0)( gsse::at( ci )( cell_slat_quan_cont ) )    = 0.0;
            gsse::at(0)( gsse::at( ci )( cell_needle_quan_cont ) )  = 0.0;
            gsse::at(0)( gsse::at( ci )( cell_spindle_quan_cont ) ) = 0.0;
            gsse::at(0)( gsse::at( ci )( cell_spade_quan_cont ) )   = 0.0;
            gsse::at(0)( gsse::at( ci )( cell_cap_quan_cont ) )     = 0.0;
            gsse::at(0)( gsse::at( ci )( cell_sliver_quan_cont ) )  = 0.0;
         }
      }
   #ifdef DEBUG_CLASSIFY
      std::cout << "classifying .." << std::endl;
   #endif

  
      for( int si = 0; si < gsse::size( segments_topology ); si++ )
      {
      #ifdef DEBUG_CLASSIFY
         std::cout << "segment: " << si << std::endl;
      #endif      
         CellContainer& cell_cont = 
            gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)));      
            
         CellQuantityContainer& cell_quality_quan_cont = 
            gsse::at(key_quality)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));      

         CellQuantityContainer& cell_round_quan_cont = 
            gsse::at(key_round)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_wedge_quan_cont = 
            gsse::at(key_wedge)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_slat_quan_cont = 
            gsse::at(key_slat)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_needle_quan_cont = 
            gsse::at(key_needle)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_spindle_quan_cont = 
            gsse::at(key_spindle)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_spade_quan_cont = 
            gsse::at(key_spade)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_cap_quan_cont = 
            gsse::at(key_cap)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));  

         CellQuantityContainer& cell_sliver_quan_cont = 
            gsse::at(key_sliver)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))));              
      
         double parray[12];
      
         for( long ci = 0; ci < gsse::size( cell_cont ); ci++ )
         {
            std::copy( (gsse::at(gsse::at(0)(gsse::at(ci)(cell_cont)))(geometry)).begin(),
                       (gsse::at(gsse::at(0)(gsse::at(ci)(cell_cont)))(geometry)).end(), 
                       parray );
            std::copy( (gsse::at(gsse::at(1)(gsse::at(ci)(cell_cont)))(geometry)).begin(),
                       (gsse::at(gsse::at(1)(gsse::at(ci)(cell_cont)))(geometry)).end(), 
                       parray+3 );
            std::copy( (gsse::at(gsse::at(2)(gsse::at(ci)(cell_cont)))(geometry)).begin(),
                       (gsse::at(gsse::at(2)(gsse::at(ci)(cell_cont)))(geometry)).end(), 
                       parray+6 );
            std::copy( (gsse::at(gsse::at(3)(gsse::at(ci)(cell_cont)))(geometry)).begin(),
                       (gsse::at(gsse::at(3)(gsse::at(ci)(cell_cont)))(geometry)).end(), 
                       parray+9 );

            //
            // classify
            //        
            SimplexCl simplexc1( parray, parray + 12 );
            
            //
            // load domain cell quantities ..
            //
            double quality = gsse::at(0)(qual(geometry, cell_cont[ci]));
            gsse::at(0)( gsse::at( ci )( cell_quality_quan_cont ) ) = quality;//simplexc1.Ratio;
            gsse::at(0)( gsse::at( ci )( cell_round_quan_cont ) )   = simplexc1.Round;
            gsse::at(0)( gsse::at( ci )( cell_wedge_quan_cont ) )   = simplexc1.Wedge;
            gsse::at(0)( gsse::at( ci )( cell_slat_quan_cont ) )    = simplexc1.Slat;
            gsse::at(0)( gsse::at( ci )( cell_needle_quan_cont ) )  = simplexc1.Needle;
            gsse::at(0)( gsse::at( ci )( cell_spindle_quan_cont ) ) = simplexc1.Spindle;
            gsse::at(0)( gsse::at( ci )( cell_spade_quan_cont ) )   = simplexc1.Spade;
            gsse::at(0)( gsse::at( ci )( cell_cap_quan_cont ) )     = simplexc1.Cap;
            gsse::at(0)( gsse::at( ci )( cell_sliver_quan_cont ) )  = simplexc1.Sliver;
            
            //
            // set histogram
            //
            histogram[simplexc1.ResultName]++;
            
            namespace phoenix = boost::phoenix; 
            using namespace phoenix::local_names;   
            
            gsse::for_each( simplexc1.DiAngle, phoenix::push_back( phoenix::ref( angles ), _1 ) );

         #ifdef DEBUG_CLASSIFY_FULL
            std::cout << "----------------------------------------------------" << std::endl;
            std::cout << "    cell: " << ci << std::endl;
            std::cout << "      quality:               " << quality << std::endl;
            std::cout << "      round:                 " << simplexc1.Round << " %" << std::endl;
            std::cout << "      wedge:                 " << simplexc1.Wedge << " %" << std::endl;
            std::cout << "      slat:                  " << simplexc1.Slat << " %" << std::endl;
            std::cout << "      needle:                " << simplexc1.Needle << std::endl;
            std::cout << "      spindle:               " << simplexc1.Spindle << " %" << std::endl;
            std::cout << "      spade:                 " << simplexc1.Spade << " %" << std::endl;
            std::cout << "      cap:                   " << simplexc1.Cap << " %" << std::endl;
            std::cout << "      sliver:                " << simplexc1.Sliver << " %" << std::endl;
            std::cout << "      dihedral angles - min: " << (simplexc1.DiAngle).front() << " deg" << std::endl;            
            std::cout << "                        max: " << (simplexc1.DiAngle).back() << " deg" << std::endl;            
            std::cout << "----------------------------------------------------" << std::endl;
            std::cout << std::endl;
         #endif             
         }      
      }         
      {
         std::ofstream  stream; 
         std::string file("latex.txt");
         stream.open(file.c_str());   
         gsse::writeLatex<3>::run(histogram, stream);
         stream.close();         
      }
      {
         std::ofstream  stream; 
         std::string file("angles.txt");
         stream.open(file.c_str());
         gsse::algorithm::print_1d( angles, stream );
         stream.close();
      }      

   }
};
// ------------------------------------------------------------------
//
// ==================================================================
//
} // end namespace: detail
//
// ==================================================================
//
template < typename DomainT >
void classify( DomainT& domain )
{
   gsse::detail::classify<
      gsse::result_of::property_DIMT<
         gsse::property_domain<DomainT>
      >::value 
   >::compute(domain);
}
//
// ==================================================================
//
} // end namespace: gsse

#endif
