/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_DATASTRUCTURES_UTILS_HH)
#define GSSE_DATASTRUCTURES_UTILS_HH

// *** system includes
// *** BOOST includes
#include <boost/fusion/container/vector/convert.hpp>
// *** GSSE includes
#include "gsse/util/at_complex.hpp"
#include "gsse/util/at_complex.hpp"
#include "gsse/util/common.hpp"
#include "gsse/util/tag_of.hpp"
#include "gsse/util/access.hpp"
#include "gsse/util/specifiers.hpp"   

// ############################################################
//
namespace gsse {   namespace datastructure {

template<typename AccessSpecifier>
struct size
{
   template<typename Container>
   long operator()(Container const& container)
   {
      return gsse::size(gsse::at_dim<AccessSpecifier>( gsse::at_fiber(container) ) );
   }
};


} // namespace datastructure

namespace
{
gsse::datastructure::size<gsse::access_specifier::AT_vx>  size_vx = gsse::datastructure::size<gsse::access_specifier::AT_vx>();
gsse::datastructure::size<gsse::access_specifier::AT_ee>  size_ee = gsse::datastructure::size<gsse::access_specifier::AT_ee>();
gsse::datastructure::size<gsse::access_specifier::AT_f0>  size_f0 = gsse::datastructure::size<gsse::access_specifier::AT_f0>();
gsse::datastructure::size<gsse::access_specifier::AT_cl>  size_cl = gsse::datastructure::size<gsse::access_specifier::AT_cl>();
}
// ===============================


// ################################################ property part #################################################

namespace result_of {

// [RH][TODO]
//  domain features should be removed.. 
//

template<typename GSSE_Domain>
struct property_SpaceTopologySegmentsT
{
   typedef typename GSSE_Domain::SpaceTopologySegmentsT type;
};
template<typename GSSE_Domain>
struct property_SpaceQuantitySegmentsT
{
   typedef typename GSSE_Domain::SpaceQuantitySegmentsT type;
};
template<typename GSSE_Domain>
struct property_SpaceMatrixQuantitySegmentsT
{
   typedef typename GSSE_Domain::SpaceMatrixQuantitySegmentsT type;
};
template<typename GSSE_Domain>
struct property_FBPosT
{
   typedef typename GSSE_Domain::FBPosT type;
};
template<typename GSSE_Domain>
struct property_CoordPosT
{
   typedef typename GSSE_Domain::CoordPosT type;
};
template<typename GSSE_Domain>
struct property_QuanStorage_0form
{
   typedef typename GSSE_Domain::QuanStorage_0form type;
};


// template<typename GSSE_Domain>
// struct property_CellComplex
// {
//    typedef typename GSSE_Domain::CellComplex type;
// };
// template<typename GSSE_Domain>
// struct property_QuanComplex
// {
//    typedef typename GSSE_Domain::QuanComplex type;
// };
template<typename GSSE_Domain>
struct property_QuanCL
{
   typedef typename GSSE_Domain::QuanStorage_form_cl type;
};

// ########################
// #### [RH][info][new]
//
template<typename GSSE_Space>
struct property_DIMT
{
   static const long value = GSSE_Space::DIMT ;
};
template<typename GSSE_Space>
struct property_DIMG
{
   static const long value = GSSE_Space::DIMG;
};
template<typename GSSE_Space>
struct property_DIMQ
{
   static const long value = GSSE_Space::DIMQ;
};

template<typename GSSE_Space>
struct property_Space
{
   typedef typename GSSE_Space::SpaceV type;
};
template<typename GSSE_Space>
struct property_CellTopology
{
   typedef typename GSSE_Space::CellTopology type;
};
template<typename GSSE_Space>
struct property_SubSpaceCC
{
   typedef typename GSSE_Space::SubSpace_CellComplex type;
};
template<typename GSSE_Space>
struct property_SubSpaceQC
{
   typedef typename GSSE_Space::SubSpace_QuanComplex type;
};
template<typename GSSE_Space>
struct property_Geometry
{
   typedef typename GSSE_Space::FBPosT type;
};
template<typename GSSE_Space>
struct property_CoordT
{
   typedef typename GSSE_Space::CoordT type;
};
template<typename GSSE_Space>
struct property_CellT
{
   typedef typename GSSE_Space::CellT type;

};

//[FS] added - this type is needed
template<typename GSSE_Space>
struct property_EdgeT
{
   typedef typename GSSE_Space::EdgeT type;

};

template<typename GSSE_Space>
struct property_QuanT
{
   typedef typename GSSE_Space::QuanT type;
};
template<typename GSSE_Space>
struct property_CellComplex
{
   typedef typename GSSE_Space::CellComplex type;
};
template<typename GSSE_Space>
struct property_QuanComplex
{
   typedef typename GSSE_Space::QuanComplex type;
};
   
}// namespace result_of






template<typename Domain>
struct property_domain
{
   typedef typename gsse::result_of::at_dim<Domain, gsse::access_specifier::AC>::type  SpaceTopologySegmentsT;
   typedef typename gsse::result_of::at_dim<Domain, gsse::access_specifier::AQ>::type  SpaceQuantitySegmentsT;
  //   typedef typename gsse::result_of::at_dim<Domain, gsse::access_specifier::AMQ>::type SpaceMatrixQuantitySegmentsT;
   typedef typename gsse::result_of::at_dim<Domain, gsse::access_specifier::AP>::type FBPosT;

   typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type              CellComplex;  
   typedef typename CellComplex::CellT                                                       CellTopology;


