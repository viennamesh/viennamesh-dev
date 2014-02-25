#ifndef VIENNAMESH_CORE_SIZING_FUNCTION_HPP
#define VIENNAMESH_CORE_SIZING_FUNCTION_HPP

#include "viennagrid/algorithm/interface.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/inclusion.hpp"
#include "viennagrid/algorithm/geometry.hpp"

#include "pugixml/pugixml.hpp"

namespace viennamesh
{

  template<typename ElementT, typename AccessorFieldT>
  typename viennagrid::result_of::coord<ElementT>::type gradient( ElementT const & element, AccessorFieldT const & accessor_field )
  {
    typedef typename viennagrid::result_of::point<ElementT>::type PointType;
    typedef typename viennagrid::result_of::coord<ElementT>::type NumericType;

    PointType p0 = viennagrid::point( viennagrid::vertices(element)[0] );
    PointType p1 = viennagrid::point( viennagrid::vertices(element)[1] );
    PointType p2 = viennagrid::point( viennagrid::vertices(element)[2] );

    NumericType s0 = accessor_field(viennagrid::vertices(element)[0]);
    NumericType s1 = accessor_field(viennagrid::vertices(element)[1]);
    NumericType s2 = accessor_field(viennagrid::vertices(element)[2]);


    PointType p10 = p1-p0;
    PointType p20 = p2-p0;

    NumericType det = viennagrid::determinant( p10, p20 );

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



  template<typename ElementTypeOrTagT, typename PointT, typename SegmentT>
  typename viennagrid::result_of::coord<SegmentT>::type distance_to_interface( PointT const & point, SegmentT const & seg0, SegmentT const & seg1 )
  {
    typedef typename viennagrid::result_of::const_element_range<SegmentT, ElementTypeOrTagT>::type ConstElementRangeType;
    typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

    typedef typename viennagrid::result_of::coord<SegmentT>::type CoordType;

    ConstElementRangeType elements(seg0);
    if (elements.empty())
      return -1;

    CoordType min_distance = -1;

    for (ConstElementIteratorType eit = elements.begin(); eit != elements.end(); ++eit)
    {
      if (is_boundary(seg1, *eit))
      {
        CoordType current_distance = viennagrid::distance(point, *eit);

        if (min_distance < 0)
          min_distance = current_distance;
        else if (current_distance < min_distance)
          min_distance = current_distance;
      }
    }

    return min_distance;
  }

  template<typename ElementTypeOrTagT, typename PointT, typename SegmentPointerContainerT>
  typename viennagrid::result_of::coord<PointT>::type distance_to_segment_boundaries( PointT const & point, SegmentPointerContainerT const & segments )
  {
    typedef typename viennagrid::detail::result_of::value_type<typename SegmentPointerContainerT::value_type>::type SegmentType;
    typedef typename viennagrid::result_of::const_element_range<SegmentType, ElementTypeOrTagT>::type ConstElementRangeType;
    typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

    typedef typename viennagrid::result_of::coord<SegmentType>::type CoordType;

    if (segments.empty())
      return -1;

    typename SegmentPointerContainerT::const_iterator seg_it = segments.begin();

    ConstElementRangeType elements(**seg_it++);
    if (elements.empty())
      return -1;

    CoordType min_distance = -1;

    for (ConstElementIteratorType fit = elements.begin(); fit != elements.end(); ++fit)
    {
      bool is_on_all_boundaries = true;
      for (typename SegmentPointerContainerT::const_iterator sit = seg_it; sit != segments.end(); ++sit)
      {
        if (!is_boundary(**sit, *fit))
        {
          is_on_all_boundaries = false;
          break;
        }
      }

      if (is_on_all_boundaries)
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


  template<typename PointT, typename SegmentT>
  typename viennagrid::result_of::coord<SegmentT>::type distance_to_segment_boundaries( PointT const & point, SegmentT const & seg0, SegmentT const & seg1 )
  {
    typedef typename viennagrid::result_of::facet_tag<SegmentT>::type FacetTag;
    return distance_to_segment_boundaries<FacetTag>(point, seg0, seg1);
  }



  namespace sizing_function
  {
    template<typename SizingFunctionT>
    typename SizingFunctionT::result_type get( shared_ptr<SizingFunctionT> const & sf, typename SizingFunctionT::point_type const & point )
    {
      return sf->get(point);
    }




    template<typename PointT>
    class base_sizing_function
    {
    public:

      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;

      typedef PointT point_type;
      typedef NumericType result_type;

      virtual ~base_sizing_function() {}

      virtual NumericType get( PointT const & ) const = 0;
//       virtual NumericType get_area( PointT const &, PointT const &, PointT const & ) const = 0;

    private:
    };

    typedef base_sizing_function<point_1d> base_sizing_function_1d;
    typedef base_sizing_function<point_2d> base_sizing_function_2d;
    typedef base_sizing_function<point_3d> base_sizing_function_3d;

    typedef shared_ptr<base_sizing_function_1d> sizing_function_1d_handle;
    typedef shared_ptr<base_sizing_function_2d> sizing_function_2d_handle;
    typedef shared_ptr<base_sizing_function_3d> sizing_function_3d_handle;







    template<typename MeshT, typename FieldT>
    struct mesh_interpolate_functor : public base_sizing_function< typename viennagrid::result_of::point<MeshT>::type >
    {
      mesh_interpolate_functor( MeshT const & mesh_, FieldT const & field_ ) : mesh(mesh_), field(field_) {}

      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef NumericType result_type;

      NumericType get( PointType const & pt ) const
      {
        typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
        typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

        ConstCellRangeType cells(mesh);
        for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
        {
          if ( viennagrid::is_inside( *cit, pt ) )
          {
            PointType p0 = viennagrid::point( viennagrid::vertices(*cit)[0] );
            PointType p1 = viennagrid::point( viennagrid::vertices(*cit)[1] );
            PointType p2 = viennagrid::point( viennagrid::vertices(*cit)[2] );

            NumericType f0 = std::abs(viennagrid::determinant( p1-pt, p2-pt )) / 2.0;
            NumericType f1 = std::abs(viennagrid::determinant( p0-pt, p2-pt )) / 2.0;
            NumericType f2 = std::abs(viennagrid::determinant( p0-pt, p1-pt )) / 2.0;

            NumericType s0 = field(viennagrid::vertices(*cit)[0]);
            NumericType s1 = field(viennagrid::vertices(*cit)[1]);
            NumericType s2 = field(viennagrid::vertices(*cit)[2]);

            NumericType val = (s0*f0 + s1*f1 + s2*f2) / (f0 + f1 + f2);

            return val;
          }
        }

        return -1;
      }

      MeshT const & mesh;
      FieldT const & field;
    };


    template<typename MeshT, typename FieldT>
    shared_ptr< mesh_interpolate_functor<MeshT, FieldT> > mesh_interpolate( MeshT const & mesh, FieldT const & field )
    {
      return shared_ptr< mesh_interpolate_functor<MeshT, FieldT> >( new mesh_interpolate_functor<MeshT, FieldT>(mesh, field) );
    }





    template<typename MeshT, typename FieldT>
    struct mesh_gradient_functor : public base_sizing_function< typename viennagrid::result_of::point<MeshT>::type >
    {
      mesh_gradient_functor( MeshT const & mesh_, FieldT const & field_ ) : mesh(mesh_), field(field_) {}

      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef NumericType result_type;

      NumericType get( PointType const & pt ) const
      {
        typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
        typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

        ConstCellRangeType cells(mesh);
        for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
        {
          if ( viennagrid::is_inside( *cit, pt ) )
            return gradient( *cit, field );
        }

        return -1;
      }

      MeshT const & mesh;
      FieldT const & field;
    };

    template<typename MeshT, typename FieldT>
    shared_ptr< mesh_gradient_functor<MeshT, FieldT> > mesh_gradient( MeshT const & mesh, FieldT const & field )
    {
      return shared_ptr< mesh_gradient_functor<MeshT, FieldT> >( new mesh_gradient_functor<MeshT, FieldT>(mesh, field) );
    }








    template<typename SegmentT>
    struct distance_to_segment_functor : public base_sizing_function< typename viennagrid::result_of::point<SegmentT>::type >
    {
      distance_to_segment_functor( SegmentT const & segment_ ) : segment(&segment_) {}

      typedef typename viennagrid::result_of::point<SegmentT>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

      NumericType get( PointType const & pt ) const
      {
        return viennagrid::boundary_distance( pt, *segment );
      }

      SegmentT const * segment;
    };

    template<typename SegmentT>
    shared_ptr< distance_to_segment_functor<SegmentT> > mesh_gradient( SegmentT const & segment )
    {
      return shared_ptr< distance_to_segment_functor<SegmentT> >( new distance_to_segment_functor<SegmentT>(segment) );
    }





    template<typename SegmentT>
    struct distance_to_interface_functor : public base_sizing_function< typename viennagrid::result_of::point<SegmentT>::type >
    {
      distance_to_interface_functor( SegmentT const & segment0_, SegmentT const & segment1_ ) : segment0(&segment0_), segment1(&segment1_) {}

      typedef typename viennagrid::result_of::point<SegmentT>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef NumericType result_type;

      NumericType get( PointType const & pt ) const
      {
        typedef typename viennagrid::result_of::facet_tag<SegmentT>::type FacetTag;
        return distance_to_interface<FacetTag>( pt, *segment0, *segment1 );
      }

      SegmentT const * segment0;
      SegmentT const * segment1;
    };

    template<typename SegmentT>
    shared_ptr< distance_to_interface_functor<SegmentT> > distance_to_interface( SegmentT const & segment0, SegmentT const & segment1 )
    {
      return shared_ptr< distance_to_interface_functor<SegmentT> >( new distance_to_interface_functor<SegmentT>(segment0, segment1) );
    }


    template<typename ElementTagT, typename SegmentT>
    struct distance_to_segment_boundaries_functor : public base_sizing_function< typename viennagrid::result_of::point<SegmentT>::type >
    {
      distance_to_segment_boundaries_functor( std::vector<SegmentT const *> segments )
      {
        typedef std::vector<SegmentT const *> SegmentPointerContainerType;
        typedef typename viennagrid::result_of::const_element_range<SegmentT, ElementTagT>::type ConstElementRangeType;
        typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

        if (segments.empty())
          return;

        typename SegmentPointerContainerType::const_iterator seg_it = segments.begin();

        mesh = & ( (*seg_it)->mesh());

        ConstElementRangeType elements(**seg_it++);
        if (elements.empty())
          return;

        for (ConstElementIteratorType fit = elements.begin(); fit != elements.end(); ++fit)
        {
          bool is_on_all_boundaries = true;
          for (typename SegmentPointerContainerType::const_iterator sit = seg_it; sit != segments.end(); ++sit)
          {
            if (!is_boundary(**sit, *fit))
            {
              is_on_all_boundaries = false;
              break;
            }
          }

          if (is_on_all_boundaries)
            boundary_handles.push_back( fit.handle() );
        }
      }

      typedef typename viennagrid::result_of::point<SegmentT>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef NumericType result_type;

      typedef typename SegmentT::mesh_type MeshType;
      typedef typename viennagrid::result_of::const_handle<MeshType, ElementTagT>::type ConstElementHandleType;
      typedef std::vector<ConstElementHandleType> BoundaryElementHandleContainer;

      NumericType get( PointType const & pt ) const
      {
        NumericType min_distance = -1;

        for (typename BoundaryElementHandleContainer::const_iterator beit = boundary_handles.begin(); beit != boundary_handles.end(); ++beit)
        {
          NumericType current_distance = viennagrid::distance(pt, viennagrid::dereference_handle(*mesh, *beit));

          if (min_distance < 0)
            min_distance = current_distance;
          else if (current_distance < min_distance)
            min_distance = current_distance;
        }

        return min_distance;
      }


      MeshType const * mesh;
      BoundaryElementHandleContainer boundary_handles;
    };

    template<typename ElementTypeOrTagT, typename SegmentT>
    shared_ptr< distance_to_segment_boundaries_functor<typename viennagrid::result_of::element_tag<ElementTypeOrTagT>::type, SegmentT> > distance_to_segment_boundaries( SegmentT const & segment0, SegmentT const & segment1, SegmentT const & segment2 )
    {
      typedef typename viennagrid::result_of::element_tag<ElementTypeOrTagT>::type ElementTag;
      std::vector<SegmentT const *> segments;

      segments.push_back(&segment0);
      segments.push_back(&segment1);
      segments.push_back(&segment2);

      return shared_ptr< distance_to_segment_boundaries_functor<ElementTag, SegmentT> >( new distance_to_segment_boundaries_functor<ElementTag, SegmentT>(segments) );
    }




//     template<typename PointT>
//     struct is_in_rect
//     {
//       typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
//       typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;
//
//       typedef NumericType result_type;
//
//       template<typename FunctorT>
//       is_in_rect( FunctorT const & functor_, PointT const & pt0, PointT const & pt1 ) : function(viennamesh::bind(functor_, _1))
//       {
//         ll[0] = std::min(pt0[0], pt1[0]);
//         ll[1] = std::min(pt0[1], pt1[1]);
//
//         ur[0] = std::max(pt0[0], pt1[0]);
//         ur[1] = std::max(pt0[1], pt1[1]);
//       }
//
//       NumericType operator() ( PointT const & pt ) const
//       {
//         if ( (pt[0] >= ll[0]) && (pt[0] <= ur[0]) && (pt[1] >= ll[1]) && (pt[1] <= ur[1]) )
//           return function(pt);
//
//         return -1;
//       }
//
//       PointT ll;
//       PointT ur;
//       SizingFunctionType function;
//     };



    template<typename SegmentT>
    struct is_in_segments_functor : public base_sizing_function<typename viennagrid::result_of::point<SegmentT>::type>
    {
      typedef typename viennagrid::result_of::point<SegmentT>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef typename viennamesh::result_of::sizing_function<PointType>::type SizingFunctionType;

      typedef base_sizing_function<PointType> BaseSizingFunctionType;
      typedef shared_ptr<BaseSizingFunctionType> BaseSizingFunctionHandleType;

      typedef NumericType result_type;


      is_in_segments_functor( std::vector<SegmentT const *> const & segments_, BaseSizingFunctionHandleType const & function_) : segments(segments_), function(function_) {}

      is_in_segments_functor( BaseSizingFunctionHandleType const & function_ ) : function(function_) {}

      is_in_segments_functor( BaseSizingFunctionHandleType const & function_, SegmentT const & segment0_ ) : function(function_)
      {
        segments.push_back(&segment0_);
      }

      is_in_segments_functor( BaseSizingFunctionHandleType const & function_, SegmentT const & segment0_, SegmentT const & segment1_ ) : function(function_)
      {
        segments.push_back(&segment0_);
        segments.push_back(&segment1_);
      }

      NumericType get( PointType const & pt ) const
      {
        typedef typename viennagrid::result_of::const_cell_range<SegmentT>::type ConstCellRangeType;
        typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

        for (unsigned int i = 0; i < segments.size(); ++i)
        {
          ConstCellRangeType cells(*segments[i]);
          for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
          {
            if ( viennagrid::is_inside( *cit, pt ) )
              return function->get(pt);
          }
        }

        return -1;
      }

      std::vector<SegmentT const *> segments;
      BaseSizingFunctionHandleType function;
    };

    template<typename FunctionHandleT, typename SegmentT>
    shared_ptr< is_in_segments_functor<SegmentT> > is_in_segments( FunctionHandleT const & function, SegmentT const & segment0 )
    {
      return shared_ptr< is_in_segments_functor<SegmentT> >( new is_in_segments_functor<SegmentT>(function, segment0) );
    }

    template<typename FunctionHandleT, typename SegmentT>
    shared_ptr< is_in_segments_functor<SegmentT> > is_in_segments( FunctionHandleT const & function, SegmentT const & segment0, SegmentT const & segment1 )
    {
      return shared_ptr< is_in_segments_functor<SegmentT> >( new is_in_segments_functor<SegmentT>(function, segment0, segment1) );
    }









    template<typename SegmentT>
    struct is_near_double_interface_functor : public base_sizing_function<typename viennagrid::result_of::point<SegmentT>::type>
    {
      typedef typename viennagrid::result_of::point<SegmentT>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef typename viennamesh::result_of::sizing_function<PointType>::type SizingFunctionType;

      typedef base_sizing_function<PointType> BaseSizingFunctionType;
      typedef shared_ptr<BaseSizingFunctionType> BaseSizingFunctionHandleType;

      is_near_double_interface_functor( BaseSizingFunctionHandleType const & function_, SegmentT const & segment0_, SegmentT const & segment1_, NumericType max_distance_between_interfaces_ ) : segment0(&segment0_), segment1(&segment1_), function(function_), max_distance_between_interfaces(max_distance_between_interfaces_) {}

      typedef NumericType result_type;

      NumericType get( PointType const & pt ) const
      {
        NumericType distance_segment0 = viennagrid::boundary_distance( *segment0, pt );
        NumericType distance_segment1 = viennagrid::boundary_distance( *segment1, pt );

        if ( distance_segment0 + distance_segment1 < max_distance_between_interfaces )
          return function->get(pt);

        return -1;
      }

      SegmentT const * segment0;
      SegmentT const * segment1;
      BaseSizingFunctionHandleType function;
      NumericType max_distance_between_interfaces;
    };

    template<typename FunctionHandleT, typename SegmentT>
    shared_ptr< is_near_double_interface_functor<SegmentT> > is_near_double_interface( FunctionHandleT const & function, SegmentT const & segment0, SegmentT const & segment1, double max_distance_between_interfaces )
    {
      return shared_ptr< is_near_double_interface_functor<SegmentT> >( new is_near_double_interface_functor<SegmentT>(function, segment0, segment1, max_distance_between_interfaces) );
    }


/*

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
    };*/




    template<typename PointT>
    struct min_functor : public base_sizing_function<PointT>
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
      typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;
      typedef NumericType result_type;

      typedef base_sizing_function<PointT> BaseSizingFunctionType;
      typedef shared_ptr<BaseSizingFunctionType> BaseSizingFunctionHandleType;
      typedef std::vector<BaseSizingFunctionHandleType> BaseSizingFunctionHandleContainerType;

      min_functor(BaseSizingFunctionHandleContainerType const & functions_) : functions(functions_) {}

      min_functor(BaseSizingFunctionHandleType const & function1_, BaseSizingFunctionHandleType const & function2_)
      {
        functions.push_back(function1_);
        functions.push_back(function2_);
      }

      NumericType get( PointT const & pt ) const
      {
        NumericType val = -1;

        for (typename BaseSizingFunctionHandleContainerType::const_iterator fit = functions.begin(); fit != functions.end(); ++fit)
        {
          NumericType val_f = (*fit)->get(pt);
          if (val_f < 0)
            continue;

          if (val < 0)
            val = val_f;
          else if (val_f < val)
            val = val_f;
        }

        return val;
      }

      BaseSizingFunctionHandleContainerType functions;
    };

    template<typename SizingFunction1T, typename SizingFunction2T>
    shared_ptr< base_sizing_function<typename SizingFunction1T::point_type> > min( shared_ptr<SizingFunction1T> const & function1, shared_ptr<SizingFunction2T> const & function2 )
    {
      return shared_ptr< min_functor<typename SizingFunction1T::point_type> >( new min_functor<typename SizingFunction1T::point_type>(function1, function2) );
    }



    template<typename PointT>
    struct max_functor : public base_sizing_function<PointT>
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
      typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;
      typedef NumericType result_type;

      typedef base_sizing_function<PointT> BaseSizingFunctionType;
      typedef shared_ptr<BaseSizingFunctionType> BaseSizingFunctionHandleType;
      typedef std::vector<BaseSizingFunctionHandleType> BaseSizingFunctionHandleContainerType;

      max_functor(BaseSizingFunctionHandleContainerType const & functions_) : functions(functions_) {}

      max_functor(BaseSizingFunctionHandleType const & function1_, BaseSizingFunctionHandleType const & function2_)
      {
        functions.push_back(function1_);
        functions.push_back(function2_);
      }

      NumericType get( PointT const & pt ) const
      {
        NumericType val = -1;

        for (typename BaseSizingFunctionHandleContainerType::const_iterator fit = functions.begin(); fit != functions.end(); ++fit)
        {
          NumericType val_f = (*fit)->get(pt);
          if (val_f < 0)
            continue;

          if (val < 0)
            val = val_f;
          else if (val_f > val)
            val = val_f;
        }

        return val;
      }

      BaseSizingFunctionHandleContainerType functions;
    };



//     template<typename PointT>
//     struct max
//     {
//       typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
//       typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;
//
//       template<typename Functor1T, typename Functor2T>
//       max(Functor1T const & functor1_, Functor2T const & functor2_) : function1(viennamesh::bind(functor1_, _1)), function2(viennamesh::bind(functor2_, _1)) {}
//
//       NumericType operator() ( PointT const & pt ) const
//       { return std::max( function1(pt), function2(pt) ); }
//
//       SizingFunctionType function1;
//       SizingFunctionType function2;
//     };

    template<typename PointT>
    struct constant_functor : public base_sizing_function<PointT>
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
      typedef double result_type;

      constant_functor(NumericType value_) : value(value_) {}

      NumericType get( PointT const & ) const
      { return value; }

      NumericType value;
    };

    template<typename PointT>
    shared_ptr< base_sizing_function<PointT> > constant( typename viennagrid::result_of::coord<PointT>::type value )
    {
      return shared_ptr< constant_functor<PointT> >( new constant_functor<PointT>(value) );
    }


//     template<typename PointT>
//     struct map_to_valid
//     {
//       typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
//       typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;
//
//       template<typename FunctorT>
//       map_to_valid(FunctorT const & functor_, NumericType min_value_, NumericType max_value_) : function(viennamesh::bind(functor_, _1)), min_value(min_value_), max_value(max_value_) {}
//
//       NumericType operator() ( PointT const & pt ) const
//       {
//         NumericType tmp = function1(pt);
//         if (tmp <= 0)
//           return max_value;
//         return std::max(tmp, min_value);
//       }
//
//       SizingFunctionType function;
//
//       NumericType min_value;
//       NumericType max_value;
//     };


    template<typename PointT>
    struct linear_interpolate_functor : public base_sizing_function<PointT>
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
      typedef NumericType result_type;

      typedef base_sizing_function<PointT> BaseSizingFunctionType;
      typedef shared_ptr<BaseSizingFunctionType> BaseSizingFunctionHandleType;

      linear_interpolate_functor(BaseSizingFunctionHandleType const & function_, NumericType lower_, NumericType upper_, NumericType lower_to_, NumericType upper_to_) : function(function_), lower(lower_), upper(upper_), lower_to(lower_to_), upper_to(upper_to_) {}

      NumericType get( PointT const & pt ) const
      {
        NumericType tmp = function->get(pt);
        if (tmp <= 0)
          return tmp;

        if (tmp < lower)
          return lower_to;
        if (tmp > upper)
          return upper_to;

        return lower_to + (tmp-lower)/(upper-lower)*(upper_to-lower_to);
      }

      BaseSizingFunctionHandleType function;

      NumericType lower;
      NumericType upper;

      NumericType lower_to;
      NumericType upper_to;
    };


