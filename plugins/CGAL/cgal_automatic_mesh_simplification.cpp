/* ============================================================================
   Copyright (c) 2011-2016, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

/*
 *   This algorithm depends on the mesh comparison capabilities of the statistics plugin. Thus, statistics plugin must be enabled!
*/

#include "cgal_mesh.hpp"
#include "cgal_automatic_mesh_simplification.hpp"



namespace viennamesh
{
    namespace cgal
    {
        cgal_automatic_mesh_simplification::cgal_automatic_mesh_simplification() {}
        std::string cgal_automatic_mesh_simplification::name()
        {
            return "cgal_automatic_mesh_simplification";
        }



        /* Lindstrom Turk parameter determination*
         *
         * Note that a parameter sweep results typically in a non-predictable and non-monotonic quality metric behaviour. Thus, only a trial and error approach
         * can be followed up. However, different parameter settings normally yield quality differences < 3% So, the following
         * non-sophisticated sweep may not be worthwhile at all.
         *
         * Parallelization possible!
         */

        bool best_policy_setup_determination(viennamesh::algorithm_handle& coarser, viennamesh::algorithm_handle& stats, std::string& cost_result, std::string& placement_result, viennagrid_numeric& volW_result, viennagrid_numeric& boundW_result,
                                             viennagrid_numeric& shapeW_result, viennagrid_numeric& best_metric, data_handle<cgal::polyhedron_surface_mesh>& best_coarsened_mesh)
        {
            const std::pair<std::string,std::string> policiesC[4] = { {"lindstrom-turk", "lindstrom-turk"}, {"lindstrom-turk", "midpoint"}, {"edgelength", "lindstrom-turk"}, {"edgelength", "midpoint"} };


            best_metric = -1; //negative value == invalid
            std::size_t best_policy = 0; //index of best policy


            //for lindstrom-turk parameter determination


            std::size_t best_lindstrom_parameter_triple[3] = {0,0,0}; //indices of weightC array below
            const std::size_t weightsC_len = 8;
            const double weightsC[weightsC_len] = {0, 0.1,  0.2, 0.33, 0.4, 0.5, 0.6, 0.7};




            for(std::size_t i = 0; i < 4; ++i) //4 = number of possible policy combinations
            {
                viennamesh::LoggingStack stack( std::string("Coarsening with policies: (" + policiesC[i].first + ", " + policiesC[i].second + ")") );
                coarser.set_input("cost_policy", policiesC[i].first);
                coarser.set_input("placement_policy",policiesC[i].second);

                viennagrid_numeric metric_value;


                if( (policiesC[i].first == "lindstrom-turk") && (policiesC[i].second == "lindstrom-turk") )
                {
                    //every combination of parameters given in weightC is tried out
                    for(std::size_t i0 = 0; i0 < weightsC_len; ++i0)
                    {
                        for(std::size_t i1 = 0; i1 < weightsC_len; ++i1)
                        {
                            for(std::size_t i2 = 0; i2 < weightsC_len; ++i2)
                            {
                                if( (weightsC[i0] == 0) && (weightsC[i1] == 0) && (weightsC[i2] == 0) ) //all zero typically leads to disastrous mesh quality, which can lead to segmentation faults inside libigl principal_curvature
                                    continue;

                                info(5) << "LINDSTROM-TURK Weights:\n\tvolume weight = " << weightsC[i0] << ", boundary weight = " << weightsC[i1]
                                        << ", shape weight = " << weightsC[i2] << std::endl;

                                coarser.set_input("lindstrom_volume_weight", weightsC[i0]);
                                coarser.set_input("lindstrom_boundary_weight", weightsC[i1]);
                                coarser.set_input("lindstrom_shape_weight", weightsC[i2]);
                                coarser.run();

                                stats.set_input("mesh", coarser.get_output("mesh"));
                                stats.run();

                                metric_value = stats.get_output<viennagrid_numeric>("mesh_quality_metric")();

                                if(metric_value < best_metric ||  best_metric < 0)
                                {
                                    best_metric = metric_value;

                                    best_coarsened_mesh = coarser.get_output<cgal::polyhedron_surface_mesh>("mesh");

                                    best_lindstrom_parameter_triple[0] = i0;
                                    best_lindstrom_parameter_triple[1] = i1;
                                    best_lindstrom_parameter_triple[2] = i2;
                                }
                            }
                        }
                    }

                }
                else
                {
                    //for policy combinations that involve lindstrom-turk, choose the parameter set that previously resulted in the best quality
                    coarser.set_input("lindstrom_volume_weight", weightsC[best_lindstrom_parameter_triple[0]]);
                    coarser.set_input("lindstrom_boundary_weight", weightsC[best_lindstrom_parameter_triple[1]]);
                    coarser.set_input("lindstrom_shape_weight", weightsC[best_lindstrom_parameter_triple[2]]);

                    coarser.run();

                    stats.set_input("mesh", coarser.get_output("mesh"));
                    stats.run();

                    metric_value = stats.get_output<viennagrid_numeric>("mesh_quality_metric")();

                    if(metric_value < best_metric ||  best_metric < 0)
                    {
                        best_metric = metric_value;
                        best_policy = i;
                        best_coarsened_mesh = coarser.get_output<cgal::polyhedron_surface_mesh>("mesh");
                    }
                }
            }


            cost_result = policiesC[best_policy].first;
            placement_result = policiesC[best_policy].second;
            volW_result =  weightsC[best_lindstrom_parameter_triple[0]];
            boundW_result =  weightsC[best_lindstrom_parameter_triple[1]];
            shapeW_result = weightsC[best_lindstrom_parameter_triple[2]];



            return true;

        }


