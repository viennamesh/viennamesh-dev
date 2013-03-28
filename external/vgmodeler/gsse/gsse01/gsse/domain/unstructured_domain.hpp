/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_UNSTRUCTURED_DOMAIN_HH_ID
#define GSSE_UNSTRUCTURED_DOMAIN_HH_ID

// *** system includes
//
#include <iomanip>
#include <vector>
#include <map>
#include <set>
#include <fstream>  
#include <algorithm>

// *** BOOST includes
//
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/at.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>

// *** GSSE includes
//
#include "gsse/segment.hpp"
#include "gsse/iterator.hpp"
#include "gsse/math/geometric_point.hpp"
#include "gsse/domain/base_domain.hpp"


// *** external Mauch library
//
// namespace external_mauch
//{

#include "geom/orq/CellArray.h"

// #include "ads/array/FixedArray.h"
// #include "geom/orq3d/Record.h"
// #include "geom/orq3d/Octree.h"
//#include "geom/orq3d/CellXYSearchY.h"
//}


namespace gsse
{
namespace detail_domain
{

template <typename NumericTypeForCoordinates, 
          typename SegmentT, 
          typename NumericTQuantity,
          typename IdentifierTQuantity,
          unsigned int DIM=topology_traits<typename segment_traits<SegmentT>::topology_t>::dimension_topology >
class unstructured_domain  : public base_domain<typename segment_traits<SegmentT>::topology_t, NumericTQuantity, IdentifierTQuantity>
{
public:

   typedef unstructured_domain<NumericTypeForCoordinates, SegmentT, NumericTQuantity, IdentifierTQuantity, DIM>         self;
   typedef unstructured_domain<NumericTypeForCoordinates, SegmentT, NumericTQuantity, IdentifierTQuantity, DIM>         self_t;
   typedef base_domain<typename segment_traits<SegmentT>::topology_t, NumericTQuantity,IdentifierTQuantity>             base_domain_type;

public:
   typedef SegmentT                                                               segment_t;
   typedef SegmentT                                                               segment_type;

   typedef typename base_domain_type::storage_type                                storage_type;

protected:
   typedef typename segment_traits<segment_t>::topology_t                         topology_t;
   typedef typename segment_traits<segment_t>::quantity_type		          quantity_type;

   typedef typename segment_traits<segment_t>::segment_vertex_quantity_iterator   segment_vertex_quantity_iterator;
   typedef typename segment_traits<segment_t>::segment_edge_quantity_iterator     segment_edge_quantity_iterator;
   typedef typename segment_traits<segment_t>::segment_facet_quantity_iterator    segment_facet_quantity_iterator;
   typedef typename segment_traits<segment_t>::segment_cell_quantity_iterator     segment_cell_quantity_iterator;

   ///////////////////////////////////////////////////////////////////////////
   //
   // wrapping typedefs
   //  
   ///////////////////////////////////////////////////////////////////////////

   typedef typename topology_traits<topology_t>::vertex_type                   vertex_type;
   typedef typename topology_traits<topology_t>::edge_type                     edge_type;
   typedef typename topology_traits<topology_t>::facet_type                    facet_type;
   typedef typename topology_traits<topology_t>::cell_type                     cell_type;
	    	     
   typedef typename topology_traits<topology_t>::vertex_iterator               vertex_iterator;
   typedef typename topology_traits<topology_t>::cell_iterator                 cell_iterator;
	    	     
   typedef typename topology_traits<topology_t>::cell_on_vertex_iterator       cell_on_vertex_iterator;
   typedef typename topology_traits<topology_t>::vertex_on_edge_iterator       vertex_on_edge_iterator;
   typedef typename topology_traits<topology_t>::edge_on_vertex_iterator       edge_on_vertex_iterator;
   typedef typename topology_traits<topology_t>::vertex_on_cell_iterator       vertex_on_cell_iterator;
   typedef typename topology_traits<topology_t>::cell_on_edge_iterator         cell_on_edge_iterator;

   typedef typename topology_traits<topology_t>::vertex_on_facet_iterator      vertex_on_facet_iterator;
   typedef typename topology_traits<topology_t>::edge_on_cell_iterator         edge_on_cell_iterator;
   typedef typename topology_traits<topology_t>::facet_on_cell_iterator        facet_on_cell_iterator;

   typedef typename topology_traits<topology_t>::vertex_handle                 vertex_handle;
   typedef typename topology_traits<topology_t>::edge_handle                   edge_handle;
   typedef typename topology_traits<topology_t>::facet_handle                  facet_handle;
   typedef typename topology_traits<topology_t>::cell_handle                   cell_handle;

   typedef typename topology_traits<topology_t>::dimension_tag                 dimension_tag;
   typedef typename topology_traits<topology_t>::topology_tag                  topology_tag;
   typedef typename topology_traits<topology_t>::dimension_topology_tag        dimension_topology_tag;


 public:
   static const unsigned int dimension            = topology_traits<topology_t>::dimension_tag::dim;
   static const unsigned int dimension_topology   = topology_traits<topology_t>::dimension_tag::dim;
   static const unsigned int dimension_geometry   = DIM;

   typedef typename base_domain_type::vertex_quantity_type::key_iterator                vertex_key_iterator;
   typedef typename base_domain_type::edge_quantity_type::key_iterator                  edge_key_iterator;
   typedef typename base_domain_type::facet_quantity_type::key_iterator                 facet_key_iterator;
   typedef typename base_domain_type::cell_quantity_type::key_iterator                  cell_key_iterator;

   // old GSSE version
   //
   //typedef generic_point<NumericTypeForCoordinates, dimension_geometry> point_t;

   // new GSSE::mauch version
   //
   typedef ads::FixedArray<dimension_geometry, NumericTypeForCoordinates> point_t;
//   class segment_iterator;


//protected:
  // *****************************************************************
  // local unstructured point list
  //
  std::vector<segment_t>                               segment_container;
  typedef typename std::vector<segment_t>::iterator    segment_iterator;


  typedef std::map<vertex_handle, point_t>             global_point_list_t;
  global_point_list_t                                  global_point_list;
  std::map<vertex_handle, std::set<segment_iterator> > segment_vertex_list;
   

   // [RH] 2007/11:: global domain quantity
   //
   typedef std::string            quan_key_t;
   typedef gsse_quantity_sparse<long, quan_key_t, storage_type>        domain_quan_type;
   typedef typename quantity_traits<domain_quan_type>::key_iterator    domain_key_iterator;
   domain_quan_type                                                    domain_quan;
   // [RH][TODO].. make numerical and string quantity the same
   //
   typedef gsse_quantity_sparse<vertex_handle, quan_key_t, std::string>    domain_squantity_type;



  // [RH] new for the Mauch::ORQ
  //
   typedef typename global_point_list_t::const_iterator Record_t; 
   typedef point_t                                      Multikey_t;  

   struct gp_accessor : 
     public std::unary_function<Record_t,Multikey_t>  // The functor to access the multikey.
   {   
     const Multikey_t& operator()(const Record_t& l_record) const 
     {
       return (*l_record).second;
     }
   };

   typedef geom::CellArray<dimension_geometry,Record_t, Multikey_t, typename  Multikey_t::value_type, gp_accessor> CellArray;
   typedef typename CellArray::BBox BBox;
   typedef typename CellArray::SemiOpenInterval SemiOpenInterval;

 //  CellArray*  point_container;
   typedef boost::shared_ptr<CellArray> point_container_smartptr;

   point_container_smartptr pc_sptr; 
  //
  //================================



public:

   // Constructors
  
   unstructured_domain() 
   { 
   }
   
  ~unstructured_domain()
  {
//	std::cout << "Domain destructor:: smart pointer memory count: " << pc_sptr.use_count() << std::endl;
  }

  

   // ********************************************************************************************************
   // Segment Iterators
  
//    class segment_iterator
//      : public boost::iterator_facade<
//      segment_iterator                        // Derived
//      , segment_type                          // Base
//      , boost::random_access_traversal_tag   // CategoryOrTraversal     
//      , segment_t&                            // Base
//       >
//    {
//    public:
      
//       typedef self base_type;
      
//       segment_iterator() {}
      
//       segment_iterator(base_type & base, int mc) : mc(mc), base_(&base) 
//          {
//             std::cout << " seg it ctr: with mc: " << mc << std::endl;
//          }
      
//       bool equal(segment_iterator const& other) const  { return mc == other.mc; }
//       long distance_to(segment_iterator const& other) const {return mc - other.mc;}
//       void increment() {++mc;}
//       void decrement() {--mc;}
//       void advance (int i) {mc += i;}
//       segment_t& dereference() const {return base().segment_container[mc];}
      
//       operator size_t () 
//       {
//          std::cout << "  cast operator .. " << std::endl;
//          return mc;
//       }

//       bool valid() const {return mc < static_cast<int>(base().segment_size());}
//       void reset() {mc = 0;}
//       base_type & base() {return *base_;}
//       base_type & base() const {return *base_;}
      
//    private:
//       int mc;
//       base_type * base_;
//    };
   
//    segment_iterator segment_begin() {return segment_iterator(*this, 0);}
//    segment_iterator segment_end()   {return segment_iterator(*this, segment_container.size());}
   typename std::vector<segment_t>::iterator 
     segment_begin() {return segment_container.begin();}
   typename std::vector<segment_t>::iterator 
     segment_end()   {return segment_container.end();}

