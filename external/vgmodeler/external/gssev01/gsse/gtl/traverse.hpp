/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_2008_LAMBDA_TRAVERSAL_HH
#define GSSE_2008_LAMBDA_TRAVERSAL_HH


// *** BOOST includes
//
#include <boost/type_traits.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/if.hpp>

// [RH] adaptation to boost >= 1.37
#include <boost/spirit/home/phoenix.hpp>   

// old boost < 1.37
// #include <boost/spirit/phoenix/core.hpp>
// #include <boost/spirit/phoenix/core/composite.hpp>
// #include <boost/spirit/phoenix/core/compose.hpp>
// #include <boost/spirit/phoenix/operator.hpp>
// #include <boost/spirit/phoenix/scope/local_variable.hpp>
// #include <boost/spirit/phoenix/core/nothing.hpp>

  
#include <boost/fusion/sequence.hpp>

#include <boost/lambda/lambda.hpp>
#include <boost/utility/enable_if.hpp>


// *** GSSE includes
//
#include "gsse/gtl/functions.hpp"
#include "gsse/gtl/selector.hpp" 

#include "gsse/debug_meta.hpp"   // [RH][TODO][temporary]


namespace gsse
{
namespace detail_lambda
{

    // these function(object)s are needed because the environment changes
    // if we use accumulations which store the base elements as scope keys. If we
    // are passed a non-scoped evnironemnt we can pass it as is. 

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




   typedef  boost::phoenix::actor<boost::phoenix::composite<boost::phoenix::plus_assign_eval, 
                                                            boost::fusion::vector<boost::phoenix::argument<0>,  
                                                                                  boost::phoenix::argument<1> > > >          summation;

   typedef  boost::phoenix::actor<boost::phoenix::composite<boost::phoenix::multiplies_assign_eval,
                                                            boost::fusion::vector<boost::phoenix::argument<0>,  
                                                                                 boost::phoenix::argument<1> > > >          multiplication;


   typedef  boost::phoenix::actor<boost::phoenix::composite<boost::phoenix::logical_and_eval,
                                                            boost::fusion::vector<boost::phoenix::argument<0>,  
                                                                                  boost::phoenix::argument<1> > > >          logical_and;

     typedef  boost::phoenix::actor<boost::phoenix::null_actor>   iteration;



// ###############################################
// ###############################################
// ###############################################

template <typename TraversalSubType>   
struct traversal_eval
{
   template <typename Env, typename Summand, typename NumericT>
   struct result
   {
      typedef void type;
   };
   
   template <typename RT, typename Env, typename Summand, typename NumericT>
   static typename result<Env,  Summand, NumericT>::type
   eval(Env const& env, Summand& summand, NumericT numeric)
      {

	typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type  TraversalObject;
        
	typedef typename init_functor_switch
            < 
            TraversalObject,                    // TraversalObject: e.g. domain
            TraversalSubType                      // TraversalSubType : e.g. segment --> segment_on_domain 
            >::type traversal_object;
  
//          dump_type<typename boost::mpl::at_c<typename Env::args_type, 0>::type>();
//           dump_type<TraversalSubType>();
//           dump_type<traversal_object>();
//           dump_type<typename TraversalObject::gsse_type>();
//           dump_type<typename TraversalObject::traversal_repository>();
//           dump_type< typename boost::enable_if<is_container<typename TraversalObject::gsse_type> >::type>();
//           std::cout << "val: " << is_container<typename TraversalObject::gsse_type>::value << std::endl;
// //          dump_type<typename init_functor_switch  < TraversalObject,  TraversalSubType>::mytype>();
//           dump_type<typename TraversalObject::traversal_repository>();
//           dump_type< typename  boost::mpl::at<typename TraversalObject::traversal_repository, boost::mpl::pair<TraversalObject, TraversalSubType> >::type>();
//           dump_type< boost::mpl::pair<TraversalObject, TraversalSubType> >();

          traversal_object it_boundary(  boost::fusion::at_c<0>(env.args()) );

          typename traversal_object::iterator iter   = it_boundary.begin();
          typename traversal_object::iterator iter_e = it_boundary.end();

          for (; iter != iter_e; ++iter)
          {
             typedef typename traversal_object::value_type value_type;

             value_type elem = *iter;   // [RH][TODO] .. a const reference / reference has to be used to access the underlying object directly
          
             typedef boost::phoenix::basic_environment<value_type> EnvSimple;
             EnvSimple env_simple(elem);
          
             typedef typename change_environment_impl<Env, EnvSimple>::type EnvToPass;
             EnvToPass env_pass = change_environment_impl<Env, EnvSimple>()(env, env_simple);
          
             summand.eval(env_pass);
          }

         
      }
};




template <typename TraversalSubType>
struct traversal_ls_eval
{
   template <typename Env, typename Summand, typename LocalScopeVariable>
    struct result
    {
       typedef void type;
    };

