// #ifdef VIENNAMESH_WITH_TETGEN

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
#include "cgal_mesh.hpp"
#include "cgal_simplify_mesh.hpp"

#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_length_cost.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h>


#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>

#include "boost/any.hpp"
#include <CGAL/Surface_mesh_simplification/Detail/Common.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_profile.h>
namespace CGAL {

  namespace Surface_mesh_simplification
  {
    template<class ECM_>
    class My_stop_predicate
    {
      public:

      typedef ECM_ ECM ;

      typedef Edge_profile<ECM> Profile ;

      typedef typename boost::graph_traits<ECM>::edge_descriptor edge_descriptor ;
      typedef typename boost::graph_traits<ECM>::edges_size_type size_type ;
      typedef CGAL::Simple_cartesian<double> Kernel;
      typedef CGAL::Polyhedron_3<Kernel> mesh;
      typedef mesh::Facet_iterator Facet_iterator;
      My_stop_predicate( double min_area) : target_min_area(min_area) {}

      template <typename F>
      bool operator()( F const&       // aCurrentCost
                 , Profile const&  aEdgeProfile
                 , size_type      // aInitialCount
                 , size_type      // aCurrentCount
                 ) const
      {
        ECM & sm  = aEdgeProfile.surface_mesh();
        double min_area=-1.0;
        Facet_iterator begin = sm.facets_begin();

        for ( ; begin != sm.facets_end(); ++begin){
          double min=Kernel::Compute_area_3()(
              begin->facet_begin()->vertex()->point(),
              begin->facet_begin()->next()->vertex()->point(),
              begin->facet_begin()->opposite()->vertex()->point() );
          if(min<min_area || min_area==-1.0)
            min_area=min;
        }
        return target_min_area<=min_area;
      }

      private:
        double target_min_area;
    };
  }
}


namespace viennamesh
{
  namespace cgal
  {
    struct cgal_simplify_options{
      int stop;
      double stop_val;
      int cost;
      int placement;
    };

    void out(cgal::mesh input){
      int index=0;
      typedef mesh::Vertex_iterator Vertex_iterator;
      typedef mesh::Facet_iterator Facet_iterator;
      {
        std::cout << "Vertices: \n";
        Vertex_iterator begin = input.vertices_begin();
        for ( ; begin != input.vertices_end(); ++begin,++index)
          std::cout <<begin->point()<<"\n";
        std::cout<<index<<" vertices\n";
      }
      {
        std::cout << "Cells:\n";
        Facet_iterator begin = input.facets_begin();
        for ( ; begin != input.facets_end(); ++begin)
        {
          std::cout << "(" << begin->facet_begin()->vertex()->point() << ") ";
          std::cout << "(" << begin->facet_begin()->next()->vertex()->point() << ") ";
          std::cout << "(" << begin->facet_begin()->opposite()->vertex()->point() << ")\t";
          std::cout << "\n";
        }
      }
    }

