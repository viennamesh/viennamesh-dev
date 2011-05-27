/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_TOPOLOGY_INTERFACE_NETGEN_ID
#define GSSE_TOPOLOGY_INTERFACE_NETGEN_ID

// *** system includes
//
#include<algorithm>

// *** GSSE includes
//
#include "gsse/topology.hpp"
#include "gsse/topology/topology_handles.hpp"
#include "gsse/topology/derived_iterator.hpp"
#include "gsse/topology/derived_objects.hpp"

// *** BOOST includes
//
#include <boost/array.hpp>


// *** NETGEN includes
//
namespace nglib {
#include "libsrc/interface/nglib.h"
}


namespace gsse
{

template<typename TopologyT> struct topology_traits;  // forward declaration


namespace detail_topology
{
  template <typename TopologyT>
  class unstructured_vertex_iterator
    : public boost::iterator_facade<
    unstructured_vertex_iterator<TopologyT>                    
    , typename topology_traits<TopologyT>::vertex_type              
    , boost::bidirectional_traversal_tag                       
    , typename topology_traits<TopologyT>::vertex_type              
    >
  {
    typedef unstructured_vertex_iterator<TopologyT>            self;
    typedef int                                                counter_t;
    typedef typename topology_traits<TopologyT>::vertex_type   vertex_type;
    typedef typename topology_traits<TopologyT>::vertex_handle vertex_handle;

  public:
    typedef TopologyT         base_type;
    typedef vertex_handle     my_handle;

  private:
    TopologyT const*  g;
    counter_t        mc;
    int              size;
     
  public:
    unstructured_vertex_iterator() : g(0), mc(-1) {}
    explicit
    unstructured_vertex_iterator(TopologyT const& gg) : 
      g(&gg), mc(0), size(g->vertex_size()) {}
    unstructured_vertex_iterator(TopologyT const& gg, int vv) : 
      g(&gg), mc(vv), size(g->vertex_size()) {}

    friend class boost::iterator_core_access;

    void increment() { ++mc; }
    void decrement() { --mc; }
    void advance(int n) {mc+=n;}  
    bool equal(const self& other) const 
    { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= size); }

    vertex_type dereference()  const 
    { if (mc < size) return vertex_type(*g, g->get_vertex_index(mc)); else return vertex_type(); } 

    bool valid() const 
    { return (mc < size); }
    void reset() {mc = 0;}

    TopologyT const&         topo_anchor() const { return *g; }
    base_type const&         base()        const { return *g; }
  };


  template <typename TopologyT>
  class unstructured_cell_iterator
    : public boost::iterator_facade<
    unstructured_cell_iterator<TopologyT>                     // Derived
    , typename topology_traits<TopologyT>::cell_type          // Base
    , boost::bidirectional_traversal_tag                      // CategoryOrTraversal     
    , typename topology_traits<TopologyT>::cell_type          // Base
    >
  {
    typedef unstructured_cell_iterator<TopologyT>                 self;
    typedef int                                                   counter_t;
    typedef typename topology_traits<TopologyT>::cell_type             cell_type;
    typedef typename topology_traits<TopologyT>::cell_handle      cell_handle;
    
  public:
    typedef TopologyT         base_type;
    
  public:
     unstructured_cell_iterator() : g(0), mc(-1) {}
     explicit
     unstructured_cell_iterator(TopologyT const& gg) : 
        g(&gg), mc(0), size(g->get_cell_size_internal()) {}
     
     unstructured_cell_iterator(TopologyT const& gg, int vv) : 
        g(&gg), mc(vv),size(g->get_cell_size_internal()) {}
     
     friend class boost::iterator_core_access;
     
     void increment() { ++mc; }
     void decrement() { --mc; }
     void advance(int n) {mc+=n;}    // random access 
     bool equal(const self& other) const 
        { return ( (mc == other.mc &&   topo_anchor() == other.topo_anchor())  || mc >= size); }
     cell_type dereference()   const { if (mc < size) return cell_type(*g, mc); else return cell_type();} 
     bool valid()         const { return (mc < size); }
     void reset() {mc = 0;}

     
     TopologyT const&  topo_anchor() const { return *g; }
     base_type const&  base()        const { return *g; }
     
  private:
     TopologyT const*  g;
     counter_t        mc;
     int              size;
  };
  
  