   template <typename RT, typename Env, typename Summand, typename LocalScopeVariable>
   static typename result<Env,  Summand, LocalScopeVariable>::type
   eval(Env const& env, Summand& summand, LocalScopeVariable& lsv)
      {

	typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type  TraversalObject;
        
	typedef typename init_functor_switch
            < 
	    TraversalObject,                    // TraversalObject: e.g. domain
            TraversalSubType                      // TraversalSubType : e.g. segment --> segment_on_domain 
            >::type traversal_object;
   
         traversal_object it_boundary(  boost::fusion::at_c<0>(env.args()) );
         typename traversal_object::iterator iter   = it_boundary.begin();
         typename traversal_object::iterator iter_e = it_boundary.end();

         for (; iter != iter_e; ++iter)
         {
            typedef typename traversal_object::value_type value_type;
            
            value_type elem = *iter;
    	   
            // the simple environment: Only the argument of the last
            // function is passed to the interior summation. 
            //
             typedef boost::phoenix::basic_environment<value_type> EnvSimple;
             EnvSimple env_simple(elem);
           
             // Environment selection routines
             //
             //
             // The function body (summand) has to be evaluated with
             // only ONE environment
             //
             // If the environment is already scoped, it is included  into a new scoped envir 
             //   here we need parts of the old envir. 
             // If env is not scoped the env_simple can be used
             //
    
             typedef typename change_environment_impl<Env, EnvSimple>::type EnvToPass;
             EnvToPass env_pass = change_environment_impl<Env, EnvSimple>()(env, env_simple);
   
             typedef typename LocalScopeVariable::key_type varname;
             
             typedef boost::phoenix::scoped_environment
                <EnvToPass,
                 EnvToPass,
                 boost::fusion::vector<const TraversalObject&>,
                 boost::phoenix::detail::map_local_index_to_tuple<varname> 
               > final_env;
            
             // create the tuple which contains the base element. 
             //
             boost::fusion::vector<const TraversalObject&>
                tup(  boost::fusion::at_c<0>(env.args())   );

            
            // the final environment can be assembled
            //
            final_env final(env_pass, env_pass, tup);
            
            
            summand.eval(final);
         }
      }
};



template <typename Action, typename T0>
struct actor_result
{
   typedef boost::phoenix::basic_environment<T0> env_type;
   typedef typename Action::template result<env_type>:: type type;
};


// =======================

template <typename TraversalSubType, typename CalculationType>
struct traversal_arithmetic_eval
{
    template <typename Env, typename Initial, typename Summand>
    struct result
    {
       typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type Arg1Type;
       // [RH][TODO] .. study the actor_result meta function
       //
       typedef typename actor_result<Initial, Arg1Type>::type type;
    };

