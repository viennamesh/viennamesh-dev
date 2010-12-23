/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_ADAPTED_GSSEDOMAIN_RT_INCLUDE_HH)
#define GSSE_ADAPTED_GSSEDOMAIN_RT_INCLUDE_HH

// *** BOOST includes
#include "boost/fusion/container/vector.hpp"
// *** GSSE includes
#include "gsse/util/specifiers.hpp"
#include "gsse/datastructure/domain_algorithms.hpp"
#include "gsse/datastructure/utils.hpp"
#include "gsse/util_meta/dimension.hpp"
#include "gsse/adapted/gsse_opengl.hpp"    // introduces gsse_opengl compatible dataset/domain

#include "vol.hpp"   // external dependency by [FS]

// ############################################################
//
namespace gsse { 
namespace adapted {

// use the default gssev02 internal datatypes
//   if these are used (are used by default) then we can extract the values directly
//
typedef gsse::array<double, 3>  CoordT;
typedef gsse::array<long, 3>    TriangleT;

typedef std::vector<long>       ContainerIndexT;
typedef std::vector<double>     ContainerNumericT;

typedef boost::fusion::vector<long, std::string, std::string>  CallSig;
typedef gsse::create_domain<3,2,1, gsse::cell_simplex>::type gsse_domain_321; 




// ===================================================================
// ===================================================================



struct rt_hierarchy
{
  virtual void init() = 0;
  virtual void read(std::string file_name) = 0;
  virtual void read(std::istream & istr) = 0;
  virtual void write(std::string file_name) = 0;
  virtual void write(std::ostream & ostr = std::cout) = 0;

//    void virtual extract_ps(std::vector<CoordT>& container) = 0;
//    void virtual extract_cc(std::vector<TriangleT>& container) = 0;

   // ##
 
   virtual size_t size_sg() = 0;
   virtual size_t size_vx(long cnt_seg = 0) = 0;
   virtual size_t size_cl(long cnt_seg = 0) = 0;
   virtual size_t size_ps() = 0;

   virtual ContainerNumericT vx_position(size_t vx_id)   = 0;
   virtual ContainerIndexT   vx_on_cell (long cell_id, long cnt_seg = 0) = 0;

  virtual void set_vx_position(size_t vx_id, ContainerNumericT container) = 0;


   virtual void    set_quantity(long object_id, std::string quan_name, std::string object_type, double value, long cnt_seg = 0) = 0;
   virtual void    set_quantity2(long object_id, std::string quan_name, std::string object_type, std::vector<double> value, long cnt_seg = 0) = 0;
   virtual double& set_quantity(long object_id, std::string quan_name, std::string object_type, long cnt_seg = 0) = 0;
   virtual void    get_quantity(long object_id, std::string quan_name, std::string object_type, double& value, long cnt_seg = 0) = 0;
   virtual double  get_quantity(long object_id, std::string quan_name, std::string object_type, long cnt_seg = 0) = 0;
   virtual void    get_quantity2(long object_id, std::string quan_name, std::string object_type, std::vector<double>& value, long cnt_seg = 0) = 0;

   virtual void add_ps(ContainerNumericT container, long cnt_seg = 0) = 0;
//   virtual void add_vx(long vx_id, long cnt_seg = 0) = 0;
   virtual void add_cl(ContainerIndexT, long cnt_seg = 0) = 0;

   virtual bool             is_domain_321()  = 0;
   virtual gsse_domain_321  get_domain_321() = 0;


   virtual bool  complete_domain() = 0;

  // gsse_opengl  context

   virtual bool                            is_domain_opengl()  = 0;
   virtual gsse::adapted::opengl::DataSet  get_domain_opengl() = 0;

  // --------------------

   virtual long dim_g() = 0;
   virtual long dim_t() = 0;
   virtual long dim_q() = 0;

   virtual void read_vol(std::istream& istr) = 0;
   virtual void read_vol(std::string data) = 0;

   virtual ~rt_hierarchy() {}
};



template<typename DomainT>
struct rt : public rt_hierarchy
{
  typedef gsse::property_domain<DomainT> PropDomain;
  typedef DomainT type;

   virtual ~rt()
   {}

   void read(std::string file_name)
   {
      std::ifstream filein(file_name.c_str());
      gsse::read(domain_, filein);
   }	

