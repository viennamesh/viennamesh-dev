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


#include <boost/optional.hpp>


namespace viennamesh
{
  using std::placeholders::_1;
  using std::placeholders::_2;


  typedef boost::optional<viennagrid_numeric> SizingFunctionReturnType;
  typedef std::function< SizingFunctionReturnType(viennagrid::point_t const &) > SizingFunctionType;



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
  SizingFunctionReturnType distance_to_interface(PointT const & point,
                                                 RegionT const & reg0,
                                                 RegionT const & reg1,
                                                 viennagrid_dimension topologic_dimension)
  {
    typedef typename viennagrid::result_of::const_element_range<RegionT>::type ConstElementRangeType;
    typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

    typedef typename viennagrid::result_of::coord<RegionT>::type CoordType;

    ConstElementRangeType elements(reg0, topologic_dimension);
    if (elements.empty())
      return SizingFunctionReturnType();

    SizingFunctionReturnType min_distance;

    for (ConstElementIteratorType eit = elements.begin(); eit != elements.end(); ++eit)
    {
      if (is_boundary(reg1, *eit))
      {
        CoordType current_distance = viennagrid::distance(point, *eit);

        if (!min_distance)
          min_distance = current_distance;
        else if (current_distance < min_distance.get())
          min_distance = current_distance;
      }
    }

    return min_distance;
  }



  namespace sizing_function
  {
    class fast_is_inside
    {
    public:

      typedef viennagrid::mesh_t MeshType;
      typedef typename viennagrid::result_of::point<MeshType>::type PointType;
      typedef typename viennagrid::result_of::element<MeshType>::type ElementType;

      typedef std::vector<ElementType> ElementContainerType;

      fast_is_inside(MeshType const & mesh_,
                     int count_x_, int count_y_,
                     double mesh_bounding_box_scale, double cell_scale) :
          mesh(mesh_), count_x(count_x_), count_y(count_y_)
      {
        // ensure that bounding box is large enough
        if (mesh_bounding_box_scale <= 1.0)
          mesh_bounding_box_scale = 1.01;
        mesh_bounding_box_scale *= cell_scale;

        grid_elements.resize(count_x*count_y);

        typedef typename viennagrid::result_of::cell_range<MeshType>::type CellRangeType;
        typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellRangeIterator;

        std::pair<PointType, PointType> bb = viennagrid::bounding_box(mesh);
        min = bb.first;
        max = bb.second;

        min = (min+max)/2.0 + (min-max)/2.0 * mesh_bounding_box_scale;
        max = (min+max)/2.0 + (max-min)/2.0 * mesh_bounding_box_scale;

        CellRangeType cells(mesh);
        for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
        {
          std::pair<PointType, PointType> bb = viennagrid::bounding_box(*cit);

          bb.first = (bb.first+bb.second)/2.0 + (bb.first-bb.second)/2.0 * cell_scale;
          bb.second = (bb.first+bb.second)/2.0 + (bb.second-bb.first)/2.0 * cell_scale;

          int min_index_x = index_x(bb.first);
          int max_index_x = index_x(bb.second)+1;

          int min_index_y = index_y(bb.first);
          int max_index_y = index_y(bb.second)+1;

          assert(min_index_x >= 0 && min_index_x <= count_x);
          assert(max_index_x >= 0 && max_index_x <= count_x);

          assert(min_index_y >= 0 && min_index_y <= count_y);
          assert(max_index_y >= 0 && max_index_y <= count_y);

          for (int y = min_index_y; y != max_index_y; ++y)
          {
            for (int x = min_index_x; x != max_index_x; ++x)
            {
              grid_elements[y*count_x+x].push_back( *cit );
            }
          }
        }
      }

      ElementContainerType operator()(PointType const & p) const
      {
//         ElementContainerType slow_result;
//
//         typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
//         typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;
//
//         ConstCellRangeType cells(mesh);
//         for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
//         {
//           if ( viennagrid::is_inside(*cit, p) )
//           {
//             slow_result.push_back(*cit);
//           }
//         }
//
//         return slow_result;


        ElementContainerType fast_result;
        {
          int i = index(p);

          if (i >= 0 && i < static_cast<int>(grid_elements.size()))
          {
            ElementContainerType const & tmp = grid_elements[i];
            for (ElementContainerType::const_iterator cit = tmp.begin(); cit != tmp.end(); ++cit)
            {
              if ( viennagrid::is_inside(*cit, p) )
              {
                fast_result.push_back(*cit);
              }
            }
          }
        }

        return fast_result;
      }

    private:

      int index_x(PointType const & p) const
      {
        return (p[0]-min[0]) * (static_cast<double>(count_x)/(max[0]-min[0]));
      }

      int index_y(PointType const & p) const
      {
        return (p[1]-min[1]) * (static_cast<double>(count_y)/(max[1]-min[1]));
      }

      int index(PointType const & p) const
      {
        return index_y(p)*count_x+index_x(p);
      }

