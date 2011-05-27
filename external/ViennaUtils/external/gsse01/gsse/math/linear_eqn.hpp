/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

////////////////////////////////////////////////////////////////////////////////////////////
//
// The "linear_equation" class implements the linearization of a 
// nonlinear equation. It is usually used after the discretization 
// of a nonlinear differential equation. The "linear_equation" stores
// the value of a function as well as the first derivative in the 
// direction of all dependent variables:
// -) EntryT is something that returns a get_value with the type of NumericT. 
// -) NumericT has to be default constructable.
//
/////////////////////////////////////////////////////////////////////////////////////////////


#ifndef GSSE_LINEAR_EQN_HH
#define GSSE_LINEAR_EQN_HH

// *** system includes
//
#include <map>
#include <cmath>
#include <iostream>
#include <cmath>

// *** BOOST includes
//
#include <boost/operators.hpp>



namespace gsse
{

template <typename NumericT>
class linear_equation :
   boost::operators< linear_equation<NumericT> >
   , boost::addable2< linear_equation<NumericT>,  NumericT
   , boost::subtractable2< linear_equation<NumericT>,  NumericT
   , boost::multipliable2< linear_equation<NumericT>,  NumericT
   , boost::dividable2< linear_equation<NumericT>,  NumericT
			> > > >
{

private:
   typedef linear_equation                              self;
   typedef NumericT                                     numeric_t;

   typedef typename std::map<long, numeric_t>           matrix_line_t;             
   typedef typename matrix_line_t::iterator             matrix_line_iterator_nc;

public:
   typedef typename matrix_line_t::const_iterator       matrix_line_iterator;      
   typedef typename matrix_line_t::const_iterator       iterator;      

   // /////////////////////////////////////
   // 
   // iterator/container capabilities for the dependent variables and
   // their coefficients. The non-constant iterators are for internal
   // purposes only.

private:

  // Internal methods for non constant member access

   matrix_line_iterator_nc nc_begin()        {  return matrix_line.begin(); }
   matrix_line_iterator_nc nc_end()          {  return matrix_line.end();   }

public:
   // /////////////////////////////////////
   // 
   // Constructors are available for EntryT, numeric_t. The default
   // constructor initializes the value with the default value of the
   // numeric_t.
   
   // creates an empty linear equation
   //
   linear_equation() {value=0.0;}   
   

   // creates a linear_equation from an entry
//    explicit
//    linear_equation(long const entry) 
//    {
//       value = 0.0;
//       matrix_line[entry] = numeric_t(1);
//    }

   
   // creates a linear_equation from an entry with corresponding rhs value
   explicit
   linear_equation(long const entry, numeric_t const val) 
   {
   //	std::cout << "constructor:: " << entry << " / " << val << std::endl;
      value = val;
      matrix_line[entry] = numeric_t(1);
   }


   // creates a linear_equation from pure numeric Type this
   // constructor is explicitely non-explicit, because it shall be
   // initialized with integer values 
   linear_equation(const numeric_t& val) : value(val)  { }

   //
   //
   ///////////////////////////////////////


   matrix_line_iterator    begin() const     {  return matrix_line.begin(); }
   matrix_line_iterator    end()   const     {  return matrix_line.end();   }
  
   // 
   // 
   ///////////////////////////////////////

   ///////////////////////////////////////
   // 
   // using the () operator we can determine the rhs value of the control function

   numeric_t&       operator()()       { return value; }
   numeric_t const& operator()() const { return value; }

   // 
   // 
   ///////////////////////////////////////

   ///////////////////////////////////////
   // 
   // using the [] operator we can return the coefficients for the
   // dependent variables

   numeric_t&       operator[](long const entry)       { return matrix_line[entry]; }
   numeric_t const& operator[](long const entry) const { return matrix_line[entry]; }

   // 
   // 
   // /////////////////////////////////////
   
   
   // /////////////////////////////////////
   // 
   //  The addition operator
   
   self& operator+=(self const& other)
   {
      matrix_line_iterator iter;
      value += other();
      
      for (iter = other.begin(); iter != other.end(); ++iter)
      {
         matrix_line[(*iter).first] += (*iter).second;
      }
      return *this;
   }

   self& operator+=(numeric_t const& other)
   {
      value += other;
      return *this;
   }

   //
   //
   ///////////////////////////////////////


   ///////////////////////////////////////
   // 
   //  The subtraction operator


   self& operator-=(self const& other)
   {
      matrix_line_iterator iter;
      value -= other();

      for (iter = other.begin(); iter != other.end(); ++iter)
      {
	matrix_line[(*iter).first] -= (*iter).second;
      }
      return *this;
   }

   self& operator-=(numeric_t const& other)
   {
      value -= other;
      return *this;
   }

   //
   //
   ///////////////////////////////////////

   ///////////////////////////////////////
   // 
   //  The unary minus operator

   self operator-() const
   {
      iterator iter;
      self result;
      
      result() = -(*this)();

      // use the const iterator
      for (iter = begin(); iter != end(); ++iter)
      {
         result.matrix_line[(*iter).first] = -(*iter).second;
      }
      return result;
   }

   //
   //
   ///////////////////////////////////////

   ///////////////////////////////////////
   // multiplies operator
   //
   //  f(x1, x2, ...) = x1 * f1 + x2 * f2 + .... + fvalue
   //  g(x1, x2, ...) = x1 * g1 + x2 * g2 + .... + gvalue
   //  e              = xn                       + evalue
   //  c              =                            cvalue
   //
   // The linearization of the multiplication yields:
   //
   //  L(f * g) = fvalue * (x1 * g1 + x2 * g2 + ...) + gvalue * (x1 * f1 + x2 * f2 + ...) + fvalue * gvalue
   //  L(f * e) = fvalue * xn                        + evalue * (x1 * f1 + x2 * f2 + ...) + fvalue * evalue
   //  L(f * e) =                                    + cvalue * (x1 * f1 + x2 * f2 + ...) + fvalue * cvalue

   self& operator*=(const linear_equation& other) 
   {
      matrix_line_iterator_nc iter;
  
      for (iter = nc_begin(); iter != nc_end(); ++iter)
      {
         (*iter).second *= other();
      }

      matrix_line_iterator other_iter;

      for (other_iter = other.begin(); other_iter != other.end(); ++other_iter)
      {
         matrix_line[(*other_iter).first] += (*other_iter).second * value;
      }  

      value *= other();              

      return *this;
   }

   self& operator*=(const numeric_t& other) 
   {
      matrix_line_iterator_nc iter;
  
      for (iter = nc_begin(); iter != nc_end(); ++iter)
      {
         (*iter).second *= other;
      }

      value *= other;              

      return *this;
   }

   //
   //
   ///////////////////////////////////////
  

   ///////////////////////////////////////
   // 
   //  The divides operator
   // 
   //  This operator is only for numeric data types because the
   //  division of two control functions is not defined for each
   //  case. There are some cases (e.g. both values are zero and one
   //  has to use del Hospital's rule) when the implementation would
   //  either not lead to correct results or does not cover the whole
   //  problematics at all (e.g. when second derivatives would be
   //  necessary). For this reason it might be proper to use
   //  functions (e.g the Bernoulli function) which provide the
   //  correct derivatives for all discontinuities. [MS]


   self& operator/=(const linear_equation& other) 
   {
      matrix_line_iterator_nc iter;
  
      for (iter = nc_begin(); iter != nc_end(); ++iter)
      {
         (*iter).second /= other();
      }

      matrix_line_iterator other_iter;

      for (other_iter = other.begin(); other_iter != other.end(); ++other_iter)
      {
         matrix_line[(*other_iter).first] -= value / ((*other_iter).second * (*other_iter).second);
      }  

      value /= other();              

      return *this;
   }

   self& operator/=(const numeric_t other) 
   {
      matrix_line_iterator_nc iter;
  
      for (iter = nc_begin(); iter != nc_end(); ++iter)
      {
         (*iter).second /= other;
      }

      value /= other;              

      return *this;
   }

   // ===============================================
   //
   numeric_t solve(const long var)
   {
      return -(*this)()/(*this)[var];
   }


   // ===============================================
   //
   template <typename Func>
   self& apply_func(Func const&)
   {  
      Func function;
      typename Func::derivative deriv;
      
      NumericT outer_derivative = deriv((*this)());

      (*this)() = function((*this)());
	
      // iterator adaptor for second needed => lambda expr
      
//       std::cout << "### derivatives: " << std::endl;
      typename linear_equation<NumericT>::matrix_line_iterator_nc iter;           
      for (iter = nc_begin(); iter != nc_end(); ++iter)
      {
//          std::cout << "second: " << (*iter).second << "    / outer : " << outer_derivative << std::endl;
         (*iter).second *= outer_derivative;
      }  
      
      return *this;
   }

   template <typename Env>
   struct result 
   { 
      typedef numeric_t type;
   };




	// application part
	//    .. heuristics for removal of couplings
	//
    template<typename ElementT>
    void remove_couplings(ElementT element)
    {
    	value = 0.0;   // reset the residual value

	for (matrix_line_iterator_nc iter = nc_begin(); iter != nc_end(); ++iter)
	{
		if (element != (*iter).first )
			(*iter).second = 0.0;    // delete coupling entries
//                 else
// 			(*iter).second = 1.0;    // retain diagonal dominance
	}
    }

	// application part
	//    .. heuristics for potential damping
	//
    template<typename ElementT>
    void damping(ElementT element1, ElementT element2, double damping_factor)    // the given range of damping
    {
 //   	std::cout << "eleme1: "<< element1 << std::endl;
 //   	std::cout << "eleme2: "<< element2 << std::endl;
 //   	std::cout << "damp:   "<< damping_factor << std::endl;
	for (matrix_line_iterator_nc iter = nc_begin(); iter != nc_end(); ++iter)
	{

   //   std::cout << "before: " <<  (*iter).second << " * [."  << (*iter).first << ".]" << std::endl;;
		if ( (*iter).first >= element1 && (*iter).first <= element2 )
		{
			(*iter).second *= damping_factor;    // damp the potential part
//std::cout << "--- damping : " << (*iter).second << std::endl;
		}
	}

    }



private:
   // Function value
   numeric_t value;    

   // *** First Derivatives in each direction 
   matrix_line_t matrix_line;
  
};
//
//
/////////////////////////////////////////////////////////////////////////////////////////////



// The output operator
//
template <typename NumericT>
std::ostream& operator<< (std::ostream &ostr, linear_equation<NumericT> const& f)
{
   typename linear_equation<NumericT>::matrix_line_iterator iter;
  
   ostr << f();
  
   for (iter = f.begin(); iter != f.end(); ++iter)
   {
      //if ((*iter).second == 0.0) continue;
      ostr << " + " << (*iter).second << " * [."  << (*iter).first << ".]";
   }
  
   return ostr;
}


struct sin_impl
{
   template <typename NumericT>
   NumericT operator()(NumericT const x)
   {
      return std::sin(x);
   }

