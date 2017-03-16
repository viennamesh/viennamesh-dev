#include "pragmatic_refine_multi_region.hpp"

//standard includes
#include <vector>
#include <string>

//Pragmatic includes
#include "Refine.h"
#include "pragmatic_mesh.hpp"

/*
#include "Mesh.h"
#include "MetricField.h"
#include "ticker.h"
#include "VTKTools.h"
*/

#include "viennagrid/viennagrid.hpp"

#ifdef HAVE_OPENMP
#include <omp.h>
#endif

namespace viennamesh
{
		pragmatic_refine_multi_region::pragmatic_refine_multi_region()	{}
		std::string pragmatic_refine_multi_region::name() {return "pragmatic_refine_multi_region";}

		inline Mesh<double>* convert_multi_region (MeshType input_mesh, Mesh<double>* mesh)
		{
			std::cout << "Calling convert_multi_region (MeshType input_mesh, Mesh<double>* mesh)" << std::endl;
			return mesh;
		}

		bool pragmatic_refine_multi_region::run(viennamesh::algorithm_handle &)
		{
			  std::cout << name() << std::endl;
     
        //typedefs
        typedef MeshType                                                        mesh_type;
        typedef typename mesh_type::region_id_type                              region_id_type;

        typedef typename viennagrid::result_of::region_range<mesh_type>::type   RegionRangeType;
        typedef typename viennagrid::result_of::point<mesh_type>::type          PointType;

        typedef typename mesh_type::const_region_type                           RegionType;
        typedef typename  viennagrid::result_of::element<mesh_type>::type       ElementType;

        typedef typename  viennagrid::result_of::id<ElementType>::type          VertexIDType;

        typedef viennagrid::result_of::cell_range<RegionType>::type             RangeType;
        typedef viennagrid::result_of::vertex_range<RegionType>::type           VertexRange;

        //input handles
        mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

        //get number of refinement passes
        data_handle<int> refinement_passes = get_input<int>("refinement_passes");

        //get dimension of cells and geometric dimension
			  int cell_dimension = viennagrid::cell_dimension( input_mesh() );
			  int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );

        if (cell_dimension !=3 )
        {
          std::cerr << "Plugin works only with 3D elements!" << std::endl;
          //return false;
        }

        //vector storing the pragmatic meshes
        std::vector<Mesh<double>*> pragmatic_meshes;

        //get the number of regions of the input mesh
		  	viennagrid_region_id * region_ids_begin;
		  	viennagrid_region_id * region_ids_end;

		  	viennagrid_mesh_regions_get(input_mesh().internal(), &region_ids_begin, &region_ids_end);

			  int no_of_regions = (*(region_ids_end-1)+1);

        //vectors storing the coordinate
        std::vector< std::vector<double>> x_coords(no_of_regions);
        std::vector< std::vector<double>> y_coords(no_of_regions);
        std::vector< std::vector<double>> z_coords(no_of_regions);

        //vector storing the ENLists of each region
        std::vector<std::vector<index_t>> ENLists_regions(no_of_regions);

        //vector storing the viennagrid vertices indices
        std::vector<std::vector<VertexIDType>> viennagrid_vertices_all_regions(no_of_regions);

        //Iterate over all regions and create individual pragmatic data structures
        viennagrid::mesh mesh = input_mesh();

        RegionRangeType regions(mesh);
      
