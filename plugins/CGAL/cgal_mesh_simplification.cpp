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

#include "cgal_mesh.hpp"
#include "cgal_mesh_simplification.hpp"

#include <boost/algorithm/string.hpp>  // sto_lower(string str)
#include <typeinfo>   // operator typeid

// Simplification function
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>

// Stop-condition policies
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>

// Cost policies
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_length_cost.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/LindstromTurk_cost.h>

// Placement policies
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/LindstromTurk_placement.h>

//Feature preservation
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Constrained_placement.h>
#include <CGAL/Kernel/global_functions.h>
#include <CGAL/Unique_hash_map.h>



namespace SMS = CGAL::Surface_mesh_simplification ;

namespace viennamesh
{
    namespace cgal
    {
        cgal_mesh_simplification::cgal_mesh_simplification() {}
        std::string cgal_mesh_simplification::name()
        {
            return "cgal_mesh_simplification";
        }

        // Create a function that simply prints all parameters in the terminal
        template <class STOP, class COST, class PLACEMENT>
        void print_selected_parameters (STOP stop, COST cost, PLACEMENT placement)
        {
            if (typeid(stop) == typeid(SMS::Count_stop_predicate<cgal::polyhedron_surface_mesh>))
                info(5) << "Selected Stop Condition: EDGE COUNT" << std::endl;
            else if (typeid(stop) == typeid(SMS::Count_ratio_stop_predicate<cgal::polyhedron_surface_mesh>))
                info(5) << "Selected Stop Condition: EDGE RATIO" << std::endl;
            else if (typeid(stop) == typeid(void*) )
                info(5) << "Selected Stop Condition: DEFAULT" << std::endl;
            else
                info(5) << "Selected Stop Condition: UNKNOWN / CUSTOMIZED" << std::endl;

            if (typeid(cost) == typeid(SMS::LindstromTurk_cost <cgal::polyhedron_surface_mesh>))
                info(5) << "Selected Cost Policy: LINDSTROM-TURK" << std::endl;
            else if (typeid(cost) == typeid(SMS::Edge_length_cost <cgal::polyhedron_surface_mesh>))
                info(5) << "Selected Cost Policy: EDGE LENGTH" << std::endl;
            else if (typeid(cost) == typeid(void*) )
                info(5) << "Selected Cost Policy: DEFAULT" << std::endl;
            else
                info(5) << "Selected Cost Policy: UNKNOWN / CUSTOMIZED" << std::endl;

            if (typeid(placement) == typeid(SMS::LindstromTurk_placement <cgal::polyhedron_surface_mesh>))
                info(5) << "Selected Placement Policy: LINDSTROM-TURK" << std::endl;
            else if (typeid(placement) == typeid(SMS::Midpoint_placement <cgal::polyhedron_surface_mesh>))
                info(5) << "Selected Placement Policy: MIDPOINT" << std::endl;
            else if (typeid(placement) == typeid(void*) )
                info(5) << "Selected Placement Policy: DEFAULT" << std::endl;
            else
                info(5) << "Selected Placement Policy: UNKNOWN / CUSTOMIZED" << std::endl;

            return;
        }

        // Create a wrapper function that calls the original CGAL "edge_collapse()" function with the chosen input paramenters
        // this function was mainly created for a better readability and a shorter code in the main algorithm "run()"

        // constrained edges are set via constrained_edge_collapse
        template <class STOP, class COST, class PLACEMENT>
        int smart_edge_collapse (cgal::polyhedron_surface_mesh & surface_mesh, STOP stop, COST cost, PLACEMENT placement )
        {
            print_selected_parameters (stop, cost, placement);
            return SMS::edge_collapse
                   (surface_mesh
                    ,stop
                    ,CGAL::parameters::vertex_index_map(get(CGAL::vertex_external_index,surface_mesh))
                    .halfedge_index_map  (get(CGAL::halfedge_external_index, surface_mesh))
                    .get_cost (cost)
                    .get_placement(placement)
                   );
        }

