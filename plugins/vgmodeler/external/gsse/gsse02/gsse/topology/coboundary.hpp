/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2008-2009 Philipp Schwaha                      philipp@gsse.at
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_TOPOLOGY_COBOUNDARY_HH)
#define GSSE_TOPOLOGY_COBOUNDARY_HH

// *** system includes
#include <vector>

// *** BOOST includes
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>   // necessary for operator,
#include <boost/spirit/home/phoenix/container.hpp>
#include "boost/static_assert.hpp"

// *** GSSE includes
#include "gsse/util/size.hpp"
#include "gsse/datastructure/container.hpp"
#include "gsse/datastructure/utils.hpp"
#include "gsse/algorithm/topological.hpp"
#include "gsse/topology/boundary.hpp"
#include "gsse/traversal.hpp"
#include "gsse/util.hpp"
#include "gsse/math/pset.hpp"
#include "gsse/topology/cells.hpp"

// ############################################################
//
//   VX/vx: does NOT mean an explicit vertex, instead it describes a given set which connects to another set
//   CL/cl: does NOT mean an explicit cell,   instead it describes a given set which connects to another set
// actually we also can use: c1, c2, .., cn
// but maybe things are not going to be easier by this
// 
namespace gsse { 



// ##################################
// ## meta-function:: boundary operation

namespace result_of { 

template<long DIMSource, long DIMRelative, typename CellTopology = gsse::cell_simplex>
struct coboundary
{
   static const long DIMCSource   = DIMSource;
   static const long DIMCRelative = DIMRelative;                 // co-boundary dimension
   static const long DIMCResult   = DIMCSource + DIMCRelative;   // dimension of result cell
   typedef typename gsse::topology::result_of::cell_calc_container<DIMCResult, CellTopology>::type  type;
};

} // namespace result_of





namespace coboundary_detail {

template<long DIM_co_bnd, 
         typename PosetGenerator,
         typename ContainerVXonCL>
typename topology::result_of::cell_calc_container<DIM_co_bnd, typename PosetGenerator::CellT>::type
create_boundary_for_cells(const PosetGenerator&   poset_generator,
                          const ContainerVXonCL&  cont_vx_on_cl_cp)     
{
   using namespace boost::phoenix;
   using namespace boost::phoenix::arg_names;

   typedef typename PosetGenerator::CellT CellType;

   static const long VX_on_CL  = gsse::cell_cardinality_calculator<CellType>::template apply<DIM_co_bnd>::value;
   typedef gsse::array<long, VX_on_CL>                                BoundaryCellCT;  
   typedef typename gsse::container::create< BoundaryCellCT >::type   ContainerBndCT; 

   // =========================================================================================================================

   // create boundary operation from queried co-boundary operation
   // e.g., from 0/1/2 -> 0/1 ; 0/2 ; ..
   // 
   ContainerBndCT conts_bnd_ct;
   //   long dim_cc = poset_generator.get_dim();
   long dim_cc = PosetGenerator::DIM_cell; // [RH][TODO] ..use common mpl value metafunction

   if ((dim_cc - DIM_co_bnd) < 0)
   {
     std::cout << "## dimension problem.. dim_cc: " << dim_cc << " DIM_co_bnd: " << DIM_co_bnd << "  --> result in dimension access: " << dim_cc - DIM_co_bnd << std::endl;
     return typename topology::result_of::cell_calc_container<DIM_co_bnd, typename PosetGenerator::CellT>::type ();
   }
   gsse::algorithm::transform_zip_2d (poset_generator[ dim_cc - DIM_co_bnd ], conts_bnd_ct, boost::phoenix::arg_names::_1 = boost::phoenix::arg_names::_2 );   

   ContainerBndCT container_new;
   for (size_t i = 0; i < gsse::size(cont_vx_on_cl_cp); ++i)
   {
      ContainerBndCT cont_bnd_ct = conts_bnd_ct;   // make a copy.. this container is then  modified.. 

      // transfer vx-indices to temporary container
      // e.g., cell: 0/10/11
      //
      typedef std::vector<long>       CellRT;
      CellRT data;
      gsse::traverse() [ boost::phoenix::push_back(boost::phoenix::ref(data), boost::phoenix::arg_names::_1) ] (gsse::at(i)(cont_vx_on_cl_cp));
      

      // replace topological elements (index elements) by vx-on-cell indices
      // e.g., 0/2 -> 0/11
      //
      gsse::traverse() [ gsse::traverse()  [  arg1 =    boost::phoenix::at(boost::phoenix::ref(data), boost::phoenix::arg_names::_1)   ]   ](cont_bnd_ct);
      
      // ===========================================================================
      gsse::traverse() 
      [
         boost::phoenix::push_back(boost::phoenix::ref(container_new), arg1)
      ](cont_bnd_ct);
   }

   return container_new;
} 

template<typename CellInitT, typename ContainerT>
ContainerT
filter(const CellInitT&   init_val, 
       const ContainerT&  cont)
{
   using namespace boost::phoenix;
   using namespace boost::phoenix::arg_names;
   using namespace boost::phoenix::local_names;

   ContainerT container_new;

   // extract just specific boundary values
   //   which corresponds to vx-cell -> init_val
   //
   gsse::traverse() 
   [
      let(_a = 0)
      [
         gsse::traverse() 
         [
            if_(boost::phoenix::arg_names::_1 == ref(init_val))
            [
               _a = 1
            ]
         ]
         ,
         if_(_a == 1)
         [
            boost::phoenix::push_back(boost::phoenix::ref(container_new), arg1)
         ]
     ]
   ](cont);
   
   gsse::algorithm::unique(container_new);
   return container_new;
}


// [info]
//   a coboundary operator creates the appropriate DIM_cobndT dimension boundaries of
//    the given cells 
//
//   DIMSource   :: the source cell dimension                (e.g., 1-cell (edge))
//   DIMRelative :: the (relative) co-boundary dimension     (e.g., +1 -> 2-cell(triangle))
//   
//
template<long DIMSource, long DIMRelative, typename CellTopology = gsse::cell_simplex>
struct cobnd_on_vx
{
   static const long DIMCSource   = DIMSource;
   static const long DIMCResult   = DIMCSource + DIMRelative;   // dimension of result cell
   typedef CellTopology         CellTopo;