    template<typename SizingFunctionT>
    shared_ptr< base_sizing_function< typename SizingFunctionT::point_type > > linear_interpolate(
          shared_ptr<SizingFunctionT> const & function,
          typename viennagrid::result_of::coord<typename SizingFunctionT::point_type>::type lower,
          typename viennagrid::result_of::coord<typename SizingFunctionT::point_type>::type upper,
          typename viennagrid::result_of::coord<typename SizingFunctionT::point_type>::type lower_to,
          typename viennagrid::result_of::coord<typename SizingFunctionT::point_type>::type upper_to )
    {
      typedef typename SizingFunctionT::point_type PointType;
      return shared_ptr< linear_interpolate_functor<PointType> >( new linear_interpolate_functor<PointType>(function, lower, upper, lower_to, upper_to) );
    }







    template<typename MeshT, typename SegmentationT>
    shared_ptr< base_sizing_function< typename viennagrid::result_of::point<MeshT>::type > > from_xml(
          pugi::xml_node const & node,
          MeshT const & mesh,
          SegmentationT const & segmentation)
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef shared_ptr< base_sizing_function<PointType> > SizingFunctionHandleType;

      typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentHandleType;
      typedef typename viennagrid::result_of::segment_id<SegmentationT>::type SegmentIDType;