   template <typename RT, typename Env, typename Initial, typename Summand>
   static typename result<Env, Initial, Summand>::type
   eval(Env const& env, Initial& init, Summand& summand)
      {
         // [RH][TODO] .. check this init for .. call by reference
         //    .. a lot of performance gain can be obtained
         //
         //   ## for now, all new objects are created by copy
         RT result = init.eval(env);

	typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type  TraversalObject;
        
	typedef typename init_functor_switch
            < 
	    TraversalObject,                    // TraversalObject: e.g. domain
            TraversalSubType                      // TraversalSubType : e.g. segment --> segment_on_domain 
            >::type traversal_object;
   

//          dump_type<typename boost::mpl::at_c<typename Env::args_type, 0>::type>();
//          dump_type<TraversalSubType>();
//          dump_type<traversal_object>();


         traversal_object it_boundary(  boost::fusion::at_c<0>(env.args()) );
   
         typename traversal_object::iterator iter   = it_boundary.begin();
         typename traversal_object::iterator iter_e = it_boundary.end();
   
         for (; iter != iter_e; ++iter)
         {
            typedef typename traversal_object::value_type value_type;
            
            value_type elem = *iter;
    	   
            // the simple environment: Only the argument of the last
            // function is passed to the interior calculation
            //
            typedef boost::phoenix::basic_environment<value_type> EnvSimple;
            EnvSimple env_simple(elem);
           
             // Environment selection routines
             //
             //
             // The function body (summand) has to be evaluated with
             // only ONE environment
             //
             // If the environment is already scoped, it is included  into a new scoped envir 
             //   here we need parts of the old envir. 
             // If env is not scoped the env_simple can be used
             //
    
            // [RH][TODO] 
            // remove this comment and study this in more detail
            //
                typedef typename change_environment_impl<Env, EnvSimple>::type EnvToPass;
                EnvToPass env_pass = change_environment_impl<Env, EnvSimple>()(env, env_simple);
//                typedef EnvSimple EnvToPass;
//                EnvToPass env_pass = env_simple;
                        
                typename Summand::template result<EnvToPass>::type 
                   increment  =  summand.eval(env_pass);

// [RH][DEBUG information]
//
//                 std::cout << "#### data type  arithmetic eval: " << std::endl;
//                 dump_type<typename Summand::template result<EnvToPass>::type >();
//                 std::cout << "result: " << result << std::endl;
//                 std::cout << "  inc: " << increment << std::endl;
//                 std::cout << std::endl;


                result = CalculationType()(result,increment);
                
                // e.g., with gsse::plus
                //
                // lineq1(x11*[10], x12*[12]) + lineq2(x21*[10], x22*[22])
                
         }

         return result;
      }
};
// =================
//
// object generators for traversal
template <typename TraversalSubType, typename CalculationType, typename Initial>
struct traversal_arithmetic_gen
{
   traversal_arithmetic_gen(Initial const& init) : init(init) {}
    
   template <typename Summand>
   boost::phoenix::actor<typename boost::phoenix::as_composite<traversal_arithmetic_eval<TraversalSubType, CalculationType>, Initial, Summand>::type>
   operator[](Summand const& sum)
      {
         
         return boost::phoenix::compose<traversal_arithmetic_eval<TraversalSubType, CalculationType> >(init, sum);
      }
    
private:
   Initial init;
};



// for [PS] moment_hd
//
template <typename TraversalSubType, typename CalculationType>
struct traversal_special_eval
{
    template <typename Env, typename Initial, typename Summand>
    struct result
    {
       typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type Arg1Type;
       // [RH][TODO]
       typedef typename actor_result<Initial, Arg1Type>::type type;
    };