  template <typename TopologyT>
  class unstructured_vertex_on_cell_iterator
    : public boost::iterator_facade<
    unstructured_vertex_on_cell_iterator<TopologyT>     // Derived
    , typename topology_traits<TopologyT>::vertex_type         // Base
    , boost::random_access_traversal_tag                  // CategoryOrTraversal     
    , typename topology_traits<TopologyT>::vertex_type         // Base
    , unsigned int
    >
  {
    typedef unstructured_vertex_on_cell_iterator<TopologyT>       self;
    typedef int                                                   counter_t;
    typedef typename topology_traits<TopologyT>::cell_type             cell_type;
    typedef typename topology_traits<TopologyT>::vertex_type           vertex_type;
    typedef typename topology_traits<TopologyT>::vertex_handle    vertex_handle;
    typedef typename topology_traits<TopologyT>::dimension_tag    dimension_tag;
    
  public:
     typedef cell_type   base_type;
     
     unstructured_vertex_on_cell_iterator() : mc(-1) {}
     explicit
     unstructured_vertex_on_cell_iterator(cell_type const& cc) : c(cc), mc(0) {}
     unstructured_vertex_on_cell_iterator(cell_type const& cc, counter_t count) : c(cc), mc(count) {}
     
     friend class boost::iterator_core_access;
     
     void increment() { ++mc; }
     void decrement() { --mc; }
     void advance(int n) {mc+=n;}    
     
     unsigned int distance_to(self const& other) const {return mc - other.mc;}
     
     bool equal(const self& other) const 
        { return ( (other.c.handle() == c.handle() && mc == other.mc && 
                    topo_anchor() == other.topo_anchor())  || mc >= dimension_tag::dim+1); }

     vertex_type dereference()    const 
        { 
           //std::cout << " DEREF " << mc << "   " << c.handle() << "  " 
           //          << topo_anchor().get_vertex_index_from_cell(c.handle(),mc) << std::endl;
           
           if (mc < dimension_tag::dim+1) 
              return vertex_type(topo_anchor(), topo_anchor().get_vertex_index_from_cell(c.handle(),mc)); 
           else return vertex_type();
        } 
    
     bool valid()          const { return (mc < dimension_tag::dim+1); }
     TopologyT const&      topo_anchor()  const { return c.topo_anchor(); }
     base_type const&      base()         const { return c;}
     void reset() {mc = 0;}
     
  private:
    cell_type         c;
    counter_t    mc;
  };



  
  template <typename TopologyT>
  class unstructured_cell_on_vertex_iterator
    : public boost::iterator_facade<
    unstructured_cell_on_vertex_iterator<TopologyT>  
    , typename topology_traits<TopologyT>::cell_type      
    , boost::bidirectional_traversal_tag             
    , typename topology_traits<TopologyT>::cell_type      
    >
  {
    typedef unstructured_cell_on_vertex_iterator<TopologyT>       self;
    typedef int                                                   counter_t;
    typedef typename topology_traits<TopologyT>::cell_type        cell_type;
    typedef typename topology_traits<TopologyT>::vertex_type      vertex_type;
    typedef typename topology_traits<TopologyT>::cell_handle      cell_handle;
    typedef typename topology_traits<TopologyT>::dimension_tag    dimension_tag;
    
  public:
    typedef vertex_type base_type;

  public:
    unstructured_cell_on_vertex_iterator() {}
    explicit
    unstructured_cell_on_vertex_iterator(vertex_type const& vv) : v(vv)
    {
      index = 0;
      cellarray = (int*)malloc (sizeof(int) * 1);
      int number_of_elements;
      nglib::Ng_Mesh * mesh = vv.topo_anchor().get_internal_mesh_pointer();

      nglib::Ng_GetCellOnVertex(mesh, 
				vv.handle()+1, &cellarray, number_of_elements);
      size= number_of_elements;
      //std::cout << "size: " << size << std::endl;
    }
    ~unstructured_cell_on_vertex_iterator()
    {
      if (cellarray)
	free(cellarray);
    }

    void increment() 
    {
      ++index; //std::cout << "increment: " << index <<std::endl; 
    }
    void decrement() { --index; }
    cell_type dereference()  const { return cell_type(topo_anchor(), cellarray[index]-1); }
    bool equal(self const& other) const 
    {return index == other.index;}

    bool valid()        const 
    { 
      //std::cout << " valid:  index:" << index << " .. size: "<< size << std::endl;
      return index < size; 
    }

    TopologyT const&   topo_anchor()  const { return v.topo_anchor();}
    base_type const&   base()         const { return v;}
    void reset() {index = 0;}

  private:
    int           *cellarray;
    long          index;
    long          size;
    vertex_type   v;
  };





///////////////////////////////////////////////////////////////////////////
/// @brief Unstructured n-dimensional helper class
///
/// For an easier access to cell-point indices
///
///////////////////////////////////////////////////////////////////////////

template<unsigned int internal_size>
class cell_2_vertex_indices
{
  typedef long numeric_t;
  boost::array<numeric_t, internal_size> index_v;

public:

  // for backward compatiblity -> user defined constructors

  cell_2_vertex_indices()   {
 }


  //< user-convenient constructor for 1D
  cell_2_vertex_indices(long pp1, long pp2)   
  {
    index_v[0] = pp1;
    index_v[1] = pp2;
  }

  //< user-convenient constructor for 2D
  cell_2_vertex_indices(long pp1, long pp2, long pp3)   
  {
    index_v[0] = pp1;
    index_v[1] = pp2;
    index_v[2] = pp3;
  }

  //< user-convenient constructor for 3D
  cell_2_vertex_indices(long pp1, long pp2, long pp3, long pp4)   
  {
    index_v[0] = pp1;
    index_v[1] = pp2;
    index_v[2] = pp3;
    index_v[3] = pp4;
  }

  //<  generic constructor
  template<typename InputIterator>
  cell_2_vertex_indices(InputIterator first, InputIterator last)
  {
   long index=0;
    while (first != last)
      {
	index_v[index++] = *first++;
      }
  }

 
   unsigned int size()            const { return internal_size; }
   numeric_t operator[](int i) const 
      { 
         //std::cout << i << std::endl;
         return index_v[i];  
      }

   // High Peformance Variant
   //unsigned int operator[](int i) const { if (i == 0) return i1; else return i2;  }
	
   friend std::ostream& operator<<(std::ostream& ostr, const cell_2_vertex_indices cpi)
      {
         // ostr << "cell_2_vertex_indices " << cpi[0] << "/" << cpi[1];
         return ostr;
      }
};




///////////////////////////////////////////////////////////////////////////
/// @brief Generic unstructured topology class
///
///////////////////////////////////////////////////////////////////////////

template<unsigned int dimension>
class unstructured
{
  typedef  unstructured<dimension>       self;

public:
  typedef cell_2_vertex_indices<dimension+1>         cell_2_vertex_mapping;
      
private:
  friend class topology_traits<self>::cell_on_vertex_iterator; 

  typedef vertex_handle_int<self> vertex_handle;
  typedef cell_handle_int<self>   cell_handle;

public:    

  typedef typename topology_traits<self>::vertex_iterator     vertex_iterator;
  typedef typename topology_traits<self>::cell_iterator       cell_iterator;
  typedef typename topology_traits<self>::vertex_type         vertex_type;
  typedef typename topology_traits<self>::cell_type           cell_type;

  typedef std::set<unsigned int>                               cell_reference_container;
  typedef std::pair<vertex_handle, cell_reference_container >  vertex_index_entry;
  typedef std::vector<vertex_index_entry>                      vertex_indices_container;
  typedef typename vertex_indices_container::iterator          vertex_indices_container_iterator;
  typedef typename vertex_indices_container::const_iterator    vertex_indices_container_const_iterator;
  typedef typename cell_reference_container::const_iterator    set_int_const_iterator;

  
public:
  // default constructor, copy constructor, destructor work fine here


