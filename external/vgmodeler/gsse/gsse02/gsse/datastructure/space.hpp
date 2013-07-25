/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_CONTAINER_SPACE_HH)
#define GSSE_CONTAINER_SPACE_HH

// *** system includes
#include <iomanip>
// *** BOOST includes
#include <boost/spirit/include/phoenix_core.hpp>
// *** GSSE includes
#include "gsse/datastructure/complex.hpp"
#include "gsse/datastructure/utils.hpp"
#include "gsse/traversal/decode.hpp"
#include "gsse/util.hpp"
#include "gsse/algorithm.hpp"

// ############################################################
//
namespace IO
{
namespace
{
   std::string set_open ("{");
   std::string set_close("}");
   std::string delimiter_space("\t");
   std::string sep(" ");
   std::string data_follows("DataFollows");
   std::string data_collect_parts("DataCollectParts");
   std::string data_part("DataPart");
   std::string type("Type");
   std::string val_type("ValueType");
   std::string dimension("Dimension");
   std::string dim_arbitrary("DimArbitrary");
   std::string cardinality("Cardinality");
   std::string name("Name");
   std::string number("Number");
   std::string object_dimension("ObjectDimension");
   std::string object_type("ObjectType");
   std::string storage_format("StorageFormat");
   std::string gsse_vxcl("VertexOnCell");
   std::string gsse_clvx("CellOnVertex");
   std::string gsse_cell("Cell");
   std::string gsse_vertex("Vertex");
   std::string gsse_simplex("Simplex");
   std::string gsse_indexed("Indexed");
   std::string gsse_index("Index");

   // gsse tokens
   std::string gsse_coordinates("Coordinates");
   std::string gsse_numeric_coord("double");
   std::string gsse_numeric_quan ("double");
   std::string gsse_int("Int");
   std::string gsse_space("PhysicalSpace");
   std::string gsse_segment("GSSESegment");
   std::string gsse_cellcomplex("CellComplex");
   std::string gsse_quancomplex("QuantityComplex");
}
}  //namespace IO

// ###############################################################################################################################

namespace gsse {
namespace space {

struct create
{
   template<long DIMGeo, long DIMTop, long DIMQuan
            , typename CellTopology    = gsse::cell_nothing
            , typename StorageTypeGeo  = double
            , typename StorageTypeCell = long
            , typename StorageTypeQuan = double
            >
   struct apply
   {   
      typedef gsse::array<StorageTypeGeo, DIMGeo>                  CoordPosT;
      typedef typename gsse::container::create< CoordPosT >::type  FBPosT;  

      typedef typename gsse::cellcomplex::create::template apply<DIMTop,  CellTopology, StorageTypeCell>::type    CellComplex;
      typedef typename gsse::quancomplex::create::template apply<DIMQuan, CellTopology, StorageTypeQuan>::type    QuanComplex;

      typedef typename gsse::container::create_index_dense< CellComplex, std::vector, std::string >::type   SubSpace_CellComplex;
      typedef typename gsse::container::create_index_dense< QuanComplex, std::vector, std::string >::type   SubSpace_QuanComplex;