   struct derivative
   {
      template <typename NumericT>
      NumericT operator()(NumericT const x)
      {
         return std::cos(x);
      }
   };
};


struct cos_impl
{
   template <typename NumericT>
   NumericT operator()(NumericT const x)
   {
      return std::cos(x);
   }

   struct derivative
   {
      template <typename NumericT>
      NumericT operator()(NumericT const x)
      {
         return -std::sin(x);
      }
   };
};


struct exp_impl
{
   template <typename NumericT>
   NumericT operator()(NumericT const x)
   {
      return std::exp(x);
   }

   struct derivative
   {
      template <typename NumericT>
      NumericT operator()(NumericT const x)
      {
         return std::exp(x);
      }
   };
};


// this implementation is taken from the SGFramework framework

struct bern_impl
{
   template <typename NumericT>
   NumericT operator()(NumericT const x)
   {
      NumericT y;
//        std::cout << "######  bern impl: x:" << x << std::endl;


//       if (x <= -3.742994775023696e+01)
//       { std::cout << " 1: " << -x << std::endl; }
//       else if (x <  -2.019464984136394e-02)
//       { std::cout << " 2: " << (x / (exp(x) - 1.0)) << std::endl; }
//       else if (x <= 2.449889303055096e-02)
//       { std::cout << " 3: " << (1.0 - x/2.0 * (1.0 - x/6.0 * (1.0 - x*x/60.0))) << std::endl; }
//       else if (x <  3.742994775023696e+01)
//       { y = exp(-x);   std::cout << " 4: " << ((x * y) / (1.0 - y)) << std::endl; }
//       else if (x < 7.083964185322633e+02 )
//       { std::cout << " 5: " << (x * exp(-x)) << std::endl; }
//       else { std::cout << " 6: " << (0.0) << std::endl; }


      if (x <= -3.742994775023696e+01)
      { return(-x); }
      else if (x <  -2.019464984136394e-02)
      { return(x / (exp(x) - 1.0)); }
      else if (x <= 2.449889303055096e-02)
      { return(1.0 - x/2.0 * (1.0 - x/6.0 * (1.0 - x*x/60.0))); }
      else if (x <  3.742994775023696e+01)
      { y = exp(-x);   return((x * y) / (1.0 - y)); }
      else if (x < 7.083964185322633e+02 )
      { return(x * exp(-x)); }
      else { return(0.0); }
   }

