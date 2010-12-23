/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef STRUCTURED_BASE_IMPL_ND_HH
#define STRUCTURED_BASE_IMPL_ND_HH


// *** system includes
// 
#include <vector>
#include <bitset>

// *** BOOST includes
// 
#include <boost/array.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/at.hpp>


// *** GSSE includes
// 
#include "gsse/topology/topology_handles.hpp"
#include "gsse/topology/derived_iterator.hpp"
#include "gsse/topology/derived_objects.hpp"
#include "gsse/topology/structured_iterator.hpp"
#include "gsse/topology/boundary_iterator.hpp"
#include "gsse/exceptions.hpp"            



namespace gsse
{

namespace detail_topology 
{

///////////////////////////////////////////////////////////////////////////
/// @brief ND structured topology class, implementation
///
/// Here, the following objects are generated:
/// - base data structures
/// - base iterators: lowest AND highest dimensional
///                 : lowest on highest AND highest on lowest
///
/// The following information can be received:
/// - getcellSize
/// - getvertexSize
/// - resize methods
///////////////////////////////////////////////////////////////////////////

   template <unsigned int DIM>
   class structured
   {
      typedef structured<DIM>                                                self_t;
      static const unsigned int dim = DIM;  

   public:
      typedef gsse_container                                                 gsse_type;     // type information [RH] 200801

      typedef detail_topology::generic_vertex<self_t>                        vertex_type;
      typedef detail_topology::generic_cell<self_t>                          cell_type;
      typedef detail_topology::structured_edge< self_t >                     edge_type;
      typedef detail_topology::structured_facet<self_t >                     facet_type;

      typedef detail_topology::vertex_handle_int <self_t>                    vertex_handle;
      typedef detail_topology::cell_handle_int   <self_t>                    cell_handle;
      typedef boost::tuples::tuple<vertex_handle, vertex_handle>             edge_handle;
      typedef boost::tuples::tuple<vertex_handle, vertex_handle, vertex_handle, vertex_handle>          facet_handle;   // [RH][TODO] .. use a metaprogram
   // to implement the number of vertex handles
   //  as well as to decide if a facet_handle == edge_handle
//   typedef detail_topology::facet_handle_int<self_t>                      facet_handle;   

//       typedef detail_topology::facet_handle_int  <self_t>                    facet_handle;


      // *** base iterators 				      
      //
      typedef detail_topology::structured_vertex_iterator<self_t>            vertex_iterator;
      typedef detail_topology::structured_cell_iterator<self_t>              cell_iterator;

      // subtopology object .. GSSE stores only X^0 and X^n 
      //
      typedef          std::set<edge_type>                                   edge_container_t;
      typedef          std::set<facet_type>                                  facet_container_t;
      typedef typename edge_container_t::iterator                            edge_iterator;
      typedef typename facet_container_t::iterator                           facet_iterator;


      // *** derived iterators
      //
      typedef detail_topology::structured_vertex_on_cell_iterator<DIM>       vertex_on_cell_iterator;
      typedef detail_topology::structured_vertex_on_facet_iterator<self_t>   vertex_on_facet_iterator;
      typedef detail_topology::structured_vertex_on_edge_iterator <self_t>   vertex_on_edge_iterator;
      typedef detail_topology::vertex_on_vertex_iterator <self_t>            vertex_on_vertex_iterator;

      typedef detail_topology::structured_edge_on_cell_iterator<self_t>      edge_on_cell_iterator;
      typedef detail_topology::edge_on_facet_iterator<self_t>                edge_on_facet_iterator;   // [RH][TODO] .. cached iterators.. check it
      typedef detail_topology::structured_edge_on_vertex_iterator<self_t, false>  edge_on_vertex_iterator;

      typedef detail_topology::facet_on_vertex_iterator<self_t>              facet_on_vertex_iterator;
      typedef detail_topology::facet_on_edge_iterator<self_t>                facet_on_edge_iterator;   // [RH][TODO] .. cached iterators.. check it
      typedef detail_topology::structured_facet_on_cell_iterator<self_t>     facet_on_cell_iterator;


