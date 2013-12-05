#include "viennamesh/algorithm/viennagrid.hpp"
#include "viennamesh/algorithm/io.hpp"
#include "viennamesh/algorithm/triangle.hpp"

#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/interface.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include <boost/concept_check.hpp>




double my_sizing( viennamesh::point_2d const & pt )
{
  return (pt[0]+1.0)/40.0;
}




template<typename SomethingT>
std::pair< typename viennagrid::result_of::point<SomethingT>::type, typename viennagrid::result_of::point<SomethingT>::type > bounding_box( SomethingT const & something )
{
  typedef typename viennagrid::result_of::point<SomethingT>::type PointType;
  typedef typename viennagrid::result_of::const_vertex_range<SomethingT>::type ConstVertexRangeType;
  typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

  PointType lower;
  PointType upper;

  ConstVertexRangeType vertices(something);
  if (vertices.empty())
    return std::make_pair(lower, upper);

  ConstVertexIteratorType vit = vertices.begin();
  upper = lower = viennagrid::point( *(vit++) );

  for (; vit != vertices.end(); ++vit)
  {
    lower = viennagrid::min(lower, viennagrid::point(*vit));
    upper = viennagrid::max(upper, viennagrid::point(*vit));
  }

  return std::make_pair(lower, upper);
}


template<typename PointT>
typename viennagrid::result_of::coord<PointT>::type size( std::pair<PointT, PointT> const & bounding_box )
{
  return viennagrid::norm_2(bounding_box.second - bounding_box.first);
}



template<typename LineT, typename SomethingT>
typename viennagrid::result_of::coord<SomethingT>::type distance( LineT const & line, SomethingT const & something )
{
  typedef typename viennagrid::result_of::const_line_range<SomethingT>::type ConstLineRangeType;
  typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;

  typedef typename viennagrid::result_of::coord<SomethingT>::type CoordType;

  ConstLineRangeType lines(something);
  if (lines.empty())
    return -1;

  ConstLineIteratorType lit = lines.begin();
  CoordType min_distance = viennagrid::distance(line, *(lit++));

  for (; lit != lines.end(); ++lit)
  {
    CoordType current_distance = viennagrid::distance(line, *lit);
    if (current_distance < min_distance)
      min_distance = current_distance;
  }

  return min_distance;
}




template<typename PointT, typename SegmentT>
typename viennagrid::result_of::coord<SegmentT>::type distance_to_interface( PointT const & point, SegmentT const & seg0, SegmentT const & seg1 )
{
  typedef typename viennagrid::result_of::const_facet_range<SegmentT>::type ConstFacetRangeType;
  typedef typename viennagrid::result_of::iterator<ConstFacetRangeType>::type ConstFacetIteratorType;

  typedef typename viennagrid::result_of::coord<SegmentT>::type CoordType;

  ConstFacetRangeType facets(seg0);
  if (facets.empty())
    return -1;

  CoordType min_distance = -1;

  for (ConstFacetIteratorType fit = facets.begin(); fit != facets.end(); ++fit)
  {
    if (is_boundary(seg1, *fit))
    {
      CoordType current_distance = viennagrid::distance(point, *fit);

      if (min_distance < 0)
        min_distance = current_distance;
      else if (current_distance < min_distance)
        min_distance = current_distance;
    }
  }

  return min_distance;
}


template<typename SegmentT>
typename viennagrid::result_of::coord<SegmentT>::type interface_volume( SegmentT const & seg0, SegmentT const & seg1 )
{
  typedef typename viennagrid::result_of::const_facet_range<SegmentT>::type ConstFacetRangeType;
  typedef typename viennagrid::result_of::iterator<ConstFacetRangeType>::type ConstFacetIteratorType;

  typedef typename viennagrid::result_of::coord<SegmentT>::type CoordType;

  ConstFacetRangeType facets(seg0);
  CoordType length = 0;

  for (ConstFacetIteratorType fit = facets.begin(); fit != facets.end(); ++fit)
    if (is_boundary(seg1, *fit))
      length += viennagrid::volume(*fit);

  return length;
}







