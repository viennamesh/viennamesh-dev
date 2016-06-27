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

#include "viennameshpp/forwards.hpp"
#include "viennagrid/viennagrid.hpp"

#include "pugixml.hpp"

namespace viennamesh
{
  namespace sizing_function
  {
    class fast_is_inside
    {
    public:

      typedef viennagrid::mesh                                  MeshType;
      typedef viennagrid::result_of::point<MeshType>::type      PointType;
      typedef viennagrid::result_of::element<MeshType>::type    ElementType;

      typedef std::vector<ElementType> ElementContainerType;

      fast_is_inside(MeshType const & mesh_,
                     int count_x_, int count_y_,
                     double mesh_bounding_box_scale, double cell_scale);
      ElementContainerType operator()(PointType const & p) const;

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




    class base_functor
    {
    protected:

      typedef viennagrid::mesh                                              MeshType;
      typedef viennagrid::result_of::region<MeshType>::type                 RegionType;

      typedef viennagrid::result_of::point<MeshType>::type                  PointType;
      typedef viennagrid::result_of::coord<PointType>::type                 CoordType;

      typedef viennagrid::quantity_field                                    QuantityFieldType;

      typedef viennagrid::result_of::element<MeshType>::type                ElementType;

      typedef viennagrid::result_of::const_cell_range<MeshType>::type       ConstCellRangeType;
      typedef viennagrid::result_of::iterator<ConstCellRangeType>::type     ConstCellIteratorType;

    public:

      typedef optional<CoordType> result_type;
      typedef function< result_type(viennagrid::point const &) > function_type;
      typedef std::vector<function_type> SizingFunctionContainerType;

      virtual result_type operator()( PointType const & pt ) const = 0;

    private:
    };




    class mesh_quantity_functor : public base_functor
    {
    public:
      mesh_quantity_functor( std::string const & filename,
                             std::string const & quantity_name,
                             int resolution_x, int resolution_y,
                             double mesh_bounding_box_scale, double cell_scale);

      result_type operator()( PointType const & pt ) const;

    private:
      shared_ptr<fast_is_inside> ii;

      MeshType mesh;
      QuantityFieldType quantities;
    };


    class mesh_gradient_functor : public base_functor
    {
    public:
      mesh_gradient_functor( std::string const & filename, std::string const & quantity_name,
                             int resolution_x, int resolution_y,
                             double mesh_bounding_box_scale, double cell_scale );

      result_type operator()( PointType const & pt ) const;

    private:
      shared_ptr<fast_is_inside> ii;

      MeshType mesh;
      QuantityFieldType gradient_accessor;
    };





    class distance_to_interface_functor : public base_functor
    {
    public:
      distance_to_interface_functor( MeshType const & mesh_,
                                     std::string const & region0_name,
                                     std::string const & region1_name );

      result_type operator()( PointType const & pt ) const;

    private:
      MeshType mesh;
      RegionType region0;
      RegionType region1;

      viennagrid_dimension facet_dimension;
    };




    class distance_to_region_boundaries_functor : public base_functor
    {
    public:
      distance_to_region_boundaries_functor(MeshType const & mesh_,
                                            std::vector<std::string> const & region_names,
                                            viennagrid_dimension topologic_dimension);

      result_type operator()( PointType const & pt ) const;

    private:
      typedef std::vector<ElementType> BoundaryElementContainer;

      MeshType mesh;
      shared_ptr<BoundaryElementContainer> boundary_elements;
    };




    class local_feature_size_2d_functor : public base_functor
    {
    public:
      local_feature_size_2d_functor( MeshType const & mesh_ ) : mesh(mesh_) {}

      result_type operator()( PointType const & pt ) const;

    private:
      MeshType mesh;
    };





    class is_in_regions_functor : public base_functor
    {
    public:
      is_in_regions_functor(MeshType mesh_,
                            std::vector<std::string> const & region_names_,
                            function_type const & function_);

      result_type operator()( PointType const & pt ) const;

