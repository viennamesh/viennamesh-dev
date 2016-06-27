/* ============================================================================
   Copyright (c) 2008-2010 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_TRAVERSAL_DECODE_HH)
#define GSSE_TRAVERSAL_DECODE_HH

// *** system includes
#include <iostream>

// *** BOOST includes
#include <boost/mpl/if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/logical.hpp>

#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/include/is_sequence.hpp>

// *** GSSE includes
#include "gsse/util/common.hpp"
#include "gsse/util/tag_of.hpp"

#include "gsse/util/debug_meta.hpp"   // [RH][TODO][temporary]


namespace boost { namespace fusion {
   struct boost_array_tag;
}}


// ##############################################################################
// ## GSSE v0.2 traversal
//
// ## the following concepts are available
// 
// # traversable_object_selector
//  traversable objects,       objects which can be traversed (contain begin/end mechanisms)
//  non-traversable objects,   objects which canNOT be traversed (plain old data types, ..)
//
// # sequence_selector
//  sequences:     objects, where a DimensionPack can be used
//  non-sequences: objects, where only the plain/vanilla/direct intrinsic traversal can be used (no selection)
//
// # access selector
//  sequences:      objects, where the accessed object can be selected
//  non-sequences:  plain data types (simple objects, POD)
//


// [RH] old style for trying to implemented deduced traversal
// 2008 09 17 .. maybe there is no deduced traversal
//   maybe the deduced traversal is only a weakness of current data-structures
//   the new gsse::fiberbundle can always use an intrinsic traversal  !!! 
//  each fiber contains the begin()/end() iterators
//  hence, full backward compatible with stl containers.. 
//
// [RH][TODO] .. enable the mpl/phoenix-actor call protocol for all types
//            thereby making these selectors full phoenix actors
//
//
// #############################################################################################################################################


namespace gsse{


// [RH][TODO] .. rewrite the traversable_object_selector
//    do not use the gsse::container tags, but instead
//    use the fusion::is_sequence<> meta-program
// all fusion sequences     should be accessed at their at<1> element (recursive data structure passing through the traverse[] actors
// all non-fusion sequences should directly return their value types
//

// [RH][TODO] catch POD and all other data types which are not 
//  a sequence (sequence:: not implemented yet)
//  a gsse container (TODO:: sequence/container can be combined)
//
template<typename TestObject, typename EnableT=void>
struct traversable_object_selector
{
    // general implementation for objects which are NOT traversable !!!
   //   this particular case requires a boost::fusion::sequence 
   //   where the at<1> element is passed further (at<0> -> index element)

   // in this particular case -> for std::pair / boost::fusion:sequence types (where the second one is the actual element)
   typedef typename boost::fusion::result_of::value_at_c<TestObject,1>::type type;  // object is not traversable !! .. act on current object

   template<typename Sequence>
   static 
   type&
   call(Sequence& seq)  // [RH][info]   important reference
   {
      // ################### const_cast ############
//      type& traversal_object = const_cast<type&>(boost::fusion::at_c<1>( seq )) ;  // e.g., std:pair<>, or plain old data type (POD)

      type&  
         traversal_object = boost::fusion::at_c<1>( seq ) ;  // e.g., std:pair<>, or plain old data type (POD)

      return traversal_object; 
   }

   template<typename Sequence>
   static 
   type const&
   call(Sequence const& seq)  
   {
      type const&  
         traversal_object = boost::fusion::at_c<1>( seq ) ;  // e.g., std:pair<>, or plain old data type (POD)

      return traversal_object; 
   }

};

template<typename TestObject>
struct traversable_object_selector<TestObject, 
	typename boost::enable_if<gsse::is_traversable<typename gsse::traits::tag_of<TestObject>::container_type> >::type >
{
   typedef TestObject type;     // object is already traversable (contains begin()/end() iterator) .. pass it through
   
   template<typename NonSequence>
   static 
   type&
   call(NonSequence& nseq)
   {
      type& traversal_object = nseq;   
      return traversal_object; 
   }
};



// ### new part for quan_acces 
//   is required to access the index, if the second part is a not-accessible container
//
template<typename TestObject, typename EnableT=void>
struct traversable_object_selector_Quan
{
    // general implementation for objects which are NOT traversable !!!
   typedef typename boost::fusion::result_of::value_at_c<TestObject,1>::type type;  // object is not traversable !! 
   
   template<typename Sequence>
   static 
   type&
   call(Sequence& seq)  // [RH][info]   important reference
   {
      type& traversal_object = boost::fusion::at_c<1>( seq ) ;  // e.g., std:pair<>, or plain old data type (POD)
      return traversal_object; 
   }
};



template<typename TestObject>
struct traversable_object_selector_Quan<TestObject, 
	typename boost::enable_if<gsse::is_traversable<typename gsse::traits::tag_of<TestObject>::container_type> >::type >

{
   typedef typename boost::fusion::result_of::value_at_c<TestObject,0>::type type; 
   
   template<typename Sequence>
   static 
   type&
   call(Sequence& seq)  // [RH][info]   important reference
   {
      type& traversal_object = boost::fusion::at_c<0>( seq ) ; 
      return traversal_object; 
   }
};


// ############################################################################## 
// 
//

// [RH][TODO]
//  rewrite the sequence_selector
//   to a more general dimension-access selector
//  this part can then be combined with the accessor selector
//
template<typename TestObject, typename DimensionPack, typename EvalType=void>
struct sequence_selector
{
   typedef TestObject type;
   
   template<typename T0>
   static 
   type&
   call (T0& t0)   
   {
      return t0;
   }

   template<typename T0>
   static 
   type const&
   call (T0 const& t0)  
   {
      return t0;
   }

};


template<typename TestObject, typename DimensionPack>
struct sequence_selector<TestObject, DimensionPack,  
//	typename boost::enable_if<boost::fusion::traits::is_sequence<TestObject> >::type  >    // [RH][old one]
// [RH][new][ added more dispatch to include all fusion::sequences (to enable arbitrary dimension packs) but exclude boost::array ]
//
   typename boost::enable_if<  
		boost::mpl::and_< 
			boost::fusion::traits::is_sequence<TestObject> ,
			boost::mpl::not_< 
				boost::is_same <  
					typename boost::fusion::traits::tag_of < TestObject > :: type  ,
					boost::fusion::boost_array_tag
					   	>
					> 
				>	
                	    >::type  
	>

{
   typedef typename boost::mpl::at_c<TestObject, DimensionPack::value>::type      type;
   
   template<typename T0>
   static 
   type&
   call (T0& t0)   // [RH][TODO] .. no simple ref -> a reference may occur ..  -> unpack reference .. then reference
     //    but maybe we do not need this reference at all ?? is the T0 not always an environment 
   {              
      //std::cout << "decode traversal at location: " << DimensionPack::value << std::endl;

      type& traversal_object = boost::fusion::at_c<DimensionPack::value>(t0);
      return traversal_object;
   }

   template<typename T0>
   static 
   type const&
   call (T0 const& t0)   // [RH][TODO] .. no simple ref -> a reference may occur -> unpack reference .. then reference
     //    but maybe we do not need this reference at all ?? is the T0 not always an environment 
   {              
      //std::cout << "decode traversal at location: " << DimensionPack::value << std::endl;

      type const& traversal_object = boost::fusion::at_c<DimensionPack::value>(t0);
      return traversal_object;
   }

};



// [RH][TODO] .. combine the sequence_selector/access_selector
//
template<typename TestObject, long DIM, typename EvalType=void>
struct access_selector
{
   typedef TestObject type;
   
   template<typename Env>
   static 
   type&
   call (Env& env)
   {
      return env;
   }
};



template<typename TestObject, long DIM>
struct access_selector<TestObject, DIM,
	typename boost::enable_if<boost::fusion::traits::is_sequence<TestObject> >::type  >
{
   typedef typename boost::fusion::result_of::value_at_c<TestObject,DIM>::type type;
   
   template<typename Env>
   static 
   type&
   call (Env env)
   {
      type& my_object = boost::fusion::at_c<DIM>(env);
      return my_object;
   }
   
};




// ##########################################################################################################
//  phoenix environment accessors
//
//

template<unsigned long DIM>
struct accessN
{
   template <typename C>
   struct result
   {
      typedef typename access_selector<C, DIM>::type type;
   };

   template <typename C>
   typename result<C>::type 
   operator()(C& c) const
   {
      return access_selector<C, DIM>::call(c);
   }
};
boost::phoenix::function<accessN<0> > const access  = accessN<0>();
boost::phoenix::function<accessN<0> > const access0 = accessN<0>();
boost::phoenix::function<accessN<1> > const access1 = accessN<1>();
boost::phoenix::function<accessN<2> > const access2 = accessN<2>();



template<unsigned long DIM>
struct accessN_actor
{
  typedef boost::mpl::true_ no_nullary;   // [boost][phoenix][mpl].. avoid the evaluation of nullary operator()

   template <typename Env>
   struct result
   {
      typedef typename boost::fusion::result_of::at_c<typename Env::args_type, 0>::type  CurrentEvalType;
      typedef typename access_selector< CurrentEvalType, DIM >::type                     type;

   };

   template <typename Env>
   typename result<Env>::type 
   eval(Env const& env)  const
   {
      typedef typename boost::fusion::result_of::at_c<typename Env::args_type, 0>::type  CurrentEvalType;
      typedef typename access_selector< CurrentEvalType, DIM >::type                     SelectedObject;

      CurrentEvalType&  accessor = (boost::fusion::at_c<0>(env.args()));
      SelectedObject&   sel_obj  = access_selector<CurrentEvalType, DIM>::call(accessor);
      return sel_obj;
   }
};

namespace
{
boost::phoenix::actor<accessN_actor<0> > acc =  boost::phoenix::actor<accessN_actor<0> >();
boost::phoenix::actor<accessN_actor<0> > acc0 =  boost::phoenix::actor<accessN_actor<0> >();
boost::phoenix::actor<accessN_actor<1> > acc1 =  boost::phoenix::actor<accessN_actor<1> >();
boost::phoenix::actor<accessN_actor<2> > acc2 =  boost::phoenix::actor<accessN_actor<2> >();
}


// ######
//  phoenix environment size actor
//
//
template<unsigned long DIM>
struct actor_size
{
   typedef boost::mpl::true_ no_nullary;   
   
   template <typename Env>
   struct result
   {
      typedef size_t type;   // size type
   };

   template <typename Env>
   typename result<Env>::type 
   eval(Env const& env)  const
   {
      typedef typename boost::fusion::result_of::at_c<typename Env::args_type, 0>::type  CurrentEvalType;
      typedef typename access_selector< CurrentEvalType, DIM >::type                     SelectedObject;
      
      CurrentEvalType&  accessor = (boost::fusion::at_c<0>(env.args()));
      SelectedObject&   sel_obj  = access_selector<CurrentEvalType, DIM>::call(accessor);
      
      return sel_obj.size();
   }
};

namespace
{
boost::phoenix::actor<actor_size<0> > asize  =  boost::phoenix::actor<actor_size<0> >();
boost::phoenix::actor<actor_size<0> > asize0 =  boost::phoenix::actor<actor_size<0> >();
boost::phoenix::actor<actor_size<1> > asize1 =  boost::phoenix::actor<actor_size<1> >();
}



// ##########################################################################################################
//  quantity accessors
//
//
template<typename Container>
struct quan_access
{
   quan_access(Container& container, std::string quan_name):container(container), quan_name(quan_name) {}

  typedef boost::mpl::false_ no_nullary;   // [boost][phoenix][mpl].. avoid the evaluation of nullary operator()

   template <typename C>
   struct result
   {
      typedef typename Container::mapped_type::mapped_type& type;
   };

   template <typename Env>
   typename result<Env>::type 
   eval(Env const& env)  const
   {
      typedef typename boost::fusion::result_of::at_c<typename Env::args_type, 0>::type  CurrentEvalType;
      typedef typename gsse::traversable_object_selector< CurrentEvalType >              SelectedObject;
  
   
      CurrentEvalType  accessor = (boost::fusion::at_c<0>(env.args()));

//       std::cout << "## test: " << std::endl;
//       dump_type<typename Env::args_type>();
//       dump_type<CurrentEvalType>();
//       dump_type<Container>();
//      std::cout << "acc: " << accessor << std::endl;

      return container[ quan_name ][ accessor ];
   }


   Container&  container;
   std::string quan_name;
};


template<typename Container>
struct quan_access2
{
   quan_access2(Container& container, std::string quan_name):container(container), quan_name(quan_name) {}


  typedef boost::mpl::true_ no_nullary;   // [boost][phoenix][mpl]..  evaluate nullary operator()

   template <typename C>
   struct result
   {
      typedef  double& type;
   };


   template <typename Env>
   typename result<Env>::type
   eval(Env const& env) const
   {
      typedef typename boost::fusion::result_of::at_c<typename Env::args_type, 0>::type  CurrentEvalType;
      dump_type<CurrentEvalType>();
      
      typedef typename CurrentEvalType::eval_type eval_type;
      dump_type<eval_type>();
      eval_type eval_t;
      CurrentEvalType  accessor = (boost::fusion::at_c<0>(env.args()));


//       std::cout << "in arg1 .. " << std::endl;
//       std::cout << "quan name: " << quan_name << std::endl;
//       std::cout << "val: " << container[quan_name][0] << std::endl;
      return container[quan_name][0];
   }

   Container&  container;
   std::string quan_name;
};


template<typename Container>
struct quan_access_3
{
   quan_access_3(Container& container, std::string quan_name):container(container), quan_name(quan_name) {}

  typedef boost::mpl::false_ no_nullary;   // [boost][phoenix][mpl].. avoid the evaluation of nullary operator()

   template <typename C>
   struct result
   {
      typedef  double& type;
   };

   template <typename Env>
   typename result<Env>::type 
   eval(Env const& env)  const
   {
//      dump_type<Env>();

      typedef typename boost::fusion::result_of::at_c<typename Env::args_type, 0>::type  CurrentEvalType;
      typedef typename gsse::traversable_object_selector< CurrentEvalType >              SelectedObject;

      CurrentEvalType  accessor = (boost::fusion::at_c<0>(env.args()));
//      dump_type<CurrentEvalType>();

      return container[ SelectedObject::call(accessor) ][quan_name];
   }

   Container&  container;
   std::string quan_name;
};



template<typename Container>
struct quan_access_4
{
   quan_access_4(Container& container, std::string quan_name):container(container), quan_name(quan_name) {}

   typedef boost::mpl::true_ no_nullary;  // has to be called by an argument

   template <typename Env>
   struct result
   {
      typedef typename Container::mapped_type::mapped_type& type;
//      typedef std::string& type;
   };

   template <typename Env>
   typename result<Env>::type 
   eval(Env const& env)  const
   {
      dump_type<Env>();
      typedef typename boost::fusion::result_of::at_c<typename Env::args_type, 0>::type  CurrentEvalType;

//      CurrentEvalType  accessor = (boost::fusion::at_c<0>(env.args())).eval(env);
//      CurrentEvalType  accessor = eval(env);
      long accessor = (boost::fusion::at_c<0>(env.args())).eval( boost::fusion::at_c<0>(env.args())  );  // [RH][TODO]

//      return container[ accessor ][quan_name];
   }

   Container&  container;
   std::string quan_name;
};

//
// ########################################
//



namespace result_of{
namespace extension{



// ### GSSE v0.2 traversal protocol  .. (plain) container
//
//  a (plain) container contains only a single data set
//   gsse name: intrinsic traversal
//  hence, no dimensional access is required (DimensionPack is not / cannot be  used)
//

template<typename ContainerType, typename DimensionPack=void, typename EvalType=void>
struct decode_traversal
{
   template<typename Env>
   struct apply
   {
      typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type                           EnvContainer;                  
      typedef typename gsse::traversable_object_selector<EnvContainer>                              SelectedObject;
   
      typedef typename gsse::sequence_selector<typename SelectedObject::type, DimensionPack>::type  TraversalObject;   
// [RH][new] automatic const/value deduction
      typedef typename gsse::result_of::value<TraversalObject>::type TypeInside;


      static 
      TraversalObject&
      call(Env env)
      {
         TraversalObject& traversal_object = gsse::sequence_selector<typename SelectedObject::type, DimensionPack>::call
            (SelectedObject::call(  boost::fusion::at_c<0>( env.args()) ));    // [RH][debug][path][XX1]
         return traversal_object; 
      }

       static 
       TraversalObject const&
       call2(Env const& env)
       {
          TraversalObject const& traversal_object = gsse::sequence_selector<typename SelectedObject::type, DimensionPack>::call
             (SelectedObject::call(  boost::fusion::at_c<0>( env.args()) ));    // [RH][debug][path][XX1]
          return traversal_object; 
       }


      static 
      TypeInside
      call2(typename TraversalObject::iterator the_it)
      {
         TypeInside new_it = (*the_it);
         return new_it;
      }

   };
};





// ######################################################################################################
//  associative sequence (meta)-functions
//
//
template<typename TestObject, typename DimensionPack, typename FusionTag>
struct associative_sequence_selector
{
   typedef TestObject type;
   
   template<typename T0>
   static 
   type&
   call (T0& t0)   
   {
      return t0;
   }
};


template<typename TestObject, typename DimensionPack>
struct associative_sequence_selector<TestObject, DimensionPack,  boost::fusion::map_tag>
{
   typedef typename boost::fusion::result_of::value_at_key<TestObject, DimensionPack>::type      type;
   
   template<typename T0>
   static 
   type&
   call (T0& t0)  
   {              
      //std::cout << "decode traversal at location: " << DimensionPack::value << std::endl;

      type& traversal_object = boost::fusion::at_key<DimensionPack>(t0);
      return traversal_object;
   }
};


template<typename TestObject, typename DimensionPack>
struct associative_sequence_selector<TestObject, DimensionPack,  boost::fusion::vector_tag>
{
   typedef typename boost::fusion::result_of::value_at_c<TestObject, 1>::type      FirstType;
   typedef typename boost::fusion::result_of::value_at_key<FirstType, DimensionPack>::type      type;
   

   template<typename T0>
   static 
   type&

   call (T0& t0)  
   {              
      //std::cout << "decode traversal at location: " << DimensionPack::value << std::endl;

      type& traversal_object = boost::fusion::at_key<DimensionPack>(boost::fusion::at_c<1>(t0));
      return traversal_object;
   }
};


// ==============================================================
//  the "decode_traversal_gsse" 
//   is called by the associative traversal accessors
//  here a special treatment for 
//   no key
//   fusion::map
//   fusion::vector
//  is implemented
//
template<typename ContainerType, typename DimensionPack>
struct decode_traversal_gsse
{
   static const int IndexEnvNext = 1;
	
   template<typename Env>
   struct apply
   {
      typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type                               EnvContainer;           
      
      typedef typename associative_sequence_selector<EnvContainer,DimensionPack, typename EnvContainer::fusion_tag>::type  TraversalObject;
      typedef typename TraversalObject::value_type  TypeInside;                          

      static 
      TraversalObject& 
      call(Env env )
      {
         TraversalObject& traversal_object =  associative_sequence_selector<EnvContainer,DimensionPack, typename EnvContainer::fusion_tag>::call(boost::fusion::at_c<0>(env.args())); 
         return traversal_object; 
      }
   };
};


}  //namespace extension
} // namespace result_of
}//namespace gsse


#endif





// // [RH][TODO] 
// //   not used right now due to direct "derivation" from boost::fusion::vector
// //

// // ### GSSE v0.2 traversal protocol  .. cell complex 
// //
// //   decode specialization to cope with collections of containers (concept: cell complex)
// //    DimensionPack is used to select a container out of the cell complex
// //   e.g.,  for a 4D cell complex
// //    DIM_0D::  vertex         fiber bundle  (4-facet)
// //    DIM_1D::  edge           fiber bundle  (3-facet)
// //    DIM_2D::  sub-sub cell   fiber bundle  (2-facet)
// //    DIM_3D::  sub cell       fiber bundle  (1-facet)
// //    DIM_4D::  cell           fiber bundle  (0-facet)
// //
// //   gsse names: intrinsic traversal
// //             : deduced   traversal
// //   required concept: selection/access 
// //

// // template<typename ContainerType, typename DimensionPack=void, typename EvalType=void>
// // struct decode_traversal
// template<typename ContainerType, typename DimensionPack>
// struct decode_traversal<ContainerType, DimensionPack, 
// 	typename boost::enable_if<gsse::is_gsse_container<typename ContainerType::ContainerProperties> >::type >
// {
//    static const int DIM = DimensionPack::value;
//    static const int IndexEnvNext = 1;
	
//    template<typename Env>
//    struct apply
//    {
//       typedef typename boost::mpl::at_c<typename Env::args_type, 0>::type                                      EnvContainer;           
// //      typedef typename boost::fusion::result_of::value_at_c<typename EnvContainer::ContainerNData, DIM>::type  PreSelectedObject;          
//       typedef typename boost::fusion::result_of::value_at_c<EnvContainer, DIM>::type  PreSelectedObject;          
//       typedef typename gsse::traversable_object_selector<PreSelectedObject>                                    SelectedObject;
      
//       typedef typename SelectedObject::type         TraversalObject;
//       typedef typename TraversalObject::value_type  TypeInside;                          
      
//       static 
//       TraversalObject&
//       call(Env env )
//       {
//          // std::cout << "### decode for dimensionpack ##  " << std::endl;

// // [RH][TODO] .. enable common protocol	  
// //		TraversalObject& traversal_object = gsse::sequence_selector<typename SelectedObject::type, DimensionPack>::call
// //   			(SelectedObject::call(  boost::fusion::at_c<0>( env.args()) ));
// //
//          TraversalObject& traversal_object =  boost::fusion::at_c<DIM>(boost::fusion::at_c<0>(env.args()));  // for all other data structures

// //         TraversalObject& traversal_object =  boost::fusion::at_c<DIM>(boost::fusion::at_c<0>(env.args()).cc_data);   // for GSSE's cell complex data structure
 
//          return traversal_object; 
//       }

//       static 
//       TypeInside
//       call2(typename TraversalObject::iterator the_it)
//       {
//          TypeInside new_it = (*the_it);
//          return new_it;
//       }

//    };
// };