      typedef detail_topology::structured_cell_on_vertex_iterator<DIM>       cell_on_vertex_iterator;
      typedef detail_topology::cell_on_edge_iterator<self_t>                 cell_on_edge_iterator;
      typedef detail_topology::cell_on_facet_iterator<self_t>                cell_on_facet_iterator;
      

      typedef mesh_dim_tag<DIM>                                              dimension_tag;    
      typedef mesh_dim_tag<DIM>                                              dimension_topology_tag;    


   public:
      structured() {} 
      template<typename InputIterator>
      structured (InputIterator first, InputIterator last)
         {
            calculate_indices(first, last);
         }


   
      template<typename InputIterator>
      void calculate_indices(InputIterator first, InputIterator last)
         {
            unsigned int acc(1);
            unsigned int acc_cell(1);
            unsigned int i(0);
            while (first != last)
            {
               // 
               // special convenience data structures 
               //   for the numbers of vertices in a dimension   -> pointsdim_cumm
               //   for the numbers of cells in a dimension      -> pointsdim_cumm_cells
               //
               //std::cout << "----------------- " << std::endl;
               pointsdim[i]=*first;
               pointsdim_cumm[i]=acc;
               pointsdim_cumm_cells[i]=acc_cell;

//                std::cout << "i: " << i << std::endl;
//                std::cout << "pointsdim[i]: " << pointsdim[i] << std::endl;
//                std::cout << "pointsdim_cumm[i]: " << pointsdim_cumm[i] << std::endl;
//                std::cout << "pointsdim_cumm_cells[i]: " << pointsdim_cumm_cells[i] << std::endl;

               acc *= *first;
               acc_cell *= (*first - 1);
               ++i;
               ++first;
            }
            if (pointsdim.size() != DIM)
               throw dimension_problem();
         }  
      ~structured() {}

      // *** end of constructors


      // *** base methods 

      int get_dimension() { return dim; }


      // complete resize with arbitrary dim at newdim1
      //
      void resize_dim_n(const unsigned int newdim1, const int dim) 
         { 
	   pointsdim[dim-1] = newdim1; 
           calculate_indices(pointsdim.begin(), pointsdim.end());
         }

      // resize one dimension with additional dimension (newdim1)
      // 

      void add_dim_n(const unsigned int& newdim1, const int& dim) 
         { 
            pointsdim.at(dim-1) += newdim1; 
            calculate_indices(pointsdim.begin(), pointsdim.end());
         }


      unsigned int cell_size() const 
         {
            return get_cell_size_internal();
         }
      
      unsigned int get_cell_size_internal() const 
         { 
            int prod(1);
            for (unsigned int i = 0; i < DIM; i++)
            {
               prod *= (pointsdim[i]-1);
            } 
            return prod;
         }
      
      unsigned int vertex_size() const 
         {
            return get_vertex_size_internal();
         }
      
      unsigned int get_vertex_size_internal() const  
         { 
            int prod(1);
            for (unsigned int i = 0; i < DIM; i++)
            {
               prod *= pointsdim[i];
//                std::cout << "dim: " << i << "   pointsdim: " << pointsdim[i] << std::endl;
            } 
            return prod;
         }

      unsigned int edge_size()               const { return edge_container.size(); } 
      unsigned int facet_size()              const { return facet_container.size();}


      long get_dim_n_size(const int dim) const {return pointsdim[dim-1];}


      unsigned int get_vertex_dim_n_index(const vertex_type& v, unsigned int d) const
         {
            return ( v.handle() / pointsdim_cumm[d-1] ) % (get_dim_n_size(d));
         }
      
      unsigned int get_cell_dim_n_index  (const cell_type& c, unsigned int d) const
         {
            //std::cout << "cell [" << d-1 << "]" << pointsdim_cumm_cells[d-1] << std::endl;
            return ( c.handle() / pointsdim_cumm_cells[d-1] ) % (get_dim_n_size(d)-1);
         }

   // =======================================================
   //

   bool is_vertex_handle_inside(const vertex_handle& vh) const
      {
            if (vh >= static_cast<int>(get_vertex_size_internal()))
               return false;
            else
               return true;
      }