      MeshType mesh;
      std::vector<ElementContainerType> grid_elements;

      PointType min;
      PointType max;

      int count_x;
      int count_y;
    };






    struct mesh_quantity_functor
    {
      typedef viennagrid::mesh_t MeshType;
      typedef viennagrid::result_of::element<MeshType>::type ElementType;

      typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      typedef viennagrid::quantity_field QuantityFieldType;



      mesh_quantity_functor( std::string const & filename,
                             std::string const & quantity_name,
                             int resolution_x, int resolution_y,
                             double mesh_bounding_box_scale, double cell_scale)
      {
        viennagrid::io::vtk_reader<MeshType> reader;
        viennagrid::io::add_scalar_data_on_vertices( reader, quantities, quantity_name );
        reader( mesh, filename );

        ii = std::make_shared<fast_is_inside>( mesh, resolution_x, resolution_y, mesh_bounding_box_scale, cell_scale );
      }

      typedef typename viennagrid::result_of::element<MeshType>::type VertexType;
      typedef typename viennagrid::result_of::element<MeshType>::type CellType;

      typedef typename viennagrid::result_of::point<MeshType>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef SizingFunctionReturnType result_type;

      result_type operator()( PointType const & pt ) const
      {
        fast_is_inside::ElementContainerType cells = (*ii)(pt);
        if (cells.empty())
          return result_type();

        CellType cell = cells[0];
        PointType p0 = viennagrid::get_point( viennagrid::vertices(cell)[0] );
        PointType p1 = viennagrid::get_point( viennagrid::vertices(cell)[1] );
        PointType p2 = viennagrid::get_point( viennagrid::vertices(cell)[2] );

        NumericType f0 = viennagrid::spanned_volume( pt, p1, p2 );
        NumericType f1 = viennagrid::spanned_volume( p0, pt, p2 );
        NumericType f2 = viennagrid::spanned_volume( p0, p1, pt );

        NumericType s0 = quantities.get(viennagrid::vertices(cell)[0]);
        NumericType s1 = quantities.get(viennagrid::vertices(cell)[1]);
        NumericType s2 = quantities.get(viennagrid::vertices(cell)[2]);

        NumericType val = (s0*f0 + s1*f1 + s2*f2) / (f0 + f1 + f2);

        return val;
      }

      std::shared_ptr<fast_is_inside> ii;

      MeshType mesh;
      QuantityFieldType quantities;
    };


    struct mesh_gradient_functor
    {
      typedef viennagrid::mesh_t MeshType;
      typedef viennagrid::result_of::element<MeshType>::type ElementType;

      typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      typedef viennagrid::quantity_field QuantityFieldType;



      mesh_gradient_functor( std::string const & filename, std::string const & quantity_name,
                             int resolution_x, int resolution_y,
                             double mesh_bounding_box_scale, double cell_scale )
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

        ii = std::make_shared<fast_is_inside>( mesh, resolution_x, resolution_y, mesh_bounding_box_scale, cell_scale );
      }

      typedef typename viennagrid::result_of::element<MeshType>::type VertexType;

      typedef typename viennagrid::result_of::point<MeshType>::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;
      typedef SizingFunctionReturnType result_type;

      result_type operator()( PointType const & pt ) const
      {
        fast_is_inside::ElementContainerType cells = (*ii)(pt);
        if (cells.empty())
          return result_type();

        NumericType result = gradient_accessor.get(cells[0]);
        return result;
      }

      std::shared_ptr<fast_is_inside> ii;

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

      typedef SizingFunctionReturnType result_type;

      result_type operator()( PointType const & pt ) const
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

      typedef SizingFunctionReturnType result_type;

      typedef typename viennagrid::result_of::const_element<MeshType>::type ConstElementType;
      typedef std::vector<ConstElementType> BoundaryElementContainer;