  void read(std::istream &istr)
  {
    gsse::read(domain_, istr);
  }

   void init()
   {
      gsse::init(domain_);
   }

   void write(std::string file_name)
   {
      std::ofstream fileout(file_name.c_str());
      gsse::write(domain_, fileout);
   }

   void write(std::ostream & ostr = std::cout)
      {
         gsse::write(domain_, ostr);
      }

   long dim_g()
   {
     long value = 0;
     value = gsse::result_of::property_DIMG<PropDomain>::value;   
     return value;
   }
   long dim_t()
   {
     long value = 0;
     value = gsse::result_of::property_DIMT<PropDomain>::value;   
     return value;
   }
   long dim_q()
   {
     long value = 0;
     value = gsse::result_of::property_DIMQ<PropDomain>::value;   
     return value;
   }



 
   void read_vol(std::istream& istr)
      {
         std::cout << ".. in rt_domain .." << std::endl;
         converter::vol::convert_vol_2_gsse(istr, domain_);
      }

   void read_vol(std::string data)
      {
         std::cout << ".. in rt_domain .." << std::endl;
	 std::stringstream istr(data);
         converter::vol::convert_vol_2_gsse(istr, domain_);
	 
	 std::cout << "..segment size: " <<  gsse::size(gsse::at_dim<AC>(domain_)) << std::endl;
      }



//    void  extract_cc(std::vector<TriangleT>& container) 
//    {
//       // ##[info]## for now, for a 3T, 3G domain
//       //
//       typedef typename gsse::result_of::property_CellT<PropDomain>::type CellT;

      

//       long cnt_seg = 0;
//       std::cout << "#### " << std::endl;
//       std::cout << " size cl: " << gsse::size_cl( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_) )) << std::endl;

//       for (long cnt_cl = 0; cnt_cl < gsse::size_cl( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_) )); ++cnt_cl)
//       {
         
//          CellT cell = gsse::at_cl(cnt_cl)( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_) ));
// //         std::cout << " celli: " << cell << std::endl;

//          // boundary operation
//          //
//          typename gsse::result_of::facet_on_cell<PropDomain>::type    facet_on_cell;
//          typename gsse::result_of::facet_on_cell<PropDomain>::result  ft_on_cl = facet_on_cell(cell);

// //         dump_type<typename gsse::result_of::facet_on_cell<PropDomain>::result>();
// //         gsse::print_1d(ft_on_cl);

//          gsse::traverse()
//          [
//             boost::phoenix::push_back(boost::phoenix::ref(container),
//                                       boost::phoenix::arg_names::_1)
//          ](ft_on_cl);


// // old code style
// //
// //          TriangleT t1;  t1[0] = cell[0];   t1[1] = cell[1];     t1[2] = cell[2];
// //          TriangleT t2;  t2[0] = cell[1];   t2[1] = cell[2];     t2[2] = cell[3];
// //          TriangleT t3;  t3[0] = cell[2];   t3[1] = cell[3];     t3[2] = cell[0];
// //          TriangleT t4;  t4[0] = cell[3];   t4[1] = cell[0];     t4[2] = cell[1];

// //          std::cout << "   t1: " << t1 << std::endl;
// //          std::cout << "   t2: " << t2 << std::endl;
// //          std::cout << "   t3: " << t3 << std::endl;
// //          std::cout << "   t4: " << t4 << std::endl;

// //          gsse::pushback(container) = t1;
// //          gsse::pushback(container) = t2;
// //          gsse::pushback(container) = t3;
// //          gsse::pushback(container) = t4;
//       }
//    }


