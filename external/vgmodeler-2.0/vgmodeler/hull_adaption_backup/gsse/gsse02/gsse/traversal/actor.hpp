/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2008-2009 Philipp Schwaha                      philipp@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_TRAVERSAL_ACTOR_HH)
#define GSSE_TRAVERSAL_ACTOR_HH

// *** system includes
// *** BOOST includes
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_scope.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>   // necessary for operator,
#include <boost/spirit/include/phoenix_scope.hpp>       // local scopes
#include <boost/fusion/sequence.hpp>

// *** GSSE includes
#include "gsse/util/common.hpp"
#include "gsse/util/access.hpp"
#include "gsse/util/tag_of.hpp"
#include "gsse/traversal/decode.hpp"
//#include "gsse/topology/coboundary.hpp"


// ############################################################
//

namespace gsse{
namespace traversal{

// forward specification for boundary functor
//
template<long NumberT>
struct relative_dimension_minus: public boost::mpl::int_<NumberT>
{ };

template<long NumberT>
struct relative_dimension_plus: public boost::mpl::int_<NumberT>
{ };


// [RH][info][debug part]
// template <typename DimensionPack, typename Enable=void>   
// struct traversal_eval
// {
//    template <typename Env, typename EvalObject, typename NumericT>
//    struct result
//    {
//       typedef void type;
//    };

//    template <typename RT, typename Env, typename EvalObject, typename NumericT>
//    static typename result<Env,  EvalObject, NumericT>::type
//    eval(Env const& env, EvalObject& calc_object, NumericT numeric)
//    {
//       dump_type<typename DimensionPack::access_mechanism>();
//    }
// };


namespace detail {

   // these function(object)s are needed because the environment changes
   // if we use accumulations which store the base elements as scope keys. If we
   // pass a non-scoped environment we can pass it as is. 
   //
   template <typename Env, typename NewEnv>
   struct change_environment_impl
   {
      typedef NewEnv type;
      
      type operator()(Env const& env, NewEnv const& new_env) const 
      {
         return new_env;
      }
   };
   

   // Otherwise we have to Include the new environment in the old in
   // order to keep all the scoped variable keys valid. 
   //
   template <typename Env, typename NewEnv, typename Locals, typename Map>
   struct change_environment_impl< boost::phoenix::scoped_environment<Env, Env, Locals, Map>, NewEnv>
   {
      typedef boost::phoenix::scoped_environment<Env, Env, Locals, Map> old_env;
      typedef boost::phoenix::scoped_environment<NewEnv, NewEnv, Locals, Map> type;
      
      type operator()(old_env const& env, NewEnv const& new_env) const 
      {
         return type(new_env, new_env, env.locals);
      }
   };


// #####################################################################################################################################
//  the main work traversal function
//   no dispatch
//   hence for all simple container/data structure traversal operations
//
template <typename DimensionPack, typename Enable=void>   
struct traversal_eval
{
   template <typename Env, typename EvalObject, typename NumericT>
   struct result
   {
      typedef void type;
   };
   