   struct derivative
   {
      template <typename NumericT>
      NumericT operator()(NumericT const x)
      {
         NumericT y, z;

         if (x <= -4.117169706049845e+01)
         { return(-1.0); }
         else if (x <= -3.742994775023696e+01)
         { return((1.0 - x)*exp(x) - 1.0); }
         else if (x <  -1.440190569485648e-02)
         { y = exp(x); z = y - 1.0; return(((1.0 - x)*y - 1.0)/(z*z)); }
         else if (x <= 1.934659498020159e-02)
         { return(-0.5 + x/6.0 * (1.0 - x*x/30.0)); }
         else if (x <  3.742994775023696e+01)
         { y = exp(-x); z = 1 - y;  return(((1.0 - x)*y - y*y )/(z*z)); }
         else if (x < 7.083964185322633e+02)
         { y = exp(-x); return((1.0 - x)*y - y*y); }
         else { return(0.0); }
      }
   };
};

struct bern_impl2
{
   template <typename NumericT>
   NumericT operator()(NumericT const x)
   {
      if (std::abs(x) > 0.1) return (x / (std::exp(x) - 1));
      else return 1.0 - x * 0.5;
   }

   struct derivative
   {
      template <typename NumericT>
      NumericT operator()(NumericT const x)
      {
	return 0.0;
      }
   };
};



// struct edge_log
// {
//    template <typename NumericT>
//    NumericT operator()(NumericT const x)
//    {
//       if (std::abs(x) > 0.1) return (x / (std::exp(x) - 1));
//       else return 1.0 - x * 0.5;
//    }

//    struct derivative
//    {
//       template <typename NumericT>
//       NumericT operator()(NumericT const x)
//       {
// 	return 0.0;
//       }
//    };
// };





template <typename NumericT>
linear_equation<NumericT> sin(linear_equation<NumericT> x)
{
   linear_equation<NumericT> result(x);
   result.apply_func(sin_impl());
   return result;
}

double sin(double x)
{
   return std::sin(x);
}

template <typename NumericT>
linear_equation<NumericT> cos(linear_equation<NumericT> x)
{
   linear_equation<NumericT> result(x);
   result.apply_func(cos_impl());
   return result;
}

double cos(double x)
{
   return std::cos(x);
}

template <typename NumericT>
linear_equation<NumericT> exp(linear_equation<NumericT> x)
{
   linear_equation<NumericT> result(x);
   result.apply_func(exp_impl());
   return result;
}

double exp(double x)
{
   return std::exp(x);
}


template <typename NumericT>
linear_equation<NumericT> bern(linear_equation<NumericT> x)
{
//	std::cout << "bern lambda impl with linear_equation " << std::endl;
   linear_equation<NumericT> result(x);
   result.apply_func(bern_impl());
   return result;
}

//  template <typename LinearEquationT>
//  LinearEquationT bern(LinearEquationT x)
//  {
//     LinearEquationT  result(x);
//     (result.get()).apply_func(bern_impl());
//     return result;
//  }


// template <typename LinearEquationT>
// typename boost::phoenix::detail::unwrap_local_reference<LinearEquationT>::type  bern(LinearEquationT x)
// {
// //   LinearEquationT result(x);
//    typename boost::phoenix::detail::unwrap_local_reference<LinearEquationT>::type local_result(x);
//    local_result.apply_func(bern_impl());
// //   (result.get()).apply_func(bern_impl());
//    return local_result;
// }


double bern(double x)
{
   return bern_impl()(x);
}

double bern2(double x)
{
   return bern_impl2()(x);
}


// added for HD [PS]
struct log_impl
{
   template <typename NumericT>
   NumericT operator()(NumericT const x)
   {
//        std::cout << "##### log op(): " << x << " / " << std::log(x) << std::endl;
//        if (x <= 0.0)
//          return 1.0;
//       else
         return std::log(x);
   }

   struct derivative
   {
      template <typename NumericT>
      NumericT operator()(NumericT const x)
      {
//       std::cout << "##### log derivative op(): " << x << " / " << NumericT(1) / (x) << std::endl;
         return NumericT(1) / (x);
      }
   };
};

template <typename NumericT>
linear_equation<NumericT> log(linear_equation<NumericT> x)
{
   linear_equation<NumericT> result(x);
   result.apply_func(log_impl());
   return result;
}

template<typename NumericT>
NumericT log(NumericT num)
{
   return std::log(num);
}

}  //end of namespace gsse



#endif