   unsigned int segment_size() const{return segment_container.size();}
   
  // ****************************************************************
  // Access to points
  
  long point_size()        const {  return global_point_list.size();     }

   point_t & get_point(const vertex_type& ve) 
      { return (*(global_point_list.find(ve.handle()))).second;}

   point_t const& get_point_fromhandle(const vertex_handle& vh) const 
      { return (*(global_point_list.find(vh))).second;}
  
  
  // ****************************************************************
  // domain constrution
  //
  
  vertex_type add_point(const point_t& pt, segment_iterator segit) 
  {
    vertex_type v;
    
      v = (*segit).add_user_point(vertex_handle(global_point_list.size()));
      
      global_point_list[v.handle()] = pt;
      segment_vertex_list[v.handle()].insert(segit);
      
      return v;
  }

   void fast_point_insert(const point_t& pt) 
   {
      global_point_list[vertex_handle(global_point_list.size())] = pt;
   }

   // adds only a vertex information, not the point information
   //
   void add_vertex(const vertex_handle& v, segment_iterator segit) 
      {
         // CHECK IF
         // v.handle < global_point_list.size()
         //(*segit).add_user_point(v);

         segment_vertex_list[v].insert(segit);	 
      }


   // add an existing segment and obtain the segment iterator
   //
   segment_iterator add_segment(segment_type & seg)
      {
         segment_container.push_back(seg);
         segment_iterator result(segment_begin());
         result += segment_size()-1;
         return result;
      }


   // add a new segment and obtain the segment iterator
   //
   segment_iterator add_segment()
      {
         long cell_counter = 0;
         for (segment_iterator seg_it = segment_begin();
              seg_it != segment_end(); ++seg_it)
         {
            cell_counter += (*seg_it).cell_size();
         }

         segment_type seg;
         seg.set_cell_index_offset(cell_counter);


         segment_container.push_back(seg);
         segment_iterator result(segment_begin());
         result += segment_size()-1;
         return result;
      }
   // add a new segment and obtain the segment iterator
   //
   segment_iterator add_segment(std::string segname)
      {
         long cell_counter = 0;
         for (segment_iterator seg_it = segment_begin();
              seg_it != segment_end(); ++seg_it)
         {
            cell_counter += (*seg_it).cell_size();
         }

         segment_type seg;
         seg.set_cell_index_offset(cell_counter);
         seg.set_name(segname);

         segment_container.push_back(seg);
         segment_iterator result(segment_begin());
         result += segment_size()-1;
         return result;
      }
   //
   //
   // *************************************************************************
   // Segment On Vertex Iterator
   //
  int dump_vertex_segment_info()
  {
    std::cout << "DUMP: " << std::endl;

    typename std::map<vertex_handle, std::set<segment_iterator> >::iterator mit;
    typename std::set<segment_iterator>::iterator ssit;

    for (mit = segment_vertex_list.begin(); mit != segment_vertex_list.end(); ++mit)
      {
	std::cout << (*mit).first << std::endl;;

	for (ssit = (*mit).second.begin(); 
	     ssit != (*mit).second.end();
	     ++ssit)
	  {
	    std::cout << "                                          " << **ssit << std::endl;
	  }
      }
    return 0;
  }

   
   class segment_on_vertex_iterator
      : public boost::iterator_facade<
      segment_on_vertex_iterator                       // Derived
      , segment_iterator                               // Base
      , boost::bidirectional_traversal_tag             // CategoryOrTraversal     
      , segment_iterator                               // Base
      , long
      >
   {
      typedef segment_on_vertex_iterator self;
   public:
      typedef vertex_type base_type;
   private:

      friend class boost::iterator_core_access;

   public:

     segment_on_vertex_iterator() {}

     template <typename SegmentIterator>
     segment_on_vertex_iterator(SegmentIterator & segit, base_type const& v) : v(v) 
     {
       internal_begin = segit.base().segment_vertex_list[v.handle()].begin();
       internal_end   = segit.base().segment_vertex_list[v.handle()].end();
       internal_iter  = internal_begin;
     }

// [RH][TODO] .. recheck all of this segment on vertex it
//
//      template <typename SegmentIterator>
//      segment_on_vertex_iterator(SegmentIterator & segit, vertex_handle const& vh) : v(v) 
//      {
//        internal_begin = segit.base().segment_vertex_list[vh].begin();
//        internal_end   = segit.base().segment_vertex_list[vh].end();
//        internal_iter  = internal_begin;
//      }

      // [RH] new ctr
      //
     template <typename DomainT>
     segment_on_vertex_iterator(DomainT& domain, vertex_handle const& vh) //: v(v) 
     {
       internal_begin = domain.segment_vertex_list[vh].begin();
       internal_end   = domain.segment_vertex_list[vh].end();
       internal_iter  = internal_begin;
     }

     
     void increment() {  ++internal_iter;  }
     void decrement() {  --internal_iter;  }
     //void advance(unsigned int i) {  internal_iter += i;  }
     //unsigned int distance_to(self const& other) const 
     //{ return internal_iter - other.internal_iter;}

     
     // [RH][TODO] .. check the segment equality
     //
     bool equal(const self& other) const 
     { return  &*(internal_iter) == &*(other.internal_iter); }

     segment_iterator dereference()  const { return *internal_iter; }               
     bool valid()                    const { return internal_iter != internal_end; }
     void reset()                          { internal_iter = internal_begin;}

     base_type const& base() {return v;}
     
     friend std::ostream& operator<<(std::ostream& ostr, const self& me)
     {
       ostr << "seg it: " << me.valid();
       return ostr;
     }

     const base_type& base() const { return v;}

   private:
      typename std::set<segment_iterator>::iterator internal_iter;
      typename std::set<segment_iterator>::iterator internal_begin;
      typename std::set<segment_iterator>::iterator internal_end;
      base_type v;
   };

  friend class segment_on_vertex_iterator;



public:
   // [RH] 2007/11 :: global domain quantities
   //
   domain_key_iterator  domain_key_begin()  const {return domain_quan.key_begin(); }
   domain_key_iterator  domain_key_end()    const {return domain_quan.key_end();   }
//   domain_skey_iterator domain_skey_begin() const {return domain_squan.key_begin();}
//   domain_skey_iterator domain_skey_end()   const {return domain_squan.key_end();  }
   
   unsigned int domain_key_size()           const {return domain_quan.key_size();  }
//   unsigned int domain_skey_size()          const {return domain_squan.key_size(); }
   
   void retrieve_domain_quantity(const quan_key_t& key, storage_type& value) 
      {
         long localhandle(0);
         value = domain_quan.retrieve_quantity(localhandle, key);
      }
   void store_domain_quantity(const quan_key_t& key, const storage_type& value)       
      {
         long localhandle(0);
         domain_quan.insert(localhandle,key,value);
      }
   

   // [RH] added.. in V0.9 (200711) for point containers
   //
   void set_boundingbox(point_t llpoint, point_t urpoint, long resolution_ticks=20)
      {
         // set the cell resolution on /resolution_ticks/ cells per coordinate axis
         //
         point_t resolution_point = (urpoint - llpoint)/resolution_ticks;
          //std::cout << "# GSSE spatial resolution: " << resolution_point << std::endl;
	  pc_sptr.reset( new CellArray(point_t(resolution_point), SemiOpenInterval(point_t(llpoint), point_t(urpoint))) );

          //std::cout << "new memory count: " << pc_sptr.use_count() << std::endl;
 
      }


   void create_pointcontainer()
      {
         // get the bounding box
         //
         point_t llpoint( (* global_point_list.begin()).second );
         point_t urpoint( (* global_point_list.begin()).second );
         for (Record_t rec_it = global_point_list.begin(); rec_it != global_point_list.end(); ++rec_it) 
         {
	   
            llpoint[0] = std::min(llpoint[0], (*rec_it).second[0]); 
            urpoint[0] = std::max(urpoint[0], (*rec_it).second[0]); 

	    // [RH][TODO][!!!] .. convert this into a compile time program
	    //
	    if (dimension_geometry > 1)
	      {
		llpoint[1] = std::min(llpoint[1], (*rec_it).second[1]); 
		urpoint[1] = std::max(urpoint[1], (*rec_it).second[1]); 

		if (dimension_geometry >2 )
		  {
		    llpoint[2] = std::min(llpoint[2], (*rec_it).second[2]); 
		    urpoint[2] = std::max(urpoint[2], (*rec_it).second[2]); 
		    std::cout << "## three dimensional .. " << std::endl;
		    std::cout << "## point ll: " << llpoint << std::endl;
		    std::cout << "## point ur: " << urpoint << std::endl;

		    if (dimension_geometry > 3)
		      {
			std::cout << " ############  geometrical dimension has to be <= 3 !!!!! ## " << std::endl;
		      }

		  }
	      }
         }

         // resize the bounding box per 1 percent
         // 
//          std::cout << "# GSSE bounding box (ll/ur): (" << llpoint << ")/(" << urpoint << ")"<<std::endl;
         point_t resize_p = (urpoint - llpoint)/100 * 1;
         llpoint -= resize_p;
         urpoint += resize_p;

         //std::cout << "# GSSE bounding box (ll/ur): (" << llpoint << ")/(" << urpoint << ")"<<std::endl;
         set_boundingbox(llpoint, urpoint);

	 if (pc_sptr.use_count()  == 0)
	   {
	     std::cout << "# GSSE no point container created  !!!! " << std::endl;
	     return;
	   }


         for (Record_t rec_it  = global_point_list.begin(); rec_it != global_point_list.end(); ++rec_it) 
         {
           // std::cout << "inserting  " <<  (*rec_it).second  << std::endl;
	    pc_sptr->insert(rec_it);
         }
         //std::cout << "### GSSE domain: spatial data structure finalized.. " << std::endl;
    
  }

