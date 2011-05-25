/* ============================================================================
   Copyright (c) 2008-2009 Philipp Schwaha                      philipp@gsse.at
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_MATH_PSET_HPP
#define GSSE_MATH_PSET_HPP

// *** system includes
#include <algorithm>

// *** GSSE includes
#include "gsse/topology/cells.hpp"

// ############################################################
//
namespace gsse { namespace math{

namespace simplex{
// an element of the boundary of a n-simplex can be obtained by
// taking only n-1 elements of its complete set
// by rotating the set and then extracting the subsets, all 
// boundary elements are obtained.
// 
// this is implemented in the following two structs
//
// operations such as the following should be performed on simple 
// data types (e.g. long) and not more complex data. it is better 
// to set up a permutation map and the use this to access the original 
// data in place, if this is at all possible
//

template<typename ContainerT>
struct boundary_of_generator
{
   ContainerT dataset;
   boundary_of_generator(const ContainerT& input) : dataset(input) {};

   // several calls to this are necessary to get the complete boundary
   // 
   ContainerT operator()()
   {
      // rotate. the result is stored so that subsequent calls generate the 
      // next element in the boundary
      // 
      std::rotate(dataset.begin(), dataset.begin()+1, dataset.end());

      // take only the first part
      // 
      ContainerT return_data(dataset.begin(), dataset.end() - 1);

      // helps to keep to recognize and remove duplicates
      //
      std::sort(return_data.begin(), return_data.end());
      return return_data;
   }
};


template<typename OutputContainerT>
struct boundary_of
{
   // we bind to an output container
   // 
   OutputContainerT& output;
   boundary_of(OutputContainerT& output) : output(output) {};

   template<typename ItemT>
   OutputContainerT operator()(const ItemT& item)
   {
      OutputContainerT temp_container(item.size());
      boundary_of_generator<ItemT> bound(item);
      
      // generate the whole boundary
      //
      std::generate(temp_container.begin(), temp_container.end(), bound);

      // insert only new elements
      //
      for (typename OutputContainerT::iterator it(temp_container.begin());
           it != temp_container.end(); ++it)
      {
         typename OutputContainerT::iterator look = 
            std::find(output.begin(), output.end(), *it);
         if (look == output.end())
            output.push_back(*it);
      }
      return output;
   }
};

// use the previous structs to generate a poset for a simplex
// the dimension is extracted from the number of elements in 
// the container's very first element, which has to be passed in
//
template<typename ContainerT>
void generate_simplex_poset_permutation(ContainerT& container)
{
   typedef typename ContainerT::value_type inner_type;

   // kind of a serious hack! ...
   //
   size_t max(container[0][0].size()-1);
      
   // fill all dimensions
   //
   for (size_t j(0); j < max; j++)
   {
      inner_type item(container[j]);
      inner_type temp;
      boundary_of<inner_type> bound(temp);
      // for_
      for (typename inner_type::const_iterator it(item.begin()); it!=item.end(); it++)
      {
         bound(*it);
      }
      container.push_back(temp);
   }
}

} // namespace simplex

//
// new addition for (hyper)-cube posets [PS]
//
namespace cube {

template<typename ContainerT>
struct boundary_of_generator_cube
{
   // typedef typename ContainerT::value_type intermediate_type;
   // typedef ContainerT intermediate_type;

   ContainerT dataset;
   size_t partition;
   size_t counter;
   boundary_of_generator_cube(const ContainerT& input) : dataset(input), partition(1), counter(0) {};

   // several calls to this are necessary to get the complete boundary
   // 
   ContainerT operator()()
   {
      // this is a bit ugly
      // we need to explicitly keep track of the states (partition & counter)
      // two elements should be generated for each value of partition (always "parallel" n-1 objects)
      // hence the counter is used to control advancement of partition
      // 
      if (!(counter % 2))
      {
         partition *= 2;
      }

      // we get a block length depending on the value of partition
      // 
      size_t block_length(dataset.size() / partition);
      
      ContainerT return_data;

      // alwas extract a range of elements (length is block_length)
      // from the data to form a n-1 boundary object
      // 
      for (typename ContainerT::iterator dataset_iterator(dataset.begin()); 
           dataset_iterator != dataset.end(); 
           (dataset_iterator += 2 * block_length ))
      {
         return_data.insert(return_data.end(), 
                            dataset_iterator + (counter % 2) * block_length, 
                            dataset_iterator + block_length + (counter % 2) * block_length);
      }

      // helps to keep to recognize and remove duplicates
      //
      std::sort(return_data.begin(), return_data.end());

//      for (typename ContainerT::iterator it(return_data.begin()); it != return_data.end(); it++)
//      {
//         std::cout << *it ;
//      }
//      std::cout << std::endl;

      counter++;
      return return_data;
   }
};

template<typename OutputContainerT>
struct boundary_of_cube
{
   // we bind to an output container
   // 
   OutputContainerT& output;
   boundary_of_cube(OutputContainerT& output) : output(output) {};

   template<typename ItemT>
   OutputContainerT operator()(const ItemT& item)
   {
      // we have 2n faces (n is the dimension) 
      //
      OutputContainerT temp_container((2 * cell_reverse_cardinality_calculator<gsse::cell_cube>()(item.size())));

      // std::cout << "** " << 2 * std::log(item.size()) / std::log(2) << std::endl;

      boundary_of_generator_cube<ItemT> bound(item);
      
      // generate the whole boundary
      //
      std::generate(temp_container.begin(), temp_container.end(), bound);

      // insert only new elements
      //
      for (typename OutputContainerT::iterator it(temp_container.begin());
           it != temp_container.end(); ++it)
      {
         typename OutputContainerT::iterator look = 
            std::find(output.begin(), output.end(), *it);
         if (look == output.end())
            output.push_back(*it);
      }
      return output;
   }
};


// use the previous structs to generate a poset for a cube
// the dimension is extracted from the number of elements in 
// the container's very first element, which has to be passed in
//
template<typename ContainerT>
void generate_cube_poset_permutation(ContainerT& container)
{
   typedef typename ContainerT::value_type inner_type;

   // kind of a serious hack! ...
   //
   size_t max(cell_reverse_cardinality_calculator<gsse::cell_cube>()(container[0][0].size()));

   // fill all dimensions
   //
   for (size_t j(0); j < max; j++)
   {
      inner_type item(container[j]);
      inner_type temp;
      boundary_of_cube<inner_type> bound(temp);
      // for_
      for (typename inner_type::const_iterator it(item.begin()); it!=item.end(); it++)
      {
         bound(*it);
      }
      container.push_back(temp);
   }
}

} // namespace cube


// ####################################



template<typename CellType>
struct generate_poset_permutation
{
   template<typename ContainerT>
   void operator()(ContainerT& container) { std::cout << "not implemented ..." << std::endl; }
};

template<>
struct generate_poset_permutation<gsse::cell_simplex>
{
   template<typename ContainerT>
   void operator()(ContainerT& poset_container) const
   {
      // std::cout << "generating simplex" << std::endl;
      gsse::math::simplex::generate_simplex_poset_permutation(poset_container);
   }
};

template<>
struct generate_poset_permutation<gsse::cell_cube>
{
   template<typename ContainerT>
   void operator()(ContainerT& poset_container) const
   {
      // std::cout << "generating cube" << std::endl;
      gsse::math::cube::generate_cube_poset_permutation(poset_container);
   }
};


template<long DIM_cc, typename CellType>
struct poset_generator
{
   typedef std::vector<long>       CellRT;
   typedef std::vector<CellRT>     CellRT_inner_type;
   typedef std::vector<CellRT_inner_type> poset_container_type;

   typedef CellType CellT;
   static const long DIM_cell = DIM_cc;
// ------

   poset_generator()
   {
      CellRT cl_init;

      // std::cout << " DIM: " << DIM_cc <<  " cardinality: " << gsse::cell_cardinality_calculator<CellType>::template apply<DIM_cc>::cardinality << std::endl;
      for (long di = 0; di < gsse::cell_cardinality_calculator<CellType>::template apply<DIM_cc>::value; ++di)   // get cardinality according to type
      {
         cl_init.push_back(di);
      }
      poset_container.push_back(CellRT_inner_type(1,cl_init));
      
      gsse::math::generate_poset_permutation<CellType>()(poset_container);
   }

   CellRT_inner_type const& operator[](long bnd_index) const
   {
      return poset_container[bnd_index];
   }

//#####
private:
   poset_container_type poset_container;
};

// template<long DIM_cc, typename CellType>
// struct poset_generator
// {
//    typedef std::vector<long>       CellRT;
//    typedef std::vector<CellRT>     CellRT_inner_type;
//    typedef std::vector<CellRT_inner_type> poset_container_type;
// 
//    typedef CellType CellT;
//    static const long DIM_cell = DIM_cc;
// // ------
// 
//    poset_generator()
//    {
//       CellRT cl_init;
// 
//       // [RH][TODO]
//       //   extend this to a const char* or some other CT/RT data type dispatcher
//       //
//       if (gsse::cell_type_runtime<CellType>::apply::name == 'S')
//       {
//          for (long di = 0; di < DIM_cc+1; ++di)   //simplex type
//          {
//             cl_init.push_back(di);
//          }
//          poset_container.push_back(CellRT_inner_type(1,cl_init));
//          
//          gsse::math::generate_simplex_poset_permutation(poset_container);
//       }
//       else
//          if (gsse::cell_type_runtime<CellType>::apply::name == 'C')
//          {
//             // std::cout << "## coboundary operator:: just simplex is implemented ##  -- " << std::endl;
//             for (long di = 0; di < std::pow(2,DIM_cc); ++di)   
//             {
//                cl_init.push_back(di);
//             }
//             poset_container.push_back(CellRT_inner_type(1,cl_init));
//             gsse::math::generate_cube_poset_permutation(poset_container);
//          }
//          else 
//          {
//             std::cout << "## coboundary operator:: only simplex & cube are implemented ## " << std::endl;
//          }
//    }
// 
//    CellRT_inner_type const& operator[](long bnd_index) const
//    {
//       return poset_container[bnd_index];
//    }
// 
// //#####
// private:
//    poset_container_type poset_container;
// };

// template<typename CellType>
// struct poset_generator_rt
// {
//    typedef std::vector<long>       CellRT;
//    typedef std::vector<CellRT>     CellRT_inner_type;
//    typedef std::vector<CellRT_inner_type> poset_container_type;

//    typedef CellType CellT;
// //   static const long DIM_cell = DIM_cc;
// // ------

//    poset_generator_rt()
//    {}
//    long get_dim() const { return dimension;}

//    void set_dim(long dim)
//    {
//       dimension = dim;


//       CellRT cl_init;

//       // [RH][TODO]
//       //   extend this to a const char* or some other CT/RT data type dispatcher
//       //
// //      dump_type<CellType>();
//       if (gsse::cell_type_runtime<CellType>::apply::name == 'S')
//       {
//          for (long di = 0; di < dim+1; ++di)   //simplex type
//          {
//             cl_init.push_back(di);
//          }
//          poset_container.push_back(CellRT_inner_type(1,cl_init));
      
//          gsse::math::simplex::generate_simplex_poset_permutation(poset_container);
//       }
//       else
//          if (gsse::cell_type_runtime<CellType>::apply::name == 'C')
//       {
//          for (long di = 0; di < std::pow(2,dim); ++di)   //cube type
//          {
//             cl_init.push_back(di);
//          }
//          poset_container.push_back(CellRT_inner_type(1,cl_init));
//          gsse::math::cube::generate_cube_poset_permutation(poset_container);
         
//          //std::cout << "## coboundary operator:: just simplex is implemented ## " << std::endl;
//       }
//       else 
//       {
//          std::cout << "## coboundary operator:: only simplex & cube is implemented ## " << std::endl;
//       }
//    }

//    CellRT_inner_type const& operator[](long bnd_index) const
//    {
//       return poset_container[bnd_index];
//    }

// //#####
// private:
//    long dimension;
//    poset_container_type poset_container;
// };


} //  namespace math
} // namespace gsse

#endif
