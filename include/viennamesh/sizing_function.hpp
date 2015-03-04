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

#include <memory>
#include <functional>

#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/inclusion.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/io/vtk_reader.hpp"

#include "viennamesh/exceptions.hpp"

#include "pugixml.hpp"

namespace viennamesh
{
  using std::placeholders::_1;
  using std::placeholders::_2;
  typedef std::function< double( viennagrid::point_t const & ) > SizingFunctionType;



  template<typename ElementT, typename AccessorFieldT>
  typename viennagrid::result_of::coord<ElementT>::type gradient( ElementT const & element, AccessorFieldT const & accessor_field )
  {
    typedef typename viennagrid::result_of::point<ElementT>::type PointType;
    typedef typename viennagrid::result_of::coord<ElementT>::type NumericType;

    PointType p0 = viennagrid::get_point( viennagrid::vertices(element)[0] );
    PointType p1 = viennagrid::get_point( viennagrid::vertices(element)[1] );
    PointType p2 = viennagrid::get_point( viennagrid::vertices(element)[2] );

    NumericType s0 = accessor_field.get(viennagrid::vertices(element)[0]);
    NumericType s1 = accessor_field.get(viennagrid::vertices(element)[1]);
    NumericType s2 = accessor_field.get(viennagrid::vertices(element)[2]);


    PointType p10 = p1-p0;
    PointType p20 = p2-p0;

    NumericType det = viennagrid::determinant( p10, p20 );

    PointType M0(2);
    PointType M1(2);

    M0[0] = p20[1];
    M0[1] = -p10[1];

    M1[0] = -p20[0];
    M1[1] = p10[0];

    M0 /= det;
    M1 /= det;

    PointType s(2);
    s[0] = s1-s0;
    s[1] = s2-s0;

    NumericType gradient = std::abs(viennagrid::inner_prod(M0, s)) + std::abs(viennagrid::inner_prod(M1, s));

    return gradient;
  }