        template <class STOP, class COST >
        int smart_edge_collapse (cgal::polyhedron_surface_mesh & surface_mesh, STOP stop, COST cost, void* placement)
        {
            (void) placement; // has no effect, just here to avoid compiler warnings about unused parameter

            print_selected_parameters (stop, cost, placement);
            return SMS::edge_collapse
                   (surface_mesh
                    ,stop
                    ,CGAL::parameters::vertex_index_map(get(CGAL::vertex_external_index,surface_mesh))
                    .halfedge_index_map  (get(CGAL::halfedge_external_index, surface_mesh))
                    .get_cost (cost)
                   );
        }

        template <class STOP, class PLACEMENT >
        int smart_edge_collapse (cgal::polyhedron_surface_mesh & surface_mesh, STOP stop, void* cost, PLACEMENT placement)
        {
            (void) cost; // has no effect, just here to avoid compiler warnings about unused parameter

            print_selected_parameters (stop, cost, placement);
            return SMS::edge_collapse
                   (surface_mesh
                    ,stop
                    ,CGAL::parameters::vertex_index_map(get(CGAL::vertex_external_index,surface_mesh))
                    .halfedge_index_map  (get(CGAL::halfedge_external_index, surface_mesh))
                    .get_placement (placement)
                   );
        }

        template <class STOP >
        int smart_edge_collapse (cgal::polyhedron_surface_mesh & surface_mesh, STOP stop, void* cost, void* placement)
        {
            (void) cost; // has no effect, just here to avoid compiler warnings about unused parameter
            (void) placement;

            print_selected_parameters (stop, cost, placement);
            return SMS::edge_collapse
                   (surface_mesh
                    ,stop
                    ,CGAL::parameters::vertex_index_map(get(CGAL::vertex_external_index,surface_mesh))
                    .halfedge_index_map  (get(CGAL::halfedge_external_index, surface_mesh))
                   );
        }

        /*
        * Collapses the surface_mesh with the stop predicate 'stop', the cost policy 'cost'
        * while preserving edges whose neighboring triangles' surface normals have an angle greater thant feature_angle to each other.
        * The type of the used specialication of SMS::Constrained_placement has to be given as template parameter (CONSTRAINED_PLACEMENT)
        * and a constraints map (e.g., Angle_is_constrained_edge_map below) 'cmap' has to be given as argument.
        */
        template <class CONSTRAINED_PLACEMENT, class STOP, class COST, class CONSTRAINED_MAP>
        int constrained_edge_collapse(cgal::polyhedron_surface_mesh & surface_mesh, STOP stop, COST cost,  CONSTRAINED_MAP cmap, viennagrid_numeric feature_angle)
        {

            info(5) << "FEATURE PRESERVATION MODE, critical angle = " << feature_angle << " rad" << std::endl;
            print_selected_parameters (stop, cost, cmap);

            //This solution seems to be dirty, is there a more elegant way to check angle condition and set edge constrained?
            boost::graph_traits<cgal::polyhedron_surface_mesh>::edge_iterator b,e;
            boost::tie(b,e) = edges(surface_mesh);

            for(cgal::polyhedron_surface_mesh::Edge_iterator eit = surface_mesh.edges_begin(), eend = surface_mesh.edges_end(); eit != eend; ++eit, ++b)
            {
                if(!eit->is_border_edge())
                {
                    Point_3 p11 =  eit->facet()->facet_begin()->vertex()->point();
                    Point_3 p12 =  eit->facet()->facet_begin()->next()->vertex()->point();
                    Point_3 p13 =  eit->facet()->facet_begin()->next()->next()->vertex()->point();

                    Point_3 p21 =  eit->opposite()->facet()->facet_begin()->vertex()->point();
                    Point_3 p22 =  eit->opposite()->facet()->facet_begin()->next()->vertex()->point();
                    Point_3 p23 =  eit->opposite()->facet()->facet_begin()->next()->next()->vertex()->point();

                    CGAL::Vector_3<Kernel> n1 = CGAL::cross_product(p12 - p11, p13 - p11);
                    CGAL::Vector_3<Kernel> n2 = CGAL::cross_product(p22 - p21, p23 - p22);

                    double cosine = n1 * n2 / CGAL::sqrt(n1*n1) / CGAL::sqrt(n2 * n2);

                    if(cosine > 1)
                    {
                        cosine = 1;
                    }
                    else if(cosine < -1)
                    {
                        cosine = -1;
                    }

                    double angle = std::acos(cosine);


                    if(angle > feature_angle)
                    {
                        cmap.set_constrained(*b,true);
                    }
                }
            }

            return SMS::edge_collapse
                   (surface_mesh
                    ,stop
                    ,CGAL::parameters::vertex_index_map(get(CGAL::vertex_external_index,surface_mesh))
                    .halfedge_index_map  (get(CGAL::halfedge_external_index, surface_mesh))
                    .edge_is_constrained_map(cmap)
                    .get_cost(cost)
                    .get_placement(CONSTRAINED_PLACEMENT(cmap))
                   );

        }