   template<typename CellInit,
            typename Cont_CLonVX,
            typename Cont_VXonCL>
   typename gsse::topology::result_of::cell_calc_container<DIMCResult, CellTopology>::type  
   operator() (const CellInit&     cell_init,
               const Cont_CLonVX&  cont_cl_on_vx,
               const Cont_VXonCL&  cont_vx_on_cl)
   {
      //  reverse calculation of celltopology dimension
     static const long DIMcell = gsse::cell_reverse_cardinality_calculator<CellTopology>::template apply<Cont_VXonCL::value_type::static_size>::value;  // [RH][TODO] use metafunctions to extract size
     gsse::math::poset_generator<DIMcell, CellTopology> poset_generator;

      // set operation, select sets from 
      //      *) cont_vx_on_cl 
      //      *) selected  by elements of cont_cl_on_vx
      //
      // example:
      //   vx on cl::0:  0/1/2
      //             1:  0/2/3
      //
      //   cl on vx: 0:  0/1/2/3/4/5/6/..
      //             1:  0/1

      Cont_VXonCL cont_cli_on_vx;
      gsse::algorithm::topological::copy_by_view( 
         cont_cli_on_vx,  // ToContainer
         cont_cl_on_vx,     // ContainerView
         cont_vx_on_cl      // ContainerSource
         );


      typename gsse::topology::result_of::cell_calc_container<DIMCResult, CellTopology>::type  
      cont_cobnd_on_init =
         gsse::coboundary_detail::create_boundary_for_cells<DIMCResult>(poset_generator,
                                                                        cont_cli_on_vx);
//       std::cout << "## cobnd on init-cell::" << std::endl;
//       gsse::algorithm::print_1d(cont_cobnd_on_init);


      typename gsse::topology::result_of::cell_calc_container<DIMCResult, CellTopology>::type  
         container_cobnd_on_init_f = 
         gsse::coboundary_detail::filter(cell_init, cont_cobnd_on_init);

//       std::cout << "## filtered cobnd on init-cell:" << std::endl;
//       gsse::algorithm::print_1d(container_cobnd_on_init_f);


      return container_cobnd_on_init_f;
   }
  

private:
  //   gsse::math::poset_generator_rt<CellTopology> poset_generator;

};

//#################################################################################################
} // namespace coboundary-detail




template<long DIMSource, long DIMRelative, typename CellTopology = gsse::cell_simplex>
struct coboundary
{
   //
   static const long DIMCSource   = DIMSource;
   static const long DIMCResult   = DIMCSource + DIMRelative;   // dimension of result cell
   typedef CellTopology         CellTopo;