   template <typename RT, typename Env, typename Initial, typename Summand>
   static typename result<Env, Initial, Summand>::type
   eval(Env const& env, Initial& init, Summand& summand)
      {

	typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type  TraversalObject;
        
	typedef typename init_functor_switch
            < 
	    TraversalObject,                    // TraversalObject: e.g. domain
            TraversalSubType                      // TraversalSubType : e.g. segment --> segment_on_domain 
            >::type traversal_object;
   
         traversal_object it_boundary(  boost::fusion::at_c<0>(env.args()) );
         typename traversal_object::iterator iter;
   

    
         // [RH][TODO] .. check this init for .. call by reference
         //    .. a lot of performance gain can be obtained
         //
         //   ## for now, all new objects are created by copy
         RT result = RT(0); //
   
         for (iter = it_boundary.begin(); iter != it_boundary.end(); ++iter)
         {
            typedef typename traversal_object::value_type value_type;
            
            value_type elem = *iter;

            typedef boost::phoenix::basic_environment<value_type> EnvSimple;
            EnvSimple env_simple(elem);
            
            typedef typename change_environment_impl<Env, EnvSimple>::type EnvToPass;
            EnvToPass env_pass = change_environment_impl<Env, EnvSimple>()(env, env_simple);


//            result += init.eval(env_pass);
//            std::cout << "## first init result: " << result << std::endl;

            typename Summand::template result<EnvToPass>::type 
               increment  =  summand.eval(env_pass);
// [RH][DEBUG information]
//
//                 std::cout << "#### data type  arithmetic eval special: " << std::endl;
//                 dump_type<typename Summand::template result<EnvToPass>::type >();
//                 std::cout << "  inc    : " << increment << std::endl;
//                 std::cout << " log(inc): " << gsse::detail_math::log_impl()(increment) << std::endl;
//                  std::cout << std::endl;

                 result +=  gsse::detail_math::log_impl()(  init.eval(env_pass)  ) * increment;
         }
   
//      std::cout << "result: " << result << std::endl;
      return result;
    }
};
// =================
//
// object generators for traversal
template <typename TraversalSubType, typename CalculationType, typename Initial>
struct traversal_special_gen
{
   traversal_special_gen(Initial const& init) : init(init) {}
    
   template <typename Summand>
   boost::phoenix::actor<typename boost::phoenix::as_composite<traversal_special_eval<TraversalSubType, CalculationType>, Initial, Summand>::type>
   operator[](Summand const& sum)
      {
         
         return boost::phoenix::compose<traversal_special_eval<TraversalSubType, CalculationType> >(init, sum);
      }
    
private:
   Initial init;
};



// ==================================================   LOCAL SCOPE VARIABLES ================

template <typename TraversalSubType, typename CalculationType>
struct traversal_arithmetic_ls_eval
{
   template <typename Env, typename Initial, typename Summand, typename LocalScopeVariable>
    struct result
    {
       typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type Arg1Type;
       // [RH][TODO] 
       typedef typename actor_result<Initial, Arg1Type>::type type;
    };

   template <typename RT, typename Env, typename Initial, typename Summand, typename LocalScopeVariable>
   static typename result<Env, Initial, Summand, LocalScopeVariable>::type
   eval(Env const& env, Initial& init, Summand& summand, LocalScopeVariable& lsv)
      {

         // [RH][TODO] .. check this init for .. call by reference
         //    .. a lot of performance gain can be obtained
         //
         //   ## for now, all new objects are created by copy
         //
         RT result = init.eval(env);

	typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type  TraversalObject;

         typedef typename init_functor_switch
            < 
            TraversalObject,                            // TraversalObject: e.g. domain
            TraversalSubType                              // TraversalSubType : e.g. segment --> segment_on_domain 
            >::type traversal_object;
   
         traversal_object it_boundary(  boost::fusion::at_c<0>(env.args()) );
         typename traversal_object::iterator iter   = it_boundary.begin();
         typename traversal_object::iterator iter_e = it_boundary.end();
   
         for (; iter != iter_e; ++iter)
         {
            typedef typename traversal_object::value_type value_type;
            
            value_type elem = *iter;

            // the simple environment: Only the argument of the last
            // function is passed to the interior calculation
            //
            typedef boost::phoenix::basic_environment<value_type> EnvSimple;
            EnvSimple env_simple(elem);
           
             // Environment selection routines
             //
             //
             // The function body (summand) has to be evaluated with
             // only ONE environment
             //
             // If the environment is already scoped, it is included  into a new scoped envir 
             //   here we need parts of the old envir. 
             // If env is not scoped the env_simple can be used
             //
                typedef typename change_environment_impl<Env, EnvSimple>::type EnvToPass;
                EnvToPass env_pass = change_environment_impl<Env, EnvSimple>()(env, env_simple);
                
                typedef typename LocalScopeVariable::key_type varname;

                typedef boost::phoenix::scoped_environment
                   <EnvToPass,
                    EnvToPass,
                    boost::fusion::vector<const TraversalObject&>,
                    boost::phoenix::detail::map_local_index_to_tuple<varname> >
                final_env;
            
                // Create the tuple which contains the base element. 
                //
                boost::fusion::vector<const TraversalObject&>
                   tup(    boost::fusion::at_c<0>(env.args())   );

                // the final environment can be assembled
                //
                final_env final(env_pass, env_pass, tup);

                summand.eval(final);

                typename Summand::template result<final_env>::type 
                   increment  =  summand.eval(final);

                result = CalculationType()(result,increment);
                
                // e.g., with gsse::plus
                //
                // lineq1(x11*[10], x12*[12]) + lineq2(x21*[10], x22*[22])
                

         }
         return result;
      }
};


template <typename TraversalSubType, typename CalculationType>
struct traversal_arithmetic_ls_eval_grad
{
   template <typename Env, typename Initial, typename Summand, typename LocalScopeVariable>
    struct result
    {
       typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type Arg1Type;
       // [RH][TODO] 
       typedef typename actor_result<Initial, Arg1Type>::type type;
    };