template<typename ConnectorElementTypeOrTagT, typename SegmentHandleT, typename ElementTagT, typename WrappedConfigT>
bool connects_to_segment( SegmentHandleT const & segment, viennagrid::element<ElementTagT, WrappedConfigT> const & element )
{
  typedef viennagrid::element<ElementTagT, WrappedConfigT> ElementType;
  typedef typename viennagrid::result_of::const_element_range<ElementType, ConnectorElementTypeOrTagT>::type ConstBoundaryElementRangeType;
  typedef typename viennagrid::result_of::iterator<ConstBoundaryElementRangeType>::type ConstBoundaryElementIteratorType;

  ConstBoundaryElementRangeType boundary_elements(element);
  for (ConstBoundaryElementIteratorType beit = boundary_elements.begin(); beit != boundary_elements.end(); ++beit)
  {
    if (viennagrid::is_in_segment(segment, *beit))
      return true;
  }

  return false;
}

template<typename SegmentHandleT, typename ElementTagT, typename WrappedConfigT>
bool connects_to_segment( SegmentHandleT const & segment, viennagrid::element<ElementTagT, WrappedConfigT> const & element )
{
  return connects_to_segment<viennagrid::vertex_tag>(segment, element);
}



// http://www.blackpawn.com/texts/pointinpoly/
template<typename PointT>
typename viennagrid::result_of::coord<PointT>::type determinant( PointT const & p0, PointT const & p1 )
{
  return p0[0]*p1[1] - p0[1]*p1[0];
}


template<typename PointT>
bool same_side( PointT const & p1, PointT const & p2, PointT const & a, PointT const & b )
{
  typedef typename viennagrid::result_of::coord<PointT>::type NumericT;

  NumericT cp1 = determinant( b-a, p1-a );
  NumericT cp2 = determinant( b-a, p2-a );

  if (cp1*cp2 >= 0)
    return true;
  else
    return false;
}


template<typename TriangleT, typename PointT>
bool is_inside( TriangleT const & triangle, PointT const & p )
{
  PointT const & a = viennagrid::point( viennagrid::vertices(triangle)[0] );
  PointT const & b = viennagrid::point( viennagrid::vertices(triangle)[1] );
  PointT const & c = viennagrid::point( viennagrid::vertices(triangle)[2] );

  if ( same_side(p, a, b, c) && same_side(p, b, a, c) && same_side(p, c, a, b) )
    return true;
  else
    return false;
}



template<typename ElementT, typename FieldT>
typename viennagrid::result_of::coord<ElementT>::type gradient( ElementT const & element, FieldT const & field )
{
  typedef typename viennagrid::result_of::point<ElementT>::type PointType;
  typedef typename viennagrid::result_of::coord<ElementT>::type NumericType;

  PointType p0 = viennagrid::point( viennagrid::vertices(element)[0] );
  PointType p1 = viennagrid::point( viennagrid::vertices(element)[1] );
  PointType p2 = viennagrid::point( viennagrid::vertices(element)[2] );

  NumericType s0 = field(viennagrid::vertices(element)[0]);
  NumericType s1 = field(viennagrid::vertices(element)[1]);
  NumericType s2 = field(viennagrid::vertices(element)[2]);


  PointType p10 = p1-p0;
  PointType p20 = p2-p0;

  NumericType det = determinant( p10, p20 );

  PointType M0;
  PointType M1;

  M0[0] = p20[1];
  M0[1] = -p10[1];

  M1[0] = -p20[0];
  M1[1] = p10[0];

  M0 /= det;
  M1 /= det;

  PointType s;
  s[0] = s1-s0;
  s[1] = s2-s0;

  NumericType gradient = std::abs(viennagrid::inner_prod(M0, s)) + std::abs(viennagrid::inner_prod(M1, s));

  return gradient;
}




namespace functors
{




  template<typename MeshT, typename FieldT>
  struct mesh_interpolate
  {
    mesh_interpolate( MeshT const & mesh_, FieldT const & field_ ) : mesh(mesh_), field(field_) {}

    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
    typedef NumericType result_type;