  std::vector<vertex_handle> query_pointcontainer(point_t p1, point_t p2)
  {
     if (pc_sptr.use_count() == 0)   // no spatial resolution container is present
      {
	return std::vector<vertex_handle>(0);
      }

    //
    // queries !!!
    //
    BBox window(p1, p2);
    std::vector<Record_t> inside;
    std::vector<vertex_handle>  inside_point_container;
    pc_sptr->computeWindowQuery(std::back_inserter(inside), window);

    for (typename std::vector<Record_t>::const_iterator rec_it = inside.begin(); rec_it != inside.end(); ++rec_it) 
      {
	vertex_handle position = (*(* rec_it )).first;
	// std::cout << " found pos: " << position << std::endl;
	inside_point_container.push_back(position);
      }
    inside.clear();

    return inside_point_container;
  }

//   std::vector<vertex_type> query_pointcontainer_vt(point_t p1, point_t p2)
//   {
//     //
//     // queries !!!
//     //
//     BBox window(p1, p2);
//     std::vector<Record_t> inside;
//     std::vector<vertex_type>  inside_point_container;
//     point_container->computeWindowQuery(std::back_inserter(inside), window);

//     for (typename std::vector<Record_t>::const_iterator rec_it = inside.begin(); rec_it != inside.end(); ++rec_it) 
//       {
// 	vertex_handle position = (*(* rec_it )).first;
// 	//std::cout << " found pos: " << position << std::endl;
// 	inside_point_container.push_back(   position);
//       }
//     inside.clear();

//     return inside_point_container;
//   }


   // *************************************************************************************
   // IO methods

   typedef typename std::map<vertex_handle, point_t>::iterator global_point_iterator;

   global_point_iterator   point_begin()
   {
	return global_point_list.begin();
   }			
   global_point_iterator  point_end()
   {
	return global_point_list.end();
   }			

   long vertex_size()
   {
      long number_of_vertex(0);

      segment_iterator segit;
      for (segit = segment_begin(); segit != segment_end(); ++segit)
      {
         number_of_vertex += ( (*segit).vertex_size() );
      }

      return number_of_vertex;
   }   

   long edge_size()
   {
      long number_of_edge(0);

      segment_iterator segit;
      for (segit = segment_begin(); segit != segment_end(); ++segit)
      {
         number_of_edge += ( (*segit).edge_size() );
      }

      return number_of_edge;
   }   


   long facet_size()
   {
      long number_of_facet(0);

      segment_iterator segit;
      for (segit = segment_begin(); segit != segment_end(); ++segit)
      {
         number_of_facet += ( (*segit).facet_size() );
      }

      return number_of_facet;
   }   


   long cell_size()
   {
      long number_of_cell(0);

      segment_iterator segit;
      for (segit = segment_begin(); segit != segment_end(); ++segit)
      {
         number_of_cell += ( (*segit).cell_size() );
      }

      return number_of_cell;
   }   

   // [RH] .. since GCC 3.4.. depending-lookup is implemented correctly and
   //   the base_domain methods have to be forwarded 
   //
   using base_domain_type::retrieve_quantity;
//   using base_domain_type::store_quantity_notsafe;
   using base_domain_type::store_quantity;
   using base_domain_type::store_quantity_direct;
   using base_domain_type::operator();

   using base_domain_type::vertex_key_size;
   using base_domain_type::edge_key_size;
   using base_domain_type::facet_key_size;
   using base_domain_type::cell_key_size;

//    using typename base_domain_type::vertex_key_iterator;
//    using typename base_domain_type::edge_key_iterator;
//    using typename base_domain_type::facet_key_iterator;
//    using typename base_domain_type::cell_key_iterator;

   using base_domain_type::vertex_key_begin;
   using base_domain_type::vertex_key_end;
   using base_domain_type::edge_key_begin;
   using base_domain_type::edge_key_end;
   using base_domain_type::facet_key_begin;
   using base_domain_type::facet_key_end;
   using base_domain_type::cell_key_begin;
   using base_domain_type::cell_key_end;



   // ##############################################################################################
   //
   // *** register quantity
   //
   void register_vertex_quantity(const quan_key_t& quan_k, const vertex_handle& vh)
      {
         segment_iterator segit;
         for (segit = segment_begin(); segit != segment_end(); ++segit)
         {
            if ((*segit).is_vertex_handle_inside(vh))
            {
               (*segit).add_vertex_quantity(quan_k);
            }
         }
      }
   void register_cell_quantity(const quan_key_t& quan_k, const cell_handle& ch)
      {
         segment_iterator segit;
         for (segit = segment_begin(); segit != segment_end(); ++segit)
         {
            if ((*segit).is_cell_handle_inside(ch))
            {
               (*segit).add_cell_quantity(quan_k);
            }
         }
      }
   void register_edge_quantity(const quan_key_t& quan_k, const edge_handle& eh)
      {
         segment_iterator segit;
         for (segit = segment_begin(); segit != segment_end(); ++segit)
         {
            if (  ((*segit).is_vertex_handle_inside(boost::get<0>(eh))) && 
                  ((*segit).is_vertex_handle_inside(boost::get<1>(eh))) )
            {
               (*segit).add_edge_quantity(quan_k);
            }
         }
      }
//    void register_facet_quantity(const quan_key_t& quan_k, const facet_handle& eh)
//       {
//          segment_iterator segit;
//          for (segit = segment_begin(); segit != segment_end(); ++segit)
//          {
//             if (  ((*segit).is_vertex_handle_inside(eh.first)) && 
//                   ((*segit).is_vertex_handle_inside(eh.second)) )
//             {
//                (*segit).add_facet_quantity(quan_k);
//             }
//          }
//       }


   typedef typename base_domain_type::vertex_key_type  vertex_key_type;
   typedef typename base_domain_type::edge_key_type    edge_key_type;
   typedef typename base_domain_type::facet_key_type   facet_key_type;
   typedef typename base_domain_type::cell_key_type    cell_key_type;
   
   typedef storage_type storage_vertex_type;
   typedef storage_type storage_edge_type;
   typedef storage_type storage_facet_type;
   typedef storage_type storage_cell_type;

   void store_quantity_safe(const vertex_type& object, const vertex_key_type& key, const storage_vertex_type& value)
      {  
         register_vertex_quantity(key, object.handle());
         store_quantity(object, key, value);    
      }
   void store_quantity_safe(const edge_type& object, const edge_key_type& key, const storage_edge_type& value)
      {  
         register_edge_quantity(key, object.handle());
         store_quantity(object, key, value);    
      }
   // [RH] [TODO] .. check the facet handle
   //
   void store_quantity_safe(const facet_type& object, const facet_key_type& key, const storage_facet_type& value)
      {  
//         register_facet_quantity(key, object.handle());
         store_quantity(object, key, value);    
      }

   void store_quantity_safe(const cell_type& object, const cell_key_type& key, const storage_cell_type& value)
      {  
         register_cell_quantity(key, object.handle());
         store_quantity(object, key, value);    
      }





   // *************************************************************************************
   // gsse's unstructured reader
   //
   typedef typename segment_traits<segment_t>::cell_2_vertex_mapping              cell_2_vertex_mapping;

   long read_file(const std::string& filename, bool verbose = true)
   {
      std::string file_ending;
      std::string::size_type pos = filename.rfind(".")+1;
      //std::cout << "  filename: " << filename.size() << std::endl;
      //std::cout << "pos: "<< pos << std::endl;
      file_ending = filename.substr(pos); 
      //std::cout << "fileending: " << file_ending<< std::endl;
      //std::cout << " reduced  file: " << file_ending.substr(0,3) << std::endl;
      
      if (file_ending == "wss" || file_ending == "ws0")
      {
         if (verbose)
            std::cout << "GSSE READ FILE: wss/ws0 is no longer supported.. please use gsse's ioconverter .. " << std::endl;
         return -1;
      }
      else if (file_ending.substr(0,3) == "gau")
      {
         if (verbose)
            std::cout << "### GSSE READ FILE: reading gsse native unstructured file ..  " << std::endl;
         return read_native_gsse_file(filename, verbose);
         //return read_native_gsse_file_c_code(filename);
      }

      if (verbose)
         std::cout << "GSSE READ FILE: fileformat is not supported..: : "  << file_ending << std::endl;

      return -1; 
   }



