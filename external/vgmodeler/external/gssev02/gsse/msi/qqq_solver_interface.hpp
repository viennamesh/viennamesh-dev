/* ============================================================================
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2004-2008 Philipp Schwaha                      philipp@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_QQQ_INTERFACE2_HH
#define GSSE_QQQ_INTERFACE2_HH


// *** QQQ includes
//
#include "external/iue_qqq/src/numerics_all.hh"
#include "external/iue_qqq/src/mattools.hh"


// *** boost includes
//
#include <boost/tuple/tuple.hpp>

// *** system includes
//
#include <vector>
#include <fstream>
#include <iostream>

namespace gsse
{

class qqq_insert;
class qqq_rhs;
class qqq_additive_insert;
class qqq_additive_rhs;
class qqq_solution;

class qqq_solver_interface
{
   qqqSolverParameters parms;
   qqqError            error;

   typedef double numeric_t;
   qqqEQS*              eqs;

   

   // this container is used due to the compressed matrix
   //   storage formats  .. memory overhead.. !!!
   //
   typedef boost::tuple<long, long, numeric_t> matrix_as_type;
   std::vector<matrix_as_type>   matrix_as_container;

   bool               initialized;
   long     	      dimension;
public:

   // default constructor
   //   no initialization is done..
   //
   qqq_solver_interface()
      {
         initialized = false;
      }

   // special initializiation constructur with number of elements
   // [JW] activated preInitFlags(parms) as a standard call in here ...
   qqq_solver_interface(long numele)
   {
      dimension            = numele;  
      parms.dimension      = numele;
      initialize  (parms);

   }

   ~qqq_solver_interface()
   {
      if (!initialized) return;

      (*eqs).free();
      delete eqs;
   }



   // ----------------------------------
   // @brief: default initialization for simple solving procedure
   //
   void initialize(long numele)
   {
      dimension            = numele;  
      parms.dimension      = numele;
      initialize  (parms);
   }

   void initialize(qqqSolverParameters& parms)
   {
      initialized = true;

      // assembly and basic solver configuration    .
      //
      parms.useE     = true;  // use pre-elimination
      parms.useTb    = true;  // Transformation matrix
      parms.useTv    = true;  // Variable Transformation
      parms.useSc    = true;  // Column scaling
      parms.useSr    = true;  // Row scaling
      parms.useP     = true;  // Priority
      parms.nrhs         = 1;           // number of rhs vectors
      parms.scale    = false;
      parms.sort     = false;
      parms.lByteMax = 1000*1024*1024;  // 1GB max memory alloc

      eqs = new qqqEQS(parms);

      if (!(*eqs).allocate(parms))       // throw XXX
         std::cout << "not allocated.. " << std::endl;

      if (!(*eqs).allocateNewton(parms))               // throw XXX          
      {                     
         std::cout << "Could not allocate Newton structures\n";     
      }
   }
   

   void insert_value(int  row, int column, double value)
   {
      (*eqs).As(row, column)=value;
    
      matrix_as_container.push_back(  boost::make_tuple(row, column, value));   // just for additional output
   }

   void add_value(int  row, int column, double value)
   {
      (*eqs).As(row, column)+=value;
   }

   void replace_value(int  row, int column, double value)
   {
      (*eqs).As(row, column)=value;
   }  

   void init_rhs(int row, double value)
   {
      (*eqs).bs(row,0)=value;
    }

   void add_rhs(int row, double value)
   {
      (*eqs).bs(row,0)+=value;
   }
   void init_rhs(std::vector<double>  rhs)
   {
      for (unsigned int i = 0; i < rhs.size(); ++i)
      {
         (*eqs).bs(i,0) = rhs[i];
      }
   }

   long size() 
   {
      return dimension;
   }
   void init_rhs(double d, int size)
      {
         for (unsigned int i = 0; size; ++i)
         {
            (*eqs).bs(i,0) = d;
         }
      }
   
   

   double get_x_value(int row)
   {
//      return (*eqs).bs(row,0);
      return (*eqs).x(row,0);
   }
	
  
  bool get_initialize()
  {
    return initialized;
    
  }
  
   double get_rhs_norm()
   {
      double normz = -1;
      std::cout << "not yet implemented.. : " << std::endl;
      return normz;
   }
   
   long get_dimension()
      {
         return dimension;
      }   
   
   void prepare_solver()
   {      
   }

   void switch_to_no_output_mode()
   {      
      // [JW] dummy function
      // declared here as its declared at the tril interface too ...
      // --> consistancy reasons ..
   }

   void set_options_pack3()       // iterative  .. all on.. 
   {
      parms.direct  = false;
      parms.iterative = !parms.direct;

      parms.scaCalcVec   = true;
      parms.sort         = true;
      parms.scale        = true;
      parms.precond      = true;
      parms.solAccuracy  = 1e-8;
      parms.removeUnused = true;
      parms.compare      = false ;    // solve twice and compare
      parms.lItMax       = 1000;
      parms.lNumbItMax   = parms.lByteMax / sizeof(double);

   }

   ///////////////////////////////////////////
   // [JW] ALTERNATIVE OPTION PACK 2 - TEST 1  
   // ..this one works much better..
   void set_options_pack1()      // iterative .. more restriced
   {

      parms.lByteMax = 2000*1024*1024;  // 2GB max memory alloc
     // parms.scaCalcVec   = true;
      parms.sort         = true;
      parms.scale        = true;
      parms.precond      = true;
      parms.solAccuracy  = 1e-8;
      //parms.removeUnused = true;
   //   parms.compare      = true;    // solve twice and compare
      parms.lItMax       = 10000;
     // parms.lNumbItMax   = parms.lByteMax / sizeof(double);
      parms.gmresM = 200;


   //parms.linearSystem = "rhop2_linear";
   //parms.innerSystem  = "rhop2_inner";
   //parms.writeAuxiliary = true;

}


/*
   ///////////////////////////////////////////
   // [JW] ALTERNATIVE OPTION PACK 2 - TEST 2  
   void set_options_pack2()      // iterative .. more restriced
   {
      parms.direct  = false;
      parms.iterative = !parms.direct;
      parms.iterativeType = qqqIT_GMRES;

      parms.scaCalcVec   = true;
      parms.sort         = true;
      parms.scale        = true;
      parms.precond      = false;
      parms.solAccuracy  = 1e-12;
      parms.removeUnused = true;
      parms.compare      = false ;    // solve twice and compare
      parms.lItMax       = 1000;
      parms.lNumbItMax   = parms.lByteMax / sizeof(double);
   }
*/
   void set_options_pack6()    // direct solver.. 
   {
      parms.direct       = true;        
      parms.iterative    = !parms.direct;
      parms.lNumbDiMax   = parms.lByteMax / sizeof(double);
      parms.scaCalcVec   = true;
      parms.scale        = true;
      parms.solAccuracy  = 1e-5;
      parms.lItMax       = 1000;
      parms.removeUnused = true;
      parms.lNumbDiMax   = parms.lByteMax / sizeof(double);

   }


   // [JW] added this dummy as i need a 7th op @ tril
   //
   void set_options_pack7()    
   {
      std::cout << "qqq_solver_interface:: NOT yet implemented ..." << std::endl;

   }

   void set_options_pack2()    // bicgstab
   {
      parms.direct  = false;
      parms.iterative = !parms.direct;
      parms.iterativeType = qqqIT_BICGSTAB;

      parms.precond      = true;
      parms.scaCalcVec   = true;
      parms.scale        = true;
      parms.solAccuracy  = 1e-5;
      parms.lItMax       = 1000;
      parms.removeUnused = true;
      parms.lNumbDiMax   = parms.lByteMax / sizeof(double);
   }


   void set_options_pack4()  // cg
  {
      parms.direct  = false;
      parms.iterative = !parms.direct;

      parms.lByteMax = 2000*1024*1024;  // 2GB max memory alloc
      parms.scale        = true;
      parms.sort         = true;

//      parms.precond      = true;
      parms.solAccuracy  = 1e-5;
      parms.lItMax       = 1000;
      parms.gmresM = 200;
  }

   void set_options_pack5()  // gmres
  {
      parms.direct  = false;
      parms.iterative = !parms.direct;
      parms.iterativeType = qqqIT_GMRES;

      parms.lByteMax = 2000*1024*1024;  // 2GB max memory alloc
      parms.scale        = true;
      parms.sort         = true;

      parms.precond      = true;
      parms.solAccuracy  = 1e-5;
      parms.lItMax       = 1000;
      parms.gmresM = 200;

  }

   void set_options_pack_fem()
   {

   }

   void switch_to_full_output_mode()
   {
      parms.linearSystem   = "linear"; // use "-" for stdout
      parms.innerSystem    = "inner";
      parms.writeAuxiliary = true;
   }

   // [JW] added activation of the matrix output facilities
   //
   void track_matrix()
   {
      parms.trackMatrix = true;
   }

    bool solve(long MaxIters, double MaxAccur)
{
	bool status_solved = false;

	parms.solAccuracy  = MaxAccur;
	parms.lItMax       = MaxIters;

/*	std::cout << "### solve with pack 1: " << std::endl;
	status_solved = (*eqs).solve(parms);
	std::cout << "   status: " << status_solved << std::endl;

	std::cout << "### solve with pack 2: " << std::endl;
   	set_options_pack3(); 
	status_solved = (*eqs).solve(parms);
	std::cout << "   status: " << status_solved << std::endl;
*/

	status_solved = solve();

	return status_solved;
}

   bool solve()
   {
      if (!(*eqs).solve(parms))
      {

#ifdef SOLVEROUTPUT
         std::cout << "\033[1;31m::  GSSE::solver interface:: NOT solved !!!::" << std::endl;
         std::cout << "Parameter error string = \"" << parms.getErrorString() << "\" \033[0m"<< std::endl;;
#endif
	return false;
      }
#ifdef DEBUG
      std::cout << "GSSE::solver interface (qqq):: solved !!" << std::endl;
#endif
      
      return true;
   }


  // external multiplication of system matrix
  //
  bool multiply(const std::vector<double>& input, std::vector<double>& output ) const
  {
     numeric_t *tinput;
     numeric_t *toutput;

     tinput  = new numeric_t[dimension];
     toutput = new numeric_t[dimension];

     for (int i = 0; i < dimension; ++i)
     {
 	tinput[i] =  input[i];
     }
//     qqqPrd(toutput,A,tinput);   //calculate 

     for (int i = 0; i < dimension; ++i)
     {
 	output[i] = toutput[i];
     }

    return true;
  }


   void dump()    // not yet implemented
   {
      std::cout << "QQQ matrix interface: not yet implemented.. !! " << std::endl;
//       std::ofstream fileout("matrix_dump_qqq.rh");
//       fileout.close();
   }


   void dump_html_table(long number_equ = 1) 
      {

      }
   void write_mtx(const std::string& filename)
      {
         std::string filename_fullas=filename+".mtx";
         std::string filename_rhs=filename+"_rhs.mtx";

         std::ofstream fileout_as(filename_fullas.c_str());
         std::ofstream fileout_bs(filename_rhs.c_str());
         
         fileout_as << "RH:qqq_outputfile matrix simple real general" << std::endl;
         fileout_as << dimension << "  " << dimension << "  " << matrix_as_container.size() << std::endl;
         for (size_t i = 0 ; i < matrix_as_container.size(); ++i)
         {
            fileout_as << boost::get<0>(matrix_as_container[i])+1 << " "     // mtx starts with "1"
                       << boost::get<1>(matrix_as_container[i])+1 << " " 
                       << boost::get<2>(matrix_as_container[i]) << " " << std::endl;
         }
         fileout_as.close();

         fileout_bs << "RH:qqq_outputfile matrix simple real general" << std::endl;
         fileout_bs << dimension << "  1" << std::endl;

         for (int i = 0; i < dimension; ++i)
         {
            fileout_bs << (*eqs).bs(i,0) << std::endl;
         }
         
         fileout_bs.close();
      }


   void write_dx(const std::string& filename)
      {
         std::string filename_gen=filename+".general";
         std::string filename_mat=filename+".mat";

         std::ofstream fileout_gen(filename_gen.c_str());
         std::ofstream fileout_mat(filename_mat.c_str());
         
         fileout_gen << "file = " << filename_mat      << std::endl;
         fileout_gen << "points = " << matrix_as_container.size()       << std::endl;
         fileout_gen << "format = ascii"               << std::endl;
         fileout_gen << "interleaving = field "        << std::endl;
         fileout_gen << "majority = row"               << std::endl;
         fileout_gen << "field = locations, field0"    << std::endl;
         fileout_gen << "structure = 2-vector, scalar" << std::endl;
         fileout_gen << "type = float, float"          << std::endl;
         fileout_gen << "end "                         << std::endl;
         fileout_gen.close();

         for (size_t i = 0 ; i < matrix_as_container.size(); ++i)
         {
            fileout_mat << boost::get<1>(matrix_as_container[i])+1 << "\t -"     // mtx starts with "1"
                       << boost::get<0>(matrix_as_container[i])+1 << "\t " 
                       << boost::get<2>(matrix_as_container[i]) << " " << std::endl;
         }
         fileout_mat.close();
      }




   // check routine, if a direct solver can be used on normal computer systems
   //
   bool is_calculable(long dimension)
      {
         if (parms.direct && dimension < 3000)
            return true;
         else if (!parms.direct)
            return true;

         return false;
      }


   // -----------------------------------
   // special metaprogramming data type definitions
   //
   typedef  qqq_insert          entry_accessor;
   typedef  qqq_rhs             rhs_accessor;
   typedef  qqq_additive_insert additive_entry_accessor;
   typedef  qqq_additive_rhs    additive_rhs_accessor;
   typedef  qqq_solution        solution_accessor;
};