  template<typename PointT, typename RegionT>
  typename viennagrid::result_of::coord<RegionT>::type distance_to_interface(PointT const & point,
                                                                              RegionT const & seg0,
                                                                              RegionT const & seg1,
                                                                              viennagrid_dimension topologic_dimension)
  {
    typedef typename viennagrid::result_of::const_element_range<RegionT>::type ConstElementRangeType;
    typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

    typedef typename viennagrid::result_of::coord<RegionT>::type CoordType;

    ConstElementRangeType elements(seg0, topologic_dimension);
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
//     template<typename MeshT, typename RegionationT, typename FieldT>
//     struct mesh_interpolate_functor : public base_sizing_function<MeshT, RegionationT>
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


    struct mesh_gradient_functor
    {
      typedef viennagrid::mesh_t MeshType;
      typedef viennagrid::result_of::element<MeshType>::type ElementType;

      typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      typedef viennagrid::quantity_field QuantityFieldType;



      mesh_gradient_functor( std::string const & filename, std::string const & quantity_name )
      {
        QuantityFieldType quantities;
        viennagrid::io::vtk_reader<MeshType> reader;
        viennagrid::io::add_scalar_data_on_vertices( reader, quantities, quantity_name );
        reader( mesh, filename );

        ConstCellRangeType cells(mesh);
        gradient_accessor.set_values_dimension(1);
        gradient_accessor.resize( cells.size() );

        for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
        {
          gradient_accessor.set(*cit, viennamesh::gradient(*cit, quantities));
        }
      }

      typedef typename viennagrid::result_of::element<MeshType>::type VertexType;

      typedef typename viennagrid::result_of::point<MeshType>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef NumericType result_type;

      NumericType operator()( PointType const & pt ) const
      {

        ConstCellRangeType cells(mesh);
        for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
        {
          if ( viennagrid::is_inside( *cit, pt ) )
          {
            NumericType grad = gradient_accessor.get(*cit);
//             info(1) << "  gradient = " << grad << std::endl;
            return grad;
          }
//             return gradient( *cit, quantities );
        }

        return -1;
      }

      MeshType mesh;

      QuantityFieldType gradient_accessor;
    };


//     template<typename MeshT, typename RegionationT, typename FieldT>
//     struct referenced_mesh_gradient_functor : public base_sizing_function<MeshT, RegionationT>
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









//     template<typename MeshT, typename RegionationT>
//     struct distance_to_region_functor : public base_sizing_function<MeshT, RegionationT>
//     {
//       typedef typename viennagrid::result_of::region_handle<RegionationT>::type RegionType;
//
//       distance_to_region_functor( RegionType const & region_ ) : region(&region_) {}
//
//       typedef typename viennagrid::result_of::point<MeshT>::type PointType;
//       typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
//
//       NumericType get( PointType const & pt ) const
//       {
//         return viennagrid::boundary_distance( pt, *region );
//       }
//
//       RegionType const * region;
//     };






    struct distance_to_interface_functor
    {
      typedef viennagrid::mesh_t MeshType;

      distance_to_interface_functor( MeshType const & mesh_,
                                     std::string const & region0_name_,
                                     std::string const & region1_name_ ) :
                                     mesh(mesh_), facet_dimension(viennagrid::facet_dimension(mesh_)), region0_name(region0_name_), region1_name(region1_name_) {}

      typedef typename viennagrid::result_of::point<MeshType>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef NumericType result_type;

      NumericType operator()( PointType const & pt ) const
      {
        return distance_to_interface( pt,
                                      mesh.get_region(region0_name),
                                      mesh.get_region(region1_name),
                                      facet_dimension );
      }

      MeshType mesh;
      viennagrid_dimension facet_dimension;

      std::string region0_name;
      std::string region1_name;
    };




    struct distance_to_region_boundaries_functor
    {
      typedef viennagrid::mesh_t MeshType;
      typedef typename viennagrid::result_of::region<MeshType>::type RegionType;

      distance_to_region_boundaries_functor(MeshType const & mesh_,
                                            std::vector<std::string> const & region_names,
                                            viennagrid_dimension topologic_dimension) :
                                            mesh(mesh_), boundary_elements(new BoundaryElementContainer)
      {
        typedef typename viennagrid::result_of::const_element_range<RegionType>::type ConstElementRangeType;
        typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIterator;

        if (region_names.empty())
          throw create_sizing_function_exception( "distance_to_region_boundaries_functor: No region names specified" );

        std::vector<std::string>::const_iterator seg_it = region_names.begin();
        ConstElementRangeType elements( mesh.get_region(*seg_it++), topologic_dimension );

        if (elements.empty())
          throw create_sizing_function_exception( "distance_to_region_boundaries_functor: No elements found in mesh" );

        for (std::vector<std::string>::const_iterator snit = region_names.begin(); snit != region_names.end(); ++snit)
        {
          if ( !mesh.region_exists( *snit ) )
          {
            std::stringstream ss;

            ss << "distance_to_region_boundaries_functor: Region \"" << *snit << "\" not found in regionation" << std::endl;
            ss << "Available regions: ";

            typedef typename viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
            typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

            RegionRangeType regions(mesh);
            for (RegionRangeIterator sit = regions.begin(); sit != regions.end(); ++sit)
            {
              if (sit != regions.begin())
                ss << ", ";
              ss << "\"" << (*sit).name() << "\"";
            }

            throw create_sizing_function_exception(ss.str());
          }
        }

        for (ConstElementIterator fit = elements.begin(); fit != elements.end(); ++fit)
        {
          bool is_on_all_boundaries = true;
          for (std::vector<std::string>::const_iterator sit = seg_it; sit != region_names.end(); ++sit)
          {
            if (!is_boundary(mesh.get_region(*sit), *fit))
            {
              is_on_all_boundaries = false;
              break;
            }
          }

          if (is_on_all_boundaries)
            boundary_elements->push_back( *fit );
        }

        if (boundary_elements->empty())
        {
          std::stringstream ss;

          ss << "distance_to_region_boundaries_functor: no elements found which are boundary elements of the regions: ";

          for (std::vector<std::string>::const_iterator snit = region_names.begin(); snit != region_names.end(); ++snit)
          {
            if (snit != region_names.begin())
              ss << ", ";
            ss << "\"" << *snit << "\"";
          }

          throw create_sizing_function_exception(ss.str());
        }
      }

      typedef typename viennagrid::result_of::point<MeshType>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef NumericType result_type;

      typedef typename viennagrid::result_of::const_element<MeshType>::type ConstElementType;
      typedef std::vector<ConstElementType> BoundaryElementContainer;

      NumericType operator()( PointType const & pt ) const
      {
        NumericType min_distance = -1;

        for (typename BoundaryElementContainer::const_iterator beit = boundary_elements->begin();
                                                               beit != boundary_elements->end();
                                                             ++beit)
        {
          NumericType current_distance = viennagrid::distance(pt, *beit);

          if (min_distance < 0)
            min_distance = current_distance;
          else if (current_distance < min_distance)
            min_distance = current_distance;
        }

        return min_distance;
      }


      MeshType mesh;
      std::shared_ptr<BoundaryElementContainer> boundary_elements;
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

    struct local_feature_size_2d_functor
    {
      typedef viennagrid::mesh_t MeshType;

      local_feature_size_2d_functor( MeshType const & mesh_ ) : mesh(mesh_) {}

      typedef typename viennagrid::result_of::point<MeshType>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef NumericType result_type;

      NumericType operator()( PointType const & pt ) const
      {
        typedef typename viennagrid::result_of::const_element_range<MeshType>::type ConstVertexRangeType;
        typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

        typedef typename viennagrid::result_of::const_element_range<MeshType>::type ConstLineRangeType;
        typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineRangeIterator;

        ConstVertexRangeType vertices( mesh, 0 );
        ConstLineRangeType lines( mesh, 1 );

        NumericType lfs = -1.0;

        for (ConstLineRangeIterator lit0 = lines.begin(); lit0 != lines.end(); ++lit0)
        {
          if (!viennagrid::is_any_boundary(mesh, *lit0))
            continue;

          NumericType distance_to_lit0 = viennagrid::distance(*lit0, pt);

          ConstLineRangeIterator lit1 = lit0; ++lit1;
          for (; lit1 != lines.end(); ++lit1)
          {
            if (!viennagrid::is_any_boundary(mesh, *lit1))
              continue;

            if (viennagrid::vertices(*lit0)[0] == viennagrid::vertices(*lit1)[0] ||
                viennagrid::vertices(*lit0)[0] == viennagrid::vertices(*lit1)[1] ||
                viennagrid::vertices(*lit0)[1] == viennagrid::vertices(*lit1)[0] ||
                viennagrid::vertices(*lit0)[1] == viennagrid::vertices(*lit1)[1])
            {
              continue;
            }

            NumericType distance_to_lit1 = viennagrid::distance(*lit1, pt);

            NumericType max_distance = std::max(distance_to_lit0, distance_to_lit1);

            if (lfs < 0 || max_distance < lfs)
              lfs = max_distance;
          }

        }

        return lfs;
      }

      MeshType mesh;
    };





    struct is_in_regions_functor
    {
      typedef viennagrid::mesh_t MeshType;
      typedef typename viennagrid::result_of::region<MeshType>::type RegionType;

      typedef typename viennagrid::result_of::point<MeshType>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

      typedef NumericType result_type;


      is_in_regions_functor(MeshType mesh_,
                            std::vector<std::string> const & region_names_,
                            SizingFunctionType const & function_) :
                              mesh(mesh_), region_names(region_names_), function(function_)
      {
        for (std::vector<std::string>::const_iterator snit = region_names.begin(); snit != region_names.end(); ++snit)
        {
          if ( !mesh.region_exists( *snit ) )
          {
            std::stringstream ss;

            ss << "distance_to_region_boundaries_functor: Region \"" << *snit << "\" not found in mesh" << std::endl;
            ss << "Available regions: ";

            typedef typename viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
            typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionIterator;

            RegionRangeType regions(mesh);
            for (RegionIterator sit = regions.begin(); sit != regions.end(); ++sit)
            {
              if (sit != regions.begin())
                ss << ", ";
              ss << "\"" << (*sit).name() << "\"";
            }

            throw create_sizing_function_exception(ss.str());
          }
        }
      }

      NumericType operator()( PointType const & pt ) const
      {
        typedef typename viennagrid::result_of::const_cell_range<RegionType>::type ConstCellRangeType;
        typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

        for (unsigned int i = 0; i < region_names.size(); ++i)
        {
          ConstCellRangeType cells( mesh.get_region(region_names[i]) );
          for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
          {
            if ( viennagrid::is_inside( *cit, pt ) )
              return function(pt);
          }
        }

        return -1;
      }

      MeshType mesh;
      std::vector<std::string> region_names;

      SizingFunctionType function;
    };










//     template<typename MeshT, typename RegionationT>
//     struct is_near_double_interface_functor : public base_sizing_function< typename viennagrid::result_of::point<MeshT>::type >
//     {
//       typedef typename viennagrid::result_of::region_handle<RegionationT>::type RegionType;
//
//       typedef typename viennagrid::result_of::point<MeshT>::type PointType;
//       typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
//       typedef typename viennamesh::result_of::sizing_function<PointType>::type SizingFunctionType;
//
//       typedef base_sizing_function<MeshT, RegionationT> BaseSizingFunctionType;
//       typedef shared_ptr<BaseSizingFunctionType> BaseSizingFunctionHandleType;
//
//       is_near_double_interface_functor( BaseSizingFunctionHandleType const & function_, RegionType const & region0_, RegionType const & region1_, NumericType max_distance_between_interfaces_ ) : region0(&region0_), region1(&region1_), function(function_), max_distance_between_interfaces(max_distance_between_interfaces_) {}
//
//       typedef NumericType result_type;
//
//       NumericType get( PointType const & pt ) const
//       {
//         NumericType distance_region0 = viennagrid::boundary_distance( *region0, pt );
//         NumericType distance_region1 = viennagrid::boundary_distance( *region1, pt );
//
//         if ( distance_region0 + distance_region1 < max_distance_between_interfaces )
//           return function->get(pt);
//
//         return -1;
//       }
//
//       RegionType const * region0;
//       RegionType const * region1;
//       BaseSizingFunctionHandleType function;
//       NumericType max_distance_between_interfaces;
//     };




    template<typename PointT>
    struct min_functor
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
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

    template<typename MeshT>
    SizingFunctionType from_xml(pugi::xml_node const & node,
                                MeshT const & mesh,
                                std::string const & base_path = "")
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;

      std::string name = node.name();

      if (name == "constant")
      {
        if ( !node.child_value("value") )
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"value\" missing" );

        double value = lexical_cast<double>(node.child_value("value"));
        return std::bind( constant_functor<PointType>(value), _1 );
      }
      else if (name == "min")
      {
        std::vector<SizingFunctionType> functions;
        for (pugi::xml_node source = node.child("source"); source; source = source.next_sibling("source"))
          functions.push_back( from_xml<MeshT>(source.first_child(), mesh, base_path) );

        if (functions.empty())
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": no sources specified" );

        return std::bind( min_functor<PointType>(functions), _1 );
      }
      else if (name == "max")
      {
        std::vector<SizingFunctionType> functions;
        for (pugi::xml_node source = node.child("source"); source; source = source.next_sibling("source"))
          functions.push_back( from_xml<MeshT>(source.first_child(), mesh, base_path) );

        if (functions.empty())
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": no sources specified" );

