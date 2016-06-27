/* ============================================================================
   Copyright (c) 2003-2008 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2004-2008 Philipp Schwaha                      philipp@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_ALGORITHM_MODIFICATION_HH)
#define GSSE_ALGORITHM_MODIFICATION_HH

// *** system includes
#include <functional>
#include <limits>
#include <algorithm>
#include <cstdlib>

// *** BOOST includes
#include <boost/assert.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/accumulate.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/iterator.hpp>

// *** GSSE includes
#include "gsse/exception/exceptions.hpp"
#include "gsse/util/resize.hpp"
#include "gsse/util/size.hpp"
#include "gsse/traversal/actor.hpp"


// ############################################################
//
namespace gsse
{


template<typename Container, typename Object>
struct inserter_class
{
   inserter_class(Container& container, const Object& objecth):container(container), objecth(objecth) {}

   template <typename Data>
   void operator()(const Data& data)  
   {
      container[data].insert(objecth);
   }
private:
   Container&     container;
   const Object&  objecth;

};
template<typename Container, typename Object>
inserter_class<Container, Object> inserter(Container& container, const Object& object)
{
   return inserter_class<Container,Object>(container, object);
}


template <typename Elements, typename Inserter>
void for_all(Elements const& elements, Inserter inserter)
{
   std::for_each(elements.begin(), elements.end(), inserter);
}




namespace algorithm
{

template<typename Initiator>
struct traverse_further
{
   traverse_further(Initiator& initiator):initiator(initiator)
      {}

   template <typename Data>
   void operator()(Data& data) const
   {
      std::generate(data.begin(), data.end(), initiator);
   }

   Initiator initiator;
};

template <typename Container, typename Initiator>
void init_2d(Container& container, Initiator initiator)
{
   boost::fusion::for_each(container, traverse_further<Initiator>(initiator));
}



template <typename Container, typename Initiator>
void init(Container& container, Initiator initiator)
{
   std::generate(container.begin(), container.end(), initiator);
}



template<typename Numeric>
struct constant_value
{
   constant_value(Numeric nc):nc(nc){}

   Numeric operator()()
   {
      return nc;
   }

   Numeric nc;
};
template<typename Numeric>
struct constant   //shortcut
{
   constant(Numeric nc):nc(nc){}

   Numeric operator()()
   {
      return nc;
   }

   Numeric nc;
};


struct zero_object
{
   long operator()()
   {
      return 0;
   }


};

namespace
{
zero_object zero = zero_object();
}


template<typename Numeric>
struct increment
{
  increment(Numeric nc, Numeric inc=1):nc(nc), inc(inc){}

   Numeric operator()()
   {
      return nc+=inc;
   }

  Numeric nc;
  Numeric inc;
};


template<typename Numeric>
increment<Numeric> inc(Numeric num)
{
   return increment<Numeric>(num);
}

template<typename Numeric>
increment<Numeric> inc2(Numeric num, Numeric Inc)
{
  return increment<Numeric>(num,Inc);
}



template<typename Numeric>
struct random
{
  random(Numeric nc, Numeric inc=1):nc(nc), inc(inc)
  {
    
  }

   Numeric operator()()
   {
     return static_cast<Numeric>(std::rand()%10);
   }

  Numeric nc;
  Numeric inc;
};


template<typename Numeric>
random<Numeric> rand(Numeric num, Numeric Inc)
{
  return random<Numeric>(num,Inc);
}



struct product
{
   template<typename Sig>
   struct result;
  
   template<typename Lhs, typename Rhs>
   struct result<product(Lhs,Rhs)>
      : boost::remove_reference<Lhs>
      {};

   // [RH] metaprogramming backward compability
   // [RH][TODO][temporary] .. remove this, if we use boost 1.35
   template<typename Sig>
   struct result_type;
   template<typename Lhs, typename Rhs>
   struct result_type<product(Lhs,Rhs)>
      : boost::remove_reference<Lhs>
      {};


   template<typename Lhs, typename Rhs>
   Lhs operator()(const Lhs& lhs, const Rhs& rhs) const
   {
      return lhs * rhs;
   }
};

// [RH][TODO][P6]
//   use parametrized Numeric
//
struct product_m
{
   product_m(long num):num(num) {}

   template<typename Sig>
   struct result;
  
   template<typename Lhs, typename Rhs>
   struct result<product_m(Lhs,Rhs)>
      : boost::remove_reference<Lhs>
      {};

   // [RH] metaprogramming backward compability
   // [RH][TODO][temporary] .. remove this, if we use boost 1.35
   template<typename Sig>
   struct result_type;
   template<typename Lhs, typename Rhs>
   struct result_type<product_m(Lhs,Rhs)>
      : boost::remove_reference<Lhs>
      {};


   template<typename Lhs, typename Rhs>
   Lhs operator()(const Lhs& lhs, const Rhs& rhs) const
   {
      return (lhs-num) * rhs;
   }
   
protected:
   long num;
};



struct sum
{
   template<typename Sig>
   struct result;
  
   template<typename Lhs, typename Rhs>
   struct result<sum(Lhs,Rhs)>
      : boost::remove_reference<Lhs>
      {};

   // [RH] metaprogramming backward compability
   // [RH][TODO][temporary] .. remove this, if we use boost 1.35
   template<typename Sig>
   struct result_type;
   template<typename Lhs, typename Rhs>
   struct result_type<sum(Lhs,Rhs)>
      : boost::remove_reference<Lhs>
      {};

   template<typename Lhs, typename Rhs>
   Lhs operator()(const Lhs& lhs, const Rhs& rhs) const
   {
      return lhs + rhs;
   }
};

struct difference
{
   template<typename Sig>
   struct result;
  
   template<typename Lhs, typename Rhs>
   struct result<difference(Lhs,Rhs)>
      : boost::remove_reference<Lhs>
      {};

   // [RH] metaprogramming backward compability
   // [RH][TODO][temporary] .. remove this, if we use boost 1.35
   template<typename Sig>
   struct result_type;
   template<typename Lhs, typename Rhs>
   struct result_type<difference(Lhs,Rhs)>
      : boost::remove_reference<Lhs>
      {};

   template<typename Lhs, typename Rhs>
   Lhs operator()(const Lhs& lhs, const Rhs& rhs) const
   {
      return lhs - rhs;
   }
};


// calculates a tensor-product
//  @param: c_index             container_index
//  @param: container_dim_acc   accumulative dimension values
//
template <typename Container>
typename Container::value_type 
tensorproduct(const Container& c_index, 
              const Container& c_dim, 
              const Container& c_acc)
{
   if (c_index.size() == 0)
      return 0;

   typedef typename Container::value_type Numeric;
   using boost::fusion::end;
   using boost::fusion::prior;
   using boost::fusion::deref;

   Numeric value =   
      boost::fusion::accumulate(
         boost::fusion::transform(c_index, c_acc, product()), 
         Numeric(0), 
         sum()
         );

  
#ifdef GSSETEST

   if (c_index.size() > 2)
   {
      Numeric max_value_ld = boost::fusion::deref(boost::fusion::prior(boost::fusion::end(c_dim)));
      Numeric max_value_la = boost::fusion::deref(boost::fusion::prior(boost::fusion::end(c_acc)));

#ifdef DEBUG_NUMERIC
   std::cout << " numeric limit:     " 
             << (std::numeric_limits<Numeric>::max)() << std::endl;
   std::cout << " numeric limit / l: " 
             << (std::numeric_limits<Numeric>::max)() / max_value_la << std::endl;
#endif

   if ( (std::numeric_limits<Numeric>::max)() / max_value_la < max_value_ld)
   {
      throw numerical_calculation_error(" tensorproduct:: max_value_la ("+
                     boost::lexical_cast<std::string>(max_value_la) +
                     ") is greater than numerical limit from Numerics ("+
                     boost::lexical_cast<std::string>((std::numeric_limits<Numeric>::max)()) +
                     ") ");
   }
   
   if ( max_value_la * max_value_ld < value )
   {
      throw dimension_problem(" tensorproduct:: val ("+
                     boost::lexical_cast<std::string>(value) +
                     ") is greater than dimensionality (" +
                     boost::lexical_cast<std::string>(max_value_la * max_value_ld) +
                     ") ");
   }
   }
#endif

   return value;
}

template <typename Container>
Container tensorproduct_inverse(typename Container::value_type value, 
                                const Container& c_dim, 
                                const Container& c_acc)
{
   if (c_dim.size() == 0)
      return c_dim;

   typedef typename Container::value_type Numeric;
   Container c_index;

   for (long cnt = 1; cnt <= gsse::size(c_dim) ; ++cnt)
   {
      c_index[cnt-1] = (value / c_acc[cnt-1]) % (c_dim[cnt-1]);

//       std::cout << "v:  " << value << std::endl;
//       std::cout << "ca: " << c_acc[cnt-1] << std::endl;
//       std::cout << "cd: " << c_dim[cnt] << std::endl;
//       std::cout << "/:  " << value / c_acc[cnt-1] << std::endl;;
//       std::cout << "%:  " << (value / c_acc[cnt-1]) % (c_dim[cnt]) << std::endl;
//       std::cout << std::endl;
   }

   return c_index;

}

//
// dimension_accumulative
//  calculates the dimensional entries of an implicit topology  
//    in an accumulative way
//  has to be calculated to inverse the tensor product calculation
//  and start with  1
//   then           container_dim[0] 
//   for back transformation 
template <typename Container>
Container dimension_accumulative(const Container& c_dim)
{
   using boost::fusion::at_c;

   if (c_dim.size() == 0)
      return c_dim;

   Container c_acc;   // topology::implicit dimensions accumulative

   at_c<0>(c_acc) = 1;
   if (c_dim.size() < 2)
      return c_acc;

   at_c<1>(c_acc) =  at_c<0>(c_dim);

   if (c_dim.size() < 3)
      return c_acc;


   // calculate the product for each entry, 
   //
   // [RH][TODO][P4] .. use boost::fusion::iterators for +1, ..
   //
   std::transform(c_dim.begin() +1 , c_dim.end()   -1, 
                  c_acc.begin() +1 , c_acc.begin() +2, gsse::algorithm::product() );

   return c_acc;
}


// ##############################################################################
//
// transform_zip_2d 
//   models the stl/boost zip concept 
//   where iterators from two sequences  are used to generate a parallel-traverse
//    
// Func: a STL functor
//
template<typename ContainerSource, typename ContainerTarget, typename Functor>
long transform_zip_2d(ContainerSource const& container_source,
                      ContainerTarget&      container_target,
                      Functor func)
{
//   dump_type<Functor>();

   typename ContainerSource::const_iterator                it_source;
   typename ContainerSource::value_type::const_iterator    it_source2;
   typename ContainerTarget::iterator                it_target;
   typename ContainerTarget::value_type::iterator    it_target2;
   
   gsse::resize( container_source.size() ) (container_target);
   
   it_source = container_source.begin();
   it_target = container_target.begin();
   for (size_t ci = 0; ci < gsse::size(container_source); ++ci)
   {
      it_source2 = (*it_source).begin() ;
      it_target2 = (*it_target).begin() ;
      
      gsse::resize( container_source[ci].size() ) (container_target[ci]);
      
      for (size_t di = 0; di < gsse::size(container_source[ci]); ++di)
      {
         func(*it_target2, *it_source2);
         
         ++it_source2;
         ++it_target2;
      }
      ++ it_source;
      ++ it_target;
   }

   return 0;
}

// ##############################################################################
//
// generic unique algorithm
//   which cleans up a container of double entries
//
template<typename Container>
long unique(Container& container)
{
   std::cout << "### gsse::algorithm::unique !! not YET implemented .. only for std::vector" << std::endl;
   return 0;
}



// generic unqiue algorithm
//  specialized for random access container  [TODO]
//
template<typename T1>
long unique(std::vector<T1>& container)
{
   std::sort  (container.begin(), container.end());
   size_t new_size = std::unique(container.begin(), container.end()) - container.begin();
   container.resize( new_size );
   
   return 0;
} 


template < typename ReturnType, typename Container >
ReturnType 
root_mean_square( Container & cont )
{
   namespace phoenix = boost::phoenix;

   ReturnType temp = 0;
   gsse::traverse()
   [
      phoenix::ref(temp) += phoenix::arg_names::_1 * phoenix::arg_names::_1
   ](cont);
   
   return std::sqrt( temp / gsse::size(cont) );
}


}  // namespace algorithm
}  // namespace gsse


#endif

