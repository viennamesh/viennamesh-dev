
#include<iostream>

#include "viennagrid/domain.hpp"

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/fold.hpp>

#include <boost/shared_ptr.hpp>

namespace tag {
struct wrapper {};
struct foo {};
}

struct void_ {};

struct wrapper
{
   typedef tag::wrapper type;
};


namespace traits {

template<typename T>
struct tag_of 
{
   //typedef typename traits::details::tag_of_impl<typename T::type>::type type;
   typedef typename T::type type;
};

template<>
struct tag_of  <double>
{
   //typedef typename traits::details::tag_of_impl<typename T::type>::type type;
   typedef tag::foo type;
};

 } // end namespace traits




struct foo1 
{
   typedef tag::foo type;
   typedef double result_type;   

   template<typename T>
   result_type operator()(T& t)
   {
      return eval(t, typename traits::tag_of<T>::type());
   }
   template<typename T>
   result_type eval(T& t, tag::wrapper const&)
   {
      std::cout << "foo1::wrapper" << std::endl;
      return 1.0;
   }
   template<typename T>
   result_type eval(T& t, tag::foo const&)
   {
      std::cout << "foo1::foo" << std::endl;
      return 1.0;      
   }   
};

struct foo2 
{
   typedef tag::foo type;
   typedef double result_type;   

   template<typename T>
   result_type operator()(T& t)
   {
      return eval(t, typename traits::tag_of<T>::type());
   }
   template<typename T>
   result_type eval(T& t, tag::wrapper const&)
   {
      std::cout << "foo2::wrapper" << std::endl;   
      return 1.0;      
   }
   template<typename T>
   result_type eval(T& t, tag::foo const&)
   {
      std::cout << "foo2::foo" << std::endl;   
      return 1.0;      
   }   
};

struct foo3
{
   typedef tag::foo type;
   typedef double result_type;

   template<typename T>
   result_type operator()(T& t)
   {
      return eval(t, typename traits::tag_of<T>::type());
   }
   template<typename T>
   result_type eval(T& t, tag::wrapper const&)
   {
      std::cout << "foo3::wrapper" << std::endl;   
      return 1.0;      
   }
   template<typename T>
   result_type eval(T& t, tag::foo const&)
   {
      std::cout << "foo3::foo" << std::endl;   
      return 1.0;      
   }   
};

struct caller
{
   typedef double result_type;
   
   template<typename State, typename Functor>
   result_type operator()(State state, Functor functor) 
   {
      return functor(state);
   }
};

int main()
{
   wrapper mywrapper;

   double result = boost::fusion::fold(
      boost::fusion::make_vector(foo1(), foo2(), foo3()),
      mywrapper,
      caller()
   );
   
   std::cout << "result: " << result << std::endl;






//   typedef viennagrid::domain<viennagrid::config::triangular_2d>     domain_22u_type;
//   typedef viennagrid::domain<viennagrid::config::triangular_3d>     domain_23u_type;
//   typedef viennagrid::domain<viennagrid::config::tetrahedral_3d>    domain_33u_type;

//   typedef boost::shared_ptr<>            dynamic_domain_type;
//   dynamic_domain_type vgrid_dynamic = dynamic_domain_type(new );


   return 0;
}

















