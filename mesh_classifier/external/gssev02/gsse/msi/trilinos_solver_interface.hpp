/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_TRILINOS_INTERFACE_HH
#define GSSE_TRILINOS_INTERFACE_HH


#include "Epetra_ConfigDefs.h"
#include "Epetra_SerialComm.h"
#include "Epetra_Map.h"
#include "Epetra_Vector.h"
#include "Epetra_CrsMatrix.h"


// *** boost includes
//
#include <boost/tuple/tuple.hpp>

// ****************** trilinos ***************************
//
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <fstream>

#ifdef EPETRA_MPI
#include "mpi.h"
#include "Epetra_MpiComm.h"
#else
#include "Epetra_SerialComm.h"
#endif

#include "AztecOO.h"
#include "AztecOO_Version.h"

#include "Epetra_Time.h"
#include "Epetra_Map.h"
#include "Epetra_Vector.h"
#include "Epetra_CrsMatrix.h"

// // ****************** trilinos ***************************

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <fstream>

// ****************** trilinos ***************************

namespace gsse
{

// ******** trilinos-gsse matrix/solver matrix_solver_interface

// *********************************************************
// our trilinos matrix solver interace for linear problems
//
//

class trilinos_insert;
class trilinos_rhs;
class trilinos_additive_insert;
class trilinos_additive_rhs;
class trilinos_solution;

class trilinos_solver_interface
{
   Epetra_SerialComm* comm;
   Epetra_Map*        Map;
   int                NumMyElements;
   Epetra_CrsMatrix*  A;
   Epetra_Vector*     x;
   Epetra_Vector*     b;

   AztecOO*           solver;
   bool               initialized;
   long  	      dimension;

   // this container is used due to the compressed matrix
   //   storage formats  .. memory overhead.. !!!
   //
   typedef double numeric_t;

   typedef boost::tuple<long, long, numeric_t> matrix_as_type;
   std::vector<matrix_as_type>   matrix_as_container;


public:

   trilinos_solver_interface()
      {
         initialized = false;
      }

   bool is_calculable(long dimension)
      {
         return true;
      }

   void initialize(long numele)
   {
      initialized = true;
      NumMyElements = numele;
		dimension = numele;      

      comm = new Epetra_SerialComm();
      Map  = new Epetra_Map(NumMyElements, 0, *comm);
      A    = new Epetra_CrsMatrix(Copy,*Map,3);
      x    = new Epetra_Vector(*Map);
      b    = new Epetra_Vector(*Map);
      
      //(*A).TransformToLocal();
      //(*A).Print(std::cout);

      for (int k=0; k<numele; ++k) (*x)[k] = 0.0;

      
      solver = 0;
   }

   trilinos_solver_interface(long numele)
   {
      initialize(numele);
   }

   ~trilinos_solver_interface()
   {
      if (!initialized) return;
      if (x != 0) delete  x;
      if (b != 0) delete  b;
      if (A != 0) delete  A;
      if (Map != 0) delete  Map;
      if (comm != 0) delete  comm;
      if (solver != 0) delete  solver;
   }

   void insert_value(int  row, int column, numeric_t value)
   {
      (*A).InsertGlobalValues(row, 1, &value, &column);
      matrix_as_container.push_back(  boost::make_tuple(row, column, value));
   }

   void add_value(int  row, int column, numeric_t value)
   {
      int a = (*A).SumIntoGlobalValues(row, 1, &value, &column);
      if (a > 0)
      {
         (*A).InsertGlobalValues(row, 1, &value, &column);
      }
   }

   void replace_value(int  row, int column, numeric_t value)
   {
     (*A).ReplaceGlobalValues(row, 1, &value, &column);
   }  

   void init_rhs(int row, numeric_t value)
   {
      (*b)[row] = value;
   }

   void add_rhs(int row, numeric_t value)
   {
     (*b)[row] += value;
   }

   numeric_t get_x_value(int row)
   {
      return (*x)[row];
   }
	