   template <typename RT, typename Env, typename Initial, typename Summand, typename LocalScopeVariable>
   static typename result<Env, Initial, Summand, LocalScopeVariable>::type
   eval(Env const& env, Initial& init, Summand& summand, LocalScopeVariable& lsv)
      {

         // [RH][TODO] .. check this init for .. call by reference
         //    .. a lot of performance gain can be obtained
         //
         //   ## for now, all new objects are created by copy
         //
         RT result = init.eval(env);

	typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type  TraversalObject;

         typedef typename init_functor_switch
            < 
            TraversalObject,                            // TraversalObject: e.g. domain
            TraversalSubType                              // TraversalSubType : e.g. segment --> segment_on_domain 
            >::type traversal_object;
   
         traversal_object it_boundary(  boost::fusion::at_c<0>(env.args()) );
         typename traversal_object::iterator iter   = it_boundary.begin();
         typename traversal_object::iterator iter_e = it_boundary.end();
   
         for (; iter != iter_e; ++iter)
         {
            typedef typename traversal_object::value_type value_type;
            
            value_type elem = *iter;

            // the simple environment: Only the argument of the last
            // function is passed to the interior calculation
            //
            typedef boost::phoenix::basic_environment<value_type> EnvSimple;
            EnvSimple env_simple(elem);
           
             // Environment selection routines
             //
             //
             // The function body (summand) has to be evaluated with
             // only ONE environment
             //
             // If the environment is already scoped, it is included  into a new scoped envir 
             //   here we need parts of the old envir. 
             // If env is not scoped the env_simple can be used
             //
                typedef typename change_environment_impl<Env, EnvSimple>::type EnvToPass;
                EnvToPass env_pass = change_environment_impl<Env, EnvSimple>()(env, env_simple);
                
                typedef typename LocalScopeVariable::key_type varname;

                typedef boost::phoenix::scoped_environment
                   <EnvToPass,
                    EnvToPass,
                    boost::fusion::vector<const TraversalObject&>,
                    boost::phoenix::detail::map_local_index_to_tuple<varname> >
                final_env;
            
                // Create the tuple which contains the base element. 
                //
                boost::fusion::vector<const TraversalObject&>
                   tup(    boost::fusion::at_c<0>(env.args())   );

                // the final environment can be assembled
                //
                final_env final(env_pass, env_pass, tup);

//                summand.eval(final);

//                double grad_orient = 1.;
//                 if ( lsv != elem )
//                    grad_orient *= -1.;

//                std::cout << "### local scope ###  " << std::endl;
//                dump_type<final_env>();

                typename Summand::template result<final_env>::type 
                   increment  =  summand.eval(final);

                result = CalculationType()(result,increment);
                
                // e.g., with gsse::plus
                //
                // lineq1(x11*[10], x12*[12]) + lineq2(x21*[10], x22*[22])
                

         }
         return result;
      }
};

// =================
//
// object generators for traversal
//
template <typename TraversalSubType, typename NumericT>
struct traversal_gen
{  
   traversal_gen(NumericT numeric):numeric(numeric){}
    