   template <typename RT, typename Env, typename EvalObject, typename NumericT>
   static typename result<Env,  EvalObject, NumericT>::type
   eval(Env const& env, EvalObject& calc_object, NumericT numeric) 
   {
//       std::cout << "### global env.. looking for scoped environment.. " << std::endl;
//      dump_type<Env>();

      typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type                  EnvContainer;
      typedef gsse::result_of::extension::decode_traversal<EnvContainer, DimensionPack>    DecodedTraversal;
      
      // [RH][hack] 
      //   have to use different indices for the first call of this eval (index = 0)
      //   and for all other times (index = 1)
      //
      static const long IndexEnvNext = boost::mpl::size<typename Env::args_type>::value - 1;
      typedef typename boost::mpl::at_c<typename Env::args_type, IndexEnvNext>::type       EnvContainerNext;
      
      typedef typename DecodedTraversal::template apply<Env>::TraversalObject              TraversalObject;
      TraversalObject& traversal_object =  DecodedTraversal::template apply<Env>::call(env);   // [RH][debug][path][XX2]

      typedef typename DecodedTraversal::template apply<Env>::TypeInside             TypeInside;
      typedef boost::phoenix::basic_environment< TypeInside, EnvContainerNext >      NewEnvironment; 

      typedef typename change_environment_impl<Env, NewEnvironment>::type EnvToPass;
     
// [RH][new] correct const-ness  deduction
//
//      typedef typename TraversalObject::iterator TheIterator;
      typedef typename gsse::result_of::itr<TraversalObject>::type TheIterator;
      TheIterator the_it;
      
//       dump_type<TheIterator>();
//       dump_type<NewEnvironment>();
//       dump_type<TraversalObject>();

      TheIterator ti_begin =  traversal_object.begin();
      TheIterator ti_end   =  traversal_object.end();

      for (the_it  = ti_begin ;
           the_it != ti_end   ;
           ++the_it)
      {
         NewEnvironment env_new ( *the_it, boost::fusion::at_c<IndexEnvNext>(env.args()) );
         EnvToPass env_pass = change_environment_impl<Env, NewEnvironment>()(env, env_new);
         calc_object.eval( env_pass );

//          std::cout << "pass environment" << std::endl;
//          dump_type<EnvToPass>();
      }


   }
};


// ####################
//  the boundary traversal function
//
template<long BoundaryDimension>
struct traversal_eval<relative_dimension_minus<BoundaryDimension> >
{
   template <typename Env, typename EvalObject, typename NumericT>
   struct result
   {
      typedef void type;
   };
   
   
   template <typename RT, typename Env, typename EvalObject, typename NumericT>
   static typename result<Env,  EvalObject, NumericT>::type
   eval(Env const& env, EvalObject& calc_object, NumericT numeric)
   {
      typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type                  EnvContainer;
      typedef typename boost::mpl::at_c<typename Env::args_type, 1>::type                  CellComplexT;

      typedef typename boost::mpl::int_<0>::type DimensionPack;   // [RH][new] direct dimension.. no boundary operation until now !
      typedef gsse::result_of::extension::decode_traversal<EnvContainer, DimensionPack>    DecodedTraversal;

      typedef typename DecodedTraversal::template apply<Env>::TraversalObject              TraversalObject;
      TraversalObject traversal_object =  DecodedTraversal::template apply<Env>::call(env);
      static const long IndexEnvNext =  1;


      // here, the boundary operations start ... 
      //    poset objects are used    
      //
      typename CellComplexT::poset_generator_simplexT& permutation_simplex = boost::fusion::at_c<1>(env.args()).get_permutation_simplex();

//      static const long DIM =                 CellComplexT::DIM_cell+1;                      // DIMension derived from poset    // e.g., for 2D -> 3
      static const long DIM =                 TraversalObject::Size;                           // DIMension derived from poset    // e.g., for 2D -> 3
      static const long DIM_nb_set_elements = DIM-BoundaryDimension;                           // DIM:: number of set elements

      // [RH][TODO]
      //   enable dim_minus<2> -> poset generator
      //
      long DIM_nb_sets                      = permutation_simplex[DIM-BoundaryDimension].size();
      if (DIM == 4)       // simplex 3D
         DIM_nb_sets = 4;
      else if (DIM == 3)
         DIM_nb_sets = 3;
      else if (DIM == 2)
         DIM_nb_sets = 2;
//        std::cout << "size of nb set selements: " << DIM_nb_set_elements << std::endl;
//        std::cout << "size of nb sets:          " << DIM_nb_sets << std::endl;
    

      for (long nb_boundary = 0; nb_boundary < DIM_nb_sets; ++nb_boundary)
      {
         typedef std::pair<long, gsse::array<long, DIM_nb_set_elements  > >temp_containerT;   // [RH][TODO] .. check why a pair /automatic decoding/ is required
         temp_containerT temp_container;
      
         temp_container.first = nb_boundary;   // index facade for fiberbundle concept
         for (long dim_i = 0; dim_i < DIM_nb_set_elements; ++dim_i)
         {
//             std::cout << "  val: " << permutation_simplex[DIM-BoundaryDimension][nb_boundary][ dim_i ] << std::endl;
//            temp_container.second[dim_i] = traversal_object[ permutation_simplex[DIM-BoundaryDimension][nb_boundary][ dim_i ]    ];  // BoundaryDimension

//              std::cout << "array access at: " << dim_i << std::endl;
//              std::cout << "  val: " << permutation_simplex[0][nb_boundary][ dim_i ] << std::endl;
//              std::cout << "   size of traveral object: " << traversal_object.size() << std::endl;
            temp_container.second[dim_i] = traversal_object[ permutation_simplex[0][nb_boundary][ dim_i ]    ];  // BoundaryDimension
         }
//          std::cout << "container: ";
//          for (long dim_i = 0; dim_i < DIM_nb_set_elements; ++dim_i)
//          {
//             std::cout << " " << temp_container.second[dim_i]  ;
//          }
//          std::cout << std::endl;

//         typedef boost::phoenix::basic_environment< temp_containerT >         NewEnvironment; 
         typedef boost::phoenix::basic_environment< temp_containerT, CellComplexT >            NewEnvironment; 


//         NewEnvironment env_new ( temp_container );
         NewEnvironment env_new ( temp_container, boost::fusion::at_c<IndexEnvNext>(env.args()) );

         calc_object.eval( env_new );
      }
   }
};




template<long BoundaryDimension>
struct traversal_eval<relative_dimension_plus<BoundaryDimension> >
{
   template <typename Env, typename EvalObject, typename NumericT>
   struct result
   {
      typedef void type;
   };
   
   
   template <typename RT, typename Env, typename EvalObject, typename NumericT>
   static typename result<Env,  EvalObject, NumericT>::type
   eval(Env const& env, EvalObject& calc_object, NumericT numeric)
   {
      typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type                  EnvContainer;
      typedef typename boost::mpl::at_c<typename Env::args_type, 1>::type                  CellComplexT;
      dump_type<CellComplexT>();

      typedef typename boost::mpl::int_<0>::type DimensionPack;   // [RH][new] direct dimension.. no boundary operation until now !
      typedef gsse::result_of::extension::decode_traversal<EnvContainer, DimensionPack>    DecodedTraversal;

      typedef typename DecodedTraversal::template apply<Env>::TraversalObject              TraversalObject;
      TraversalObject traversal_object =  DecodedTraversal::template apply<Env>::call(env);
//      static const long IndexEnvNext =  1;


      // here, the boundary operations start ... 
      //    poset objects are used    
      //
//       typename CellComplexT::poset_generator_simplexT& permutation_simplex = boost::fusion::at_c<1>(env.args()).get_permutation_simplex();

//       static const long DIM =                 TraversalObject::Size;                           // DIMension derived from poset    // e.g., for 2D -> 3
//       static const long DIM_nb_set_elements = DIM-BoundaryDimension;                           // DIM:: number of set elements

      long DIM_nb_sets                      = 2;

      for (long nb_boundary = 0; nb_boundary < DIM_nb_sets; ++nb_boundary)
      {
//          typedef std::pair<long, gsse::array<long, DIM_nb_set_elements  > >temp_containerT;   // [RH][TODO] .. check why a pair /automatic decoding/ is required
//          temp_containerT temp_container;
      
//          temp_container.first = nb_boundary;   // index facade for fiberbundle concept
//          for (long dim_i = 0; dim_i < DIM_nb_set_elements; ++dim_i)
//          {
// //            temp_container.second[dim_i] = traversal_object[ permutation_simplex[0][nb_boundary][ dim_i ]    ];  // BoundaryDimension
//          }
//         typedef boost::phoenix::basic_environment< temp_containerT, CellComplexT >            NewEnvironment; 

//         NewEnvironment env_new ( temp_container, boost::fusion::at_c<IndexEnvNext>(env.args()) );
//         calc_object.eval( env_new );
      }
   }
};


// ####################
//  new associative key traversal function
//
template <typename DimensionPack>   
struct traversal_eval<DimensionPack, 
	typename boost::enable_if<gsse::is_associative<typename DimensionPack::access_mechanism> >::type >

{
   template <typename Env, typename EvalObject, typename NumericT>
   struct result
   {
      typedef void type;
   };
   
   
   template <typename RT, typename Env, typename EvalObject, typename NumericT>
   static typename result<Env,  EvalObject, NumericT>::type
   eval(Env const& env, EvalObject& calc_object, NumericT numeric)
   {
      static const long IndexEnvNext = boost::mpl::size<typename Env::args_type>::value - 1;

      typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type                       EnvContainer;
      typedef gsse::result_of::extension::decode_traversal_gsse<EnvContainer, DimensionPack>    DecodedTraversal;
      
      // [RH][unfortunate][hack] 
      //   have to use different indices for the first call of this eval (index = 0)
      //   and for all other times (index = 1)
      //
      typedef typename boost::mpl::at_c<typename Env::args_type, IndexEnvNext>::type       EnvContainerNext;
      typedef typename DecodedTraversal::template apply<Env>::TraversalObject              TraversalObject;
      TraversalObject& traversal_object =  DecodedTraversal::template apply<Env>::call(env);

      typedef typename DecodedTraversal::template apply<Env>::TypeInside                   TypeInside;
      typedef boost::phoenix::basic_environment< TypeInside, EnvContainerNext >            NewEnvironment; 

      typedef typename change_environment_impl<Env, NewEnvironment>::type EnvToPass;

      typedef typename TraversalObject::iterator TheIterator;
      TheIterator the_it;


      TheIterator ti_begin =  traversal_object.begin();
      TheIterator ti_end   =  traversal_object.end();

      for (the_it  = ti_begin ;
           the_it != ti_end   ;
           ++the_it)
      {
         NewEnvironment env_new ( *the_it, boost::fusion::at_c<IndexEnvNext>(env.args()) );
         EnvToPass env_pass = change_environment_impl<Env, NewEnvironment>()(env, env_new);
         calc_object.eval( env_pass );

//         calc_object.eval( env_new );
      }

   }
};



// ####################
//  new fiberbundle/base space transfer traversal function
//
template <typename DimensionPack>   
struct traversal_eval<DimensionPack, 
	typename boost::enable_if<gsse::is_transfer<typename DimensionPack::access_mechanism> >::type >

{
   template <typename Env, typename EvalObject, typename NumericT>
   struct result
   {
      typedef void type;
   };
   
   
   template <typename RT, typename Env, typename EvalObject, typename NumericT>
   static typename result<Env,  EvalObject, NumericT>::type
   eval(Env const& env, EvalObject& calc_object, NumericT numeric)
   {
//      std::cout << "in transfer traversal .. " << std::endl;
      static const long IndexEnvNext = boost::mpl::size<typename Env::args_type>::value - 1;

      typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type                  EnvContainer;
      typedef typename boost::mpl::at_c<typename Env::args_type, IndexEnvNext>::type       EnvContainerNext;

      typedef typename boost::fusion::result_of::value_at_key<EnvContainerNext, typename DimensionPack::type>::type TransferedType;

      typedef typename TransferedType::value_type::value_type                       TypeInside;
      typedef boost::phoenix::basic_environment< TypeInside, EnvContainerNext >     NewEnvironment; 

      typedef typename TransferedType::value_type::iterator TheIterator;
      typedef typename TransferedType::value_type TraversalObject;
      TraversalObject& traversal_object= gsse::at(boost::fusion::at_c<0>(env.args()))(boost::fusion::at_key<typename DimensionPack::type>(  boost::fusion::at_c<IndexEnvNext>(env.args()) ));
      TheIterator the_it;
      TheIterator ti_begin =  traversal_object.begin();
      TheIterator ti_end   =  traversal_object.end();

      for (the_it  = ti_begin ;
           the_it != ti_end   ;
           ++the_it)
      {
         NewEnvironment env_new ( *the_it, boost::fusion::at_c<IndexEnvNext>(env.args()) );

         calc_object.eval( env_new );
      }
   }
};



// ################################################################################################################################

// template <typename DimensionPack, typename Enable=void>   
// struct traversal_ls_eval
// {
//    template <typename Env, typename EvalObject, typename LocalScopeVariable>
//    struct result
//    {
//       typedef void type;
//    };
   
//    template <typename RT, typename Env, typename EvalObject, typename LocalScopeVariable>
//    static typename result<Env,  EvalObject, LocalScopeVariable>::type
//    eval(Env const& env, EvalObject& calc_object, LocalScopeVariable& lsv)
//    {
//       dump_type<Env>();
//       dump_type<LocalScopeVariable>();

//       static const long IndexEnvNext = boost::mpl::size<typename Env::args_type>::value - 1;

//       typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type                       EnvContainer;
//       typedef gsse::result_of::extension::decode_traversal_gsse<EnvContainer, DimensionPack>    DecodedTraversal;
      
//       // [RH][unfortunate][hack] 
//       //   have to use different indices for the first call of this eval (index = 0)
//       //   and for all other times (index = 1)
//       //
//       typedef typename boost::mpl::at_c<typename Env::args_type, IndexEnvNext>::type       EnvContainerNext;
//       typedef typename DecodedTraversal::template apply<Env>::TraversalObject              TraversalObject;
//       TraversalObject& traversal_object =  DecodedTraversal::template apply<Env>::call(env);

//       typedef typename DecodedTraversal::template apply<Env>::TypeInside                   TypeInside;
//       typedef boost::phoenix::basic_environment< TypeInside, EnvContainerNext >            NewEnvironment; 



//       typedef typename TraversalObject::iterator TheIterator;
//       TheIterator the_it;


//       TheIterator ti_begin =  traversal_object.begin();
//       TheIterator ti_end   =  traversal_object.end();

//       for (the_it  = ti_begin ;
//            the_it != ti_end   ;
//            ++the_it)
//       {
//          NewEnvironment new_environment( *the_it , boost::fusion::at_c<IndexEnvNext>(env.args()));

// //              typedef typename change_environment_impl<Env, NewEnvironment>::type EnvToPass;
// //              EnvToPass env_pass = change_environment_impl<Env, NewEnvironment>()(env, new_environment);
// //              dump_type<EnvToPass>();
   
// //          typedef typename LocalScopeVariable::key_type varname;

// //                 typedef boost::phoenix::scoped_environment
// //                   <NewEnvironment,
// //                    NewEnvironment,
// //                    boost::fusion::vector<const LocalScopeVariable&>,
// //                    boost::phoenix::detail::map_local_index_to_tuple<varname> 
// //                  > final_env;
            
// // //              // create the tuple which contains the base element. 
// // //              //
// // //              boost::fusion::vector<const TraversalObject&>
// // //                 tup(  boost::fusion::at_c<IndexEnvNext>(env.args())   );


// //                boost::fusion::vector<const LocalScopeVariable&>
// //                   tup(  lsv   );
            
// // // //             // the final environment can be assembled
// // // //             //
// //                final_env final(new_environment, new_environment, tup);

// //               calc_object.eval(  final );

// //         NewEnvironment env_new ( *the_it, boost::fusion::at_c<IndexEnvNext>(env.args())  ,  lsv );
//       }


//    }
// };


  
} // namespace detail

// #####################################################################################################################################
// 
   
// object generators for traversal
//
template <typename DimensionPack, typename NumericT>
struct traversal_gen
{  
   traversal_gen(NumericT numeric):numeric(numeric){}
   