   void init_rhs(std::vector<numeric_t>  rhs)
   {
      for (unsigned int i = 0; i < rhs.size(); ++i)
      {
         (*b)[i] = rhs[i];
      }
   }

  void init_rhs(numeric_t d, int size)
  {
    for (unsigned int i = 0; size; ++i)
      {
	(*b)[i] = d;
      }
  }
  
  bool get_initialize()
  {
    return initialized;
    
  }
  
   numeric_t get_rhs_norm()
   {
      numeric_t normz;
      (*b).Norm1(&normz); // Compute 1-norm of z
      return normz;
   }

	long get_dimension()
	{
		return dimension;
	}   

   void prepare_solver()
   {      
      (*A).TransformToLocal();   

      // Finish up
      (*A).FillComplete();
      (*A).OptimizeStorage();
            
      // Create Linear Problem
      Epetra_LinearProblem problem(A, x, b);

      // Create AztecOO instance
      solver = new AztecOO(problem);

//     solver->SetAztecOption(AZ_output, AZ_none);
 // solver->SetErrorStream(ostream);
 }


/*  aztec/examples
 *    AZ_defaults(options, params);
 *   
 *    options[AZ_solver]   = AZ_cgs;
 *    options[AZ_scaling]  = AZ_none;
 *    options[AZ_precond]  = AZ_ls;
 *    options[AZ_conv]     = AZ_r0;
 *    options[AZ_output]   = 1;
 *    options[AZ_pre_calc] = AZ_calc;
 *    options[AZ_max_iter] = 1550;
 *    options[AZ_poly_ord] = 5;
 *    options[AZ_overlap]  = AZ_none;
 *    options[AZ_kspace]   = 60;
 *    options[AZ_aux_vec]  = AZ_resid;
 *    params[AZ_tol]       = 4.00e-9;
 *    params[AZ_drop]      = 0.0;
 *    params[AZ_ilut_fill] = 1.5;
 *    params[AZ_omega]     = 1.;
 *
 *
 *
 * */
   void set_options_pack4()
   {
      solver->SetAztecOption(AZ_solver, AZ_bicgstab);
//     solver->SetAztecOption(AZ_output, AZ_none);
   }

   void set_options_pack6()
   {
      solver->SetAztecOption(AZ_solver, AZ_gmres);
      solver->SetAztecOption(AZ_kspace, 10000);
      solver->SetAztecOption(AZ_precond,   AZ_Jacobi);
//      solver->SetAztecOption(AZ_precond, AZ_none);
   }

   //
   // [JW] i prefer this option pack .. ilut with gmres(1k krylov) 
   //   btw: 10k krylov vectors is a ram killer ... 
   //       aztecOO documentation suggests krylov vectors around max iterations .. - 1k ..
   //
   void set_options_pack7()
   {
      solver->SetAztecOption(AZ_solver, AZ_gmres);
      solver->SetAztecOption(AZ_kspace, 1000);
   }

   void set_options_pack5()
   {
      solver->SetAztecOption(AZ_solver, AZ_gmres);
      solver->SetAztecOption(AZ_kspace, 10000);
      solver->SetAztecOption(AZ_precond,   AZ_Neumann);

//       solver->SetAztecOption(AZ_solver, AZ_gmres_condnum);
//       solver->SetAztecOption(AZ_kspace, 1000);
//     solver->SetAztecOption(AZ_output, AZ_none);
   }

