/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

// *** system includes
//
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <cmath>
#include <limits>


// *** BOOST includes
//
#include <boost/array.hpp>
#include <boost/type_traits/remove_reference.hpp>


#include <boost/fusion/include/accumulate.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/fold.hpp>

#include <boost/fusion/sequence/intrinsic.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/container/list/cons.hpp>
#include <boost/fusion/iterator/advance.hpp>
#include <boost/fusion/adapted/array.hpp>
#include <boost/fusion/include/insert.hpp>
#include <boost/fusion/include/push_front.hpp>
#include <boost/fusion/include/next.hpp>
#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/end.hpp>
#include <boost/fusion/include/prior.hpp>
#include <boost/fusion/include/erase.hpp>



#include <boost/fusion/container/vector/convert.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>

#include <boost/detail/lightweight_test.hpp>

// *** GSSE includes
//
#include "gsse/math/power.hpp"
#include "gsse/debug_meta.hpp"   // [RH][TODO][temporary]


// ############################################################
// ############################################################
//
namespace gsse
{

namespace algorithm
{

struct saver
{
   saver(std::ostream& ostr):ostr(ostr) {}
   
   template <typename Pair>
   void operator()(Pair const& data) const
   {
      ostr << data.second << " .. " ;
   }
   
   std::ostream& ostr;
};



template <typename Container>
void store(Container const& container, std::ostream& ostr= std::cout)
{
   boost::fusion::for_each(container, saver(ostr));
   ostr << std::endl;
}


struct printer
{
   printer(std::ostream& ostr):ostr(ostr) {}
   
   template <typename Data>
   void operator()(Data const& data) const
   {
      ostr << data << " .. " ;
   }
   
   std::ostream& ostr;
};



template <typename Container>
void print(Container const& container, std::ostream& ostr= std::cout)
{
   boost::fusion::for_each(container, printer(ostr));
   ostr << std::endl;
}



// [RH][TODO][info] .. for_each const problem for for_each(First const& first)
//
struct loader
{
   loader(std::istream& istr):istr(istr) {}
    
   template <typename Pair>
   void operator()(Pair& data) 
   {
      istr >> data.second;
   }
   
   std::istream& istr;
};

template <typename Stuff>
void load(Stuff& stuff, std::istream& istr= std::cin)
{
//   boost::fusion::for_each(stuff, loader(istr));
}



template<typename Initiator>
struct traverse_further
{
   traverse_further(Initiator& initiator):initiator(initiator)
      {}

   template <typename Data>
   void operator()(Data& data) const
   {
      boost::fusion::for_each(data, initiator);
   }

   Initiator initiator;
};

template <typename Container, typename Initiator>
void init2(Container& container, Initiator& initiator)
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
struct increment
{
   increment(Numeric nc):nc(nc){}

   Numeric operator()()
    {
        return nc++;
    }

   Numeric nc;
};




} // namespace algorithm



// ###############################################################


namespace topology_implicit
{

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



template <typename Container>
typename Container::value_type tensorproduct(const Container& container, const Container& container_dim_acc)
{
   typedef typename Container::value_type Numeric;

//    std::cout << "print AA: " << std::endl;
//    gsse::algorithm::print(container_dim_acc);
   
   Numeric value =  
      boost::fusion::accumulate(
         boost::fusion::transform(container, container_dim_acc, product()), 
         Numeric(0), 
         sum()
         );
   
   return value;
}

template <typename Container>
Container tensorproduct_inverse(typename Container::value_type value, 
                                const Container& container_dim, 
                                const Container& container_acc)
{
   using boost::fusion::at_c;

   if (container_dim.size() == 0)
      return container_dim;


   typedef typename Container::value_type Numeric;
   Container container_index;

   at_c<0>(container_index) = ( value   ) % (at_c<0>(container_dim));

   if (container_dim.size() < 2)
      return container_index;

   for (size_t cnt = 1; cnt < container_dim.size() ; ++cnt)
   {
      container_index[cnt] = (value / container_acc[cnt-1]) % (container_dim[cnt]);
   }

   return container_index;

}

//
// dimension_accumulative
//  calculates the dimensional entries of an implicit topology  in an accumulative way
//  has to be calculated to inverse the tensor product calculation
//  and start with  1
//   then           container_dim[0] 
//   for back transformation 
template <typename Container>
Container dimension_accumulative(const Container& container_dim)
{
   using boost::fusion::at_c;
   using boost::fusion::next;
   using boost::fusion::prior;
   using boost::fusion::begin;
   using boost::fusion::end;

   if (container_dim.size() == 0)
      return container_dim;

   Container container_dim_acc;   // topology::implicit dimensions accumulative

   at_c<0>(container_dim_acc) = 1;
   if (container_dim.size() < 2)
      return container_dim_acc;

   at_c<1>(container_dim_acc) =  at_c<0>(container_dim);

   if (container_dim.size() < 3)
      return container_dim_acc;

   // calculate the product of each entry, 
   //
   // [RH][TODO][P4] .. use boost::fusion::iterators for +1, ..
   //
   std::transform(container_dim.begin() +1 ,   container_dim.end() -1, 
                  container_dim_acc.begin()+1, container_dim_acc.begin()+2, gsse::topology_implicit::product() );

   return container_dim_acc;
}

} // namespace topology_implicit

} // namespace gsse