        //struct for associating the surface meshes' edges ith a boost graph.
        struct Angle_is_constrained_edge_map
        {

            typedef boost::graph_traits<cgal::polyhedron_surface_mesh>::edge_descriptor key_type;
            typedef bool value_type;
            typedef value_type reference;
            typedef boost::readable_property_map_tag category;
            typedef CGAL::Unique_hash_map<key_type,bool> Constrains_map;

            const cgal::polyhedron_surface_mesh* sm_ptr;
            Constrains_map mConstrains ;

            Angle_is_constrained_edge_map(const cgal::polyhedron_surface_mesh& sm)
                : sm_ptr(&sm), mConstrains(false)
            {}

            void set_constrained ( key_type const & e, bool is )
            {
                mConstrains[e]=is;
            }


            bool is_constrained( key_type const& e ) const
            {
                return mConstrains.is_defined(e) ? mConstrains[e] : false ;
            }

            //This function is called by CGAL's SMS::edge_collapse() to check if an edge is set as constrained
            friend bool get(Angle_is_constrained_edge_map m, const key_type& edge)
            {

                return m.is_constrained(edge) ;
            }
        };

// Main algorithm
        bool cgal_mesh_simplification::run(algorithm_handle&)
        {

            //Get mesh data (REQUIRED)
            data_handle<cgal::polyhedron_surface_mesh> input_mesh = get_required_input<cgal::polyhedron_surface_mesh>("mesh");

            //Get stop predicate (REQUIRED)
            data_handle<viennamesh_string> stop_predicate = get_required_input<viennamesh_string>("stop_predicate");

            //Declare data handles for the stop predicate
            data_handle<int> count_of_edges = make_data<int>();
            data_handle<viennagrid_numeric> ratio_of_edges = make_data<viennagrid_numeric>();

            //Get strategy policies (OPTIONAL) - if not set, default is LINDSTROM_TURK with standard weighting factors
            data_handle<viennamesh_string> cost_policy = get_input<viennamesh_string>("cost_policy");
            data_handle<viennamesh_string> placement_policy = get_input<viennamesh_string>("placement_policy");

            //Get lindstromturk weighting factors
            data_handle<viennagrid_numeric> lindstrom_volume_weight = get_input<viennagrid_numeric>("lindstrom_volume_weight");
            data_handle<viennagrid_numeric> lindstrom_boundary_weight = get_input<viennagrid_numeric>("lindstrom_boundary_weight");
            data_handle<viennagrid_numeric> lindstrom_shape_weight = get_input<viennagrid_numeric>("lindstrom_shape_weight");

            //Create output mesh handle
            data_handle<cgal::polyhedron_surface_mesh> output_mesh = make_data<cgal::polyhedron_surface_mesh>();

            //Declare internal reference to output mesh
            cgal::polyhedron_surface_mesh & my_mesh = const_cast<cgal::polyhedron_surface_mesh&> (output_mesh());
            my_mesh = input_mesh();

            /* Feature preservation by angle (given in rad) - Warning: Feature preservation is experimental!
             * Extremely low performance (coarsening times of even hours) can be possible!
             * It is not guaranteed that the stop edge ratio / stop edge count can be achieved, if the provided mesh posseses too many feature defining edges.
             */
            data_handle<viennagrid_numeric> constrained = get_input<viennagrid_numeric>("feature_angle");



            // Define other internal variables in order to provide case insensitivity
            std::string stop_predicate_lc= "";	// lower case version of the string
            std::string cost_policy_lc= "";		// lower case version of the string
            std::string placement_policy_lc= "";	// lower case version of the string
            int removed_edges;			// stores output of edge_collapse() function (our main CGAL function)

            // If a stop predicate was specified, read corresponding parameter
            if ( stop_predicate.valid() )
            {
                stop_predicate_lc = stop_predicate();
                boost::algorithm::to_lower(stop_predicate_lc); //make string lower case
            }

            // If a cost policy was specified, read corresponding parameter
            if ( cost_policy.valid() )
            {
                cost_policy_lc = cost_policy();
                boost::algorithm::to_lower(cost_policy_lc); //make string lower case
            }

            // If a placement policy was specified, read corresponding parameter
            if ( placement_policy.valid() )
            {
                placement_policy_lc = placement_policy();
                boost::algorithm::to_lower(placement_policy_lc); //make string lower case
            }

            //LindstromTurk default values (the same as the default values given in CGAL Triangulated Surface Mesh Simplifiaction Reference Manual)
            viennagrid_numeric volume_weight = 0.5;
            viennagrid_numeric boundary_weight = 0.5;
            viennagrid_numeric shape_weight = 0.0;



            if(lindstrom_volume_weight.valid())
            {
                volume_weight = lindstrom_volume_weight();
            }

            if(lindstrom_boundary_weight.valid())
            {
                boundary_weight = lindstrom_boundary_weight();
            }

            if(lindstrom_shape_weight.valid())
            {
                shape_weight = lindstrom_shape_weight();
            }


            //--------- Extensive if statement with the target to call the CGAL simplification algorithm 'edge_collapse' with the provided parameters/policies -------//

            //------------------------------------------------------------
            // -------------- Case of STOP PREDICATE = "COUNT" -----------
            //------------------------------------------------------------


            if (stop_predicate_lc == "count")
            {
                // ---------Get required Parameter, check if valid, and display status in terminal ---------
                count_of_edges = get_required_input<int>("count");

                if (count_of_edges()<3)
                {
                    error(1)<< count_of_edges() << " is not equal to or greater than 3 (number_of_edges).\n";
                    return false;
                }
                else
                    info(5) << "Selected Stop Parameter: edge_number <= " << count_of_edges() << "\n";
                // -----------------------------------------------------------------------------------------

                SMS::Count_stop_predicate<cgal::polyhedron_surface_mesh> stop(count_of_edges());

                if (cost_policy_lc == "lindstromturk" || cost_policy_lc == "lindstrom-turk" )
                {
                    SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));


                    if (placement_policy_lc == "lindstromturk" || placement_policy_lc == "lindstrom-turk" )
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh>,
                                    Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());


                        }
                        else //standard placement
                        {
                            SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh> placement(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                    else if (placement_policy_lc == "midpoint"  || placement_policy_lc == "mid-point")
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::Midpoint_placement<cgal::polyhedron_surface_mesh>,
                                    Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());


                        }
                        else
                        {
                            SMS::Midpoint_placement <cgal::polyhedron_surface_mesh> placement;
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                    else
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh>,
                                    Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);