   size_t size_sg()
   {
      return gsse::size( gsse::at_dim<gsse::access_specifier::AC>(domain_)  );
   }

   
   size_t size_vx(long cnt_seg)
   {
#ifdef DEBUG
     std::cout << "..vertex size: " << gsse::size_vx( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_) )) << std::endl;
#endif
     return gsse::size_vx( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_) ));
   }
   
   // [TODO] create _ee, _ft sizes if available
   //  check, how to implemen the _available_

   size_t size_cl(long cnt_seg)
   {
      return gsse::size_cl( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_) ));
   }

   size_t size_ps()
   {
      return gsse::size( gsse::at_dim<gsse::access_specifier::AP>(domain_) );
   }

   ContainerNumericT vx_position(size_t vx_id)
   {

      typedef typename gsse::result_of::property_CoordPosT<PropDomain>::type CoordPosT;
#ifdef DEBUG
      std::cout << " size pts: " << gsse::size( gsse::at_dim<gsse::access_specifier::AP>(domain_) ) << std::endl;
#endif

//       // ================================
//       // [RH][info] [compability]
//       // if the gsse::adapted data structures are used, then the following line is possible
//       //
//       container = gsse::at_dim<gsse::access_specifier::AP>(domain_);
//       //
//       // ================================
      
      if (vx_id >= gsse::size( gsse::at_dim<gsse::access_specifier::AP>(domain_) ))
      {
         std::cout << " ## Run-time domain:: vx_id: " << vx_id << " not within position domain space ..size: "
                   << gsse::size( gsse::at_dim<gsse::access_specifier::AP>(domain_)) << std::endl;
            // TODO .. try catch -> exception
            //   geometrical point limit 
      }

      CoordPosT pos = gsse::at(vx_id)( gsse::at_dim<gsse::access_specifier::AP>(domain_) );
      ContainerNumericT container(pos.size());
      
      for (size_t cnt = 0; cnt < pos.size(); ++cnt)
      {
         container[cnt] = pos[cnt];
      }

      return container;
   }

  void set_vx_position(size_t vx_id, ContainerNumericT container)
   {

      typedef typename gsse::result_of::property_CoordPosT<PropDomain>::type CoordPosT;
#ifdef DEBUG
      std::cout << " size pts: " << gsse::size( gsse::at_dim<gsse::access_specifier::AP>(domain_) ) << std::endl;
#endif

//       // ================================
//       // [RH][info] [compability]
//       // if the gsse::adapted data structures are used, then the following line is possible
//       //
//       container = gsse::at_dim<gsse::access_specifier::AP>(domain_);
//       //
//       // ================================
      
      if (vx_id >= gsse::size( gsse::at_dim<gsse::access_specifier::AP>(domain_) ))
      {
         std::cout << " ## Run-time domain:: vx_id: " << vx_id << " not within position domain space ..size: "
                   << gsse::size( gsse::at_dim<gsse::access_specifier::AP>(domain_)) << std::endl;
            // TODO .. try catch -> exception
            //   geometrical point limit 
      }

      CoordPosT pos; // = gsse::at(vx_id)( gsse::at_dim<gsse::access_specifier::AP>(domain_) );
      gsse::resize(3)(pos);
      
      for (size_t cnt = 0; cnt < pos.size(); ++cnt)
      {
	pos[cnt]  = container[cnt];
      }

      gsse::at(vx_id)( gsse::at_dim<gsse::access_specifier::AP>(domain_) ) = pos;
   }
   

   ContainerIndexT vx_on_cell(long cell_id, long cnt_seg) 
   {
      typedef typename gsse::result_of::property_CellT<PropDomain>::type CellT;

      if (cell_id >= gsse::size_cl( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_) )))
      {
         std::cout << " ## Run-time domain:: cell_id: " << cell_id << " not within cell index domain space ..size: "
                   << gsse::size_cl( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_) )) << std::endl;
            // TODO .. try catch -> exception
            //   cell id not within id space
      }

      
      CellT cell = gsse::at_cl(cell_id)( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_) ));
      ContainerIndexT container(cell.size());
      
      for (size_t cnt = 0; cnt < cell.size(); ++cnt)
      {
         container[cnt] = cell[cnt];
      }

      return container;
   }



// ########## quan part

   void set_quantity(long object_id, std::string quan_name, std::string object_type, double value, long cnt_seg = 0) 
   {
      if (object_type == "vertex")
      {
         typedef typename gsse::result_of::property_QuanStorage_0form<PropDomain>::type QuanT_0form;
         QuanT_0form quan(1);
         quan[0] = value;

         gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) = quan;
      }
//       else if (object_type == "edge")
//       {
//          typedef typename gsse::result_of::property_QuanStorage_0form<PropDomain>::type QuanT_0form;
//          QuanT_0form quan(1);
//          quan[0] = value;

