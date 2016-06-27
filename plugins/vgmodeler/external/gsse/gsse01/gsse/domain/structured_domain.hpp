/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_STRUCTURED_DOMAIN_HH_ID
#define GSSE_STRUCTURED_DOMAIN_HH_ID

// *** BOOST includes
//
#include <boost/iterator/iterator_facade.hpp>


// *** GSSE includes
//
#include "gsse/domain/base_domain.hpp"
#include "gsse/segment.hpp"
#include "gsse/domain/structured_pointlist.hpp"
#include "gsse/math/geometric_point.hpp"

// *** system includes
//
#include <iomanip>


namespace gsse
{
namespace detail_domain
{

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
// Base domain: structured mesh, ND
//
//
// for now (2005-10-17), 
//    the structured domain has only one structured segment
//
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


template <  typename NumericTypeForCoordinates, 
            typename SegmentT,
            typename NumericTQuantity,
            typename IdentifierTQuantity,
            unsigned int DIM=topology_traits<typename segment_traits<SegmentT>::topology_t>::dimension_topology > 
class structured_domain  : public base_domain<typename segment_traits<SegmentT>::topology_t, NumericTQuantity,IdentifierTQuantity>

{

   typedef base_domain<typename segment_traits<SegmentT>::topology_t, NumericTQuantity,IdentifierTQuantity>           base_domain_type;
   typedef structured_domain<NumericTypeForCoordinates, SegmentT, NumericTQuantity, IdentifierTQuantity, DIM>         self_t;


public:
   typedef SegmentT                                                              segment_t;
   typedef SegmentT                                                              segment_type;
   typedef typename segment_traits<segment_t>::topology_t                        topology_t;
   typedef typename segment_traits<segment_t>::quantity_type		         quantity_type;
   typedef typename segment_traits<segment_t>::storage_type                      storage_type;


   typedef typename topology_traits<topology_t>::cell_type                       cell_type;
   typedef typename topology_traits<topology_t>::vertex_type                     vertex_type;
   typedef typename topology_traits<topology_t>::edge_type                       edge_type;        
   typedef typename topology_traits<topology_t>::facet_type                      facet_type;        
   
   typedef typename topology_traits<topology_t>::cell_iterator                   cell_iterator;
   typedef typename topology_traits<topology_t>::vertex_iterator                 vertex_iterator;

   typedef typename topology_traits<topology_t>::edge_on_cell_iterator           edge_on_cell_iterator;
   typedef typename topology_traits<topology_t>::edge_on_vertex_iterator         edge_on_vertex_iterator;
   typedef typename topology_traits<topology_t>::vertex_on_edge_iterator         vertex_on_edge_iterator;
   typedef typename topology_traits<topology_t>::vertex_on_cell_iterator         vertex_on_cell_iterator;
   typedef typename topology_traits<topology_t>::cell_on_edge_iterator           cell_on_edge_iterator;
   typedef typename topology_traits<topology_t>::cell_on_vertex_iterator         cell_on_vertex_iterator;
   typedef typename topology_traits<topology_t>::facet_on_cell_iterator          facet_on_cell_iterator;

   typedef typename topology_traits<topology_t>::vertex_handle                   vertex_handle;
   typedef typename topology_traits<topology_t>::edge_handle                     edge_handle;
   typedef typename topology_traits<topology_t>::facet_handle                    facet_handle;
   typedef typename topology_traits<topology_t>::cell_handle                     cell_handle;
   typedef typename topology_traits<topology_t>::direction_n_iterator            direction_n_iterator;

   typedef typename topology_traits<topology_t>::dimension_tag                   dimension_tag;
   typedef typename topology_traits<topology_t>::dimension_topology_tag          dimension_topology_tag;

   typedef typename base_domain_type::vertex_quantity_type::key_iterator                vertex_key_iterator;
   typedef typename base_domain_type::edge_quantity_type::key_iterator                  edge_key_iterator;
   typedef typename base_domain_type::facet_quantity_type::key_iterator                 facet_key_iterator;
   typedef typename base_domain_type::cell_quantity_type::key_iterator                  cell_key_iterator;