   // [concept requirement] dense index space for cell-on-vertex 
   //
   template<typename Cell,
            typename TopologyVX,
            typename TopologyCL>
   typename gsse::result_of::coboundary<DIMSource, DIMRelative,CellTopology>::type 
   operator() (const Cell& cell,
               const TopologyVX&         topology_vx,   
               const TopologyCL&         topology_cl)   
   {
      gsse::coboundary_detail::cobnd_on_vx<DIMSource, DIMRelative, CellTopology>  cobnd;
      typedef typename gsse::result_of::coboundary<DIMSource, DIMRelative,CellTopology>::type cnt_final_type;
      cnt_final_type cnt_final;

      typedef typename gsse::result_of::val<cnt_final_type>::type cnt_final_val_type;
      std::map< cnt_final_val_type, bool >              uniquer;

      for (size_t ci = 0; ci < gsse::size( cell ) ; ++ci)
      {
         typename gsse::result_of::coboundary<DIMSource, DIMRelative,CellTopology>::type 
            cont_result_v0 = cobnd( cell[ci],   
                                    gsse::at_fiber(gsse::at( cell[ci] )( topology_vx )),
                                    topology_cl );
         

         for (size_t cont_i = 0; cont_i < gsse::size (cont_result_v0); ++cont_i)
         {
            if ( gsse::algorithm::topological::subset( cell, cont_result_v0[cont_i] )  )
            {
               cnt_final_val_type temp = cont_result_v0[cont_i];
               std::sort(temp.begin(), temp.end());
               if(!uniquer[temp])
               {
                  cnt_final.push_back(cont_result_v0[cont_i]);
                  uniquer[temp]=true;
               }
            }
         }
      }

      //gsse::algorithm::unique(cnt_final);

      return cnt_final;
   }


   // [concept requirement] sparse index space for cell-on-vertex 
   //
   template<typename Cell,
            typename TopologyVX,
            typename TopologyCL,
            typename GlobLocMapT>  
   typename gsse::result_of::coboundary<DIMSource, DIMRelative,CellTopology>::type 
   operator() (const Cell& cell,
               const TopologyVX&     topology_vx,   
               const TopologyCL&     topology_cl,
               GlobLocMapT&        global_local) 

   {

      gsse::coboundary_detail::cobnd_on_vx<DIMSource, DIMRelative, CellTopology>       cobnd;
      typedef typename gsse::result_of::coboundary<DIMSource, DIMRelative,CellTopology>::type cnt_final_type;
      cnt_final_type cnt_final;

      typedef typename gsse::result_of::val<cnt_final_type>::type cnt_final_val_type;
      std::map< cnt_final_val_type, bool >              uniquer;


      for (size_t ci = 0; ci < gsse::size( cell ) ; ++ci)
      {
// 	std::cout << "using index : " << global_local[cell[ ci ]] << " for cell index: " << cell[ci] << std::endl;
	
// 	std::cout << "size of vx: " << gsse::size( topology_vx  ) << std::endl;
// 	std::cout << "size of cl: " << gsse::size( topology_cl  ) << std::endl;
            
         typename gsse::result_of::coboundary<DIMSource, DIMRelative,CellTopology>::type 
            cont_result_v0 = cobnd( cell[ci],   
                                    gsse::at_fiber (gsse::at(global_local[cell[ ci ]])( topology_vx ) ),  
                                    topology_cl );
         

         for (size_t cont_i = 0; cont_i < gsse::size (cont_result_v0); ++cont_i)
         {
            if ( gsse::algorithm::topological::subset( cell, cont_result_v0[cont_i] )  )
            {
               cnt_final_val_type temp = cont_result_v0[cont_i];
               std::sort(temp.begin(), temp.end());
               if(!uniquer[temp])
               {
                  cnt_final.push_back(cont_result_v0[cont_i]);
                  uniquer[temp]=true;
               }
            }
         }
      }

      //gsse::algorithm::unique(cnt_final);

      return cnt_final;
   }
};


template<typename Segment, long DIMSource, long DIMRelative, typename CellTopo>
struct segment_coboundary
{
   static const long DIMCSource   = DIMSource;
   static const long DIMCResult   = DIMCSource + DIMRelative;   // dimension of result cell
   typedef CellTopo CellTopology ;

// =============

   segment_coboundary(Segment const& segment):segment(segment){}

