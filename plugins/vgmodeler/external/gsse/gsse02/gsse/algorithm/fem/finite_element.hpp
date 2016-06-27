/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_FEM_FELEMENT_INCLUDE_HH)
#define GSSE_FEM_FELEMENT_INCLUDE_HH

// *** GSSE includes
#include "gsse/topology/elements.hpp"
#include "gsse/algorithm/fem/matrix.hpp"  


namespace gsse { namespace fem{

// ----
template<typename ElementType, long Order>
struct FiniteElement
{ };


template<>
struct FiniteElement<gsse::topology::segment, 1>
{
   typedef gsse::topology::segment ElementType;

   enum { NB_Points   = 2 }; 
   enum { NB_Edges    = 1 };
   enum { DIM    = 1 };
   enum { NDOF   = 2 };
   enum { ORDER  = 1 };

   template<typename PointT>
   gsse::matrix get_shape(const PointT& point) const
   {
      gsse::matrix mx_p(2,1);
      gsse::set_to_zero(mx_p);

      mx_p(0,0) =   point[0];
      mx_p(1,0) = 1-point[0];
      return mx_p;
   }
   
   template<typename PointT>
   gsse::matrix get_Dshape(const PointT& point) const
   {
      gsse::matrix mx_Ds(2,1);  // [TODO] .. use a 2x2 matrix ?? 
      gsse::set_to_zero(mx_Ds);

      mx_Ds(0,0) =  1.;
      mx_Ds(1,0) = -1.;

      return mx_Ds;
   }
};

// [FS] for order = 2
//
template<>
struct FiniteElement<gsse::topology::segment, 2>
{
   typedef gsse::topology::segment ElementType;

   enum { NB_Points   = 2 }; 
   enum { NB_Edges    = 1 };
   enum { DIM    = 2 };
   enum { NDOF   = 4 };
   enum { ORDER  = 2 };

   template<typename PointT>
   gsse::matrix get_shape(const PointT& point) const
   {
      gsse::matrix mx_p(2,1);
      gsse::set_to_zero(mx_p);

      mx_p(0,0) =   point[0];
      mx_p(1,0) = 1-point[0];
      return mx_p;
   }
   
   template<typename PointT>
   gsse::matrix get_Dshape(const PointT& point) const
   {
      gsse::matrix mx_Ds(2,1);  // [TODO] .. use a 2x2 matrix ?? 
      gsse::set_to_zero(mx_Ds);

      mx_Ds(0,0) =  1.;
      mx_Ds(1,0) = -1.;

      return mx_Ds;
   }
};



template<>
struct FiniteElement<gsse::topology::triangle, 1>
{
   typedef gsse::topology::triangle ElementType;

   enum { NB_Points   = 3 }; 
   enum { NB_Edges    = 3 };
   enum { DIM    = 2 };
   enum { NDOF   = 3 };
   enum { ORDER  = 1 };

   template<typename PointT>
   gsse::matrix get_shape(const PointT& point) const
   {
      gsse::matrix mx_p(3,1);
      gsse::set_to_zero(mx_p);

      mx_p(0,0) = point[0];
      mx_p(1,0) = point[1];
      mx_p(2,0) = 1 - point[0] - point[1];
      return mx_p;
   }
   
   template<typename PointT>
   gsse::matrix get_Dshape(const PointT& point) const
   {
      gsse::matrix mx_Ds(3,2);
      gsse::set_to_zero(mx_Ds);

      mx_Ds(0,0) = 1.;
      mx_Ds(0,1) = 0.;
      
      mx_Ds(1,0) = 0.;
      mx_Ds(1,1) = 1.;
      
      mx_Ds(2,0) = -1.;
      mx_Ds(2,1) = -1.;

      return mx_Ds;
   }
};


template<>
struct FiniteElement<gsse::topology::triangle, 2>
{
   typedef gsse::topology::triangle ElementType;

   enum { NB_Points   = 3 }; 
   enum { NB_Edges    = 3 };
   enum { DIM    = 2 };
   enum { NDOF   = 6 };
   enum { ORDER  = 2 };

   boost::array<int,12> order_edge;

   template<typename PointT>
   gsse::matrix get_shape(const PointT& point) const
   {
      gsse::matrix mx_p(NDOF,1);
      gsse::set_to_zero(mx_p);

      typename PointT::value_type cx   = point[0];
      typename PointT::value_type cy   = point[1];
      typename PointT::value_type lam3 = 1-cx-cy;

      mx_p(0,0) = cx * (2*cx-1);
      mx_p(0,1) = cy * (2*cy-1);
      mx_p(0,2) = lam3 * (2*lam3-1);
      mx_p(0,3) = 4 * cy * lam3;
      mx_p(0,4) = 4 * cx * lam3;
      mx_p(0,5) = 4 * cx * cy;

      return mx_p;
   }
   