   long read_native_gsse_file(const std::string& filename, bool verbose=true)
   { 
      std::ifstream inputfile(filename.c_str(), std::ios::binary);
      unsigned int local_dimension_topology, local_dimension_geometry;
      std::string file_ending;
      std::string::size_type pos = filename.rfind(".")+1;
      file_ending = filename.substr(pos); 
      local_dimension_geometry = boost::lexical_cast<unsigned int>( file_ending.substr(3,1) );
      local_dimension_topology = local_dimension_geometry;

      if (file_ending.size() > 4)
	local_dimension_topology = boost::lexical_cast<unsigned int>(file_ending.substr(4,2));
      
      if (verbose)
      {
         std::cout << " [GSSE] reader: topological dimension: " << local_dimension_topology << std::endl;
         std::cout << " [GSSE] reader: geometrical dimension: " << local_dimension_geometry << std::endl;
      }

      if (!inputfile.is_open())
         throw file_not_found("Unstructured domain:: file not found .. ");
      
      BOOST_STATIC_ASSERT(dimension_topology <= 3);
      BOOST_STATIC_ASSERT(dimension_topology >= 1);
      BOOST_STATIC_ASSERT(dimension_geometry <= 3);
      BOOST_STATIC_ASSERT(dimension_geometry >= 1);

      
      // here, only the first topoloy information is used..
      //
      unsigned int tempdimgeometry=0; 
      inputfile >> tempdimgeometry;
      
	if (tempdimgeometry           != local_dimension_geometry  ||
	    tempdimgeometry           != dimension_geometry        ||
	    local_dimension_topology  != dimension_topology)
         throw dimension_problem ("Unstructured domain:: readin_simple_file with wrong dimension");



      // ############################ geometry #########################
      // geometry must have only one entry.. one domain = one geometry
      //  ..hence, vertex-handles have to be unique
      //    + unique hull property -> edges/facets/cells are unique as well
      //  .. BUT cells have a unique cell id (to save storage) .. remove this ?? [RH][TODO]
      //
      int number;
      inputfile >> number;   
      if (verbose)
         std::cout << " [GSSE] reader: geometrical points: "<< number <<std::endl; 
      point_t local_point;
      for (int nu=0; nu < number; ++nu)
      {
         for (unsigned int dim = 0; dim < dimension_geometry; ++dim)
         {
            double tempcoord;
            inputfile >> tempcoord;
	    local_point[dim] = tempcoord;
         }
         fast_point_insert(local_point);    
      }

     
      // ############################ topology #########################
      //   segment structure
      //    one segment - contains one topology 
      //    segmented vertices
      //    const segment quantities
      //
      int segment_size;
      inputfile >> segment_size;
      //std::cout << "seg size: "<< segment_size; 


      // read in the first segment name from the file
      //
      std::string seg_name;
      inputfile >> seg_name;  
      // [RH] .. if the first input of seg_name = Segment  -> GSSE V1.
      // then we have to read further  (backward compability)  .. should be dropped later
      //
      if (seg_name == "Segment:")
	{
	   inputfile >> seg_name;
      }

      // [RH] new.. 200712 for domain quantities and global cell ids
      //
      long cell_counter = 0;
      //
      for (int seg_cnt = 0; seg_cnt < segment_size; ++seg_cnt)
      {
         // create one default topo rep inside the segment (implicit)
         //
         segment_iterator segit = add_segment();
         (*segit).set_cell_index_offset(cell_counter);

         (*segit).set_name(seg_name);
         if (verbose)
            std::cout << "  ## Segment " << seg_name << std::endl;

         int number_elements, tempnn;
         inputfile >> number_elements;
         inputfile >> tempnn;
         if (verbose)
         {
            std::cout << "    # number cell:     " << number_elements << std::endl;
            std::cout << "    # number vertices: " << tempnn << std::endl;
         }

         // process the segment vector 
         //

         for (int nu=0; nu < number_elements; ++nu)
         {
            if (dimension_topology== 1)
            {
               int tmp, ind[2];
               inputfile >> tmp >> ind[0] >> ind[1];
               
               // [RH] [TODO] .. this is not the right input behaviour
               //
               (*segit).add_cell_2(cell_2_vertex_mapping(ind, ind+2));
               add_vertex(ind[0], segit);
               add_vertex(ind[1], segit);
               
            }
            else if (dimension_topology== 2)
            {
               int tmp, ind1, ind2, ind3;
               inputfile >> tmp >> ind1 >> ind2 >> ind3;
               // std::cout << "tmp: " <<tmp<< "  ind1: " << ind1 << "  ind2: "<< ind2 << "  ind3: " << ind3 <<std::endl;
               
               (*segit).add_cell_2(cell_2_vertex_mapping(ind1, ind2, ind3));
               add_vertex(ind1, segit);
               add_vertex(ind2, segit);
               add_vertex(ind3, segit);
            }
            else if (dimension_topology == 3)
            {
               int tmp, ind1, ind2, ind3, ind4;
               inputfile >> tmp >> ind1 >> ind2 >> ind3 >> ind4;
               //std::cout << "tmp: " <<tmp<< "  ind1: " << ind1 << "  ind2: "<< ind2 << "  ind3: " << ind3 <<std::endl;
               
               (*segit).add_cell_2(cell_2_vertex_mapping(ind1, ind2, ind3, ind4));
               add_vertex(ind1, segit);
               add_vertex(ind2, segit);
               add_vertex(ind3, segit);
               add_vertex(ind4, segit);
            }
         }

         // #######################################################################33
         //
         // GSSE's quantity part   .. fiberbundle/sheaf model 
         // 
         std::string tempstr;
         
//	std::cout << "GSSE ##  reader .. :: quantity section: " << std::endl;

         while(1)
         { 
            inputfile >> tempstr;  


	    if (tempstr != "Quantity:") 
            {
               seg_name = tempstr;
               if (seg_name == "Segment:")
               {
                  inputfile >> seg_name;
               }
               
               break;
            }
            
            inputfile >> tempstr; 
          // std::cout << "\t ## tempstr: "<< tempstr << std::endl; 
            // ==========================
            // segment quantities
            //

            if (tempstr == "domain")
            {
               long number_quan;
               inputfile >> number_quan;    
               for (long quani=0; quani < number_quan; ++quani)
               {
                  inputfile >> tempstr; 
           //       std::cout << " domain  quan: " << tempstr <<std::endl;
                  long numb_val;
                  inputfile >> numb_val;
                  for (long nv=0; nv < numb_val;++nv)
                  {
                     long index;
                     double tempval2;
                     inputfile >> index;
                     inputfile >> tempval2;
                     //std::cout << "    val: " << tempval2 << std::endl;
                     store_domain_quantity( tempstr,  storage_type(1,1,tempval2));
                  }
               } 
            }

            
            if (tempstr == "segment")
            {
               long number_quan;
               inputfile >> number_quan;    
               for (long quani=0; quani < number_quan; ++quani)
               {
                  inputfile >> tempstr; 
                  //std::cout << "  quan: " << tempstr;
                  long numb_val;
                  inputfile >> numb_val;
                  for (long nv=0; nv < numb_val;++nv)
                  {
                     long index;
                     double tempval2;
                     inputfile >> index;
                     inputfile >> tempval2;
                     //std::cout << "    val: " << tempval2 << std::endl;
                     (*segit).store_quantity( tempstr, storage_type(1,1,tempval2));
                  }
               } 
            }
            if (tempstr == "string-segment")
            {
               long number_quan;
               inputfile >> number_quan;    
               for (long quani=0; quani < number_quan; ++quani)
               {
                  inputfile >> tempstr; 
                  //std::cout << "  quan: " << tempstr;
                  long numb_val;
                  inputfile >> numb_val;
                  for (long nv=0; nv < numb_val;++nv)
                  {
                     long index;
                     std::string tempval2;
                     inputfile >> index;
                     inputfile >> tempval2;
                     //std::cout << "    val: " << tempval2 << std::endl;
                     (*segit).store_quantity( tempstr,  tempval2);
                  }
               } 
            }

            
            // ==========================
            // vertex quantities   .. backward compatibel.. simple fiber bundle /// extension for sheafs .. 
            //
            else if (tempstr == "vertex")
            {
               long number_quan;
               inputfile >> number_quan;    
               for (long quani=0; quani < number_quan; ++quani)
               {
                  inputfile >> tempstr; // quan name
//                  std::cout << " vertex quan: " << tempstr << std::endl;
                  long numb_val;
                  inputfile >> numb_val;
                  for (long nv=0; nv < numb_val;++nv)
                  {
                     long index;
                     double tempval2;
                     inputfile >> index;
                     inputfile >> tempval2;
                     //std::cout << "    val: " << tempval2 << std::endl;

                     vertex_handle vh = static_cast<vertex_handle>(index);
                     store_quantity_direct(  vh , tempstr, storage_type(1,1,tempval2));
                  }
               } 
            }
            else if (tempstr == "sheaf_vertex")
            {
               long number_quan;
               std::string quan_name;
               inputfile >> number_quan;    

               for (long quani=0; quani < number_quan; ++quani)
               {
                  inputfile >> quan_name; // quan name
                  //std::cout << "    ## sheaf vertex .. quanname: "<< quan_name << std::endl;          

                  // [RH] 200801 .. efficient storage of quantities
                  //   the actual quantity values are stored within the domain
                  //   the key information is stored in the respective segment
                  //
                  (*segit).add_vertex_quantity(quan_name);

                  long numb_val;
                  long data_nb_1 =1, data_nb_2 =1;
                  
    
                  inputfile >> tempstr; // new GSSE style: here comes the .. type 2 2  .. stands for a 2x2 matrix

                  //std::cout << "    ## new gsse thing: "<< tempstr << std::endl;      

                  inputfile >> data_nb_1;
                  //std::cout << "    ## data_nb_1: "<< data_nb_1 << std::endl;      
                  inputfile >> data_nb_2;
                  //std::cout << "    ## data_nb_2: "<< data_nb_2 << std::endl;      
                  inputfile >>  numb_val;

                  //std::cout << "    ## numb_val: "<< numb_val << std::endl;      

						
                  
                  for (long nv=0; nv < numb_val;++nv)
                  {
                     long index;
                     inputfile >> index;

                    
                     storage_type local_data(data_nb_1, data_nb_2, 0.0);  // init the matrix
                     for (long counter1 = 0; counter1 < data_nb_1; ++counter1)
                     {


                        for (long counter2 = 0; counter2 < data_nb_2; ++counter2)
			{


                           typename storage_type::numeric_t tempvalue;
                           inputfile >> tempvalue;

                           	local_data(counter1, counter2) = tempvalue;

			}
                     }

                      //std::cout << " .. # local data pattern : num " << nv << " / data: " << local_data << std::endl;                     

                     vertex_handle vh = static_cast<vertex_handle>(index);
                     store_quantity_direct(  vh, quan_name,  local_data );
                  }
               } 
            }
            
            
            // ==========================
            // edge quantities
            //
            
            else if (tempstr == "edge")
            {
               long number_quan;
               inputfile >> number_quan;    
               for (long quani=0; quani < number_quan; ++quani)
               {
                  inputfile >> tempstr; // quan name
                  //std::cout << "  quan: " << tempstr;
                  long numb_val;
                  inputfile >> numb_val;
                  for (long nv=0; nv < numb_val;++nv)
                  {
                     long index;
                     double tempval2;
                     inputfile >> index;
                     inputfile >> tempval2;
                     //std::cout << "    val: " << tempval2 << std::endl;
                     store_quantity_direct(  static_cast<edge_handle>(index), tempstr,  storage_type(1,1,tempval2));
                  }
               } 
            }
            else if (tempstr == "sheaf_edge")
            {
               long number_quan;
               std::string quan_name;
               inputfile >> number_quan;    
               //std::cout << "    ## sheaf edge .. number: "<< number_quan << std::endl;          
               for (long quani=0; quani < number_quan; ++quani)
               {
                  inputfile >> quan_name; // quan name
                  (*segit).add_edge_quantity(quan_name);
                  //std::cout << "    ## sheaf edge .. quanname: "<< quan_name << std::endl;          

                  long numb_val;
                  long data_nb_1 =1, data_nb_2 =1;
                  
                  inputfile >> tempstr; // new GSSE style: here comes the .. type 2 2  .. stands for a 2x2 matrix
                  inputfile >> data_nb_1;
                  inputfile >> data_nb_2;
                  inputfile >>  numb_val;
        	  // std::cout << "    ## sheaf edge .. number val: "<< numb_val << std::endl;          
                  
                  for (long nv=0; nv < numb_val;++nv)
                  {
                     edge_handle index;
                     inputfile >> index;
                     //std::cout << "  edge handle: " << index << std::endl;

                     storage_type local_data(data_nb_1, data_nb_2, 0.0);  // init the matrix
                     for (long counter1 = 0; counter1 < data_nb_1; ++counter1)
                     {
                        for (long counter2 = 0; counter2 < data_nb_2; ++counter2)
			{
                           typename storage_type::numeric_t tempvalue;
                           inputfile >> tempvalue;
                           local_data(counter1, counter2) = tempvalue;
			}
                     }
                     //std::cout << " .. # local data pattern: " << local_data << std::endl;
		  
                     store_quantity_direct( index, quan_name,  local_data );
                  }
               } 
            }
            
            
            else if (tempstr == "facet")
            {
               long number_quan;
               inputfile >> number_quan;    
               for (long quani=0; quani < number_quan; ++quani)
               {
                  inputfile >> tempstr; // quan name
                  //std::cout << "  quan: " << tempstr;
                  long numb_val;
                  inputfile >> numb_val;
                  for (long nv=0; nv < numb_val;++nv)
                  {
                     facet_handle index;
                     double tempval2;
                     inputfile >> index;
                     inputfile >> tempval2;
                     //std::cout << "    val: " << tempval2 << std::endl;
                     store_quantity_direct(  index, tempstr,  storage_type(1,1,tempval2));
                     
                  }
               } 
            }
            else if (tempstr == "sheaf_facet")
            {
               long number_quan;
               std::string quan_name;
               inputfile >> number_quan;    
               for (long quani=0; quani < number_quan; ++quani)
               {
                  inputfile >> quan_name; // quan name
                  (*segit).add_facet_quantity(quan_name);
        	 //  std::cout << "    ## sheaf facet .. quanname: "<< quan_name << std::endl;          
                  
                  long numb_val;
                  long data_nb_1 =1, data_nb_2 =1;
                  
                  inputfile >> tempstr; // new GSSE style: here comes the .. type 2 2  .. stands for a 2x2 matrix
                  inputfile >> data_nb_1;
                  inputfile >> data_nb_2;
                  inputfile >>  numb_val;
                  
                  for (long nv=0; nv < numb_val;++nv)
                  {
                     facet_handle index;
                     inputfile >> index;
                     
                     storage_type local_data(data_nb_1, data_nb_2, 0.0);  // init the matrix
                     for (long counter1 = 0; counter1 < data_nb_1; ++counter1)
                     {
                        for (long counter2 = 0; counter2 < data_nb_2; ++counter2)
			{
                           typename storage_type::numeric_t tempvalue;
                           inputfile >> tempvalue;
                           local_data(counter1, counter2) = tempvalue;
			}
                     }
                     //std::cout << " .. # local data pattern: " << local_data << std::endl;
                     
                     store_quantity_direct(  index , quan_name,  local_data );
                  }
               } 
            }
            
            
            // ==========================
            // cell quantities
            //
            
            else if (tempstr == "cell")
            {
               long number_quan;
               inputfile >> number_quan;    
               for (long quani=0; quani < number_quan; ++quani)
               {
                  inputfile >> tempstr; // quan name
                  //std::cout << "  quan: " << tempstr;
                  long numb_val;
                  inputfile >> numb_val;
                  for (long nv=0; nv < numb_val;++nv)
                  {
                     long index;
                     double tempval2;
                     inputfile >> index;
                     inputfile >> tempval2;
                     //std::cout << "    val: " << tempval2 << std::endl;
                     store_quantity_direct(  static_cast<cell_handle>(index), tempstr,  storage_type(1,1,tempval2));
                     
                  }
               } 
            }
            else if (tempstr == "sheaf_cell")
            {
               long number_quan;
               std::string quan_name;
               inputfile >> number_quan;    
               for (long quani=0; quani < number_quan; ++quani)
               {
                  inputfile >> quan_name; // quan name
                  (*segit).add_cell_quantity(quan_name);

        	  // std::cout << "    ## sheaf cell .. quanname: "<< quan_name << std::endl;          
                  
                  long numb_val;
                  long data_nb_1 =1, data_nb_2 =1;
                  
                  inputfile >> tempstr; // new GSSE style: here comes the .. type 2 2  .. stands for a 2x2 matrix
                  inputfile >> data_nb_1;
                  inputfile >> data_nb_2;
                  inputfile >>  numb_val;
                  
                  for (long nv=0; nv < numb_val;++nv)
                  {
                     long index;
                     inputfile >> index;
                     
                     storage_type local_data(data_nb_1, data_nb_2, 0.0);  // init the matrix
                     for (long counter1 = 0; counter1 < data_nb_1; ++counter1)
                     {
                        for (long counter2 = 0; counter2 < data_nb_2; ++counter2)
			{
                           typename storage_type::numeric_t tempvalue;
                           inputfile >> tempvalue;
                           local_data(counter1, counter2) = tempvalue;
			}
                     }
                     //std::cout << " .. # local data pattern: " << local_data << std::endl;
                     
                     store_quantity_direct(  static_cast<cell_handle>(index), quan_name,  local_data );
                  }
               } 
            }
            
            else
            {
               // Use the entry which was just read in for the next
               // segment
               
               seg_name = tempstr;
            }
         }
         
         //segment_container.push_back(segment);
         cell_counter += number_elements;



      }

      
      if (verbose)
         std::cout << "---------------------------------------------------"<<std::endl;
      return 0;
   }
  


