/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 René Heinzl         rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha
     Copyright (c) 2005-2006 Michael Spevak
     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

// *** GSSE includes
//
#include "gsse/matrix_solver_interface.hpp"
#include "gsse/math/assemble.hpp"


#ifndef GSSE_ALGO
#define GSSE_ALGO

// This is restricted to the standard solver interface

namespace gsse
{

template <typename Iterator, typename Equation, typename Variable, typename Entry>
void solve_linear(Iterator iter, 
		  Equation eqn, Variable var, Entry ent, long size)
{
  matrix_solver_interface msi(size); // (iter.end() - iter);
  msi_insert ins(msi);
  msi_rhs    rhs(msi);
  
  for(; iter.valid(); ++iter)
    {
      typename Iterator::value_type val = *iter;
      assemble_line(static_cast<long>(ent(val)), eqn(val), ins, rhs);
      //std::cout << ent(val) << "   " <<  eqn(val) << std::endl;
    }

  //msi.dump();
  msi.prepare_solver();
  //msi.switch_to_full_output_mode();

  msi.set_options_pack4();
  msi.solve();
  
  for(iter.reset(); iter.valid(); ++iter)
    {
      typename Iterator::value_type val = *iter;
      var(val) =  msi.get_x_value(static_cast<long>(ent(val)));
    }
}

}

#endif
