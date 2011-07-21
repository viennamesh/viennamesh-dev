/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */


  /******************* ID Handling *******************************/

#ifndef VIENNADATA_IDHANDLER_GUARD
#define VIENNADATA_IDHANDLER_GUARD

#include <map>
#include <vector>
#include <list>
#include <set>
#include <iostream>
#include <algorithm>
#include <memory>

namespace viennadata
{

  struct no_id
  {
    no_id() {};
    //NoID(long id) {};

    //for compatibility:
    void vdata_id(long id_) { };
    const no_id * vdata_id() const { return this; };

  };

  class has_id
  {
    public:
      has_id() : id_(-1) {};
      //ProvideID(long id): id_(id) {}

      long vdata_id() const { return id_; };
      void vdata_id(long id) { id_ = id; };

    protected:
      long id_;
  };
  
} //namespace

#endif