// ############################################################
// ############################################################
//





namespace tag_person
{
    struct name;
    struct age;

}

typedef boost::fusion::map<
     boost::fusion::pair<tag_person::name, std::string>
   , boost::fusion::pair<tag_person::age,  int> >
person;



void test2()
{
    using namespace boost::fusion;
    {
       person  a_person;

       using namespace tag_person;
       at_key<name>(a_person) = std::string("name");
       at_key<age>(a_person)  = 12;;

       std::ofstream file("testmeout.rh");

       gsse::algorithm::store(a_person, file);

    }


    {
       person  a_person;

       using namespace tag_person;

       std::ifstream file("testmeout.rh");

       gsse::algorithm::load(a_person);

       gsse::algorithm::store(a_person);

    }


    {
       boost::array<int,3> arr = {{1,2,3}};

       std::cout << *begin(arr) << std::endl;
       std::cout << *next(begin(arr)) << std::endl;
       std::cout << *advance_c<2>(begin(arr)) << std::endl;
       std::cout << *prior(end(arr)) << std::endl;
       std::cout << at_c<2>(arr) << std::endl;

    }
}


// ############################################################
//



// ============================
// GSSE V0.1 topology implicit
//



namespace topology_tag_container
{
   struct topo_object;

   struct topology_implicit;

   struct cell_ncube;
   struct cell_simplex;
}





// ==============
//
// calculate cardinality of topological set, for now:
//   ncube
//   simplex 
//
//  type: metafunctions
//
template <unsigned long DIM, typename CellType> 
struct calculate_cardinality_index
{};

template<unsigned long DIM>
struct calculate_cardinality_index<DIM, topology_tag_container::cell_ncube>
{
   static const int value = gsse::power<2,DIM>::value;
};
template<unsigned long DIM>
struct calculate_cardinality_index<DIM, topology_tag_container::cell_simplex>
{
   static const int value = DIM+1;
};


// ==============
//
// creates the topological data types
//  type: metafunctions
//
template<
   template <typename T1, typename T2>   class Container, 
   template <typename TA, unsigned long> class IndexContainer,     // !! concept requires a::     static const number
   typename OldContainer, 
   typename DataType, 
   typename Cell,
   unsigned long DIM
   >
struct create_topology
{
   typedef typename create_topology<
      Container, 
      IndexContainer,
      Container<
         IndexContainer<DataType, 
                        calculate_cardinality_index<DIM, Cell>::value>, 
         OldContainer >, 
      DataType,  
      Cell,
      DIM-1 
      >::type  type;
};
// ===  recursion end
//
template<
   template <typename T1, typename T2>   class Container, 
   template <typename TA, unsigned long> class IndexContainer,     // !! concept requires a::     static const number
   typename OldContainer, 
   typename DataType,
   typename Cell
   >
struct create_topology<Container, IndexContainer, OldContainer, DataType,Cell,0>
{
   typedef Container<
      IndexContainer<DataType, 
                     calculate_cardinality_index<0, Cell>::value>, 
      OldContainer 
      > type; 

};


template<
   template <typename T1, typename T2>   class Container, 
   template <typename TA, unsigned long> class IndexContainer,     // !! concept requires a::     static const number
   typename DataType, 
   typename Cell,
   unsigned long DIM
   >
struct create_topology_helper
{
   typedef typename create_topology<
      Container, 
      IndexContainer,
      Container<
        IndexContainer<DataType, 
                       calculate_cardinality_index<DIM, Cell>::value  >,
        boost::fusion::nil >, 
      DataType,  
      Cell,
      DIM -1
      >::type  type;
};


struct printme
{
   printme(std::ostream& ostr):ostr(ostr) {}
   
   template <typename Data>
   void operator()(Data const& data) const
   {
      ostr << data << " .. " ;
   }
   
   std::ostream& ostr;
};


struct setvalue
{
   setvalue(std::ostream& ostr):ostr(ostr) {}
   
   template <typename Data>
   void operator()(Data const& data) const
   {
      ostr << data << " .. " ;
   }
   
   std::ostream& ostr;
};


// ==============
//


template<unsigned long DIM, typename Cell, typename IndexType>
class topology_container
{
 
public:
   static const unsigned long dim = DIM;

   // create datatypes
   //
   typedef boost::array<IndexType, calculate_cardinality_index<DIM, Cell>::value>   Index;        //[RH][TODO] implicit/ncube calculation .. more general with functor
   typedef boost::array<Index, DIM>   Topo_object;