    private:
      MeshType mesh;
      std::vector<std::string> region_names;

      function_type function;
    };








    class min_functor : public base_functor
    {
    public:
      min_functor(SizingFunctionContainerType const & functions_) : functions(functions_) {}

      result_type operator()( PointType const & pt ) const;

    private:
      SizingFunctionContainerType functions;
    };



    class max_functor : public base_functor
    {
    public:
      max_functor(SizingFunctionContainerType const & functions_) : functions(functions_) {}

      result_type operator()( PointType const & pt ) const;

    private:
      SizingFunctionContainerType functions;
    };




    class constant_functor : public base_functor
    {
    public:
      constant_functor(result_type value_) : value(value_) {}

      result_type operator()( PointType const & ) const
      { return value; }

    private:
      result_type value;
    };


    class add_functor : public base_functor
    {
    public:
      add_functor(SizingFunctionContainerType const & functions_) : functions(functions_) {}

      result_type operator()( PointType const & pt ) const;

    private:
      SizingFunctionContainerType functions;
    };

    class mul_functor : public base_functor
    {
    public:
      mul_functor(SizingFunctionContainerType const & functions_) : functions(functions_) {}

      result_type operator()( PointType const & pt ) const;

    private:
      SizingFunctionContainerType functions;
    };


    class abs_functor : public base_functor
    {
    public:
      abs_functor(function_type const & function_) : function(function_) {}

      result_type operator()( PointType const & pt ) const
      {
        result_type tmp = function(pt);
        if (!tmp)
          return tmp;

        return std::abs( tmp.get() );
      }

    private:
      function_type function;
    };


    class less_functor : public base_functor
    {
    public:
      less_functor(function_type const & function_, CoordType threshold_) : function(function_), threshold(threshold_) {}

      result_type operator()( PointType const & pt ) const
      {
        result_type tmp = function(pt);
        if (!tmp)
          return tmp;

        return tmp.get() < threshold ? 1.0 : 0.0;
      }

    private:
      function_type function;
      CoordType threshold;
    };

    class greater_functor : public base_functor
    {
    public:
      greater_functor(function_type const & function_, CoordType threshold_) : function(function_), threshold(threshold_) {}

      result_type operator()( PointType const & pt ) const
      {
        result_type tmp = function(pt);
        if (!tmp)
          return tmp;

        return tmp.get() > threshold ? 1.0 : 0.0;
      }

    private:
      function_type function;
      CoordType threshold;
    };

    class in_interval_functor : public base_functor
    {
    public:
      in_interval_functor(function_type const & function_, CoordType lower_, CoordType upper_) : function(function_), lower(lower_), upper(upper_) {}

      result_type operator()( PointType const & pt ) const
      {
        result_type tmp = function(pt);
        if (!tmp)
          return tmp;

        return ((lower < tmp.get()) && (tmp.get() < upper)) ? 1.0 : 0.0;
      }

    private:
      function_type function;
      CoordType lower;
      CoordType upper;
    };

    class linear_interpolate_functor : public base_functor
    {
    public:
      linear_interpolate_functor(function_type const & function_,
                                 CoordType lower_, CoordType upper_, CoordType lower_to_, CoordType upper_to_) : function(function_), lower(lower_), upper(upper_), lower_to(lower_to_), upper_to(upper_to_) {}

      result_type operator()( PointType const & pt ) const
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

    private:
      function_type function;

      CoordType lower;
      CoordType upper;

      CoordType lower_to;
      CoordType upper_to;
    };






    base_functor::function_type from_xml(pugi::xml_node const & node,
                                         viennagrid::const_mesh const & mesh,
                                         std::string const & base_path = "");

    base_functor::function_type from_xml(std::string const & xml_string,
                                         viennagrid::const_mesh const & mesh,
                                         std::string const & base_path = "");

    base_functor::function_type from_xmlfile(std::string const & xml_filename,
                                             viennagrid::const_mesh const & mesh,
                                             std::string const & base_path = "");

  }
}

#endif
