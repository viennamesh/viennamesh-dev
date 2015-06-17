/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_ADAPTED_GIL_INCLUDE_HH)
#define GSSE_ADAPTED_GIL_INCLUDE_HH

// *** BOOST includes
#include <boost/array.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/planar_pixel_reference.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/image_view_factory.hpp>
// #include <boost/gil/extension/io/tiff_io.hpp>
// #include <boost/gil/extension/io/jpeg_io.hpp>
// #include <boost/gil/extension/io/tiff_dynamic_io.hpp>
// #include <boost/gil/extension/io/jpeg_dynamic_io.hpp>
// #include <boost/gil/extension/io/png_dynamic_io.hpp>

// [JW] libpng 1.4 dropped definitions of png_infopp_NULL and int_p_NULL
// thefore we provide the following workaround
// note: need to be present before png_io.hpp is included!
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
// ---------------------------------------------
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/color_convert.hpp>


namespace gsse { 
namespace adapted {


namespace gil {

typedef boost::array<long, 4>  PixelT;


template <typename ImageView, typename Container>
void image_traversal(const ImageView& img, Container& container) 
{
   using namespace boost::gil;

   std::cout << "img height: " << img.height() << std::endl;
   std::cout << "img width : " << img.width() << std::endl;
   std::cout << " values:" << num_channels<ImageView>::value << std::endl;

   long cnt_row = 0;
   long cnt_col = 0;
   for (typename ImageView::iterator it=img.begin(); it!=img.end(); ++it )
   {
      for (int c=0; c < num_channels<ImageView>::value; ++c)
      {
         long val = (*it)[c];
         container[ cnt_row ][ cnt_col ][c] = val;

//         std::cout << "cntrow: " << cnt_row << "  cntcol: " << cnt_col << std::endl;
      }


      if ( cnt_col == (img.width()-1) )
      {
         ++cnt_row;
         cnt_col = 0;
      }
      else
         ++cnt_col;
      
   }
}

template <typename V, typename R>
void convert_img(V& img_view, R& container) 
{
   using namespace boost::gil;
   image_traversal(color_converted_view<rgba8_pixel_t>(img_view), container);
}

} // namespace gil
} // namespace adapted
} // namespace gsse

#endif