   void set_options_pack1()
   {
	   solver->SetAztecOption(AZ_solver,  AZ_cgs);
	   solver->SetAztecOption(AZ_scaling,   AZ_none);
	   solver->SetAztecOption(AZ_precond,   AZ_ls);
	   solver->SetAztecOption(AZ_conv,      AZ_r0);
	   //    solver->SetAztecOption(AZ_output,    1);
	   solver->SetAztecOption(AZ_pre_calc,  AZ_calc);
	   solver->SetAztecOption(AZ_max_iter,  1550);
	   solver->SetAztecOption(AZ_poly_ord,  5);
	   solver->SetAztecOption(AZ_overlap,   AZ_none);
	   solver->SetAztecOption(AZ_kspace,    60);
	   solver->SetAztecOption(AZ_aux_vec,   AZ_resid);
	   solver->SetAztecParam(AZ_tol,       4.00e-9);
	   solver->SetAztecParam(AZ_drop,       0.0);
	   solver->SetAztecParam(AZ_ilut_fill,  1.50);
	   solver->SetAztecParam(AZ_omega,      1.);

//     solver->SetAztecOption(AZ_output, AZ_none);
   }
   void set_options_pack2()
   {
	   solver->SetAztecOption(AZ_solver,  AZ_bicgstab);
	   solver->SetAztecOption(AZ_scaling,   AZ_none);
	   solver->SetAztecOption(AZ_precond,   AZ_ls);
	   solver->SetAztecOption(AZ_conv,      AZ_r0);
	   //    solver->SetAztecOption(AZ_output,    1);
	   solver->SetAztecOption(AZ_pre_calc,  AZ_calc);
	   solver->SetAztecOption(AZ_max_iter,  1550);
	   solver->SetAztecOption(AZ_poly_ord,  5);
	   solver->SetAztecOption(AZ_overlap,   AZ_none);
	   solver->SetAztecOption(AZ_kspace,    60);
	   solver->SetAztecOption(AZ_aux_vec,   AZ_resid);
	   solver->SetAztecParam(AZ_tol,       4.00e-9);
	   solver->SetAztecParam(AZ_drop,       0.0);
	   solver->SetAztecParam(AZ_ilut_fill,  1.50);
	   solver->SetAztecParam(AZ_omega,      1.);

//     solver->SetAztecOption(AZ_output, AZ_none);
   }
   void set_options_pack3()
   {
	   solver->SetAztecOption(AZ_solver,  AZ_tfqmr);
	   solver->SetAztecOption(AZ_scaling,   AZ_none);
	   solver->SetAztecOption(AZ_precond,   AZ_ls);
	   solver->SetAztecOption(AZ_conv,      AZ_r0);
	   //    solver->SetAztecOption(AZ_output,    1);
	   solver->SetAztecOption(AZ_pre_calc,  AZ_calc);
	   solver->SetAztecOption(AZ_max_iter,  1550);
	   solver->SetAztecOption(AZ_poly_ord,  5);
	   solver->SetAztecOption(AZ_overlap,   AZ_none);
	   solver->SetAztecOption(AZ_kspace,    60);
	   solver->SetAztecOption(AZ_aux_vec,   AZ_resid);
	   solver->SetAztecParam(AZ_tol,       4.00e-9);
	   solver->SetAztecParam(AZ_drop,       0.0);
	   solver->SetAztecParam(AZ_ilut_fill,  1.50);
	   solver->SetAztecParam(AZ_omega,      1.);
//     solver->SetAztecOption(AZ_output, AZ_none);
   }



void set_options_pack_fem()
   {
      solver->SetAztecOption(AZ_precond,          AZ_dom_decomp );
      solver->SetAztecOption(AZ_subdomain_solve,  AZ_lu);
      solver->SetAztecOption(AZ_solver,           AZ_cg);
   }


   //[CK] i took this funtion out of the solve-function
   void switch_to_no_output_mode()
   {
     solver->SetAztecOption(AZ_output, AZ_none);
   }
   void switch_to_full_output_mode()
   {
      solver->SetAztecOption(AZ_output, AZ_all);
   }

   void dump()
   {
     std::ofstream fileout("matrix_dump.rh");
      (*A).Print (fileout);
      for (int i=0; i < NumMyElements; ++i)
	fileout << "RHS(" << i << "): " << (*b)[i] << std::endl;
      for (int i=0; i < NumMyElements; ++i)
	fileout << "RHS(" << i << "): " << (*x)[i] << std::endl;
      fileout.close();
   }