   // [concept requirement] dense index space for cell-on-vertex 
   //
   template<typename Cell>
   typename gsse::result_of::coboundary<DIMSource, DIMRelative,CellTopology>::type 
   operator() (const Cell& cell)   
   {
      gsse::coboundary<DIMSource, DIMRelative, CellTopology>  cobnd;

      typename gsse::result_of::coboundary<DIMSource, DIMRelative,CellTopology>::type 
         cnt_final = cobnd(cell,   
                           gsse::at_dim<gsse::access_specifier::AT_vx>( gsse::at_fiber( segment )),
                           gsse::at_dim<gsse::access_specifier::AT_cl>( gsse::at_fiber( segment ))  );
      return cnt_final;
   }


   // [concept requirement] sparse index space for cell-on-vertex 
   // [RH][TODO]
   //   here extract the transfer map automatically
   //
   template<typename Cell, typename IndexMap>
   typename gsse::result_of::coboundary<DIMSource, DIMRelative,CellTopology>::type 
   operator() (const Cell& cell, IndexMap& index_map)   
   {
      gsse::coboundary<DIMSource, DIMRelative, CellTopology>  cobnd;

      typename gsse::result_of::coboundary<DIMSource, DIMRelative,CellTopology>::type 
         cnt_final = cobnd(cell,   
                           gsse::at_dim<gsse::access_specifier::AT_vx>( gsse::at_fiber( segment )),
                           gsse::at_dim<gsse::access_specifier::AT_cl>( gsse::at_fiber( segment )),
                           index_map);
      return cnt_final;
   }

private:
   Segment const& segment;
};



// ==================================================================================================================================
//
// convenience declarations
//    meta-function:: coboundary operation
//
namespace result_of {


// ############################################
// ############ cell on vertex ################
// ###
template<typename T1, typename T2 = void>
struct cell_on_vertex
{};
template<long DIMT, typename CellTopologyT>
struct cell_on_vertex<gsse::property_data<DIMT, CellTopologyT> >
{
   static const long DIMSource = 0;
   static const long DIMRelative = DIMT; 

   typedef gsse::coboundary<DIMSource, DIMRelative, CellTopologyT>  type;
   typedef typename gsse::result_of::coboundary<0, DIMT, CellTopologyT>::type  result;
};

template<typename DataStructure>
struct cell_on_vertex<gsse::property_complex<DataStructure> >
{
   static const long DIMSource = 0;
   static const long DIMRelative = gsse::property_complex<DataStructure>::DIMT;

   typedef gsse::coboundary<DIMSource, DIMRelative, typename gsse::property_complex<DataStructure>::CellTopology>   type;
   typedef typename gsse::result_of::coboundary<DIMSource, DIMRelative, typename gsse::property_complex<DataStructure>::CellTopology>::type  result;
};

template<typename DataStructure>
struct cell_on_vertex<gsse::property_space<DataStructure> >
{
   static const long DIMSource = 0;
   static const long DIMRelative = gsse::property_space<DataStructure>::DIMT;