   // create celltype containers
   //
   typedef typename create_topology_helper<
      boost::fusion::cons,                        // global container 
      boost::array,                               // index container
      IndexType,                                  // type: index
      Cell,                                       // type: cell
      3                                           // start dimension
      >::type Topology_repository_cons;
   // 
   // convert to fusion::vector (for direct access)
   typedef typename boost::fusion::result_of::as_vector< Topology_repository_cons >::type Topology_repository;


   Topology_repository repository;



};


template<unsigned long DIM, typename Cell, typename IndexType=long>
class topology_container_implicit : public topology_container<DIM, Cell, IndexType>
{
   typedef topology_container_implicit<DIM, Cell, IndexType> Self;
public:

   boost::array<IndexType, DIM>   axes;   // implicit topology

   friend std::ostream& operator<<(std::ostream& ostr, Self& self)
   {
      boost::fusion::for_each(self.axes, printme(ostr));
   }
};


template<unsigned long DIM, typename Cell, typename IndexType=long>
class topology_container_explicit : public topology_container<DIM, Cell, IndexType>
{
   typedef topology_container_explicit<DIM, Cell, IndexType> Self;
   typedef topology_container<DIM, Cell, IndexType>          Self_base;

   // compiler: use the correct datatype
   typedef typename Self_base::Topology_repository Topology_repository;  
   

public:


   typedef   boost::fusion::vector<  
     std::vector< typename boost::fusion::result_of::value_at_c<Topology_repository,0>::type >,       // X0 container .. type: vertex
     std::vector< typename boost::fusion::result_of::value_at_c<Topology_repository,DIM-1>::type >    // XD container .. type: cell
    > topo_container;

   friend std::ostream& operator<<(std::ostream& ostr, Self& self)
   {
//      boost::fusion::for_each(self.axes, printme(ostr));
   }
};



// =================================
// algorithms, helper functions
//
template<typename Container>
void set_dimension(Container& container, long index, long dimension)
{
   BOOST_ASSERT(index < container.size());
   container[index] = dimension;
}




template<typename Container>
int test_container(Container& container)
{
   using namespace boost::fusion;
   using namespace topology_tag_container;

   // <0> :: models X0 space
   //
   std::cout << "test cell cardinality cell for dim 0: " << 
      (at_c<0>(container.repository)).size()  
             << std::endl;

   typedef typename Container::value_type Numeric;


   gsse::algorithm::init2(container.repository, gsse::algorithm::constant_value<Numeric>(0));

   dump_type<typename Container::Topology_repository>();

   return 0;
}


template<typename Container>
void test_tensorproduct_calculation(Container& c_index)
{
   typedef typename Container::value_type Numeric;

//   gsse::algorithm::init(c_index, gsse::algorithm::increment<Numeric>(1) );
   gsse::algorithm::init(c_index, gsse::algorithm::constant_value<Numeric>(1) );

   std::cout << "c_index element: " << std::endl;
   gsse::algorithm::print(c_index);
   std::cout << std::endl;


   // ############## 
   // preparation 
   //

   Container container_dim;      // topology::implicit dimensions
   gsse::algorithm::init(container_dim,      gsse::algorithm::increment<Numeric>(3) );

   Container container_dim_acc;  // topology::implicit dimensions accumulative
   container_dim_acc = gsse::topology_implicit::dimension_accumulative(container_dim);

   std::cout << "container dim acc: " << std::endl;
   gsse::algorithm::print(container_dim_acc);
   std::cout << "container dim: " << std::endl;
   gsse::algorithm::print(container_dim);
   std::cout << std::endl;


   // #############
   // actual algorithms
   //
   
   // calculate an index from a tensor element
   //
   Numeric value = gsse::topology_implicit::tensorproduct(c_index, container_dim_acc);
   std::cout << " value: " << value << std::endl;

   // calculate tensor element from index with corresponding dimensions
   //
   Container c_index2 = gsse::topology_implicit::tensorproduct_inverse( value, container_dim , container_dim_acc);
   std::cout << "container index: " << std::endl;
   gsse::algorithm::print(c_index2);
   



}

int main()
{
   typedef long Numeric;
   boost::array<Numeric, 5> container;


   test_tensorproduct_calculation(container);

   return 0;


    // V0.0.1 :: base class for topology container
    //
    {
// [RH][TODO] .. enable this later
//
//        typedef topology_container<3, topology_tag_container::cell_ncube,   long>  Ts;
//        typedef topology_container<3, topology_tag_container::cell_simplex, long>  Tu;

//        Ts ts;
//        Tu tu;

//        test_container(ts);
//        test_container(tu);
    }



    // V0.0.2:: derived classes for topology container
    //
    {
       typedef topology_container_implicit<3, topology_tag_container::cell_ncube,   long>  Tc;

       Tc tc;
//       test_container(tc);

       set_dimension(tc.axes, 0, 2);
       set_dimension(tc.axes, 1, 3);
       set_dimension(tc.axes, 2, 4);
       std::cout << tc << std::endl;

    }
    {
       typedef topology_container_implicit<3, topology_tag_container::cell_simplex, long>  Tc;

       Tc tc;
//     test_container(tc);

    }
    return 0;
}