   typedef typename segment_traits<segment_t>::segment_vertex_quantity_iterator   segment_vertex_quantity_iterator;
   typedef typename segment_traits<segment_t>::segment_edge_quantity_iterator     segment_edge_quantity_iterator;
   typedef typename segment_traits<segment_t>::segment_facet_quantity_iterator    segment_facet_quantity_iterator;
   typedef typename segment_traits<segment_t>::segment_cell_quantity_iterator     segment_cell_quantity_iterator;



   static const unsigned int dimension = topology_traits<topology_t>::dimension_tag::dim;
   static const unsigned int dimension_topology  = topology_traits<topology_t>::dimension_tag::dim;
   static const unsigned int dimension_geometry = DIM;


   ///////////////////////////////////////////////////////////////////////////
   //
   // here we store the segment information
   // - iterators can be derived from the container iterator
   // - the following things are stored within the segments
   //
   ///////////////////////////////////////////////////////////////////////////


   friend class structured_segment_iterator;
   //
   // todo.. make this pointers const 
   //
   // ==============================================================================

   class structured_segment_iterator
      : public boost::iterator_facade<
      structured_segment_iterator                      // Derived
      , segment_t                                      // Base
      , boost::bidirectional_traversal_tag             // CategoryOrTraversal     
      , segment_t&                                     // Base
      , unsigned int
      >
   {
      typedef structured_segment_iterator self;


      friend class boost::iterator_core_access;

   public:
 
      typedef structured_domain<NumericTypeForCoordinates, SegmentT, NumericTQuantity, IdentifierTQuantity, DIM> base_type;

     structured_segment_iterator() : segment(0), valid_(false), base_() {}

      explicit 
      structured_segment_iterator(segment_t* segment, bool valid_, base_type const& b) 
         : segment(segment), valid_(valid_), base_(&b) {}
	     
      void increment() { valid_ = false; }
      void decrement() { valid_ = true; }
      void advance(unsigned int i )  { valid_ = (i == 0);}
     unsigned int distance_to (self const& other) const
     { return static_cast<unsigned int>(valid_) - static_cast<unsigned int>(other.valid_); }

      // TODO .. check the segment equality

     bool equal(const self& other) const 
     { return ( segment == other.segment && valid() == other.valid()); }

     segment_t& dereference()      const { return *segment; }               
     bool valid()                  const { return valid_; }
     
     friend std::ostream& operator<<(std::ostream& ostr, const self& me)
     {
       ostr << "seg it: " << me.valid_;
       return ostr;
     }
     
     void reset() {valid_ = true;}
     base_type const& base() const { return *base_;}
     
   private:
      segment_t* segment;
      bool valid_;
      base_type const* base_;
   };


  class structured_segment_on_vertex_iterator
      : public boost::iterator_facade<
    structured_segment_on_vertex_iterator                      // Derived
      , structured_segment_iterator                               // Base
      , boost::bidirectional_traversal_tag             // CategoryOrTraversal     
      , structured_segment_iterator                               // Base
      >
  {
     typedef structured_segment_on_vertex_iterator self;
     
     
     friend class boost::iterator_core_access;

   public:

     typedef vertex_type base_type;


    structured_segment_on_vertex_iterator() {}

    explicit
    structured_segment_on_vertex_iterator
    (structured_segment_iterator seg_it, base_type const& b) : 
      seg_it(seg_it), b(b) 
    {
      available = true;
      valid_ = true;
    }

      // This is the segment on vertex iterator interface
      // requirement. Of course the vertex does not really play an
      // important role here. [MS]
	     
    void increment() { valid_ = false; }
    void decrement() { if (available) valid_ = true; }

      // TODO .. check the segment equality

     bool equal(const self& other) const 
     { return ( seg_it == other.seg_it && valid() == other.valid()); }
     
    structured_segment_iterator dereference()  
       const { return seg_it; }  

    bool valid()                    const { return valid_; }
     
     friend std::ostream& operator<<(std::ostream& ostr, const self& me)
     {
       ostr << "seg it: " << me.valid_;
       return ostr;
     }
     
     void reset() { if (available) valid_ = true; valid_ = true;}
     const base_type& base() const { return b;}
     
   private:
    structured_segment_iterator seg_it;
    bool valid_;
    bool available;
    base_type b;
   };


public:

  typedef typename segment_traits<segment_t>::quan_key_t    quan_key_t; 

   // [RH] 2007/11:: global domain quantity
   //
   typedef gsse_quantity_sparse<long, quan_key_t, storage_type>         domain_quan_type;
   typedef typename quantity_traits<domain_quan_type>::key_iterator    domain_key_iterator;
   domain_quan_type                                                    domain_quan;

  typedef structured_segment_iterator           segment_iterator;
  typedef structured_segment_on_vertex_iterator segment_on_vertex_iterator;


   ///////////////////////////////////////////////////////////////////////////
   //
   // geometry part
   //  
   ///////////////////////////////////////////////////////////////////////////

//   typedef geometry<topology_t, typename topology_traits<topology_t>::topology_tag, 
//                    NumericTypeForCoordinates, segment_t> 
//   geometry_t;      


   static const long dim = topology_traits<topology_t>::dimension;

   typedef generic_point<NumericTypeForCoordinates, dimension_geometry> point_t;

   typedef structured_pointlist<topology_t, point_t> pointlist_type;

public:

  structured_domain()   {      current_segment=-1;    }
             
   // *** Wrapper methods
    structured_segment_iterator  segment_begin()  
   { return structured_segment_iterator(&segment, true, *this); }
    structured_segment_iterator  segment_end()    
   { return structured_segment_iterator(&segment, false, *this);}
   unsigned int segment_size() const{return 1;}

                          
public:

   point_t get_point(const vertex_type& ve)  const
      { return global_point_list[ve]; }

  point_t const& get_point_fromhandle(const vertex_handle& vh) const 
  { 
    return global_point_list [ segment.retrieve_topology().get_vertex_from_handle(vh)];
  }


   // [RH][TODO].. remove this.. depricated interface
   //
   // additional mechanism to resize a structured domain
   //
   //
   void add_point(const point_t& pt, segment_iterator seg_it) 
   {
      // separate point into n-dimensions
      // add each coordinate into a dimension
      for (unsigned int i = 0; i < dimension_tag::dim; ++i)
      {
         //                   std::cout << "dim size: "<< global_point_list.get_dim_n(i+1) << std::endl;
         global_point_list.insert_dim_tick(i+1,pt[i]);
         //                   std::cout << "dim size: "<< global_point_list.get_dim_n(i+1) << std::endl;
                  
         (*seg_it).retrieve_topology().resize_dim_n(global_point_list.get_dim_n(i+1),i+1);
      }
   }


   // ##############################################################################################
   //
   // *** register quantity
   //
   void register_vertex_quantity(const quan_key_t& quan_k, const vertex_handle& vh)
      {
         segment_iterator seg_it=segment_begin();
         (*seg_it).add_vertex_quantity(quan_k);
      }
    void register_edge_quantity(const quan_key_t& quan_k, const edge_handle& eh)
      {
         segment_iterator seg_it=segment_begin();
         (*seg_it).add_edge_quantity(quan_k);
      }
      void register_facet_quantity(const quan_key_t& quan_k, const facet_handle& fh)
      {
         segment_iterator seg_it=segment_begin();
         (*seg_it).add_facet_quantity(quan_k);
      }
    void register_cell_quantity(const quan_key_t& quan_k, const cell_handle& ch)
      {
         segment_iterator seg_it=segment_begin();
         (*seg_it).add_cell_quantity(quan_k);
      }
 

// ===========================================
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
         store_quantity_notsafe(object, key, value);    
      }
   void store_quantity_safe(const edge_type& object, const edge_key_type& key, const storage_edge_type& value)
      {  
         register_edge_quantity(key, object.handle());
         store_quantity_notsafe(object, key, value);    
      }
   void store_quantity_safe(const cell_type& object, const cell_key_type& key, const storage_cell_type& value)
      {  
         register_cell_quantity(key, object.handle());
         store_quantity_notsafe(object, key, value);    
      }


// ===========================================