   template <typename Summand>
   boost::phoenix::actor<typename boost::phoenix::as_composite<traversal_eval<TraversalSubType>, Summand, NumericT>::type>
   operator[](Summand const& sum)
      {
         return boost::phoenix::compose<traversal_eval<TraversalSubType> >( sum , numeric );
      }
   NumericT numeric;
};


template <typename TraversalSubType, typename LocalScopeVariable>
struct traversal_ls_gen
{
   traversal_ls_gen(LocalScopeVariable const& lsv):lsv(lsv) {}
    
   template <typename Summand>
   boost::phoenix::actor<typename boost::phoenix::as_composite<traversal_ls_eval<TraversalSubType>, Summand, LocalScopeVariable>::type>
   operator[](Summand const& sum)
      {
         
         return boost::phoenix::compose<traversal_ls_eval<TraversalSubType> >(sum, lsv);
      }
   LocalScopeVariable  lsv;
};


template <typename TraversalSubType, typename CalculationType, typename Initial, typename LocalScopeVariable>
struct traversal_arithmetic_ls_gen
{
   traversal_arithmetic_ls_gen(Initial const& init, LocalScopeVariable const& lsv) : init(init), lsv(lsv) {}
    
   template <typename Summand>
   boost::phoenix::actor<typename boost::phoenix::as_composite<traversal_arithmetic_ls_eval<TraversalSubType, CalculationType>, Initial, Summand, LocalScopeVariable>::type>
   operator[](Summand const& sum)
      {
         
         return boost::phoenix::compose<traversal_arithmetic_ls_eval<TraversalSubType, CalculationType> >(init, sum, lsv);
      }
    
private:
   Initial             init;
   LocalScopeVariable  lsv;
};



template <typename TraversalSubType, typename CalculationType, typename Initial, typename LocalScopeVariable>
struct traversal_arithmetic_ls_gen_grad
{
   traversal_arithmetic_ls_gen_grad(Initial const& init, LocalScopeVariable const& lsv) : init(init), lsv(lsv) {}
    