   long max_handle;

  unstructured(long segment_number = 0) : segment_number(segment_number)
  {
//     nglib::Ng_Init();

//     std::string filename("localmesh.vol");
//     mesh = nglib::Ng_LoadMesh(filename.c_str());

//     max_handle =0;
//     max_handle = nglib::Ng_GetNE(mesh);
  }


  void set_topo_size(long vertex_count)
  {
  }
  
  


  unsigned int get_cell_size_internal()   const { return nglib::Ng_GetNE(mesh); }
  unsigned int get_vertex_size_internal() const { return nglib::Ng_GetNP(mesh);  }
  unsigned int get_max_handle()           const { return max_handle + 1;  }
   

  // --------   generic part ----------------

  unsigned int cell_size()               const { return get_cell_size_internal(); }
  unsigned int vertex_size()             const { return get_vertex_size_internal(); } 
  inline vertex_iterator vertex_begin () const { return vertex_iterator(*this,0); }
  inline vertex_iterator vertex_end   () const { return vertex_iterator(*this, -1); }
  inline cell_iterator   cell_begin   () const { return cell_iterator(*this);}
  inline cell_iterator   cell_end     () const { return cell_iterator(*this, -1);}



  // ----------------------------------------
  //
  //  convenient methods
  //
   


  bool check_orientation(cell_handle ch1, cell_handle ch2)
  {
    bool status=true;

    nglib::Ng_Volume_Element_Type volumeelement;
    int pointarray1[4],
        pointarray2[4];
    nglib::Ng_GetVolumeElement (mesh, ch1, pointarray1);
    nglib::Ng_GetVolumeElement (mesh, ch2, pointarray2);
    
    return status;
  }
    
  void change_orientation(cell_handle ch)
  {
    
  }

   vertex_handle get_vertex_index(unsigned int index) const    
  { return index; }   // just for now
  // this means that only elements with starting value
  // 0 and continous range can be stored and handled
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



  cell_handle get_cell_from_vertex(vertex_handle vh, unsigned int index)
  {
//     int *cellarray = (int*)malloc (sizeof(int) * 1);
//     int number_of_elements;
//     nglib::Ng_GetCellOnVertex(mesh, vh+1, &cellarray, number_of_elements);
//     std::cout << "output: " << cellarray[0] << std::endl;
  }

   

   vertex_handle get_vertex_index_from_cell(cell_handle ch, unsigned int index) const
      { 
	if (index >= 4)
	  std::cout << "Error in getvertexindexfromcell" << std::endl;

	int pointarray[4];
	nglib::Ng_GetVolumeElement (mesh, ch+1, pointarray);

	return vertex_handle( pointarray[index]-1); 
      }
   
   bool operator==(const self& other) const { return this == &other; } 
   


  // add cells
  //
  void add_cell_2(const cell_2_vertex_mapping& cpi)
  {
    int volume_index_array[4];
    volume_index_array[0]  = cpi[0];
    volume_index_array[1]  = cpi[1];
    volume_index_array[2]  = cpi[2];
    volume_index_array[3]  = cpi[3];

    nglib::Ng_AddVolumeElement(mesh, nglib::NG_TET, volume_index_array);
  }
  
  void add_user_point(double* coord)
  { 
    nglib::Ng_AddPoint (mesh,  coord);
  }
  
  friend std::ostream& operator<<(std::ostream& ostr, const unstructured<dimension>& tri2d)
  {
    ostr << " #### GSSE::  topology interface .. netgen.. " << std::endl;
    return ostr;
  }

  void** const get_internal_mesh_pointer() const
  {
    return mesh;
  }

private:
  nglib::Ng_Mesh *mesh;
  long           segment_number; 
};

} // end of namespace::detail_topology

} // end of namespace::gsse

#endif