        /*
           Faster, but it is quite likely that the global mesh quality metric minimum is not found.
           The algorithm is based purely on empirical observation of the test meshes' quality arising from different weighting factor combinations.
        */
        bool fast_best_policy_setup_determination(viennamesh::algorithm_handle& coarser, viennamesh::algorithm_handle& stats, std::string& cost_result, std::string& placement_result, viennagrid_numeric& volW_result, viennagrid_numeric& boundW_result,
                viennagrid_numeric& shapeW_result, viennagrid_numeric& best_metric, data_handle<cgal::polyhedron_surface_mesh>& best_coarsened_mesh)
        {


            const std::pair<std::string,std::string> policiesC[4] = { {"lindstrom-turk", "lindstrom-turk"}, {"lindstrom-turk", "midpoint"}, {"edgelength", "lindstrom-turk"}, {"edgelength", "midpoint"} };


            best_metric = -1; //negative value == invalid
            std::size_t best_policy = 0; //index of best policy


            //for lindstrom-turk parameter determination

            viennagrid_numeric metric_value;


            std::size_t policy_index = 0;

            //(lindstrom-turk, lindstrom turk) section
            {

                viennamesh::LoggingStack stack( std::string("Coarsening with policies: (" + policiesC[policy_index].first + ", " + policiesC[policy_index].second + ")") );
                coarser.set_input("cost_policy", policiesC[policy_index].first);
                coarser.set_input("placement_policy",policiesC[policy_index].second);


                /* First try combination with one weight significantly smaller than the others
                *      Note that setting one or more weights exactly to 0 can possibly lead to extremely poor mesh quality,
                *      which in turn can prevent to calculate curvature differences using libigl.
                */
                const size_t guess_number = 3;
                const viennagrid_numeric initial_guesses[guess_number][3] = { {0.45, 0.45, 0.1}, {0.45, 0.1, 0.45}, {0.1, 0.45, 0.45} } ;

                for(size_t i = 0; i < guess_number; ++i)
                {
                    if( (initial_guesses[i][0] == 0) && (initial_guesses[i][1] == 0) && (initial_guesses[i][2] == 0) ) //all zero typically leads to disastrous mesh quality, which can lead to segmentation faults inside libigl principal_curvature
                        continue;


                    info(5) << "LINDSTROM-TURK Weights:\n\tvolume weight = " << initial_guesses[i][0] << ", boundary weight = " << initial_guesses[i][1]
                            << ", shape weight = " << initial_guesses[i][2]<< std::endl;

                    coarser.set_input("lindstrom_volume_weight", initial_guesses[i][0]);
                    coarser.set_input("lindstrom_boundary_weight", initial_guesses[i][1]);
                    coarser.set_input("lindstrom_shape_weight", initial_guesses[i][2]);
                    coarser.run();

                    stats.set_input("mesh", coarser.get_output("mesh"));
                    stats.run();

                    metric_value = stats.get_output<viennagrid_numeric>("mesh_quality_metric")();

                    if(metric_value < best_metric ||  best_metric < 0)
                    {
                        best_metric = metric_value;

                        best_coarsened_mesh = coarser.get_output<cgal::polyhedron_surface_mesh>("mesh");

                        best_policy = policy_index;
                        volW_result = initial_guesses[i][0];
                        boundW_result = initial_guesses[i][1];
                        shapeW_result = initial_guesses[i][2];
                    }
                }

                //now try distinctive combinations without zero weights

                const std::size_t weightsC_len = 10;
                const double weightsC[weightsC_len] = {0.1, 0.2, 0.33, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1};

                //starting point is (0.333, 0.333, 0.333)
                viennagrid_numeric curr_weights[3] = {0.333, 0.333, 0.333};

                //volume weight is varied
                for(std::size_t i = 0; i < weightsC_len; ++i)
                {
                    info(5) << "LINDSTROM-TURK Weights:\n\tvolume weight = " << weightsC[i] << ", boundary weight = " << curr_weights[1]
                            << ", shape weight = " << curr_weights[2] << std::endl;

                    coarser.set_input("lindstrom_volume_weight", weightsC[i]);
                    coarser.set_input("lindstrom_boundary_weight", curr_weights[1]);
                    coarser.set_input("lindstrom_shape_weight", curr_weights[2] );
                    coarser.run();

                    stats.set_input("mesh", coarser.get_output("mesh"));
                    stats.run();

                    metric_value = stats.get_output<viennagrid_numeric>("mesh_quality_metric")();

                    if(metric_value < best_metric ||  best_metric < 0)
                    {
                        best_metric = metric_value;
                        best_coarsened_mesh = coarser.get_output<cgal::polyhedron_surface_mesh>("mesh");
                        best_policy = policy_index;

                        curr_weights[0] = weightsC[i];

                        volW_result = curr_weights[0];
                        boundW_result = curr_weights[1];
                        shapeW_result = curr_weights[2];
                    }
                }

                //boundary weight is varied
                for(std::size_t i = 0; i < weightsC_len; ++i)
                {
                    info(5) << "LINDSTROM-TURK Weights:\n\tvolume weight = " << curr_weights[0] << ", boundary weight = " << weightsC[i]
                            << ", shape weight = " << curr_weights[2] << std::endl;

                    coarser.set_input("lindstrom_volume_weight",curr_weights[0]);
                    coarser.set_input("lindstrom_boundary_weight", weightsC[i]);
                    coarser.set_input("lindstrom_shape_weight", curr_weights[2] );
                    coarser.run();

                    stats.set_input("mesh", coarser.get_output("mesh"));
                    stats.run();

                    metric_value = stats.get_output<viennagrid_numeric>("mesh_quality_metric")();

                    if(metric_value < best_metric ||  best_metric < 0)
                    {
                        best_metric = metric_value;
                        best_coarsened_mesh = coarser.get_output<cgal::polyhedron_surface_mesh>("mesh");
                        best_policy = policy_index;

                        curr_weights[1] = weightsC[i];

                        volW_result = curr_weights[0];
                        boundW_result = curr_weights[1];
                        shapeW_result = curr_weights[2];
                    }
                }

                //shape weight is varied
                for(std::size_t i = 0; i < weightsC_len; ++i)
                {
                    info(5) << "LINDSTROM-TURK Weights:\n\tvolume weight = " << curr_weights[0] << ", boundary weight = " << curr_weights[1]
                            << ", shape weight = " << weightsC[i] << std::endl;

                    coarser.set_input("lindstrom_volume_weight",curr_weights[0]);
                    coarser.set_input("lindstrom_boundary_weight", curr_weights[1]);
                    coarser.set_input("lindstrom_shape_weight",  weightsC[i] );
                    coarser.run();

                    stats.set_input("mesh", coarser.get_output("mesh"));
                    stats.run();

                    metric_value = stats.get_output<viennagrid_numeric>("mesh_quality_metric")();

                    if(metric_value < best_metric ||  best_metric < 0)
                    {
                        best_metric = metric_value;
                        best_coarsened_mesh = coarser.get_output<cgal::polyhedron_surface_mesh>("mesh");
                        best_policy = policy_index;

                        curr_weights[2] = weightsC[i];

                        volW_result = curr_weights[0];
                        boundW_result = curr_weights[1];
                        shapeW_result = curr_weights[2];
                    }
                }


            }//(lindstrom-turk, lindstrom turk) section end


            //all other policy combinations start
            for(policy_index = 1; policy_index < 4; ++policy_index)
            {
                viennamesh::LoggingStack stack( std::string("Coarsening with policies: (" + policiesC[policy_index].first + ", " + policiesC[policy_index].second + ")") );
                coarser.set_input("cost_policy", policiesC[policy_index].first);

                coarser.set_input("lindstrom_volume_weight",volW_result);
                coarser.set_input("lindstrom_boundary_weight", boundW_result);
                coarser.set_input("lindstrom_shape_weight", shapeW_result);

                coarser.run();

                stats.set_input("mesh", coarser.get_output("mesh"));
                stats.run();

                metric_value = stats.get_output<viennagrid_numeric>("mesh_quality_metric")();

                if(metric_value < best_metric ||  best_metric < 0)
                {
                    best_metric = metric_value;
                    best_policy = policy_index;
                    best_coarsened_mesh = coarser.get_output<cgal::polyhedron_surface_mesh>("mesh");
                }

            } //all other policy combinations end



            cost_result = policiesC[best_policy].first;
            placement_result = policiesC[best_policy].second;

            return true;

        }