   template <typename EvalObject>
   boost::phoenix::actor<typename boost::phoenix::as_composite<traversal::detail::traversal_eval<DimensionPack>, EvalObject, NumericT>::type>
   operator[](EvalObject const&  eval_object)  
   {
      return boost::phoenix::compose<traversal::detail::traversal_eval<DimensionPack> >( eval_object , numeric );
   }
   NumericT numeric;
};

// [RH][info]
//   special non-const traversal generator 
//   for debugging purpose, ...
//
template <typename DimensionPack, typename NumericT>
struct traversal_gen_notconst
{  
   traversal_gen_notconst(NumericT numeric):numeric(numeric){}
   
   template <typename EvalObject>
   boost::phoenix::actor<typename boost::phoenix::as_composite<traversal::detail::traversal_eval<DimensionPack>, EvalObject, NumericT>::type>
   operator[](EvalObject &  eval_object)  
   {
      return boost::phoenix::compose<traversal::detail::traversal_eval<DimensionPack> >( eval_object , numeric );
   }
   NumericT numeric;
};

// [RH][info]
//   local scope debug code
//
// template <typename DimensionPack, typename LocalScopeVariable>
// struct traversal_ls_gen
// {  
//    traversal_ls_gen(LocalScopeVariable const& lsv):lsv(lsv){}
   
//    template <typename EvalObject>
//    boost::phoenix::actor<typename boost::phoenix::as_composite<traversal::detail::traversal_ls_eval<DimensionPack>, EvalObject, LocalScopeVariable>::type>
//    operator[](EvalObject const& eval_object)  
//    {
//       return boost::phoenix::compose<traversal::detail::traversal_ls_eval<DimensionPack> >( eval_object , lsv );
//    }
//    LocalScopeVariable lsv;
// };



// ##############



}//namespace traversal

namespace
{
   
template <typename DimensionPack >
traversal::traversal_gen<DimensionPack, double>
traverse()
{  
   return traversal::traversal_gen<DimensionPack, double>(1.);
}


traversal::traversal_gen<void, double>
traverse()
{  
   return traversal::traversal_gen<void, double>(1.);
}

traversal::traversal_gen_notconst<void, double>
traverse_nc()
{  
   return traversal::traversal_gen_notconst<void, double>(1.);
}

}

// template <typename DimensionPack, typename LocalScopeVariable >
// traversal::traversal_ls_gen<DimensionPack, LocalScopeVariable>
// traverse2(LocalScopeVariable lsv)
// {
//    return traversal::traversal_ls_gen<DimensionPack,LocalScopeVariable>(lsv);
// }


// [RH]
// unfortunate hack to accomplish the combined traversal / local scope variable
//  but not possible with the current phoenix environment protocoll
//
//   composites     -> have to implement a static eval()
//   actors for ls  -> have to use local member variables 
// 
//  hence -> not possible
//
//

//    template <typename DimensionPack, typename Vars, typename Map>
//    struct let_actor 
//    {
//        typedef typename
//             boost::mpl::fold<
//                Vars
//              , boost::mpl::false_
//              , boost::phoenix::detail::compute_no_nullary
//            >::type
//        no_nullary;

//       template<typename Env>
//       struct result_newenv
//       {
//          typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type                       EnvContainer;
//          typedef gsse::result_of::extension::decode_traversal_gsse<EnvContainer, DimensionPack>    DecodedTraversal;
//          typedef typename DecodedTraversal::template apply<Env>::TypeInside                        TypeInside;

//          typedef boost::phoenix::basic_environment< TypeInside >                                   type;
//       };

    
//       template <typename Env, typename T2>
//        struct result
//        {
//            typedef typename 
//                boost::fusion::result_of::as_vector<
//                  typename boost::fusion::result_of::transform<
//                        Vars
//                        , boost::phoenix::detail::initialize_local<Env>
//                    >::type
//                >::type 
//            locals_type;

//           // [RH][??] it seems, that this result is based on the
//           //   result of the base class
//           // 
// //          typedef typename result_newenv<Env>::type NewEnv;
//           typedef Env NewEnv;

// // //            typedef typename Base::template
// // //               result<boost::phoenix::scoped_environment<NewEnv, Env, locals_type, Map> >::type
// // //            result_type;
        
// //            typedef typename 
// //              boost::phoenix::detail::unwrap_local_reference<result_type>::type 
// //            type;

//           typedef void type;
//        };


//        let_actor(Vars const& vars)
//            : vars(vars) {}


//       template <typename RT, typename Env, typename EvalObject>
//       //      static typename result<Env,  EvalObject, >::type
//       //      static 
//       void // [RH][TODO]
//       eval(Env const& env, EvalObject& calc_object)
//       {         
// //        template <typename Env>
// //        void 
// //        //       typename result<Env>::type
// //        eval(Env const& env) const
// //        {
//           typedef typename result_newenv<Env>::type NewEnv;
//           typedef typename result_newenv<Env>::DecodedTraversal DecodedTraversal;

//           std::cout << "Env: " << std::endl;
//           dump_type<Env>();
//           dump_type<NewEnv>();


//           typedef typename DecodedTraversal::template apply<Env>::TraversalObject              TraversalObject;
//           TraversalObject& traversal_object =  DecodedTraversal::template apply<Env>::call(env);
//           dump_type<TraversalObject>();

//           typedef typename TraversalObject::iterator TheIterator;
//           TheIterator the_it;


//           // -------------------------------------------------------------------
//           // local scope calculations
//           //
//            typedef typename 
//                boost::fusion::result_of::as_vector<
//                    typename boost::fusion::result_of::transform<
//                         Vars
//                       , boost::phoenix::detail::initialize_local<Env>
//                    >::type
//                >::type 
//            locals_type;
//            std::cout << "vars: " << std::endl;
//            dump_type<Vars>();

//            std::cout << "locals type: " << std::endl;
//            dump_type<locals_type>();

//            locals_type locals = 
//                 boost::fusion::as_vector(
//                    boost::fusion::transform(
//                        vars
//                       , boost::phoenix::detail::initialize_local<Env>(env)));
//            // -------------------------------------------------------------------


//            // [RH][TODO]
//            //
//            TheIterator ti_begin =  traversal_object.begin();
// //            TheIterator ti_end   =  traversal_object.end();
           
// //            for (the_it  = ti_begin ;
// //                 the_it != ti_end   ;
// //                 ++the_it)
// //            {
// //               typedef typename result_newenv<Env>::type NewEnv;
// //               NewEnv new_env ( *the_it );
// //            }

//            typedef typename result_newenv<Env>::type NewEnv;
//            NewEnv new_env ( * (ti_begin) );

//            typedef boost::phoenix::scoped_environment<NewEnv, Env, locals_type, Map> FinalEnv;
           
//            calc_object.eval( FinalEnv( new_env, env, locals) );


// //            typedef boost::phoenix::scoped_environment<NewEnv, Env, locals_type, Map> TestType;
// //            std::cout << " ## first env .. " << std::endl;
// //            dump_type<Env>();
// //            std::cout << std::endl;
// //            std::cout << " ## scoped env .. " << std::endl;
// //            dump_type<TestType>();
        
// //            typedef typename result<Env>::type RT;
// //            dump_type<RT>();

// //            return RT(Base::eval(
// //                    boost::phoenix::scoped_environment<NewEnv, Env, locals_type, Map>(
// //                    new_env
// //                  , env
// //                  , locals)));
// //            Base::eval(
// //               boost::phoenix::scoped_environment<NewEnv, Env, locals_type, Map>(
// //                  new_env
// //                  , env
// //                  , locals));

         

//        }

//        Vars vars;
//    };


//    template <typename DimensionPack, typename Vars, typename Map>
//    struct let_actor_gen
//    {
//       // template <typename Base>
//        //       boost::phoenix::actor<let_actor<DimensionPack, Base, Vars, Map> > const

//        template <typename EvalObject>
//        boost::phoenix::actor<typename boost::phoenix::as_composite<let_actor<DimensionPack, Vars, Map>, EvalObject>::type>
//        operator[](EvalObject const& eval_object)  
// //        boost::phoenix::actor<let_actor<DimensionPack, Base, Vars, Map> > const
// //        operator[](boost::phoenix::actor<Base> const& base) const
//        {
// //          return let_actor<DimensionPack, Base, Vars, Map>(base, vars);
//           return boost::phoenix::compose<let_actor<DimensionPack, Vars, Map> >( vars, eval_object );

//        }

//        let_actor_gen(Vars const& vars)
//            : vars(vars) {}

//        Vars vars;
//    };


//       template <typename DimensionPack, typename K0, typename V0>
//       let_actor_gen<DimensionPack, 
//            boost::fusion::vector<V0>
//            , boost::phoenix::detail::map_local_index_to_tuple<K0>
//         >
//         traverse_let2(
//            boost::phoenix::actor<boost::phoenix::composite<boost::phoenix::assign_eval, boost::fusion::vector<boost::phoenix::local_variable<K0>, V0> > > const& a0
//         ) 
//         {
//             return boost::fusion::vector<V0>(boost::fusion::at_c<1>(a0));
//         }

//       template <typename DimensionPack, typename K0, typename K1, typename V0, typename V1>
//       let_actor_gen<DimensionPack, 
//            boost::fusion::vector<V0, V1>
//            , boost::phoenix::detail::map_local_index_to_tuple<K0, K1>
//         >
//         traverse_let2(
//            boost::phoenix::actor<boost::phoenix::composite<boost::phoenix::assign_eval, boost::fusion::vector<boost::phoenix::local_variable<K0>, V0> > > const& a0
//          , boost::phoenix::actor<boost::phoenix::composite<boost::phoenix::assign_eval, boost::fusion::vector<boost::phoenix::local_variable<K1>, V1> > > const& a1
//         ) 
//         {
//             return boost::fusion::vector<V0, V1>(boost::fusion::at_c<1>(a0), boost::fusion::at_c<1>(a1));
//         }


//================



}  // namespace gsse


#endif
