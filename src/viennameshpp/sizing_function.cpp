#include "viennameshpp/sizing_function.hpp"

#include "viennamesh/cpp_error.hpp"

#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/inclusion.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/io/vtk_reader.hpp"


namespace viennamesh
{
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
  typename viennagrid::result_of::coord<PointT>::type distance_to_interface(PointT const & point,
                                                                            RegionT const & reg0,
                                                                            RegionT const & reg1,
                                                                            viennagrid_dimension topologic_dimension)
  {
    typedef typename viennagrid::result_of::const_element_range<RegionT>::type ConstElementRangeType;
    typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

    typedef typename viennagrid::result_of::coord<RegionT>::type CoordType;

    ConstElementRangeType elements(reg0, topologic_dimension);
    if (elements.empty())
      return CoordType();

    CoordType min_distance = -1;

    for (ConstElementIteratorType eit = elements.begin(); eit != elements.end(); ++eit)
    {
      if (is_boundary(reg1, *eit))
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

    fast_is_inside::fast_is_inside(MeshType const & mesh_,
                    int count_x_, int count_y_,
                    double mesh_bounding_box_scale, double cell_scale) :
        mesh(mesh_), count_x(count_x_), count_y(count_y_)
    {
      // ensure that bounding box is large enough
      if (mesh_bounding_box_scale <= 1.0)
        mesh_bounding_box_scale = 1.01;
      mesh_bounding_box_scale *= cell_scale;

      grid_elements.resize(count_x*count_y);

      typedef viennagrid::result_of::cell_range<MeshType>::type CellRangeType;
      typedef viennagrid::result_of::iterator<CellRangeType>::type CellRangeIterator;

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



    fast_is_inside::ElementContainerType fast_is_inside::operator()(PointType const & p) const
    {
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







    mesh_quantity_functor::mesh_quantity_functor( std::string const & filename,
                            std::string const & quantity_name,
                            int resolution_x, int resolution_y,
                            double mesh_bounding_box_scale, double cell_scale)
    {
      viennagrid::io::vtk_reader<MeshType> reader;
      viennagrid::io::add_scalar_data_on_vertices( reader, quantities, quantity_name );
      reader( mesh, filename );

      ii = make_shared<fast_is_inside>( mesh, resolution_x, resolution_y, mesh_bounding_box_scale, cell_scale );
    }


    mesh_quantity_functor::result_type mesh_quantity_functor::operator()( PointType const & pt ) const
    {
      fast_is_inside::ElementContainerType cells = (*ii)(pt);
      if (cells.empty())
        return result_type();

      ElementType cell = cells[0];
      PointType p0 = viennagrid::get_point( viennagrid::vertices(cell)[0] );
      PointType p1 = viennagrid::get_point( viennagrid::vertices(cell)[1] );
      PointType p2 = viennagrid::get_point( viennagrid::vertices(cell)[2] );

      CoordType f0 = viennagrid::spanned_volume( pt, p1, p2 );
      CoordType f1 = viennagrid::spanned_volume( p0, pt, p2 );
      CoordType f2 = viennagrid::spanned_volume( p0, p1, pt );

      CoordType s0 = quantities.get(viennagrid::vertices(cell)[0]);
      CoordType s1 = quantities.get(viennagrid::vertices(cell)[1]);
      CoordType s2 = quantities.get(viennagrid::vertices(cell)[2]);

      CoordType val = (s0*f0 + s1*f1 + s2*f2) / (f0 + f1 + f2);

      return val;
    }








    mesh_gradient_functor::mesh_gradient_functor( std::string const & filename, std::string const & quantity_name,
                            int resolution_x, int resolution_y,
                            double mesh_bounding_box_scale, double cell_scale )
    {
      QuantityFieldType quantities;
      viennagrid::io::vtk_reader<MeshType> reader;
      viennagrid::io::add_scalar_data_on_vertices( reader, quantities, quantity_name );
      reader( mesh, filename );

      ConstCellRangeType cells(mesh);

      gradient_accessor.init(viennagrid::cell_dimension(mesh), 1);
      gradient_accessor.resize( cells.size() );

      for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      {
        gradient_accessor.set(*cit, viennamesh::gradient(*cit, quantities));
      }

      ii = make_shared<fast_is_inside>( mesh, resolution_x, resolution_y, mesh_bounding_box_scale, cell_scale );
    }


    mesh_gradient_functor::result_type mesh_gradient_functor::operator()( PointType const & pt ) const
    {
      fast_is_inside::ElementContainerType cells = (*ii)(pt);
      if (cells.empty())
        return result_type();

      CoordType result = gradient_accessor.get(cells[0]);
      return result;
    }






    distance_to_interface_functor::distance_to_interface_functor( MeshType const & mesh_,
                                    std::string const & region0_name,
                                    std::string const & region1_name ) :
                                    mesh(mesh_),
                                    region0( mesh.get_region(region0_name) ), region1( mesh.get_region(region1_name) ),
                                    facet_dimension(viennagrid::facet_dimension(mesh_)) {}

    distance_to_interface_functor::result_type distance_to_interface_functor::operator()( PointType const & pt ) const
    {
      return distance_to_interface( pt, region0, region1, facet_dimension );
    }






    distance_to_region_boundaries_functor::distance_to_region_boundaries_functor(MeshType const & mesh_,
                                            std::vector<std::string> const & region_names,
                                            viennagrid_dimension topologic_dimension) :
                                            mesh(mesh_), boundary_elements(new BoundaryElementContainer)
    {
      typedef viennagrid::result_of::const_element_range<RegionType>::type ConstElementRangeType;
      typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIterator;

      if (region_names.empty())
        VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "distance_to_region_boundaries_functor: No region names specified" );

      std::vector<std::string>::const_iterator seg_it = region_names.begin();
      ConstElementRangeType elements( mesh.get_region(*seg_it++), topologic_dimension );

      if (elements.empty())
        VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "distance_to_region_boundaries_functor: No elements found in mesh" );

      for (std::vector<std::string>::const_iterator snit = region_names.begin(); snit != region_names.end(); ++snit)
      {
        if ( !mesh.region_exists( *snit ) )
        {
          std::stringstream ss;

          ss << "distance_to_region_boundaries_functor: Region \"" << *snit << "\" not found in regionation" << std::endl;
          ss << "Available regions: ";

          typedef viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
          typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

          RegionRangeType regions(mesh);
          for (RegionRangeIterator sit = regions.begin(); sit != regions.end(); ++sit)
          {
            if (sit != regions.begin())
              ss << ", ";
            ss << "\"" << (*sit).get_name() << "\"";
          }

          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION,ss.str());
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

        VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION,ss.str());
      }
    }


    distance_to_region_boundaries_functor::result_type distance_to_region_boundaries_functor::operator()( PointType const & pt ) const
    {
      result_type min_distance;

      for (BoundaryElementContainer::const_iterator beit = boundary_elements->begin();
                                                    beit != boundary_elements->end();
                                                    ++beit)
      {
        CoordType current_distance = viennagrid::distance(pt, *beit);

        if (!min_distance || current_distance < min_distance.get())
          min_distance = current_distance;
      }

      return min_distance;
    }





    local_feature_size_2d_functor::result_type local_feature_size_2d_functor::operator()( PointType const & pt ) const
    {
      typedef viennagrid::result_of::const_element_range<MeshType>::type ConstVertexRangeType;
//         typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

      typedef viennagrid::result_of::const_element_range<MeshType>::type ConstLineRangeType;
      typedef viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineRangeIterator;

      ConstVertexRangeType vertices( mesh, 0 );
      ConstLineRangeType lines( mesh, 1 );

      result_type lfs;

      for (ConstLineRangeIterator lit0 = lines.begin(); lit0 != lines.end(); ++lit0)
      {
        if (!viennagrid::is_any_boundary(*lit0))
          continue;

        CoordType distance_to_lit0 = viennagrid::distance(*lit0, pt);

        ConstLineRangeIterator lit1 = lit0; ++lit1;
        for (; lit1 != lines.end(); ++lit1)
        {
          if (!viennagrid::is_any_boundary(*lit1))
            continue;

          if (viennagrid::vertices(*lit0)[0] == viennagrid::vertices(*lit1)[0] ||
              viennagrid::vertices(*lit0)[0] == viennagrid::vertices(*lit1)[1] ||
              viennagrid::vertices(*lit0)[1] == viennagrid::vertices(*lit1)[0] ||
              viennagrid::vertices(*lit0)[1] == viennagrid::vertices(*lit1)[1])
          {
            continue;
          }

          CoordType distance_to_lit1 = viennagrid::distance(*lit1, pt);
          CoordType max_distance = std::max(distance_to_lit0, distance_to_lit1);

          if (!lfs || max_distance < lfs.get())
            lfs = max_distance;
        }

      }

      return lfs;
    }







    is_in_regions_functor::is_in_regions_functor(MeshType mesh_,
                                                 std::vector<std::string> const & region_names_,
                                                 function_type const & function_) :
                            mesh(mesh_), region_names(region_names_), function(function_)
    {
      for (std::vector<std::string>::const_iterator snit = region_names.begin(); snit != region_names.end(); ++snit)
      {
        if ( !mesh.region_exists( *snit ) )
        {
          std::stringstream ss;

          ss << "distance_to_region_boundaries_functor: Region \"" << *snit << "\" not found in mesh" << std::endl;
          ss << "Available regions: ";

          typedef viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
          typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionIterator;

          RegionRangeType regions(mesh);
          for (RegionIterator sit = regions.begin(); sit != regions.end(); ++sit)
          {
            if (sit != regions.begin())
              ss << ", ";
            ss << "\"" << (*sit).get_name() << "\"";
          }

          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION,ss.str());
        }
      }
    }