   // additional mechanism to resize a structured domain
   //
   //
   void add_point_causes_invalidated_handles(const point_t& pt, segment_iterator seg_it) 
   {
      // separate point into n-dimensions
      // add each coordinate into a dimension
      for (unsigned int i = 0; i < dimension_tag::dim; ++i)
      {
         //                   std::cout << "dim size: "<< global_point_list.get_dim_n(i+1) << std::endl;
         global_point_list.insert_dim_tick(i+1,pt[i]);
         //                   std::cout << "dim size: "<< global_point_list.get_dim_n(i+1) << std::endl;
                  
         (*seg_it).retrieve_topology().resize_dim_n(global_point_list.get_dim_n(i+1),i+1);
      }
   }


private:
   pointlist_type  global_point_list;
   segment_t       segment;
   int             current_segment;



public:

   // [RH] ToDo
   class global_point_iterator {};

   pointlist_type & retrieve_pointlist() {return global_point_list;}

   // [RH] .. since GCC 3.4.. dependent lookup is implemented correctly and
   //   the base_domain functions have to be forwarded 
   //
   using base_domain_type::retrieve_quantity;
   using base_domain_type::store_quantity;
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


   ///////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////
   //
   // IO methods

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
      long number_of_cells(  segment.retrieve_topology().cell_size() );
      return number_of_cells;
   }
   
	//
	// [JW] added additional parameter for offering the same interface as unstruct ...
	//
   void read_file(const std::string& filename, bool verbose = true)
   {
      //static const int dimension = topology_traits<topology_t>::dimension_tag::dim;
     
      ///////////////////////////////////////////////////////////////////////////
      //
      // first, we build up the domain structures
      //  
      ///////////////////////////////////////////////////////////////////////////

      std::ifstream inputfile(filename.c_str());

      if (!inputfile.is_open())
         throw file_not_found("Structured domain:: file not found .. ");
         
      std::string temp1;
      unsigned int file_dim;
      std::vector<int> dim_v;

      inputfile >> temp1 >> file_dim;

      //
      // check for dimension
      //
      if (file_dim != dimension_tag::dim)                                // here dimension dependent
         throw dimension_problem("Structured domain:: readin_file with wrong dimension");
         
      for (unsigned int i = 0; i < dimension_tag::dim; i ++)
      {
         int tempdim;
         inputfile >> tempdim;
         dim_v.push_back(tempdim);
      }
         
      std::vector<std::vector<NumericTypeForCoordinates> > dimticks(dimension_tag::dim);
      std::string inputticks;
      inputfile >> inputticks;

      //                std::cout << "input: " << inputticks << std::endl;
      if (inputticks == "linearvalues")
      {
         for (unsigned int i = 0; i < dimension_tag::dim; i++)
         {
            for (int j = 0; j < dim_v[i]; j++) 
            { 
               double temptick;
               inputfile >> temptick;
               dimticks[i].push_back(temptick ); 
            }
         }
            
      }

      else if (inputticks == "tickvalues")
      {
         // read the point of origin
         //
         std::vector<NumericTypeForCoordinates> offset;
         for (unsigned int i = 0; i <  dimension_tag::dim; i++)
         {
            double temptick;
            inputfile >> temptick;
            offset.push_back(temptick);
         }

         for (unsigned int i = 0; i < dimension_tag::dim; i++)
         {
            double temptick;
            inputfile >> temptick;
            for (int j = 0; j < dim_v[i]; j++) 
            { 
               dimticks[i].push_back( offset[i] + j * temptick ); 
            }
         }

      }
      else 
      {
         // throw an exception
         exit(-1);
      }
         
      segment.retrieve_topology() = topology_t(dim_v.begin(), dim_v.end());                                 
      global_point_list           = pointlist_type(dimticks);
        
      // quantity section.. simple reader for scalar quantities
      //  
      //
      //

      std::string tempstr;
      std::string seg_name;
	structured_segment_iterator seg_it = segment_begin();

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
				      (*seg_it).store_quantity( tempstr, storage_type(1,1,tempval2));
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
				      (*seg_it).store_quantity( tempstr,  tempval2);
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
			      //                   std::cout << "    ## sheaf vertex .. quanname: "<< quan_name << std::endl;          

			      // [RH] 200801 .. efficient storage of quantities
			      //   the actual quantity values are stored within the domain
			      //   the key information is stored in the respective segment
			      //
			      (*seg_it).add_vertex_quantity(quan_name);

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
			      (*seg_it).add_edge_quantity(quan_name);
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
				      long index;
				      double tempval2;
				      inputfile >> index;
				      inputfile >> tempval2;
				      //std::cout << "    val: " << tempval2 << std::endl;
				      store_quantity_direct(  static_cast<facet_handle>(index), tempstr,  storage_type(1,1,tempval2));

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
			      (*seg_it).add_facet_quantity(quan_name);
			      //  std::cout << "    ## sheaf facet .. quanname: "<< quan_name << std::endl;          

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

				      store_quantity_direct(  static_cast<facet_handle>(index), quan_name,  local_data );
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
			      (*seg_it).add_cell_quantity(quan_name);

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
   }

