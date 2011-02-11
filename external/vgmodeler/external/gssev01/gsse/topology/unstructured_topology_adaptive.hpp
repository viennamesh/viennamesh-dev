/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_TOPOLOGY_ADAPTIVE_HH_ID
#define GSSE_TOPOLOGY_ADAPTIVE_HH_ID

#include "gsse/topology_handles.hpp"
#include "gsse/derived_iterator.hpp"
#include "gsse/derived_objects.hpp"

namespace gsse
{

///////////////////////////////////////////////////////////////////////////
/// @brief Unstructured n-dimensional helper class
///
/// For an easier access to cell-point indices
///
///////////////////////////////////////////////////////////////////////////

namespace detail_topology
{

class cell_2_vertex_indices
{
  std::vector<unsigned int> index_v;
public:
  // 
  // for backward compatiblity -> user defined constructors
  //
   // for std::map usage ??? 
  cell_2_vertex_indices()   
  {
  }


  //< user-convenient constructor for 1D
  cell_2_vertex_indices(const unsigned int& pp1, const unsigned int& pp2)   
  {
    index_v.push_back(pp1);
    index_v.push_back(pp2);
  }

  //< user-convenient constructor for 2D
  cell_2_vertex_indices(const unsigned int& pp1, const unsigned int& pp2, const unsigned int& pp3)
  {
    index_v.push_back(pp1);
    index_v.push_back(pp2);
    index_v.push_back(pp3);
  }

  //< user-convenient constructor for 3D
  cell_2_vertex_indices(const unsigned int& pp1, const unsigned int& pp2, const unsigned int& pp3, const unsigned int& pp4) 
  {
    index_v.push_back(pp1);
    index_v.push_back(pp2);
    index_v.push_back(pp3);
    index_v.push_back(pp4);
  }

  //<  generic constructor
  template<typename InputIterator>
  cell_2_vertex_indices(InputIterator first, InputIterator last)
  {
    while (first != last)
      {
	index_v.push_back(*first++);
      }
  }

  unsigned int size()            const { return index_v.size(); }
  unsigned int operator[](int i) const { return index_v[i];  }
	
  friend std::ostream& operator<<(std::ostream& ostr, const cell_2_vertex_indices cpi)
  {
    ostr << "cell_2_vertex_indices " ;
    return ostr;
  }
};





///////////////////////////////////////////////////////////////////////////
/// @brief Generic unstructured topology class
///
///////////////////////////////////////////////////////////////////////////

template<unsigned int dimension>
class unstructured_adaptive
{
  typedef  unstructured_adaptive<dimension>       self;

   // for the adaptive cell container
   long cell_number;
   long cell_size;

public:
  typedef cell_2_vertex_indices         cell_2_pointindices_t;
      
private:
  friend class topology_traits<self>::cell_on_vertex_iterator;

  typedef vertex_handle_int<self> vertex_handle;
  typedef cell_handle_int<self>   cell_handle;
      
      
  ///////////////////////////////////////////////////////////////////////////
  //
  //  IMPLICIT topology information storage
  //
  ///////////////////////////////////////////////////////////////////////////
      

  ///////////////////////////////////////////////////////////////////////////
  /// @brief cell index container
  ///
  /// global container for storing cells with corresponding vertex index
  ///   cellindices vector[ cell index ]  =  [ [ 1 ] -> 0, 1, 2 || ..]
  ///////////////////////////////////////////////////////////////////////////

   typedef std::map<cell_handle, cell_2_pointindices_t >          cell_indices_t;
   cell_indices_t cell_indices_container;


public:    
   typedef typename cell_indices_t::iterator                      cell_indices_iterator;
   typedef typename cell_indices_t::const_iterator                cell_indices_const_iterator;


  typedef typename topology_traits<self>::vertex_iterator     vertex_iterator;
  typedef typename topology_traits<self>::cell_iterator       cell_iterator;
  typedef typename topology_traits<self>::vertex              vertex;
  typedef typename topology_traits<self>::cell                cell;

