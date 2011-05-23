
#include <iostream>
#include <vector>
#include <functional>
#include <numeric>

#include "viennagrid/domain.hpp"
#include "viennautils/dumptype.hpp"

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/fold.hpp>

#include <boost/any.hpp>
#include <boost/function.hpp>
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
   double operator()(wrapper& t) 
   {
      std::cout << "foo1 here .." << std::endl;
   }
};

struct foo2 
{
   double operator()(wrapper& t) 
   {
      std::cout << "foo2 here .." << std::endl;
   }
};

struct foo3 
{
   double operator()(wrapper& t) 
   {
      std::cout << "foo3 here .." << std::endl;
   }
};

//template<typename IniT>
//struct caller 
//{
//   caller(IniT init):init(init) {}

//   IniT init;

//   template<typename Functor>
//   void operator()(Functor functor) 
//   {
//      functor(init);
//   }
//};

struct caller{

   template<typename T>
   void operator()(T t)
   {
      wrapper mywrapper;
      t(mywrapper);
   }
};

struct sfoo1
{
   int operator()(int x)
   {
      std::cout << "sfoo1 here - x: " << x << std::endl;
   }
};

struct sfoo2
{
   int operator()(int x)
   {
      std::cout << "sfoo2 here - x: " << x << std::endl;
   }
};


int main()
{
   wrapper mywrapper;

   std::vector< boost::function<int (int)> >    sequence;
   sequence.push_back(sfoo1());
   sequence.push_back(sfoo2());   
   
   
   sequence[0](12);
   sequence[1](3);   
   
 //  std::for_each(sequence.begin(), sequence.end(), caller());

   return 0;
}

















