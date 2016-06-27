/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_LAMBDA_QUAN_ACCESS_2008_HH
#define GSSE_LAMBDA_QUAN_ACCESS_2008_HH

// *** BOOST includes   
//
// [RH] adaptation to boost >= 1.37
#include <boost/spirit/home/phoenix.hpp>   

// old boost < 1.37
// #include <boost/spirit/phoenix/statement.hpp> // V2 .. ?? has to be the first include !!!!
// #include <boost/spirit/phoenix/actor.hpp>     // V2
// #include <boost/spirit/phoenix/core.hpp>
// #include <boost/spirit/phoenix/operator.hpp>
// #include <boost/spirit/phoenix/scope.hpp>
// #include <boost/spirit/phoenix/scope/local_variable.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/at.hpp>

// *** GSSE includes
//
#include "gsse/exceptions.hpp"


namespace gsse
{

template <typename FuncArguments, unsigned int IndexSpaceDepth>
struct stl_access_impl
{    
   explicit stl_access_impl(FuncArguments& func_arg, std::string quan_name) : func_arg(func_arg), quan_name(quan_name) {}

   typedef boost::mpl::false_ no_nullary;   
   template <typename Arg>
   struct result
   {
      typedef typename FuncArguments::storage_type&    type;
   };
      
   template <typename Arg>
   typename result<Arg>::type eval(Arg const& arg)  const
   {
//	dump_type<Arg>();
// gsse::lambda::quan access
//   return func_arg(boost::fusion::at_c<0>(arg.args()), quan_name)(0, 0);


      // ### stl :: container :: quan access
      //    implicit index space
      //
      if (IndexSpaceDepth == 0)
         return func_arg(  boost::fusion::at_c<0>(arg.args())  );

      // ### gsse :: lambda :: quan access
      //   explicit index space
      //
//       else if (IndexSpaceDepth == 1)    
//          return func_arg(boost::fusion::at_c<0>(arg.args()), quan_name);
         
   }

private:
   FuncArguments&    func_arg;   // the actual domain
   std::string       quan_name;
};

// ===========================================
// object generator 
//
template <typename FuncArguments, unsigned int IndexSpaceDepth>
boost::phoenix::actor<stl_access_impl<FuncArguments,IndexSpaceDepth> > 
stl_access(FuncArguments& func_arg, std::string quan_name)   
{
   return boost::phoenix::actor<stl_access_impl<FuncArguments,IndexSpaceDepth> >  (stl_access_impl<FuncArguments,IndexSpaceDepth>(func_arg, quan_name));
}
   
template <typename FuncArguments, unsigned int IndexSpaceDepth>
struct stl_access_t
{
   typedef boost::phoenix::actor<stl_access_impl<FuncArguments,IndexSpaceDepth> > type;
};
   



/*

template<typename ContainerT>
struct stl_container_accessor
{
  stl_container_accessor(ContainerT& container):container(container) {}

  typename ContainerT::value_type& operator()(typename ContainerT::index_type index)
  {
	return container[index];
  }

protected:
 ContainerT& container;
};

*/




// ##############################################################################################
// ##############################################################################################
   //
   // Boost::phoenix::actors are used to implement quan_create/access lambda actors
   //
   //  the eval method is used, where the actor is first passed as lambda object 
   //  hence no (_1, arg1) is necessary
   //
// ##############################################################################################
// ##############################################################################################
//
template <typename FuncArguments>
struct quan_access_impl
{    
   explicit quan_access_impl(FuncArguments& func_arg, std::string quan_name) : func_arg(func_arg), quan_name(quan_name) {}

   typedef boost::mpl::false_ no_nullary;   // internal:: avoid the evaluation of nullary operator()

   // boost::phoenix::meta function data type 
   //
   template <typename Arg>
   struct result
   {
      typedef typename FuncArguments::storage_type    storage_type;
      typedef typename storage_type::numeric_t&       type;
   };
      
   // [RH][TODO] .. an exception is needed if the wrong topological object is used
   //
   
   
   // boost::phoenix::meta function evaluation routine
   //
   template <typename Arg>
   typename result<Arg>::type eval(Arg const& arg)  const
   {
//      std::cout << " quan access 1.. " << std::endl;
      return func_arg(boost::fusion::at_c<0>(arg.args()), quan_name)(0, 0);
   }

private:
   FuncArguments&    func_arg;   // the actual domain
   std::string       quan_name;
};

// ===========================================
// object generator 
//
template <typename FuncArguments>
boost::phoenix::actor<quan_access_impl<FuncArguments> > 
scalar_quan_access(FuncArguments& func_arg, std::string quan_name)   
{
   return boost::phoenix::actor<quan_access_impl<FuncArguments> >  (quan_access_impl<FuncArguments>(func_arg, quan_name));
}
   
template <typename FuncArguments>
struct scalar_quan_access_t
{
   typedef boost::phoenix::actor<quan_access_impl<FuncArguments> > type;
};
   




template <typename FuncArguments, typename ObjectT>
struct quan_access_safe_impl
{   
   explicit quan_access_safe_impl(FuncArguments& func_arg, std::string quan_name) : func_arg(func_arg), quan_name(quan_name) {}

   typedef boost::mpl::false_ no_nullary;   // internal:: avoid the evaluation of nullary operator()

   // boost::phoenix::meta function data type 
   //
   template <typename Arg>
   struct result
   {
      typedef typename FuncArguments::storage_type    storage_type;
      typedef typename storage_type::numeric_t&       type;
   };
      
