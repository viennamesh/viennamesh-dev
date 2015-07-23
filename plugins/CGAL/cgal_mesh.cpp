#include "cgal_mesh.hpp"
#include "viennagridpp/core.hpp"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>

namespace viennamesh
{
template <class HDS>
class Build_triangle : public CGAL::Modifier_base<HDS> {
public:
    Build_triangle(double* aPoints,int anumPoints, int* aFaces, int anumFaces) {mPoints=aPoints;mFaces=aFaces;numPoints=anumPoints;numFaces=anumFaces;}
    void operator()( HDS& hds) {
        // Postcondition: hds is a valid polyhedral surface.
        CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
        B.begin_surface( numPoints, numFaces);
        typedef typename HDS::Vertex   Vertex;
        typedef typename Vertex::Point Point;
	for(int i=0;i<numPoints;i++)
		B.add_vertex( Point( mPoints[i*3], mPoints[i*3+1], mPoints[i*3+2]));
	for(int i=0;i<numFaces;i++){
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

  viennamesh_error convert(viennagrid::mesh const & input, cgal::mesh & output)
  {
    typedef viennagrid::mesh ViennaGridMeshType;

   
    typedef viennagrid::result_of::const_vertex_range<ViennaGridMeshType>::type     ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type   ConstVertexIteratorType;

    typedef viennagrid::result_of::const_element_range<ViennaGridMeshType, 2>::type ConstCellRangeType;
    typedef viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;
    typedef cgal::mesh::HalfedgeDS HalfedgeDS;
    ConstVertexRangeType vertices(input);
    int num_points= vertices.size();
    double* points=(double*)malloc(num_points*3*sizeof(double));

    int index=0;
    for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
    {
      points[3*index+0]=viennagrid::get_point(input, *vit)[0];
      points[3*index+1]=viennagrid::get_point(input, *vit)[1];
      points[3*index+2]=viennagrid::get_point(input, *vit)[2];
    }


    ConstCellRangeType cells(input);

    int num_faces =cells.size();
    int* faces=(int*)malloc(num_faces*3*sizeof(int));
    index=0;
    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit, ++index)
    {
      if((*cit).is_triangle()==false){
	free(faces);
	free(points);
        return false;
      }
      typedef viennagrid::mesh                                              MeshType;
      typedef viennagrid::result_of::element<MeshType>::type ElementType;
      typedef viennagrid::result_of::const_element_range<ElementType>::type ConstBoundaryElementRangeType;
      typedef viennagrid::result_of::iterator<ConstBoundaryElementRangeType>::type ConstBoundaryElementIteratorType;
      int sm_index=0;
      ConstBoundaryElementRangeType boundary_vertices( *cit, 0 );
      for (ConstBoundaryElementIteratorType vit = boundary_vertices.begin(); vit != boundary_vertices.end(); ++vit, ++sm_index)
      {
        faces[3*index+sm_index]=(*vit).id() ;//3*index+sm_index;
      }
    }

    Build_triangle<HalfedgeDS> triangle(points,num_points,faces,num_faces);
    output.delegate(triangle);
    free(faces);
    free(points);
    return VIENNAMESH_SUCCESS;
  }




  viennamesh_error convert(cgal::mesh const & input, viennagrid::mesh & output)
  {
    typedef cgal::mesh::Facet_const_iterator 			Facet_iterator;

    Facet_iterator begin = input.facets_begin();
    for ( ; begin != input.facets_end(); ++begin)
    {
      viennagrid::make_triangle(
        output,
	viennagrid::make_vertex( output,
	  viennagrid::make_point(begin->facet_begin()->vertex()->point().x(),
			       begin->facet_begin()->vertex()->point().y(),
		 	       begin->facet_begin()->vertex()->point().z())),
	viennagrid::make_vertex( output,
	  viennagrid::make_point(begin->facet_begin()->next()->vertex()->point().x(),
			       begin->facet_begin()->next()->vertex()->point().y(),
		 	       begin->facet_begin()->next()->vertex()->point().z())),
	viennagrid::make_vertex( output,
	  viennagrid::make_point(begin->facet_begin()->opposite()->vertex()->point().x(),
			       begin->facet_begin()->opposite()->vertex()->point().y(),
		 	       begin->facet_begin()->opposite()->vertex()->point().z()))
      );
    }

    return VIENNAMESH_SUCCESS;
  }

  template<>
  viennamesh_error internal_convert<viennagrid_mesh, cgal::mesh>(viennagrid_mesh const & input, cgal::mesh & output)
  { return convert( input, output ); }

  template<>
  viennamesh_error internal_convert<cgal::mesh, viennagrid_mesh>(cgal::mesh const & input, viennagrid_mesh & output)
  {
    viennagrid::mesh output_pp(output);
    return convert( input, output_pp );
  }

}