      typedef boost::fusion::map<
           boost::fusion::pair<gsse::access_specifier::AP, FBPosT>
         , boost::fusion::pair<gsse::access_specifier::AC, SubSpace_CellComplex>
         , boost::fusion::pair<gsse::access_specifier::AQ, SubSpace_QuanComplex>
      > type;  
   };
};
}  // namespace space


// ##############################################################################################################################


template<typename Space>
void init_topology_in_space(Space& space)
{
   typedef gsse::property_space<Space>  gssespace_type;

   // ##########
   typedef typename gsse::result_of::property_SubSpaceCC<gssespace_type>::type  subspace_cc_type;
   typedef typename gsse::result_of::property_CellComplex<gssespace_type>::type cellcomplex_type;

   subspace_cc_type& ss_cc = gsse::at_dim<gsse::AC>(space);

   typedef typename gsse::result_of::property_CellT<gssespace_type>::type   cell_type;

   size_t size_subspace=2;
   gsse::resize(size_subspace)(ss_cc);
   
   std::string name_segments;
   for (size_t cnt_ss = 0; cnt_ss < size_subspace; ++cnt_ss)
   {
      std::string name_subspace = "name";

      gsse::at_index(gsse::at(cnt_ss)(ss_cc)) = name_subspace;
      
      
      // ===========================================
      // subspace:: cell complex
      size_t size_ss_cells=12;
      for (size_t i = 0 ; i < size_ss_cells; ++i)
      {
         cell_type cell;
         gsse::algorithm::init(cell,  gsse::algorithm::inc( size_ss_cells * cnt_ss + i) );  
         gsse::pushback(gsse::at_dim<gsse::AT_cl>(  gsse::at_fiber(gsse::at(cnt_ss)(ss_cc)))) = cell;

         // cell on vertex information
         //   
         for (size_t inside_i = 0; inside_i < gsse::size(cell); ++inside_i)
         {
            gsse::pushback( 
               gsse::at_fiber(
                  gsse::at(  
                     gsse::at(inside_i)(cell))( 
                        gsse::at_dim<gsse::AT_vx>( 
                           gsse::at_fiber(gsse::at(cnt_ss)(ss_cc))
                           ) ) ) )= i;   // gsse::at is always safe
         }
      }  // subspace cells

      for (size_t cnt_vx = 0; cnt_vx < gsse::size( gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(cnt_ss)(ss_cc) ))); ++cnt_vx)
      {
         // detects automatically is an index space is available
         //  if there is none -> a gsse::identity object is returned
         //
         gsse::at_index (
                          gsse::at(cnt_vx)( 
                                            gsse::at_dim<AT_vx>( gsse::at_fiber(gsse::at(cnt_ss)(ss_cc)) ) 
                                           ) 
                          ) = cnt_vx;  
      }

   }   // subspace traversal
}


