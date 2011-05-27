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

#include "benchmark-utils.hpp"
#include "viennadata/api.hpp"

class SlimClass
{
  public:
    SlimClass(double v = 1.0, size_t i = 0) : value_(v), id_(i) {}
    
    double value() const { return value_; }
    size_t id() const { return id_; }
  private:
    double value_;
    size_t id_;
};


template <size_t num_bytes>
class FatClass
{
  public:
    FatClass(double v = 1.0, size_t i = 0) : value_(v), id_(i) {}
    
    double value() const { return value_; }
    size_t id() const { return id_; }
  private:
    double value_;
    char payload[num_bytes];
    size_t id_;
};

struct data_key {};

template <typename T>
void run(size_t num, size_t repeat)
{
  std::vector<T> objects(num);
  std::string key_string = "test";
  std::vector<double> refvector(num);

  //reserve can speed up first dense access (optional)
  viennadata::reserve<std::string, double>(num)(objects[0]);
  viennadata::reserve<long, double>(num)(objects[0]);
  viennadata::reserve<data_key, double>(num)(objects[0]);
  
  for (size_t i=0; i<num; ++i)
  {
    refvector[i] = ::sqrt(static_cast<double>(i));
    objects[i] = T(::sqrt(static_cast<double>(i)), i);
   
    viennadata::access<std::string, double>(key_string)(objects[i]) = objects[i].value();
    viennadata::access<std::string, double>("bla")(objects[i]) = objects[i].value();
    viennadata::access<long, double>(42)(objects[i]) = objects[i].value();
    viennadata::access<long, double>(7)(objects[i]) = objects[i].value();
    viennadata::access<data_key, double>()(objects[i]) = objects[i].value();    
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

  //
  //
  
  std::cout << " - Testing ViennaData access (slow)..." << std::endl;
  result = 0.0;
  t.start();
  for (size_t r=0; r<repeat; ++r)
    for (size_t i=0; i<num; ++i)
      result += viennadata::access<std::string, double>(key_string)(objects[i]); 
  elapsed = t.get();
  std::cout << "   Time elapsed: " << elapsed <<  "   (result: " << result << ")" << std::endl;

  //
  //
  
  std::cout << " - Testing ViennaData access (medium)..." << std::endl;
  result = 0.0;
  t.start();
  for (size_t r=0; r<repeat; ++r)
    for (size_t i=0; i<num; ++i)
      result += viennadata::access<long, double>(42)(objects[i]); 
  elapsed = t.get();
  std::cout << "   Time elapsed: " << elapsed <<  "   (result: " << result << ")" << std::endl;

  //
  //
  
  std::cout << " - Testing ViennaData value access (fast)..." << std::endl;
  result = 0.0;
  t.start();
  for (size_t r=0; r<repeat; ++r)
    for (size_t i=0; i<num; ++i)
      result += viennadata::access<data_key, double>()(objects[i]); 
  elapsed = t.get();
  std::cout << "   Time elapsed: " << elapsed <<  "   (result: " << result << ")" << std::endl;
  
}