   bool solve(long MaxIters, double MaxAccur)
   {
     int err=  solver->Iterate(MaxIters, MaxAccur);
     if (err != 0)
     {
     //	std::cout << "## entering trilinos automatic option mode.. " << std::endl;
   //	set_options_pack1();

     //int err=  solver->Iterate(MaxIters, MaxAccur);
   //  std::cout << "return code: " << err << std::endl;

     	return false;
	}
      if (solver->NumIters() ==0 || solver->NumIters() ==1000) 
      {
#ifdef SOLVEROUTPUT
         std::cout << "\033[1;31m:: GSSE::solver interface:: NOT solved !!\033[0m" << std::endl;
#endif
         return false;
      }
#ifdef DEBUG
      std::cout << "GSSE::solver interface (trilinos):: solved !!" << std::endl;
#endif
      return true;
   }

   bool solve()
   {
     //solver->SetAztecOption(AZ_output, AZ_none);
      solver->Iterate(1000, 1.1E-12);
      
      if (solver->NumIters() ==0 || solver->NumIters() ==1000) 
      {
         std::cout << "\033[1;31m::  GSSE::solver interface:: NOT solved !!\033[0m" << std::endl;
         return false;
      }
      
#ifdef DEBUG
      std::cout << "GSSE::solver interface (trilinos):: solved !!" << std::endl;
#endif
      return true;
   }


  // external multiplication of system matrix
  //

  bool multiply(const std::vector<numeric_t>& input, std::vector<numeric_t>& output ) const
  {

    Epetra_Vector temp_input(*Map);
    Epetra_Vector temp_output(*Map);

    for (int i = 0; i < NumMyElements; ++i)
      {
	temp_input[i] =  input[i];
        //std::cout << "temp: " << temp_input[i] << std::endl;
      }

  //Multiply (bool TransA, const Epetra_Vector &x, Epetra_Vector &y) const
  //	Returns the result of a Epetra_CrsMatrix multiplied by a Epetra_Vector x in y. 

    (*A).Multiply(false, temp_input, temp_output) ;

    for (int i = 0; i < NumMyElements; ++i)
      {
	output[i] = temp_output[i];
        //std::cout << "temp o: " << temp_output[i] << std::endl;
      }

    return true;
  }


   void write_mtx(const std::string& filename)
      {
         std::string filename_fullas=filename+".mtx";
         std::string filename_rhs=filename+"_rhs.mtx";

         std::ofstream fileout_as(filename_fullas.c_str());
         std::ofstream fileout_bs(filename_rhs.c_str());
         
         fileout_as << "RH:trilinos_outputfile matrix simple real general" << std::endl;
         fileout_as << dimension << "  " << dimension << "  " << matrix_as_container.size() << std::endl;
         for (size_t i = 0 ; i < matrix_as_container.size(); ++i)
         {
            fileout_as << boost::get<0>(matrix_as_container[i])+1 << " "     // mtx starts with "1"
                       << boost::get<1>(matrix_as_container[i])+1 << " " 
                       << boost::get<2>(matrix_as_container[i]) << " " << std::endl;
         }
         fileout_as.close();

         fileout_bs << "RH:trilinos_outputfile matrix simple real general" << std::endl;
         fileout_bs << dimension << "  1" << std::endl;

         for (int i = 0; i < dimension; ++i)
         {
            fileout_bs << (*b)[i] << std::endl;
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
         fileout_gen << "points = " << matrix_as_container.size()       << std::endl;   // requires the actual number of entries
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
            fileout_mat << boost::get<1>(matrix_as_container[i])+1 << " -"     // start with the column and negate .. ?? why ?
                        << boost::get<0>(matrix_as_container[i])+1 << " " 
                        << boost::get<2>(matrix_as_container[i]) << " " << std::endl;
         }
         fileout_mat.close();
      }


   void dump_html_table(long number_equ = 1)
      {
         print_all_html_table(number_equ);
      }

