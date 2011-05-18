/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_PROPERTY_TREE_200802
#define GSSE_PROPERTY_TREE_200802

// ########### [RH]  is to be replaced by boost::property_tree ########
// 


// *** system includes
//
#include <algorithm>
#include <map>

// *** BOOST includes
//
// #include <boost/lambda/lambda.hpp>
//#include <boost/fusion/sequence.hpp>

// *** GSSE includes
//

namespace gsse
{

struct property_tree
{
 
 	std::string& read_quan_property_at(std::string quan_name, std::string property)
	{
	   return main_container[quan_name][property];
	}

	std::map<std::string, std::string>& operator[](std::string quan_name) { return main_container[quan_name]; }

private:
	std::map<std::string, std::map<std::string, std::string> > main_container; 
};


}



#endif