    is_in_regions_functor::result_type is_in_regions_functor::operator()( PointType const & pt ) const
    {
      typedef viennagrid::result_of::const_cell_range<RegionType>::type ConstCellRangeType;
      typedef viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

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




    add_functor::result_type add_functor::operator()( PointType const & pt ) const
    {
      result_type val;

      for (SizingFunctionContainerType::const_iterator fit = functions.begin(); fit != functions.end(); ++fit)
      {
        result_type current = (*fit)(pt);
        if (!current)
          continue;

        if (!val)
          val = current;
        else
          val = current.get() + val.get();
      }

      return val;
    }

    mul_functor::result_type mul_functor::operator()( PointType const & pt ) const
    {
      result_type val;

      for (SizingFunctionContainerType::const_iterator fit = functions.begin(); fit != functions.end(); ++fit)
      {
        result_type current = (*fit)(pt);
        if (!current)
          continue;

        if (!val)
          val = current;
        else
          val = current.get() * val.get();
      }

      return val;
    }


    min_functor::result_type min_functor::operator()( PointType const & pt ) const
    {
      result_type val;

      for (SizingFunctionContainerType::const_iterator fit = functions.begin(); fit != functions.end(); ++fit)
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




    max_functor::result_type max_functor::operator()( PointType const & pt ) const
    {
      result_type val;

      for (SizingFunctionContainerType::const_iterator fit = functions.begin(); fit != functions.end(); ++fit)
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










    base_functor::function_type from_xml(pugi::xml_node const & node,
                                         viennagrid::const_mesh const & mesh,
                                         std::string const & base_path)
    {
      std::string name = node.name();

      if (name == "constant")
      {
        if ( !node.child_value("value") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"value\" missing" );

        double value = lexical_cast<double>(node.child_value("value"));
        return bind( constant_functor(value), _1 );
      }
      else if (name == "abs")
      {
        if ( !node.child_value("source") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"source\" missing" );

        base_functor::function_type source = from_xml(node.child("source").first_child(), mesh, base_path);

        return bind(abs_functor(source), _1);
      }
      else if (name == "less")
      {
        if ( !node.child_value("source") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"source\" missing" );
        base_functor::function_type source = from_xml(node.child("source").first_child(), mesh, base_path);

        if ( !node.child_value("threshold") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"threshold\" missing" );
        double threshold = lexical_cast<double>(node.child_value("threshold"));

        return bind(less_functor(source, threshold), _1);
      }
      else if (name == "greater")
      {
        if ( !node.child_value("source") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"source\" missing" );
        base_functor::function_type source = from_xml(node.child("source").first_child(), mesh, base_path);

        if ( !node.child_value("threshold") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"threshold\" missing" );
        double threshold = lexical_cast<double>(node.child_value("threshold"));

        return bind(greater_functor(source, threshold), _1);
      }
      else if (name == "in_interval")
      {
        if ( !node.child_value("source") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"source\" missing" );
        base_functor::function_type source = from_xml(node.child("source").first_child(), mesh, base_path);

        if ( !node.child_value("lower") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"threshold\" missing" );
        double lower = lexical_cast<double>(node.child_value("lower"));
        if ( !node.child_value("upper") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"threshold\" missing" );
        double upper = lexical_cast<double>(node.child_value("upper"));

        return bind(in_interval_functor(source, lower, upper), _1);
      }
      else if (name == "add")
      {
        std::vector<base_functor::function_type> functions;
        for (pugi::xml_node source = node.child("source"); source; source = source.next_sibling("source"))
          functions.push_back( from_xml(source.first_child(), mesh, base_path) );

        if (functions.empty())
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": no sources specified" );

        return bind( add_functor(functions), _1 );
      }
      else if (name == "mul")
      {
        std::vector<base_functor::function_type> functions;
        for (pugi::xml_node source = node.child("source"); source; source = source.next_sibling("source"))
          functions.push_back( from_xml(source.first_child(), mesh, base_path) );

        if (functions.empty())
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": no sources specified" );

        return bind( mul_functor(functions), _1 );
      }
      else if (name == "min")
      {
        std::vector<base_functor::function_type> functions;
        for (pugi::xml_node source = node.child("source"); source; source = source.next_sibling("source"))
          functions.push_back( from_xml(source.first_child(), mesh, base_path) );

        if (functions.empty())
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": no sources specified" );

        return bind( min_functor(functions), _1 );
      }
      else if (name == "max")
      {
        std::vector<base_functor::function_type> functions;
        for (pugi::xml_node source = node.child("source"); source; source = source.next_sibling("source"))
          functions.push_back( from_xml(source.first_child(), mesh, base_path) );

        if (functions.empty())
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": no sources specified" );

        return bind( max_functor(functions), _1 );
      }
      else if (name == "interpolate")
      {
        pugi::xml_attribute transform_type_node = node.attribute("transform_type");
        std::string transform_type = transform_type_node.as_string();

        if ( !node.child_value("source") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"source\" missing" );

        base_functor::function_type source = from_xml(node.child("source").first_child(), mesh, base_path);

        if (transform_type == "linear")
        {
          if ( !node.child_value("lower") )
            VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"lower\" missing" );
          if ( !node.child_value("upper") )
            VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"upper\" missing" );
          if ( !node.child_value("lower_to") )
            VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"lower_to\" missing" );
          if ( !node.child_value("upper_to") )
            VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"upper_to\" missing" );

          double lower = lexical_cast<double>(node.child_value("lower"));
          double upper = lexical_cast<double>(node.child_value("upper"));
          double lower_to = lexical_cast<double>(node.child_value("lower_to"));
          double upper_to = lexical_cast<double>(node.child_value("upper_to"));

          return bind(linear_interpolate_functor(source, lower, upper, lower_to, upper_to ), _1);
        }

        return base_functor::function_type();
      }
      else if (name == "distance_to_region_boundaries")
      {
        std::vector<std::string> region_names;
        for (pugi::xml_node region = node.child("region"); region; region = region.next_sibling("region"))
          region_names.push_back( region.text().as_string() );

        if (region_names.empty())
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": no region names specified" );

        std::string element_type = node.child_value("element_type");
        if (element_type == "line")
          return bind( distance_to_region_boundaries_functor(mesh, region_names, 1), _1 );
        else if (element_type == "facet")
          return bind( distance_to_region_boundaries_functor(mesh, region_names, viennagrid::facet_dimension(mesh)), _1 );
        else
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "distance_to_region_boundaries: Element type \"" + element_type + "\" not supported" );
      }
      else if (name == "distance_to_interface")
      {
        std::vector<std::string> region_names;
        for (pugi::xml_node region = node.child("region"); region; region = region.next_sibling("region"))
          region_names.push_back( region.text().as_string() );

        if (region_names.empty())
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": no region names specified" );

        return bind( distance_to_interface_functor(mesh, region_names[0], region_names[1]), _1 );
      }
      else if (name == "local_feature_size_2d")
      {
        return bind( local_feature_size_2d_functor(mesh), _1 );
      }
      else if (name == "is_in_regions")
      {
        std::vector<std::string> region_names;
        for (pugi::xml_node region = node.child("region"); region; region = region.next_sibling("region"))
          region_names.push_back( region.text().as_string() );

        base_functor::function_type source = from_xml(node.child("source").first_child(), mesh, base_path);

        return bind( is_in_regions_functor(mesh, region_names, source), _1 );
      }
      else if (name == "mesh_quantity")
      {
        if ( !node.child_value("mesh_file") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"mesh_file\" missing" );
        if ( !node.child_value("quantity_name") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"quantity_name\" missing" );

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

        return bind( mesh_quantity_functor(mesh_file, quantity_name, resolution_x, resolution_y, mesh_bounding_box_scale, cell_scale), _1 );
      }
      else if (name == "mesh_gradient")
      {
        if ( !node.child_value("mesh_file") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"mesh_file\" missing" );
        if ( !node.child_value("quantity_name") )
          VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\": required XML child element \"quantity_name\" missing" );

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

        return bind( mesh_gradient_functor(mesh_file, quantity_name, resolution_x, resolution_y, mesh_bounding_box_scale, cell_scale), _1 );
      }

      VIENNAMESH_ERROR(VIENNAMESH_ERROR_SIZING_FUNCTION, "Sizing function functor \"" + name + "\" not supported" );
      return base_functor::function_type();
    }

    base_functor::function_type from_xml(std::string const & xml_string,
                                         viennagrid::const_mesh const & mesh,
                                         std::string const & base_path)
    {
      pugi::xml_document sf_xml;
      sf_xml.load( xml_string.c_str() );
      return from_xml( sf_xml.first_child(), mesh, base_path );
    }

    base_functor::function_type from_xmlfile(std::string const & xml_filename,
                                             viennagrid::const_mesh const & mesh,
                                             std::string const & base_path)
    {
      pugi::xml_document sf_xml;
      sf_xml.load_file( xml_filename.c_str() );
      return from_xml( sf_xml.first_child(), mesh, base_path );
    }

  }
}
