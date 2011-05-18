/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

// this file is a template for all different types of solver interfaces
//

#ifndef GSSE_TEMPLATE_INTERFACE_HH
#define GSSE_TEMPLATE_INTERFACE_HH

// ****************** specific solver includes ***************************


// ****************** system includes ***************************
#include <vector>
#include <fstream>



namespace gsse
{


// forward declarations
//
class trilinos_insert;

// implementation
/
class template_solver_interface
{
  bool initialized;
  long number_of_elements;




   void initialize(long numele)
   {
      initialized = true;
      number_of_elements = numele;
   }
 
public:
   template_solver_interface()
      {
         initialized = false;
      }

   template_solver_interface(long numele)
   {
      initialize(numele);
   }

   ~template_solver_interface()
   {
      if (!initialized) return;
   }

   void insert_value(int  row, int column, double value)
   {
   }

   void add_value(int  row, int column, double value)
   {
   }

   void replace_value(int  row, int column, double value)
   {
   }  

   void init_rhs(int row, double value)
   {
   }

   void add_rhs(int row, double value)
   {
   }

   double get_x_value(int row)
   {
   }

   void init_rhs(std::vector<double>  rhs)
   {
   }

   void init_rhs(double d, int size)
   {
   }

   double get_rhs_norm()
   {
   }


   void prepare_solver()
   {      
   }

   void set_options_pack1()
   {

   }
   void set_options_pack2()
   {

   }
   void set_options_pack3()
   {

   }
   void set_options_pack4()
   {

   }
   void set_options_pack_fem()
   {
   }

   void switch_to_full_output_mode()
   {
   }

   void dump()
   {
     std::ofstream fileout("matrix_dump.rh");
     fileout.close();
   }

   bool solve()
   {
     return true;
   }

   typedef  template_insert          entry_accessor;
   typedef  template_rhs             rhs_accessor;
   typedef  template_additive_insert additive_entry_accessor;
   typedef  template_additive_rhs    additive_rhs_accessor;
   typedef  template_solution        solution_accessor;
};


class template_insert
{
   template_solver_interface * msi;

public:

  template_insert() {}

  template_insert(template_solver_interface & msi) : msi(&msi) {}

  void operator()(long index1, long index2, double entry)
  {
  }

};

class template_rhs
{
   template_solver_interface * msi;

public:
  template_rhs()  {}
  template_rhs(template_solver_interface & msi) : msi(&msi) {}

  void operator()(long index1, double entry)
  {
  }
};

class template_additive_insert
{
  template_solver_interface * msi;

public:
  template_additive_insert() {}
  template_additive_insert(template_solver_interface & msi) : msi(&msi) {}

  void operator()(long index1, long index2, double entry)
  {
  }
};

class template_additive_rhs
{
  template_solver_interface * msi;
public:
  template_additive_rhs()  {}
  template_additive_rhs(template_solver_interface & msi) : msi(&msi) {}

  void operator()(long index1, double entry)
  {
  }
};




class template_solution
{
  template_solver_interface * msi;
public:
  template_solution()  {}
  template_solution(template_solver_interface & msi) : msi(&msi) {}

  double operator()(long index1)
  {
  }
};


//
//
// *********************************************************


}   //end of namespace gsse



#endif