   template <typename Summand>
   boost::phoenix::actor<typename boost::phoenix::as_composite<traversal_arithmetic_ls_eval_grad<TraversalSubType, CalculationType>, Initial, Summand, LocalScopeVariable>::type>
   operator[](Summand const& sum)
      {
         
         return boost::phoenix::compose<traversal_arithmetic_ls_eval_grad<TraversalSubType, CalculationType> >(init, sum, lsv);
      }
    
private:
   Initial             init;
   LocalScopeVariable  lsv;
};




// ====================================================================================================================================


}    // namespace detail_lambda


typedef  boost::phoenix::actor<boost::phoenix::composite<boost::phoenix::plus_assign_eval,
                                                         boost::fusion::vector<boost::phoenix::argument<0>,  
                                                                               boost::phoenix::argument<1> > > >          plus;
typedef  boost::phoenix::actor<boost::phoenix::composite<boost::phoenix::minus_assign_eval,
                                                         boost::fusion::vector<boost::phoenix::argument<0>,  
                                                                               boost::phoenix::argument<1> > > >          minus;
typedef  boost::phoenix::actor<boost::phoenix::composite<boost::phoenix::multiplies_assign_eval,
                                                         boost::fusion::vector<boost::phoenix::argument<0>,  
                                                                               boost::phoenix::argument<1> > > >          multiplies;
typedef  boost::phoenix::actor<boost::phoenix::composite<boost::phoenix::divides_assign_eval,
                                                         boost::fusion::vector<boost::phoenix::argument<0>,  
                                                                               boost::phoenix::argument<1> > > >           divides;


template <typename TraversalSubType, typename CalculationType, typename NumericT>
detail_lambda::traversal_arithmetic_gen<TraversalSubType, CalculationType, NumericT>
traverse(NumericT init )
{
   return detail_lambda::traversal_arithmetic_gen<TraversalSubType, CalculationType, NumericT>(init);
}

template <typename TraversalSubType, typename CalculationType, typename NumericT, typename LocalScopeVariable>
detail_lambda::traversal_arithmetic_ls_gen<TraversalSubType, CalculationType, NumericT, LocalScopeVariable>
traversel(NumericT init, LocalScopeVariable lsv )
{
   return detail_lambda::traversal_arithmetic_ls_gen<TraversalSubType, CalculationType, NumericT, LocalScopeVariable>(init, lsv);
}



template <typename TraversalSubType, typename LocalScopeVariable >
detail_lambda::traversal_ls_gen<TraversalSubType, LocalScopeVariable>
traverse(LocalScopeVariable lsv)
{
   return detail_lambda::traversal_ls_gen<TraversalSubType,LocalScopeVariable>(lsv);
}



// =================================
// =================================
// =================================



template <typename TraversalSubType >
detail_lambda::traversal_gen<TraversalSubType, double>
traverse()
{  
   return detail_lambda::traversal_gen<TraversalSubType, double>(1.);
}



template <typename TraversalSubType, typename NumericT>
detail_lambda::traversal_arithmetic_gen<TraversalSubType, detail_lambda::summation, NumericT>
sum(NumericT init )
{      
   return detail_lambda::traversal_arithmetic_gen<TraversalSubType, detail_lambda::summation, NumericT>(init);
}

template <typename TraversalSubType, typename NumericT, typename LocalScopeVariable>
detail_lambda::traversal_arithmetic_ls_gen<TraversalSubType, detail_lambda::summation, NumericT, LocalScopeVariable>
sum(NumericT init, LocalScopeVariable lsv )
{
   return detail_lambda::traversal_arithmetic_ls_gen<TraversalSubType, detail_lambda::summation, NumericT,LocalScopeVariable>(init, lsv);
}


//[RH] 20081014 .. [PS,RH] .. gradient functor
//
template <typename TraversalSubType, typename NumericT, typename LocalScopeVariable>
detail_lambda::traversal_arithmetic_ls_gen_grad<TraversalSubType, detail_lambda::summation, NumericT, LocalScopeVariable>
grad(NumericT init, LocalScopeVariable lsv )
{
   return detail_lambda::traversal_arithmetic_ls_gen_grad<TraversalSubType, detail_lambda::summation, NumericT,LocalScopeVariable>(init, lsv);
}


template <typename TraversalSubType, typename NumericT>
detail_lambda::traversal_arithmetic_gen<TraversalSubType, detail_lambda::multiplication, NumericT>
product(NumericT init )
{
   return detail_lambda::traversal_arithmetic_gen<TraversalSubType, detail_lambda::multiplication, NumericT>(init);
}


template <typename TraversalSubType, typename NumericT>
detail_lambda::traversal_special_gen<TraversalSubType, detail_lambda::summation, NumericT>
edge_log(NumericT init )
{
   return detail_lambda::traversal_special_gen<TraversalSubType, detail_lambda::summation, NumericT>(init);
}





}  // namespace gsse 

#endif













// ===========================================
//
// GSSE's arithmetic operators without init-data type
//

/*
struct plus 
{
   template<typename TP1, typename TP2> 
    TP1 operator()(const TP1& t1, const TP2& t2) const
    { return t1 + t2; }
};
struct minus
{
    template<typename TP> 
    TP operator()(const TP& t1, const TP& t2) const
    { return t1 - t2; }
};
struct multiplies
{
    template<typename TP> 
    TP operator()(const TP& t1, const TP& t2) const
    { return t1 * t2; }
};
struct divides
{
    template<typename TP> 
    TP operator()(const TP& t1, const TP& t2) const
    { return t1 / t2; }
};

// not used right now
//
template <typename Tp>
struct arithmetic_default_functor : public std::binary_function<Tp, Tp, Tp>
{
   template<typename  Tp1, typename Tp2>
   Tp1 operator()(const Tp1& t1, const Tp2& t2) const
      { 
         return t1 ; 
      }
};
template<typename T>   // T models ## additive monoid ##
T identity_element (const plus&)
{
  return T(0);
}
template<typename T>   // T models ## additive monoid ##
T identity_element (const minus&)
{
  return T(0);
}
template<typename T>   // T models ## multiplicative monoid ##
T identity_element (const multiplies&)
{
  return T(1);
}
template<typename T>   // T models ## multiplicative monoid ##
T identity_element (const divides&)
{
  return T(1);
}



*/