template<typename Space>
void init_space(Space& space)
{
   typedef gsse::property_space<Space>  gssespace_type;

   // ##########
   typedef typename gsse::result_of::property_Geometry<gssespace_type>::type    geometry_type;
   typedef typename gsse::result_of::property_SubSpaceCC<gssespace_type>::type  subspace_cc_type;
   typedef typename gsse::result_of::property_SubSpaceQC<gssespace_type>::type  subspace_qc_type;

   typedef typename gsse::result_of::property_CellComplex<gssespace_type>::type cellcomplex_type;
   typedef typename gsse::result_of::property_QuanComplex<gssespace_type>::type quancomplex_type;

   subspace_cc_type& ss_cc = gsse::at_dim<gsse::AC>(space);
   subspace_qc_type& ss_qc = gsse::at_dim<gsse::AQ>(space);
   geometry_type&    geom  = gsse::at_dim<gsse::AP>(space);

   typedef typename gsse::result_of::property_CoordT<gssespace_type>::type  coord_type;
   typedef typename gsse::result_of::property_CellT<gssespace_type>::type   cell_type;
   typedef typename gsse::result_of::property_QuanT<gssespace_type>::type   quan_type;

//    dump_type<coord_type>();
//    dump_type<cell_type>();
//    dump_type<quan_type>();
   
   // ####################

   // ===========================================
   // coordinate space (geometry)
   //
   size_t size_vertices = 12;
   for (size_t i =0 ; i < size_vertices; ++i)
   {
      coord_type coord;
      //      gsse::algorithm::init(coord,  gsse::algorithm::inc(i*2+1) );  
      gsse::algorithm::init(coord,  gsse::algorithm::rand(1+i+1./(i*2+1),1+i*2+1./(i+1)) );  
      gsse::at(i)(geom) = coord;
   }
   

   size_t size_subspace=2;
   gsse::resize(size_subspace)(ss_cc);
   gsse::resize(size_subspace)(ss_qc);
   
   std::string name_segments;
   for (size_t cnt_ss = 0; cnt_ss < size_subspace; ++cnt_ss)
   {
      std::string name_subspace = "name";

      gsse::at_index(gsse::at(cnt_ss)(ss_cc)) = name_subspace;
      gsse::at_index(gsse::at(cnt_ss)(ss_qc)) = name_subspace;   
      
      
      // ===========================================
      // subspace:: cell complex
      size_t size_ss_cells=12;
      for (size_t i = 0 ; i < size_ss_cells; ++i)
      {
         cell_type cell;
         gsse::algorithm::init(cell,  gsse::algorithm::inc( size_ss_cells * cnt_ss + i) );  
         gsse::pushback(gsse::at_dim<gsse::AT_cl>(  gsse::at_fiber(gsse::at(cnt_ss)(ss_cc)))) = cell;

         // cell on vertex information
         //   
         for (size_t inside_i = 0; inside_i < gsse::size(cell); ++inside_i)
         {

	   //	   dump_type<subspace_cc_type>();
	   // TODO
	   //            gsse::pushback( gsse::at(  gsse::at(inside_i)(cell))( gsse::at_dim<gsse::AT_vx>( gsse::at_fiber(gsse::at(cnt_ss)(ss_cc)) ) ) ) = i;   // gsse::at is always safe
         }
      }  // subspace cells

      
      // ===========================================
      // subspace:: quantity complex
      {
         std::string quan_dimension = "vertex";
         size_t nb_quantities = 1;
         
         for (size_t nb_q = 0; nb_q < nb_quantities; ++nb_q)
         {
            std::string quan_name = "testquan_vx";
            size_t quan_size        = 1;
            
            for (size_t i = 0; i < quan_size; ++i)
            {
               long cell_vx;
               cell_vx = i;
           
               if (quan_dimension == "vertex")
               {
                  quan_type quan;   // 3-vector
                  gsse::algorithm::init(quan,  gsse::algorithm::inc(i) );  
	
		   gsse::at(cell_vx)(gsse::at(quan_name)(gsse::at_dim<gsse::AT_vx>(  gsse::at_fiber(gsse::at(cnt_ss)(ss_qc))))) = quan;

		//  gsse::at(cell_vx)(gsse::at(quan_name)(gsse::at_dim<gsse::AT_vx>(  gsse::at_fiber(gsse::at(cnt_ss)(ss_qc))))) = quan;
//                  gsse::at(cell_vx)(gsse::at_vx(quan_name)( gsse::at(cnt_ss)(ss_qc))) = quan;
// [RH][info] ..[TODO] .. for now, cannot be used to  const-issue of std::map
               }
            } // quan size
         }  //nb quantitites
      } // quantity complex

      {
         std::string quan_dimension = "cell";
         size_t nb_quantities = 1;
         
         for (size_t nb_q = 0; nb_q < nb_quantities; ++nb_q)
         {
            std::string quan_name = "testquan_cl";
            size_t quan_size        = 3;
            
            for (size_t i = 0; i < quan_size; ++i)
            {
               cell_type cell ;
               gsse::algorithm::init(cell,  gsse::algorithm::inc( size_ss_cells * cnt_ss + i) );  
          
               if (quan_dimension == "cell")
               {
                  quan_type quan;
                  gsse::algorithm::init(quan,  gsse::algorithm::inc(i) );  
		  //                  gsse::print_1d(quan);
                  
		  // [RH][TODO] .. resolve access issue .. operator[]
		  //                  gsse::at(cell)(gsse::at(quan_name)(gsse::at_dim<gsse::AT_cl>(  gsse::at_fiber(gsse::at(cnt_ss)(ss_qc))))) = quan;
               }
            } // quan size
         }  //nb quantitites
      } // quantity complex

   }   // subspace traversal
}


