/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */


#ifndef GSSE_GENERICDELAUNAY_HH_ID
#define GSSE_GENERICDELAUNAY_HH_ID 

// *** system includes
//
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <ctime>
#include <sstream>
#include <iterator>                             //iterator traits
 
// *** BOOST includes
//
#include <boost/rational.hpp>
#include <boost/lexical_cast.hpp>

// *** GSSE includes
//
#include "external/bigint.hpp"


namespace gsse
{
 
// ################################################################
// ################################################################
//  .. typedefs ...
typedef big_radix_whole<10>             big_decimal_t;
typedef boost::rational<big_decimal_t>  rational_t;

typedef rational_t                      numeric_t;

typedef std::vector<numeric_t>          NumericVector;
// ################################################################
// ################################################################
 

 
 
// ################################################################
//     cast operators
// ################################################################
template<class Int, class Float>
Float  castBI2F(Int bigint)
{
	std::ostringstream ss1;
	ss1 << bigint;
	std::string s1 = ss1.str();
	Float testfloat = boost::lexical_cast<Float>(s1);
	return testfloat;
}

// 2*(lim-1) must be representable as Int
template<class Int, class Float> 
boost::rational<Int> castF2BI(const Float& v, const Int& lim)
{
	typedef boost::rational<Int> Result;
	// cf. http://www.algorithms.org/MoinMoin/wiki-moinmoin/moin.cgi/FareyRationalNumbers
	if(v<0)
	{
		return -castF2BI(-v,lim);
	}

	Int lower0 = 0;
	Int lower1 = 1;
	Int upper0 = 1;
	Int upper1 = 0;

	while(1)
	{
		Int median0 = lower0+upper0;
		Int median1 = lower1+upper1;
		Float fmedian0=castBI2F<Int,Float>(median0);
		Float fmedian1=castBI2F<Int,Float>(median1);
		if(v*fmedian1>fmedian0)
		{
			if(lim<median1)
			{
				return Result(upper0,upper1);
			}
			else
			{
				lower0=median0;
				lower1=median1;
			}
		}
		else if(v*fmedian1<fmedian0)
		{
			if(lim<median1)
			{
				return Result(lower0,lower1);
			}
			else
			{
				upper0=median0;
				upper1=median1;
			}
		}
		else // equal
		{
			if(lim>=median1)
			{
				return Result(median0,median1);
			}
			else if(lower1<upper1)
			{
				return Result(lower0,lower1);
			}
			else
			{
				return Result(upper0,upper1);
			}
		}
	}
}



NumericVector Tetra_BariCenter(NumericVector &Points)
{
	NumericVector result;
	result.push_back((Points[0] + Points[3] + Points[6] + Points[9] ) / 4);
	result.push_back((Points[1] + Points[4] + Points[7] + Points[10]) / 4);
	result.push_back((Points[2] + Points[5] + Points[8] + Points[11]) / 4);

// 	std::cout << "Baricentre: " << result[0] << "   " << result[1] << "   " << result[2] << std::endl;

	return result;
}

NumericVector CrossProduct(NumericVector &V1, NumericVector &V2)
{
	NumericVector result;
	result.push_back(V1[1]*V2[2] - V1[2]*V2[1]);
	result.push_back(V1[2]*V2[0] - V1[0]*V2[2]);
	result.push_back(V1[0]*V2[1] - V1[1]*V2[0]);
	return result;
}

numeric_t ScalarProduct(NumericVector &V1, NumericVector &V2)
{
	return V1[0]*V2[0] + V1[1]*V2[1] + V1[2]*V2[2];
}


bool SurfaceNormals(NumericVector& Points, NumericVector& BariCenter, NumericVector& CircumCenter)
{
	NumericVector AB;
	NumericVector AC;
	//  NumericVector AD;
	NumericVector BC;
	NumericVector BD;
	NumericVector CD;

	AB.push_back(Points[0] - Points[3]);
	AB.push_back(Points[1] - Points[4]);
	AB.push_back(Points[2] - Points[5]);

	AC.push_back(Points[0] - Points[6]);
	AC.push_back(Points[1] - Points[7]);
	AC.push_back(Points[2] - Points[8]);

	//  AD.push_back(Points[0] - Points[9]);
	//  AD.push_back(Points[1] - Points[10]);
	//  AD.push_back(Points[2] - Points[11]);

	BC.push_back(Points[3] - Points[6]);
	BC.push_back(Points[4] - Points[7]);
	BC.push_back(Points[5] - Points[8]);

	BD.push_back(Points[3] - Points[9]);
	BD.push_back(Points[4] - Points[10]);
	BD.push_back(Points[5] - Points[11]);

	CD.push_back(Points[6] - Points[9]);
	CD.push_back(Points[7] - Points[10]);
	CD.push_back(Points[8] - Points[11]);

	NumericVector nABC;
	NumericVector nABD;
	NumericVector nACD;
	NumericVector nBCD;

	nABC = CrossProduct(AB, AC);
	nABD = CrossProduct(AB, BD);
	nACD = CrossProduct(AC, CD);
	nBCD = CrossProduct(BC, CD);

	NumericVector A2BC;
	NumericVector B2BC;
	NumericVector A2CC;
	NumericVector B2CC;

	A2BC.push_back(Points[0] - BariCenter[0]);
	A2BC.push_back(Points[1] - BariCenter[1]);
	A2BC.push_back(Points[2] - BariCenter[2]);

	B2BC.push_back(Points[3] - BariCenter[0]);
	B2BC.push_back(Points[4] - BariCenter[1]);
	B2BC.push_back(Points[5] - BariCenter[2]);

	A2CC.push_back(Points[0] - CircumCenter[0]);
	A2CC.push_back(Points[1] - CircumCenter[1]);
	A2CC.push_back(Points[2] - CircumCenter[2]);

	B2CC.push_back(Points[3] - CircumCenter[0]);
	B2CC.push_back(Points[4] - CircumCenter[1]);
	B2CC.push_back(Points[5] - CircumCenter[2]);

	numeric_t check_ABC = ScalarProduct(A2BC, nABC) * ScalarProduct(A2CC, nABC);
	numeric_t check_ABD = ScalarProduct(A2BC, nABD) * ScalarProduct(A2CC, nABD);
	numeric_t check_ACD = ScalarProduct(A2BC, nACD) * ScalarProduct(A2CC, nACD);
	numeric_t check_BCD = ScalarProduct(B2BC, nBCD) * ScalarProduct(B2CC, nBCD);

//  	std::cout << ScalarProduct(A2BC, nABC) << "  " << ScalarProduct(A2CC, nABC) << "  " << check_ABC <<std::endl;
//  	std::cout << ScalarProduct(A2BC, nABD) << "  " << ScalarProduct(A2CC, nABD) << "  " << check_ABD <<std::endl;
//  	std::cout << ScalarProduct(A2BC, nACD) << "  " << ScalarProduct(A2CC, nACD) << "  " << check_ACD <<std::endl;
//  	std::cout << ScalarProduct(B2BC, nBCD) << "  " << ScalarProduct(B2CC, nBCD) << "  " << check_BCD <<std::endl;

	bool status;
      	if (check_ABC < numeric_t(0)) status=false;
	if (check_ABD < numeric_t(0)) status=false;
	if (check_ACD < numeric_t(0)) status=false;
	if (check_BCD < numeric_t(0)) status=false;

	if (!status)
	{
// 		std::ostream&   outputstream(std::cout);
// 		outputstream << "######### Not Delaunay Conform: ####### " << std::endl;
// 		outputstream << "output vector 1" << std::endl;
// 		outputRatBigInt(outputstream, ScalarProduct(A2BC, nABC));
// 		outputRatBigInt(outputstream, ScalarProduct(A2CC, nABC));
// 		outputRatBigInt(outputstream, check_ABC);
// 		outputstream << std::endl;
// 		outputstream << "..output vector 2" << std::endl;
// 		outputRatBigInt(outputstream, ScalarProduct(A2BC, nABD));
// 		outputRatBigInt(outputstream, ScalarProduct(A2CC, nABD));
// 		outputRatBigInt(outputstream, check_ABD);
// 		outputstream << std::endl;
// 		outputstream << "..output vector 3" << std::endl;
// 		outputRatBigInt(outputstream, ScalarProduct(A2BC, nACD));
// 		outputRatBigInt(outputstream, ScalarProduct(A2CC, nACD));
// 		outputRatBigInt(outputstream, check_ACD);
// 		outputstream << std::endl;
// 		outputstream << "..output vector 4" << std::endl;
// 		outputRatBigInt(outputstream, ScalarProduct(B2BC, nBCD));
// 		outputRatBigInt(outputstream, ScalarProduct(B2CC, nBCD));
// 		outputRatBigInt(outputstream, check_BCD);
// 		outputstream << std::endl;
// 		outputstream << "######### Not Delaunay Conform: ####### " << std::endl;
//  	std::cout << ScalarProduct(A2BC, nABC) << "  " << ScalarProduct(A2CC, nABC) << "  " << check_ABC <<std::endl;
//  	std::cout << ScalarProduct(A2BC, nABD) << "  " << ScalarProduct(A2CC, nABD) << "  " << check_ABD <<std::endl;
//  	std::cout << ScalarProduct(A2BC, nACD) << "  " << ScalarProduct(A2CC, nACD) << "  " << check_ACD <<std::endl;
//  	std::cout << ScalarProduct(B2BC, nBCD) << "  " << ScalarProduct(B2CC, nBCD) << "  " << check_BCD <<std::endl;
	}

	return status;

}


// 
// returns: true for delaunay conformity
// returns: false for non-delaunay conformity
//
// template<typename InputIterator>
// bool testDelaunayForTetrahedra(InputIterator first, InputIterator last)
// {
// 	NumericVector Vec;
	
// 	typedef typename std::iterator_traits<InputIterator>::value_type numeric_type;

// 	while (first != last)
// 	{
// 		numeric_t temp;
// 		temp = castF2BI<rational_t,numeric_type>((*first),100);
// 		Vec.push_back(temp); 
// 		++first;
// 	}

// 	//NumericVector CC = Tetra_CircumCenter(Vec);
// 	//NumericVector BC = Tetra_BariCenter(Vec);

// 	return (SurfaceNormals(Vec, BC, CC));
// }




// 
// returns: true for delaunay conformity
// returns: false for non-delaunay conformity
//
// template<typename GenericPointT>
// bool delaunay_test_tetrahedron(const GenericPointT& p1, 
// 			       const GenericPointT& p2, 
// 			       const GenericPointT& p3, 
// 			       const GenericPointT& p4 )
// {
// 	NumericVector Vec;
	
// 	numeric_t temp;

// 	temp = castF2BI<rational_t, typename GenericPointT::numeric_t>(p1[0],100);
// 	Vec.push_back(temp); 
// 	temp = castF2BI<rational_t, typename GenericPointT::numeric_t>(p1[1],100);
// 	Vec.push_back(temp); 
// 	temp = castF2BI<rational_t, typename GenericPointT::numeric_t>(p1[2],100);
// 	Vec.push_back(temp); 

// 	temp = castF2BI<rational_t, typename GenericPointT::numeric_t>(p2[0],100);
// 	Vec.push_back(temp); 
// 	temp = castF2BI<rational_t, typename GenericPointT::numeric_t>(p2[1],100);
// 	Vec.push_back(temp); 
// 	temp = castF2BI<rational_t, typename GenericPointT::numeric_t>(p2[2],100);
// 	Vec.push_back(temp); 

// 	temp = castF2BI<rational_t, typename GenericPointT::numeric_t>(p3[0],100);
// 	Vec.push_back(temp); 
// 	temp = castF2BI<rational_t, typename GenericPointT::numeric_t>(p3[1],100);
// 	Vec.push_back(temp); 
// 	temp = castF2BI<rational_t, typename GenericPointT::numeric_t>(p3[2],100);
// 	Vec.push_back(temp); 

// 	temp = castF2BI<rational_t, typename GenericPointT::numeric_t>(p4[0],100);
// 	Vec.push_back(temp); 
// 	temp = castF2BI<rational_t, typename GenericPointT::numeric_t>(p4[1],100);
// 	Vec.push_back(temp); 
// 	temp = castF2BI<rational_t, typename GenericPointT::numeric_t>(p4[2],100);
// 	Vec.push_back(temp); 


// // 	NumericVector CC = Tetra_CircumCenter(Vec);
// // 	NumericVector BC = Tetra_BariCenter(Vec);

// 	return (SurfaceNormals(Vec, BC, CC));
// }



} // namespace gsse

#endif
