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

#include "line_coarsening.hpp"

#include "viennagrid/algorithm/angle.hpp"


namespace viennamesh
{

  template<typename ElementRangeT, typename AccessorT, typename T>
  void merge_lines(ElementRangeT const & elements,
                   AccessorT accessor,
                   T from, T to)
  {
    typedef typename viennagrid::result_of::const_iterator<ElementRangeT>::type ElementRangeIterator;
    for (ElementRangeIterator eit = elements.begin(); eit != elements.end(); ++eit)
    {
      if ( accessor.get(*eit) == from )
        accessor.set(*eit, to);
    }
  }



  template<typename MeshT>
  void coarsen(MeshT const & mesh,
               viennagrid::mesh const & output_mesh,
               double angle)
  {
    typedef typename viennagrid::result_of::element<MeshT>::type                  ElementType;

    typedef typename viennagrid::result_of::element_range<MeshT>::type            ElementRangeType;
    typedef typename viennagrid::result_of::iterator<ElementRangeType>::type      ElementIteratorType;



    ElementRangeType lines(mesh, 1);



    typedef std::vector<viennagrid::element_id> NewLineIDContainerType;
    typedef typename viennagrid::result_of::accessor<NewLineIDContainerType, ElementType>::type NewLineIDAcessorType;

    NewLineIDContainerType line_id_container( lines.size() );
    NewLineIDAcessorType line_ids(line_id_container);

    for (ElementIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
      line_ids.set(*lit, (*lit).id());



    ElementRangeType vertices(mesh, 0);
    for (ElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      typedef typename viennagrid::result_of::coboundary_range<MeshT>::type CoboundaryVertexRangeType;

      CoboundaryVertexRangeType coboundary_lines(mesh, *vit, 1);
      if (coboundary_lines.size() != 2)
        continue;

      if (!viennagrid::equal_regions(viennagrid::regions(coboundary_lines[0]),
                                     viennagrid::regions(coboundary_lines[1])))
        continue;

      ElementType middle = (*vit);
      ElementType first = viennagrid::vertices(coboundary_lines[0])[0] == middle ?
                                viennagrid::vertices(coboundary_lines[0])[1] :
                                viennagrid::vertices(coboundary_lines[0])[0];
      ElementType last = viennagrid::vertices(coboundary_lines[1])[0] == middle ?
                                viennagrid::vertices(coboundary_lines[1])[1] :
                                viennagrid::vertices(coboundary_lines[1])[0];


      double current_angle = viennagrid::angle( viennagrid::get_point(mesh, first),
                                                viennagrid::get_point(mesh, last),
                                                viennagrid::get_point(mesh, middle) );
      if (current_angle > angle)
      {
        merge_lines( lines,
                     line_ids,
                     line_ids.get(coboundary_lines[0]),
                     line_ids.get(coboundary_lines[1]) );
      }

    }


    std::map<viennagrid::element_id, std::vector<ElementType> > new_lines;

    for (ElementIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
      new_lines[line_ids.get(*lit)].push_back( *lit );


    typedef typename viennagrid::result_of::element_copy_map<>::type CopyMapType;
    CopyMapType copy_map( output_mesh, false );

    for (typename std::map<viennagrid::element_id, std::vector<ElementType> >::iterator nlit = new_lines.begin(); nlit != new_lines.end(); ++nlit)
    {
      std::map<ElementType, int> points;

      for (std::size_t i = 0; i != (*nlit).second.size(); ++i)
      {
        points[ viennagrid::vertices((*nlit).second[i])[0] ]++;
        points[ viennagrid::vertices((*nlit).second[i])[1] ]++;
      }

      std::vector<ElementType> line_to_create;

      for (typename std::map<ElementType, int>::iterator pit = points.begin(); pit != points.end(); ++pit)
      {
        if ((*pit).second == 1)
          line_to_create.push_back( (*pit).first );
      }

      assert( line_to_create.size() == 2 );

      ElementType nv0 = copy_map( line_to_create[0] );
      ElementType nv1 = copy_map( line_to_create[1] );

      ElementType nl = viennagrid::make_line(output_mesh, nv0, nv1);

      viennagrid::copy_region_information((*nlit).second[0], nl);
    }
  }




  line_coarsening::line_coarsening() {}
  std::string line_coarsening::name() { return "line_coarsening"; }

  bool line_coarsening::run(viennamesh::algorithm_handle &)
  {
    data_handle<double> angle = get_required_input<double>("angle");
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    mesh_handle output_mesh = make_data<mesh_handle>();

    info(1) << "Input line count: " << viennagrid::elements(input_mesh(), 1).size() << std::endl;

    coarsen( input_mesh(), output_mesh(), angle() );

    info(1) << "Output line count: " << viennagrid::elements(output_mesh(), 1).size() << std::endl;

    set_output("mesh", output_mesh);

    return true;
  }
}