template<typename Space>
void print_space(Space& space)
{
  using namespace boost::phoenix;
  using namespace boost::phoenix::arg_names;

  // ### geometry
  //
  std::cout << "3" << std::endl;
  std::cout << gsse::size( gsse::at_dim<gsse::AP>(space) ) << std::endl;

  std::cout.setf( std::ios::right,std::ios::adjustfield);
  std::cout << std::setprecision(12)  << std::setiosflags(std::ios::fixed);
  
  gsse::print( gsse::at_dim<gsse::AP>(space) );



  std::cout << gsse::size (gsse::at_dim<gsse::AC>(space)) << std::endl;
  for (size_t cnt_ss = 0; cnt_ss < gsse::size(gsse::at_dim<gsse::AC>(space)); ++cnt_ss)
    {
      std::cout << gsse::at_index( gsse::at(cnt_ss) (gsse::at_dim<gsse::AC>(space)) ) << std::endl;
      std::cout << gsse::size (gsse::at_dim<gsse::AT_cl>(gsse::at_fiber(gsse::at_dim<gsse::AC>(space)[cnt_ss]))  ) << std::endl;
      
      long cnt_cl = 0;


      // ### cell complex
      //
      gsse::traverse<gsse::AT_cl>()
      [
	 std::cout << (boost::ref(cnt_cl)) << val("\t  ")
	 , boost::ref(cnt_cl)++
	 ,
	 gsse::traverse()
	 [
	  std::cout << boost::phoenix::arg_names::_1 << val("  ")
         ]
	 , std::cout << val("") << std::endl
      ]( gsse::at_fiber (gsse::at(cnt_ss) (gsse::at_dim<gsse::AC>(space)) ));



      // ### quan complex
      //
      std::cout << "Quantity: " << "vertex " << 
	gsse::size (gsse::at_dim<gsse::AT_vx>(gsse::at_fiber(gsse::at(cnt_ss)(gsse::at_dim<gsse::AQ>(space))))) << std::endl;
      
       gsse::traverse<gsse::AT_vx>()
       [
         std::cout << val("") << gsse::acc0 << std::endl   // ## quantity name
         ,
         std::cout << gsse::asize1 << std::endl
         ,
         gsse::traverse()
         [
            std::cout << gsse::acc << val("\t ")
            ,
            gsse::traverse() // for multiple values
            [
               std::cout << val(" ") << boost::phoenix::arg_names::_1 
            ]
            ,
            std::cout << val("") << std::endl
         ]
       ]( gsse::at_fiber (gsse::at(cnt_ss) (gsse::at_dim<gsse::AQ>(space)) ));


//       std::cout << "Quantity: " << "cell " << 
//          gsse::size (gsse::at_dim<gsse::AT_cl>(gsse::at_fiber(gsse::at(cnt_ss)(gsse::at_dim<gsse::AQ>(space))))) << std::endl;
      
//       gsse::traverse<gsse::AT_cl>()
//       [
//          std::cout << val("") << gsse::acc0 << std::endl   // ## quantity name
//          ,
//          std::cout << gsse::asize1 << std::endl
//          ,
//          gsse::traverse()
//          [
//             std::cout << gsse::acc << val("\t ")
//             ,
//             gsse::traverse() // for multiple values
//             [
//                std::cout << val(" ") << _1 
//             ]
//             ,
//             std::cout << val("") << std::endl
//          ]
//       ]( gsse::at_fiber (gsse::at(cnt_ss) (gsse::at_dim<gsse::AQ>(space)) ));

    }
}



// ######################################################################################################################


template<typename ContainerT, typename IndexT>
boost::phoenix::actor<gsse::quan_access<ContainerT> >
aq(ContainerT& container, IndexT& index)
{
   return  boost::phoenix::actor<gsse::quan_access<ContainerT> >(gsse::quan_access<ContainerT>(container, index));
}