   // boost::phoenix::meta function evaluation routine
   //
   template <typename Arg>
   typename result<Arg>::type eval(Arg const& arg)  const
   {
      return func_arg(ObjectT(boost::fusion::at_c<0>(arg.args())), quan_name)(0, 0);
   }

private:
   FuncArguments&    func_arg;   // the actual domain
   std::string       quan_name;
};

// ===========================================
// object generator 
//
template <typename FuncArguments, typename ObjectT>
boost::phoenix::actor<quan_access_safe_impl<FuncArguments, ObjectT> > 
scalar_quan_access_safe(FuncArguments& func_arg, std::string quan_name)   
{
   return boost::phoenix::actor<quan_access_safe_impl<FuncArguments, ObjectT> >  (quan_access_safe_impl<FuncArguments, ObjectT>(func_arg, quan_name));
}
   
template <typename FuncArguments, typename ObjectT>
struct scalar_quan_access_safe_t
{
   typedef boost::phoenix::actor<quan_access_safe_impl<FuncArguments, ObjectT> > type;
};
   





// ##############################################################################################
// ##############################################################################################
//
//
template <typename FuncArguments>
struct quan_create_impl
{    
   explicit quan_create_impl(FuncArguments& func_arg, std::string quan_name) : func_arg(func_arg), quan_name(quan_name)  {  }

   typedef boost::mpl::false_ no_nullary;

   
   // Boost phoenix/fusion return type calculation 
   //
   template <typename Arg>
   struct result
   {
      // get the result type
      //
      typedef typename FuncArguments::storage_type         storage_type;
      typedef typename storage_type::numeric_t&      type;
   };
      
      
   // boost::phoenix actor concept -> has to implement the eval routine
   //
   template <typename Arg>
   typename result<Arg>::type eval(Arg const& arg)  const
   {
      typename result<Arg>::storage_type storage(1, 1, 0);

      // func_arg.add_object_quantity<typename boost::mpl::at_c<typename Arg::args_type, 0>::type >(quan_name);
	 

      func_arg.store_quantity(boost::fusion::at_c<0>(arg.args()), quan_name, storage);
      return func_arg(boost::fusion::at_c<0>(arg.args()), quan_name)(0, 0);
   }
      


private:
   FuncArguments&       func_arg;   // the actual domain
   std::string          quan_name;
      
};

// =============================================
// object generator 
//
template <typename FuncArguments>
boost::phoenix::actor<quan_create_impl<FuncArguments> > 
scalar_quan_create(FuncArguments& func_arg, std::string quan_name)   
{
   return boost::phoenix::actor<quan_create_impl<FuncArguments> >
      (quan_create_impl<FuncArguments>(func_arg, quan_name));
}

template <typename FuncArguments>
struct scalar_quan_create_t
{
   typedef boost::phoenix::actor<quan_create_impl<FuncArguments> > type;
};


// ##############################################################################################
// ##############################################################################################
//
// [RH]  .. NEW gsse coordinate functor
//
template <typename FuncArguments>
struct coord_access_impl
{    
   explicit coord_access_impl(FuncArguments& func_arg) : func_arg(func_arg)  {  }

// enable nullary/zero arguments for convenient lambda style 
//
   typedef boost::mpl::false_ no_nullary;

   template <typename Arg>
   struct result
   {
      typedef typename FuncArguments::point_t  type;
   };
      
   template <typename Arg>
   typename result<Arg>::type eval(Arg const& arg)  const
   {
      return func_arg.get_point(boost::fusion::at_c<0>(arg.args()));
   }
      

private:
   FuncArguments&       func_arg;   // the actual domain
};

// ===========================================================================================
// object generator 
//
template <typename FuncArguments>
boost::phoenix::actor<coord_access_impl<FuncArguments> > 
coord_access(FuncArguments& func_arg)   
{
   return boost::phoenix::actor<coord_access_impl<FuncArguments> >
      (coord_access_impl<FuncArguments>(func_arg));
}
   
// [RH] 200712 modifications.. 
//      the type of the actor
//
template <typename FuncArguments>
struct coord_access_t
{
   typedef boost::phoenix::actor<coord_access_impl<FuncArguments> > type;
};
   
// [RH]  .. NEW gsse coordinate functor :: with ref access
//
template <typename FuncArguments>
struct coord_refaccess_impl
{    
   explicit coord_refaccess_impl(FuncArguments& func_arg) : func_arg(func_arg)  {  }

// enable nullary/zero arguments for convenient lambda style 
//
   typedef boost::mpl::false_ no_nullary;

   template <typename Arg>
   struct result
   {
      typedef typename FuncArguments::point_t&  type;
   };
      
      
   template <typename Arg>
   typename result<Arg>::type eval(Arg const& arg)  const
   {
      return func_arg.get_point(boost::fusion::at_c<0>(arg.args()));
   }
      


private:
   FuncArguments&       func_arg;   // the actual domain
};

// ===========================================================================================
// object generator 
//
template <typename FuncArguments>
boost::phoenix::actor<coord_refaccess_impl<FuncArguments> > 
coord_refaccess(FuncArguments& func_arg)   
{
   return boost::phoenix::actor<coord_refaccess_impl<FuncArguments> >
      (coord_refaccess_impl<FuncArguments>(func_arg));
}
   
template <typename FuncArguments>
struct coord_refaccess_t
{
   typedef boost::phoenix::actor<coord_refaccess_impl<FuncArguments> > type;
};



}  

#endif