    NumericType operator() ( PointType const & pt ) const
    {
      typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      ConstCellRangeType cells(mesh);
      for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      {
        if ( is_inside( *cit, pt ) )
        {
          PointType p0 = viennagrid::point( viennagrid::vertices(*cit)[0] );
          PointType p1 = viennagrid::point( viennagrid::vertices(*cit)[1] );
          PointType p2 = viennagrid::point( viennagrid::vertices(*cit)[2] );

          NumericType f0 = std::abs(determinant( p1-pt, p2-pt )) / 2.0;
          NumericType f1 = std::abs(determinant( p0-pt, p2-pt )) / 2.0;
          NumericType f2 = std::abs(determinant( p0-pt, p1-pt )) / 2.0;

          NumericType s0 = field(viennagrid::vertices(*cit)[0]);
          NumericType s1 = field(viennagrid::vertices(*cit)[1]);
          NumericType s2 = field(viennagrid::vertices(*cit)[2]);

          NumericType val = (s0*f0 + s1*f1 + s2*f2) / (f0 + f1 + f2);

          return (s0*f0 + s1*f1 + s2*f2) / (f0 + f1 + f2);
        }
      }

      return -1;
    }

    MeshT const & mesh;
    FieldT const & field;
  };





  template<typename MeshT, typename FieldT>
  struct mesh_gradient
  {
    mesh_gradient( MeshT const & mesh_, FieldT const & field_ ) : mesh(mesh_), field(field_) {}

    typedef typename viennagrid::result_of::point<MeshT>::type PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
    typedef NumericType result_type;

    NumericType operator() ( PointType const & pt ) const
    {
      typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      ConstCellRangeType cells(mesh);
      for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      {
        if ( is_inside( *cit, pt ) )
          return gradient( *cit, field );
      }

      return -1;
    }

    MeshT const & mesh;
    FieldT const & field;
  };








  template<typename SegmentT>
  struct distance_to_segment
  {
    distance_to_segment( SegmentT const & segment_ ) : segment(&segment_) {}

    typedef typename viennagrid::result_of::point<SegmentT>::type PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

    NumericType operator() ( PointType const & pt ) const
    {
      return viennagrid::boundary_distance( pt, *segment );
    }

    SegmentT const * segment;
  };

  template<typename SegmentT>
  struct distance_to_interface
  {
    distance_to_interface( SegmentT const & segment0_, SegmentT const & segment1_ ) : segment0(&segment0_), segment1(&segment1_) {}

    typedef typename viennagrid::result_of::point<SegmentT>::type PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
    typedef NumericType result_type;

    NumericType operator() ( PointType const & pt ) const
    {
      return ::distance_to_interface( pt, *segment0, *segment1 );
    }

    SegmentT const * segment0;
    SegmentT const * segment1;
  };



  template<typename PointT>
  struct is_in_rect
  {
    typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
    typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;

    typedef NumericType result_type;

    template<typename FunctorT>
    is_in_rect( FunctorT const & functor_, PointT const & pt0, PointT const & pt1 ) : function(viennamesh::bind(functor_, _1))
    {
      ll[0] = std::min(pt0[0], pt1[0]);
      ll[1] = std::min(pt0[1], pt1[1]);

      ur[0] = std::max(pt0[0], pt1[0]);
      ur[1] = std::max(pt0[1], pt1[1]);
    }

    NumericType operator() ( PointT const & pt ) const
    {
      if ( (pt[0] >= ll[0]) && (pt[0] <= ur[0]) && (pt[1] >= ll[1]) && (pt[1] <= ur[1]) )
        return function(pt);

      return -1;
    }

    PointT ll;
    PointT ur;
    SizingFunctionType function;
  };



  template<typename SegmentT>
  struct is_in_segments
  {
    typedef typename viennagrid::result_of::point<SegmentT>::type PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
    typedef typename viennamesh::result_of::sizing_function<PointType>::type SizingFunctionType;

    typedef NumericType result_type;

    template<typename FunctorT>
    is_in_segments( FunctorT const & functor_ ) : function(viennamesh::bind(functor_, _1)) {}

    template<typename FunctorT>
    is_in_segments( FunctorT const & functor_, SegmentT const & segment0_ ) : function(viennamesh::bind(functor_, _1))
    {
      segments.push_back(&segment0_);
    }

    template<typename FunctorT>
    is_in_segments( FunctorT const & functor_, SegmentT const & segment0_, SegmentT const & segment1_ ) : function( viennamesh::bind(functor_, _1) )
    {
      segments.push_back(&segment0_);
      segments.push_back(&segment1_);
    }