   // not supportet right now
   //
   long read_native_gsse_file_hp(const std::string& filename)
   {
      std::cout << "#### not supported right now.. exit..!!! " << std::endl;
      return -1;



       const int dimension = topology_traits<topology_t>::dimension_tag::dim;
      
       std::ifstream file_in(filename.c_str(), std::ios::binary);

       if (!file_in.is_open())
           throw file_not_found("Unstructured domain:: file not found .. ");

       BOOST_STATIC_ASSERT(dimension < 4);
       BOOST_STATIC_ASSERT(dimension > 1);

       int tempdim;
       file_in >> tempdim;

       std::cout << "Dimension: " << tempdim << std::endl;

       if (tempdim != dimension)
           throw dimension_problem("Unstructured domain:: readin_simple_file_hp with wrong dimension");


       // ############################ geometry #########################
       // geometry must have only one entry.. one domain = one geometry
       //

       int number;
       file_in >> number;   
       //std::cout << "geom number: "<< number <<std::endl; 
       for (int nu=0; nu < number; ++nu)
       {
           std::vector<NumericTypeForCoordinates> coord;

           for (int dim = 0; dim < dimension; ++dim)
           {
               double tempcoord;
               file_in >> tempcoord;
               coord.push_back(tempcoord);
           }
           //std::cout << "temp: " << point_t(coord.begin(), coord.end()) <<std::endl;
           fast_point_insert(point_t(coord.begin(), coord.end()));    

       }

       // ############################ topology #########################
       int segment_size;
       file_in >> segment_size;
       //std::cout << "seg size: "<< segment_size; 
       for (int segit = 0; segit < segment_size; ++segit)
       {
           segment_type tempsegment;      // create one default topo rep inside the segment (implicit)
           std::string seg_name;
           file_in >> seg_name;

           tempsegment.set_name(seg_name);

           // process the segment vector vector
           //
           int number_elements,tempnn;
           file_in >> number_elements;
           file_in >> tempnn;
           std::cout << "number ele: " << number_elements << std::endl;
           std::cout << "tempnn: " << tempnn << std::endl;
           tempsegment.set_topo_size(tempnn);

           for (int nu=0; nu < number_elements; ++nu)
           {
               if (dimension== 2)
               {
                   int tmp, ind1, ind2, ind3;
                   int ve_i1, ve_i2, ve_i3;
                   file_in >> tmp;
                   file_in >> ind1 >> ve_i1 ;
                   file_in >> ind2 >> ve_i2 ;
                   file_in >> ind3 >> ve_i3 ;
                   //special insert with additional information
                   //
                   tempsegment.add_cell_3(cell_2_vertex_mapping(ind1, ind2, ind3),
                           cell_2_vertex_mapping(ve_i1, ve_i2, ve_i3));
               }
               else if (dimension == 3)
               {
                   int tmp, ind1, ind2, ind3, ind4;
                   int ve_i1, ve_i2, ve_i3, ve_i4;
                   file_in >> tmp;
                   file_in >> ind1 >> ve_i1 ;
                   file_in >> ind2 >> ve_i2 ;
                   file_in >> ind3 >> ve_i3 ;
                   file_in >> ind4 >> ve_i4;
                   // special insert with additional information
                   //
                   tempsegment.add_cell_3(cell_2_vertex_mapping(ind1, ind2, ind3, ind4),
                           cell_2_vertex_mapping(ve_i1, ve_i2, ve_i3,ve_i4));
               }
           }


           // ####### quantity ##############################

           int number_quan_all;
           file_in >> number_quan_all;
           for (int nqa=0; nqa < number_quan_all; ++nqa)
           {
               std::string quan_name;
               file_in >> quan_name;
               int number_quan;
               file_in >> number_quan; 

               for (int nu=0; nu < number_quan; ++nu)
               {
                   int tmpindex;
                   file_in >> tmpindex;	
                   double tempquan;
                   file_in >> tempquan;
                   //std::cout << "quan index: "<< tmpindex << "   quan: " << tempquan << std::endl; 
                   store_quantity(static_cast<vertex_handle>(tmpindex),
                           quan_name,
                           storage_type(1,1,tempquan));
               }
           }
           segment_container.push_back(tempsegment);
       }
       std::cout << "---------------------------------------------------"<<std::endl;
       return 0;
       
   }






