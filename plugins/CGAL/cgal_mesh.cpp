#include "cgal_mesh.hpp"
#include "viennagrid/viennagrid.hpp"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>

namespace viennamesh
{
  template <class HDS>
  class Build_triangle : public CGAL::Modifier_base<HDS>
  {
  public:
    Build_triangle(double* aPoints,int anumPoints, int* aFaces, int anumFaces) : mPoints(aPoints), mFaces(aFaces), numPoints(anumPoints), numFaces(anumFaces) {}
    void operator()( HDS& hds)
    {
      // Postcondition: hds is a valid polyhedral surface.
      CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
      B.begin_surface( numPoints, numFaces);
      typedef typename HDS::Vertex   Vertex;
      typedef typename Vertex::Point Point;

      for(int i=0;i<numPoints;i++)
      {
        B.add_vertex( Point( mPoints[i*3], mPoints[i*3+1], mPoints[i*3+2]));
      }

      
      int temp;
      for(int i=0;i<numFaces;i++)
      {
        B.begin_facet();	
        B.add_vertex_to_facet( mFaces[3*i]);
        B.add_vertex_to_facet( mFaces[3*i+1]);
        B.add_vertex_to_facet( mFaces[3*i+2]);
        B.end_facet();
      }

      B.end_surface();
    }
  private:
    double* mPoints;
    int* mFaces;
    int numPoints;
    int numFaces;
  };

  viennamesh_error convert(viennagrid::mesh const & input, cgal::polyhedron_surface_mesh & output)
  {
    typedef viennagrid::mesh ViennaGridMeshType;

    typedef viennagrid::result_of::const_vertex_range<ViennaGridMeshType>::type     ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type   ConstVertexIteratorType;

    typedef viennagrid::result_of::const_element_range<ViennaGridMeshType, 2>::type ConstCellRangeType;
    typedef viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;
    
    typedef cgal::polyhedron_surface_mesh::HalfedgeDS HalfedgeDS;
    
    ConstVertexRangeType vertices(input);
    int num_points= vertices.size();
    std::vector<double> points(num_points*3);

    int index=0;
    for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
    {
      points[3*index+0]=viennagrid::get_point(input, *vit)[0];
      points[3*index+1]=viennagrid::get_point(input, *vit)[1];
      points[3*index+2]=viennagrid::get_point(input, *vit)[2];
    }

    ConstCellRangeType cells(input);
    int num_faces =cells.size();
    std::vector<int> faces(num_faces*3);

    index=0;
    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit, ++index)
    {
      if((*cit).is_triangle()==false)
        return false;
      typedef viennagrid::mesh                                              MeshType;
      typedef viennagrid::result_of::element<MeshType>::type ElementType;
      typedef viennagrid::result_of::const_element_range<ElementType>::type ConstBoundaryElementRangeType;
      typedef viennagrid::result_of::iterator<ConstBoundaryElementRangeType>::type ConstBoundaryElementIteratorType;
      int sm_index=0;
      ConstBoundaryElementRangeType boundary_vertices( *cit, 0 );
      for (ConstBoundaryElementIteratorType vit = boundary_vertices.begin(); vit != boundary_vertices.end(); ++vit, ++sm_index)
      {
        faces[3*index+sm_index]=(*vit).id().index();//3*index+sm_index;
      }
    }

    Build_triangle<HalfedgeDS> triangle(&points[0], num_points, &faces[0], num_faces);
    output.delegate(triangle);

    return VIENNAMESH_SUCCESS;
  }


  int get_id_of_vertex(cgal::Point_3 p, cgal::polyhedron_surface_mesh const & input)
  {
    typedef cgal::polyhedron_surface_mesh::Vertex_const_iterator Vertex_iterator;
    int pos =0;
    {
      Vertex_iterator begin = input.vertices_begin();
      for ( ; begin != input.vertices_end(); ++begin, ++pos)
        if(begin->point()==p)
          return pos;
    }
    return -1;
  }

  viennamesh_error convert(cgal::polyhedron_surface_mesh const & input, viennagrid::mesh & output)
  {
    typedef viennagrid::mesh                               		MeshType;
    typedef viennagrid::result_of::element<MeshType>::type 		VertexType;
    typedef cgal::polyhedron_surface_mesh::Vertex_const_iterator	Vertex_iterator;
    typedef cgal::polyhedron_surface_mesh::Facet_const_iterator		Facet_iterator;
    int numberofpoints =0;
    
    numberofpoints = input.size_of_vertices();
   
    // create EMPTY vector containining Viennagrid Vertices with size numberofpoints
    std::vector<VertexType> vertex_handles(numberofpoints);
    
    //std::map<CGAL_POINT_ID_TYPE, VertexType> vertex_handles; // ToDo (Florian) 
    
    // iterate through all CGAL vertices and store the coordinates in the viennagrid vertices for each
    {
      Vertex_iterator v = input.vertices_begin();
      for (int i = 0; i < numberofpoints; ++i, ++v)
      {
	//vertex_handles[ CGAL_ID(v->point()) ] = viennagrid::make_vertex( output,	// ToDo (Florian)
	
        vertex_handles[i] = viennagrid::make_vertex( output,
        viennagrid::make_point(v->point().x(),v->point().y(),v->point().z())
        );
      }
    }

    // interate through all CGAL Facets and create a Viennagrid triangle each time 
    Facet_iterator f = input.facets_begin();
    for ( ; f != input.facets_end(); ++f)
    {
      viennagrid::make_triangle(
          output,
	  //vertex_handles[CGAL_ID(begin->facet_begin()->vertex()] // ToDo (Florian)
          vertex_handles[get_id_of_vertex(f->facet_begin()->vertex()->point(),input)],
          vertex_handles[get_id_of_vertex(f->facet_begin()->next()->vertex()->point(),input)],
          vertex_handles[get_id_of_vertex(f->facet_begin()->opposite()->vertex()->point(),input)]
      );
    }
    
    return VIENNAMESH_SUCCESS;
  }

  template<>
  viennamesh_error internal_convert<viennagrid_mesh, cgal::polyhedron_surface_mesh>(viennagrid_mesh const & input, cgal::polyhedron_surface_mesh & output)
  { return convert( input, output ); }

  template<>
  viennamesh_error internal_convert<cgal::polyhedron_surface_mesh, viennagrid_mesh>(cgal::polyhedron_surface_mesh const & input, viennagrid_mesh & output)
  {
    viennagrid::mesh output_pp(output);
    return convert( input, output_pp );
  }

}