//          gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_ee>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) = quan;
//       }
//       else if (object_type == "facet")
//       {
//          typedef typename gsse::result_of::property_QuanStorage_0form<PropDomain>::type QuanT_0form;
//          QuanT_0form quan(1);
//          quan[0] = value;

//          gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_ft>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) = quan;
//       }
      else if (object_type == "cell")
      {
         typedef typename gsse::result_of::property_QuanStorage_0form<PropDomain>::type QuanT_0form;
         QuanT_0form quan(1);
         quan[0] = value;

         gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) = quan;
      }

      
   }

   void set_quantity2(long object_id, std::string quan_name, std::string object_type, std::vector<double> value, long cnt_seg = 0) 
   {
      if (object_type == "vertex")
      {
         typedef typename gsse::result_of::property_QuanStorage_0form<PropDomain>::type QuanT_0form;
         QuanT_0form quan(4);
         quan[0] = value[0];
         quan[1] = value[1];
         quan[2] = value[2];
         quan[3] = value[3];

#ifdef DEBUG
         std::cout << ".. before quantity add.." << std::endl;
#endif

         gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) = quan;

#ifdef DEBUG
         std::cout << ".. after quantity add.." << std::endl;
         std::cout << ".. quan: " 
                   << gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) )))))[0]
                   << " "
                   << gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) )))))[1]
                   << " "
                   << gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) )))))[2]
                   << " "
                   << gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) )))))[3]
                   << std::endl;
#endif
      }

      else if(object_type == "segment")
	{
	  typedef typename gsse::result_of::property_QuanStorage_0form<PropDomain>::type QuanT_0form;
	  QuanT_0form quan(4);

	  for(size_t i=0; i<gsse::size(value); ++i)
	    quan[i] = value[i];
	  
	  // #ifdef DEBUG
	  std::cout << ".. before quantity add.." << std::endl;
	  // #endif
	  
	  // [FS] fmi
	  gsse::at(object_id)(gsse::at(quan_name)( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::ASQ>(domain_)))) = quan;

	  std::cout << "..after add: " << gsse::at(object_id)(gsse::at(quan_name)( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::ASQ>(domain_))))[0] << std::endl;
	  std::cout << "..after add: " << gsse::at(object_id)(gsse::at(quan_name)( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::ASQ>(domain_))))[1] << std::endl;
	  std::cout << "..after add: " << gsse::at(object_id)(gsse::at(quan_name)( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::ASQ>(domain_))))[2] << std::endl;
	  std::cout << "..after add: " << gsse::at(object_id)(gsse::at(quan_name)( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::ASQ>(domain_))))[3] << std::endl;
	}
   }

   void get_quantity2(long object_id, std::string quan_name, std::string object_type, std::vector<double>& value, long cnt_seg = 0) 
   {
      if (object_type == "vertex")
      {
         long size = gsse::size(gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))));
         for(long i=0; i<size;i++)
         {
            value[i] = gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) [i];
         }
      }
      else if(object_type == "segment")
	{
	  long size = gsse::size(gsse::at(object_id)(gsse::at(quan_name)(gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::ASQ>(domain_)))));

	  std::cout << "..size: " << size << std::endl;

	  for(long i=0; i<size;i++)
	    {	 	  	     
	      value[i] = gsse::at(object_id)(gsse::at(quan_name)( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::ASQ>(domain_))))[i];
	    }
	}
   }


   double& set_quantity(long object_id, std::string quan_name, std::string object_type, long cnt_seg = 0) 
   {
      if (object_type == "vertex")
      {
         return 
            gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) [0];
      }
//       else if (object_type == "edge")
//       {
//          return gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_ee>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) [0];
//       }
//       else if (object_type == "facet")
//       {
//          return gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_ft>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) [0];
//       }
      else if (object_type == "cell")
      {
         return gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) )))))[0];
      }
      else
      {
         std::cout << "### warning.. object type (" << object_type << ") not found !! " << std::endl;
      }
      static double temp_val(0);   //nasy c++ problem (no rvalues yet)
      return temp_val;
      
   }



   void get_quantity(long object_id, std::string quan_name, std::string object_type, double& value, long cnt_seg = 0) 
   {
      if (object_type == "vertex")
      {
         value = gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) [0];
      }