   typedef gsse::segment_coboundary< 
      typename gsse::result_of::val<typename gsse::property_space<DataStructure>::SubSpace_CellComplex>::type
      , DIMSource, DIMRelative, typename gsse::property_space<DataStructure>::CellTopology>  type;
   typedef typename gsse::result_of::coboundary<DIMSource, DIMRelative, typename gsse::property_space<DataStructure>::CellTopology>::type   result;
};

// ############################################
// ############ facet on vertex ################
// ###
template<typename T1, typename T2 = void>
struct facet_on_vertex
{};
template<long DIMT, typename CellTopologyT>
struct facet_on_vertex<gsse::property_data<DIMT, CellTopologyT> >
{
   typedef gsse::coboundary<0, 2, CellTopologyT>   type;
   typedef typename gsse::result_of::coboundary<0, 2, CellTopologyT>::type  result;
};

template<typename DataStructure>
struct facet_on_vertex<gsse::property_complex<DataStructure> >
{
   typedef gsse::coboundary<0, 2, typename gsse::property_complex<DataStructure>::CellTopology>   type;
   typedef typename gsse::result_of::coboundary<0, 2, typename gsse::property_complex<DataStructure>::CellTopology>::type   result;
};

template<typename DataStructure>
struct facet_on_vertex<gsse::property_space<DataStructure> >
{
   typedef gsse::segment_coboundary< 
      typename gsse::result_of::val<typename gsse::property_space<DataStructure>::SubSpace_CellComplex>::type
      , 0 , 2, typename gsse::property_space<DataStructure>::CellTopology>  type;
   typedef typename gsse::result_of::coboundary<0, 2, typename gsse::property_space<DataStructure>::CellTopology>::type   result;
};


// ############################################
// ############ edge on vertex ################
// ###
template<typename T1, typename T2 = void>
struct edge_on_vertex
{};
template<long DIMT, typename CellTopologyT>
struct edge_on_vertex<gsse::property_data<DIMT, CellTopologyT> >
{
   typedef gsse::coboundary<0, 1, CellTopologyT>   type;
   typedef typename gsse::result_of::coboundary<0, 1, CellTopologyT>::type  result;
};

template<typename DataStructure>
struct edge_on_vertex<gsse::property_complex<DataStructure> >
{
   typedef gsse::coboundary<0, 1, typename gsse::property_complex<DataStructure>::CellTopology>   type;
   typedef typename gsse::result_of::coboundary<0, 1, typename gsse::property_complex<DataStructure>::CellTopology>::type  result;
};

template<typename DataStructure>
struct edge_on_vertex<gsse::property_space<DataStructure> >
{
   typedef gsse::segment_coboundary< 
      typename gsse::result_of::val<typename gsse::property_space<DataStructure>::SubSpace_CellComplex>::type
      , 0 ,1, typename gsse::property_space<DataStructure>::CellTopology>  type;
   typedef typename gsse::result_of::coboundary<0, 1, typename gsse::property_space<DataStructure>::CellTopology>::type  result;
};




// ############################################
// ############ cell on edge ##################
// ###
template<typename T1, typename T2 = void>
struct cell_on_edge
{};
template<long DIMT, typename CellTopologyT>
struct cell_on_edge<gsse::property_data<DIMT, CellTopologyT> >
{
   // DIMT-1 -> relative coboundary dimension
   //
   typedef gsse::coboundary<1, DIMT-1, CellTopologyT>  type;
   typedef typename gsse::result_of::coboundary<1, DIMT-1, CellTopologyT>::type   result;
};

template<typename DataStructure>
struct cell_on_edge<gsse::property_complex<DataStructure> >
{
   static const long DIM = gsse::property_complex<DataStructure>::DIMT;

   typedef gsse::coboundary<1, DIM-1, typename gsse::property_complex<DataStructure>::CellTopology>   type;
   typedef typename gsse::result_of::coboundary<1, DIM-1, typename gsse::property_complex<DataStructure>::CellTopology>::type   result;
};

template<typename DataStructure>
struct cell_on_edge<gsse::property_space<DataStructure> >
{
   static const long DIM = gsse::property_space<DataStructure>::DIMT;