    NumericType operator() ( PointType const & pt ) const
    {
      typedef typename viennagrid::result_of::const_cell_range<SegmentT>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      for (unsigned int i = 0; i < segments.size(); ++i)
      {
        ConstCellRangeType cells(*segments[i]);
        for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
        {
          if ( is_inside( *cit, pt ) )
            return function(pt);
        }
      }

      return -1;
    }

    std::vector<SegmentT const *> segments;
    SizingFunctionType function;
  };

  template<typename SegmentT>
  struct is_near_double_interface
  {
    typedef typename viennagrid::result_of::point<SegmentT>::type PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
    typedef typename viennamesh::result_of::sizing_function<PointType>::type SizingFunctionType;

    template<typename FunctorT>
    is_near_double_interface( FunctorT const & functor_, SegmentT const & segment0_, SegmentT const & segment1_, NumericType max_distance_between_interfaces_ ) : segment0(&segment0_), segment1(&segment1_), function(viennamesh::bind(functor_, _1)), max_distance_between_interfaces(max_distance_between_interfaces_) {}

    typedef NumericType result_type;

    NumericType operator() ( PointType const & pt ) const
    {
      NumericType distance_segment0 = viennagrid::boundary_distance( *segment0, pt );
      NumericType distance_segment1 = viennagrid::boundary_distance( *segment1, pt );

      if ( distance_segment0 + distance_segment1 < max_distance_between_interfaces )
        return function(pt);

      return -1;
    }

    SegmentT const * segment0;
    SegmentT const * segment1;
    SizingFunctionType function;
    NumericType max_distance_between_interfaces;
  };




  template<typename SegmentT>
  struct distance_to_double_interface
  {
    distance_to_double_interface( SegmentT const & segment0_, SegmentT const & segment1_, double max_distance_between_segments_) : segment0(&segment0_), segment1(&segment1_), max_distance_between_segments(max_distance_between_segments_) {}

    typedef typename viennagrid::result_of::point<SegmentT>::type PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

    NumericType operator() ( PointType const & pt ) const
    {
      std::pair<PointType, PointType> closest_points_segment0 = viennagrid::closest_points_on_boundary( *segment0, pt );
      std::pair<PointType, PointType> closest_points_segment1 = viennagrid::closest_points_on_boundary( *segment1, pt );

      NumericType distance_between_segments = viennagrid::norm_2( closest_points_segment1.second - closest_points_segment0.second );

      if (distance_between_segments < max_distance_between_segments)
        return -1;

      return viennagrid::norm_2( pt - (closest_points_segment1.second+closest_points_segment0.second)/2.0 );
    }

    SegmentT const * segment0;
    SegmentT const * segment1;

    NumericType max_distance_between_segments;
  };




  template<typename PointT>
  struct min
  {
    typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
    typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;
    typedef NumericType result_type;

    template<typename Functor1T, typename Functor2T>
    min(Functor1T const & functor1_, Functor2T const & functor2_) : function1(viennamesh::bind(functor1_, _1)), function2(viennamesh::bind(functor2_, _1)) {}

    NumericType operator() ( PointT const & pt ) const
    {
      NumericType val1 = function1(pt);
      NumericType val2 = function2(pt);
      if (val1 < 0)
        return val2;
      if (val2 < 0)
        return val1;

      return std::min( val1, val2 );
    }

    SizingFunctionType function1;
    SizingFunctionType function2;
  };

  template<typename PointT>
  struct max
  {
    typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
    typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;

    template<typename Functor1T, typename Functor2T>
    max(Functor1T const & functor1_, Functor2T const & functor2_) : function1(viennamesh::bind(functor1_, _1)), function2(viennamesh::bind(functor2_, _1)) {}

    NumericType operator() ( PointT const & pt ) const
    { return std::max( function1(pt), function2(pt) ); }

    SizingFunctionType function1;
    SizingFunctionType function2;
  };

  template<typename PointT>
  struct constant
  {
    typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
    typedef double result_type;

    constant(NumericType value_) : value(value_) {}

    NumericType operator() ( PointT const & ) const
    { return value; }

    NumericType value;
  };


  template<typename PointT>
  struct map_to_valid
  {
    typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
    typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;