template<typename ContainerT, typename IndexT>
struct aq_creator
{
   typedef boost::phoenix::actor<gsse::quan_access<ContainerT> > type;
};


// ##########
template<typename ContainerT, typename IndexT>
boost::phoenix::actor<gsse::quan_access_4<ContainerT> >
aq2(ContainerT& container, IndexT& index)
{
   return  boost::phoenix::actor<gsse::quan_access_4<ContainerT> >(gsse::quan_access_4<ContainerT>(container, index));
}

template<typename ContainerT, typename IndexT>
struct aq_creator2
{
   typedef boost::phoenix::actor<gsse::quan_access_4<ContainerT> > type;
};



// #############################################################################################################
//   gssev02 :: inter-dimensional object creation algorithms
// #############################################################################################################
//
template<typename Space>
void create_edges(Space& space)
{
   typedef  gsse::property_space<Space>     space_property_type;
   typedef typename gsse::result_of::property_SubSpaceCC<space_property_type>::type    subspace_cc_type;
   typedef typename gsse::result_of::property_CellComplex<space_property_type>::type   cellcomplex_type;
   typedef typename gsse::result_of::property_CellTopology<space_property_type>::type  celltopology_type;

   static const long DIM = gsse::result_of::property_DIMT<space_property_type>::value;

// -----------------------
   subspace_cc_type& ss_cc = gsse::at_dim<gsse::AC>(space);
   
   // boundary operator
   //
   //    DIM-1 .. calculate relative edge dimensions
   //
   typedef gsse::boundary<DIM, DIM-1 , gsse::cell_simplex> Bnd_sD_1;
   Bnd_sD_1 bnd_sD_1;
   typedef typename gsse::result_of::boundary<DIM, DIM-1, celltopology_type>::type::value_type  EdgeTypes;
   

// // ##########

   for (size_t cnt_ss_cc = 0; cnt_ss_cc < gsse::size (ss_cc ); ++cnt_ss_cc)
   {
      std::set< EdgeTypes >                    container_edges;
      std::map< EdgeTypes, bool >              edge_uniquer;
      typename std::set< EdgeTypes >::iterator ci_it;
      
      for ( size_t cnt_cell = 0; cnt_cell < gsse::size(gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(cnt_ss_cc) (ss_cc))))); ++cnt_cell)
      {
         typename gsse::result_of::boundary<DIM, DIM-1, celltopology_type>::type
            cont_result = bnd_sD_1( gsse::at(cnt_cell) (gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(cnt_ss_cc) (ss_cc)))) ));
         

         for (size_t ce_i = 0; ce_i < gsse::size(cont_result); ++ce_i)
         {
            // check if edge has already been added to the edge container
            EdgeTypes edge = cont_result[ce_i];
            // the orientation here ensures uniqueness of the edges within the container
            std::sort(edge.begin(), edge.end());
            // if it is a new edge, add the oriignal oriented edge ..
            if(!edge_uniquer[edge])
            { 
               container_edges.insert ( cont_result[ce_i] );
               edge_uniquer[edge]=true;            
            }
         }
      }
      
      for (ci_it = container_edges.begin();
           ci_it != container_edges.end(); 
           ++ci_it)
      {
         gsse::pushback (gsse::at_dim<AT_ee>( gsse::at_fiber((gsse::at(cnt_ss_cc) (ss_cc))))) = (*ci_it);
      }
//       std::cout << "# all edges: " << std::endl;
//       gsse::print_1d( gsse::at_dim<AT_ee>( gsse::at_fiber((gsse::at(cnt_ss_cc) (ss_cc)))) );   // [TODO] for sets
    }
}