        return std::bind( max_functor<PointType>(functions), _1 );
      }
      else if (name == "interpolate")
      {
        pugi::xml_attribute transform_type_node = node.attribute("transform_type");
        std::string transform_type = transform_type_node.as_string();

        if ( !node.child_value("source") )
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"source\" missing" );

        SizingFunctionType source = from_xml<MeshT>(node.child("source").first_child(), mesh, base_path);

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

          return std::bind(linear_interpolate_functor<PointType>(source, lower, upper, lower_to, upper_to ), _1);
        }

        return SizingFunctionType();
      }
      else if (name == "distance_to_region_boundaries")
      {
        std::vector<std::string> region_names;
        for (pugi::xml_node region = node.child("region"); region; region = region.next_sibling("region"))
          region_names.push_back( region.text().as_string() );

        if (region_names.empty())
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": no region names specified" );

        std::string element_type = node.child_value("element_type");
        if (element_type == "line")
          return std::bind( distance_to_region_boundaries_functor(mesh, region_names, 1), _1 );
        else if (element_type == "facet")
          return std::bind( distance_to_region_boundaries_functor(mesh, region_names, viennagrid::facet_dimension(mesh)), _1 );
        else
          throw create_sizing_function_exception( "distance_to_region_boundaries: Element type \"" + element_type + "\" not supported" );
      }
      else if (name == "distance_to_interface")
      {
        std::vector<std::string> region_names;
        for (pugi::xml_node region = node.child("region"); region; region = region.next_sibling("region"))
          region_names.push_back( region.text().as_string() );

        if (region_names.empty())
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": no region names specified" );

        return std::bind( distance_to_interface_functor(mesh, region_names[0], region_names[1]), _1 );
      }
      else if (name == "local_feature_size_2d")
      {
        return std::bind( local_feature_size_2d_functor(mesh), _1 );
      }
      else if (name == "is_in_regions")
      {
        std::vector<std::string> region_names;
        for (pugi::xml_node region = node.child("region"); region; region = region.next_sibling("region"))
          region_names.push_back( region.text().as_string() );

        SizingFunctionType source = from_xml<MeshT>(node.child("source").first_child(), mesh, base_path);

        return std::bind( is_in_regions_functor(mesh, region_names, source), _1 );
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

        return std::bind( mesh_gradient_functor(mesh_file, quantity_name), _1 );
      }

      throw create_sizing_function_exception( "Sizing function functor \"" + name + "\" not supported" );

      return SizingFunctionType();
    }

    template<typename MeshT>
    SizingFunctionType from_xml(std::string const & xml_string,
                                MeshT const & mesh,
                                std::string const & base_path = "")
    {
      pugi::xml_document sf_xml;
      sf_xml.load( xml_string.c_str() );
      return from_xml<MeshT>( sf_xml.first_child(), mesh, base_path );
    }

    template<typename MeshT>
    SizingFunctionType from_xmlfile(std::string const & xml_filename,
                                    MeshT const & mesh,
                                    std::string const & base_path = "")
    {
      pugi::xml_document sf_xml;
      sf_xml.load_file( xml_filename.c_str() );
      return from_xml<MeshT>( sf_xml.first_child(), mesh, base_path );
    }

  }
}

#endif