    template<typename FunctorT>
    map_to_valid(FunctorT const & functor_, NumericType min_value_, NumericType max_value_) : function(viennamesh::bind(functor_, _1)), min_value(min_value_), max_value(max_value_) {}

    NumericType operator() ( PointT const & pt ) const
    {
      NumericType tmp = function1(pt);
      if (tmp <= 0)
        return max_value;
      return std::max(tmp, min_value);
    }

    SizingFunctionType function;

    NumericType min_value;
    NumericType max_value;
  };


  template<typename PointT>
  struct linear_interpolate
  {
    typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
    typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;
    typedef NumericType result_type;

    template<typename FunctorT>
    linear_interpolate(FunctorT const & functor_, NumericType lower_, NumericType upper_, NumericType lower_to_, NumericType upper_to_) : function(viennamesh::bind(functor_, _1)), lower(lower_), upper(upper_), lower_to(lower_to_), upper_to(upper_to_) {}

    NumericType operator() ( PointT const & pt ) const
    {
      NumericType tmp = function(pt);
      if (tmp <= 0)
        return tmp;

      if (tmp < lower)
        return lower_to;
      if (tmp > upper)
        return upper_to;

      return lower_to + (tmp-lower)/(upper-lower)*(upper_to-lower_to);
    }

    SizingFunctionType function;

    NumericType lower;
    NumericType upper;

    NumericType lower_to;
    NumericType upper_to;
  };
}





int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  // creating a hull extraction algorithm
  viennamesh::algorithm_handle mesher( new viennamesh::triangle::algorithm() );





  typedef viennagrid::triangular_2d_mesh MeshType;
  typedef viennagrid::triangular_2d_segmentation SegmentationType;
  typedef viennagrid::triangular_2d_segment_handle SegmentHandleType;
  typedef viennagrid::result_of::point<MeshType>::type PointType;


  viennagrid::segmented_mesh<MeshType, SegmentationType> segmented_mesh;
  mesher->reference_output( "default", segmented_mesh );

  // linking the output from the reader to the mesher
  mesher->link_input( "default", reader, "default" );


  // Setting the filename for the reader and writer
  reader->set_input( "filename", "/export/florian/work/projects/2013_11 ViennaSHE Yannick/geometry.deva" );

  mesher->set_input( "min_angle", 0.52 );     // minimum angle in radiant, 0.35 are about 20 degrees
  mesher->set_input( "delaunay", true  );     // we want a Delaunay triangulation
  mesher->set_input( "algorithm_type", "incremental_delaunay" );  // incremental Delaunay algorithm is used



  // start the algorithms
  reader->run();
  mesher->run();



  MeshType & mesh = segmented_mesh.mesh;
  SegmentationType & segmentation = segmented_mesh.segmentation;



  SegmentHandleType oxide1 = segmentation.get_segment(0);  // oxide 1
  SegmentHandleType silicon = segmentation.get_segment(1);  // silicon
  SegmentHandleType oxide2 = segmentation.get_segment(2);  // oxide 2
  SegmentHandleType gate = segmentation.get_segment(3);  // gate contact
  SegmentHandleType drain = segmentation.get_segment(4);  // drain contact
  SegmentHandleType bulk = segmentation.get_segment(5);  // bulk contact
  SegmentHandleType source = segmentation.get_segment(6);  // source contact
  SegmentHandleType substrate = segmentation.get_segment(7);  // substrate contact

  // auflösen bei: contact-silicon-interfaces, über gate (im oxide und im silicon)


  std::pair<PointType, PointType> bb = bounding_box( mesh );
  double mesh_size = viennagrid::norm_2( bb.second - bb.first );

  double distance_gate_silicon = viennagrid::boundary_distance( gate, silicon );
  double min_cell_size = distance_gate_silicon/2;

  std::cout << "Mesh size: " << mesh_size << std::endl;
  std::cout << "Distance gate-silicon: " << distance_gate_silicon << std::endl;

  typedef viennagrid::result_of::vertex<MeshType>::type VertexType;
  typedef viennagrid::result_of::cell<MeshType>::type CellType;

  typedef viennagrid::result_of::cell_range<MeshType>::type CellRangeType;
  typedef viennagrid::result_of::iterator<CellRangeType>::type CellIteratorType;




  viennamesh::algorithm_handle mesher_sizing_function( new viennamesh::triangle::algorithm() );

  mesher_sizing_function->link_input( "default", reader, "default" );

  viennagrid::segmented_mesh<MeshType, SegmentationType> segmented_sized_mesh;
  mesher_sizing_function->reference_output( "default", segmented_sized_mesh );