      result_type operator()( PointType const & pt ) const
      {
        result_type min_distance;

        for (typename BoundaryElementContainer::const_iterator beit = boundary_elements->begin();
                                                               beit != boundary_elements->end();
                                                             ++beit)
        {
          NumericType current_distance = viennagrid::distance(pt, *beit);

          if (!min_distance || current_distance < min_distance.get())
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

      typedef SizingFunctionReturnType result_type;

      result_type operator()( PointType const & pt ) const
      {
        typedef typename viennagrid::result_of::const_element_range<MeshType>::type ConstVertexRangeType;
        typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

        typedef typename viennagrid::result_of::const_element_range<MeshType>::type ConstLineRangeType;
        typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineRangeIterator;

        ConstVertexRangeType vertices( mesh, 0 );
        ConstLineRangeType lines( mesh, 1 );

        result_type lfs;

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

            if (!lfs || max_distance < lfs.get())
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

      typedef SizingFunctionReturnType result_type;


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

      result_type operator()( PointType const & pt ) const
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

        return result_type();
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
      typedef SizingFunctionReturnType result_type;

      typedef std::vector<SizingFunctionType> SizingFunctionContainerType;

      min_functor(SizingFunctionContainerType const & functions_) : functions(functions_) {}

      result_type operator()( PointT const & pt ) const
      {
        result_type val;

        for (typename SizingFunctionContainerType::const_iterator fit = functions.begin(); fit != functions.end(); ++fit)
        {
          result_type current = (*fit)(pt);
          if (!current)
            continue;

          if (!val)
            val = current;
          else if (current.get() < val.get())
            val = current;
        }

        return val;
      }

      SizingFunctionContainerType functions;
    };



    template<typename PointT>
    struct max_functor
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
      typedef SizingFunctionReturnType result_type;

      typedef std::vector<SizingFunctionType> SizingFunctionContainerType;

      max_functor(SizingFunctionContainerType const & functions_) : functions(functions_) {}

      result_type operator()( PointT const & pt ) const
      {
        result_type val;

        for (typename SizingFunctionContainerType::const_iterator fit = functions.begin(); fit != functions.end(); ++fit)
        {
          result_type current = (*fit)(pt);
          if (!current)
            continue;

          if (val)
            val = current;
          else if (current.get() > val.get())
            val = current;
        }

        return val;
      }

      SizingFunctionContainerType functions;
    };




    template<typename PointT>
    struct constant_functor
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
      typedef SizingFunctionReturnType result_type;

      constant_functor(NumericType value_) : value(value_) {}

      result_type operator()( PointT const & ) const
      { return value; }

      NumericType value;
    };


    template<typename PointT>
    struct abs_functor
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
      typedef SizingFunctionReturnType result_type;

      abs_functor(SizingFunctionType const & function_) : function(function_) {}

      result_type operator()( PointT const & pt ) const
      {
        result_type tmp = function(pt);
        if (!tmp)
          return tmp;

        return std::abs( tmp.get() );
      }

      SizingFunctionType function;
    };


    template<typename PointT>
    struct linear_interpolate_functor
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
      typedef SizingFunctionReturnType result_type;

      linear_interpolate_functor(SizingFunctionType const & function_, NumericType lower_, NumericType upper_, NumericType lower_to_, NumericType upper_to_) : function(function_), lower(lower_), upper(upper_), lower_to(lower_to_), upper_to(upper_to_) {}

      result_type operator()( PointT const & pt ) const
      {
        result_type tmp = function(pt);
        if (!tmp)
          return tmp;

        if (tmp.get() < lower)
          return lower_to;
        if (tmp.get() >= upper)
          return upper_to;

        return lower_to + (tmp.get()-lower)/(upper-lower)*(upper_to-lower_to);
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
      else if (name == "abs")
      {
        if ( !node.child_value("source") )
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"source\" missing" );

        SizingFunctionType source = from_xml<MeshT>(node.child("source").first_child(), mesh, base_path);

        return std::bind(abs_functor<PointType>(source), _1);
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
      else if (name == "mesh_quantity")
      {
        if ( !node.child_value("mesh_file") )
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"mesh_file\" missing" );
        if ( !node.child_value("quantity_name") )
          throw create_sizing_function_exception( "Sizing function functor \"" + name + "\": required XML child element \"quantity_name\" missing" );

        std::string mesh_file = node.child_value("mesh_file");
        if (!base_path.empty())
          mesh_file = base_path + "/" + mesh_file;

        std::string quantity_name = node.child_value("quantity_name");

        int resolution_x = 100;
        if ( node.child("resolution_x") )
          resolution_x = lexical_cast<int>(node.child_value("resolution_x"));
        int resolution_y = 100;
        if ( node.child("resolution_x") )
          resolution_y = lexical_cast<int>(node.child_value("resolution_x"));

        double mesh_bounding_box_scale = 1.01;
        if ( node.child("mesh_bounding_box_scale") )
          mesh_bounding_box_scale = lexical_cast<double>(node.child_value("mesh_bounding_box_scale"));
        double cell_scale = 1.01;
        if ( node.child("cell_scale") )
          cell_scale = lexical_cast<double>(node.child_value("cell_scale"));

        return std::bind( mesh_quantity_functor(mesh_file, quantity_name, resolution_x, resolution_y, mesh_bounding_box_scale, cell_scale), _1 );
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

        int resolution_x = 100;
        if ( node.child("resolution_x") )
          resolution_x = lexical_cast<int>(node.child_value("resolution_x"));
        int resolution_y = 100;
        if ( node.child("resolution_x") )
          resolution_y = lexical_cast<int>(node.child_value("resolution_x"));

        double mesh_bounding_box_scale = 1.01;
        if ( node.child("mesh_bounding_box_scale") )
          mesh_bounding_box_scale = lexical_cast<double>(node.child_value("mesh_bounding_box_scale"));
        double cell_scale = 1.01;
        if ( node.child("cell_scale") )
          cell_scale = lexical_cast<double>(node.child_value("cell_scale"));

        return std::bind( mesh_gradient_functor(mesh_file, quantity_name, resolution_x, resolution_y, mesh_bounding_box_scale, cell_scale), _1 );
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
