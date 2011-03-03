/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_PETSC_INTERFACE_HH
#define GSSE_PETSC_INTERFACE_HH

// ****************** specific solver includes ***************************
#include "/home/students/petsc-2.3.3-p13/include/petscksp.h"
static char help[] = "Block Jacobi preconditioner ...";


// ****************** system includes ***************************
#include <vector>
#include <fstream>
#include<iostream>


namespace gsse
{


// ----------------------------------
// forward declarations
//

class petsc_insert;
class petsc_rhs;
class petsc_additive_insert;
class petsc_additive_rhs;
class petsc_solution;

// ----------------------------------

// implementation
//
class petsc_solver_interface
{
  bool initialized;

  Vec            x, b;         /* approx solution, RHS, exact solution */
  //  Vec            u; 
  Mat            A;            /* linear system matrix */
  KSP            ksp;         /* linear solver context */
  PC             pc;           /* preconditioner context */
  PetscReal      norm;         /* norm of solution error */
  PetscErrorCode ierr;
  PetscInt       i,n,col[3],its;
  PetscMPIInt    size;
  PetscScalar    neg_one,one,value[3];
  std::vector<double>   sol_vec;
 
  Vec temp_input;     // different mulitplications..
  Vec temp_output;


  int argc;
  char ** args;


public:
  long initialize(long numele)
  {
    initialized = true;

    n = numele;
    neg_one = -1.0;
    one = 1.0;
    argc = 0;
    args = 0;


    // -------------------
    PetscInitialize(&argc,&args,(char *)0,help);
    ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);
    if (size != 1) SETERRQ(1,"This is a uniprocessor example only!");
    ierr = PetscOptionsGetInt(PETSC_NULL,"-n",&n,PETSC_NULL);CHKERRQ(ierr);

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
       Compute the matrix and right-hand-side vector that define
       the linear system, Ax = b.
       - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    ierr = VecCreate(PETSC_COMM_WORLD,&x);CHKERRQ(ierr);
    ierr = PetscObjectSetName((PetscObject) x, "Solution");CHKERRQ(ierr);
    ierr = VecSetSizes(x,PETSC_DECIDE,n);CHKERRQ(ierr);
    ierr = VecSetFromOptions(x);CHKERRQ(ierr);
    ierr = VecDuplicate(x,&b);CHKERRQ(ierr);
    //    ierr = VecDuplicate(x,&u);CHKERRQ(ierr);

    ierr = MatCreate(PETSC_COMM_WORLD,&A);CHKERRQ(ierr);
    ierr = MatSetSizes(A,PETSC_DECIDE,PETSC_DECIDE,n,n);CHKERRQ(ierr);
    ierr = MatSetFromOptions(A);CHKERRQ(ierr);

    VecCreate(PETSC_COMM_WORLD,&temp_input);
    ierr = PetscObjectSetName((PetscObject) temp_input, "Tempinput");CHKERRQ(ierr);
    VecSetSizes(temp_input,PETSC_DECIDE,n);
    ierr = VecSetFromOptions(temp_input);CHKERRQ(ierr);
    VecCreate(PETSC_COMM_WORLD,&temp_output);
    ierr = PetscObjectSetName((PetscObject) temp_output, "Tempoutput");CHKERRQ(ierr);
    VecSetSizes(temp_output,PETSC_DECIDE,n);
    ierr = VecSetFromOptions(temp_output);CHKERRQ(ierr);


    return 0;
  }

private:

  long internal_solve()
  {

    /* 
       Create linear solver context
    */
    ierr = KSPCreate(PETSC_COMM_WORLD,&ksp);CHKERRQ(ierr);
    ierr = KSPSetOperators(ksp,A,A,DIFFERENT_NONZERO_PATTERN);CHKERRQ(ierr);
    ierr = KSPGetPC(ksp,&pc);CHKERRQ(ierr);
    ierr = PCSetType(pc,PCJACOBI);CHKERRQ(ierr);
    ierr = KSPSetTolerances(ksp,1.e-7,PETSC_DEFAULT,PETSC_DEFAULT,PETSC_DEFAULT);CHKERRQ(ierr);
    ierr = KSPSetFromOptions(ksp);CHKERRQ(ierr);
 
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
       Solve the linear system
       - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    ierr = KSPSolve(ksp,b,x);CHKERRQ(ierr); 

    PetscScalar*   solution_vector;
    VecGetArray(x,&solution_vector);
    for (long i =0; i < n; ++i)
      {
	sol_vec.push_back(solution_vector[i]);
      }

    int reason=0;
    KSPGetConvergedReason(ksp,(KSPConvergedReason*)&reason);
    switch(reason)
      {
      case 2:
	std::cout << "### CONVERGED ### " << std::endl;
	std::cout << "### KSP ..KSP_CONVERGED_RTOL .. " << std::endl;
	break;
      case 3:
	std::cout << "### CONVERGED ### " << std::endl;
	std::cout << "### KSP ..KSP_CONVERGED_ATOL .. " << std::endl;
	break;
      case 4:
	std::cout << "### CONVERGED ### " << std::endl;
	std::cout << "### KSP ..KSP_CONVERGED_ITS .. " << std::endl;
	break;
      case 5:
	std::cout << "### CONVERGED ### " << std::endl;
	std::cout << "### KSP ..KSP_CONVERGED_QCG_NEG_CURVE .. " << std::endl;
	break;
      case 6:
	std::cout << "### CONVERGED ### " << std::endl;
	std::cout << "### KSP ..KSP_CONVERGED_QCG_CONSTRAINED .. " << std::endl;
	break;
      case 7:
	std::cout << "### CONVERGED ### " << std::endl;
	std::cout << "### KSP ..KSP_CONVERGED_STEP_LENGTH .. " << std::endl;
	break;
      case -3:
	std::cout << "### !! N O T  !! CONVERGED ### " << std::endl;
	std::cout << "### KSP ..KSP_DIVERGED_ITS .. " << std::endl;
	break;
      case -4:
	std::cout << "### !! N O T  !! CONVERGED ### " << std::endl;
	std::cout << "### KSP ..KSP_DIVERGED_DTOL .. " << std::endl;
	break;
      case -5:
	std::cout << "### !! N O T  !! CONVERGED ### " << std::endl;
	std::cout << "### KSP ..KSP_DIVERGED_BREAKDOWN  .. " << std::endl;
	break;
      case -6:
	std::cout << "### !! N O T  !! CONVERGED ### " << std::endl;
	std::cout << "### KSP ..KSP_DIVERGED_BREAKDOWN_BICG .. " << std::endl;
	break;
      case -7:
	std::cout << "### !! N O T  !! CONVERGED ### " << std::endl;
	std::cout << "### KSP ..KSP_DIVERGED_NONSYMMETRIC .. " << std::endl;
	break;
      case -8:
	std::cout << "### !! N O T  !! CONVERGED ### " << std::endl;
	std::cout << "### KSP ..KSP_DIVERGED_INDEFINITE_PC .. " << std::endl;
	break;

      default:
	std::cout << "### KSP .. no problem detected.. " << std::endl;
	break;

      }

    

    /* 
    ierr = KSPView(ksp,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);



       Check the error
    ierr = VecAXPY(x,neg_one,u);CHKERRQ(ierr);
    ierr  = VecNorm(x,NORM_2,&norm);CHKERRQ(ierr);
    ierr = KSPGetIterationNumber(ksp,&its);CHKERRQ(ierr);
    ierr = PetscPrintf(PETSC_COMM_WORLD,"Norm of error %A, Iterations %D\n",
		       norm,its);CHKERRQ(ierr);
    */
    return 0;
  }

  int insert_value_internal(int row, int column, double value)
  {
    ierr = MatSetValues(A,1,&row,1,&column,&value,INSERT_VALUES);CHKERRQ(ierr);
    return 0;
  }
  int add_value_internal(int row, int column, double value)
  {
    ierr = MatSetValues(A,1,&row,1,&column,&value,ADD_VALUES);CHKERRQ(ierr);
    return 0;
  }
  int replace_value_internal(int row, int column, double value)
  {
    ierr = MatSetValues(A,1,&row,1,&column,&value,INSERT_VALUES);CHKERRQ(ierr);
    return 0;
  }

  int prepare_solver_internal()
  {
    ierr = MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
    ierr = MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
    return 0;
  }

  long finalize()
  {
    ierr = VecDestroy(x);CHKERRQ(ierr); 
    //    ierr = VecDestroy(u);CHKERRQ(ierr); 
    ierr = VecDestroy(b);CHKERRQ(ierr); ierr = MatDestroy(A);CHKERRQ(ierr);
    ierr = KSPDestroy(ksp);CHKERRQ(ierr);
    ierr = PetscFinalize();CHKERRQ(ierr);
    return 0;
  }
 
 public:
  petsc_solver_interface()
  {
    initialized = false;
  }

  petsc_solver_interface(long numele)
  {
    initialize(numele);
  }

  ~petsc_solver_interface()
  {
    if (!initialized) return;
    finalize();
  }

   bool is_calculable(long dimension)
      {
         return true;
      }


  void insert_value(int  row, int column, double value)
  {
    insert_value_internal(row, column, value);
  }