      string name = node.name();

      if (name == "constant")
      {
        double value = lexical_cast<double>(node.child_value("value"));
        return SizingFunctionHandleType( new constant_functor<PointType>(value) );
      }

      if (name == "min")
      {
        std::vector<SizingFunctionHandleType> functions;
        for (pugi::xml_node source = node.child("source"); source; source = source.next_sibling("source"))
          functions.push_back( from_xml(source, mesh, segmentation) );

        return SizingFunctionHandleType( new min_functor<PointType>(functions) );
      }

      if (name == "max")
      {
        std::vector<SizingFunctionHandleType> functions;
        for (pugi::xml_node source = node.child("source"); source; source = source.next_sibling("source"))
          functions.push_back( from_xml(source.first_child(), mesh, segmentation) );

        return SizingFunctionHandleType( new max_functor<PointType>(functions) );
      }

      if (name == "interpolate")
      {
        pugi::xml_attribute transform_type_node = node.attribute("transform_type");
        string transform_type = transform_type_node.as_string();

        if (transform_type == "linear")
        {
          double lower = lexical_cast<double>(node.child_value("lower"));
          double upper = lexical_cast<double>(node.child_value("upper"));
          double lower_to = lexical_cast<double>(node.child_value("lower_to"));
          double upper_to = lexical_cast<double>(node.child_value("upper_to"));

          SizingFunctionHandleType source = from_xml(node.child("source").first_child(), mesh, segmentation);

          return SizingFunctionHandleType(new linear_interpolate_functor<PointType>(source, lower, upper, lower_to, upper_to ));
        }

        return SizingFunctionHandleType();
      }

      if (name == "distance_to_segment_boundaries")
      {
        std::vector<SegmentHandleType const*> segments;
        for (pugi::xml_node segment = node.child("segment"); segment; segment = segment.next_sibling("segment"))
          segments.push_back( &segmentation.get_segment( lexical_cast<SegmentIDType>(segment.text().as_string()) ) );

        return SizingFunctionHandleType( new distance_to_segment_boundaries_functor<viennagrid::line_tag, SegmentHandleType>(segments) );
      }

      if (name == "is_in_segments")
      {
        std::vector<SegmentHandleType const*> segments;
        for (pugi::xml_node segment = node.child("segment"); segment; segment = segment.next_sibling("segment"))
          segments.push_back( &segmentation.get_segment( lexical_cast<SegmentIDType>(segment.text().as_string()) ) );

        SizingFunctionHandleType source = from_xml(node.child("source").first_child(), mesh, segmentation);

        return SizingFunctionHandleType( new is_in_segments_functor<SegmentHandleType>(segments, source) );
      }

      return SizingFunctionHandleType();
    }

  }
}

#endif
