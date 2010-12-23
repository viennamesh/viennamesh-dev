/* ============================================================================
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2004-2009 Philipp Schwaha                      philipp@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_SPACE_ORIENT_HH_ID
#define GSSE_SPACE_ORIENT_HH_ID 

// *** system includes
#include<iostream>
// *** GSSE includes
#include"gsse/geometry/metric_operations.hpp"


namespace gsse {


#define SWK_LEFT        -1
#define SWK_ON           0
#define SWK_RIGHT        1

#define RH_IN            1           
#define RH_ON            0
#define RH_OUT          -1


// ###  Jonathan Richard Shewchuk                                               
//   http://www.cs.cmu.edu/~quake/robust.html .           
//
// AND
//
// provides Shewchuk's oriented 3d algorithm ...
// WaferState Server version from matrix.hh
//
template<typename GenericPointT, typename NumericT=double>
class SwkMatrix3x3
{       
  NumericT  a [3][3];
  
  double  det;
  NumericT  calc_det () const
  {
    return ( a [0][0] * a [1][1] * a [2][2] +
	     a [0][1] * a [1][2] * a [2][0] +
	     a [0][2] * a [1][0] * a [2][1] -
	     a [2][0] * a [1][1] * a [0][2] -
	     a [2][1] * a [1][2] * a [0][0] -
	     a [2][2] * a [1][0] * a [0][1]   );
  }  
  
  void setMatrix(const GenericPointT& p1, const GenericPointT& p2, 
		 const GenericPointT& p3, const GenericPointT& pt)
  {
    //std::cout << "p1: " << p1 << std::endl;
    //std::cout << "p2: " << p2 << std::endl;
    //std::cout << "p3: " << p3 << std::endl;

    //std::cout << "ptn: " << pt << std::endl;


    a [0][0] = p1[0] - pt[0];
    a [0][1] = p1[1] - pt[1];
    a [0][2] = p1[2] - pt[2];
    
    a [1][0] = p2[0] - pt[0];
    a [1][1] = p2[1] - pt[1];
    a [1][2] = p2[2] - pt[2];
    
    a [2][0] = p3[0] - pt[0];
    a [2][1] = p3[1] - pt[1];
    a [2][2] = p3[2] - pt[2];

    //std::cout << a[0][0] << "  " << a[0][1] << "  " << a[0][2] << std::endl;
    //std::cout << a[1][0] << "  " << a[1][1] << "  " << a[1][2] << std::endl;
    //std::cout << a[2][0] << "  " << a[2][1] << "  " << a[2][2] << std::endl;
    
    det = calc_det ();
    //std::cout << "det = " << det << std::endl;
  }

 public:
  
  SwkMatrix3x3 (const GenericPointT& p1, const GenericPointT& p2, const GenericPointT& p3,
		const GenericPointT& pt)
  {
    /*
    // sort the points first
    //
    GenericPointT pos[3];
    pos[0] = p1;
    if (p2 < pos[0])
      {
 	pos[1] = pos[0];
 	pos[0] = p2;
      }
    else
      {
 	pos[1] = p2;
      }
    if (p3 < pos[0])
      {
 	pos[2] = pos[1];
 	pos[1] = pos[0];
 	pos[0] = p3;
      }
    else if (p3 < pos[1])
      {
 	pos[2] = pos[1];
  	pos[1] = p3;
      }
    else
      {
 	pos[2] = p3;
      }
    
    setMatrix(pos[0], pos[1], pos[2], pt);
    */

    setMatrix(p1, p2, p3, pt);
  }
  
    


  double orient3D () const
  {
    //    std::cout << "det: " << det << std::endl;
    return det;
  }

  short orient3D_short () const
  {
    if (det < 0) return (SWK_LEFT);
    if (det > 0) return (SWK_RIGHT);
    return SWK_ON;
  }

};

// uses the orientation test to test, if 
//   a point is within a tetrahedron
//
template<typename GenericPointT>
short swk_orient3D  (const GenericPointT& p1, 
		     const GenericPointT& p2, 
		     const GenericPointT& p3, 
		     const GenericPointT& p4, 
		     const GenericPointT& ptn ) 
{
  SwkMatrix3x3<GenericPointT, double>  swk_matrix1 (p1, p4, p2, ptn);
  SwkMatrix3x3<GenericPointT, double>  swk_matrix2 (p2, p4, p3, ptn);
  SwkMatrix3x3<GenericPointT, double>  swk_matrix3 (p3, p4, p1, ptn); // [MaS] switched p1,p3 and p4
  SwkMatrix3x3<GenericPointT, double>  swk_matrix4 (p1, p2, p3, ptn);

  short orient_val_left=0;
  short orient_val_right=0;

  if (swk_matrix1.orient3D() < 0)
    orient_val_left +=1;
  if (swk_matrix1.orient3D() > 0)
    orient_val_right +=1;

  if (swk_matrix2.orient3D() < 0)
    orient_val_left +=1;
  if (swk_matrix2.orient3D() > 0)
    orient_val_right +=1;


  if (swk_matrix3.orient3D() < 0)
    orient_val_left +=1;
  if (swk_matrix3.orient3D() > 0)
    orient_val_right +=1;


  if (swk_matrix4.orient3D() < 0)
    orient_val_left +=1;
  if (swk_matrix4.orient3D() > 0)
    orient_val_right +=1;


  //std::cout << "left: " << orient_val_left  << std::endl;
  //std::cout << "right: " << orient_val_right << std::endl;
   

  if ((orient_val_left == 4) || (orient_val_right == 4))   //[MaS]  clearly inside, added || (orient_val_right == 4)
    return  RH_IN;
  
  if ((orient_val_left + orient_val_right) != 4)   // boundary
    return RH_ON;

  else if (orient_val_right != 4 || orient_val_left != 4   )    // outside
    return RH_OUT;
    
    
  return -1;

} 