  ///////////////////////////////////////////////////////////////////////////
  /// !brief vertex index container
  ///
  /// global container for storing vertices with corresponding cell index
  ///   vertexIndicies vector[ vertex index , set [cell indices ] ]  =   [ [ 0 ] ->  [  1, 2, 3 ..] || .. ]
  ///
  ///////////////////////////////////////////////////////////////////////////

  typedef std::set<unsigned int>                               cell_reference_container;
  typedef std::pair<vertex_handle, cell_reference_container >  vertex_index_entry;
  typedef std::vector<vertex_index_entry>                      vertex_indices_container;
  typedef typename vertex_indices_container::iterator          vertex_indices_container_iterator;
  typedef typename vertex_indices_container::const_iterator    vertex_indices_container_const_iterator;
  typedef typename cell_reference_container::const_iterator    set_int_const_iterator;
      
private:
  ///////////////////////////////////////////////////////////////////////////
  //
  //  the "real" container
  //
  ///////////////////////////////////////////////////////////////////////////

  vertex_indices_container                                     vertex_indices;

      
public:
  // default constructor, copy constructor, destructor work fine here
   unstructured_adaptive()
      {
         cell_number = -1;
         cell_size = 0;
      }

   long get_unique_cell_number()
      {
         return ++cell_number;
      }

   cell_indices_const_iterator  get_ci_iterator_begin() const
      {
         return cell_indices_container.begin();
      }
   cell_indices_const_iterator  get_ci_iterator_end() const
      {
         return cell_indices_container.end();
      }


 void remove_cell(cell cell_)
 {
    --cell_size;

//     std::cout << "  remove cell: " << cell_ << std::endl;
    
    // acces the cell_indices container
    // ...  access vertex_indices by entrys from cell indices
    //
    // i.e. cell index 4 -> vertices: 1,2,3
    //  erase vertex_index 1,2,3 -> cell_index 4
    
    typename cell_indices_t::iterator ci_it =   cell_indices_container.find(cell_.handle());
    
    if (ci_it == cell_indices_container.end()) 
    {
       std::cout << "ERROR with topology access" << std::endl;
       return;
    }
    
    cell_2_vertex_indices cpi = (*ci_it).second;
    
    for (unsigned int i=0; i < cpi.size(); i++)
    {
       vertex_indices_container_iterator vivit;
       for (vivit = vertex_indices.begin(); vivit != vertex_indices.end(); ++vivit)
       {
          // TODO !!!!  .. check the cast
          // if the vertex_index was already inserted, add the new_cell_numer
          //
          if ( (*vivit).first == static_cast<int>(cpi[i]))
          { 
             (*vivit).second.erase(cell_.handle());
             break;
          }
       }
    }
    
    // erase cell index
    cell_indices_container.erase(cell_.handle());
    
    
 }
 
