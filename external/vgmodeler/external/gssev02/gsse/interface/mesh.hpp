/* ============================================================================
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at   
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GSSE_MESH_INTERFACE
#define GSSE_MESH_INTERFACE

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/map_fwd.hpp>

#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/include/at_key.hpp>

#include <boost/array.hpp>

#include <boost/fusion/container/map/map.hpp>
#include <boost/fusion/container/generation/make_map.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>
#include <boost/fusion/sequence/intrinsic/has_key.hpp>
#include <boost/fusion/sequence/intrinsic/begin.hpp>
#include <boost/fusion/sequence/io/out.hpp>
#include <boost/fusion/iterator/key_of.hpp>
#include <boost/fusion/iterator/deref_data.hpp>
#include <boost/fusion/iterator/value_of_data.hpp>
#include <boost/fusion/iterator/next.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/fusion/support/category_of.hpp>
#include <boost/fusion/iterator/equal_to.hpp>
#include <boost/fusion/include/equal_to.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/assert.hpp>
#include <iostream>
#include <string>
#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/any.hpp>
#include <boost/mpl/filter_view.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/not_equal_to.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/fusion/iterator/deref.hpp>
#include <boost/fusion/include/deref.hpp>
#include <boost/fusion/include/size.hpp>

#include "gsse/datastructure/space.hpp"
#include "gsse/datastructure/utils.hpp"
#include "gsse/topology/coboundary.hpp"
#include "gsse/topology/boundary.hpp"
#include "gsse/util/at_complex.hpp"
#include "gsse/algorithm/mesh/keys.hpp"
#include "gsse/util/timer.hpp"
#include "gsse/util/logger.hpp"

#include "strategy.hpp"
#include "interfaces/triangle.hpp"
#include "interfaces/tetgen.hpp"
#include "interfaces/cgal.hpp"

using namespace gsse::access_specifier; 

using boost::fusion::pair;
using boost::fusion::make_pair;

namespace gsse { 
   namespace mesh {

      struct strategy_type        {};
      struct local_feature_size   {};
      struct direct_type          {};
      struct ERROR_the_requested_algorithm_is_not_supported {};            
        
      namespace result_of {
         
         template<typename SearchType>
         struct meshing_strategy_check
         {
            template<typename ItemType>
            struct apply
            {
               typedef typename boost::is_same<SearchType, typename ItemType::strategy_type>::type type;
            };
         };   
         
         template<typename SearchType>
         struct topology_input_dimension_check
         {
            template<typename ItemType>
            struct apply
            {
               typedef typename boost::mpl::equal_to<SearchType, boost::mpl::int_<ItemType::topological_input_dimension_tag> >::type type;
            };
         };   
         
         template<typename SearchType>
         struct geometry_input_dimension_check
         {
            template<typename ItemType>
            struct apply
            {
               typedef typename boost::mpl::equal_to<SearchType, boost::mpl::int_<ItemType::geometrical_input_dimension_tag> >::type type;
            };
         };   
         
         struct dothefold
         {
            struct fold_op
            {
               template <typename Sig> struct result;
               template <class S, class State, class Element> 
               struct result< S(State &,Element &) > 
               {
                  typedef typename boost::mpl::filter_view< State, Element >::type  type;
               };
            };
            
            template <typename Seq, typename State>
            struct apply : boost::fusion::result_of::fold<Seq, State, fold_op>::type { };
         };
                  
         template<typename StrategyT, typename InputSpaceT, typename OutputSpaceT>
         struct eval
         {
            typedef  gsse::property_space<InputSpaceT>              space_property_type;
            static const int topology_input_dimension = gsse::result_of::property_DIMT<space_property_type>::value;
            static const int geometry_input_dimension = gsse::result_of::property_DIMG<space_property_type>::value;
            
            typedef typename boost::fusion::result_of::value_at_key<StrategyT, gsse::mesh::strategy_type>::type  strategy_t; 
            typedef typename boost::fusion::result_of::value_at_key<StrategyT, gsse::mesh::local_feature_size>::type lfs_t; 

            // [TODO] check if a certain type is included 
            //        why: it should be possible to diretly address the mesher, therefore set the direct_type
            //             and only this mesher will be chosen
            //
//         boost::fusion::result_of::has_key<Seq, Key>::type
//         typedef typename boost::fusion::result_of::value_at_key<StrategyT, gsse::mesh::direct_type>::type direct_t; 

            
            // [INFO] the algorithms need to be registered here
            typedef boost::fusion::vector<triangle_wrapper, 
                                          tetgen_wrapper, 
                                          cgal2d_wrapper, 
                                          cgal3d_wrapper>  algorithm_sequence;
            
            // [INFO] register the checking predicates
            typedef boost::fusion::vector<meshing_strategy_check<strategy_t>, 
                                          topology_input_dimension_check<boost::mpl::int_<topology_input_dimension> >,
                                          geometry_input_dimension_check<boost::mpl::int_<geometry_input_dimension> > >       
            predicate_sequence;
            
            // [INFO] apply the predicates onto the algorithms
            typedef typename dothefold::apply< predicate_sequence, algorithm_sequence >::type          fold_result;
            
            // [INFO] check if the result has elements -> if empty notify user, that his setup is not implemented
            static const int var = boost::fusion::result_of::size<fold_result>::type::value;
            typedef boost::mpl::not_equal_to<boost::mpl::int_<var>, boost::mpl::int_<0> > debug;
            
            
            BOOST_MPL_ASSERT_MSG(debug::value, ERROR_the_requested_algorithm_is_not_supported, (strategy_t) );   
            
            
            // [INFO] we always use the first functor of the sequence
            // [TOOD] maybe this can be improved ?
            typedef typename boost::fusion::result_of::begin<fold_result>::type                        first_fold_result;
            typedef typename boost::fusion::result_of::deref<first_fold_result>::type                  algorithm_type;

//         typedef typename algorithm_type::result_type                                               algorithm_result_type;
            
            // [INFO] this is our generic approach for a result type 
//          typedef boost::fusion::map<boost::fusion::pair<result_tag,algorithm_result_type>, 
//                                     boost::fusion::pair<error_tag, std::string>, 
//                                     boost::fusion::pair<info_tag, std::string> >                                 result_type;

            typedef  algorithm_type type;
         };
         
      } // end namespace: result_of


      template<typename StrategyT, typename InputSpaceT, typename OutputSpaceT>
      void generate(InputSpaceT& input_space, OutputSpaceT& output_space, std::ostream& output = std::cout)
      {         
         typedef typename gsse::mesh::result_of::eval<StrategyT, InputSpaceT, OutputSpaceT>::type mesher_type;
                 
         typename gsse::timer<gsse::boost_timer>::type  timer;
         gsse::restart(timer);
        
         mesher_type()(input_space, output_space);                                           

         GSSELOGGER( gsse::logger::blue ) << "\t## meshing time: " << gsse::elapsed(timer) << "\n";
      }


      template<typename InputSpaceT, typename OutputSpaceT>
      void generate(InputSpaceT& input_space, OutputSpaceT& output_space, std::ostream& output = std::cout)
      {         
         typedef typename boost::fusion::map<
            pair<gsse::mesh::strategy_type, gsse::mesh::strategy::conforming_delaunay>, 
            pair<gsse::mesh::local_feature_size, boost::mpl::int_<3> > > StrategyT;

         typedef typename gsse::mesh::result_of::eval<StrategyT, InputSpaceT, OutputSpaceT>::type mesher_type;
                 
         typename gsse::timer<gsse::boost_timer>::type  timer;
         gsse::restart(timer);
        
         mesher_type()(input_space, output_space);                                           

         GSSELOGGER( gsse::logger::blue ) << "\t## meshing time: " << gsse::elapsed(timer) << "\n";
      }
            
   } // end namespace: mesh      
} // end namespace: gsse

#endif