   ///////////////////////////////////////////////////////////////////////////
   // gsse_writer
   ///////////////////////////////////////////////////////////////////////////


   long write_file(const std::string& filename, bool verbose = false)
   {
       std::string file_ending;
       std::string::size_type pos = filename.rfind(".")+1;
       //std::cout << "  filename: " << filename.size() << std::endl;
       //std::cout << "pos: "<< pos << std::endl;
       file_ending = filename.substr(pos); 
       //std::cout << "fileending: " << file_ending<< std::endl;
       //std::cout << " reduced  file: " << file_ending.substr(0,3) << std::endl;

       if (file_ending =="")
       {
          if (verbose)
             std::cout << "GSSE WRITE FILE: fileformat is not supported.. " << std::endl;
          return -1;          
       }        
       if (file_ending == "wss")
       {
          if (verbose)
             std::cout << "GSSE WRITE FILE: it is not recommended to write in a wss file..  please use gsse's ioconverter .. " << std::endl;
          return -1;
       }
       else if (file_ending.substr(0,3) == "gau")
       {
          if (verbose)
             std::cout << "### GSSE WRITE FILE: writing gsse unstructured file .... " << std::endl;
          return write_gsse_native_file(filename);
       }
       
       if (verbose)
          std::cout << "GSSE WRITE FILE: fileformat is not supported.. " << std::endl;
       return -1; 
   }
   
   

