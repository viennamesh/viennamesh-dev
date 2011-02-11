/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at    

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_INTERFACE_GIL
#define GSSE_INTERFACE_GIL
//
// ===================================================================================
//
// *** BOOST includes
//
//#include <boost/mpl/vector.hpp>
//#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>
//

// *** [TODO] .. check, if these includes are available
//
// #include "../util.hpp" 
// #include "gsse.hpp"


//
// *** GSSE includes
//
#include <gsse/adapted/gil.hpp>
#include <gsse/algorithm/mesh/create.hpp>
//
// ===================================================================================
//

namespace gsse { 
namespace adapted {
namespace gil {
//
// ===================================================================================
//
template < int Selection >
struct GilImageType
{};

template < >
struct GilImageType < 0 >
{
   typedef boost::gil::gray8_image_t      type;
   typedef boost::gil::gray8_pixel_t      pixel_type;   
   static const long color_size = 256;
};
template < >
struct GilImageType < 1 >
{
   typedef boost::gil::gray16_image_t     type;
   typedef boost::gil::gray16_pixel_t     pixel_type;      
   static const long color_size = 65536;
};
template < >
struct GilImageType < 2 >
{
   typedef boost::gil::rgb8_image_t       type;
   typedef boost::gil::rgb8_pixel_t       pixel_type;
   static const long color_size = 256;   
};
template < >
struct GilImageType < 3 >
{
   typedef boost::gil::rgb16_image_t      type;
   typedef boost::gil::rgb16_pixel_t      pixel_type;   
   static const long color_size = 65536;   
};
template < >
struct GilImageType < 4 >
{
   typedef boost::gil::rgba8_image_t      type;
   typedef boost::gil::rgba8_pixel_t      pixel_type;   
   static const long color_size = 256;   
};
template < >
struct GilImageType < 5 >
{
   typedef boost::gil::rgba16_image_t     type;
   typedef boost::gil::rgba16_pixel_t     pixel_type;      
   static const long color_size = 65536;   
};
//
// ===================================================================================
//
template < typename Domain, typename ImageType >
void
load_gil_domain ( Domain&     domain, 
                  ImageType&  img, 
                  std::string const& filename,
                  std::string const& quan_name)
{

   std::cout << "load gil domain .." << std::endl;
   // ---------------------------------------------------------------------
   /*
   typedef boost::mpl::vector
                      < boost::gil::gray8_image_t, 
                        boost::gil::gray16_image_t, 
                        boost::gil::rgb8_image_t, 
                        boost::gil::rgb16_image_t,
                        boost::gil::rgba8_image_t,
                        boost::gil::rgba16_image_t >     image_types;
   //
   // runtime image .. needs further investigation ...
   //
   //  source: 
   //  http://stlab.adobe.com/gil/html/giltutorial.html
   //
   boost::gil::any_image<image_types>                    img;
   */

   boost::gil::png_read_image(filename,img);   
     
   ////boost::gil::image_read_info(img);
   
   // ---------------------------------------------------------------------
   //
   // reading the quantity data
   // [JW] credits go to [RH]
   //

   using gsse::adapted::gil::PixelT;
   std::vector<std::vector<PixelT> > container(img.height(), std::vector<PixelT>(img.width()));
   //
   // in the future we may want to set the pixel type acoordingly ...
   //
   gsse::adapted::gil::image_traversal
   (
      boost::gil::color_converted_view<boost::gil::rgba8_pixel_t>
      (
         boost::gil::view(img)
      ), 
      container
   );
   //gsse::adapted::gil::convert_img(boost::gil::view(img), container);
   // ---------------------------------------------------------------------
   long size_row = container.size();
   long size_col = container[0].size();
   
   double row_col_ratio = 0.0;
   if( size_col > 0.0 )
      row_col_ratio = size_row / (size_col*1.0); // just to be sure that its a float division

   // 
   // we want to scale the image:
   //   by choosing size_x = 2.0 - the length of the mesh will be 2.0
   //   by choosing size_y = size_x * row_col_ratio we scale the height of the mesh
   //      according to the original image ratio ..
   // as a result: each image pixel is represented by two triangles which together form a 
   // square. 
   //   
   
   double size_x = 2.;
   double size_y = size_x * row_col_ratio;
   // ---------------------------------------------------------------------
   //
   // we can directly fill the gml::domain too 
   // the segment size increments automatically ..
      // [JW] credits go to [RH]   
   //

   typedef gsse::property_domain<Domain>                                                     DomainProperty;   
   typedef typename gsse::result_of::property_SpaceTopologySegmentsT<DomainProperty>::type   SpaceTopologySegments;
   typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<DomainProperty>::type   SpaceQuantitySegments;   
   typedef typename gsse::result_of::property_FBPosT<DomainProperty>::type                   FBPos;         

   SpaceTopologySegments&  segments_topology = gsse::at_dim< gsse::access_specifier::AC >(domain); 
   SpaceQuantitySegments&  segments_quantity = gsse::at_dim< gsse::access_specifier::AQ >(domain); 
   FBPos&                  geometry          = gsse::at_dim< gsse::access_specifier::AP >(domain);       

   gsse::mesh::fill_container_from_implicit(  
      gsse::at_dim<gsse::access_specifier::AT_cl>(gsse::at_fiber(gsse::at(0)(segments_topology))),
      size_row, size_col );
   //gsse::mesh::fill_container_from_implicit      ( gml::cell(gml_domain,0), size_row, size_col);

   gsse::mesh::fill_container_from_implicit_coord( geometry,  size_row, size_col,   size_x, size_y);
   //gsse::mesh::fill_container_from_implicit_coord( gml::coo( gml_domain ),  size_row, size_col,   size_x, size_y);

   finalize_topology(domain);

   gsse::resize(1)(segments_topology);      
   gsse::resize(1)(segments_quantity);      
   
   // ---------------------------------------------------------------------
   //gml_domain.finalize(); // to get the full cell complex as well as the index maps ..
   // ---------------------------------------------------------------------
   // load the vertex quantities
   //
   long real_vi = 0;
   long col_i   = 0;
   long row_i   = 0;

   for( long vi = 0; vi < gsse::size( gsse::at_dim<gsse::access_specifier::AT_vx>(gsse::at_fiber(gsse::at(0)(segments_topology))) ); vi++ )
   {
      //
      // computing the access indices for the quantity container 
      // [JW] credits go to [RH]
      //
      real_vi = gsse::at_index( gsse::at(vi)(
         gsse::at_dim<gsse::access_specifier::AT_vx>(gsse::at_fiber(gsse::at(0)(segments_topology)))  
      ) );
      col_i   = real_vi % size_col;
      row_i   = (real_vi - col_i) / size_col;
      //
      // storing all channels on a single vector quantity
      //
      for( int ch_i = 0; ch_i < gsse::size(container[row_i][col_i]); ch_i++ )
      {
         gsse::at(ch_i)(gsse::at(vi)( 
            gsse::at(quan_name)(gsse::at_dim<gsse::access_specifier::AT_vx>(gsse::at_fiber(gsse::at(0)(segments_quantity))))
         )) = container[row_i][col_i][ch_i];         
      }
   }

   // ---------------------------------------------------------------------
}


} // namespace gil
} // namespace adapted
} // namespace gsse

#endif



















