
#include <iostream>
#include <vector>
#include <functional>
#include <numeric>

#include "viennagrid/domain.hpp"
#include "viennautils/dumptype.hpp"

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



struct foo 
{
   virtual double operator()(wrapper& t) 
   {
      std::cout << "foo here .." << std::endl;
   }
};

struct foo1 : foo 
{
   double operator()(wrapper& t) 
   {
      std::cout << "foo1 here .." << std::endl;
   }
};

struct foo2 : foo 
{
   double operator()(wrapper& t) 
   {
      std::cout << "foo2 here .." << std::endl;
   }
};

struct foo3 : foo 
{
   double operator()(wrapper& t) 
   {
      std::cout << "foo3 here .." << std::endl;
   }
};

struct caller 
{
   template<typename State, typename PtrFunctorT>
   double
   operator()(State state, PtrFunctorT functor) 
   {
      //return (*functor)(state);
   }
};


//template<typename IniT>
//struct caller 
//{
//   caller(IniT init):init(init) {}

//   IniT init;

//   template<typename PtrFunctorT>
//   void operator()(PtrFunctorT pfunctor) 
//   {
//      (*pfunctor)(init);
//   }
//};


int main()
{
   wrapper mywrapper;

   std::vector< boost::shared_ptr<foo> >     functor_sequence;

   boost::shared_ptr<foo1>  pfoo1(new foo1);
   boost::shared_ptr<foo2>  pfoo2(new foo2);
   boost::shared_ptr<foo3>  pfoo3(new foo3);      

   functor_sequence.push_back(pfoo1);
   functor_sequence.push_back(pfoo2);   
   functor_sequence.push_back(pfoo3);

   //caller mycaller;

   //std::accumulate(functor_sequence.begin(), functor_sequence.end(), mywrapper, caller());
//   std::for_each(functor_sequence.begin(), functor_sequence.end(), caller<wrapper>(mywrapper));




//   typedef viennagrid::domain<viennagrid::config::triangular_2d>     domain_22u_type;
//   typedef viennagrid::domain<viennagrid::config::triangular_3d>     domain_23u_type;
//   typedef viennagrid::domain<viennagrid::config::tetrahedral_3d>    domain_33u_type;

//   typedef boost::shared_ptr<>            dynamic_domain_type;
//   dynamic_domain_type vgrid_dynamic = dynamic_domain_type(new );


   return 0;
}

