//       else if (object_type == "edge")
//       {
//          value = gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_ee>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) [0];
//       }
//       else if (object_type == "facet")
//       {
//          value = gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_ft>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) )))))[0];
//       }
      else if (object_type == "cell")
      {
         value = gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) )))))[0];
      }
      else
      {
         std::cout << "### warning.. object type (" << object_type << ") not found !! " << std::endl;
      }

   }

   double get_quantity(long object_id, std::string quan_name, std::string object_type, long cnt_seg = 0) 
   {
      if (object_type == "vertex")
      {
         return gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) [0];
      }
//       else if (object_type == "edge")
//       {
//          return gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_ee>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) ))))) [0];
//       }
//       else if (object_type == "facet")
//       {
//          value = gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_ft>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) )))))[0];
//       }
      else if (object_type == "cell")
      {
         return gsse::at(object_id)(gsse::at(quan_name)(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)( gsse::at_dim<gsse::access_specifier::AQ>(domain_) )))))[0];
      }
      else
      {
         std::cout << "### warning.. object type (" << object_type << ") not found !! " << std::endl;
      }
      return double(0);

   }



// ########## modification part

   void add_ps(ContainerNumericT container, long cnt_seg ) 
   {
      if (gsse::size(gsse::at_dim<gsse::access_specifier::AP>(domain_)) )
      {
#ifdef DEBUG
         std::cout << "############### warning ########## : trying to add geometrical points to an already initialized domain.. CREATE a new domain instead !!! ## " << std::endl;
#endif

      }
      typedef typename gsse::result_of::property_CoordPosT<PropDomain>::type CoordPosT;
      CoordPosT pos;

      for (size_t cnt = 0; cnt < container.size(); ++cnt)
      {
#ifdef DEBUG
         std::cout << "..container[cnt]: " << container[cnt] << std::endl;
#endif
         pos[cnt] = container[cnt];
      }
      
      gsse::pushback (gsse::at_dim<gsse::access_specifier::AP>(domain_) ) = pos;

#ifdef DEBUG
      std::cout << "..after pushback: " << gsse::size(gsse::at_dim<gsse::access_specifier::AP>(domain_) ) << std::endl;
#endif

      // ## not used right now.. can check the topological vertex ids 
//      long vx_id = gsse::size(gsse::at_dim<gsse::access_specifier::AP>(domain_)) - 1; //new topological vx id;
   }


//    void add_vx(long vx_id, long cnt_seg ) 
//    {

