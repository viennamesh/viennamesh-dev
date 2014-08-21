#ifndef VIENNAMESH_CORE_SIZING_FUNCTION_HPP
#define VIENNAMESH_CORE_SIZING_FUNCTION_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "viennagrid/algorithm/interface.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/inclusion.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/io/vtk_reader.hpp"

#include "viennamesh/utils/pugixml/pugixml.hpp"

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



  namespace sizing_function
  {
//     template<typename MeshT, typename SegmentationT, typename FieldT>
//     struct mesh_interpolate_functor : public base_sizing_function<MeshT, SegmentationT>
//     {
//       mesh_interpolate_functor( MeshT const & mesh_, FieldT const & field_ ) : mesh(mesh_), field(field_) {}
//
//       typedef typename viennagrid::result_of::point<MeshT>::type PointType;
//       typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
//       typedef NumericType result_type;
//
//       NumericType get( PointType const & pt ) const
//       {
//         typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
//         typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;
//
//         ConstCellRangeType cells(mesh);
//         for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
//         {
//           if ( viennagrid::is_inside( *cit, pt ) )
//           {
//             PointType p0 = viennagrid::point( viennagrid::vertices(*cit)[0] );
//             PointType p1 = viennagrid::point( viennagrid::vertices(*cit)[1] );
//             PointType p2 = viennagrid::point( viennagrid::vertices(*cit)[2] );
//
//             NumericType f0 = std::abs(viennagrid::determinant( p1-pt, p2-pt )) / 2.0;
//             NumericType f1 = std::abs(viennagrid::determinant( p0-pt, p2-pt )) / 2.0;
//             NumericType f2 = std::abs(viennagrid::determinant( p0-pt, p1-pt )) / 2.0;
//
//             NumericType s0 = field(viennagrid::vertices(*cit)[0]);
//             NumericType s1 = field(viennagrid::vertices(*cit)[1]);
//             NumericType s2 = field(viennagrid::vertices(*cit)[2]);
//
//             NumericType val = (s0*f0 + s1*f1 + s2*f2) / (f0 + f1 + f2);
//
//             return val;
//           }
//         }
//
//         return -1;
//       }
//
//       MeshT const & mesh;
//       FieldT const & field;
//     };


    template<typename MeshT, typename SegmentationT>
    struct mesh_gradient_functor
    {
      typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

      mesh_gradient_functor( std::string const & filename, std::string const & quantity_name ) :
          mesh(viennamesh::make_parameter<SegmentedMeshType>()),
          values(new ValuesContainerType), values_field(new FieldType(*values))
      {
        viennagrid::io::vtk_reader<MeshT, SegmentationT> reader;
        viennagrid::io::add_scalar_data_on_vertices( reader, *values_field, quantity_name );
        reader( mesh().mesh, mesh().segmentation, filename );
      }

      typedef typename viennagrid::result_of::vertex<MeshT>::type VertexType;

      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef NumericType result_type;

      NumericType operator()( PointType const & pt ) const
      {
        typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
        typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

        ConstCellRangeType cells(mesh().mesh);
        for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
        {
          if ( viennagrid::is_inside( *cit, pt ) )
            return gradient( *cit, *values_field );
        }

        return -1;
      }

      typedef std::vector<NumericType> ValuesContainerType;
      typedef typename viennagrid::result_of::field<ValuesContainerType, VertexType>::type FieldType;

      typename viennamesh::result_of::parameter_handle< SegmentedMeshType>::type mesh;

      shared_ptr<ValuesContainerType> values;
      shared_ptr<FieldType> values_field;
    };


//     template<typename MeshT, typename SegmentationT, typename FieldT>
//     struct referenced_mesh_gradient_functor : public base_sizing_function<MeshT, SegmentationT>
//     {
//       referenced_mesh_gradient_functor( MeshT const & mesh_, FieldT const & field_ ) : mesh(mesh_), field(field_) {}
//
//       typedef typename viennagrid::result_of::point<MeshT>::type PointType;
//       typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
//       typedef NumericType result_type;
//
//       NumericType get( PointType const & pt ) const
//       {
//         typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
//         typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;
//
//         ConstCellRangeType cells(mesh);
//         for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
//         {
//           if ( viennagrid::is_inside( *cit, pt ) )
//             return gradient( *cit, field );
//         }
//
//         return -1;
//       }
//
//       MeshT const & mesh;
//       FieldT const & field;
//     };









//     template<typename MeshT, typename SegmentationT>
//     struct distance_to_segment_functor : public base_sizing_function<MeshT, SegmentationT>
//     {
//       typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentType;
//
//       distance_to_segment_functor( SegmentType const & segment_ ) : segment(&segment_) {}
//
//       typedef typename viennagrid::result_of::point<MeshT>::type PointType;
//       typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
//
//       NumericType get( PointType const & pt ) const
//       {
//         return viennagrid::boundary_distance( pt, *segment );
//       }
//
//       SegmentType const * segment;
//     };






    template<typename MeshT, typename SegmentationT>
    struct distance_to_interface_functor
    {
      typedef typename viennamesh::result_of::const_parameter_handle< viennagrid::segmented_mesh<MeshT, SegmentationT> >::type ConstMeshParameterType;
      typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentType;

      distance_to_interface_functor( ConstMeshParameterType const & mesh_,
                                     std::string const & segment0_name_,
                                     std::string const & segment1_name_ ) :
                                     mesh(mesh_), segment0_name(segment0_name_), segment1_name(segment1_name_) {}

      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef NumericType result_type;

      NumericType operator()( PointType const & pt ) const
      {
        typedef typename viennagrid::result_of::facet_tag<SegmentType>::type FacetTag;
        return distance_to_interface<FacetTag>( pt,
                                                mesh().segmentation(segment0_name),
                                                mesh().segmentation(segment1_name) );
      }

      ConstMeshParameterType mesh;
      std::string segment0_name;
      std::string segment1_name;
    };




    template<typename ElementTagT, typename MeshT, typename SegmentationT>
    struct distance_to_segment_boundaries_functor
    {
      typedef typename viennamesh::result_of::const_parameter_handle< viennagrid::segmented_mesh<MeshT, SegmentationT> >::type ConstMeshParameterType;
      typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentType;

      distance_to_segment_boundaries_functor( ConstMeshParameterType const & mesh_,
                                              std::vector<std::string> const & segment_names ) : mesh(mesh_), boundary_handles(new BoundaryElementHandleContainer)
      {
        typedef typename viennagrid::result_of::const_element_range<SegmentType, ElementTagT>::type ConstElementRangeType;
        typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

        if (segment_names.empty())
          throw create_sizing_function_exception( "distance_to_segment_boundaries_functor: No segment names specified" );

        std::vector<std::string>::const_iterator seg_it = segment_names.begin();
        ConstElementRangeType elements( mesh().segmentation(*seg_it++) );

        if (elements.empty())
          throw create_sizing_function_exception( "distance_to_segment_boundaries_functor: No elements found in mesh" );

        for (std::vector<std::string>::const_iterator snit = segment_names.begin(); snit != segment_names.end(); ++snit)
        {
          if ( !mesh().segmentation.segment_present( *snit ) )
          {
            std::stringstream ss;

            ss << "distance_to_segment_boundaries_functor: Segment \"" << *snit << "\" not found in segmentation" << std::endl;
            ss << "Available segments: ";

            for (typename SegmentationT::const_iterator sit = mesh().segmentation.begin(); sit != mesh().segmentation.end(); ++sit)
            {
              if (sit != mesh().segmentation.begin())
                ss << ", ";
              ss << "\"" << (*sit).name() << "\"";
            }

            throw create_sizing_function_exception(ss.str());
          }
        }

        for (ConstElementIteratorType fit = elements.begin(); fit != elements.end(); ++fit)
        {
          bool is_on_all_boundaries = true;
          for (std::vector<std::string>::const_iterator sit = seg_it; sit != segment_names.end(); ++sit)
          {
            if (!is_boundary(mesh().segmentation(*sit), *fit))
            {
              is_on_all_boundaries = false;
              break;
            }
          }

          if (is_on_all_boundaries)
            boundary_handles->push_back( fit.handle() );
        }

        if (boundary_handles->empty())
        {
          std::stringstream ss;

          ss << "distance_to_segment_boundaries_functor: no elements found which are boundary elements of the segments: ";

          for (std::vector<std::string>::const_iterator snit = segment_names.begin(); snit != segment_names.end(); ++snit)
          {
            if (snit != segment_names.begin())
              ss << ", ";
            ss << "\"" << *snit << "\"";
          }

          throw create_sizing_function_exception(ss.str());
        }
      }

      typedef typename viennagrid::result_of::point<SegmentType>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef NumericType result_type;

      typedef typename SegmentType::mesh_type MeshType;
      typedef typename viennagrid::result_of::const_handle<MeshType, ElementTagT>::type ConstElementHandleType;
      typedef std::vector<ConstElementHandleType> BoundaryElementHandleContainer;

      NumericType operator()( PointType const & pt ) const
      {
        NumericType min_distance = -1;

        for (typename BoundaryElementHandleContainer::const_iterator beit = boundary_handles->begin(); beit != boundary_handles->end(); ++beit)
        {
          NumericType current_distance = viennagrid::distance(pt, viennagrid::dereference_handle(mesh().mesh, *beit));

          if (min_distance < 0)
            min_distance = current_distance;
          else if (current_distance < min_distance)
            min_distance = current_distance;
        }

        return min_distance;
      }


      ConstMeshParameterType mesh;
      shared_ptr<BoundaryElementHandleContainer> boundary_handles;
    };




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



    template<typename MeshT, typename SegmentationT>
    struct is_in_segments_functor
    {
      typedef typename viennamesh::result_of::const_parameter_handle< viennagrid::segmented_mesh<MeshT, SegmentationT> >::type ConstMeshParameterType;
      typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentType;

      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef typename viennamesh::result_of::sizing_function<PointType>::type SizingFunctionType;

      typedef NumericType result_type;


      is_in_segments_functor( ConstMeshParameterType mesh_,
                              std::vector<std::string> const & segment_names_,
                              SizingFunctionType const & function_) :
                              mesh(mesh_), segment_names(segment_names_), function(function_)
      {
        for (std::vector<std::string>::const_iterator snit = segment_names.begin(); snit != segment_names.end(); ++snit)
        {
          if ( !mesh().segmentation.segment_present( *snit ) )
          {
            std::stringstream ss;

            ss << "distance_to_segment_boundaries_functor: Segment \"" << *snit << "\" not found in segmentation" << std::endl;
            ss << "Available segments: ";

            for (typename SegmentationT::const_iterator sit = mesh().segmentation.begin(); sit != mesh().segmentation.end(); ++sit)
            {
              if (sit != mesh().segmentation.begin())
                ss << ", ";
              ss << "\"" << (*sit).name() << "\"";
            }

            throw create_sizing_function_exception(ss.str());
          }
        }
      }

      NumericType operator()( PointType const & pt ) const
      {
        typedef typename viennagrid::result_of::const_cell_range<SegmentType>::type ConstCellRangeType;
        typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

        for (unsigned int i = 0; i < segment_names.size(); ++i)
        {
          ConstCellRangeType cells( mesh().segmentation(segment_names[i]) );
          for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
          {
            if ( viennagrid::is_inside( *cit, pt ) )
              return function(pt);
          }
        }

        return -1;
      }

      ConstMeshParameterType mesh;
      std::vector<std::string> segment_names;

      SizingFunctionType function;
    };










//     template<typename MeshT, typename SegmentationT>
//     struct is_near_double_interface_functor : public base_sizing_function< typename viennagrid::result_of::point<MeshT>::type >
//     {
//       typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type SegmentType;
//
//       typedef typename viennagrid::result_of::point<MeshT>::type PointType;
//       typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
//       typedef typename viennamesh::result_of::sizing_function<PointType>::type SizingFunctionType;
//
//       typedef base_sizing_function<MeshT, SegmentationT> BaseSizingFunctionType;
//       typedef shared_ptr<BaseSizingFunctionType> BaseSizingFunctionHandleType;
//
//       is_near_double_interface_functor( BaseSizingFunctionHandleType const & function_, SegmentType const & segment0_, SegmentType const & segment1_, NumericType max_distance_between_interfaces_ ) : segment0(&segment0_), segment1(&segment1_), function(function_), max_distance_between_interfaces(max_distance_between_interfaces_) {}
//
//       typedef NumericType result_type;
//
//       NumericType get( PointType const & pt ) const
//       {
//         NumericType distance_segment0 = viennagrid::boundary_distance( *segment0, pt );
//         NumericType distance_segment1 = viennagrid::boundary_distance( *segment1, pt );
//
//         if ( distance_segment0 + distance_segment1 < max_distance_between_interfaces )
//           return function->get(pt);
//
//         return -1;
//       }
//
//       SegmentType const * segment0;
//       SegmentType const * segment1;
//       BaseSizingFunctionHandleType function;
//       NumericType max_distance_between_interfaces;
//     };




    template<typename PointT>
    struct min_functor
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
      typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;
      typedef NumericType result_type;

      typedef std::vector<SizingFunctionType> SizingFunctionContainerType;

      min_functor(SizingFunctionContainerType const & functions_) : functions(functions_) {}

      NumericType operator()( PointT const & pt ) const
      {
        NumericType val = -1;

        for (typename SizingFunctionContainerType::const_iterator fit = functions.begin(); fit != functions.end(); ++fit)
        {
          NumericType val_f = (*fit)(pt);
          if (val_f < 0)
            continue;

          if (val < 0)
            val = val_f;
          else if (val_f < val)
            val = val_f;
        }

        return val;
      }

      SizingFunctionContainerType functions;
    };



    template<typename PointT>
    struct max_functor
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
      typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;
      typedef NumericType result_type;

      typedef std::vector<SizingFunctionType> SizingFunctionContainerType;

      max_functor(SizingFunctionContainerType const & functions_) : functions(functions_) {}

      NumericType operator()( PointT const & pt ) const
      {
        NumericType val = -1;

        for (typename SizingFunctionContainerType::const_iterator fit = functions.begin(); fit != functions.end(); ++fit)
        {
          NumericType val_f = (*fit)(pt);
          if (val_f < 0)
            continue;

          if (val < 0)
            val = val_f;
          else if (val_f > val)
            val = val_f;
        }

        return val;
      }

      SizingFunctionContainerType functions;
    };




    template<typename PointT>
    struct constant_functor
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
      typedef double result_type;

      constant_functor(NumericType value_) : value(value_) {}

      NumericType operator()( PointT const & ) const
      { return value; }

      NumericType value;
    };


    template<typename PointT>
    struct linear_interpolate_functor
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
      typedef typename viennamesh::result_of::sizing_function<PointT>::type SizingFunctionType;
      typedef NumericType result_type;

      linear_interpolate_functor(SizingFunctionType const & function_, NumericType lower_, NumericType upper_, NumericType lower_to_, NumericType upper_to_) : function(function_), lower(lower_), upper(upper_), lower_to(lower_to_), upper_to(upper_to_) {}

      NumericType operator()( PointT const & pt ) const
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

    template<typename MeshT, typename SegmentationT>
    typename viennamesh::result_of::sizing_function< typename viennagrid::result_of::point<MeshT>::type >::type
    from_xml(
          pugi::xml_node const & node,
          viennamesh::parameter_handle_t< parameter_wrapper< viennagrid::segmented_mesh<MeshT, SegmentationT > > > const & mesh,
          std::string const & base_path = "")
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennamesh::result_of::sizing_function<PointType>::type SizingFunctionType;

      std::string name = node.name();

      if (name == "constant")
      {
        if ( !node.child_value("value") )
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"value\" missing" );

        double value = lexical_cast<double>(node.child_value("value"));
        return viennamesh::bind( constant_functor<PointType>(value), _1 );
      }
      else if (name == "min")
      {
        std::vector<SizingFunctionType> functions;
        for (pugi::xml_node source = node.child("source"); source; source = source.next_sibling("source"))
          functions.push_back( from_xml<MeshT, SegmentationT>(source.first_child(), mesh, base_path) );

        if (functions.empty())
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": no sources specified" );

        return viennamesh::bind( min_functor<PointType>(functions), _1 );
      }
      else if (name == "max")
      {
        std::vector<SizingFunctionType> functions;
        for (pugi::xml_node source = node.child("source"); source; source = source.next_sibling("source"))
          functions.push_back( from_xml<MeshT, SegmentationT>(source.first_child(), mesh, base_path) );

        if (functions.empty())
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": no sources specified" );

        return viennamesh::bind( max_functor<PointType>(functions), _1 );
      }
      else if (name == "interpolate")
      {
        pugi::xml_attribute transform_type_node = node.attribute("transform_type");
        std::string transform_type = transform_type_node.as_string();

        if ( !node.child_value("source") )
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"source\" missing" );

        SizingFunctionType source = from_xml<MeshT, SegmentationT>(node.child("source").first_child(), mesh, base_path);

        if (transform_type == "linear")
        {
          if ( !node.child_value("lower") )
            throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"lower\" missing" );
          if ( !node.child_value("upper") )
            throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"upper\" missing" );
          if ( !node.child_value("lower_to") )
            throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"lower_to\" missing" );
          if ( !node.child_value("upper_to") )
            throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"upper_to\" missing" );

          double lower = lexical_cast<double>(node.child_value("lower"));
          double upper = lexical_cast<double>(node.child_value("upper"));
          double lower_to = lexical_cast<double>(node.child_value("lower_to"));
          double upper_to = lexical_cast<double>(node.child_value("upper_to"));

          return viennamesh::bind(linear_interpolate_functor<PointType>(source, lower, upper, lower_to, upper_to ), _1);
        }

        return SizingFunctionType();
      }
      else if (name == "distance_to_segment_boundaries")
      {
        std::vector<std::string> segment_names;
        for (pugi::xml_node segment = node.child("segment"); segment; segment = segment.next_sibling("segment"))
          segment_names.push_back( segment.text().as_string() );

        if (segment_names.empty())
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": no segment names specified" );

        std::string element_type = node.child_value("element_type");
        if (element_type == "line")
          return viennamesh::bind( distance_to_segment_boundaries_functor<viennagrid::line_tag, MeshT, SegmentationT>(mesh, segment_names), _1 );
        else if (element_type == "facet")
          return viennamesh::bind( distance_to_segment_boundaries_functor<typename viennagrid::result_of::facet_tag<MeshT>::type, MeshT, SegmentationT>(mesh, segment_names), _1 );
        else
          throw create_sizing_function_exception( "distance_to_segment_boundaries: Element type \"" + element_type + "\" not supported" );
      }
      else if (name == "distance_to_interface")
      {
        std::vector<std::string> segment_names;
        for (pugi::xml_node segment = node.child("segment"); segment; segment = segment.next_sibling("segment"))
          segment_names.push_back( segment.text().as_string() );

        if (segment_names.empty())
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": no segment names specified" );

        return viennamesh::bind( distance_to_interface_functor<MeshT, SegmentationT>(mesh, segment_names[0], segment_names[1]), _1 );
      }
      else if (name == "is_in_segments")
      {
        std::vector<std::string> segment_names;
        for (pugi::xml_node segment = node.child("segment"); segment; segment = segment.next_sibling("segment"))
          segment_names.push_back( segment.text().as_string() );

        SizingFunctionType source = from_xml<MeshT, SegmentationT>(node.child("source").first_child(), mesh, base_path);

        return viennamesh::bind( is_in_segments_functor<MeshT, SegmentationT>(mesh, segment_names, source), _1 );
      }
      else if (name == "mesh_gradient")
      {
        if ( !node.child_value("mesh_file") )
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"mesh_file\" missing" );
        if ( !node.child_value("quantity_name") )
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"quantity_name\" missing" );

        std::string mesh_file = node.child_value("mesh_file");
        if (!base_path.empty())
          mesh_file = base_path + "/" + mesh_file;

        std::string quantity_name = node.child_value("quantity_name");

        return viennamesh::bind( mesh_gradient_functor<MeshT, SegmentationT>(mesh_file, quantity_name), _1 );
      }

      throw create_sizing_function_exception( "Sizing function functor \"" + name + "\" not supported" );

      return SizingFunctionType();
    }

    template<typename MeshT, typename SegmentationT>
    typename viennamesh::result_of::sizing_function< typename viennagrid::result_of::point<MeshT>::type >::type from_xml(
          std::string const & xml_string,
          viennamesh::parameter_handle_t< parameter_wrapper< viennagrid::segmented_mesh<MeshT, SegmentationT > > > const & mesh,
          std::string const & base_path = "")
    {
      pugi::xml_document sf_xml;
      sf_xml.load( xml_string.c_str() );
      return from_xml<MeshT, SegmentationT>( sf_xml.first_child(), mesh, base_path );
    }

    template<typename MeshT, typename SegmentationT>
    typename viennamesh::result_of::sizing_function< typename viennagrid::result_of::point<MeshT>::type >::type from_xmlfile(
          std::string const & xml_filename,
          viennamesh::parameter_handle_t< parameter_wrapper< viennagrid::segmented_mesh<MeshT, SegmentationT > > > const & mesh,
          std::string const & base_path = "")
    {
      pugi::xml_document sf_xml;
      sf_xml.load_file( xml_filename.c_str() );
      return from_xml<MeshT, SegmentationT>( sf_xml.first_child(), mesh, base_path );
    }

  }
}

#endif