   typedef typename gsse::result_of::at_dim<CellComplex, gsse::access_specifier::AT_vx>::type  ContainerCLonVX;
   typedef typename gsse::result_of::at_dim<CellComplex, gsse::access_specifier::AT_ee>::type  ContainerEE;
   typedef typename gsse::result_of::at_dim<CellComplex, gsse::access_specifier::AT_cl>::type  ContainerVXonCL;
   typedef typename gsse::result_of::val<ContainerVXonCL>::type        CellT;
   typedef typename gsse::result_of::val<ContainerEE>::type            EdgeT;

   typedef typename gsse::result_of::at_dim_index<SpaceQuantitySegmentsT>::type                QuanComplex;
   typedef typename gsse::result_of::at_dim<QuanComplex, gsse::access_specifier::AT_vx>::type  Quan_0form;
   typedef typename gsse::result_of::at_isd<Quan_0form, 1>::type                               QuanStorage_0form;
   typedef typename gsse::result_of::at_dim<QuanComplex, gsse::access_specifier::AT_cl>::type  Quan_clform;
   typedef typename gsse::result_of::at_isd<Quan_clform, 1>::type                              QuanStorage_form_cl;

   typedef typename gsse::result_of::at_dim_isd  <FBPosT>::type                  CoordPosT;


   static const long DIMTopology = CellComplex::DIM_cc;

   // [JW] computing the ct-size via the gsse::result_of mechanism, 
   // as it is capable of handling different ct-containers ...
   // location: gsse/utils/size.hpp
   //
   //static const long DIMGeometry = FBPosT::value_type::Size;
   static const long DIMGeometry = gsse::result_of::size<typename FBPosT::value_type>::value;


   // ==   shortcuts
   static const long DIMT = DIMTopology;
   static const long DIMG = DIMGeometry;
   static const long DIMQ = DIMT;   //[RH][TODO]
};


template<typename Space>
struct property_space
{
   typedef typename gsse::result_of::at_dim<Space, gsse::access_specifier::AP>::type  FBPosT;
   typedef typename gsse::result_of::at_dim<Space, gsse::access_specifier::AC>::type  SubSpace_CellComplex;
   typedef typename gsse::result_of::at_dim<Space, gsse::access_specifier::AQ>::type  SubSpace_QuanComplex;
   
   typedef typename gsse::result_of::at_fiber_val<typename gsse::result_of::val<SubSpace_CellComplex>::type>::type CellComplex;
   typedef typename gsse::result_of::at_fiber_val<typename gsse::result_of::val<SubSpace_QuanComplex>::type>::type QuanComplex;

   typedef typename CellComplex::CellTopology                               CellTopology;


   typedef typename boost::fusion::result_of::as_vector<CellComplex>::type  CellComplexV;
   typedef typename boost::fusion::result_of::as_vector<Space>::type        SpaceV;

   typedef typename gsse::result_of::val<FBPosT>::type           CoordT;
   typedef typename gsse::result_of::at_dim<CellComplex, gsse::AT_cl>::type ContainerVXonCL;
   typedef typename gsse::result_of::val<ContainerVXonCL>::type  CellT;
   // [RH][TODO] 
   //   use a more generic meta-protocol to extract the quantity datatypes
   typedef typename gsse::result_of::at_dim<QuanComplex, gsse::AT_cl>::type::mapped_type::mapped_type QuanT;


   static const long DIMG = FBPosT::value_type::Size;
   static const long DIMT = CellComplex::DIM;
   static const long DIMQ = CellComplex::DIM;


   // type -> segment
   // [temporary]
//   typedef SubSpace_CellComplex type;
};

// ##########

template<typename Complex>
struct property_complex
{
   typedef typename Complex::CellT                                  CellTopology;
   typedef typename gsse::result_of::at_dim<Complex, gsse::access_specifier::AT_vx>::type  ContainerCLonVX;
//   typedef typename gsse::result_of::at_dim<Complex, gsse::access_specifier::AT_ee>::type  ContainerEE;
   typedef typename gsse::result_of::at_dim<Complex, gsse::access_specifier::AT_cl>::type  ContainerVXonCL;
   typedef typename gsse::result_of::val<ContainerVXonCL>::type        CellT;
//   typedef typename gsse::result_of::val<ContainerEE>::type            EdgeT;

   static const long DIMT = Complex::DIM_cc;
   typedef CellT     cell_type;
};

// [RH][new]
//
template<typename CellComplex>
struct property_cellcomplex
{
   typedef typename CellComplex::CellT                                  CellTopology;
   typedef typename gsse::result_of::at_dim<CellComplex, gsse::access_specifier::AT_vx>::type  ContainerCLonVX;
   typedef typename gsse::result_of::at_dim<CellComplex, gsse::access_specifier::AT_cl>::type  ContainerVXonCL;
   typedef typename gsse::result_of::val<ContainerVXonCL>::type        CellT;

   static const long DIMT = CellComplex::DIM_cc;
   typedef CellT     cell_type;
};
template<typename QuanComplex>
struct property_quancomplex
{
   typedef typename QuanComplex::CellT                                  CellTopology;
   typedef typename gsse::result_of::at_dim<QuanComplex, gsse::access_specifier::AT_cl>::type  ContainerCL;
   typedef typename ContainerCL::mapped_type    CLContainer;
   typedef typename CLContainer::key_type       CellT;
   typedef typename CLContainer::mapped_type    CLContainerStorageType;

   static const long DIMT = QuanComplex::DIM_cc;
   typedef CellT     cell_type;
};



template<long DIM, typename CellTopologyT>
struct property_data
{
   static const long     DIMT = DIM;
   typedef CellTopologyT CellTopology;
};



} // namespace gsse


#endif