  void add_value(int  row, int column, double value)
  {
    add_value_internal(row, column, value);
  }

  void replace_value(int  row, int column, double value)
  {
    insert_value_internal(row, column, value);
  }  

  void init_rhs(int row, double value)
  {
    VecSetValues(b,1,&row,&value, INSERT_VALUES);
  }

  void add_rhs(int row, double value)
  {
   VecSetValues(b,1,&row,&value, ADD_VALUES);
  }

  double get_x_value(int row)
  {
    return sol_vec.at(row) ;
  }

  void init_rhs(std::vector<double>  rhs)
  {
    std::cerr << "init_rhs(std::vector<double>  rhs) : not YET implemented " << std::endl;
  }

  void init_rhs(double d, int size)
  {
    std::cerr << "init_rhs(double d, int size) : not YET implemented " << std::endl;
  }

  bool get_initialize()
  {
    return initialized;
    
  }

  double get_rhs_norm()
  {
    double rhs_norm;
    std::cerr << "double get_rhs_norm() : not YET implemented " << std::endl;
    return rhs_norm;
  }

  long get_dimension()
  {
    return n;
  }   
  
  void prepare_solver()
  {      
    prepare_solver_internal();
  }

  void set_options_pack1()
  {
    std::cerr << "set_options_pack1() : not YET implemented " << std::endl;
  }
  void set_options_pack2()
  {
    std::cerr << "set_options_pack2() :  not YET implemented " << std::endl;
  }
  void set_options_pack3()
  {
    std::cerr << "set_options_pack3() : not YET implemented " << std::endl;
  }
  void set_options_pack4()
  {
    std::cerr << "set_options_pack4() : not YET implemented " << std::endl;
  }
  void set_options_pack_fem()
  {
    std::cerr << "set_options_pack_fem() : not YET implemented " << std::endl;
  }

  void switch_to_full_output_mode()
  {
    std::cerr << "switch_to_full_output_mode() : not YET implemented " << std::endl;
  }

  void dump()
  {
    std::ofstream fileout("matrix_dump.rh");
    std::cerr << "not YET implemented " << std::endl;
    fileout.close();
  }

  bool solve()
  {
    internal_solve();
    return true;
  }


  // external multiplication of system matrix
  //

  bool multiply(const std::vector<double>& input, std::vector<double>& output ) const
  {

    for (int i = 0; i < n; ++i)
      {
	VecSet(temp_input,input[i]);
      }

    MatMult(A, temp_input, temp_output) ;

    for (int i = 0; i < n; ++i)
      {
	PetscInt tempi=i;
	PetscScalar tempv;
	VecGetValues(temp_output, 1,&tempi,&tempv);
	output[i] = tempv;
      }

    return true;
  }






   void write_mtx(const std::string& filename)
      {
         std::cout << "PETSC: not yet implemented.. " << std::endl;
      }


   void write_dx(const std::string& filename)
      {
         std::cout << "PETSC: not yet implemented.. " << std::endl;
      }


  typedef  petsc_insert          entry_accessor;
  typedef  petsc_rhs             rhs_accessor;
  typedef  petsc_additive_insert additive_entry_accessor;
  typedef  petsc_additive_rhs    additive_rhs_accessor;
  typedef  petsc_solution        solution_accessor;
};



class petsc_insert
{
  petsc_solver_interface * msi;

 public:

  petsc_insert() {}

  petsc_insert(petsc_solver_interface & msi) : msi(&msi) {}

  void operator()(long index1, long index2, double entry)
  {
    (*msi).insert_value(index1, index2, entry);
  }

};

class petsc_rhs
{
  petsc_solver_interface * msi;

 public:
  petsc_rhs()  {}
  petsc_rhs(petsc_solver_interface & msi) : msi(&msi) {}

  void operator()(long index1, double entry)
  {
         (*msi).init_rhs(index1, entry);
  }
};

class petsc_additive_insert
{
  petsc_solver_interface * msi;

 public:
  petsc_additive_insert() {}
  petsc_additive_insert(petsc_solver_interface & msi) : msi(&msi) {}

  void operator()(long index1, long index2, double entry)
  {
         (*msi).add_value(index1, index2, entry);
  }
};

class petsc_additive_rhs
{
  petsc_solver_interface * msi;
 public:
  petsc_additive_rhs()  {}
  petsc_additive_rhs(petsc_solver_interface & msi) : msi(&msi) {}

  void operator()(long index1, double entry)
  {
         (*msi).add_rhs(index1, entry);
  }
};




class petsc_solution
{
  petsc_solver_interface * msi;
 public:
  petsc_solution()  {}
  petsc_solution(petsc_solver_interface & msi) : msi(&msi) {}

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