                            info(1) << "No placement policy for feature preservation given. Set to default value lindstrom-turk" << std::endl;
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());


                        }
                        else //standard placement
                        {


                            void* placement = NULL;
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                }

                else if (cost_policy_lc == "edgelength" || cost_policy_lc == "edge-length")
                {
                    SMS::Edge_length_cost <cgal::polyhedron_surface_mesh> cost;

                    if (placement_policy_lc == "lindstromturk" || placement_policy_lc == "lindstrom-turk" )
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh>,
                                    Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());

                        }
                        else
                        {

                            SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh> placement(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                    else if (placement_policy_lc == "midpoint" || placement_policy_lc == "mid-point")
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::Midpoint_placement<cgal::polyhedron_surface_mesh>,
                                    Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());


                        }
                        else
                        {
                            SMS::Midpoint_placement <cgal::polyhedron_surface_mesh> placement;
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                    else
                    {

                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::Midpoint_placement<cgal::polyhedron_surface_mesh>,
                                    Angle_is_constrained_edge_map > CplacementT;

                            info(1) << "No placement policy for feature preservation given. Set to default value lindstrom-turk" << std::endl;
                            Angle_is_constrained_edge_map cmap(my_mesh);
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());
                        }


                        void* placement = NULL;
                        removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                    }
                }

                else
                {
                    void* cost = NULL;

                    if (placement_policy_lc == "lindstromturk" || placement_policy_lc == "lindstrom-turk" )
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh>, Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);

                            info(1) << "No cost policy for feature preservation given. Set to default value lindstrom-turk" << std::endl;
                            SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));

                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());


                        }
                        else
                        {
                            SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh> placement(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                    else if (placement_policy_lc == "midpoint" || placement_policy_lc == "mid-point")
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::Midpoint_placement<cgal::polyhedron_surface_mesh>,
                                    Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);

                            info(1) << "No cost policy for feature preservation given. Set to default value lindstrom-turk" << std::endl;
                            SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));

                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());


                        }
                        else
                        {
                            SMS::Midpoint_placement <cgal::polyhedron_surface_mesh> placement;
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                    else
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh>,
                                    Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);

                            info(1) << "No cost policy for feature preservation given. Set to default value lindstrom-turk" << std::endl;
                            info(1) << "No placement policy for feature preservation given. Set to default value lindstrom-turk" << std::endl;
                            SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));

                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());


                        }
                        else
                        {
                            void* placement = NULL;
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                }
            }

            //------------------------------------------------------------
            //------------ Case of STOP PREDICATE = "RATIO" --------------
            //------------------------------------------------------------

            else if (stop_predicate_lc == "ratio" )
            {
                // ---------Get required Parameter, check if valid, and display status in terminal ---------
                ratio_of_edges = get_required_input<viennagrid_numeric>("ratio");

                if (ratio_of_edges() <= 0 || 1 < ratio_of_edges() )
                {
                    error(1) << ratio_of_edges() <<" is not a value between 0 and 1 (ratio_of_edges).\n";
                    return false;
                }
                else
                    info(5) << "Selected Stop Parameter: edge_ratio = " << ratio_of_edges() << "\n";
                // -----------------------------------------------------------------------------------------


                SMS::Count_ratio_stop_predicate<cgal::polyhedron_surface_mesh> stop( ratio_of_edges() );

                if (cost_policy_lc == "lindstromturk" || cost_policy_lc == "lindstrom-turk")
                {
                    SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));


                    if (placement_policy_lc == "lindstromturk" || placement_policy_lc == "lindstrom-turk" )
                    {

                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh>, Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);
                            SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());


                        }
                        else
                        {

                            SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh> placement(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                    else if (placement_policy_lc == "midpoint" || placement_policy_lc == "mid-point")
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::Midpoint_placement<cgal::polyhedron_surface_mesh>, Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);
                            SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());


                        }
                        else
                        {

                            SMS::Midpoint_placement <cgal::polyhedron_surface_mesh> placement;
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                    else
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh>, Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);
                            SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));
                            info(1) << "No placement policy for feature preservation given. Set to default value lindstrom-turk" << std::endl;

                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());


                        }
                        else
                        {
                            void* placement = NULL;
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                }

                else if (cost_policy_lc == "edgelength" || cost_policy_lc == "edge-length" )
                {
                    SMS::Edge_length_cost <cgal::polyhedron_surface_mesh> cost;

                    if (placement_policy_lc == "lindstromturk" || placement_policy_lc == "lindstrom-turk" )
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh>, Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());


                        }
                        else
                        {
                            SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh> placement(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                    else if (placement_policy_lc == "midpoint" || placement_policy_lc == "mid-point")
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::Midpoint_placement<cgal::polyhedron_surface_mesh>, Angle_is_constrained_edge_map > CplacementT;
                            Angle_is_constrained_edge_map cmap(my_mesh);

                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());
                        }
                        else
                        {
                            SMS::Midpoint_placement <cgal::polyhedron_surface_mesh> placement;
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                    else
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh>, Angle_is_constrained_edge_map > CplacementT;

                            Angle_is_constrained_edge_map cmap(my_mesh);
                            info(1) << "No placement policy for feature preservation given. Set to default value lindstrom-turk" << std::endl;
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());


                        }
                        else
                        {
                            void* placement = NULL;
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                }

                else
                {
                    void* cost = NULL;

                    if (placement_policy_lc == "lindstromturk" || placement_policy_lc == "lindstrom-turk" )
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh>, Angle_is_constrained_edge_map > CplacementT;
                            Angle_is_constrained_edge_map cmap(my_mesh);
                            SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));
                            info(1) << "No cost policy for feature preservation given. Set to default value lindstrom-turk" << std::endl;
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());
                        }
                        else
                        {

                            SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh> placement(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                    else if (placement_policy_lc == "midpoint" || placement_policy_lc == "mid-point")
                    {
                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::Midpoint_placement<cgal::polyhedron_surface_mesh>, Angle_is_constrained_edge_map > CplacementT;
                            Angle_is_constrained_edge_map cmap(my_mesh);
                            SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));
                            info(1) << "No cost policy for feature preservation given. Set to default value lindstrom-turk" << std::endl;
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());
                        }
                        else
                        {

                            SMS::Midpoint_placement <cgal::polyhedron_surface_mesh> placement;
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                    else
                    {

                        if(constrained.valid()) //feature preservation by angle
                        {
                            typedef typename SMS::Constrained_placement<SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh>, Angle_is_constrained_edge_map > CplacementT;
                            Angle_is_constrained_edge_map cmap(my_mesh);
                            SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost(SMS::LindstromTurk_params(volume_weight,boundary_weight,shape_weight));
                            info(1) << "No cost policy for feature preservation given. Set to default value lindstrom-turk" << std::endl;
                            info(1) << "No placement policy for feature preservation given. Set to default value lindstrom-turk" << std::endl;
                            removed_edges = constrained_edge_collapse<CplacementT>(my_mesh, stop, cost, cmap, constrained());
                        }
                        else
                        {
                            void* placement = NULL;
                            removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
                        }
                    }
                }
            }



            //------------------------------------------------------------
            //------------ Case of STOP PREDICATE = INVALID --------------
            //------------------------------------------------------------
            else
            {
                error(1) <<"\""<<stop_predicate_lc << "\" is not a valid parameter for stop_predicate.\n";
                return false;
            }

            //------------------------------------------------------------
            //---------------------------- END ---------------------------
            //------------------------------------------------------------

            info(5) << "Done...\n" << input_mesh().size_of_halfedges()/2 << " original edges.\n" << removed_edges << " edges removed.\n" << my_mesh.size_of_halfedges()/2 << " final edges.\n" ;
            set_output("mesh", my_mesh);

            return true;
        }
    }
}

// EOF //