    typedef gsse::segment_coboundary< 
       typename gsse::result_of::val<typename gsse::property_space<DataStructure>::SubSpace_CellComplex>::type
       ,1 ,DIM-1 , typename gsse::property_space<DataStructure>::CellTopology>  type;
    typedef typename gsse::result_of::coboundary<1, DIM-1, typename gsse::property_space<DataStructure>::CellTopology>::type   result;
};

// #############################################
// ############ facet on edge ##################
// ###
template<typename T1, typename T2 = void>
struct facet_on_edge
{};
template<long DIMT, typename CellTopologyT>
struct facet_on_edge<gsse::property_data<DIMT, CellTopologyT> >
{
   // DIMT-1 -> relative coboundary dimension
   //
   typedef gsse::coboundary<1, 1, CellTopologyT>  type;
   typedef typename gsse::result_of::coboundary<1, 1, CellTopologyT>::type   result;
};

template<typename DataStructure>
struct facet_on_edge<gsse::property_complex<DataStructure> >
{
   typedef gsse::coboundary<1, 1, typename gsse::property_complex<DataStructure>::CellTopology>   type;
   typedef typename gsse::result_of::coboundary<1, 1, typename gsse::property_complex<DataStructure>::CellTopology>::type   result;
};

template<typename DataStructure>
struct facet_on_edge<gsse::property_space<DataStructure> >
{
   typedef gsse::segment_coboundary< 
      typename gsse::result_of::val<typename gsse::property_space<DataStructure>::SubSpace_CellComplex>::type
      , 1 ,1, typename gsse::property_space<DataStructure>::CellTopology>  type;
   typedef typename gsse::result_of::coboundary<1, 1, typename gsse::property_space<DataStructure>::CellTopology>::type   result;
};

// ############################################
// ############ f0 on edge ####################
// ###
template<typename T1, typename T2 = void>
struct f0_on_edge
{};
template<long DIMT, typename CellTopologyT>
struct f0_on_edge<gsse::property_data<DIMT, CellTopologyT> >
{
   typedef gsse::coboundary<1, 1, CellTopologyT>  type;
   typedef typename gsse::result_of::coboundary<1, 1, CellTopologyT>::type   result;
};

template<typename DataStructure>
struct f0_on_edge<gsse::property_complex<DataStructure> >
{
   typedef gsse::coboundary<1, 1, typename gsse::property_complex<DataStructure>::CellTopology>   type;
   typedef typename gsse::result_of::coboundary<1, 1, typename gsse::property_complex<DataStructure>::CellTopology>::type   result;
};

template<typename DataStructure>
struct f0_on_edge<gsse::property_space<DataStructure> >
{
   typedef gsse::segment_coboundary< 
      typename gsse::result_of::val<typename gsse::property_space<DataStructure>::SubSpace_CellComplex>::type
      , 1 ,1, typename gsse::property_space<DataStructure>::CellTopology>  type;
   typedef typename gsse::result_of::coboundary<1, 1, typename gsse::property_space<DataStructure>::CellTopology>::type   result;
};

// ############################################
// ############ f1 on edge ####################
// ###
template<typename T1, typename T2 = void>
struct f1_on_edge
{};
template<long DIMT, typename CellTopologyT>
struct f1_on_edge<gsse::property_data<DIMT, CellTopologyT> >
{
  // [RH][TODO] 
  //  actually we can test if DIMT < 1+2 .. if yes, then warning/compiler message

   typedef gsse::coboundary<1, 2, CellTopologyT>  type;
   typedef typename gsse::result_of::coboundary<1, 2, CellTopologyT>::type   result;
};

template<typename DataStructure>
struct f1_on_edge<gsse::property_complex<DataStructure> >
{
   typedef gsse::coboundary<1, 2, typename gsse::property_complex<DataStructure>::CellTopology>   type;
   typedef typename gsse::result_of::coboundary<1, 2, typename gsse::property_complex<DataStructure>::CellTopology>::type   result;
};

template<typename DataStructure>
struct f1_on_edge<gsse::property_space<DataStructure> >
{
   typedef gsse::segment_coboundary< 
      typename gsse::result_of::val<typename gsse::property_space<DataStructure>::SubSpace_CellComplex>::type
      , 1, 2, typename gsse::property_space<DataStructure>::CellTopology>  type;
   typedef typename gsse::result_of::coboundary<1, 2, typename gsse::property_space<DataStructure>::CellTopology>::type   result;
};


// ############################################
// ############ cell on facet ##################
// ###
template<typename T1, typename T2 = void>
struct cell_on_facet
{};
template<long DIMT, typename CellTopologyT>
struct cell_on_facet<gsse::property_data<DIMT, CellTopologyT> >
{
   // DIMT-1 -> relative coboundary dimension
   //
   typedef gsse::coboundary<2, DIMT-2, CellTopologyT>  type;
   typedef typename gsse::result_of::coboundary<2, DIMT-2, CellTopologyT>::type   result;
};

template<typename DataStructure>
struct cell_on_facet<gsse::property_complex<DataStructure> >
{
   static const long DIM = gsse::property_complex<DataStructure>::DIMT;

   typedef gsse::coboundary<2, DIM-2, typename gsse::property_complex<DataStructure>::CellTopology>   type;
   typedef typename gsse::result_of::coboundary<2, DIM-2, typename gsse::property_complex<DataStructure>::CellTopology>::type   result;
};

template<typename DataStructure>
struct cell_on_facet<gsse::property_space<DataStructure> >
{
   static const long DIM = gsse::property_space<DataStructure>::DIMT;

   typedef gsse::segment_coboundary< 
      typename gsse::result_of::val<typename gsse::property_space<DataStructure>::SubSpace_CellComplex>::type
      , 2 ,DIM-2, typename gsse::property_space<DataStructure>::CellTopology>  type;
   typedef typename gsse::result_of::coboundary<2, DIM-2, typename gsse::property_space<DataStructure>::CellTopology>::type   result;
};


// ==============================================

} // namespace result_of
}  // namespace gsse


#endif