   bool is_cell_handle_inside(const cell_handle& ch) const
      {
         if (ch >= static_cast<int>(get_cell_size_internal()))
            return true;

         return false;
      }
   //
   // =======================================================


//       // *** iterator base 

//       vertex_iterator vertex_begin() const; 
//       vertex_iterator vertex_end()   const;
//       cell_iterator   cell_begin()   const;
//       cell_iterator   cell_end()     const;




   // temporary iterators
   //
   void build_up_edge_container()
   {
	   for (cell_iterator c_it = cell_begin(); c_it != cell_end(); ++c_it)
	   {
		   for (edge_on_cell_iterator eoc_it(*c_it); eoc_it.valid(); ++eoc_it)
		   {
			   edge_container.insert( *eoc_it );
		   }
	   }

   } 
// [RH][TODO]
//
   edge_iterator   edge_begin()  const  {   return edge_container.begin();   }    // !! here the build_up must be called before !! 
   edge_iterator   edge_begin()     
   { 
	   build_up_edge_container();
	   return edge_container.begin(); 
   }
   edge_iterator   edge_end()     const  { return edge_container.end(); }

   void build_up_facet_container()
   {
	   for (cell_iterator c_it = cell_begin(); c_it != cell_end(); ++c_it)
	   {
		   for (facet_on_cell_iterator foc_it(*c_it); foc_it.valid(); ++foc_it)
		   {
			   facet_container.insert( *foc_it );
		   }
	   }

   }
// [RH][TODO]  .. check all const
//
   facet_iterator   facet_begin() const {   return facet_container.begin();    }  // !! here the build_up must be called before.. !! 
   facet_iterator   facet_begin()     
   { 
	   build_up_facet_container();
	   return facet_container.begin(); 
   }
   facet_iterator   facet_end()     const  { return facet_container.end(); }







// =========================
      
//       vertex_type     get_index_vertex(boost::array<long, DIM>& index_v) const;
//       cell_type       get_index_cell  (boost::array<long, DIM>& index_v) const;
      
//       boost::array<long, DIM> get_vertex_index(vertex_type const& v) const;
//       boost::array<long, DIM> get_cell_index(cell_type const& c) const;
      
      // *** end of iterator base       
      
      // *** convenient methods
      
//       bool    is_handle_inside(const vertex_handle& vh);
//       vertex_type  get_vertex_from_handle(const vertex_handle& vh);

      bool operator==(const self_t& other) const { return this == &other; } 
  

// [RH][TODO] ???
//
//       friend std::ostream& operator<<(std::ostream& ostr, const structured& car)
//          {
//             ostr << "  Cartesian " << DIM << "D output operator.. " << std::endl;
    
//             unsigned int d = 0;
//             while (d < DIM)
//             {
//                ostr << "   pointsdim: " << d << ": " << car.pointsdim[d];
//                ostr << std::endl;
//                d++;
//             }
//             return ostr;
//          }

      // ** end of convenient methods

      // *** member variables



   // @brief Get a vertex object from a given handle
   // this can be done explicitly more easily
   //
   vertex_type  get_vertex_from_handle(const vertex_handle& vh)
   {
      return vertex_type(*this, vh);
   }



   // @brief Structured base iterators methods
   //
   vertex_iterator vertex_begin() const 
   { 
      return vertex_iterator(*this);
   }


   // @brief Structured base iterators methods
   //
   vertex_iterator vertex_end()   const 
   { 
      return typename structured<DIM>::vertex_iterator(*this, -1);
   }

   // @brief Structured base iterators methods
   //
   cell_iterator cell_begin() const 
   { 
      return typename structured<DIM>::cell_iterator(*this);
   }

   cell_iterator cell_end()     const 
   { return cell_iterator(*this, -1);}


 
   vertex_type  get_index_vertex(boost::array<long, DIM>& index_v) const
   {
      int newindex(index_v[0]);
      int indexmult(1);
      for (unsigned int i=1; i < index_v.size(); i++)
      {
         indexmult*=(pointsdim[i-1]);
         newindex+= index_v[i] * indexmult;
      }
      return vertex_type(*this,newindex);
   }

    


