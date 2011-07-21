/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                             rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <string>

#include "examples/benchmarks/common.hpp"
#include "viennadata/api.hpp"



template <typename T>
void run_oop(size_t num, size_t repeat)
{
  std::vector<T> objects(num);
  std::vector<double> refvector(num);

  for (size_t i=0; i<num; ++i)
  {
    refvector[i] = ::sqrt(static_cast<double>(i));
    objects[i] = T(::sqrt(static_cast<double>(i)), i);
  }
  
  double result;
  double elapsed;
  Timer t;
  
  //
  //
  
  std::cout << " - Summing vector directly..." << std::endl;
  result = 0.0;
  t.start();
  for (size_t r=0; r<repeat; ++r)
    for (size_t i=0; i<num; ++i)
      result += refvector[i]; 
  elapsed = t.get();
  std::cout << "   Time elapsed: " << elapsed << std::endl;
  std::cout << "   Time elapsed: " << elapsed <<  "   (result: " << result << ")" << std::endl;
  
  //
  //
  
  std::cout << " - Testing direct value access..." << std::endl;
  result = 0.0;
  t.start();
  for (size_t r=0; r<repeat; ++r)
    for (size_t i=0; i<num; ++i)
      result += objects[i].value(); 
  elapsed = t.get();
  std::cout << "   Time elapsed: " << elapsed <<  "   (result: " << result << ")" << std::endl;

}



int main(int argc, char *argv[])
{
  std::cout << "--- Test 1: SlimClass, 1 000 objects, 1000 repetitions ---" << std::endl;
  run_oop<SlimClass>(1000, 1000);
  std::cout << "--- Test 2: SlimClass, 1 000 000 objects ---" << std::endl;
  run_oop<SlimClass>(1000000, 1);
  std::cout << "--- Test 3: FatClass<10>, 1 000 objects, 1000 repetitions ---" << std::endl;
  run_oop<FatClass<10> >(1000, 1000);
  std::cout << "--- Test 3: FatClass<10>, 1 000 000 objects ---" << std::endl;
  run_oop<FatClass<10> >(1000000, 1);
  std::cout << "--- Test 5: FatClass<100>, 1 000 objects, 1000 repetitions ---" << std::endl;
  run_oop<FatClass<100> >(1000, 1000);
  std::cout << "--- Test 5: FatClass<100>, 1 000 000 objects ---" << std::endl;
  run_oop<FatClass<100> >(1000000, 1);
  std::cout << "--- Test 7: FatClass<1000>, 1 000 objects, 1000 repetitions ---" << std::endl;
  run_oop<FatClass<1000> >(1000, 1000);
  std::cout << "--- Test 7: FatClass<1000>, 1 000 000 objects ---" << std::endl;
  run_oop<FatClass<1000> >(1000000, 1);
  
  return EXIT_SUCCESS;
}

