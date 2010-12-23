/* ============================================================================
   Copyright (c) 2004-2009 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2005-2008 Philipp Schwaha                      philipp@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_DIRECT_GAUSS
#define GSSE_DIRECT_GAUSS

#include<vector>
#include<iostream>
#include<fstream>
#include<iomanip>

namespace gsse
{

template <typename NumericT> struct gauss_insert;  
template <typename NumericT> struct gauss_add;     
template <typename NumericT> struct gauss_rhs;     
template <typename NumericT> struct gauss_rhs_add;     
template <typename NumericT> struct gauss_solution;


template <typename NumericType>
struct gauss_solver_interface
{
private:
   std::vector<std::vector<NumericType> > matrix;
   std::vector<NumericType> rhs;
   std::vector<NumericType> solution;
   size_t dimension;
	
   bool initialized;

public:

   gauss_solver_interface()
      {
         initialized = false;
      }

   size_t size() { return dimension;}

   bool is_calculable(long dimension)
      {
         if (dimension > 3000)
             return false;
         return true;
      }

   void initialize(long numele)
   {
      initialized = true;

      dimension=numele;
      solution.resize(numele);
      rhs.resize(numele);
      matrix.resize(numele);
      for (long i=0; i<numele; ++i)
         matrix[i].resize(numele);

      for (long i=0; i<numele; ++i)
      {
         rhs[i] = 0.0;
         solution[i] = 0.0;
         for (long j=0; j<numele; ++j)
         {
            matrix[i][j] = 0.0;
         }
      }
   }

   gauss_solver_interface(long numele)
   {
      initialize(numele);
   }
   
   void insert_value(int row, int column, NumericType value)
   {
      matrix[row][column] = value;
   }

   void add_value(int  row, int column, NumericType value)
   {
      //std::cout << "add ad row: " << row << " col: " << column << std::endl;
      matrix[row][column] += value;
   }
  

   void init_rhs(int row, NumericType value)
   {
      rhs[row] = value;
   }

   bool get_initialize()
   {
      return initialized;
   }

   void add_rhs(int row, NumericType value)
   {
      rhs[row] += value;
   }
   
   long get_dimension()
   {
      return dimension;
   }   

   NumericType get_rhs_value(int row)
   {
      return rhs[row];
   }	
	
   NumericType get_matrix_value(int row, int column)
   {
      return matrix[row][column];
   }	
   
   NumericType get_x_value(int row)
   {
      return solution[row];
   }

   void prepare_solver()
   {
     
   }

   void set_options_pack1()  {  }
   void set_options_pack2()  {  }
   void set_options_pack3()  {  }
   void set_options_pack4()  {  }
   void set_options_pack_fem(){  }
   void switch_to_full_output_mode() { }

   bool solve()
   {
      gauss_forward(matrix, rhs);
      gauss_backward(matrix, rhs, solution);
      std::cout << "GSSE::solver interface (direct gauss):: solved !!" << std::endl;

      return true;
   }

   void dump()
      {
         print_all(matrix, rhs);
      }

   // [PS] added 10.10.2008  
   //
   void dump_html_table(long number_equ = 1)
   {
      print_all_html_table(matrix, rhs, number_equ);
   }


  // external multiplication of system matrix
  //
  bool multiply(const std::vector<double>& input, std::vector<double>& output ) const
  {
     std::cout << " ## NOT yet implemented !!. " << std::endl;
     return false;
  }


   void write_mtx(const std::string& filename)
   {
      std::cout << "GAUSS: not yet implemented.. " << std::endl;
   }
   
   void write_dx(const std::string& filename)
   {
      std::cout << "GAUSS: not yet implemented.. " << std::endl;
   }
   

      
private:
   
   void matrix_readin(std::vector<std::vector<NumericType> >& matrix, std::vector<NumericType>& righthand, const char* filename)
   {
      std::ifstream readin(filename);
      int order;
      readin >> order ;
      
      matrix = std::vector<std::vector<NumericType> >(order, std::vector<NumericType>(order));
      for (int i = 0; i < order; i++)
      {
         for (int j = 0; j < order; j++)
         {
            NumericType temp;
            readin >> temp;
            matrix[i][j] = temp;
         }
      }
      
      for (int i = 0; i < order; i++)
      {
         NumericType temp;
         readin >> temp;
         righthand.push_back(temp);
      }
   }
   
   void gauss_forward(std::vector<std::vector<NumericType> >& matrix, std::vector<NumericType>& righthand )
   {
      //std::cout << "  start:" << std::endl;
      //  print_all(matrix, righthand, righthand);
      for (unsigned int i=0; i < (matrix[0].size()-1); i++)
      {
         
         if (matrix[i][i] == NumericType(0))
         {
            unsigned int j;
            for (j = i+1; j < matrix.size(); j++)
            {
               //std::cout << "Pivotization" << std::endl;
               if (matrix[j][i] != NumericType(0)) 
               {
                  for (unsigned int k=i; k < (matrix.size()); k++)
                     std::swap(matrix[i][k], matrix[j][k]);
                  std::swap(righthand[i], righthand[j]);
                  break;
               }
               if (j == matrix.size()-1)
               {
                  std::cerr << "division by zero.. exit" << std::endl;
//                  exit(-1);
                  // [RH][TODO] throw exception
               }
            }
         }
         
         //      print_all(matrix, righthand, righthand);
         
         for (unsigned int j=i+1; j < (matrix.size()); j++)
         {
            for (unsigned int k=i+1; k < (matrix[j].size()); k++)
            {			
               // 				std::cout << "i/j/k: " << i << "/" << j << "/" << k << std::endl;
               // 				std::cout << "matrix[j][k]: " << matrix[j][k] << std::endl;
               // 				std::cout << "matrix[j][i]: " << matrix[j][i] << std::endl;
               // 				std::cout << "matrix[i][i]: " << matrix[i][i] << std::endl;
               // 				std::cout << "matrix[i][k]: " << matrix[i][k] << std::endl;
               matrix[j][k]-=  (matrix[j][i] / matrix[i][i]) * matrix[i][k];
            }
            righthand[j] -= matrix[j][i] / matrix[i][i] * righthand[i];
            for (unsigned int k=0; k < (i+1); k++)
            {			
               matrix[j][k]= NumericType(0);
            }
            //      std::cout << "*";
         }
         //std::cout << std::endl;
         //      print_all(matrix, righthand, righthand);
         
      }
   }
   
   void gauss_backward(const std::vector<std::vector<NumericType> >& matrix, 
                       const std::vector<NumericType>& righthand, 
                       std::vector<NumericType>& solution )
   {
      if (solution.size() != matrix[0].size())
         solution.resize( matrix[0].size() );
      
      for (int i=(matrix[0].size()-1); i >= 0; i--)
      {
         unsigned int j;
         NumericType sum(0);
         for (j=i; j < (matrix.size()); j++)
         {
            sum += matrix[i][j]*solution[j];
         }
         solution[i] = (righthand[i]  - sum) / matrix[i][i];
// 		std::cout << "right: " << righthand[i] <<  "   sol: " << solution[i] << std::endl;
      }
   }
   
   NumericType matrix_mul(const std::vector<std::vector<NumericType> >& matrix, 
                          const std::vector<NumericType>& solution, const int& row )
   {
      NumericType sum(0);
      for (unsigned int k=0; k < (matrix[row].size()); k++)
      {			
         sum += matrix[row][k] * solution[k] ;
      }
      return sum;
//  	std::cout << "row["<<row<<"] : " << sum << std::endl;
   }
   
   
   void print_all(const std::vector<std::vector<NumericType> >& matrix, 
                  const std::vector<NumericType>& righthand)
   {
      std::cout << "## matrix: " << std::endl;
      for (unsigned int j=0; j < (matrix.size()); j++)
      {
         for (unsigned int k=0; k < (matrix[j].size()); k++)
         {		
            if (matrix[j][k] != 0.0)
               std::cout <<  std::setw(7) << std::setfill(' ') << j 
                         <<  std::setw(7) << std::setfill(' ') << k  
                         <<  std::setw(30) << std::setfill(' ') << std::right << matrix[j][k] << std::endl;
         }
      }
      std::cout << "## rhs: " << std::endl;
      for (unsigned int j=0; j < (matrix.size()); j++)
      {
//         if (righthand[j] != 0.0)
            std::cout <<  std::setw(7) << std::setfill(' ') << j 
                      <<  std::setw(30) << std::setfill(' ') << std::right << righthand[j] << std::endl;
      }
   }
   
   // [PS] added 10.10.2008  
   //
   void print_all_html_table(const std::vector<std::vector<NumericType> >& matrix, 
                             const std::vector<NumericType>& righthand,
                             long number_equ)
   {
      std::cerr << "<table border=\"1\" >" << std::endl;
      std::cerr << "<tr> <td> &nbsp; </td>";
      for (unsigned int k=0; k < (matrix[0].size()); k++)
      {
         std::cerr << "<td> " << k << " </td>" ;
      }
      std::cerr << "<td> rhs </td> </tr>" << std::endl;
      
      for (unsigned int j=0; j < (matrix.size()); j++)
      {
         std::cerr << "<tr> &nbsp; <td> " << j << "/" << j/number_equ << " </td>";
         for (unsigned int k=0; k < (matrix[j].size()); k++)
         {		
            // if (matrix[j][k] != 0.0)
            //std::cerr <<  std::setw(7) << std::setfill(' ') << j 
            //          <<  std::setw(7) << std::setfill(' ') << k  
            //          <<  std::setw(30) << std::setfill(' ') << std::right << matrix[j][k] << std::endl;
            if (matrix[j][k] != 0.0)
               std::cerr << "<td> " << matrix[j][k] << "</td> ";
            else
               std::cerr << "<td> &nbsp; </td>";
         }
//         }
//         for (unsigned int j=0; j < (matrix.size()); j++)
//         {
//            if (righthand[j] != 0.0)
         //std::cerr <<  std::setw(7) << std::setfill(' ') << j 
         //          <<  std::setw(30) << std::setfill(' ') << std::right << righthand[j] << std::endl;
         if (righthand[j] != 0.0)
            std::cerr << "<td> " << righthand[j] << "</td> </tr>" << std::endl;
         else
            std::cerr << "<td> &nbsp; </td> </tr>" << std::endl;
      }
      std::cerr << "</table>" << std::endl;
      
   }
   

public:
   typedef gauss_insert<NumericType>   entry_accessor;
   typedef gauss_add<NumericType>      additive_entry_accessor;
   typedef gauss_rhs<NumericType>      rhs_accessor;
   typedef gauss_rhs_add<NumericType>  additive_rhs_accessor;
   typedef gauss_solution<NumericType> solution_accessor;
};


template <typename NumericType>
class gauss_insert
{
   gauss_solver_interface<NumericType> * msi;

public:

  gauss_insert() {}

  gauss_insert(gauss_solver_interface<NumericType> & msi) : msi(&msi) {}

  void operator()(long index1, long index2, NumericType entry)
  {
     (*msi).insert_value(index1, index2, entry);
  }

};

template <typename NumericType>
class gauss_add
{
   gauss_solver_interface<NumericType> * msi;

public:

   gauss_add() {}

   gauss_add(gauss_solver_interface<NumericType> & msi) : msi(&msi) {}

   void operator()(long index1, long index2, NumericType entry)
   {
      (*msi).add_value(index1, index2, entry);
   }
   
};

template <typename NumericType>
class gauss_rhs
{
  gauss_solver_interface<NumericType> * msi;

public:

   gauss_rhs()  {}
   
   gauss_rhs(gauss_solver_interface<NumericType> & msi) : msi(&msi) {}

  void operator()(long index1, NumericType entry)
  {
    (*msi).init_rhs(index1, entry);
  }


};

template <typename NumericType>
class gauss_rhs_add
{
  gauss_solver_interface<NumericType> * msi;

public:

   gauss_rhs_add()  {}
   
   gauss_rhs_add(gauss_solver_interface<NumericType> & msi) : msi(&msi) {}

  void operator()(long index1, NumericType entry)
  {
    (*msi).add_rhs(index1, entry);
  }


};


template <typename NumericType>
class gauss_solution
{
  gauss_solver_interface<NumericType> * msi;

public:

   gauss_solution()  {}
   
   gauss_solution(gauss_solver_interface<NumericType>  & msi) : msi(&msi) {}
   
   NumericType operator()(long index1)
      {
         return (*msi).get_x_value(index1);
      }
};


}  //namespace gsse


#endif