   cell_type get_index_cell(boost::array<long, DIM>& index_v) const
   {
      int newindex(index_v[0]);
      int indexmult(1);
      for (unsigned int i=1; i < index_v.size(); i++)
      {
         indexmult*=(pointsdim[i-1]-1);
         newindex+= index_v[i] * indexmult;
      }
      return cell_type(*this, newindex); 
   }

   boost::array<long, DIM> get_vertex_index (vertex_type const& v) const
   {
       boost::array<long, DIM> result;
       
       for (unsigned int i=1; i<=DIM; i++)
	 {
	   result[i-1] = get_vertex_dim_n_index(v, i);
	 }
       return result;
     }

   boost::array<long, DIM> get_cell_index (cell_type const& c) const
   {
       boost::array<long, DIM> result;
       
       for (unsigned int i=1; i<=DIM; i++)
	 {
            result[i-1] = get_cell_dim_n_index(c, i);
	 }
       return result;
     }

// --------------------------
// n-dimension index calculation
//    dimensions: [d1, d2, ... ,dn]
//
//    dimKindex = static_cast<int>( v.handle() / (PROD(j=1,k-1) dj)) % dk
//   
// -------------------------------------------------------------------------------------





   // ######################################################################
   //
   //  topological traversal functors
   // 
   template<typename TopologyT>
   struct traverse_vertex
   {
      typedef typename TopologyT::vertex_iterator iterator;
      typedef typename TopologyT::vertex_type     value_type;
      
      traverse_vertex(TopologyT& topology) : topology(topology) {}
      
      iterator begin() { return topology.vertex_begin();}
      iterator end()   { return topology.vertex_end(); }
   protected:
      TopologyT& topology;
   };

   template<typename TopologyT>
   struct traverse_cell
   {
      typedef typename TopologyT::cell_iterator iterator;
      typedef typename TopologyT::cell_type     value_type;
      
      traverse_cell(TopologyT& topology) : topology(topology) {}
      
      iterator begin() { return topology.cell_begin();}
      iterator end()   { return topology.cell_end(); }
   protected:
      TopologyT& topology;
   };


   // ----
   template<typename TopologyT>
   struct traverse_edge
   {
      typedef typename TopologyT::edge_iterator iterator;
      typedef typename TopologyT::edge_type     value_type;
      
      traverse_edge(TopologyT& topology) : topology(topology) {   }
      
      iterator begin() { return topology.edge_begin();}
      iterator end()   { return topology.edge_end(); }
   protected:
      TopologyT& topology;
   };

   template<typename TopologyT>
   struct traverse_facet
   {
      typedef typename TopologyT::facet_iterator iterator;
      typedef typename TopologyT::facet_type     value_type;
      
      traverse_facet(TopologyT& topology) : topology(topology) {  }
      
      iterator begin() { return topology.facet_begin();}
      iterator end()   { return topology.facet_end(); }
   protected:
      TopologyT& topology;
   };


   typedef boost::mpl::map<
      boost::mpl::pair<boost::mpl::pair<self_t, vertex_type>,  traverse_vertex<self_t> >,
      boost::mpl::pair<boost::mpl::pair<self_t, cell_type>,    traverse_cell  <self_t> >,
      boost::mpl::pair<boost::mpl::pair<self_t, edge_type>,    traverse_edge  <self_t> >,
      boost::mpl::pair<boost::mpl::pair<self_t, facet_type>,   traverse_facet <self_t> >
      > traversal_repository;



   private:

      boost::array<unsigned int, DIM> pointsdim;

      // object iterators
      //
      edge_container_t                       edge_container;
      facet_container_t                      facet_container;
      

   public:
      boost::array<unsigned int, DIM> pointsdim_cumm;
      boost::array<unsigned int, DIM> pointsdim_cumm_cells;

      mutable std::map<edge_type, std::vector<facet_type> > edge_facet_map;
      mutable std::map<facet_type, std::vector<edge_type> > facet_edge_map;
   };




} // end of namespace::detail_topology

} // end of namespace::gsse

#endif