//    }

   void add_cl(ContainerIndexT container, long cnt_seg ) 
   {
      if (gsse::size_cl( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_) )))
      {
#ifdef DEBUG
         std::cout << "############### warning ########## : trying to add cells to an already initialized domain.. CREATE a new domain instead !!! ## " << std::endl;
#endif
      }

      typedef typename gsse::result_of::property_CellT<PropDomain>::type CellT;
      CellT cell;
      for (size_t cnt = 0; cnt < container.size(); ++cnt)
      {
         cell[cnt] = container[cnt];
      }

      gsse::pushback(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_))))) = cell;
      long cell_size = gsse::size_cl( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_) ));
      for (size_t inside_i = 0; inside_i < gsse::size(cell); ++inside_i)
      {
         gsse::pushback( gsse::at_fiber( gsse::at_vx(  gsse::at(inside_i)(cell)) ( gsse::at(cnt_seg)(gsse::at_dim<gsse::access_specifier::AC>(domain_))) )  ) = inside_i + cell_size;  // cell ids are only valid within a segment 
      }
   }



   bool  is_domain_321() 
   {
      if (  (dim_g() >= 3) && (dim_t() >=2) && (dim_q() >= 1))
       	return true;
      return false;
   }
   
   gsse_domain_321  get_domain_321()    // [RH][TODO] .. use move semantics here
   {
	if (!is_domain_321())
	{
		 // [TODO] .. throw an exception / create log entry / emit warning
		std::cout << " ### warning ## current gsse domain is not 321 compatible ## " << std::endl;
	}
  
  	
	// [TODO]
	//  create the opengl data structure here..
	//
	gsse_domain_321 gsse_321;

   	return gsse_321 ;
   }



  // #######################################



  // gsse_opengl  context
  
  bool is_domain_opengl()  
  {
    //[TODO]
    // check memory layout

      if (  (dim_g() >= 3) && (dim_t() >=2) && (dim_q() >= 1))
       	return true;

      return false;
  }

  gsse::adapted::opengl::DataSet  get_domain_opengl()  //[TODO] use move semantics if available
  {
    gsse::adapted::opengl::DataSet dataset;



    // ====   geometry (positions) fiberbundle
    //
    if (  (dim_g() == 3) )
    {
      gsse::adapted::opengl::FBPointsT fb_points;
    

      // [info] two possible scenarios
      // [TODO] check both  .. optimize this
      //
      //  1) positions are already in opengl conform laytout
      //    fb_points =  gsse::at_dim<gsse::access_specifier::AP>( domain_ );
      
      //  2) positions have to be transfered
      for (size_t cnt = 0; cnt < gsse::size( gsse::at_dim<gsse::access_specifier::AP>( domain_ ) ) ; ++cnt)
      {
	  gsse::adapted::opengl::PointT new_point;

	  // [TODO] . .this loop can be omitted, if gssev02 domain uses consistent metrical_object / gsse::arary as position part
	  // gsse/adapted/gsse_opengl.hpp <-> gsse/datastructure/domain.hpp
	  //
	  for (size_t cnt_pos = 0; cnt_pos < gsse::size(gsse::at_dim<gsse::adapted::opengl::spec::position>(new_point)); ++cnt_pos)
          {
	    gsse::at(cnt_pos)(gsse::at_dim<gsse::adapted::opengl::spec::position>(new_point))  = gsse::at(cnt_pos)(gsse::at(cnt) (gsse::at_dim<gsse::access_specifier::AP>( domain_ )));
	  }
	  gsse::pushback( fb_points ) = new_point;
      }
      
      // ----------
      gsse::at_dim<gsse::access_specifier::AP> (dataset) = fb_points;

    }  // geometrical dimension not equal to 3
    else
    {
      std::cout << "## warning ..geometrical dimension not 3 .. dim_g: " << dim_g() << std::endl;
    }


    // ====   topological fiberbundle (cell complex)
    //
    if (  (dim_t() == 2) )
    {
      // direct transfer of 2-skeleton
      //
      gsse::adapted::opengl::SpaceTopologySegmentsT topology_segments;


      for (size_t cnt_segment = 0; cnt_segment < gsse::size( gsse::at_dim<gsse::access_specifier::AC>( domain_ ) ) ; ++cnt_segment)
      {
	
	for (size_t cnt_cell = 0; cnt_cell < gsse::size( gsse::at_dim<gsse::access_specifier::AT_cl>(gsse::at_fiber(gsse::at(cnt_segment)(gsse::at_dim<gsse::access_specifier::AC>( domain_ ))) )) ; ++cnt_cell)
        {	
 	  gsse::adapted::opengl::CellT cell;

	  // [info] .. this loop is necessary due to the fact, that dim_t() is a runtime information
	  //           AT_cl is a compiletime informatio
	  //
	  for (size_t cnt_celli = 0 ; cnt_celli < gsse::size(cell); ++cnt_celli)
          {
	    gsse::at(cnt_celli)(cell) = gsse::at(cnt_celli)(gsse::at(cnt_cell)(gsse::at_dim<gsse::access_specifier::AT_cl>(gsse::at_fiber(gsse::at(cnt_segment)(gsse::at_dim<gsse::access_specifier::AC>( domain_ ))) )));
	  }
	  std::cout << "cell: " << cell << std::endl;

	  gsse::pushback(gsse::at_dim<gsse::access_specifier::AT_cl>( gsse::at_fiber(gsse::at(cnt_segment)( topology_segments )))) = cell;
	}
      }
      
      gsse::at_dim<gsse::access_specifier::AC>(dataset) = topology_segments;
    }
    else   
    {
      //have to transfer/convert/create 2-skeleton


      gsse::adapted::opengl::SpaceTopologySegmentsT topology_segments;


      for (size_t cnt_segment = 0; cnt_segment < gsse::size( gsse::at_dim<gsse::access_specifier::AC>( domain_ ) ) ; ++cnt_segment)
      {
	// create corresponding accessor for 2-skeleton
	//
	typedef gsse::property_domain<DomainT>                                        PropertyDomain;
	typedef typename gsse::result_of::property_CellComplex<PropertyDomain>::type  CellComplex;
	typedef typename gsse::util_meta::dim_2_key< CellComplex, 2>::type            acc_2skeleton;


	// [TODO] .. if there are no cells in acc_2skeleton, then they must be created..
	// 

	for (size_t cnt_cell = 0; cnt_cell < gsse::size( gsse::at_dim<acc_2skeleton>(gsse::at_fiber(gsse::at(cnt_segment)(gsse::at_dim<gsse::access_specifier::AC>( domain_ ))) )) ; ++cnt_cell)
        {	
 	  gsse::adapted::opengl::CellT cell;

	  for (size_t cnt_celli = 0 ; cnt_celli < gsse::size(cell); ++cnt_celli)
          {
	    gsse::at(cnt_celli)(cell) = gsse::at(cnt_celli)(gsse::at(cnt_cell)(gsse::at_dim<acc_2skeleton>(gsse::at_fiber(gsse::at(cnt_segment)(gsse::at_dim<gsse::access_specifier::AC>( domain_ ))) )));
	  }
	  std::cout << "cell: " << cell << std::endl;

	  gsse::pushback(gsse::at_dim<gsse::access_specifier::AT_cl>( gsse::at_fiber(gsse::at(cnt_segment)( topology_segments )))) = cell;
	}
      }

      gsse::at_dim<gsse::access_specifier::AC>(dataset) = topology_segments;
    }

    return dataset;
  }

  // --------------------


   bool complete_domain()
   {
      create_cl_on_vx(domain_);

      return true;
   }

  // ## be carefull with this..
   DomainT& domain() { return domain_;}

