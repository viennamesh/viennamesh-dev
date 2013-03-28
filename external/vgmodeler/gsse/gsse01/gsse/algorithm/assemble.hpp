/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_ASSEMBLE_HH
#define GSSE_ASSEMBLE_HH

namespace gsse
{
  
  template <typename LinEqnType, typename InsertFunctor, typename RHSFunctor>
  void assemble_line(long line_number, LinEqnType const& eqn, InsertFunctor& insert, RHSFunctor& rhs)
  {
    typename LinEqnType::iterator iter;

    for(iter = eqn.begin(); iter != eqn.end(); ++iter)
      {
	long col_number = (*iter).first;
	double entry = (*iter).second;
	// std::cout << index1 << "  " << index2 << "  " << entry << std::endl;
        //if (std::abs(entry) > 1E-7)
           insert(line_number, col_number, entry);
      }
    double rhs_entry = -eqn();
    rhs(line_number, rhs_entry);
  }
}

#endif