   void print_all_html_table(long number_equ)
      {

         std::cerr << "##### trilinos html output ## " << std::endl;
         std::cerr << "      trilinos size: " << matrix_as_container.size() << std::endl;

//          fileout_as << dimension << "  " << dimension << "  " << matrix_as_container.size() << std::endl;
//          for (size_t i = 0 ; i < matrix_as_container.size(); ++i)
//          {
//             fileout_as << boost::get<0>(matrix_as_container[i])+1 << " "     // mtx starts with "1"
//                        << boost::get<1>(matrix_as_container[i])+1 << " " 
//                        << boost::get<2>(matrix_as_container[i]) << " " << std::endl;
//          }
//          fileout_as.close();

//          fileout_bs << "RH:trilinos_outputfile matrix simple real general" << std::endl;
//          fileout_bs << dimension << "  1" << std::endl;

//          for (int i = 0; i < dimension; ++i)
//          {
//             fileout_bs << (*b)[i] << std::endl;
//          }

         std::vector<std::vector<std::string> > matrix_strings(dimension, std::vector<std::string>(dimension, "<td> &nbsp; </td>"));
         for (size_t i = 0 ; i < matrix_as_container.size(); ++i)
         {
            std::stringstream tokens;
            tokens << " <td> ";
            tokens  << boost::get<2>(matrix_as_container[i]) ;
            tokens << " </td> ";
            matrix_strings[ boost::get<0>(matrix_as_container[i]) ] [ boost::get<1>(matrix_as_container[i]) ] = tokens.str();
         }
       

         std::cerr << "<table border=\"1\" >" << std::endl;
         std::cerr << "<tr> <td> &nbsp; </td>";
         for (unsigned int k=0; k < (dimension); k++)
         {
            std::cerr << "<td> " << k << " </td>" ;
         }
         std::cerr << "<td> rhs </td> </tr>" << std::endl;
         
         for (unsigned int j=0; j < (dimension); j++)
         {
            std::cerr << "<tr> &nbsp; <td> " << j << "/" << j/number_equ << " </td>";
            for (unsigned int k=0; k < (dimension); k++)
            {		
               std::cerr << matrix_strings[j][k] ;

//                if (matrix[j][k] != 0.0)
//                   std::cerr << "<td> " << matrix[j][k] << "</td> ";
//                else
//                   std::cerr << "<td> &nbsp; </td>";
            }
            if ((*b)[j] != 0.0)
               std::cerr << "<td> " <<(*b)[j] << "</td> </tr>" << std::endl;
            else
               std::cerr << "<td> &nbsp; </td> </tr>" << std::endl;
         }
         std::cerr << "</table>" << std::endl;

      }


   typedef  trilinos_insert          entry_accessor;
   typedef  trilinos_rhs             rhs_accessor;
   typedef  trilinos_additive_insert additive_entry_accessor;
   typedef  trilinos_additive_rhs    additive_rhs_accessor;
   typedef  trilinos_solution        solution_accessor;
};


class trilinos_insert
{
   trilinos_solver_interface * msi;

public:

  trilinos_insert() {}
  trilinos_insert(trilinos_solver_interface & msi) : msi(&msi) {}

  void operator()(long index1, long index2, double entry)
  {
    (*msi).insert_value(index1, index2, entry);
  }
};

class trilinos_rhs
{
   trilinos_solver_interface * msi;

public:

   trilinos_rhs()  {}
   trilinos_rhs(trilinos_solver_interface & msi) : msi(&msi) {}

   void operator()(long index1, double entry)
      {
         (*msi).init_rhs(index1, entry);
      }
};

class trilinos_additive_insert
{
   trilinos_solver_interface * msi;
   
public:
   
   trilinos_additive_insert() {}
   
   trilinos_additive_insert(trilinos_solver_interface & msi) : msi(&msi) {}
   
   void operator()(long index1, long index2, double entry)
      {
         (*msi).add_value(index1, index2, entry);
      }
   
};

class trilinos_additive_rhs
{
   trilinos_solver_interface * msi;
   
public:

   trilinos_additive_rhs()  {}

   trilinos_additive_rhs(trilinos_solver_interface & msi) : msi(&msi) {}
   
   void operator()(long index1, double entry)
      {
         (*msi).add_rhs(index1, entry);
      }
};




class trilinos_solution
{
  trilinos_solver_interface * msi;

public:

  trilinos_solution()  {}

  trilinos_solution(trilinos_solver_interface & msi) : msi(&msi) {}

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