private:
   DomainT domain_;
};




// template <class BasePtr>
// bool check_dynamic_pointer_cast(const BasePtr &ptr)
// {
//    //Check that dynamic_pointer_cast versus dynamic_cast
//    return
//    //Correct cast with dynamic_pointer_cast
//    boost::get_pointer(boost::dynamic_pointer_cast<derived>(ptr)) ==
//       //Correct cast with dynamic_cast
//       dynamic_cast<derived*>(boost::get_pointer(ptr)) 
//    &&
//    //Incorrect cast with dynamic_pointer_cast
//    boost::get_pointer(boost::dynamic_pointer_cast<derived_derived>(ptr)) ==
//       //Incorrect cast with dynamic_cast
//       dynamic_cast<derived_derived*>(boost::get_pointer(ptr));
// }


template<typename DomainRT>
size_t size_sg(DomainRT& domain_rt)
{
   return domain_rt->size_sg();
}

template<typename DomainRT>
size_t size_vx(DomainRT& domain_rt, long segment_cnt = 0)
{
   return domain_rt->size_vx(segment_cnt);
}

template<typename DomainRT>
size_t size_cl(DomainRT& domain_rt, long segment_cnt = 0)
{
   return domain_rt->size_cl(segment_cnt);
}

template<typename DomainRT>
size_t size_ps(DomainRT& domain_rt)
{
   return domain_rt->size_ps();
}

template<typename DomainRT>
ContainerNumericT rt_pos(DomainRT& domain_rt, long vx_id)
{
   return domain_rt->vx_position(vx_id);
}


template<typename DomainRT>
ContainerIndexT rt_cell(DomainRT& domain_rt, long cell_id, long cnt_seg=0)
{
   return domain_rt->vx_on_cell(cell_id, cnt_seg);
}


template<typename DomainRT, typename CallSig>
double& quan_set(DomainRT& domain_rt, CallSig call_sig, long cnt_seg = 0)
{
   return domain_rt->set_quantity(boost::fusion::at_c<0>(call_sig),
                                  boost::fusion::at_c<1>(call_sig),
                                  boost::fusion::at_c<2>(call_sig),
                                  cnt_seg);

}
template<typename DomainRT, typename CallSig>
double quan_get(DomainRT& domain_rt, CallSig call_sig, long cnt_seg = 0)
{
   return domain_rt->get_quantity(boost::fusion::at_c<0>(call_sig),
                                  boost::fusion::at_c<1>(call_sig),
                                  boost::fusion::at_c<2>(call_sig),
                                  cnt_seg);

}


} // namespace adapted
} // namespace gsse

#endif