// uses the orientation test to test, if 
//   a point is within a triangle :: 3D
//
template<typename GenericPointT>
short orient3d_triangle  (const GenericPointT& p1, 
			  const GenericPointT& p2, 
			  const GenericPointT& p3, 
			  const GenericPointT& ptn ) 
{

  // possible bounding box check
  // [RH] TODO
  //

  typedef GenericPointT Vector_t;

  Vector_t p12(p2 - p1);
  Vector_t p13(p3 - p1);
  Vector_t useNormal = gsse::ex(p12, p13);
  useNormal = gsse::normalize(useNormal);
        
  short ori1, ori2;

  // Test line p1 - p2
  GenericPointT temppoint (p1[0] + useNormal[0],
			   p1[1] + useNormal[1],
			   p1[2] + useNormal[2]);
  SwkMatrix3x3<GenericPointT, double>  swk_matrix1 (p1, p2,  temppoint, ptn);
  SwkMatrix3x3<GenericPointT, double>  swk_matrix2 (p1, p2,  temppoint, p3);
  ori1 = swk_matrix1.orient3D_short();
  ori2 = swk_matrix2.orient3D_short();
  //std::cout << "p12: ori1: " << ori1 << std::endl;
  //std::cout << "p12: ori2: " << ori2 << std::endl;
  if (ori1 == RH_ON || ori2 == RH_ON)
    return RH_ON;
  if (ori1 != ori2)
    return (RH_OUT);

  // Test line p1 - p3
  SwkMatrix3x3<GenericPointT, double>  swk_matrix3 (p1, p3,  temppoint, ptn);
  SwkMatrix3x3<GenericPointT, double>  swk_matrix4 (p1, p3,  temppoint, p2);
  ori1 = swk_matrix3.orient3D_short();
  ori2 = swk_matrix4.orient3D_short();
  //std::cout << "p13: ori1: " << ori1 << std::endl;
  //std::cout << "p13: ori2: " << ori2 << std::endl;
  if (ori1 == RH_ON || ori2 == RH_ON)
    return RH_ON;
  if (ori1 != ori2)
    return (RH_OUT);



  // Test line p2 - p3
  GenericPointT temppoint2 (p2[0] + useNormal[0],
			    p2[1] + useNormal[1],
			    p2[2] + useNormal[2]);
  SwkMatrix3x3<GenericPointT, double>  swk_matrix5 (p2, p3,  temppoint2, ptn);
  SwkMatrix3x3<GenericPointT, double>  swk_matrix6 (p2, p3,  temppoint2, p1);
  ori1 = swk_matrix5.orient3D_short();
  ori2 = swk_matrix6.orient3D_short();
  //std::cout << "p23: ori1: " << ori1 << std::endl;
  //std::cout << "p23: ori2: " << ori2 << std::endl;
  if (ori1 == RH_ON || ori2 == RH_ON)
    return RH_ON;
  if (ori1 != ori2)
    return (RH_OUT);

  
  return (RH_IN);
	
}



// ##################################################################

// uses the orientation test to test, if a 
//   a point is within a triangle

// Return a positive value if the points pa, pb, and pc occur  
//    in counterclockwise order; a negative value if they occur 
//    in clockwise order; and zero if they are collinear.  The  
//    result is also a rough approximation of twice the signed  
//    area of the triangle defined by the three points.         

template<typename GenericPointT, typename GenericPointT2>
double swk_orient2D(const GenericPointT& pa,
		    const GenericPointT& pb,
		    const GenericPointT2& pc )
{
  double acx, bcx, acy, bcy;

  acx = pa[0] - pc[0];
  bcx = pb[0] - pc[0];
  acy = pa[1] - pc[1];
  bcy = pb[1] - pc[1];
  return acx * bcy - acy * bcx;
}




// return value: 0  -> inside
// return value: 1 -> on boundary
// return value: 2  -> outside

template<typename GenericPointT, typename GenericPointT2>
short orient2D_triangle(const GenericPointT& p1,
			const GenericPointT& p2,
			const GenericPointT& p3,
			const GenericPointT2& ptn   )
{
  short orient_val_left=0;
  short orient_val_right=0;

    if (swk_orient2D(p1,p2, ptn) > 0)
      orient_val_left +=1;
    if (swk_orient2D(p1,p2, ptn) < 0)
      orient_val_right +=1;


    if (swk_orient2D(p2,p3, ptn) > 0)
      orient_val_left +=1;
    if (swk_orient2D(p2,p3, ptn) < 0)
      orient_val_right +=1;


    if (swk_orient2D(p3,p1, ptn) > 0)
      orient_val_left +=1;
    if (swk_orient2D(p3,p1, ptn) < 0)
      orient_val_right +=1;

//     std::cout << "left: " << orient_val_left  << std::endl;
//     std::cout << "right: " << orient_val_right << std::endl;

   

    if (orient_val_left == 3)   // clearly inside
      return  0;

    if ((orient_val_right == 0 && orient_val_left == 2)
	|| 
	(orient_val_right == 2 && orient_val_left == 0 )   )    // on border
      return 1;

    if ((orient_val_right == 0 && orient_val_left == 1)
	|| 
	( orient_val_right == 1 && orient_val_left == 0  )  )    // on a point
      return 3;

    
    else if ((orient_val_right == 2 && orient_val_left == 1)
	     || 
	     (orient_val_right == 1 && orient_val_left == 2  )  )    // outside
      return  2;
    

    if (orient_val_right == 3)   // ???? check this again.. points were not in counterclockwise order
      return  0;
    
    return -1;  // somewhere on a straight line
}

} // namespace gsse

#endif