   long write_gsse_native_file(const std::string& filename)
   {
      const unsigned int dimension = topology_traits<topology_t>::dimension_tag::dim;
      //const unsigned int dimension_topology = topology_traits<topology_t>::dimension_topology;
      // dimension_geometry .. from domain class

    
      BOOST_STATIC_ASSERT(dimension >= 1);
      BOOST_STATIC_ASSERT(dimension <= 3);

      std::ofstream            file_out;
      std::string              newfilename(filename);
    
      file_out.open( newfilename.c_str() );
    
      // the file must handle the geometrical dimension
      // 
      file_out << dimension_geometry << std::endl;

      // ****************************************************************************************
      // *** begin of the POINTS section
      //
      file_out << point_size() << std::endl;
      file_out.setf(std::ios::right, std::ios::adjustfield);
    
   
      // get all geometrical points
      //
      global_point_iterator pl_it(global_point_list.begin()); 

      // output all points to the wss file
      //
      for (; pl_it != global_point_list.end(); ++pl_it)
      {
         //std::cout << (*pl_it).second << std::endl;
         file_out << std::setprecision(12) << std::setiosflags(std::ios::scientific) << (*pl_it).second << std::endl;
      }

     
      
      // ****************************************************************************************
      // *** begin of the SEGMENTS
      //
      vertex_iterator vit;
      segment_iterator seg_it;
 
     file_out << segment_size() << std::endl; 
    

     // =================================
     // *** begin of the segment::topology (GRID)
     //
     // [RH] new.. 200712 for domain quantities and global cell ids
     //
     long cell_counter = 0;
     //
     // ========
     
     long quan_size;
     for (seg_it = segment_begin(); seg_it != segment_end(); seg_it++)
     {
        // *** At the moment each segment has the name Segment + Name  (new [RH] 200801) 
        //
//	std::cout << "Segment: " << (*seg_it).get_segment_name()  << std::endl;

#ifdef GSSEV01
	file_out << "Segment: " << (*seg_it).get_segment_name()  << std::endl;   // [RH] new gsse style 
#endif
	file_out << (*seg_it).get_segment_name()  << std::endl;	
      	file_out << (*seg_it).cell_size() << std::endl;
	file_out << (*seg_it).vertex_size() << std::endl;

        cell_iterator cit;
	
        // [RH] new.. 200712 for domain quantities and global cell ids
        //
        long cell_index_number; //  =cell_counter;  
        cell_index_number = (*seg_it).get_cell_index_offset();
	 
        // *** iterate over all cells
        //
        for (cit = (*seg_it).cell_begin(); cit != (*seg_it).cell_end(); cit++,++cell_index_number) // 200712 new gsse domain / quantities
         {
       	    file_out << cell_index_number << "\t";
	 
	    vertex_on_cell_iterator vocit(*cit);

            // *** iterate over all interior vertices
            while (vocit.valid())
            {
               // *** output the handle of the cell elements
               file_out << "  " << (*vocit).handle();

               vocit++;
            }
            file_out << std::endl;
         }

         // **************************************************************************************
         // *** begin of the Quantities (ATTRIBUTES)
         //
         // ============================
         // write segment quantity
         //
         quan_size = (*seg_it).segment_key_size();
         if (quan_size != 0)
         {
            file_out << "Quantity: " << "segment " <<  quan_size << std::endl;
            typename segment_traits<segment_t>::segment_key_iterator skit = (*seg_it).segment_key_begin();
            for (; skit != (*seg_it).segment_key_end();++skit)
            {
               file_out << *skit << std::endl;
               file_out << "1" << std::endl;
               
               storage_type value;
               (*seg_it).retrieve_quantity(*skit, value);
               if (value.size_1()==0)
                  file_out << 0 << " " << 0.0 << std::endl;
               else
               file_out << 0 << " " << value(0, 0) << std::endl;
            }
         }
         // write segment string quantity
         //
         quan_size = (*seg_it).segment_skey_size();
         if (quan_size != 0)
         {
            file_out << "Quantity: " << "string-segment " <<  quan_size << std::endl;
            typename segment_traits<segment_t>::segment_skey_iterator skit = (*seg_it).segment_skey_begin();
            for (; skit != (*seg_it).segment_skey_end();++skit)
            {
               file_out << *skit << std::endl;
               file_out << "1" << std::endl;
               
               std::string value;
               (*seg_it).retrieve_quantity(*skit, value);
               file_out << 0 << " " << value << std::endl;
            }
         }


         // ============================
         // write vertex quantity
         //
         quan_size = (*seg_it).vertex_key_size();

         if (quan_size != 0)
         {
            file_out << "Quantity: " << "sheaf_vertex " <<  quan_size << std::endl;
//            std::cout  << "Quantity: " << "sheaf_vertex " <<  quan_size << std::endl;
//            typename base_domain_type::vertex_quantity_type::key_iterator vkit = vertex_key_begin();
            // [RH] 200801 ..  efficient segmented quantity storage
            //
            segment_vertex_quantity_iterator vkit = (*seg_it).vertex_key_begin();
            for (; vkit != (*seg_it).vertex_key_end();++vkit)
            {
	    	long size_1 = operator()(*((*seg_it).vertex_begin()), *vkit).size_1();
                long size_2 = operator()(*((*seg_it).vertex_begin()), *vkit).size_2();
		if (size_1 ==0)    // never use 0,0 as quantity type
			size_1 = 1;
		if (size_2 ==0)
			size_2 = 1;


               file_out << *vkit << " type "  
                        << size_1 << "  " 
                        << size_2 <<  std::endl;
        

               file_out << (*seg_it).vertex_size() << std::endl;
               typename segment_traits<segment_t>::vertex_iterator vit;
               for (vit = (*seg_it).vertex_begin(); vit != (*seg_it).vertex_end();++vit)
               {
                  storage_type value;
                  typename segment_traits<segment_t>::vertex_type ve;

                  try
                  {
                     retrieve_quantity(*vit, *vkit, value);
//                      if (value.size_1()==0)
//                         file_out << (*vit).handle() << " " << 0.0 << std::endl;
//                      else
                     file_out << (*vit).handle() << " " << value << std::endl;
                  }
                  catch(...)
                  {
                     file_out << (*vit).handle() << " " << 0.0 << std::endl;
                  }

               }               
            }
         }


         // ============================
         // write edge quantity
        
         quan_size = (*seg_it).edge_key_size();

         if (quan_size != 0)
         {
            // [RH][TODO] remove this temporary edge container
            //
            std::set<edge_type>  edge_container;
            typedef typename std::set<edge_type>::iterator  edge_iterator;
            for (cell_iterator c_it = (*seg_it).cell_begin(); c_it != (*seg_it).cell_end(); ++c_it)
            {
               for (edge_on_cell_iterator eoc_it(*c_it); eoc_it.valid(); ++eoc_it)
               {
                  edge_container.insert( *eoc_it );
               }
            }
            
            file_out << "Quantity: " << "sheaf_edge " <<  quan_size << std::endl;
            // [RH] 200801 ..  efficient segmented quantity storage
            //
            segment_edge_quantity_iterator ekit = (*seg_it).edge_key_begin();
            for (; ekit != (*seg_it).edge_key_end();++ekit)
//            typename base_domain_type::edge_quantity_type::key_iterator ekit = edge_key_begin();
//            for (; ekit != edge_key_end();++ekit)
            {
	    	long size_1 = operator()(*(edge_container.begin()), *ekit).size_1();
                long size_2 = operator()(*(edge_container.begin()), *ekit).size_2();
		if (size_1 ==0)    // never use 0,0 as quantity type
			size_1 = 1;
		if (size_2 ==0)
			size_2 = 1;


               file_out << *ekit << " type "  
                        << size_1 << "  " 
                        << size_2 <<  std::endl;
               file_out << edge_container.size() << std::endl;
               
               for (edge_iterator eit = edge_container.begin(); eit != edge_container.end();++eit)
               {
                  storage_type value;
                  
                  try
                  {
                     retrieve_quantity(*eit, *ekit, value);
                     if (value.size_1()==0)
                        file_out << (*eit).handle() << " " << 0.0 << std::endl;
                     else
                        file_out << (*eit).handle() << " " << value(0, 0) << std::endl;
                  }
                  catch(...)
                  {
                     file_out << (*eit).handle() << " " << value(0, 0) << std::endl;                     
                  }
               }
            }
         }
        
         // ============================
         // write facet quantity
        
         quan_size = (*seg_it).facet_key_size();
//         std::cout << "facet writer.. size: " << quan_size << std::endl;
         if (quan_size != 0)
         {
            // [RH][TODO] remove this temporary edge container
            //
            std::set<facet_type>  facet_container;
            typedef typename std::set<facet_type>::iterator  facet_iterator;
            for (cell_iterator c_it = (*seg_it).cell_begin(); c_it != (*seg_it).cell_end(); ++c_it)
            {
               for (facet_on_cell_iterator foc_it(*c_it); foc_it.valid(); ++foc_it)
               {
                  facet_container.insert( *foc_it );
               }
            }

            file_out << "Quantity: " << "sheaf_facet " <<  quan_size << std::endl;

            segment_facet_quantity_iterator fkit = (*seg_it).facet_key_begin();
            for (; fkit != (*seg_it).facet_key_end();++fkit)
//            typename base_domain_type::facet_quantity_type::key_iterator  fkit = facet_key_begin();
//            for (; fkit != facet_key_end();++fkit)
            {
	      file_out << *fkit << " type " 
 		       << operator()(*(facet_container.begin()), *fkit).size_1() << "  " 
 		       << operator()(*(facet_container.begin()), *fkit).size_2() <<  std::endl;

               file_out << facet_container.size() << std::endl;
               for (facet_iterator fit = facet_container.begin();  fit != facet_container.end();++fit)
               {
                  storage_type value;
                  
                  try
                  {
                     retrieve_quantity(*fit, *fkit, value);
                     if (value.size_1()==0)
                        file_out << (*fit).handle() << " " << 0.0 << std::endl;
                     else
                        file_out << (*fit).handle() << " " << value(0, 0) << std::endl;
                  }
                  catch(...)
                  {
                        file_out << (*fit).handle() << " " << 0.0 << std::endl;
                  }
               }
            }
         }

         // ============================
         // write cell quantity
        
         quan_size = (*seg_it).cell_key_size();
         if (quan_size != 0)
         {
            file_out << "Quantity: " << "sheaf_cell " <<  quan_size << std::endl;
            segment_cell_quantity_iterator ckit = (*seg_it).cell_key_begin();
            for (; ckit != (*seg_it).cell_key_end();++ckit)
//            typename base_domain_type::cell_quantity_type::key_iterator ckit = cell_key_begin();
//            for (; ckit != cell_key_end();++ckit)
            {
	      file_out << *ckit << " type " 
 		       << operator()(*((*seg_it).cell_begin()), *ckit).size_1() << "  " 
 		       << operator()(*((*seg_it).cell_begin()), *ckit).size_2() <<  std::endl;

               file_out << (*seg_it).cell_size() << std::endl;
               typename segment_traits<segment_t>::cell_iterator cit;
               for (cit = (*seg_it).cell_begin(); cit != (*seg_it).cell_end();++cit)
               {
                  storage_type value;
                  
                  try
                  {
                     retrieve_quantity(*cit, *ckit, value);
                     if (value.size_1()==0)
                        file_out << (*cit).handle() << " " << 0.0 << std::endl;
                     else
                        file_out << (*cit).handle() << " " << value(0, 0) << std::endl;
                  }
                  catch(...)
                  {
                     file_out << (*cit).handle() << " " << 0.0 << std::endl;                     
                  }
               }
            }
         }
         cell_counter +=(*seg_it).cell_size();     // 200712 new gsse domain / quantities
      }

      // **************************************************************************************
      // ***  Quantities (WSS::ATTRIBUTES)
      //
      // ============================
      // write domain quantity
      //
     
      quan_size = domain_quan.key_size();
      if (quan_size != 0)
      {
         file_out << "Quantity: " << "domain " <<  quan_size << std::endl;
         for (domain_key_iterator dkit = domain_quan.key_begin();
              dkit != domain_quan.key_end(); ++dkit)
         {
            file_out << *dkit << std::endl;
            file_out << "1" << std::endl;
            
            storage_type value;
            retrieve_domain_quantity(*dkit, value);
            if (value.size_1()==0)
               file_out << 0 << " " << 0.0 << std::endl;
            else
               file_out << 0 << " " << value(0, 0) << std::endl;
         }
      }


      file_out.close();
      
      return 0;
   }



 
   ///////////////////////////////////////////////////////////////////////////
   //
   //   DX writer
   //
   //   still in the development phase...   2005-07  .. V0.6
   //                                       2005-11  .. V0.7
   //  
   //   a complete domain is written into the dx format..
   // 
   //   different segments are marked by quantitites
   //
   ///////////////////////////////////////////////////////////////////////////

   void write_dx_file(const std::string& filename, std::string quan_name="")
   // if no quan_name is given, a default value "" is used
   //
   {
      vertex_iterator vit2;
      segment_iterator segit;

         
      std::ofstream            file_out;
      std::string tempfilename(filename);

      file_out.open(tempfilename.c_str());
            
      // ===============
      // geometry section
      //

      if (dimension_geometry == 2)
      {
         file_out << "object \"points\" class array type float rank 1 shape 2 items " << global_point_list.size() 
                  << " data follows" << std::endl;
      }
      else if (dimension_geometry == 3)
      {
         file_out << "object \"points\" class array type float rank 1 shape 3 items " << global_point_list.size()
                  << " data follows" << std::endl;
      }

      global_point_iterator   pl_it (global_point_list.begin());
      global_point_iterator   plend_it (global_point_list.end());
      long cnt(0);

      for (; pl_it != plend_it; ++pl_it, ++cnt)
      {
         file_out << (*pl_it).second   << "  \t\t\t # " << cnt << std::endl;
      }
      file_out << std::endl;

            
      // print total number of cells inside the domain ..
      //


      if (dimension_topology == 2)
      {
         // triangles
         file_out << "object \"grid_Line_One\" class array type int rank 1 shape 3 items " 
                  <<  cell_size() << " data follows" << std::endl;
      }
      else if (dimension_topology == 3)
      {
         // tetrahedra
         file_out << "object \"grid_Line_One\" class array type int rank 1 shape 4 items " 
                  << cell_size() << " data follows" << std::endl;
      }

      for (segit = segment_begin(); segit != segment_end(); ++segit)
      {
         cell_iterator cit;
         for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); cit++)
         {
            vertex_on_cell_iterator vocit (*cit);
                  
            while (vocit.valid())
            {
               file_out << (*vocit).handle() << "  ";
               ++vocit;
            }
            file_out << "   #" << (*cit).handle() << std::endl;
         }
      }