        for(typename RegionRangeType::iterator rit = regions.begin(); rit != regions.end(); ++rit)
        {
          //RegionType region = input_mesh().get_region( (*rit).id() );
          RegionType region = *rit;

          //get number of points and cells in the current region
          int num_points = viennagrid::elements(region,0).size();
          //TODO: change for 3D refinement
          int num_cells = viennagrid::elements(region,2).size();

          //prepare points
          std::map<ElementType, VertexIDType>  vertex_to_index_map;
          std::map<VertexIDType, ElementType>  used_vertex_map;

          //iterate over all cells in the current region
          RangeType range(region);
          
          for (RangeType::iterator it = range.begin(); it != range.end(); ++it)
          {
            VertexRange vertices(region);
            for (VertexRange::iterator vit = vertices.begin(); vit != vertices.end(); ++vit)
            {           
              typename std::map< VertexIDType, ElementType >::iterator kt = used_vertex_map.find( (*vit).id() );
              if (kt == used_vertex_map.end())
                {
                  used_vertex_map.insert( std::make_pair((*vit).id(), *vit) ); 
                }
            }
          }

          VertexIDType index = 0;
          for (typename std::map< VertexIDType, ElementType >::iterator it = used_vertex_map.begin(); it != used_vertex_map.end(); ++it)
          {
            vertex_to_index_map.insert(std::make_pair(it->second, index++) );
          } // end prepare points

          //FOR DEBUGGING ONLY
          ofstream map_output;
          map_output.open("map_output.txt", std::ofstream::out | std::ofstream::app);
          map_output << "USED_VERTEX_MAP" << std::endl;
          for (std::map< VertexIDType, ElementType >::iterator it = used_vertex_map.begin(); it != used_vertex_map.end(); ++it)          
          {
            map_output << "First: " << it->first << ", Second: " << it->second << std::endl;  
          }
          map_output << std::endl;
          map_output << "VERTEX_TO_INDEX_MAP" << std::endl;
          for (std::map< ElementType, VertexIDType>::iterator it = vertex_to_index_map.begin(); it != vertex_to_index_map.end(); ++it)          
          {
            map_output << "First: " << it->first << ", Second: " << it->second << std::endl;  
          }
          map_output << std::endl;
          //END OF FOR DEBUGGING ONLY

          //prepare cells
          std::set<ElementType> used_cells_map;
          for(RangeType::iterator it  = range.begin(); it != range.end(); ++it)
          {
            used_cells_map.insert(*it);
          } //end prepare cells

          //FOR DEBUGGING ONLY
          map_output << "USED_CELLS_SET" << std::endl;
          for (auto it : used_cells_map)          
          {
            map_output << it << std::endl;  
          }
          map_output << std::endl;  
          map_output.close();        
          //END OF FOR DEBUGGING ONLY

          //write points
          for (typename std::map< VertexIDType, ElementType >::iterator it = used_vertex_map.begin(); it != used_vertex_map.end(); ++it)
          {
            PointType point = viennagrid::get_point(input_mesh().get_region(region.id()), it->second);
            x_coords[region.id()].push_back(point[0]);
            y_coords[region.id()].push_back(point[1]);
            //TODO: change for 3D refinement
            //z_coords[region.id()].push_back(point[2]);
          }  
          //end write points

          //write vertex indices in viennagrid orientation to array
          for (typename std::set< ElementType >::iterator it = used_cells_map.begin(); it != used_cells_map.end(); ++it)
          {
              typedef typename viennagrid::result_of::const_element_range<ElementType>::type    VertexOnCellRange;
              typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
              VertexOnCellRange vertices_on_cell(*it, 0);

              size_t j = 0;

              for (VertexOnCellIterator vocit = vertices_on_cell.begin(); vocit != vertices_on_cell.end(); ++vocit, ++j)
              {
                viennagrid_vertices_all_regions[region.id()].push_back( vertex_to_index_map[*vocit]);
              }

              for (std::size_t i=0; i < viennagrid_vertices_all_regions[region.id()].size(); ++i)
              {
                ENLists_regions[region.id()].push_back( viennagrid_vertices_all_regions[region.id()].at(i).index() );
              }

              viennagrid_vertices_all_regions[region.id()].clear();
    
          }       
          //end write vertex indices in viennagrid orientation to array
               
          //create pragmatic mesh 
          Mesh<double> *mesh = nullptr;
          //TODO: change for 3D refinement
          //mesh = new Mesh<double> ( num_points, num_cells, &(ENLists_regions[region.id()][0]) ,&(x_coords[region.id()][0]), &(y_coords[region.id()][0]), &(z_coords[region.id()][0]) );
          mesh = new Mesh<double> ( num_points, num_cells, &(ENLists_regions[region.id()][0]) ,&(x_coords[region.id()][0]), &(y_coords[region.id()][0]) );
          mesh->create_boundary();

          MetricField<double,2> metric_field(*mesh);

          std::vector<double> psi(num_points);
          for(size_t i=0; i<num_points; i++)
          {
            psi[i] = pow(mesh->get_coords(i)[0], 4) + pow(mesh->get_coords(i)[1], 4) + pow(mesh->get_coords(i)[2], 4);
            //TODO: change for 3D refinement
            //psi[i] = pow(mesh->get_coords(i)[0], 4) + pow(mesh->get_coords(i)[1], 4) + pow(mesh->get_coords(i)[2], 4);
          }
          
          metric_field.add_field(&(psi[0]), 0.001);
          metric_field.update_mesh();

          pragmatic_meshes.push_back(mesh);
        }

        for (size_t i =0; i < pragmatic_meshes.size(); ++i)
        {
          std::cout << pragmatic_meshes[i]->get_number_nodes() << " " << pragmatic_meshes[i]->get_number_elements() << std::endl;
        }

        //refinement test
        for (size_t i =0; i < pragmatic_meshes.size(); ++i)   
        {      
          Refine<double,2> refine(*pragmatic_meshes[i]);  

          for(int i=0; i<3; i++)
          {      
            //TODO: change for 3D refinement     
            //refine.refine(sqrt(2.0));  
          }

          pragmatic_meshes[i]->defragment();
          
          //test output
          std::cout << pragmatic_meshes[i]->get_number_nodes() << " " << pragmatic_meshes[i]->get_number_elements() << std::endl;
          
          std::string filename;
          filename += "examples/data/pragmatic_mesh_refined_";
          filename += std::to_string(i);
  
          VTKTools<double>::export_vtu(filename.c_str(), pragmatic_meshes[i]);
        }     

        //test implementation of a pragmatic mesh merger
        //
        // create one big pragmatic data structure from the sub-partitions and convert it back into viennagrid-datastructure
        // OR
        // write directly into an XML file

        export_to_viennagrid_vtu(pragmatic_meshes); //exports the pragmatic meshes into vtu-files readable by ViennaMesh

        //return output
        //
        //implement an output mesh handle to continue with the pragmatic data structures

			  return true;
		} //end run()
}