template<typename Space>
void create_edges_nonoriented(Space& space)
{
   typedef gsse::property_space<Space>     space_property_type;
   typedef typename gsse::result_of::property_SubSpaceCC<space_property_type>::type    subspace_cc_type;
   typedef typename gsse::result_of::property_CellComplex<space_property_type>::type   cellcomplex_type;
   typedef typename gsse::result_of::property_CellTopology<space_property_type>::type  celltopology_type;

   static const long DIM = gsse::result_of::property_DIMT<space_property_type>::value;

// -----------------------
   subspace_cc_type& ss_cc = gsse::at_dim<gsse::AC>(space);
   
   // boundary operator
   //
   //    DIM-1 .. calculate relative edge dimensions
   //
   typedef gsse::boundary<DIM, DIM-1 , gsse::cell_simplex> Bnd_sD_1;
   Bnd_sD_1 bnd_sD_1;
   typedef typename gsse::result_of::boundary<DIM, DIM-1, celltopology_type>::type::value_type  EdgeTypes;
   

// // ##########

   for (size_t cnt_ss_cc = 0; cnt_ss_cc < gsse::size (ss_cc ); ++cnt_ss_cc)
   {
      std::set< EdgeTypes >                    container_edges;
      typename std::set< EdgeTypes >::iterator ci_it;
      
      for (size_t cnt_cell = 0; cnt_cell < gsse::size(gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(cnt_ss_cc) (ss_cc))))); ++cnt_cell)
      {
         typename gsse::result_of::boundary<DIM, DIM-1, celltopology_type>::type
	   cont_result = bnd_sD_1( gsse::at(cnt_cell) (gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(cnt_ss_cc) (ss_cc)))) ));
         

         for (size_t ce_i = 0; ce_i < gsse::size(cont_result); ++ce_i)
         {
	   // ### swap vertex (on edge) if order is (<)
	   //
	   if (cont_result[ce_i][0] > cont_result[ce_i][1])
           {
	     std::swap ( cont_result[ce_i][0], cont_result[ce_i][1] );
	   }
	   container_edges.insert ( cont_result[ce_i] );
	 }
      }
      
      for (ci_it = container_edges.begin();
           ci_it != container_edges.end(); 
           ++ci_it)
      {
         gsse::pushback (gsse::at_dim<AT_ee>( gsse::at_fiber((gsse::at(cnt_ss_cc) (ss_cc))))) = (*ci_it);
      }
      std::cout << "# all edges: " << std::endl;
      gsse::print_1d( gsse::at_dim<AT_ee>( gsse::at_fiber((gsse::at(cnt_ss_cc) (ss_cc)))) );   // [TODO] for sets
    }
}


// #########################

template < typename CellTopology 
           , long DIMGeo 
           , long DIMTop  = DIMGeo
           , long DIMQuan = DIMGeo
           , typename StoragePosT  = double
           , typename StorageCellT = long
           , typename StorageQuanT = double >
struct create_space
{
   typedef gsse::array<StoragePosT, DIMGeo>                  CoordPosT;
   typedef typename gsse::container::create< CoordPosT >::type  FBPosT;  
   
   typedef typename gsse::cellcomplex::create::template apply<DIMTop,  CellTopology, StorageCellT>::type    CellComplex;
   typedef typename gsse::quancomplex::create::template apply<DIMQuan, CellTopology, StorageQuanT>::type    QuanComplex;
   
   typedef typename gsse::container::create_index_dense< CellComplex, std::vector, std::string >::type   SubSpace_CellComplex;
   typedef typename gsse::container::create_index_dense< QuanComplex, std::vector, std::string >::type   SubSpace_QuanComplex;
   
   typedef boost::fusion::map<
        boost::fusion::pair<gsse::access_specifier::AP, FBPosT>
      , boost::fusion::pair<gsse::access_specifier::AC, SubSpace_CellComplex>
      , boost::fusion::pair<gsse::access_specifier::AQ, SubSpace_QuanComplex>
   > type;  
};



}  // namespace gsse



#endif