//   functors::distance_to_interface<SegmentHandleType> distance_to_drain_silicon_interface( drain, silicon );
//   functors::linear_interpolate< functors::distance_to_interface<SegmentHandleType> > distance_to_drain_silicon_interface_interpolate( distance_to_drain_silicon_interface, distance_gate_silicon*2, drain_silicon_interface, drain_silicon_interface/10, mesh_size );
// //
//   functors::distance_to_segment<SegmentHandleType> distance_to_oxide1( oxide1 );
//   functors::is_in_segments<SegmentHandleType, functors::distance_to_segment<SegmentHandleType> > is_in_silicon( silicon, distance_to_oxide1 );
//   functors::map_to_valid< functors::is_in_segments<SegmentHandleType, functors::distance_to_segment<SegmentHandleType> > > functor( is_in_silicon, min_cell_size, mesh_size );



//   functors::distance_to_double_interface<SegmentHandleType> distance_to_gate( gate, silicon, distance_gate_silicon );
//   functors::map_to_valid< functors::distance_to_double_interface<SegmentHandleType> > distance_to_gate_corrected( distance_to_gate, min_cell_size, mesh_size );

  functors::constant<PointType> interface_distance_const( distance_gate_silicon );
  functors::is_near_double_interface<SegmentHandleType> near_double_interface( interface_distance_const, gate, silicon, 5*distance_gate_silicon);
  functors::is_in_segments< SegmentHandleType> near_double_interface_segments( near_double_interface, oxide1, silicon);



  viennagrid::triangular_2d_mesh doping_mesh;
  viennagrid::triangular_2d_segmentation doping_segmentation(doping_mesh);

  typedef viennagrid::result_of::accessor_container<VertexType, double, viennagrid::std_map_tag>::type VertexDoubleContainerType;
  typedef viennagrid::result_of::field<VertexDoubleContainerType, VertexType>::type VertexDoubleFieldType;

  VertexDoubleContainerType builtin_container;
  VertexDoubleFieldType builtin_field(builtin_container);

  VertexDoubleContainerType doping_n_container;
  VertexDoubleFieldType doping_n_field(doping_n_container);
  VertexDoubleContainerType doping_p_container;
  VertexDoubleFieldType doping_p_field(doping_p_container);

  VertexDoubleContainerType p_container;
  VertexDoubleFieldType p_field(p_container);
  VertexDoubleContainerType n_container;
  VertexDoubleFieldType n_field(n_container);
  VertexDoubleContainerType potential_container;
  VertexDoubleFieldType potential_field(potential_container);


  typedef viennagrid::result_of::accessor_container<CellType, double, viennagrid::std_map_tag>::type CellDoubleContainerType;
  typedef viennagrid::result_of::field<CellDoubleContainerType, CellType>::type CellDoubleFieldType;

//   CellDoubleContainerType gradient_container;
//   CellDoubleFieldType gradient_field(gradient_container);

  viennagrid::io::vtk_reader<viennagrid::triangular_2d_mesh> vtk_reader;
  viennagrid::io::add_scalar_data_on_vertices( vtk_reader, builtin_field, "builtin" );
  viennagrid::io::add_scalar_data_on_vertices( vtk_reader, doping_n_field, "doping_n" );
  viennagrid::io::add_scalar_data_on_vertices( vtk_reader, doping_p_field, "doping_p" );
  viennagrid::io::add_scalar_data_on_vertices( vtk_reader, p_field, "p" );
  viennagrid::io::add_scalar_data_on_vertices( vtk_reader, n_field, "n" );
  viennagrid::io::add_scalar_data_on_vertices( vtk_reader, potential_field, "potential" );

  vtk_reader(doping_mesh, doping_segmentation, "/export/florian/work/projects/2013_11 ViennaSHE Yannick/doping_mesh/Vg2,508_Vd0,1.devbz.vtu_main.pvd");