    void simplify_mesh_impl(cgal::mesh input,
                        cgal::mesh & output,
                        cgal_simplify_options options)
    {
      namespace SMS = CGAL::Surface_mesh_simplification;

      if(options.stop==0)
      {
        SMS::Count_ratio_stop_predicate<cgal::mesh> stop(options.stop_val);

        if(options.cost==0 && options.placement==0)
          SMS::edge_collapse
            (input
            ,stop
            ,CGAL::vertex_index_map(get(CGAL::vertex_external_index,input))
              .halfedge_index_map  (get(CGAL::halfedge_external_index  ,input))
          );
        else if(options.cost==1 && options.placement == 0)
          SMS::edge_collapse
            (input
            ,stop
            ,CGAL::vertex_index_map(get(CGAL::vertex_external_index,input))
              .halfedge_index_map  (get(CGAL::halfedge_external_index  ,input))
              .get_cost (SMS::Edge_length_cost <cgal::mesh>())
          );
        else if(options.cost==0 && options.placement == 1)
          SMS::edge_collapse
            (input
            ,stop
            ,CGAL::vertex_index_map(get(CGAL::vertex_external_index,input))
              .halfedge_index_map  (get(CGAL::halfedge_external_index  ,input))
              .get_placement(SMS::Midpoint_placement<cgal::mesh>())
          );
        else
          SMS::edge_collapse
            (input
            ,stop
            ,CGAL::vertex_index_map(get(CGAL::vertex_external_index,input))
              .halfedge_index_map  (get(CGAL::halfedge_external_index  ,input))
              .get_cost (SMS::Edge_length_cost <cgal::mesh>())
              .get_placement(SMS::Midpoint_placement<cgal::mesh>())
          );
      }
      else if(options.stop==1)
      {
        SMS::Count_stop_predicate<cgal::mesh> stop(options.stop_val);

        if(options.cost==0 && options.placement==0)
          SMS::edge_collapse
            (input
            ,stop
            ,CGAL::vertex_index_map(get(CGAL::vertex_external_index,input))
              .halfedge_index_map  (get(CGAL::halfedge_external_index  ,input))
          );
        else if(options.cost==1 && options.placement == 0)
          SMS::edge_collapse
            (input
            ,stop
            ,CGAL::vertex_index_map(get(CGAL::vertex_external_index,input))
              .halfedge_index_map  (get(CGAL::halfedge_external_index  ,input))
              .get_cost (SMS::Edge_length_cost <cgal::mesh>())
          );
        else if(options.cost==0 && options.placement == 1)
          SMS::edge_collapse
            (input
            ,stop
            ,CGAL::vertex_index_map(get(CGAL::vertex_external_index,input))
              .halfedge_index_map  (get(CGAL::halfedge_external_index  ,input))
              .get_placement(SMS::Midpoint_placement<cgal::mesh>())
          );
        else
          SMS::edge_collapse
            (input
            ,stop
            ,CGAL::vertex_index_map(get(CGAL::vertex_external_index,input))
              .halfedge_index_map  (get(CGAL::halfedge_external_index  ,input))
              .get_cost (SMS::Edge_length_cost <cgal::mesh>())
              .get_placement(SMS::Midpoint_placement<cgal::mesh>())
          );
      }
      else
      {
        SMS::My_stop_predicate<cgal::mesh> stop(options.stop_val);

        if(options.cost==0 && options.placement==0)
          SMS::edge_collapse
            (input
            ,stop
            ,CGAL::vertex_index_map(get(CGAL::vertex_external_index,input))
              .halfedge_index_map  (get(CGAL::halfedge_external_index  ,input))
          );
        else if(options.cost==1 && options.placement == 0)
          SMS::edge_collapse
            (input
            ,stop
            ,CGAL::vertex_index_map(get(CGAL::vertex_external_index,input))
              .halfedge_index_map  (get(CGAL::halfedge_external_index  ,input))
              .get_cost (SMS::Edge_length_cost <cgal::mesh>())
          );
        else if(options.cost==0 && options.placement == 1)
          SMS::edge_collapse
            (input
            ,stop
            ,CGAL::vertex_index_map(get(CGAL::vertex_external_index,input))
              .halfedge_index_map  (get(CGAL::halfedge_external_index  ,input))
              .get_placement(SMS::Midpoint_placement<cgal::mesh>())
            );
        else
          SMS::edge_collapse
            (input
            ,stop
            ,CGAL::vertex_index_map(get(CGAL::vertex_external_index,input))
              .halfedge_index_map  (get(CGAL::halfedge_external_index  ,input))
              .get_cost (SMS::Edge_length_cost <cgal::mesh>())
              .get_placement(SMS::Midpoint_placement<cgal::mesh>())
            );
      }

      output=input;
    }




    simplify_mesh::simplify_mesh() {}

    std::string simplify_mesh::name() { return "cgal_simplify_mesh"; }

    bool simplify_mesh::run(viennamesh::algorithm_handle &)
    {
      data_handle<int> cost_option = get_input<int>("cost_option");
      data_handle<int> placement_option = get_input<int>("placement_option");

      data_handle<viennamesh_string> stop_predicate = get_required_input<viennamesh_string>("stop_predicate");
      data_handle<cgal::mesh> input_mesh = get_required_input<cgal::mesh>("mesh");

      data_handle<cgal::mesh> output_mesh = make_data<cgal::mesh>();

      cgal::mesh sm = input_mesh();
      cgal::mesh & im = sm;
      cgal::mesh & om = const_cast<cgal::mesh &>(output_mesh());

      cgal_simplify_options options;
      if (stop_predicate().find("count") != std::string::npos)
      {
        data_handle<int> count_edges = get_required_input<int>("number_of_edges");

        if(count_edges()<0)
          options.stop_val=0.0;
        else
          options.stop_val=count_edges();

        options.stop=1;
      }
      else if (stop_predicate().find("ratio") != std::string::npos)
      {
        data_handle<double> ratio = get_required_input<double>("ratio");

        if(ratio()<0)
          options.stop_val=0.0;
        else
          options.stop_val=ratio();

        options.stop=0;
      }
      else if (stop_predicate().find("area") != std::string::npos)
      {
        data_handle<double> min_area = get_required_input<double>("min_area");

        if(min_area()<0)
          options.stop_val=0.0;
        else
          options.stop_val=min_area();

        options.stop=2;
      }
      else
        return false;

      if(cost_option.valid() && cost_option()>=0 && cost_option()<2)
        options.cost=cost_option();
      else
        options.cost=0;

      if(placement_option.valid() && placement_option()>=0 && placement_option()<2)
        options.placement=placement_option();
      else
        options.cost=0;

      simplify_mesh_impl( im, om, options );
      set_output("mesh", output_mesh);

      return true;
    }
  }
}

// #endif
