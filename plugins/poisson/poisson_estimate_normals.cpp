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
#include "poisson_mesh.hpp"
#include "poisson_estimate_normals.hpp"
#include <CGAL/pca_estimate_normals.h>
#include <CGAL/jet_estimate_normals.h>
#include <CGAL/mst_orient_normals.h>
namespace viennamesh
{
  namespace poisson
  {

    struct estimate_options{
      int del;
      int jet;
      int jet_degree;
    };

    void out(PairList & input){
      std::cout << "size: "<<input.size()<<"\n";
      for (PairList::iterator it=input.begin() ; it != input.end(); ++it)
        std::cout << it->first<< " - "<<it->second<<"\n";
    }
    void estimate_normals_impl(PairList & input,
                        data_handle<poisson::Vector> output,
                        struct estimate_options options)
    {
      out(input);
      const int nb_neighbors = 6; // K-nearest neighbors = 3 rings

      if(options.jet==0)
        CGAL::pca_estimate_normals(input.begin(), input.end(),
                                 CGAL::First_of_pair_property_map<PointVectorPair>(),
                                 CGAL::Second_of_pair_property_map<PointVectorPair>(),
                                 nb_neighbors);
      else
        CGAL::jet_estimate_normals(input.begin(), input.end(),
                                 CGAL::First_of_pair_property_map<PointVectorPair>(),
                                 CGAL::Second_of_pair_property_map<PointVectorPair>(),
                                 nb_neighbors,poisson::Kernel(),options.jet_degree);
        
      PairList::iterator unoriented_points_begin =
          CGAL::mst_orient_normals(input.begin(), input.end(),
                                   CGAL::First_of_pair_property_map<PointVectorPair>(),
                                   CGAL::Second_of_pair_property_map<PointVectorPair>(),
                                   nb_neighbors);
      // Optional: delete points with an unoriented normal
      // if you plan to call a reconstruction algorithm that expects oriented normals.
      if(options.del>0)
        input.erase(unoriented_points_begin, input.end());
      out(input);
      output.resize( input.size() );
      int i=0;
      for (PairList::iterator it=input.begin() ; it != input.end(); ++it,++i){
      poisson::Vector & om = const_cast<poisson::Vector &>(output(i));
        om=poisson::Vector(it->second);
      }
    }

    estimate_normals::estimate_normals() {}

    std::string estimate_normals::name() { return "poisson_estimate_normals"; }

    bool estimate_normals::run(viennamesh::algorithm_handle &)
    {
      data_handle<int> delete_option = get_input<int>("delete_unoriented");
      data_handle<int> jet_option = get_input<int>("use_jet_estimation");
      data_handle<int> jet_degree_option = get_input<int>("jet_degree");
      point_handle input_points = get_required_input<point_handle>("points");
      struct estimate_options options;
      data_handle<poisson::Vector> output = make_data<poisson::Vector>();
      
      PairList mypoints;
      for (int i = 0; i != input_points.size(); ++i)
        mypoints.push_back(std::pair<Point,Vector>(Point(input_points(i)[0],input_points(i)[1],input_points(i)[2]),Vector()));  
  
      options.del=0;
      if(delete_option.valid())
        options.del=delete_option();
      options.jet=0;
      options.jet_degree=1;
      if(jet_option.valid()){
        options.jet=jet_option();
        if(jet_degree_option.valid() && jet_degree_option()>0)
          options.jet_degree=jet_degree_option();
      }
      PairList sm = mypoints;
      PairList & im = sm;
      std::cout << "options: " << options.del <<" - " <<options.jet <<" - " << options.jet_degree;
      estimate_normals_impl(im,output,options);

      set_output("normals", output);
      return true;
    }
  }
}

// #endif
