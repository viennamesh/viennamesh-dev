/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_FEM_INTEGRATION_RULES_HH)
#define GSSE_FEM_INTEGRATION_RULES_HH

// *** BOOST includes
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/container/vector.hpp>

// *** GSSE includes
#include "gsse/topology/elements.hpp"
#include "gsse/datastructure/container.hpp" 
#include "gsse/math/integration.hpp"
#include "gsse/traversal.hpp"
#include "gsse/algorithm.hpp"
#include "gsse/util/size.hpp"

// ============================================================================

namespace gsse { namespace fem { 
   struct weight;
   struct nb;

   struct position;   
   struct jacobian;


// ################

namespace detail {

//[RH][info]
//   gssev02::fiberbundle formulation
//
template<typename NumericTT, long DIMT>
struct  Integration
{
   static const long DIM = DIMT;
   typedef NumericTT NumericT;
   typedef boost::array<NumericT, DIM> PointT;
//   typedef gsse::vector_3ct CoordVecT;   // [RH][new]


   typedef boost::fusion::map<
      boost::fusion::pair<gsse::fem::position,  PointT>
    , boost::fusion::pair<gsse::fem::weight,    NumericT>
    , boost::fusion::pair<gsse::fem::nb,        long>
   >  IntegrationPoint;

   typedef typename gsse::container::create< IntegrationPoint >::type    IntegrationPoints;     // FB
   typedef typename gsse::container::create< IntegrationPoints >::type   IntegrationRule;      // accessible by 'order'


   typedef boost::fusion::map<
      boost::fusion::pair<gsse::topology::segment,       IntegrationRule>

    , boost::fusion::pair<gsse::topology::triangle,      IntegrationRule>
    , boost::fusion::pair<gsse::topology::quadrilateral, IntegrationRule>