   void write_file(const std::string& filename)
   {  
      //static const int dimension = topology_traits<topology_t>::dimension_tag::dim;
     
      std::ofstream file_out(filename.c_str());
      file_out << std::setprecision(12) << std::setiosflags(std::ios::scientific);


      segment_iterator seg_it;

      // ============================
      // geometry
      //

      long dimension;
      dimension = segment.retrieve_topology().get_dimension();
      file_out << "dim: " << dimension << std::endl;
      for (long dim = 1; dim <= dimension; ++dim)
      {
         file_out << segment.retrieve_topology().get_dim_n_size(dim) << std::endl;
      }
         
      file_out << "linearvalues" << std::endl;
      for (long dim =0; dim < dimension; ++dim)
      {
         //std::cout << "dim :  "<< dim << std::endl;
                
         std::vector<NumericTypeForCoordinates> tempc = global_point_list.get_dim_n_coordinates(dim);
         //std::cout << "  size: "<< tempc.size() << std::endl;
         for (unsigned int veci=0; veci<tempc.size();++veci)
         {
            file_out << tempc[veci] << std::endl;
            //std::cout << "   i: "<< veci << std::endl;
         }
      }  
         
     
      // ============================
      // write domain quantity
      //
      
      long quan_size;
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
      

      // ============================
      // write segment quantity
      //

      quan_size = segment.segment_key_size();
      if (quan_size > 0)
      {
         file_out << "Quantity: " << "segment " <<  quan_size << std::endl;
         
         typename segment_traits<segment_t>::segment_key_iterator  skit = segment.segment_key_begin();
         for (; skit != segment.segment_key_end(); ++skit)
         {
            file_out << *skit << std::endl;
            file_out << "1" << std::endl;
         
            storage_type value;
            
            segment.retrieve_quantity(*skit, value);
            
            if (value.size_1()==0)
               file_out << 0 << " " << 0.0 << std::endl;
            else
               file_out << 0 << " " << value() << std::endl;
         }
      }
         
      // ============================
      // write vertex quantity
      //
      
      quan_size = segment.vertex_key_size();
      if (quan_size > 0)
      {
         file_out << "Quantity: " << "sheaf_vertex " <<  quan_size << std::endl;
         vertex_key_iterator vkit = vertex_key_begin();
         for (; vkit != vertex_key_end();++vkit)
         {
	   file_out << *vkit << " type " 
		      << operator()(*segment.vertex_begin(), *vkit).size_1() << "  " 
		      << operator()(*segment.vertex_begin(), *vkit).size_2() <<  std::endl;
            file_out << segment.vertex_size() << std::endl;
            typename segment_traits<segment_t>::vertex_iterator vit;
            for (vit = segment.vertex_begin(); vit != segment.vertex_end();++vit)
            {
               storage_type value;
               typename segment_traits<segment_t>::vertex_type ve;
               
               retrieve_quantity(*vit, *vkit, value);
               if (value.size_1()==0)
                  file_out << (*vit).handle() << " " << 0.0 << std::endl;
               else
                  file_out << (*vit).handle() << " " << value << std::endl;
            }
         }
      }

      // ============================
      // write edge quantity
         quan_size = segment.edge_key_size();

         if (quan_size != 0)
         {
            // [RH][TODO] remove this temporary edge container
            //
            std::set<edge_type>  edge_container;
            typedef typename std::set<edge_type>::iterator  edge_iterator;
            for (cell_iterator c_it = segment.cell_begin(); c_it != segment.cell_end(); ++c_it)
            {
               for (edge_on_cell_iterator eoc_it(*c_it); eoc_it.valid(); ++eoc_it)
               {
                  edge_container.insert( *eoc_it );
               }
            }
             file_out << "Quantity: " << "sheaf_edge " <<  quan_size << std::endl;
            // [RH] 200801 ..  efficient segmented quantity storage
            //
            segment_edge_quantity_iterator ekit = segment.edge_key_begin();
            for (; ekit != segment.edge_key_end();++ekit)
//            typename base_domain_type::edge_quantity_type::key_iterator ekit = edge_key_begin();
//            for (; ekit != edge_key_end();++ekit)
            {
               file_out << *ekit << " type "  
                        << operator()(*(edge_container.begin()), *ekit).size_1() << "  " 
                        << operator()(*(edge_container.begin()), *ekit).size_2() <<  std::endl;
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
        
         quan_size = segment.facet_key_size();
         if (quan_size != 0)
         {
            // [RH][TODO] remove this temporary edge container
            //
            std::set<facet_type>  facet_container;
            typedef typename std::set<facet_type>::iterator  facet_iterator;
            for (cell_iterator c_it = segment.cell_begin(); c_it != segment.cell_end(); ++c_it)
            {
               for (facet_on_cell_iterator foc_it(*c_it); foc_it.valid(); ++foc_it)
               {
                  facet_container.insert( *foc_it );
               }
            }

            file_out << "Quantity: " << "sheaf_facet " <<  quan_size << std::endl;

            segment_facet_quantity_iterator fkit = segment.facet_key_begin();
            for (; fkit != segment.facet_key_end();++fkit)
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

           
 
//          file_out << "Quantity: " << "sheaf_edge " <<  quan_size << std::endl;
//          edge_key_iterator   ekit = edge_key_begin();
//          for (; ekit != edge_key_end();++ekit)
//          {
// 	   file_out << *ekit << " type " 
// 		      << operator()(*segment.edge_begin(), *ekit).size_1() << "  " 
// 		      << operator()(*segment.edge_begin(), *ekit).size_2() <<  std::endl;

//             file_out << segment.edge_size() << std::endl;
//             typename segment_traits<segment_t>::edge_iterator eit;
//             for (eit = segment.edge_begin(); eit != segment.edge_end();++eit)
//             {
//                storage_type value;
//                std::cout << "..  loop .. " <<  std::endl;

//                // debug .. 
// //                 std::cout << "edge handle1: " < <(*eit).handle() << std::endl;
// //                std::cout << "edge handle2: " < <(*eit).handle2() << std::endl;
// //                std::cout << "edge       : " < <(*eit) << std::endl;
               
//                retrieve_quantity(*eit, *ekit, value);

//                std::cout << "..  loop 2.. " <<  std::endl;

//                if (value.size_1()==0)
//                   file_out << (*eit).handle() << " " << 0.0 << std::endl;
//                else
//                   file_out << (*eit).handle() << " " << value << std::endl;
//             }
//          }
//       }
      
      // ============================
      // write facet quantity
         
//       quan_size = facet_key_size();
//       if (quan_size > 0)
//       {
//          file_out << "Quantity: " << "sheaf_facet " <<  quan_size << std::endl;
//          facet_key_iterator   fkit = facet_key_begin();
//          for (; fkit != facet_key_end();++fkit)
//          {
// 	   file_out << *fkit << " type " 
// 		      << operator()(*segment.facet_begin(), *fkit).size_1() << "  " 
// 		      << operator()(*segment.facet_begin(), *fkit).size_2() <<  std::endl;

//             file_out << segment.facet_size() << std::endl;
//             typename segment_traits<segment_t>::facet_iterator fit;
//             for (fit = segment.facet_begin(); fit != segment.facet_end();++fit)
//             {
//                storage_type value;
               
//                retrieve_quantity(*fit, *fkit, value);
//                if (value.size_1()==0)
//                   file_out << (*fit).handle() << " " << 0.0 << std::endl;
//                else
//                   file_out << (*fit).handle() << " " << value << std::endl;
//             }
//          }
//       }
      
      // ============================
      // write cell quantity
      
      quan_size = cell_key_size();
      if (quan_size > 0)
      {
         file_out << "Quantity: " << "sheaf_cell " <<  quan_size << std::endl;
         cell_key_iterator  ckit = cell_key_begin();
         for (; ckit != cell_key_end();++ckit)
         {
	   file_out << *ckit << " type " 
                    << operator()(*segment.cell_begin(), *ckit).size_1() << "  " 
                    << operator()(*segment.cell_begin(), *ckit).size_2() <<  std::endl;

            file_out << segment.cell_size() << std::endl;
            typename segment_traits<segment_t>::cell_iterator cit;
            for (cit = segment.cell_begin(); cit != segment.cell_end();++cit)
            {
               storage_type value;
               
               retrieve_quantity(*cit, *ckit, value);
               if (value.size_1()==0)
                  file_out << (*cit).handle() << " " << 0.0 << std::endl;
               else
                  file_out << (*cit).handle() << " " << value << std::endl;
            }
         }            
      }
   } 

   // /////////////////////////////////////////////////////////////////////////
   //
   //
   void write_dx_file(const std::string& filename, const std::string& quan_name)
   {
      
      vertex_iterator vit2;
      segment_iterator seg_it;
         
      std::ofstream            file_out;
      std::string tempfilename(filename);

      file_out.open(tempfilename.c_str());
      // ==================================================================
      // ====   geometry section
      //
      // object 3 class array type float rank 0 items 24 data follows
      //            1          3.4            5            2
      //            3.4        5.1            0.3          4.5
      //            1          2.3            4.1          2.1
      //            6            8            9.1          2.3
      //            4.5          5            3.0          4.3
      //            1.2        1.2            3.0          3.2
            
      file_out << "object \"points\" class array type float rank 1 shape " 
                 << dimension_tag::dim 
                 << " items " 
                 << global_point_list.size() 
                 << " data follows" << std::endl;

      for (seg_it = segment_begin(); seg_it != segment_end(); ++seg_it)
      {
         for (vit2 = (*seg_it).vertex_begin(); vit2 != (*seg_it).vertex_end(); vit2++)
         {
            for (unsigned int i = 0; i < dimension_tag::dim; ++i)  
            {  
               file_out << global_point_list[*vit2][i] << "  "; 
            }
            file_out << "   # " << (*vit2).handle() << std::endl;
         }
      }
      file_out << std::endl;

      //             file_out << "object \"points\" class gridpositions counts  ";
      //             for (unsigned int i = 1; i <= dimension_tag::dim; ++i)
      //             {
      //                file_out << global_point_list.get_dim_n(i) << " ";
      //             }
      //             file_out << std::endl;
      //             file_out << "origin    ";
      //             for (unsigned int i = 1; i <= dimension_tag::dim; ++i)  {  file_out << "0  ";   }   file_out << std::endl;

      //             for (unsigned int i = 1; i <= dimension_tag::dim; ++i)
      //             {
      //                file_out << "delta    ";
      //                for (unsigned int i2 = 1; i2 <= dimension_tag::dim; ++i2)  
      //                {  
      //                   if (i == i2)
      //                      file_out << "1  ";   
      //                   else
      //                      file_out << "0  ";   
      //                }   
      //                file_out << std::endl;
      //             }
      //             file_out << std::endl;

      // ===============
            
            
            
      // ==================================================================
      // ====   topological section
      //
      file_out << std::endl;
      file_out << "#  topology section " << std::endl;
      file_out << "object \"connectivity\" class gridconnections counts  ";
      for (unsigned int i = dimension_tag::dim; i>0  ;--i)
      {
         file_out << global_point_list.get_dim_n(i) << " ";
      }
      file_out << std::endl;



      // ==================================================================
      // ====   topological section
      //
      file_out << std::endl;
      file_out << "#  quantity section " << std::endl;
      // # object 3 are the data, which are in a one-to-one correspondence with
      // # the positions ("dep" on positions). The positions increment in the order
      // # "last index varies fastest", i.e. (x0, y0, z0)   (x0, y0, z1), (x0, y0, z2),
      // # (x0, y1, z0), etc.
      // #
      // object 3 class array type float rank 0 items 24 data follows
      //            1          3.4            5            2
      //            3.4        5.1            0.3          4.5
      //            1          2.3            4.1          2.1
      //            6            8            9.1          2.3
      //            4.5          5            3.0          4.3
      //            1.2        1.2            3.0          3.2
      // attribute "dep" string "positions"
      file_out << "object \""<< quan_name << "\" class array type float rank 0 items " << global_point_list.size()  << " data follows" << std::endl;

      // ==================================================================
      // ====   quantity section
      //
      // TODO !! HACK  !!

      std::vector<double> tempquan_container(global_point_list.size());

      for (seg_it = segment_begin(); seg_it != segment_end(); ++seg_it)
      {
         for (vit2 = (*seg_it).vertex_begin(); vit2 != (*seg_it).vertex_end(); vit2++)
         {
            storage_type value;
            retrieve_quantity( (*vit2), quan_name, value);
            if (value == storage_type() )
               tempquan_container[(*vit2).handle()] = 0.0;
            else
               tempquan_container[(*vit2).handle()] = value(0,0);
         }
      }
      // TODO !! HACK  !!
      for (unsigned int i = 0; i < tempquan_container.size(); i++)
      {
         if (std::abs(tempquan_container[i]) <= 1.0e-5)
            file_out << 0.0 << "   #" << i << std::endl;
         else
            file_out << tempquan_container[i] << "   #" << i << std::endl;
      }
      file_out << std::endl;


      file_out << "attribute \"dep\" string \"positions\"" << std::endl;
      file_out << std::endl;

      file_out << "object \"gsse regular grid \" class field " << std::endl;
      file_out << "component \"positions\" \"points\""  << std::endl;
      file_out << "component \"connections\" \"connectivity\"" << std::endl;
      file_out << "component \"data\" \""<<quan_name<<"\" " << std::endl;

      file_out.close();
   }


   void write_dx_file_cell(const std::string& filename, std::string quan_name="")
   {
     std::cout << "not yet implemented.. " << std::endl;
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


   }  // namespace detail
}  // namespace gsse

#endif