// metaprogramming facilities
//
class qqq_insert
{
   qqq_solver_interface * msi;

public:

  qqq_insert() {}
  qqq_insert(qqq_solver_interface & msi) : msi(&msi) {}

  void operator()(long index1, long index2, double entry)
  {
    (*msi).insert_value(index1, index2, entry);
  }
};

class qqq_rhs
{
   qqq_solver_interface * msi;

public:

   qqq_rhs()  {}
   qqq_rhs(qqq_solver_interface & msi) : msi(&msi) {}

   void operator()(long index1, double entry)
      {
         (*msi).init_rhs(index1, entry);
      }
};

class qqq_additive_insert
{
   qqq_solver_interface * msi;
   
public:
   
   qqq_additive_insert() {}
   
   qqq_additive_insert(qqq_solver_interface & msi) : msi(&msi) 
   {
   }
   
   void operator()(long index1, long index2, double entry)
   {
      (*msi).add_value(index1, index2, entry);
   }
   
};

class qqq_additive_rhs
{
   qqq_solver_interface * msi;
   
public:

   qqq_additive_rhs()  {}

   qqq_additive_rhs(qqq_solver_interface & msi) : msi(&msi) {}
   
   void operator()(long index1, double entry)
      {
         (*msi).add_rhs(index1, entry);
      }
};




class qqq_solution
{
  qqq_solver_interface * msi;

public:

  qqq_solution()  {}

  qqq_solution(qqq_solver_interface & msi) : msi(&msi) {}

   double operator()(long index1)
      {
         return (*msi).get_x_value(index1);
      }
};

//
//
// *********************************************************

}   //end of namespace gsse

#endif