   template<typename PointT>
   gsse::matrix get_Dshape(const PointT& point) const
   {
      gsse::matrix mx_Ds(NDOF,2);
      gsse::set_to_zero(mx_Ds);

      // [FS][TODO] implement this - check ngsolve !
      //
      // numerical integration
//      std::cout << "### H1 higher order trig:: not yet implemented.. " << std::endl;

//       mx_Ds(0,0) = point[0];
//       mx_Ds(1,0) = point[1];
//       mx_Ds(2,0) = 1 - point[0] - point[1];

      std::cout << "..mx_Ds: "<< mx_Ds << std::endl;

      for (int i = 0; i < 3; i++)
      {
         if (order_edge[i] >= 2)
         {             
//             int es = edges[i][0];
//             int ee = edges[i][1];

//             if (vnums[es] > vnums[ee])
//                swap (es, ee);
            
//             ii += T_ORTHOPOL::CalcTrigExt (order_edge[i], 
//                                            lami[ee]-lami[es], 1-lami[es]-lami[ee], 
//                                            sds.Addr(ii));
         }
      }

      return mx_Ds;
   }
};

template<>
struct FiniteElement<gsse::topology::tetrahedron,1>
{
   typedef gsse::topology::tetrahedron ElementType;

   enum { NB_Points   = 4 }; 
   enum { NB_Edges    = 6 };

   enum { DIM    = 3 };
   enum { NDOF   = 4 };
   enum { ORDER  = 1 };

   FiniteElement()
   {  }

   template<typename PointT>
   gsse::matrix get_shape(const PointT& point) const
   {
      gsse::matrix mx_p(4,1);
      gsse::set_to_zero(mx_p);

      mx_p(0,0) = point[0];
      mx_p(1,0) = point[1];
      mx_p(2,0) = point[2];
      mx_p(3,0) = 1 - point[0] - point[1] - point[2];
      return mx_p;
   }
   
   template<typename PointT>
   gsse::matrix get_Dshape(const PointT& point) const
   {
      gsse::matrix mx_Ds(4,3);
      gsse::set_to_zero(mx_Ds);


      mx_Ds(0,0) = 1.;
      mx_Ds(1,1) = 1.;
      mx_Ds(2,2) = 1.;

      mx_Ds(3,0) = -1.;
      mx_Ds(3,1) = -1.;
      mx_Ds(3,2) = -1.;

      return mx_Ds;
   }
};


// [FS] for order = 2
template<>
struct FiniteElement<gsse::topology::tetrahedron,2>
{
   typedef gsse::topology::tetrahedron ElementType;

   enum { NB_Points   = 4 }; 
   enum { NB_Edges    = 6 };

   enum { DIM    = 6 };
   enum { NDOF   = 8 };
   enum { ORDER  = 2 };

   FiniteElement()
   {  }

   template<typename PointT>
   gsse::matrix get_shape(const PointT& point) const
   {
      gsse::matrix mx_p(4,1);
      gsse::set_to_zero(mx_p);

      mx_p(0,0) = point[0];
      mx_p(1,0) = point[1];
      mx_p(2,0) = point[2];
      mx_p(3,0) = 1 - point[0] - point[1] - point[2];
      return mx_p;
   }
   
   template<typename PointT>
   gsse::matrix get_Dshape(const PointT& point) const
   {
      gsse::matrix mx_Ds(4,3);
      gsse::set_to_zero(mx_Ds);


      mx_Ds(0,0) = 1.;
      mx_Ds(1,1) = 1.;
      mx_Ds(2,2) = 1.;

      mx_Ds(3,0) = -1.;
      mx_Ds(3,1) = -1.;
      mx_Ds(3,2) = -1.;

      return mx_Ds;
   }
};




// ########################
template<long DIM>
struct select_element_by_dimension
{
   typedef void  type;
};

template<>
struct select_element_by_dimension<1>
{
   typedef gsse::topology::segment  type;
};

template<>
struct select_element_by_dimension<2>
{
   typedef gsse::topology::triangle  type;
};

template<>
struct select_element_by_dimension<3>
{
   typedef gsse::topology::tetrahedron  type;
};


} // namespace fem
} // namespace gsse

#endif


