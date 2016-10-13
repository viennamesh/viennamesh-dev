#include "cgal_mesh.hpp"
#include "cgal_mesh_simplification.hpp"

#include <string>
#include <boost/algorithm/string.hpp>  // sto_lower(string str)
#include <typeinfo>   // operator typeid

// Basic CGAL Libraries
//#include <CGAL/Simple_cartesian.h> 				//included in header file
//#include <CGAL/Polyhedron_3.h>				//included in header file
//#include <CGAL/IO/Polyhedron_iostream.h>			//not needed anymore
//#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>	//not needed anymore

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

namespace SMS = CGAL::Surface_mesh_simplification ;

namespace viennamesh
{  
  namespace cgal
  { 
    cgal_mesh_simplification::cgal_mesh_simplification() {}
    std::string cgal_mesh_simplification::name() { return "cgal_mesh_simplification"; }
    
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
    template <class STOP, class COST, class PLACEMENT>
    int smart_edge_collapse (cgal::polyhedron_surface_mesh & surface_mesh, STOP stop, COST cost, PLACEMENT placement )
    {
      print_selected_parameters (stop, cost, placement);
      return SMS::edge_collapse
		      (surface_mesh
		      ,stop
		      ,CGAL::parameters::vertex_index_map(get(CGAL::vertex_external_index,surface_mesh)) 
					.halfedge_index_map  (get(CGAL::halfedge_external_index  , surface_mesh)) 
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
					.halfedge_index_map  (get(CGAL::halfedge_external_index  , surface_mesh)) 
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
					.halfedge_index_map  (get(CGAL::halfedge_external_index  , surface_mesh)) 
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
					.halfedge_index_map  (get(CGAL::halfedge_external_index  , surface_mesh))
		      );
    }
  
    // Main algorithm
    bool cgal_mesh_simplification::run(algorithm_handle&)
    {
      
      //Get mesh data (REQUIRED)
      data_handle<cgal::polyhedron_surface_mesh> input_mesh = get_required_input<cgal::polyhedron_surface_mesh>("mesh");
      
      //Get stop predicate (OPTIONAL)
      data_handle<viennamesh_string> stop_predicate = get_required_input<viennamesh_string>("stop_predicate");
      
      //Define values for selected stop predicate (OPTIONAL)
      data_handle<int> count_of_edges = make_data<int>();
      data_handle<double> ratio_of_edges = make_data<double>();
      
      //Get strategy policies (OPTIONAL)
      data_handle<viennamesh_string> cost_policy = get_input<viennamesh_string>("cost_policy");
      data_handle<viennamesh_string> placement_policy = get_input<viennamesh_string>("placement_policy");
      
      //Create output mesh handle
      data_handle<cgal::polyhedron_surface_mesh> output_mesh = make_data<cgal::polyhedron_surface_mesh>();
      
      //Declare internal reference to output mesh
      cgal::polyhedron_surface_mesh & my_mesh = const_cast<cgal::polyhedron_surface_mesh&> (output_mesh());
      my_mesh = input_mesh();
      
      // Define other internal Variables
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
		  SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost;
	      
		  if (placement_policy_lc == "lindstromturk" || placement_policy_lc == "lindstrom-turk" )
		  {
			SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh> placement;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }	      
		  else if (placement_policy_lc == "midpoint"  || placement_policy_lc == "mid-point")
		  {
			SMS::Midpoint_placement <cgal::polyhedron_surface_mesh> placement;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }
		  else
		  {
			void* placement = NULL;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }
	    }

	    else if (cost_policy_lc == "edgelength" || cost_policy_lc == "edge-length")
	    {
		  SMS::Edge_length_cost <cgal::polyhedron_surface_mesh> cost;
	      
		  if (placement_policy_lc == "lindstromturk" || placement_policy_lc == "lindstrom-turk" )
		  {
			SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh> placement;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }	      
		  else if (placement_policy_lc == "midpoint" || placement_policy_lc == "mid-point")
		  {
			SMS::Midpoint_placement <cgal::polyhedron_surface_mesh> placement;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }
		  else
		  {
			void* placement = NULL;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }
	    }   
	    
	    else
	    {
		  void* cost = NULL;
	      
		  if (placement_policy_lc == "lindstromturk" || placement_policy_lc == "lindstrom-turk" )
		  {
			SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh> placement;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }	      
		  else if (placement_policy_lc == "midpoint" || placement_policy_lc == "mid-point")
		  {
			SMS::Midpoint_placement <cgal::polyhedron_surface_mesh> placement;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }
		  else
		  {
			void* placement = NULL;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }
	    }   
      }

      //------------------------------------------------------------
      //------------ Case of STOP PREDICATE = "RATIO" --------------
      //------------------------------------------------------------
      
      else if (stop_predicate_lc == "ratio" )
      {
	    // ---------Get required Parameter, check if valid, and display status in terminal ---------
	    ratio_of_edges = get_required_input<double>("ratio");
	  
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
		  SMS::LindstromTurk_cost<cgal::polyhedron_surface_mesh> cost;
	      
		  if (placement_policy_lc == "lindstromturk" || placement_policy_lc == "lindstrom-turk" )
		  {
			SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh> placement;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }	      
		  else if (placement_policy_lc == "midpoint" || placement_policy_lc == "mid-point")
		  {
			SMS::Midpoint_placement <cgal::polyhedron_surface_mesh> placement;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }
		  else
		  {
			void* placement = NULL;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }
	    }
	    
	    else if (cost_policy_lc == "edgelength" || cost_policy_lc == "edge-length" )
	    {
		  SMS::Edge_length_cost <cgal::polyhedron_surface_mesh> cost;
	      
		  if (placement_policy_lc == "lindstromturk" || placement_policy_lc == "lindstrom-turk" )
		  {
			SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh> placement;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }	      
		  else if (placement_policy_lc == "midpoint" || placement_policy_lc == "mid-point")
		  {
			SMS::Midpoint_placement <cgal::polyhedron_surface_mesh> placement;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }
		  else
		  {
			void* placement = NULL;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }
	    }   
	    
	    else
	    {
		  void* cost = NULL;
	      
		  if (placement_policy_lc == "lindstromturk" || placement_policy_lc == "lindstrom-turk" )
		  {
			SMS::LindstromTurk_placement<cgal::polyhedron_surface_mesh> placement;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }	      
		  else if (placement_policy_lc == "midpoint" || placement_policy_lc == "mid-point")
		  {
			SMS::Midpoint_placement <cgal::polyhedron_surface_mesh> placement;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
		  }
		  else
		  {
			void* placement = NULL;
			removed_edges = smart_edge_collapse (my_mesh, stop, cost, placement);
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