        typedef enum
        {
            NOT_SET,
            COUNT,
            RATIO

        } STOP_MODE; //chosen stop predicate (edge count or edge ratio)

        // Main algorithm
        bool cgal_automatic_mesh_simplification::run(algorithm_handle&)
        {
            //Algorithm input setup

            /*Get mesh data: Two different handles are created from the SAME input: cgal::polyhedron_surface_mesh and viennagrid::mesh
            * This is done in order to enhance performance.
            */
            data_handle<cgal::polyhedron_surface_mesh> input_mesh_cgal = get_required_input<cgal::polyhedron_surface_mesh>("mesh");
            mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");


            //Get stop predicate
            data_handle<viennamesh_string> stop_predicate = get_required_input<viennamesh_string>("stop_predicate");

            //Declare data handles for stop predicates
            data_handle<int> count_of_edges = make_data<int>();
            data_handle<viennagrid_numeric> ratio_of_edges = make_data<viennagrid_numeric>();

            //mesh quality weights
            data_handle<viennagrid_numeric> alpha = get_input<viennagrid_numeric>("alpha");
            data_handle<viennagrid_numeric> beta = get_input<viennagrid_numeric>("beta");
            data_handle<viennagrid_numeric> gamma = get_input<viennagrid_numeric>("gamma");
            data_handle<viennagrid_numeric> delta = get_input<viennagrid_numeric>("delta");

            /*feature preservation by angle (given in rad) - Warning: Feature preservation is experimental!
             *Extremely low performance (coarsening times of even hours) can be possible!
             */
            data_handle<viennagrid_numeric> feature_angle = get_input<viennagrid_numeric>("feature_angle");



            STOP_MODE mode = NOT_SET;

            if (stop_predicate() == "count")
            {
                count_of_edges = get_required_input<int>("count");
                mode = COUNT;
            }
            else if(stop_predicate() == "ratio")
            {
                ratio_of_edges = get_required_input<viennagrid_numeric>("ratio");
                mode = RATIO;
            }



            // Algorithm body
            viennamesh::context_handle context;


            //CGAL setup
            viennamesh::algorithm_handle coarser = context.make_algorithm("cgal_mesh_simplification");
            coarser.set_input("mesh", input_mesh_cgal());
            coarser.set_input("stop_predicate", stop_predicate());

            switch(mode)
            {
            case COUNT:
                coarser.set_input("count", count_of_edges());
                break;
            case RATIO:
                coarser.set_input("ratio", ratio_of_edges());
                break;
            default:
                error(1) << "STOP PREDICATE invalid" << std::endl;
                return false;
            }

            if(feature_angle.valid()) //Warning: feature preservation is experimental!
            {
                coarser.set_input("feature_angle", feature_angle());
            }


            //CGAL setup end

            //Statistics algorithm setup
            viennamesh::algorithm_handle stats = context.make_algorithm("make_statistic");

            stats.set_input("original_mesh", input_mesh());
            stats.set_input("metric_type", "radius_ratio"); //radius ratio provides very accurate triangle shape quality metric

            //weighting factors for calculation of comprehensive mesh quality metric. All have to be given, otherwise default values are used.

            if(alpha.valid() && beta.valid() && gamma.valid() && delta.valid() )
            {
                stats.set_input("alpha", alpha());
                stats.set_input("beta", beta());
                stats.set_input("gamma", gamma());
                stats.set_input("delta", delta());
            }

            //Statistics algorithm setup end


            std::string cost_result, placement_result; //best cost and placement policy, used for printing out to info()
            viennagrid_numeric volW, boundW, shapeW; //lindstromturk parameters (volume, boundary, shape)
            viennagrid_numeric best_metric;          //best mesh quality metric

            //coarsened mesh that is eventually set as output of this algorithm
            data_handle<cgal::polyhedron_surface_mesh> best_coarsened_mesh = make_data<cgal::polyhedron_surface_mesh>();


            //select here policy determination function
            fast_best_policy_setup_determination(coarser, stats, cost_result, placement_result, volW, boundW, shapeW, best_metric, best_coarsened_mesh);
            //best_policy_setup_determination(coarser, stats, cost_result, placement_result, volW, boundW, shapeW, best_metric, best_coarsened_mesh);


            // --- Printing to info(5) ---
            info(5) << "Best mesh quality metric = " << best_metric << ", achieved with" << std::endl;
            info(5) << "policy = (" << cost_result << ", " << placement_result << ")" << std::endl;

            if( ( cost_result != "edgelength") || (placement_result != "midpoint")  )
            {
                info(5) << "\tparameters: volume weight = " << volW << ", boundary weight = " << boundW
                        << ", shape weight = " << shapeW << std::endl;
            }

            info(5) << "Done!\nBest mesh has " <<  best_coarsened_mesh().size_of_halfedges()/2 << " final edges.\n" ;



            set_output("mesh", best_coarsened_mesh());
            set_output("best_metric", best_metric);
            set_output("best_cost_policy", cost_result);
            set_output("best_placement_policy", placement_result);

            return true;


        }
    }
}

// EOF //