  void add_cell(const cell_2_vertex_indices& cpi) 
  {
     ++cell_size;
     int new_cell_number ( get_unique_cell_number() );

     // check each vertex_index, if it is already inserted into the vertex_index_container
     //
     for (unsigned int i=0; i < cpi.size(); i++)
      {
        vertex_indices_container_iterator vivit;
	for (vivit = vertex_indices.begin(); vivit != vertex_indices.end(); ++vivit)
	  {
             // TODO !!!!  .. check the cast
             // if the vertex_index was already inserted, add the new_cell_numer
             //
	    if ( (*vivit).first == static_cast<int>(cpi[i]))
	      { 
		(*vivit).second.insert(new_cell_number);
		break;
	      }
	  }
        // if it was not already inserted, create a new entry 
        //
	if (vivit == vertex_indices.end())
	  {
	    vertex_indices.push_back( vertex_index_entry(cpi[i], cell_reference_container() ) );
	    vertex_indices[vertex_indices.size()-1].second.insert(new_cell_number);
	  }
      }

     // finally, add the new cell entry into the cell container
     //
     cell_indices_container[new_cell_number] = cpi; 
  }
  // ===================
  //  just for no.. to be consistent with the non-adaptive topology
  //  .. it is the same as add_cell
  //
   void add_cell_2(const cell_2_vertex_indices& cpi) 
  {
     ++cell_size;
     int new_cell_number ( get_unique_cell_number() );

     // check each vertex_index, if it is already inserted into the vertex_index_container
     //
     for (unsigned int i=0; i < cpi.size(); i++)
      {
        vertex_indices_container_iterator vivit;
	for (vivit = vertex_indices.begin(); vivit != vertex_indices.end(); ++vivit)
	  {
             // TODO !!!!  .. check the cast
             // if the vertex_index was already inserted, add the new_cell_numer
             //
	    if ( (*vivit).first == static_cast<int>(cpi[i]))
	      { 
		(*vivit).second.insert(new_cell_number);
		break;
	      }
	  }
        // if it was not already inserted, create a new entry 
        //
	if (vivit == vertex_indices.end())
	  {
	    vertex_indices.push_back( vertex_index_entry(cpi[i], cell_reference_container() ) );
	    vertex_indices[vertex_indices.size()-1].second.insert(new_cell_number);
	  }
      }

     // finally, add the new cell entry into the cell container
     //
     cell_indices_container[new_cell_number] = cpi; 
  }
  // ===================
      
  unsigned int get_cell_size_internal()   const { return cell_size;}
  unsigned int get_vertex_size_internal() const { return vertex_indices.size(); } 
      
  // --------   generic part ----------------
  inline vertex_iterator vertex_begin () const { return vertex_iterator(*this,0); }
  inline vertex_iterator vertex_end   () const { return vertex_iterator(*this, -1); }
  inline cell_iterator   cell_begin   () const { return cell_iterator(*this);}
  inline cell_iterator   cell_end     () const { return cell_iterator(*this, cell_indices_container.end());}
  // --------   ------------ ----------------


  ///////////////////////////////////////////////////////////////////////////
  //
  //  convenient methods
  //
  ///////////////////////////////////////////////////////////////////////////

  // I think, these can be private methods
   vertex_handle get_vertex_index(unsigned int index) const    
      { 
         return vertex_indices[index].first; 
      }
   
   vertex_handle get_vertex_index_from_cell(cell_handle ch, unsigned int index) const
      { 
         typename cell_indices_t::const_iterator ci_it =   cell_indices_container.find(ch);
         
         if (ci_it == cell_indices_container.end()) 
         {
            std::cout << "ERROR with topology access" << std::endl;
            return -1;
         }
         return ((*ci_it).second)[index];
      }

  bool operator==(const self& other) const { return this == &other; } 


  // only for now.. maybe we can eliminate this
  vertex  add_user_point(const vertex_handle& vh)
  { 
    vertex_indices.push_back( vertex_index_entry(vh, cell_reference_container() ) );
    return vertex(*this, vh);
  }

  // only for now.. maybe we can eliminate this
  bool    is_handle_inside(const vertex_handle& vh) const 
  /// !!!
  /// TODO::  I have to optimize that 
  {
    vertex_indices_container_iterator vivit;
    for (vivit=vertex_indices.begin(); vivit != vertex_indices.end(); ++vivit)
      {
	if ( (*vivit).first == vh)
	  { 
	    return true;
	  }
      }
    return false;
  }

  // only for now.. maybe we can eliminate this
  vertex  get_vertex_from_handle(const vertex_handle& vh) const { return vertex(*this, vh); }

  friend std::ostream& operator<<(std::ostream& ostr, const unstructured_adaptive<dimension>& tri2d)
  {
    ostr << "  Unstructured topology output operator.. " << std::endl;
    return ostr;
  }
};

} // end of namespace::detail_topology

} // end of namespace::gsse

#endif