//   vtk_reader(doping_mesh, doping_segmentation, "/export/florian/work/projects/2013_11 ViennaSHE Yannick/doping_mesh_2/n53_0,85acc_out.devbz.vtu_main.pvd");


  double max_gradient = -1;

  CellRangeType cells(mesh);
  for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
  {
    double current_gradient = gradient(*cit, builtin_field);
//     gradient_field(*cit) = current_gradient;
    max_gradient = std::max( max_gradient, current_gradient );
  }

  std::cout << "Max gradient = " << max_gradient << std::endl;

//   viennagrid::io::vtk_writer<MeshType, SegmentationType> vtk_writer;
//   viennagrid::io::add_scalar_data_on_cells( vtk_writer, gradient_field, "gradient" );
//   vtk_writer( mesh, segmentation, "gradient_nld" );




//   functors::mesh_gradient<MeshType, VertexDoubleFieldType> gradient_lower( doping_mesh, potential_field );
//   functors::linear_interpolate<PointType> interpolate_lower( gradient_lower, 1e4, 2e5, mesh_size, distance_gate_silicon*20 );
//   functors::is_in_rect<PointType> lower( interpolate_lower, PointType(0, -3e-7), PointType(1e-5, 4e-6) );
//
//
//   functors::mesh_gradient<MeshType, VertexDoubleFieldType> gradient_upper( doping_mesh, potential_field );
//   functors::linear_interpolate<PointType> interpolate_upper( gradient_upper, 1e4, 2e5, mesh_size, distance_gate_silicon*50 );
//   functors::is_in_rect<PointType> upper( interpolate_upper, PointType(0, 4e-6), PointType(1e-5, 1e-5) );
//
//
//   functors::min<PointType> combine_potential( lower, upper );


  functors::mesh_gradient<MeshType, VertexDoubleFieldType> gradient( doping_mesh, builtin_field );
  functors::linear_interpolate<PointType> interpolate_gradient( gradient, 0, 2e5, mesh_size, distance_gate_silicon*10 );
  functors::is_in_segments< SegmentHandleType> combine_potential_segments( interpolate_gradient, silicon );



  functors::min<PointType> combine_silicon( near_double_interface_segments, combine_potential_segments );



  double drain_silicon_interface_length = interface_volume( drain, silicon );
  functors::distance_to_interface<SegmentHandleType> distance_to_drain_silicon_interface( drain, silicon );
  functors::linear_interpolate<PointType> distance_to_drain_silicon_interface_interpolate( distance_to_drain_silicon_interface, distance_gate_silicon*2, mesh_size, drain_silicon_interface_length/10, mesh_size );

  std::cout << "Length interface drain silicon " << drain_silicon_interface_length << std::endl;


  double source_silicon_interface_length = interface_volume( source, silicon );
  functors::distance_to_interface<SegmentHandleType> distance_to_source_silicon_interface( source, silicon );
  functors::linear_interpolate<PointType> distance_to_source_silicon_interface_interpolate( distance_to_source_silicon_interface, distance_gate_silicon*2, mesh_size, source_silicon_interface_length/10, mesh_size );

  std::cout << "Length interface source silicon " << source_silicon_interface_length << std::endl;


  double bulk_silicon_interface_length = interface_volume( bulk, silicon );
  functors::distance_to_interface<SegmentHandleType> distance_to_bulk_silicon_interface( bulk, silicon );
  functors::linear_interpolate<PointType> distance_to_bulk_silicon_interface_interpolate( distance_to_bulk_silicon_interface, distance_gate_silicon*2, mesh_size, bulk_silicon_interface_length/10, mesh_size );

  std::cout << "Length interface bulk silicon " << bulk_silicon_interface_length << std::endl;


  functors::min<PointType> combine_darin_source_interface( distance_to_drain_silicon_interface_interpolate, distance_to_source_silicon_interface_interpolate );

  functors::min<PointType> combine_interfaces( combine_darin_source_interface, distance_to_bulk_silicon_interface_interpolate );



  functors::min<PointType> combine( combine_silicon, combine_interfaces );


  //   max< constant, is_in_segment<SegmentHandleType, distance_to_segment<SegmentHandleType> > > functor(  );