    , boost::fusion::pair<gsse::topology::tetrahedron,   IntegrationRule>
    , boost::fusion::pair<gsse::topology::hexahedron,    IntegrationRule>
    , boost::fusion::pair<gsse::topology::prism,         IntegrationRule>
    , boost::fusion::pair<gsse::topology::pyramid,       IntegrationRule>
   >  IntegrationRules;
};
} // namespace detail



// ## ############################
// ## fem_integration part
//
template<typename NumericT, long DIM>
struct  IntegrationCreate
{
   typedef typename detail::Integration<NumericT, DIM>::IntegrationRules  type;
};



// ###################
// [RH][TODO]
//   encapsulate static integration rules in separate compilation unit
//
   static const  long   IR_DIM = 3;
   typedef IntegrationCreate<NumericT, IR_DIM>   Integration_Creator;
  
   
   typedef Integration_Creator::type        FEM_IntegrationType;
   static  FEM_IntegrationType              FEM_IntegrationRules;

// ###################


template<typename FEM_Integration>
struct decode_fem_ip
{
   typedef typename gsse::result_of::at<FEM_Integration, gsse::topology::segment>::type  IntegrationRule;
   typedef typename gsse::result_of::val<IntegrationRule>::type                          IntegrationPoints;
   typedef typename gsse::result_of::val<IntegrationPoints>::type                        IntegrationPoint;
   typedef typename gsse::result_of::at<IntegrationPoint, gsse::fem::position>::type     PointT;
   typedef typename gsse::result_of::val<PointT>::type                                   NumericT;   // [TODO]

};


// #################################

// [RH][TODO] .. check coordinates 

template<typename FEM_Integration>
void build_integration_rules(FEM_Integration& integration_rules, long max_order = 10)  // [RH][TODO] use max order about 20
{
   // [RH][TODO] .. use separate meta functions
   //
   typedef typename decode_fem_ip<FEM_Integration>::IntegrationPoint    IntegrationPoint;   
   typedef typename decode_fem_ip<FEM_Integration>::IntegrationPoints   IntegrationPoints; 
   typedef typename decode_fem_ip<FEM_Integration>::NumericT            NumericT;   

   // #######################################
   // segments
   {
      for (long oi = 1; oi < max_order; ++oi)
      {
         long order = oi;
#ifdef GSSE_DEBUG_FULLOUTPUT
         std::cout << "### segment order: "<< order << std::endl;
#endif

         typedef std::vector<NumericT> ContainerT;
         ContainerT ct_x;
         ContainerT ct_w;
         gsse::math::gauss_legendre_gcc((order)/2 + 1, ct_x, ct_w);   //[RH][new changes]
         
         IntegrationPoints integration_points;
         int ii = 0;
         for (size_t ci = 0; ci < gsse::size(ct_x); ci++)
         {
            IntegrationPoint ip;
            gsse::debug::at<0>(gsse::debug::at<gsse::fem::position>(ip)) = ct_x[ci];
            gsse::debug::at<1>(gsse::debug::at<gsse::fem::position>(ip)) = 0;
            gsse::debug::at<2>(gsse::debug::at<gsse::fem::position>(ip)) = 0;
            
            gsse::debug::at<gsse::fem::weight>(ip)      = ct_w[ci];
            gsse::debug::at<gsse::fem::nb>(ip)          = ii++;
            
            gsse::pushback(  integration_points ) = ip;
            
#ifdef GSSE_DEBUG_OUTPUT
            gsse::algorithm::print(ip);
#endif
         }
         gsse::at( order) ( gsse::debug::at<gsse::topology::segment>(integration_rules) ) = integration_points;  
      } // order
   }

   // #######################################
   // triangle
   {  

      // ## 
      {
         long order = 1;
#ifdef GSSE_DEBUG_FULLOUTPUT
         std::cout << "### order: "<< order << std::endl;
#endif

         NumericT triangle_order1_positions[][3] =  
         {
            { 1.0/3.0,    1.0/3.0 },
         };
         
         NumericT triangle_order1_weights[] = 
         {  0.5 } ;
         
         IntegrationPoints integration_points;
         long ii = 0;
         for (long  ci = 0; ci < 1; ci++)
         {
            IntegrationPoint ip;
	    gsse::debug::at<0>(gsse::debug::at<gsse::fem::position>(ip)) = triangle_order1_positions[ci][0];
	    gsse::debug::at<1>(gsse::debug::at<gsse::fem::position>(ip)) = triangle_order1_positions[ci][1];
	    gsse::debug::at<2>(gsse::debug::at<gsse::fem::position>(ip)) = 0;

	    gsse::debug::at<gsse::fem::weight>(ip)      = triangle_order1_weights[ci];
	    gsse::debug::at<gsse::fem::nb>(ip)          = ii++;

	    gsse::pushback(  integration_points ) = ip;

#ifdef GSSE_DEBUG_FULLOUTPUT
//            gsse::algorithm::print(ip);
            std::cout << "..ip: " << ip << std::endl;
#endif
         }
         gsse::at( order) ( gsse::debug::at<gsse::topology::triangle>(integration_rules) ) = integration_points;  
      } // order


      {
         long order = 2;
#ifdef GSSE_DEBUG_FULLOUTPUT
         std::cout << "### order: "<< order << std::endl;
#endif

         NumericT triangle_order2_positions[][3] =  
         {
            { 0,   0.5 },
            { 0.5, 0,  },
            { 0.5, 0.5 }
         };

         NumericT triangle_order2_weights[] = 
         {
            1.0/6.0, 1.0/6.0 , 1.0/6.0
         };
         
         IntegrationPoints integration_points;
         long ii = 0;
         for (long ci = 0; ci < 3; ci++)
         {
            IntegrationPoint ip;
            gsse::debug::at<0>(gsse::debug::at<gsse::fem::position>(ip)) = triangle_order2_positions[ci][0];
            gsse::debug::at<1>(gsse::debug::at<gsse::fem::position>(ip)) = triangle_order2_positions[ci][1];
            gsse::debug::at<2>(gsse::debug::at<gsse::fem::position>(ip)) = 0;

            gsse::debug::at<gsse::fem::weight>(ip)      = triangle_order2_weights[ci];
            gsse::debug::at<gsse::fem::nb>(ip)          = ii++;
            
            gsse::pushback(  integration_points ) = ip;
            
#ifdef GSSE_DEBUG_OUTPUT
            gsse::algorithm::print(ip);
#endif
         }
         gsse::at( order ) ( gsse::debug::at<gsse::topology::triangle>(integration_rules) ) = integration_points;  
      } // order

      {
         long order = 3;
#ifdef GSSE_DEBUG_FULLOUTPUT
         std::cout << "### order: "<< order << std::endl;
#endif

         NumericT triangle_order3_positions[][3] =  
         {
            { 0.816847572980459, 0.091576213509771, },
            { 0.091576213509771, 0.816847572980459, },
            { 0.091576213509771, 0.091576213509771, },

            { 0.108103018168070, 0.445948490915965, },
            { 0.445948490915965, 0.108103018168070, },
            { 0.445948490915965, 0.445948490915965 }
         };

         NumericT triangle_order3_weights[] = 
         {
            0.054975871827661, 0.054975871827661, 0.054975871827661,
            0.111690794839005, 0.111690794839005, 0.111690794839005
         };
         
         IntegrationPoints integration_points;
         long ii = 0;
         for (long ci = 0; ci < 6; ci++)
         {
            IntegrationPoint ip;
            gsse::debug::at<0>(gsse::debug::at<gsse::fem::position>(ip)) = triangle_order3_positions[ci][0];
            gsse::debug::at<1>(gsse::debug::at<gsse::fem::position>(ip)) = triangle_order3_positions[ci][1];
            gsse::debug::at<2>(gsse::debug::at<gsse::fem::position>(ip)) = 0;

            gsse::debug::at<gsse::fem::weight>(ip)      = triangle_order3_weights[ci];
            gsse::debug::at<gsse::fem::nb>(ip)          = ii++;
            
            gsse::pushback(  integration_points ) = ip;
            
#ifdef GSSE_DEBUG_OUTPUT
            gsse::algorithm::print(ip);
#endif
         }
         gsse::at( order) ( gsse::debug::at<gsse::topology::triangle>(integration_rules) ) = integration_points;  
      } // order


      {
         long order = 4;
#ifdef GSSE_DEBUG_FULLOUTPUT
         std::cout << "### order: "<< order << std::endl;
#endif

         NumericT triangle_order3_positions[][3] =  
         {
            { 0.816847572980459, 0.091576213509771, },
            { 0.091576213509771, 0.816847572980459, },
            { 0.091576213509771, 0.091576213509771, },

            { 0.108103018168070, 0.445948490915965, },
            { 0.445948490915965, 0.108103018168070, },
            { 0.445948490915965, 0.445948490915965 }
         };

         NumericT triangle_order3_weights[] = 
         {
            0.054975871827661, 0.054975871827661, 0.054975871827661,
            0.111690794839005, 0.111690794839005, 0.111690794839005
         };
         
         IntegrationPoints integration_points;
         long ii = 0;
         for (long ci = 0; ci < 6; ci++)
         {
            IntegrationPoint ip;
            gsse::debug::at<0>(gsse::debug::at<gsse::fem::position>(ip)) = triangle_order3_positions[ci][0];
            gsse::debug::at<1>(gsse::debug::at<gsse::fem::position>(ip)) = triangle_order3_positions[ci][1];
            gsse::debug::at<2>(gsse::debug::at<gsse::fem::position>(ip)) = 0;

            gsse::debug::at<gsse::fem::weight>(ip)      = triangle_order3_weights[ci];
            gsse::debug::at<gsse::fem::nb>(ip)          = ii++;
            
            gsse::pushback(  integration_points ) = ip;
            
#ifdef GSSE_DEBUG_OUTPUT
            gsse::algorithm::print(ip);
#endif
         }
         gsse::at( order) ( gsse::debug::at<gsse::topology::triangle>(integration_rules) ) = integration_points;  
      } // order



      {
         long order = 5;
#ifdef GSSE_DEBUG_FULLOUTPUT
         std::cout << "### order 5/: "<< order << std::endl;
#endif

         NumericT triangle_order5_positions[][3] =  
         {
            { 0.873821971016996, 0.063089014491502, },
            { 0.063089014491502, 0.873821971016996, },
            { 0.063089014491502, 0.063089014491502, },
            { 0.501426509658179, 0.249286745170910, },
            { 0.249286745170910, 0.501426509658179, },
            { 0.249286745170910, 0.249286745170910, },
            
            { 0.636502499121399, 0.310352451033785, },
            { 0.310352451033785, 0.053145049844816, },
            { 0.053145049844816, 0.636502499121399, },
            { 0.636502499121399, 0.053145049844816, },
            { 0.310352451033785, 0.636502499121399, },
            { 0.053145049844816, 0.310352451033785, }
         };

         NumericT triangle_order5_weights[] = 
         {
            0.025422453185103, 0.025422453185103, 0.025422453185103,
            0.058393137863189, 0.058393137863189, 0.058393137863189,
            
            0.041425537809187, 0.041425537809187, 0.041425537809187,
            0.041425537809187, 0.041425537809187, 0.041425537809187 
         };
         
         IntegrationPoints integration_points;
         long ii = 0;
         for (long ci = 0; ci < 12; ci++)
         {
            IntegrationPoint ip;
            gsse::debug::at<0>(gsse::debug::at<gsse::fem::position>(ip)) = triangle_order5_positions[ci][0];
            gsse::debug::at<1>(gsse::debug::at<gsse::fem::position>(ip)) = triangle_order5_positions[ci][1];
            gsse::debug::at<2>(gsse::debug::at<gsse::fem::position>(ip)) = 0;

            gsse::debug::at<gsse::fem::weight>(ip)      = triangle_order5_weights[ci];
            gsse::debug::at<gsse::fem::nb>(ip)          = ii++;
            
            gsse::pushback(  integration_points ) = ip;
            
#ifdef GSSE_DEBUG_OUTPUT
            gsse::algorithm::print(ip);
#endif
         }
         gsse::at( order) ( gsse::debug::at<gsse::topology::triangle>(integration_rules) ) = integration_points;  
      } // order


      {
         long order = 6;
#ifdef GSSE_DEBUG_FULLOUTPUT
         std::cout << "### order 5/: "<< order << std::endl;
#endif

         NumericT triangle_order5_positions[][3] =  
         {
            { 0.873821971016996, 0.063089014491502, },
            { 0.063089014491502, 0.873821971016996, },
            { 0.063089014491502, 0.063089014491502, },
            { 0.501426509658179, 0.249286745170910, },
            { 0.249286745170910, 0.501426509658179, },
            { 0.249286745170910, 0.249286745170910, },
            
            { 0.636502499121399, 0.310352451033785, },
            { 0.310352451033785, 0.053145049844816, },
            { 0.053145049844816, 0.636502499121399, },
            { 0.636502499121399, 0.053145049844816, },
            { 0.310352451033785, 0.636502499121399, },
            { 0.053145049844816, 0.310352451033785, }
         };

         NumericT triangle_order5_weights[] = 
         {
            0.025422453185103, 0.025422453185103, 0.025422453185103,
            0.058393137863189, 0.058393137863189, 0.058393137863189,
            
            0.041425537809187, 0.041425537809187, 0.041425537809187,
            0.041425537809187, 0.041425537809187, 0.041425537809187 
         };
         
         IntegrationPoints integration_points;
         long ii = 0;
         for (long ci = 0; ci < 12; ci++)
         {
            IntegrationPoint ip;
            gsse::debug::at<0>(gsse::debug::at<gsse::fem::position>(ip)) = triangle_order5_positions[ci][0];
            gsse::debug::at<1>(gsse::debug::at<gsse::fem::position>(ip)) = triangle_order5_positions[ci][1];
            gsse::debug::at<2>(gsse::debug::at<gsse::fem::position>(ip)) = 0;

            gsse::debug::at<gsse::fem::weight>(ip)      = triangle_order5_weights[ci];
            gsse::debug::at<gsse::fem::nb>(ip)          = ii++;
            
            gsse::pushback(  integration_points ) = ip;
            
#ifdef GSSE_DEBUG_OUTPUT
            gsse::algorithm::print(ip);
#endif
         }
         gsse::at( order) ( gsse::debug::at<gsse::topology::triangle>(integration_rules) ) = integration_points;  
      } // order

      // ########################################
      // ### here, numerical calculation kicks in

      for (long oi = 7; oi < max_order; ++oi)
      // ## 
      {
         long order = oi;
#ifdef GSSE_DEBUG_FULLOUTPUT
         std::cout << "### order: "<< order << std::endl;
#endif

         typedef std::vector<NumericT> ContainerT;
         ContainerT ct_xx, ct_xy;
         ContainerT ct_wx, ct_wy;
         gsse::math::gauss_legendre_gcc(order/2 + 2, ct_xx, ct_wx);
         gsse::math::gauss_legendre_gcc(order/2 + 1, ct_xy, ct_wy);
         
         
         IntegrationPoints integration_points;
         int ii = 0;
         for (size_t ci = 0; ci < gsse::size(ct_xx); ci++)
         {
            for (size_t cj = 0; cj < gsse::size(ct_xy); cj++)
            {
               IntegrationPoint ip;
               gsse::debug::at<0>(gsse::debug::at<gsse::fem::position>(ip)) = ct_xx[ci];
               gsse::debug::at<1>(gsse::debug::at<gsse::fem::position>(ip)) = ct_xy[cj] * ( 1 - ct_xx[ci] );
               gsse::debug::at<2>(gsse::debug::at<gsse::fem::position>(ip)) = 0;

               gsse::debug::at<gsse::fem::weight>(ip)      = ct_wx[ci]*ct_wy[cj]*(1-ct_xx[ci]);
               gsse::debug::at<gsse::fem::nb>(ip)          = ii++;
               
               gsse::pushback(  integration_points ) = ip;

#ifdef GSSE_DEBUG_OUTPUT
               gsse::algorithm::print(ip);
#endif
            }
         }
         gsse::at( order) ( gsse::debug::at<gsse::topology::triangle>(integration_rules) ) = integration_points;  
      } // order

   }  //triangle




   // #######################################
   // tetrahedron
   {  

      // ## 
      {
         long order = 1;
#ifdef GSSE_DEBUG_FULLOUTPUT
         std::cout << "### order: "<< order << std::endl;
#endif

         NumericT tetrahedron_order1_positions[][3] =  
         { 
            { 0.25, 0.25, 0.25 },
         };

         
         NumericT tetrahedron_order1_weights[] = 
         {
            1.0/6.0
         };
         
         IntegrationPoints integration_points;
         long ii = 0;
         for (long ci = 0; ci < 1; ci++)
         {
            IntegrationPoint ip;
            gsse::debug::at<0>(gsse::debug::at<gsse::fem::position>(ip)) = tetrahedron_order1_positions[ci][0];
            gsse::debug::at<1>(gsse::debug::at<gsse::fem::position>(ip)) = tetrahedron_order1_positions[ci][1];
            gsse::debug::at<2>(gsse::debug::at<gsse::fem::position>(ip)) = tetrahedron_order1_positions[ci][2];

            gsse::debug::at<gsse::fem::weight>(ip)      = tetrahedron_order1_weights[ci];
            gsse::debug::at<gsse::fem::nb>(ip)          = ii++;
            
            gsse::pushback(  integration_points ) = ip;
            
#ifdef GSSE_DEBUG_OUTPUT
            gsse::algorithm::print(ip);
#endif
         }
         gsse::at( order) ( gsse::debug::at<gsse::topology::tetrahedron>(integration_rules) ) = integration_points;  
      } // order

      // ## 
      {
         long order = 2;
#ifdef GSSE_DEBUG_FULLOUTPUT
         std::cout << "### order: "<< order << std::endl;
#endif

         NumericT tetrahedron_order2_positions[][3] =  
         {
            { 0.585410196624969, 0.138196601125011, 0.138196601125011 },
            { 0.138196601125011, 0.585410196624969, 0.138196601125011 },
            { 0.138196601125011, 0.138196601125011, 0.585410196624969 },
            { 0.138196601125011, 0.138196601125011, 0.138196601125011 }
         };
         
         NumericT tetrahedron_order2_weights[] = 
         { 1.0/24.0, 1.0/24.0, 1.0/24.0, 1.0/24.0 };
         
         IntegrationPoints integration_points;
         long ii = 0;
         for (long ci = 0; ci < 1; ci++)
         {
            IntegrationPoint ip;
            gsse::debug::at<0>(gsse::debug::at<gsse::fem::position>(ip)) = tetrahedron_order2_positions[ci][0];
            gsse::debug::at<1>(gsse::debug::at<gsse::fem::position>(ip)) = tetrahedron_order2_positions[ci][1];
            gsse::debug::at<2>(gsse::debug::at<gsse::fem::position>(ip)) = tetrahedron_order2_positions[ci][2];

            gsse::debug::at<gsse::fem::weight>(ip)      = tetrahedron_order2_weights[ci];
            gsse::debug::at<gsse::fem::nb>(ip)          = ii++;
            
            gsse::pushback(  integration_points ) = ip;
            
#ifdef GSSE_DEBUG_OUTPUT
            gsse::algorithm::print(ip);
#endif
         }
         gsse::at( order) ( gsse::debug::at<gsse::topology::tetrahedron>(integration_rules) ) = integration_points;  
      } // order


//       {
//          long order = 2;
#ifdef GSSE_DEBUG_FULLOUTPUT
//          std::cout << "### order: "<< order << std::endl;
#endif


   } // tetrahedron

}

} // namespace fem
} // namespace gsse


#endif