      if (dimension_topology == 2)
      {
         // triangles
         file_out << "attribute \"element type\" string \"triangles\" " << std::endl;
      }
      else if (dimension_topology == 3)
      {
         // tetrahedra
         file_out << "attribute \"element type\" string \"tetrahedra\" " << std::endl;
      }
      file_out << "attribute \"ref\" string \"positions\" " << std::endl;
      file_out << std::endl;
      // =============================
            

      // =============================
      // quantity section
      //
      file_out << "object \""<< quan_name << "\" class array items " <<global_point_list.size()  << " data follows" << std::endl;
      
      // TODO !! HACK  !!
      std::vector<double> tempquan_container(global_point_list.size());


      int i=0;
      for (segit = segment_begin(); segit != segment_end(); ++segit)
      {
         for (vit2 = (*segit).vertex_begin(); vit2 != (*segit).vertex_end(); vit2++)
         {
            storage_type    value;

            if (quan_name == "")
            {
               value = storage_type();
            }
            else
            {
              retrieve_quantity( (*vit2), quan_name, value); 
            }
            if (value == storage_type() )
            {
               tempquan_container[(*vit2).handle()] = 0.0;
            }
            else
            {
               tempquan_container[(*vit2).handle()] = value(0,0);
            }
         }
         i++;
      }
      for (unsigned int i = 0; i < tempquan_container.size(); i++)
      {
         file_out << tempquan_container[i] << "   #" << i << std::endl;
      }
      file_out << std::endl;


      file_out << "attribute \"dep\" string \"positions\"" << std::endl;
      file_out << std::endl;
      // ===============
            

      file_out << "object \"AttPotential\" class field " << std::endl;
      file_out << "component \"data\" \""<<quan_name<<"\" " << std::endl;
      file_out << "component \"positions\" \"points\""  << std::endl;
      file_out << "component \"connections\" \"grid_Line_One\"" << std::endl;

      file_out.close();

   }


   ///////////////////////////////////////////////////////////////////////////
   //
   //   DX writer cell
   //
   ///////////////////////////////////////////////////////////////////////////

   void write_dx_file_cell(const std::string& filename, std::string quan_name="")
   // if no quan_name is given, a default value "" is used
   //
   {
      vertex_iterator vit2;
      segment_iterator segit;
         
      std::ofstream            file_out;
      std::string tempfilename(filename);

      file_out.open(tempfilename.c_str());
            
      // ===============
      // geometry section
      //

      if (dimension_geometry == 2)
      {
         file_out << "object \"points\" class array type float rank 1 shape 2 items " << global_point_list.size() 
                  << " data follows" << std::endl;
      }
      else if (dimension_geometry == 3)
      {
         file_out << "object \"points\" class array type float rank 1 shape 3 items " << global_point_list.size()
                  << " data follows" << std::endl;
      }

      global_point_iterator   pl_it (global_point_list.begin());
      global_point_iterator   plend_it (global_point_list.end());
      long cnt(0);
      for (; pl_it != plend_it; ++pl_it, ++cnt)
      {
         file_out << (*pl_it).second   << "  \t\t\t # " << cnt << std::endl;
      }
      file_out << std::endl;
            

            
      // ===============
      // topology section
      //

      // print total number of cells inside the domain ..
      //


      if (dimension_topology == 2)
      {
         // triangles
         file_out << "object \"grid_Line_One\" class array type int rank 1 shape 3 items " 
                  <<  cell_size() << " data follows" << std::endl;
      }
      else if (dimension_topology == 3)
      {
         // tetrahedra
         file_out << "object \"grid_Line_One\" class array type int rank 1 shape 4 items " 
                  << cell_size() << " data follows" << std::endl;
      }

      for (segit = segment_begin(); segit != segment_end(); ++segit)
      {
         cell_iterator cit;
         for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); cit++)
         {
            vertex_on_cell_iterator vocit (*cit);
                  
            while (vocit.valid())
            {
               file_out << (*vocit).handle() << "  ";
               ++vocit;
            }
            file_out << "   #" << (*cit).handle() << std::endl;
         }
      }

      if (dimension_topology == 2)
      {
         // triangles
         file_out << "attribute \"element type\" string \"triangles\" " << std::endl;
      }
      else if (dimension_topology == 3)
      {
         // tetrahedra
         file_out << "attribute \"element type\" string \"tetrahedra\" " << std::endl;
      }
      file_out << "attribute \"ref\" string \"positions\" " << std::endl;
      file_out << std::endl;
      // =============================
            

      // ===============
      // quantity section
      //
      file_out << "object \""<< quan_name << "\" class array items " << cell_size()  << " data follows" << std::endl;
      for (segit = segment_begin(); segit != segment_end(); ++segit)
      {
         cell_iterator cit;
         for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); cit++)
         {
            storage_type value;
            retrieve_quantity( (*cit), quan_name, value);
            file_out << value;
            file_out << "   #" << (*cit).handle() << std::endl;
         }
      }


//       // TODO !! HACK  !!
//       std::vector<double> tempquan_container(cell_size());

//       for (segit = segment_begin(); segit != segment_end(); ++segit)
//       {
//          cell_iterator cit;

//          for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); cit++)
//          {
//             typename segment_traits<segment_t>::storage_type value;
//             if (quan_name == "")
//             {
//                value = typename segment_traits<segment_t>::storage_type();
//             }
//             else
//             {
//                (*segit).retrieve_quantity( (*cit), quan_name, value);
//             }
//             if (value == typename segment_traits<segment_t>::storage_type() )
//                tempquan_container[(*cit).handle()] = 0.0;
//             //                      file_out << typename segment_traits<segment_t>::storage_type(1,1,0.0) << std::endl;
//             else
//                tempquan_container[(*cit).handle()] = value(0,0);

//          }
//       }

//       for (unsigned int i = 0; i < tempquan_container.size(); i++)
//       {
//          file_out << tempquan_container[i] << "   #" << i << std::endl;
//       }
//       file_out << std::endl;


      file_out << "attribute \"dep\" string \"connections\"" << std::endl;
      file_out << std::endl;
      // ===============
            

      file_out << "object \"AttPotential\" class field " << std::endl;
      file_out << "component \"data\" \""<<quan_name<<"\" " << std::endl;
      file_out << "component \"positions\" \"points\""  << std::endl;
      file_out << "component \"connections\" \"grid_Line_One\"" << std::endl;

      file_out.close();

   }



   ///////////////////////////////////////////////////////////////////////////
   //
   //   STL geometry file writer
   // 
   //   for one segment only   .. only for 3d geometry.. 2d topology
   //
   ///////////////////////////////////////////////////////////////////////////

   void write_stl_file(const std::string& filename)
  {
    
    segment_iterator seg_it;
    std::ofstream            file_out;
    
    if (dimension_geometry == 2)
      {
	std::cout << "Error at STL writer.. cannot be used with 2D geometry " << std::endl;
	return ;
      }
    
    file_out.open(filename.c_str());
    seg_it = segment_begin();

    file_out << "solid " << std::endl;

    
    // traverse all cells
    //
    cell_iterator cit;
    for (cit = (*seg_it).cell_begin(); cit != (*seg_it).cell_end(); cit++)
      {
	
	typename gsse::segment_traits<segment_t>::vertex_on_cell_iterator vocit(*cit);
	file_out << "   facet normal 0.0 0.0 0.0 " << std::endl 
	      << "     outer loop  " << std::endl;
	
	for (; vocit.valid(); ++vocit)
	  {
	    point_t point = get_point( *vocit );
	    file_out << "      vertex " << point << std::endl;
	  }
	file_out << "     endloop " << std::endl;
	file_out << "   endfacet " << std::endl;
      }
    file_out << "endsolid" << std::endl;
    file_out.close();
  }
  



// ######################################################################

   template<typename DomainT>
   struct traverse_segments
   {
      typedef typename DomainT::segment_iterator iterator;
      typedef typename DomainT::segment_type     value_type;
      
      traverse_segments(DomainT& domain) : domain(domain) {}

      iterator begin() { return domain.segment_begin();}
      iterator end()   { return domain.segment_end(); }
   protected:
      DomainT& domain;
   };

   typedef boost::mpl::map<
        boost::mpl::pair<boost::mpl::pair<self_t, segment_type>, traverse_segments<self_t> >
      > traversal_repository;




};

}   // detail namespace
}   // gsse namespace

#endif




// unsigned int DIM=topology_traits<typename segment_traits<SegmentT>::topology_t>::dimension_tag::dim>    // gcc 4.1.1 . .internal compiler error ;-( 
//	  unsigned int DIM=topology_traits<typename segment_traits<SegmentT>::topology_t>::dimension_topology >