//   mesh_size_functor<MeshType, CellSizeFieldType> functor(mesh, size_field);
//   viennamesh::sizing_function_2d sizing_function = viennamesh::bind<double>(functor, _1);
//   viennamesh::sizing_function_2d sizing_function = viennamesh::bind<double>(distance_to_drain_silicon_interface_interpolate, _1);
//   viennamesh::sizing_function_2d sizing_function = viennamesh::bind<double>(near_double_interface_segments, _1);
  viennamesh::sizing_function_2d sizing_function = viennamesh::bind(combine, _1);

  mesher_sizing_function->set_input( "sizing_function", sizing_function );

  mesher_sizing_function->set_input( "min_angle", 0.52 );     // minimum angle in radiant, 0.35 are about 20 degrees
  mesher_sizing_function->set_input( "delaunay", true  );     // we want a Delaunay triangulation
  mesher_sizing_function->set_input( "algorithm_type", "incremental_delaunay" );  // incremental Delaunay algorithm is used

  mesher_sizing_function->run();





  VertexDoubleContainerType sized_doping_n_container;
  VertexDoubleFieldType sized_doping_n_field(sized_doping_n_container, 0.0);
  VertexDoubleContainerType sized_doping_p_container;
  VertexDoubleFieldType sized_doping_p_field(sized_doping_p_container, 0.0);
  VertexDoubleContainerType sized_netto_doping_container;
  VertexDoubleFieldType sized_netto_doping_field(sized_netto_doping_container, 0.0);

  VertexDoubleContainerType sized_n_container;
  VertexDoubleFieldType sized_n_field(sized_n_container, 0.0);
  VertexDoubleContainerType sized_p_container;
  VertexDoubleFieldType sized_p_field(sized_p_container, 0.0);
  VertexDoubleContainerType sized_potential_container;
  VertexDoubleFieldType sized_potential_field(sized_potential_container, 0.0);

  typedef viennagrid::result_of::vertex_range<SegmentHandleType>::type VertexOnSegmentRangeType;
  typedef viennagrid::result_of::iterator<VertexOnSegmentRangeType>::type VertexOnSegmentIteratorType;

  functors::mesh_interpolate<MeshType, VertexDoubleFieldType> doping_n_interpolate( doping_mesh, doping_n_field );
  functors::mesh_interpolate<MeshType, VertexDoubleFieldType> doping_p_interpolate( doping_mesh, doping_p_field );

  functors::mesh_interpolate<MeshType, VertexDoubleFieldType> n_interpolate( doping_mesh, n_field );
  functors::mesh_interpolate<MeshType, VertexDoubleFieldType> p_interpolate( doping_mesh, p_field );
  functors::mesh_interpolate<MeshType, VertexDoubleFieldType> potential_interpolate( doping_mesh, potential_field );

  VertexOnSegmentRangeType vertices( segmented_sized_mesh.segmentation(1) );
  for (VertexOnSegmentIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
  {
    double doping_n = doping_n_interpolate( viennagrid::point(*vit) );
    double doping_p = doping_p_interpolate( viennagrid::point(*vit) );

    sized_doping_n_field(*vit) = doping_n;
    sized_doping_p_field(*vit) = doping_p;
    sized_netto_doping_field(*vit) = doping_p - doping_n;

    sized_n_field(*vit) = n_interpolate( viennagrid::point(*vit) );
    sized_p_field(*vit) = p_interpolate( viennagrid::point(*vit) );
    sized_potential_field(*vit) = potential_interpolate( viennagrid::point(*vit) );
  }



  {
    viennagrid::io::vtk_writer<MeshType, SegmentationType> vtk_writer;

    viennagrid::io::add_scalar_data_on_vertices( vtk_writer, sized_doping_n_field, "doping_n" );
    viennagrid::io::add_scalar_data_on_vertices( vtk_writer, sized_doping_p_field, "doping_p" );
    viennagrid::io::add_scalar_data_on_vertices( vtk_writer, sized_netto_doping_field, "netto_doping_field" );

    viennagrid::io::add_scalar_data_on_vertices( vtk_writer, sized_n_field, "n" );
    viennagrid::io::add_scalar_data_on_vertices( vtk_writer, sized_p_field, "p" );
    viennagrid::io::add_scalar_data_on_vertices( vtk_writer, sized_potential_field, "potential" );

    vtk_writer( segmented_sized_mesh.mesh, segmented_sized_mesh.segmentation, "meshed_nld_sizing_field" );
  }




}
