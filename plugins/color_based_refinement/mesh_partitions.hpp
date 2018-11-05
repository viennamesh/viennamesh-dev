#ifndef MESH_PARTITIONS_HPP
#define MESH_PARTITIONS_HPP

//pragmatic basic includes for data structure
//TODO: implement own data structure
#include "Mesh.h"
#include "MetricField.h"
#include "Refine.h"
#include "ElementProperty.h"
#include "Edge.h"
#include "Swapping.h"
#include "Refine_cavity.h"

//TODO: DEBUG
#include "VTKTools.h"
//END OF DEBUG

//viennamesh includes
#include "viennameshpp/plugin.hpp"

//all other includes
#include "metis.h"
#include "mtmetis.h"

#include <unordered_map>
#include <map>
#include <numeric>  
#include <chrono>
#include <boost/container/flat_map.hpp>

#include "outbox.hpp"

#ifdef HAVE_OPENMP
    #include <omp.h>
#endif

extern "C"
{
  #include "triangle_interface.h"
}

#include "tetgen.h"
#include <limits>

#ifndef MAX_THREADS
#define MAX_THREADS 8
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                Declaration                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------------//

//class MeshPartitions
//
//This class partitions an input mesh and stores its partitions in different data structurs for further processing
class MeshPartitions
{
    public:
        MeshPartitions(Mesh<double> * original_mesh, int num_regions,
         std::string filename, int thread, std::string algo);                                 //Constructor 
        //MeshPartitions(std::unique_ptr<Mesh<double>> original_mesh, int num_regions, std::string filename, int thread);
        ~MeshPartitions();                                                                    //Destructor

        //void convert_pra_to_tri(Mesh<double>* partition, struct triangulateio& tri_mesh);
        //void convert_tri_to_pra();

        //REPLACE THESE TWO VECTORS WITH A TEMPLATE VECTOR!!!
        std::vector<Mesh<double>*> pragmatic_partitions;                                      //Vector containing pointers to the pragmatic partitions
        std::vector<triangulateio> triangle_partitions;                                       //Vector containing the triangle data structures
        std::vector<tetgenio> tetgen_partitions;                                              //Vector containing the tetgen data structures

        bool MetisPartitioning();                                                             //Partition mesh using metis
        bool CreatePragmaticDataStructures_ser();                                             //Create Pragmatic Meshes storing the mesh partitions in serial
        /*bool CreatePragmaticDataStructures_par(std::vector<double>& threads_log, std::vector<double>& refine_times, std::vector<double>& l2g_build, 
                                               std::vector<double>& l2g_access, std::vector<double>& g2l_build, std::vector<double>& g2l_access,
                                               std::string algorithm, std::string options, std::vector<double>& triangulate_log,
                                               std::vector<double>& ref_detail_log);//, std::vector<double>& build_tri_ds);   //Create Pragmatic Meshes storing the mesh partitions in parallel*/
        bool CreatePragmaticDataStructures_par(std::string algorithm, std::vector<double>& threads_log, 
                                               std::vector<double>& heal_log, std::vector<double>& metric_log,
                                               std::vector<double>& call_refine_log, std::vector<double>& refine_log,
                                               std::vector<double>& mesh_log, double & for_time, double & prep_time,
                                               std::vector<double>& nodes_log, std::vector<double>& enlist_log,
                                               std::string options, std::vector<size_t>& workload,
                                               std::vector<size_t>& workload_elements, const int max_num_iterations,
                                               std::vector<double>& get_interfaces_log, std::vector<double>& defrag_log,
                                               std::vector<double>& refine_boundary_log);
        bool CreateNeighborhoodInformation(const int max_iterations);                         //Create neighborhood information for vertices and partitions
        bool ColorPartitions(std::string coloring_algorithm, std::string filename,            //Color the partitions
                             int no_of_iterations = 1);      
        bool CheckPartitionColoring();                                                        //Checks validity of the partition coloring
        bool WritePartitions();                                                               //ONLY FOR DEBUGGING!
        bool RefineInterior();                                                                //Refinement without refining boundary elements
        bool WriteMergedMesh(std::string filename);                                           //Merges partitions into a single mesh and writes it
        bool RefinementKernel(int part, double L_max);
        void GetMeshStats();

        int get_colors(){return colors;};
        int get_max(){return max;};
        std::vector<std::vector<int>>& get_color_partitions() {return color_partitions;};
        void GetRefinementStats(int* nodes, int* elements, std::string algorithm);

        //mesh healing functions
        void heal2D_1(Mesh<double>*& partition, const index_t *newVertex, int eid, int nloc, int& splitCnt, int& threadIdx);
        void heal2D_2(Mesh<double>*& partition, const index_t *newVertex, int eid, int nloc, int& splitCnt, int& threadIdx);
        void heal_facet(Mesh<double>*& partition, int nedge, std::vector<int>& new_vertices_per_element, int eid, const index_t *facet);
        void heal3D_1(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges);
        void heal3D_2(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges);
        void heal3D_3(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges, int msize, int dim, ElementProperty<double>*& property);
        void heal3D_4(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges, int msize, int dim, ElementProperty<double>*& property);
        void heal3D_5(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges, int msize, int dim, ElementProperty<double>*& property);
        void refine_wedge(Mesh<double>*& partition, const index_t top_triangle[], const index_t bottom_triangle[], const int bndr[], 
                          DirectedEdge<index_t>* third_diag, int eid, int& threadIdx, int& splitCnt, int nloc, int msize, int dim,
                          ElementProperty<double>*& property);

        bool ConsistencyCheck();                                                                //Check the consistency of the mesh

    private:
/*
        bool MetisPartitioning(Mesh<double>* original_mesh, int num_regions);                 //Partition mesh using metis
        bool CreatePragmaticDataStructures_ser(Mesh<double>* original_mesh, int num_regions); //Create Pragmatic Meshes storing the mesh partitions in serial
        bool CreatePragmaticDataStructures_par(Mesh<double>* original_mesh, int num_regions); //Create Pragmatic Meshes storing the mesh partitions in parallel
        bool CreateNeighborhoodInformation(Mesh<double>* original_mesh, int num_regions);     //Create neighborhood information for vertices and partitions
        bool ColorPartitions(int num_regions);                                                               //Color the partitions
        bool WritePartitions();                                                               //ONLY FOR DEBUGGING!
*/

        template<typename _real_t, int _dim> friend class Refine;

        Mesh<double>* original_mesh;
        int num_regions;
        int nthreads;
        std::string file;

        //Variables for Metis   
        std::vector<idx_t> eptr;
        std::vector<idx_t> eind;
        idx_t num_nodes;
        idx_t num_elements;
        idx_t ncommon;
        //idx_t num_parts;
        idx_t result;
        std::vector<idx_t> epart;
        std::vector<idx_t> npart;

        //Variables used for Pragmatic data structures
        std::vector<std::set<index_t>> nodes_per_partition;
        std::vector<index_t> _ENList;
        std::vector<std::set<index_t>> elements_per_partition;

        //index mappings for the partitions
        std::vector<std::unordered_map<index_t, index_t>> g2l_vertex;
        std::vector<std::vector<int>> l2g_vertex;
        std::vector<std::unordered_map<index_t, index_t>> g2l_element;
        std::vector<std::vector<int>> l2g_element;

        //Neighborhood Information containers
        std::vector<std::set<int>> nodes_partition_ids;
        std::vector<std::set<int>> partition_adjcy;                                           //Stores the IDs of all neighboring partitions

        std::vector<std::vector<int>> interfaces;                                             //Stores the partition IDs of the neighor which share interface i

        //std::vector<std::vector<int>> NNInterfaces;                                           //Stores for each partition and each Node-Node pair if the edges is an interface
        std::vector<std::vector<std::vector<int>>> NNInterfaces;

        std::set<int>& get_nodes_partition_ids(int n){return nodes_partition_ids[n];};

        //Color information
        size_t colors;                                                                        //Stores the number of colors used
        std::vector<int> partition_colors;                                                    //Contains the color assigned to each partition
        std::vector<std::vector<int>> color_partitions;                                       //Contains the partition ids assigned to each color

        std::vector<int> vertex_colors;
        std::vector<std::vector<int>> color_vertices;
        std::vector<int> num_color_vertices;

        std::vector<int> previous_nelements;

        double calc_edge_length(int part_id, index_t x0, index_t y0);
        double calculate_quality(const index_t* n, int part_id);
        double update_quality(index_t element, int part_id);

        int edgeNumber(Mesh<double>*& partition, index_t eid, index_t v1, index_t v2);

        //Outboxes
        std::vector<Outbox> outboxes;

        //global NNodes
        int global_NNodes;

        std::string algorithm;

        //ElementProperty<double> *property;

        //DEBUG
        int max=0;
        //END OF DEBUG

}; //end of class MeshPartitions

//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                Helper Functions                                                              //
//----------------------------------------------------------------------------------------------------------------------------------------------//

void convert_pra_to_tri(Mesh<double>* partition, triangulateio& tri_mesh)
{
    std::cout << "convert pragmatic to triangle data structure" << std::endl;

    //pointlist   
  /*  if (tri_mesh.pointlist) 
    {
        free(tri_mesh.pointlist);
    }*/
    tri_mesh.numberofpoints = partition->get_number_nodes();
    tri_mesh.pointlist = (REAL*)malloc( sizeof(REAL) * 2 * tri_mesh.numberofpoints);
    tri_mesh.numberofpointattributes = 0;

    std::cout << "point info done " << tri_mesh.numberofpoints << " " << partition->get_number_nodes() << " " << sizeof(tri_mesh.pointlist)  << std::endl;
/*
    if (out_mesh.pointlist) free(out_mesh.pointlist);
    out_mesh.pointlist = (REAL*)malloc( sizeof(REAL) * 2 * partition->get_number_nodes());
    out_mesh.numberofpoints = partition->get_number_nodes();
*/

    //std::vector<int> pragmatic_indices_to_triangle_indices(partition->get_number_nodes());

    int index = 0;
    for (size_t i = 0; i < partition->get_number_nodes(); ++i, ++index)
    {
        tri_mesh.pointlist[2*i] = partition->get_coords(i)[0];
        tri_mesh.pointlist[2*i+1] = partition->get_coords(i)[1];

        //pragmatic_indices_to_triangle_indices[i] = index;
    }

    std::cout << "points done " << std::endl;
    
    //trianglelist
    //if (tri_mesh.trianglelist) free(tri_mesh.trianglelist);
    tri_mesh.numberoftriangles = partition->get_number_elements();
    tri_mesh.trianglelist = (int*)malloc( sizeof(int) * 3 * tri_mesh.numberoftriangles );

    std::cout << "triangle info done " << tri_mesh.numberoftriangles << " " << partition->get_number_elements() << " " << sizeof(tri_mesh.trianglelist) << std::endl;

/*
    if (out_mesh.trianglelist) free(out_mesh.trianglelist);
    out_mesh.trianglelist = (int*)malloc( sizeof(int) * 3 * partition->get_number_elements() );
    out_mesh.numberoftriangles = partition->get_number_elements();
*/
    for (size_t i = 0; i < partition->get_number_elements(); ++i)
    {
        //std::cout << "  " << i << std::endl;
        const int *element_ptr = nullptr;
        element_ptr = partition->get_element(i);
       // std::cout << *(element_ptr++) << " " << *(element_ptr++) << " " << *(element_ptr++) << std::endl;
        
        tri_mesh.trianglelist[3*i] = *(element_ptr++);
        tri_mesh.trianglelist[3*i+1] = *(element_ptr++);
        tri_mesh.trianglelist[3*i+2] = *(element_ptr++);
    }

    std::cout << "triangles done" << std::endl;

    tri_mesh.numberofcorners = 3;
    tri_mesh.numberoftriangleattributes = 0;

    std::cout << "conversion done" << std::endl;

//end of create triangle mesh
} //end of convert_pra_to_tri*/
/*
void convert_tri_to_pra(triangulateio& tri_mesh, Mesh<double>* partition)
{
    std::cout << "convert triangle to pragmatic data structure" << std::endl;
} //end of convert_tri_to_pra*/

// Struct used for sorting vertices by their coordinates. It's
// meant to be used by the 1:8 wedge refinement code to enforce
// consistent order of floating point arithmetic across MPI processes.
struct Coords_t 
{
    double coords[3];

    Coords_t(const double *x)
    {
        coords[0] = x[0];
        coords[1] = x[1];
        coords[2] = x[2];
    }

    /// Less-than operator
    bool operator<(const Coords_t& in) const
    {
        bool isLess;

        for(int i=0; i<3; ++i) 
        {
            if(coords[i] < in.coords[i]) 
            {
                isLess=true;
                break;
            } 
            
            else if(coords[i] > in.coords[i]) 
            {
                isLess = false;
                break;
            }
        }

        return isLess;
    }
}; //end of struct Coords_t

//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                Implementation                                                                //
//----------------------------------------------------------------------------------------------------------------------------------------------//

//Constructor
//
//Tasks: TODO
//MeshPartitions::MeshPartitions(Mesh<double>* original_mesh, int num_regions, std::string filename)
MeshPartitions::MeshPartitions(Mesh<double> * orig_mesh, int nregions, std::string filename, int threads, std::string algo)
{   
    original_mesh = orig_mesh;
    num_regions = nregions;
    nthreads = threads;
    file = filename;
    global_NNodes = orig_mesh->get_number_nodes();
    algorithm = algo;
} //end of Constructor

//Destructor
//
//Tasks: TODO
MeshPartitions::~MeshPartitions()
{/*
    if (algorithm == "tetgen")
    {
        for (size_t i = 0; i < tetgen_partitions.size(); ++i)
        {
            std::cerr << "deleted mesh at " << pragmatic_partitions[i] << std::endl;
            tetgen_partitions[i].deinitialize();
        }
    }//*/

    //if (algorithm == "pragmatic")
    {
         //free used memory
        for (size_t i = 0; i < pragmatic_partitions.size(); ++i)
        {
            //std::cerr << "deleted mesh at " << pragmatic_partitions[i] << std::endl;
            delete pragmatic_partitions[i];
        }

        //delete merged_output;
        //std::cout << "freed memory" << std::endl;
    }
  
    viennamesh::info(5) << "Called Destructor of MeshPartitions" << std::endl;

} //end of Destructor

//GetMeshStats()
//
//Tasks: Returns various statistics about the mesh (e.g. number of vertices, number of cells, maximum number of neighboring vertices or cells, etc.)
void MeshPartitions::GetMeshStats()
{
    viennamesh::info(1) << "Getting mesh statistics" << std::endl << std::endl;
    viennamesh::info(1) << "  Number of Vertices: " << original_mesh->get_number_nodes() << std::endl;
    viennamesh::info(1) << "  Number of Elements: " << original_mesh->get_number_elements() << std::endl;

    //find maximum and average vertex degree (maximum number of neighbors of vertex in the mesh)
    size_t max_vert_degree = 0;
    std::vector<int> vert_degrees(original_mesh->get_number_nodes());
    for (size_t i = 0; i < original_mesh->get_number_nodes(); ++i)
    {
        if ( original_mesh->NNList[i].size() > max_vert_degree )
            max_vert_degree = original_mesh->NNList[i].size();

            vert_degrees[i] = original_mesh->NNList[i].size();
    }

    viennamesh::info(1) << "  Maximum Vertex Degree: " << max_vert_degree << std::endl;

    //compute average vertex degree
    double avg_vert_degree = (double) std::accumulate(vert_degrees.begin(), vert_degrees.end(), 0) / (double) original_mesh->get_number_nodes();

    viennamesh::info(1) << "  Average Vertex Degree: " << avg_vert_degree << std::endl;

    //find maximum and average element degree (maximum number of neighbors of element in the mesh)
    //a neighboring element is an element connected via a vertex
    size_t max_ele_degree = 0;
    std::vector<int> ele_degrees( original_mesh->get_number_elements() );

    viennamesh::info(1) << "" << std::endl;
}
//end of GetMeshStats()

//MetisPartitioning
//
//Tasks: Partitions the input mesh (in pragmatic data structure) into the specified number of partitions
//bool MeshPartitions::MetisPartitioning(Mesh<double>* const mesh, int num_regions)
bool MeshPartitions::MetisPartitioning()
{
    //get basic mesh information
    num_elements = original_mesh->get_number_elements();
    num_nodes = original_mesh->get_number_nodes();
    ncommon = original_mesh->get_number_dimensions();
   // std::vector<idx_t> bdry = original_mesh->copy_boundary_vector();
   // size_t num_bdry_nodes = std::accumulate(bdry.begin(), bdry.end(), 0);
   // idx_t numflag = 0;     //0...C-style numbering is assumed that starts from 0; 1...Fortran-style numbering is assumed that starts from 1

    //reserve memory
    epart.reserve(num_elements);
    npart.reserve(num_nodes);

 /*   std::vector<idx_t> xadj;
    xadj.resize(num_nodes+1);
    std::vector<idx_t> adjncy;
    adjncy.resize(2*(3*num_nodes - 3 - num_bdry_nodes)); //see: http://math.stackexchange.com/questions/1541125/total-number-of-edges-in-a-triangle-mesh-with-n-vertices
*//*
    int num_edges = 2*(3*num_nodes - 3 - num_bdry_nodes);
    idx_t *xadj;
    xadj = new idx_t[num_nodes+1];
    idx_t *adjncy;
    adjncy= new idx_t[num_edges];

 //   std::cout << adjncy.size() << std::endl;
*/
    //fill eptr and eind, as done in viennamesh plugin "metis", file "mesh_partitionig.cpp"  
    eptr.push_back(0);
    
    for (size_t i = 0; i < original_mesh->get_number_elements(); ++i)
    {
        const index_t* element_ptr = nullptr;
        element_ptr = original_mesh->get_element(i);

        for (size_t j = 0; j < (original_mesh->get_number_dimensions() + 1); ++j)
        {
            eind.push_back( *(element_ptr+j) );
        }               

        eptr.push_back( eind.size() );    
    }  

   /* //DEBUG
    ofstream outfile;
    outfile.open("box300x300.metis");

    std::vector<int> _ENList_dbg = mesh->get_enlist();
    
    outfile << mesh->get_number_elements() << std::endl;

    for (size_t i = 0; i < mesh->get_number_elements(); ++i)
    {
      outfile << _ENList_dbg[3*i]+1 << " " << _ENList_dbg[3*i+1]+1 << " " << _ENList_dbg[3*i+2]+1 << std::endl;
    }
    outfile.close();
    //END OF DEBUG*/

    //Call Metis Partitioning Function (see metis manual for details on the parameters and on the use of the metis API)
    //*/
   /* idx_t options[METIS_NOPTIONS];

    METIS_SetDefaultOptions(options);
    options[METIS_OPTION_PTYPE]=METIS_PTYPE_RB;
//*/
    METIS_PartMeshDual  (&num_elements,
                         &num_nodes,
                         eptr.data(),
                         eind.data(),
                         NULL,
                         NULL,
                         &ncommon,
                         &num_regions,
                         NULL,
                         //options,
                         NULL,    //NULL if no options used
                         &result,
                         epart.data(),
                         npart.data());

    viennamesh::info(5) << "Created " << num_regions << " mesh partitions using METIS_PartMeshDual" << std::endl;
                         //*/

        /*
   METIS_PartMeshNodal (&num_elements,
                        &num_nodes,
                        eptr.data(),
                        eind.data(),
                        NULL,
                        NULL,
                        &num_regions,
                        NULL,
                        NULL,
                        &result,
                        epart.data(),
                        npart.data());

    viennamesh::info(5) << "Created " << num_regions << " mesh partitions using METIS_PartMeshNodal" << std::endl;
                        //*/
/*
    idx_t *xadj=NULL, *adjncy=NULL;//, *nptr=NULL, *nind=NULL;
    idx_t pnumflag=0;
    double * options = mtmetis_init_options();
    
    options[MTMETIS_OPTION_NTHREADS] = nthreads;
    options[MTMETIS_OPTION_VERBOSITY] = 0;

    const uint32_t ncon = 1;

    //get the dual graph
    METIS_MeshToDual(&num_elements, &num_nodes, eptr.data(), eind.data(), &ncommon, &pnumflag, &xadj, &adjncy);

    const unsigned int* xadj_arr = const_cast<unsigned int*>( (unsigned int*) xadj);
    const unsigned int* adjncy_arr = const_cast<unsigned int*>( (unsigned int*) adjncy);

    const unsigned int nregions = num_regions;

    epart.resize(num_elements);

    unsigned int where[num_elements];

    viennamesh::info(5) << "  Partitioning with MTMETIS_PartGraphKway" << std::endl;

    MTMETIS_PartGraphKway(//const_cast<unsigned int*>( (unsigned int*) &ne), 
                          const_cast<unsigned int*>( (unsigned int*) &num_elements),
                          //const_cast<unsigned int*>( (unsigned int*) &ncon), 
                          &ncon,
                          xadj_arr, 
                          adjncy_arr, 
                          //NULL,
                          //NULL,
                          NULL, 
                          NULL, 
                          NULL, 
                          &nregions, 
                          //NULL,
                          NULL, 
                          NULL, 
                          options, 
                          //NULL,
                          //NULL,
                          //NULL);
                          &result, 
                          //where);
                          const_cast<unsigned int*>( (unsigned int*) epart.data()) );

    free (xadj);
    free (adjncy);
    free (options);

    viennamesh::info(5) << "Created " << num_regions << " mesh partitions using mt-Metis" << std::endl;

    //*/
/*
    viennamesh::info(5) << "  Partitioning with MTMETIS_PartGraphRecursive" << std::endl;
    
    MTMETIS_PartGraphRecursive(//const_cast<unsigned int*>( (unsigned int*) &ne), 
                            const_cast<unsigned int*>( (unsigned int*) &num_elements),
                            //const_cast<unsigned int*>( (unsigned int*) &ncon), 
                            &ncon,
                            xadj_arr, 
                            adjncy_arr, 
                            //NULL,
                            //NULL,
                            NULL, 
                            NULL, 
                            NULL, 
                            &nregions, 
                            //NULL,
                            NULL, 
                            NULL, 
                            options, 
                            //NULL,
                            //NULL,
                            //NULL);
                            &result, 
                            //where);
                            const_cast<unsigned int*>( (unsigned int*) epart.data()) );

    free (xadj);
    free (adjncy);
    free (options);

    viennamesh::info(5) << "Created " << num_regions << " mesh partitions using mt-Metis" << std::endl;
*//*
std::cout << "epart-size: " << epart.size() << std::endl;
    //DEBUG
    for (size_t i = 0; i < epart.size(); ++i)
    {
        std::cout << i << ": " << epart[i] << std::endl;
    }
/*
    exit(0);
    //END OF DEBUG*/

    /*//DEBUG
    ofstream epart_stream;
    epart_stream.open("epart.8");
  
    for (size_t i = 0; i < num_elements; ++i)
    {
      epart_stream << epart[i] << std::endl;
    }
    //epart_stream.close();
    //END OF DEBUG*/
/*
    //DEBUG
    std::cout << "xadj: " << std::endl;
    for (size_t i = 0; i < num_nodes+1; ++i)
        std::cout << " " << i << ": " << xadj[i] << std::endl;

    std::cout << "adjncy: " << std::endl;
    for (size_t i = 0; i < num_edges; ++i)
        std::cout << " " << i << ": " << adjncy[i] << std::endl;
    //END OF DEBUG*/

    return true;
}//end of MetisPartitioning

//CreateNeighborhoodInformation
//
//Tasks: Populate Vertex partition container and create adjacency lists for each partition
//bool MeshPartitions::CreateNeighborhoodInformation(Mesh<double>* original_mesh, int num_regions)
bool MeshPartitions::CreateNeighborhoodInformation(const int max_iterations)
{  
  //prepare a partition id container for vertices and partitions
  //this will be populated with all partitions a vertex is part of
  //nodes_partition_ids.resize(num_nodes);
  int nedge;
  if  (original_mesh->get_number_dimensions() == 2)
  {
    nedge = 3;
  }

  else
  {
    nedge = 6;      
  }

  int resizer = num_nodes * std::pow(nedge*2, max_iterations);
  resizer = num_nodes + num_nodes*nedge*1.5;
  std::cout << "resizing nodes_partition_ids to " << resizer << std::endl;

  nodes_partition_ids.resize(resizer);

  partition_adjcy.resize(num_regions);

  //populate the container
  for(size_t i = 0; i < num_elements; ++i)
  {
    //get vertices of element
    const index_t *element_ptr = nullptr;
    element_ptr = original_mesh->get_element(i);

    size_t ndims = original_mesh->get_number_dimensions();

    //iterate element vertices and add partition id
    for (size_t j = 0; j < (ndims+1); ++j)
    {
      nodes_partition_ids[*(element_ptr++)].insert(epart[i]);
    }
/*
    //DEBUG
    if (epart[i] > max)
      max = epart[i];
    //END OF DEBUG*/

    }
/*
    //DEBUG
    std::string global_nodes_ids_name = "debug_output/nodes_partition_ids.txt";

    std::ofstream global_nodes_ids_file;
    global_nodes_ids_file.open(global_nodes_ids_name.c_str());

    for (size_t nodes_partition_ids_iter = 0; nodes_partition_ids_iter < num_nodes; ++nodes_partition_ids_iter)
    {
        global_nodes_ids_file << "Vertex " << nodes_partition_ids_iter << " (global ID: " << nodes_partition_ids_iter << ") of " << num_nodes << std::endl;
        double v[original_mesh->get_number_dimensions()];
        original_mesh->get_coords(nodes_partition_ids_iter, v);
        global_nodes_ids_file << v[0] << " " << v[1] << " " << v[2] << std::endl;
        for (auto nodes_ids_iter : nodes_partition_ids[nodes_partition_ids_iter])
        {
            global_nodes_ids_file << "  " << nodes_ids_iter << std::endl;
        }
    }

    global_nodes_ids_file.close();   
    //END OF DEBUG*/

    //create partition adjacency information
    for (size_t i = 0; i < nodes_partition_ids.size(); ++i)
    {
        if (nodes_partition_ids[i].size() > 1)
        {
            for (auto set_iter : nodes_partition_ids[i])
            {
                for (auto set_iter2 : nodes_partition_ids[i])
                {
                    if (set_iter == set_iter2)
                        continue;

                    partition_adjcy[set_iter].insert(set_iter2);
                }
            }
        }
    }

    /*
    //DEBUG
    for (size_t i = 0; i < num_regions; ++i)
    {
        std::cout << "Partition " << i << " has the following neighbors: " << std::endl;

        for (auto iter : partition_adjcy[i])
        {
            std::cout << "  " << iter << std::endl;
        }
    }
    //END OF DEBUG*/  

    return true;
}
//end of CreateNeighborhoodInformation

//ColorPartitions
//
//Tasks: Color the partitions such that independent sets are created
//bool MeshPartitions::ColorPartitions(int num_regions)
bool MeshPartitions::ColorPartitions(std::string coloring_algorithm, std::string filename, int no_of_iterations)
{
    //-------------------------------------------------------------------------------------------------//
    //Greedy coloring algorithm
    //-------------------------------------------------------------------------------------------------//
    if (coloring_algorithm == "greedy" || coloring_algorithm == "greedy-sched")
    {
        if (coloring_algorithm == "greedy")
            viennamesh::info(1) << "Coloring partitions using Greedy algorithm" << std::endl;

        else  
            viennamesh::info(1) << "Obtaining initial coloring using Greedy algorithm" << std::endl;

        //resize vector
        partition_colors.resize(partition_adjcy.size());
        
        colors = 1;                 //number of used colors
        partition_colors[0] = 0;    //assign first partition color 0

        //visit every partition and assign the smallest color available (not already assigned to on of its neighbors)
        for (size_t i = 1; i < partition_colors.size(); ++i)
        {
            
            //int tmp_color = partition_colors[*(partition_adjcy[i].begin())] + 1;   //assign next color
            int tmp_color = 0; //start with smallest color 
            bool next_color = false;

            do
            {
                //check if assigned color in tmp_color is already assigned to a neighbor
                //since we assign colors to partitions in ascending ID order, check only
                //neighbors with smaller partition ID
                for (auto iter : partition_adjcy[i])
                {
                    //if chosen color is already assigned to neighbor, try next color
                    if ( i > iter && partition_colors[iter] == tmp_color) 
                    {
                        ++tmp_color;
                        next_color = true;
                        break;
                    }

                    //if chosen color is ok exit loop
                    else
                        next_color=false;
                }
            } while(next_color);

        /* for (size_t j = 1; j < partition_adjcy[i].size(); ++j)
            {
                //check if assigned color in tmp_color is already assigned to a neighbor
                if (partition_colors[partition_adjcy[i][j]] >= tmp_color)
                {
                    tmp_color = partition_colors[partition_adjcy[i][j]] + 1;
                }
            }*/

            partition_colors[i] = tmp_color;

            if ( (tmp_color + 1) > colors )
            {
                colors = tmp_color + 1;
            }
        }

        //create a vector containing the color information for each partition
        //each vector element is one color and contains the partitions with this color
        color_partitions.resize(colors);

        for (size_t i = 0; i < partition_colors.size(); ++i)
        {
            color_partitions[ partition_colors[i] ].push_back(i);
        }
/*
        //DEBUG
        //std::cout << "Number of used colors: " << colors << std::endl;
        ofstream color_file;
        std::string partition_filename = filename;
        if (coloring_algorithm == "greedy")
            partition_filename += "_partition_colors_greedy.txt";
        else
            partition_filename += "_partition_colors_ff.txt";
        color_file.open(partition_filename.c_str(), ios::app);
        color_file << "Partitions: " << partition_colors.size() << std::endl;
        color_file << "Gamma: " << partition_colors.size() / colors << std::endl;

        //std::cout << "  Partition | Color " << std::endl;
        color_file << "  Partition | Color " << std::endl;
    
        for (size_t i = 0; i < partition_colors.size(); ++i)
        {
            //std::cout << "          " << i << " | " << partition_colors[i] << std::endl;
            color_file << "          " << i << " | " << partition_colors[i] << std::endl;
        }
        color_file.close();
        //*/
/*
        std::string color_filename = filename;
        if (coloring_algorithm == "greedy")
            color_filename+="_color_partitions_greedy.txt";
        else
            color_filename+="_color_partitions_ff.txt";
        color_file.open(color_filename.c_str(), ios::app);

        //std::cout << std::endl << "      Color | #Partitions " << std::endl;
        color_file << "Partitions: " << partition_colors.size() << std::endl;
        color_file << "Gamma: " << partition_colors.size() / colors << std::endl;
        color_file << "      Color | #Partitions " << std::endl;

        for (size_t i = 0; i < color_partitions.size(); ++i)
        {
            //std::cout << "          " << i << " | " << color_partitions[i].size() << std::endl;
            color_file << "          " << i << " | " << color_partitions[i].size() << std::endl;

    /*     std::cout << "          " << i << " | ";
            for (auto it : color_partitions[i])
            {
            std::cout << it << " ";
            }
            std::cout << std::endl;//*/
 /*       }
        color_file.close();
        //END OF DEBUG*/

    }
    //-------------------------------------------------------------------------------------------------//
    //end of Greedy Coloring algorithm
    //-------------------------------------------------------------------------------------------------//

    //-------------------------------------------------------------------------------------------------//
    //Greddy Coloring with balancing using least used color
    //-------------------------------------------------------------------------------------------------//
    else if (coloring_algorithm == "greedy-lu")
    {
        viennamesh::info(1) << "Coloring partitions using Greedy algorithm with balancing using least used color" << std::endl;
    
        //resize vector
        partition_colors.resize(partition_adjcy.size(), -1);
    
        colors = 1;                 //number of used colors
        partition_colors[0] = 0;    //assign first partition color 0      
        
        //std::cout << "0 gets new color 0" << std::endl;

        std::vector<int> color_usage;   //vector storing how many times a color has been assigned (each element is a color)
        color_usage.reserve(30);        
        color_usage.push_back(1);       //initialize the first color assigned to the first partition (color 0, assigned 1 time)
    
        //visit every partition and assign the least used color available (not already assigned to on of its neighbors)
        //if no used color is permissible, assign a new color
        for (size_t part = 1; part < partition_colors.size(); ++part)
        {
            //find set of permissible colors
            std::vector<bool> permissible_colors(colors, true);

            for (auto neighbor : partition_adjcy[part])
            {
                //since we assign colors to partitions in ascending ID order, check only
                //neighbors with smaller partition ID
                if (neighbor > part)
                {
                    continue;
                }

                permissible_colors[ partition_colors[neighbor] ] = false;
            }
    
            std::vector<int> permissible_usage = color_usage;

            //set usage of colors not allowed for assignement to the numerical limit of int
            for (size_t i = 0; i < permissible_colors.size(); ++i)
            {
                if (!permissible_colors[i])
                {
                    permissible_usage[i] = std::numeric_limits<int>::max();
                }
            }

            //check if we need to create a new color
            if ( !std::accumulate(permissible_colors.begin(), permissible_colors.end(), false) )
            {
                partition_colors[part] = colors;
                color_usage.push_back(1);
                //std::cout << part << " gets new color " << colors << std::endl;
                ++colors;
            }

            else
            {
                partition_colors[part] = std::min_element(permissible_usage.begin(), permissible_usage.end()) - permissible_usage.begin();
                //std::cout << part << " gets old color " << partition_colors[part] << std::endl;
                ++color_usage[partition_colors[part]];
            }

        } //end of for loop iterating partitions

        //create a vector containing the color information for each partition
        //each vector element is one color and contains the partitions with this color
        color_partitions.resize(colors);
        
        for (size_t i = 0; i < partition_colors.size(); ++i)
        {
            color_partitions[ partition_colors[i] ].push_back(i);
        }
/*
        //DEBUG
        //std::cout << "Number of used colors: " << colors << std::endl;
        ofstream color_file;
        std::string partition_filename = filename;
        partition_filename += "_partition_colors_greedy-lu.txt";
        color_file.open(partition_filename.c_str(), ios::app);
        color_file << "Partitions: " << partition_colors.size() << std::endl;
        color_file << "Gamma: " << partition_colors.size() / colors << std::endl;

        //std::cout << "  Partition | Color " << std::endl;
        color_file << "  Partition | Color " << std::endl;
    
        for (size_t i = 0; i < partition_colors.size(); ++i)
        {
            //std::cout << "          " << i << " | " << partition_colors[i] << std::endl;
            color_file << "          " << i << " | " << partition_colors[i] << std::endl;
        }
        color_file.close();
        //*/
/*
        std::string color_filename = filename;
        color_filename+="_color_partitions_greedy-lu.txt";
        color_file.open(color_filename.c_str(), ios::app);

        //std::cout << std::endl << "      Color | #Partitions " << std::endl;
        color_file << "Partitions: " << partition_colors.size() << std::endl;
        color_file << "Gamma: " << partition_colors.size() / colors << std::endl;
        color_file << "      Color | #Partitions " << std::endl;

        for (size_t i = 0; i < color_partitions.size(); ++i)
        {
            //std::cout << "          " << i << " | " << color_partitions[i].size() << std::endl;
            color_file << "          " << i << " | " << color_partitions[i].size() << std::endl;

    /*     std::cout << "          " << i << " | ";
            for (auto it : color_partitions[i])
            {
            std::cout << it << " ";
            }
            std::cout << std::endl;//*/
 /*       }
        color_file.close();
        //END OF DEBUG*/
    }
    //-------------------------------------------------------------------------------------------------//
    //end of Greddy Coloring with balancing using least used color
    //-------------------------------------------------------------------------------------------------//

    //-------------------------------------------------------------------------------------------------//
    //Guided Balancing using Shuffling with Scheduled Reverse Moves by Hao Lu et al.
    //-------------------------------------------------------------------------------------------------//
    if (coloring_algorithm == "greedy-sched")
    {
        viennamesh::info(1) << "Coloring partitions using a Guided Balancing strategy using shuffling with scheduled reversed moves" << std::endl;

        double gamma = num_regions / colors;
        //partition_colors.resize(partition_adjcy.size(), -1);
        //std::cout << "parts: " << num_regions << ", colors: " << colors << ", gamma (parts/colors): " << gamma << std::endl;
        //partition_colors[0] = 0;

        no_of_iterations = 3;

        for (size_t iteration = 0; iteration < no_of_iterations; ++iteration)
        {
            //std::cout << "ITERATION " << iteration+1 << "/" << no_of_iterations << std::endl;

            //create ordered set of over-full bins in increasing color order
            //and create an ordered set under-full bins in decreasing color order
            std::vector<int> overfull_colors;
            std::vector<int> underfull_colors;
/*
            overfull_colors.resize(colors);
            underfull_colors.resize(colors);
*/
            for (size_t i = 0; i < colors; ++i)
            {
                if ( color_partitions[i].size() > gamma )
                {
                    overfull_colors.push_back(i);
                }

                else if ( color_partitions[i].size() < gamma )
                {
                    underfull_colors.push_back(i);
                }
/*
                else 
                    std::cout << "ideal color " << i << ": " << color_partitions[i].size() << std::endl;*/
            }

            //std::cout << "overfull_colors: " << overfull_colors.size() << ", underfull_colors: " << underfull_colors.size() << std::endl;

            //order underfull colors in increasing size
            std::sort(underfull_colors.begin(), underfull_colors.end(), [this] (int a, int b) { return color_partitions[a].size() > color_partitions[b].size();} );
            std::reverse(underfull_colors.begin(), underfull_colors.end());
/*
            std::cout << "underfull colors" << std::endl;
            for (auto it : underfull_colors)
            {
                std::cout << it << ": " << color_partitions[it].size() << std::endl;
            }//*/

            //std::cout << "overfull colors" << std::endl;

            std::vector<std::vector<int>> moves(colors); //list containing moves from overfull to underfull bins, moves[i][j] moves partition j to color i

            //for each j E Q_o do from Algorithm 4
            for (size_t of_color = 0; of_color < overfull_colors.size(); ++of_color)
            {
                int of_color_id = overfull_colors[of_color];

                std::vector<int> surplus_parts(color_partitions[ of_color_id ].size()-gamma); //contains ids of partitions which have to be recolored

                //surplus_parts.reserve();

                //std::cout << "bin " << of_color_id << " : " << color_partitions[of_color_id].size() << " parts, surplus parts: " << color_partitions[ of_color_id ].size()-gamma << std::endl;

                //find partitions with color i, and add them to the list V'(j) from Algorithm 4
                //Select V'(j) subset of V(j) such that |V'(j)|=|V(j)| - gamma
                for (size_t j = 0; j < surplus_parts.size(); ++j)
                {
                    //if (color_partitions[of_color_id][j] != 0 )
                        surplus_parts[j] = color_partitions[ of_color_id ][j];
/*
                    else 
                        std::cout << "skip " << color_partitions[of_color_id][j] << " with color " << of_color_id << std::endl;*/
                }
    /*          size_t counter = 0;
                for (size_t j = 0; j < partition_colors.size() && counter < surplus_parts.size(); ++j)
                {
                    if ( i == partition_colors[j] )
                    {
                        surplus_parts[counter]=j;
                        ++counter;
                    }
                }
    */
               // std::cout << " surplus_parts ok " << std::endl;
                /*//DEBUG
                for (auto it : surplus_parts)
                    std::cout << " " << it << std::endl;
                //END OF DEBUG*/

                //for each k E Q_u AND V'(j) neq 0 do (Algorithm 4)
                //note that the underfull_bins are sorted in descending order of their size
                for(size_t uf_color = 0; uf_color < underfull_colors.size() && !surplus_parts.empty(); ++uf_color)
                {
                    int uf_color_id = underfull_colors[uf_color];
                    //std::cout << " underfull_color " << underfull_colors[uf_color] << " has size " << color_partitions[ underfull_colors[uf_color] ].size() << std::endl;
                    std::vector<int> movable_parts(gamma - color_partitions[ uf_color_id ].size() );

                    for (size_t part_to_move = 0; part_to_move < movable_parts.size() && !surplus_parts.empty(); ++part_to_move)
                    {
                        moves[ uf_color_id ].push_back(surplus_parts[0]);
                        surplus_parts.erase( surplus_parts.begin() );
                    }

                    //moves[ underfull_colors[uf_color] ] = movable_parts;
                }
        /*        
                for (size_t k = 0; k < underfull_colors.size(); ++k)
                {
                    //find partitions in V'(j) that can be moved to k (V'_k(j)???) such that |V'_k(j)| + |V(k)| < gamma 
                    // V'_k(j) is set of partitions from surplus_parts which are moved to underfull color k
                    std::cout << surplus_parts.size() << std::endl;

                    if (underfull_colors.size() == gamma)
                        continue;

                    std::vector<int> moves_to_k( gamma - color_partitions[ underfull_colors[k] ].size() );

                    std::cout << "  underfull bin " << k << " is color " << underfull_colors[k] << " with " << color_partitions[underfull_colors[k]].size() << " elements" << std::endl;                
                    for (size_t l = 0; l < moves_to_k.size() && surplus_parts.size() > 0; ++l)
                    {
                        moves_to_k[l]=surplus_parts[l];
                        std::cout << "    color " << k << " gets " << surplus_parts[l] << std::endl;
                        moves.insert( std::pair<int,int>(k, surplus_parts[l]) );
                        surplus_parts.erase( surplus_parts.begin() );
                    }               
                }
    //*/
            }

/*         //DEBUG
            for (size_t i = 0; i < underfull_colors.size(); ++i)
            {
                std::cout << "move to color " << underfull_colors[i] << std::endl;
                std::cout << moves[underfull_colors[i]].size() << std::endl;
                for (size_t j = 0; j < moves[underfull_colors[i]].size(); ++j)
                {
                    std::cout << " " << moves[underfull_colors[i]][j] << std::endl;
                }
            }
            //END OF DEBUG*/

            for (size_t i = 0; i < underfull_colors.size(); ++i)
            {
                int counter = 0;
                int uf_color_id = underfull_colors[i];

                #pragma omp parallel for num_threads(nthreads) schedule(dynamic)
                for (size_t part = 0; part < moves[uf_color_id].size(); ++part)
                {
                    //check if new color is permissible and if yes, assign it;
                    bool permissible = true;
                    int new_color = uf_color_id;
                    int part_id = moves[new_color][part];
                    /*if (part_id == 0)
                    {
                        //std::cout << "what" << std::endl;
                        permissible = false;
                    }*/
                    //std::cout << moves[new_color][part] << std::endl;
    /*  
                    for (size_t neigh = 0; neigh < partition_adjcy[color]; ++neigh )
                    {
                        if ( color_partitions[ partition_adjcy[color][neigh]] == color)
                        {
                            permissible = false;
                        }
                    }
    */
                    for (auto neigh_id : partition_adjcy[ part_id ])
                    {
                        //std::cout << moves[new_color][part] << " has neighbor " << part_id << std::endl;
                        //forbid re-coloring if neighbor has the same color
                        if( (partition_colors[ neigh_id ] == new_color) )
                        {
                            permissible = false;
                            continue;
                        }
/*
                        //forbid coloring if neighbor with smaller partition id is also in the queue and would get the same color 
                        auto iter = std::find(moves[new_color].begin(), moves[new_color].end(), neigh_id);
                        if (iter != moves[new_color].end() && neigh_id < part_id)
                        {
                            permissible = false;
                        }*/
                    }

                    if (permissible)
                    {
                        //update partition_colors for the changed partition_id
                        partition_colors[ part_id ] = new_color; 
                        ++counter;
                    }
/*
                    else   
                        std::cout << moves[new_color][part] << " can not be colored with " << new_color << std::endl;//*/
                }

                //std::cout << "  added " << counter << " partitions to color " << underfull_colors[i] << std::endl;
            }

            //recreate partition_colors vector<vector>
            //create a vector containing the color information for each partition
            //each vector element is one color and contains the partitions with this color
            color_partitions.clear();
            color_partitions.resize(colors);
            
            for (size_t i = 0; i < partition_colors.size(); ++i)
            {
                color_partitions[ partition_colors[i] ].push_back(i);
            }
/*
            //DEBUG
            ofstream color_file;
            std::string partition_filename = filename;
            partition_filename += "_partition_colors_greedy-sched_iteration_";
            partition_filename += std::to_string(iteration);
            partition_filename += ".txt";
            color_file.open(partition_filename.c_str(), ios::app);
            
            color_file << "Threads: " << nthreads << std::endl;
            color_file << "Partitions: " << partition_colors.size() << std::endl;
            color_file << "Gamma: " << gamma << std::endl;

            //std::cout << "  Partition | Color " << std::endl;
            color_file << "  Partition | Color " << std::endl;
        
            for (size_t i = 0; i < partition_colors.size(); ++i)
            {
                //std::cout << "          " << i << " | " << partition_colors[i] << std::endl;
                color_file << "          " << i << " | " << partition_colors[i] << std::endl;
            }
            color_file.close();
            //*/
/*
            std::string color_filename = filename;
            color_filename+="_color_partitions_greedy-sched_iteration_";
            color_filename += std::to_string(iteration);
            color_filename += ".txt";;
            color_file.open(color_filename.c_str(), ios::app);

            //std::cout << std::endl << "      Color | #Partitions " << std::endl;
            color_file << "Threads: " << nthreads << std::endl;
            color_file << "Partitions: " << partition_colors.size() << std::endl;
            color_file << "Gamma: " << gamma << std::endl;
            color_file << "      Color | #Partitions " << std::endl;

            for (size_t i = 0; i < color_partitions.size(); ++i)
            {
                //std::cout << "          " << i << " | " << color_partitions[i].size() << std::endl;
                color_file << "          " << i << " | " << color_partitions[i].size() << std::endl;
/*
            std::cout << "          " << i << " | ";
                for (auto it : color_partitions[i])
                {
                std::cout << it << " ";
                }
                std::cout << std::endl;//*/
   /*         }
            color_file.close();
            //END OF DEBUG*/


        } //end of for loop iterating no_of_iteration times over the graph doing the scheduled reverse moves
    }
    //-------------------------------------------------------------------------------------------------//
    //end of Guided Balancing using Shuffling with Scheduled Reverse Moves by Hao Lu et al.
    //-------------------------------------------------------------------------------------------------//

    //-------------------------------------------------------------------------------------------------//
    //Parallel Graph Coloring Algorithm by Rokos et al. (A Fast and Scalable Graph Coloring Algorithm
    //for Multi-core and Many-core Architectures), 2015, arxiv.org
    //-------------------------------------------------------------------------------------------------//
    if (coloring_algorithm == "parallel")
    {
        viennamesh::info(1) << "Coloring partitions using a parallel graph coloring algorithm" << std::endl;
        
        partition_colors.resize(num_regions, -1);

        //Perform tentative coloring, round 0
        //#pragma omp parallel for
        /*
        for(size_t part = 0; part < num_regions; ++part)
        {
            std::cout << "part " << part << std::endl;
            std::set<int> colors_neighs;
            
            //create set of colors assigned to adjacent partitions
            for (auto neigh : partition_adjcy[part])
            {
                std::cout << " " << neigh << " has color " << partition_colors[neigh] << std::endl;
                colors_neighs.insert(partition_colors[neigh]);
            }

            std::cout << " set size " << colors_neighs.size() << std::endl;

            s

            for (auto it : colors_neighs)
                std::cout << "  " <<  it << std::endl;

            std::cout << " " << part << ": gets color " << *(colors_neighs.rbegin()) +1 << std::endl;

            partition_colors[part] = *(colors_neighs.rbegin())+1;
        } //end of tentative coloring, round 0*/
        
        //perform tentative coloring of partitions  
        #pragma omp parallel for num_threads(nthreads) schedule(dynamic)
        for(size_t part = 0; part < num_regions; ++part)
        {
            bool next_color = false;
            int tmp_color = 0;

            do
            {
                for (auto iter : partition_adjcy[part])
                {
                    if ( part > iter && partition_colors[iter] == tmp_color)
                    {
                        ++tmp_color;
                        next_color = true;
                        break;
                    }

                    else
                        next_color = false;
                }
            } while(next_color);

            partition_colors[part] = tmp_color;
        } //end of tentative coloring

        //mark all partitions for inspection
        std::vector<int> global_worklist(num_regions);
        std::iota(global_worklist.begin(), global_worklist.end(), 0);

        int round = 1;
        
        //do color checks of all colored partitions
        while( !global_worklist.empty() )
        {
            std::vector<std::vector<int>> recolored(nthreads);

            #pragma omp parallel for num_threads(nthreads) schedule(dynamic)
            for(size_t part = 0; part < global_worklist.size(); ++part)
            {
                int part_id = global_worklist[part];

                //check if a partition has to be recolored and recolor if necessary
                for (auto neigh : partition_adjcy[ part_id ])
                {
                    if ( (partition_colors[ part_id ] == partition_colors[neigh]) && (neigh > part_id) )
                    {
                        bool next_color = false;
                        int tmp_color = 0;
            
                        do
                        {
                            for (auto iter : partition_adjcy[part_id])
                            {
                                //if ( part_id < iter && partition_colors[iter] == tmp_color)
                                if (partition_colors[iter] == tmp_color)
                                {
                                    ++tmp_color;
                                    next_color = true;
                                    break;
                                }
            
                                else
                                    next_color = false;
                            }
                        } while(next_color);
            
                        partition_colors[part_id] = tmp_color;
                        recolored[omp_get_thread_num()].push_back(part_id);
                    } //end of check color of partition
                }                
            }

            global_worklist.clear();

            //create global worklist for next round of color checks
            for (auto recolor : recolored)
            {
                for (auto part_recolor : recolor)
                {
                    global_worklist.push_back(part_recolor);
                }
            }
            ++round;
        } //end of while loop doing the color checks

        //create a vector containing the color information for each partition
        //each vector element is one color and contains the partitions with this color
        colors = *( std::max_element(partition_colors.begin(), partition_colors.end()) ) + 1;

        color_partitions.clear();
        color_partitions.resize(colors);

        for (size_t i = 0; i < partition_colors.size(); ++i)
        {
            color_partitions[ partition_colors[i] ].push_back(i);
        }

        viennamesh::info(5) << "  Finished coloring after " << round << " rounds using " << colors << " colors" << std::endl;
/*
        //DEBUG
        //std::cout << "Number of used colors: " << colors << std::endl;
        ofstream color_file;
        std::string partition_filename = filename;
        partition_filename += "_partition_colors_parallel.txt";
        color_file.open(partition_filename.c_str(), ios::app);

        color_file << "Threads: " << nthreads << std::endl;
        color_file << "Partitions: " << partition_colors.size() << std::endl;
        color_file << "Gamma: " << partition_colors.size() / colors << std::endl;

        //std::cout << "  Partition | Color " << std::endl;
        color_file << "  Partition | Color " << std::endl;
    
        for (size_t i = 0; i < partition_colors.size(); ++i)
        {
            //std::cout << "          " << i << " | " << partition_colors[i] << std::endl;
            color_file << "          " << i << " | " << partition_colors[i] << std::endl;
        }
        color_file.close();
        //*/
/*
        std::string color_filename = filename;
        color_filename+="_color_partitions_parallel.txt";
        color_file.open(color_filename.c_str(), ios::app);

        //std::cout << std::endl << "      Color | #Partitions " << std::endl;
        color_file << "Threads: " << nthreads << std::endl;
        color_file << "Partitions: " << partition_colors.size() << std::endl;
        color_file << "Gamma: " << partition_colors.size() / colors << std::endl;
        color_file << "      Color | #Partitions " << std::endl;

        for (size_t i = 0; i < color_partitions.size(); ++i)
        {
            //std::cout << "          " << i << " | " << color_partitions[i].size() << std::endl;
            color_file << "          " << i << " | " << color_partitions[i].size() << std::endl;

    /*     std::cout << "          " << i << " | ";
            for (auto it : color_partitions[i])
            {
            std::cout << it << " ";
            }
            std::cout << std::endl;//*/
/*        }
        color_file.close();
        //END OF DEBUG*/
    }
    //-------------------------------------------------------------------------------------------------//
    //end of Parallel Graph Coloring Algorithm by Rokos et al. (A Fast and Scalable Graph Coloring Algorithm
    //for Multi-core and Many-core Architectures), 2015, arxiv.org
    //-------------------------------------------------------------------------------------------------//

    viennamesh::info(1) << "   Partitions param = " << num_regions << std::endl;
    viennamesh::info(1) << "   Partitions count = " << max+1 << std::endl;
    viennamesh::info(1) << "   Number of colors = " << colors << std::endl;

    return true;
}
//end of ColorPartitions

//CheckPartitionColoring()
//
//Tasks: Checks validity of the partition coloring
bool MeshPartitions::CheckPartitionColoring()
{
    viennamesh::info(1) << "Checking validity of the coloring" << std::endl;

    bool valid_coloring = true;

    //iterate partitions and check if their neighboring colors match
    //if yes, the coloring is invalid!!!
    //#pragma omp parallel for num_threads(nthreads)
    for (size_t part_id = 0; part_id < num_regions; ++part_id)
    {
        for (auto neigh_id : partition_adjcy[part_id])
        {
            if (partition_colors[part_id] == partition_colors[neigh_id])
            {
                //#pragma omp atomic write
                    valid_coloring = false;
                std::cout << "partition " << part_id << " has color " << partition_colors[part_id] << std::endl;
                std::cout << "neighbor  " << neigh_id << " has color " << partition_colors[neigh_id] << std::endl;
            }
        }
    }

    return valid_coloring;
}
//end of CheckPartitionColoring()

//CreatePragmaticDataStructures_ser
//
//Tasks: Get and order data needed to create a pragmatic data structure for each partition
//Runs only serial
//bool MeshPartitions::CreatePragmaticDataStructures_ser(Mesh<double>* const original_mesh, int num_regions)
bool MeshPartitions::CreatePragmaticDataStructures_ser()
{
    //reserve memory
    nodes_per_partition.resize(num_regions);
    elements_per_partition.resize(num_regions);
    g2l_vertex.resize(num_regions);
    l2g_vertex.resize(num_regions);

    //get ENList
    _ENList = original_mesh->get_enlist();

    //get the nodes for each partition
    for (int i = 0; i < num_elements; ++i)
    {
        //add nodes
        //TODO: the following hast to be updated for the 3D case!!
        nodes_per_partition[ epart[i] ].insert(_ENList[i*3]);
        nodes_per_partition[ epart[i] ].insert(_ENList[i*3+1]);
        nodes_per_partition[ epart[i] ].insert(_ENList[i*3+2]);

        //add element
        elements_per_partition[ epart[i] ].insert(i);
    } 
    //end of get nodes per partition

    //Create Partitions

    //vectors storing the coordinate
    std::vector< std::vector<double>> x_coords(num_regions);
    std::vector< std::vector<double>> y_coords(num_regions);
    std::vector< std::vector<double>> z_coords(num_regions);

    //vector storing the ENLists of each region
    std::vector<std::vector<index_t>> ENLists_partitions(num_regions);

     //loop over all partitions
    for (size_t i = 0; i < num_regions; ++i)
    {
        //get number of vertices and elements
        int num_points = nodes_per_partition[i].size();
        int num_cells = elements_per_partition[i].size();

        //get the vertex-to-index-mapping between old and new indices
        //and additionally the index-to-vertex-mapping
        //TODO: use unordered_map instead, to speed up the code

        std::unordered_map <index_t, index_t> global_to_local_index_map;
        std::unordered_map <index_t, index_t> local_to_global_index_map;

        index_t new_vertex_id = 0;
        for (auto it : nodes_per_partition[i])
        {
            global_to_local_index_map.insert( std::make_pair(it, new_vertex_id++) );
            //++vertex_appearances[it];
        }

        //global_to_local_index_mappings_partitions[i] = global_to_local_index_map;

        //and get also the index-to-vertex mapping (opposite direction than vertex to index mapping)
        for (auto it : global_to_local_index_map)
        {
            local_to_global_index_map[it.second] = it.first;
        }

        //local_to_global_index_mappings_partitions[i] = local_to_global_index_map;

        //pre-allocate memory
        x_coords[i].reserve(num_points);
        y_coords[i].reserve(num_points);
        ENLists_partitions[i].resize(3*num_cells);

        //get coordinates of each vertex
        int counter = 0;
        for (auto it : nodes_per_partition[i])
        {
            double p[2];
            original_mesh->get_coords( it, p);
            x_coords[i][counter] = p[0];
            y_coords[i][counter] = p[1];
            ++counter;
        }

        //create ENList with respect to the new vertex indices
        counter=0;        
        for (auto it : elements_per_partition[i])
        {      
            const index_t *element_ptr = nullptr;
            element_ptr = original_mesh->get_element(it);
            
            ENLists_partitions[i][counter++] = global_to_local_index_map[*(element_ptr++)];
            ENLists_partitions[i][counter++] = global_to_local_index_map[*(element_ptr++)];
            ENLists_partitions[i][counter++] = global_to_local_index_map[*(element_ptr++)];  
            ///++element_appearances[it];       
        }

        //create pragmatic mesh 
        Mesh<double> *partition_mesh = nullptr;

        //TODO: change for 3D refinement
        //mesh = new Mesh<double> ( num_points, num_cells, &(ENLists_regions[region.id()][0]) ,&(x_coords[region.id()][0]), &(y_coords[region.id()][0]), &(z_coords[region.id()][0]) );        
        partition_mesh = new Mesh<double> ( num_points, num_cells, &(ENLists_partitions[i][0]), &(x_coords[i][0]), &(y_coords[i][0]) );
        partition_mesh->create_boundary();
        pragmatic_partitions.push_back(partition_mesh);

        //Create metric field
        MetricField<double,2> metric_field(*partition_mesh);

        double eta = 0.0001;
        std::vector<double> psi(num_points);

        for (size_t i = 0; i < num_points; ++i)
        {
            //double x = 2*partition->get_coords(i)[0]-1;
            //double y = 2*partition->get_coords(i)[1]-1;
            double x = partition_mesh->get_coords(i)[0];
            double y = partition_mesh->get_coords(i)[1];
    
            psi[i] = 0.100000000000000*sin(50*x) + atan2(-0.100000000000000, (double)(2*x - sin(5*y)));
            //psi[i] = 800000000000;
        }
        
        metric_field.add_field(&(psi[0]), eta, 1);
        metric_field.update_mesh();

        //Create boundary information for refinement algorithm 
        std::vector<int> bdry = partition_mesh->get_boundaryRef();
        std::vector<int> boundary_nodes(partition_mesh->get_number_nodes(), 0);

        for (size_t eid = 0; eid < partition_mesh->get_number_elements(); ++eid)
        {
            const int *n = partition_mesh->get_element(eid);
            const int *boundary=&(bdry[eid*3]);

            //-1 means element is marked for deletion
            if(n[0]==-1)
                continue;

            if( partition_mesh->get_number_dimensions() == 2 ) 
            {
                for(int j=0; j<3; j++) 
                {
                    boundary_nodes[n[(j+1)%3]] = std::max(boundary_nodes[n[(j+1)%3]], bdry[eid*3+j]);
                    boundary_nodes[n[(j+2)%3]] = std::max(boundary_nodes[n[(j+2)%3]], bdry[eid*3+j]);
                }
            }
        }

        Refine<double,2> refiner(*partition_mesh);
        //refiner.refine(0.5, nodes_partition_ids, local_to_global_index_map);

        //delete partial_mesh;        //TODO: creates segfault if comments are removed
    } 
    //end of loop over all partitions

    viennamesh::info(5) << "Created " << pragmatic_partitions.size() << " pragmatic mesh data structures" << std::endl;
/*
    //DEBUG
      //write partitions
      for (size_t i = 0; i < pragmatic_partitions.size(); ++i)
      {
        std::cout << "Writing partition " << i << std::endl;
        std::cout << "  Vertex count = " << pragmatic_partitions[i]->get_number_nodes() << std::endl;
        std::cout << "  Cell count = " << pragmatic_partitions[i]->get_number_elements() << std::endl;
        
        std::string filename;
        filename += "examples/data/color_refinement/";
        filename += "output_";
        filename += "_partition_";
        filename += std::to_string( i );
      
        VTKTools<double>::export_vtu(filename.c_str(), pragmatic_partitions[i]);
      }
    //END OF DEBUG
*/
    return true;
}
//end of CreatePragmaticDataStructures_ser

//CreatePragmaticDataStructures_par
//
//Tasks: Get and order data needed to create a pragmatic data structure for each partition
//Runs in parallel
//bool MeshPartitions::CreatePragmaticDataStructures_par(Mesh<double>* const original_mesh, int num_regions)
/*bool MeshPartitions::CreatePragmaticDataStructures_par(std::vector<double>& threads_log, std::vector<double>& refine_times, std::vector<double>& l2g_build, 
                                                       std::vector<double>& l2g_access, std::vector<double>& g2l_build, std::vector<double>& g2l_access,
                                                       std::string algorithm, std::string options, std::vector<double>& triangulate_log,
                                                       std::vector<double>& ref_detail_log)//, std::vector<double>& build_tri_ds) */
bool MeshPartitions::CreatePragmaticDataStructures_par(std::string algorithm, std::vector<double>& threads_log, 
                                                       std::vector<double>& heal_log, std::vector<double>& metric_log,
                                                       std::vector<double>& call_refine_log, std::vector<double>& refine_log,
                                                       std::vector<double>& mesh_log, double & for_time, double & prep_time,
                                                       std::vector<double>& nodes_log, std::vector<double>& enlist_log,
                                                       std::string options, std::vector<size_t>& workload,
                                                       std::vector<size_t>& workload_elements, const int max_num_iterations,
                                                       std::vector<double>& get_interfaces_log, std::vector<double>& defrag_log,
                                                       std::vector<double>& refine_boundary_log)
{    
    viennamesh::info(1) << "Starting mesh adaptation with " << nthreads << " threads" << std::endl;
    std::cout << "RESIZE THE LOG_VECTORS TO MAX_THREADS TO AVOID UNNECESSARY CODE IN THE LOG-OUTPUT!!!" << std::endl;
    auto prep_tic = omp_get_wtime();

    //this->algorithm = algorithm;

    //Resize and fill vectors
    /*
    threads_log.resize(nthreads);
    mesh_log.resize(nthreads);
    heal_log.resize(nthreads);
    metric_log.resize(nthreads);
    call_refine_log.resize(nthreads);
    refine_log.resize(nthreads);
    nodes_log.resize(nthreads);
    enlist_log.resize(nthreads);
    workload.resize(nthreads);
    workload_elements.resize(nthreads);
    previous_nelements.resize(num_regions);
    get_interfaces_log.resize(nthreads);
    defrag_log.resize(nthreads);
    refine_boundary_log.resize(nthreads);*/

    threads_log.resize(MAX_THREADS);
    mesh_log.resize(MAX_THREADS);
    heal_log.resize(MAX_THREADS);
    metric_log.resize(MAX_THREADS);
    call_refine_log.resize(MAX_THREADS);
    refine_log.resize(MAX_THREADS);
    nodes_log.resize(MAX_THREADS);
    enlist_log.resize(MAX_THREADS);
    workload.resize(MAX_THREADS);
    workload_elements.resize(MAX_THREADS);
    previous_nelements.resize(num_regions);
    get_interfaces_log.resize(MAX_THREADS);
    defrag_log.resize(MAX_THREADS);
    refine_boundary_log.resize(MAX_THREADS);

    interfaces.resize(num_regions);
    NNInterfaces.resize(num_regions);

    std::fill(threads_log.begin(), threads_log.end(), 0.0);
    std::fill(mesh_log.begin(), mesh_log.end(), 0.0);
    std::fill(heal_log.begin(), heal_log.end(), 0.0);
    std::fill(metric_log.begin(), metric_log.end(), 0.0);
    std::fill(call_refine_log.begin(), call_refine_log.end(), 0.0);
    std::fill(refine_log.begin(), refine_log.end(), 0.0);
    std::fill(nodes_log.begin(), nodes_log.end(), 0.0);
    std::fill(enlist_log.begin(), enlist_log.end(), 0.0);
    std::fill(workload.begin(), workload.end(), 0);
    std::fill(workload_elements.begin(), workload_elements.end(), 0);
    std::fill(get_interfaces_log.begin(), get_interfaces_log.end(), 0.0);
    std::fill(defrag_log.begin(), defrag_log.end(), 0.0);
    std::fill(refine_boundary_log.begin(), refine_boundary_log.end(), 0.0);

    outboxes.resize(num_regions, Outbox());
    nodes_per_partition.resize(num_regions);

    std::vector<int> _ENList_orig = original_mesh->get_enlist();
    std::vector<std::set<int>> elements_part(num_regions);
    int dim = original_mesh->get_number_dimensions();

    //REPLACE THESE WITH TEMPLATE COMMAND
    pragmatic_partitions.resize(num_regions);
    triangle_partitions.resize(num_regions);
    tetgen_partitions.resize(num_regions);

    //Resizing vectors storing the index mapping information
    l2g_vertex.resize(num_regions);
    g2l_vertex.resize(num_regions);
    l2g_element.resize(num_regions);
    g2l_element.resize(num_regions);

    //get vertices and elements from original mesh
    for (size_t ele_id = 0; ele_id < original_mesh->get_number_elements(); ++ele_id)
    {
        if (dim == 2)
        {
            nodes_per_partition[ epart[ele_id] ].insert( _ENList_orig[3*ele_id] );
            nodes_per_partition[ epart[ele_id] ].insert( _ENList_orig[3*ele_id+1] );
            nodes_per_partition[ epart[ele_id] ].insert( _ENList_orig[3*ele_id+2] );
        }

        else
        {
            nodes_per_partition[ epart[ele_id] ].insert( _ENList_orig[4*ele_id] );
            nodes_per_partition[ epart[ele_id] ].insert( _ENList_orig[4*ele_id+1] );
            nodes_per_partition[ epart[ele_id] ].insert( _ENList_orig[4*ele_id+2] ); 
            nodes_per_partition[ epart[ele_id] ].insert( _ENList_orig[4*ele_id+3] ); 
        }
        elements_part[ epart[ele_id] ].insert(ele_id);
    }

    auto prep_toc = omp_get_wtime();
    prep_time = prep_toc - prep_tic;

    auto for_tic = omp_get_wtime();

    size_t max_rounds = 1;
    size_t refinement_rounds = 1;

    //set some values used in refinement (Pragmatic)
    auto msize = 0;
    auto nedge = 0;  
    auto nloc = 0;

    if (dim == 2)
    {
        msize = 3;
        nedge = 3;
        nloc = dim+1;
    }

    else
    {
        msize = 6;
        nedge = 6;
        nloc = dim+1;
    }

    //iterate the partitions several times
    for (size_t act_iter = 0; act_iter < max_num_iterations; act_iter++)
    {

        std::cout << " Resizing... " << std::endl;
        int tmp_glob_nnodes = 0;
        int tmp_glob_nelements = 0;

        if (act_iter > 0)
        {
            for (auto piter : pragmatic_partitions)
            {
                tmp_glob_nnodes += piter->get_number_nodes();
                tmp_glob_nelements += piter->get_number_elements();
            }
        }

        else 
        {
            tmp_glob_nnodes = original_mesh->get_number_nodes();
            tmp_glob_nelements = original_mesh->get_number_elements();
        }

        int resizer = tmp_glob_nnodes + tmp_glob_nnodes*nedge*1.5;
        std::cout << "resizer " << resizer << std::endl;
        nodes_partition_ids.resize(resizer);

        //std::cout << "  Iteration " << act_iter+1 << " / " << max_num_iterations << " ";
        viennamesh::info(2) << "Iteration " << act_iter+1 << " / " << max_num_iterations << std::endl;
        //iterate colors
        for (size_t color = 0; color < colors; ++color)
        {
            /*
            std::cout << std::endl << "    actual color / # of colors" << std::endl;
            std::cout << "      " << color << " / " << colors-1 << std::endl;
            std::cout << "      " << "color " << color << " has " << color_partitions[color].size() << " partitions" << std::endl;
            //*/
            #pragma omp parallel for schedule(dynamic) num_threads(nthreads)
            for (size_t part_iter = 0; part_iter < color_partitions[color].size(); ++part_iter)
            {
                auto threads_tic = omp_get_wtime();
    
                size_t part_id = color_partitions[color][part_iter];
/*
                #pragma omp critical
                {
                    std::cout << "        Working on partition " << part_id << std::endl;
                }//*/
                Outbox outbox_data;
                Mesh<double>* partition = nullptr;

                // auto msize = 0;
                // auto nedge = 0;  
                // auto nloc = 0;

                // if (dim == 2)
                // {
                //     msize = 3;
                //     nedge = 3;
                //     nloc = dim+1;
                // }

                // else
                // {
                //     msize = 6;
                //     nedge = 6;
                //     nloc = dim+1;
                // }

                //get number of vertices and elements of the local partitions
                int num_points_part = 0;
                int num_elements_part = 0;
                if (act_iter == 0)
                {
                    num_points_part = nodes_per_partition[part_id].size() + outbox_data.num_verts();
                    num_elements_part = elements_part[part_id].size();
                }

                else 
                {
                    partition = pragmatic_partitions[part_id];
                    //std::cout << " is the pointer in the codeline above pointing to the correct data?" << std::endl;
                    num_points_part = partition->get_number_nodes();
                    num_elements_part = partition->get_number_elements();
                }
                //Interface vector for vertices and facets
                //std::vector<std::vector<int>> NNInterfaces_tmp;
                std::vector<std::vector<std::vector<int>>> NNInterfaces_tmp;
                std::vector<std::vector<int>> FInterfaces_tmp;
                
                //g2l- and l2g-index mappings
                std::unordered_map<int, int> g2l_vertices_tmp;
                std::unordered_map<int, int> g2l_elements_tmp;
                std::vector<int> l2g_vertices_tmp;
                std::vector<int> l2g_elements_tmp;

                //create variables to store timing data
                double mesh_tic;
                double nodes_toc;
                double nodes_tic;
                double enlist_toc;
                double enlist_tic;

                //Get mesh data from original mesh only in the very first iteration
                if (act_iter == 0)
                {
                    //create coordinate vectors, g2l- and l2g-index-mappings for the vertices
                    g2l_vertices_tmp.reserve(num_points_part);
                    g2l_elements_tmp.reserve(num_elements_part);
                    //use vector instead of unordered maps!
                    l2g_vertices_tmp.resize(num_points_part);
                    l2g_elements_tmp.resize(num_elements_part);

                    int new_vertex_id = 0;    //new vertex id for local partition
                    
                //  auto coords_tic = std::chrono::system_clock::now();

                    //coords vector
                    std::vector<double> x_coords(num_points_part);
                    std::vector<double> y_coords(num_points_part); 
                    std::vector<double> z_coords;

                    if (dim == 3)
                        z_coords.reserve(num_points_part);

        /*
                    double l2g_time {0.0};
                    double g2l_time {0.0};
        */
                    //Get vertex coordinates from original mesh
                    nodes_tic = omp_get_wtime();

                    for (auto it : nodes_per_partition[part_id])
                    {
                        if (dim == 2)
                        {
                            double p[2];
                            original_mesh->get_coords( it, p);
                            
                            x_coords[new_vertex_id] = p[0];
                            y_coords[new_vertex_id] = p[1];
                        }

                        else
                        {
                            double p[3];
                            original_mesh->get_coords( it, p);
                            
                            x_coords[new_vertex_id] = p[0];
                            y_coords[new_vertex_id] = p[1]; 
                            z_coords[new_vertex_id] = p[2];
                        }
                        
        //             auto g2l_tic = std::chrono::system_clock::now();
                        g2l_vertices_tmp.insert( std::make_pair(it, new_vertex_id) );
        /*             std::chrono::duration<double> g2l_dur = std::chrono::system_clock::now() - g2l_tic;
                        g2l_time += g2l_dur.count();             
        */
        //              auto l2g_tic = std::chrono::system_clock::now();
                        l2g_vertices_tmp[new_vertex_id++] = it;
            /*            std::chrono::duration<double> l2g_dur = std::chrono::system_clock::now() - l2g_tic;
                        l2g_time += l2g_dur.count();
                */        
                    } //end of for over nodes_per_partition[part_id]

                    nodes_toc = omp_get_wtime();

                    //Create ENLists for local partition and the element-index-mappings
        //          auto enlist_tic = std::chrono::system_clock::now();
                    enlist_tic = omp_get_wtime();

                    auto ctr = 0;          
                    std::vector<int>ENList_part;

                    if (dim == 2)
                    {
                        ENList_part.reserve(3*num_elements_part);
                    }

                    else
                    {
                        ENList_part.reserve(4*num_elements_part);
                    }

                    //auto counter {0};
                    for (auto it : elements_part[part_id])
                    {              
                        //update l2g and g2l element mappings
                        l2g_elements_tmp[ctr/(dim+1)]=it;
                        g2l_elements_tmp.insert( std::make_pair(it, ctr/(dim+1)) );

                        const int *element_ptr = nullptr;
                        element_ptr = original_mesh->get_element(it);
                        
                        ENList_part[ctr++] = g2l_vertices_tmp[*(element_ptr++)];
                        ENList_part[ctr++] = g2l_vertices_tmp[*(element_ptr++)];
                        ENList_part[ctr++] = g2l_vertices_tmp[*(element_ptr++)]; //three times for triangles

                        if (dim == 3)
                        {
                            ENList_part[ctr++] = g2l_vertices_tmp[*(element_ptr++)]; //four times for tetrahedra
                        }
                    } //end of for loop iterating elements_part

                    enlist_toc = omp_get_wtime();

        /*           std::chrono::duration<double> enlist_time = std::chrono::system_clock::now() - enlist_tic;
                    times[5] += enlist_time.count();
                    //g2l_access[omp_get_thread_num()] += enlist_time.count();
        */

                    mesh_tic = omp_get_wtime();

                    //create pragamtic data structure and the partition boundary
                    //Mesh<double>* partition = nullptr;

                    if (dim == 2)
                        partition = new Mesh<double>(num_points_part, num_elements_part, &(ENList_part[0]), &(x_coords[0]), &(y_coords[0]));

                    else
                        partition = new Mesh<double>(num_points_part, num_elements_part, &(ENList_part[0]), &(x_coords[0]), &(y_coords[0]), &(z_coords[0]));

                    auto boundary_tic = omp_get_wtime();

                    partition->create_boundary();

                } //end of if - get mesh data from the original mesh only in the very first iteration

                //if act_iter > 1, get data from already available Pragmatic partition
                else
                {
                    //Partition already assigned at the beginning of the for-loop iterating iterations

                    g2l_vertices_tmp = g2l_vertex[part_id];
                    g2l_elements_tmp = g2l_element[part_id];
                    //use vector instead of unordered maps!
                    l2g_vertices_tmp = l2g_vertex[part_id];
                    l2g_elements_tmp = l2g_element[part_id];
                }
                /*
                //DEBUG
                std::cout << "  debug mesh output input before healing partition " << part_id << " iteration " << (act_iter+1)<< std::endl;
                std::string vtu_filename = "examples/data/color_refinement/output/input_before_healing_part";
                vtu_filename+=std::to_string(part_id);
                vtu_filename+="_iteration";
                vtu_filename+=std::to_string(act_iter);
                vtu_filename+=".vtu";
                VTKTools<double>::export_vtu(vtu_filename.c_str(), partition);
                //END OF DEBUG*/

                //store the number of elements in the mesh before healing!!! (needed for refinement)
                previous_nelements[part_id]=partition->get_number_elements();

                auto mesh_toc = omp_get_wtime();
/*
                //DEBUG
                std::string nodes_ids_name = "debug_output/nodes_partition_ids_before_healing_partition";
                nodes_ids_name += std::to_string(part_id);
                nodes_ids_name += "_iteration";
                nodes_ids_name += std::to_string(act_iter);
                nodes_ids_name += ".txt";

                std::ofstream nodes_ids_file;
                nodes_ids_file.open(nodes_ids_name.c_str());

                for (size_t nodes_partition_ids_iter = 0; nodes_partition_ids_iter < partition->get_number_nodes(); ++nodes_partition_ids_iter)
                {
                    nodes_ids_file << "Vertex " << nodes_partition_ids_iter << " (global ID: " << l2g_vertices_tmp[nodes_partition_ids_iter] << ") of " << partition->get_number_nodes() << std::endl;
                    double v[dim];
                    partition->get_coords(nodes_partition_ids_iter, v);
                    nodes_ids_file << v[0] << " " << v[1] << " " << v[2] << std::endl;
                    for (auto nodes_ids_iter : nodes_partition_ids[l2g_vertices_tmp[nodes_partition_ids_iter]])
                    {
                        nodes_ids_file << "  " << nodes_ids_iter << std::endl;
                    }
                }

                nodes_ids_file.close();
                //END OF DEBUG*/

                //Create metric field
                auto metric_tic = omp_get_wtime();
                if (act_iter == 0)
                {
                    //auto metric_tic = std::chrono::system_clock::now();                    

                /*  std::cout << " NNodes in partition: " << partition->get_number_nodes() << std::endl;
                    std::cout << " NElements in partition: " << partition->get_number_elements() << std::endl;*/

                    if (algorithm == "pragmatic" || algorithm == "tetgen" || algorithm == "pragmaticcavity")
                    {
                        //partition->get_interfaces(NNInterfaces_tmp, nodes_partition_ids, l2g_vertices_tmp, g2l_vertices_tmp, part_id, FInterfaces_tmp);
    /*
                        //DEBUG                
                        std::cout << " FInterfaces of partition " << part_id << std::endl;
                        for (auto FI_iter = 0; FI_iter < FInterfaces_tmp.size(); ++FI_iter)
                        {
                            std::cout << FI_iter << std::endl;

                            for (auto FI_iter1 = 0; FI_iter1 < FInterfaces_tmp[FI_iter].size(); ++FI_iter1)
                            {
                                std::cout << " " << FInterfaces_tmp[FI_iter][FI_iter1] << std::endl;
                            }
                        }

                        std::cout << " NNInterfaces of partition " << part_id << std::endl;
                        for (auto NNI_iter = 0; NNI_iter < NNInterfaces_tmp.size(); ++NNI_iter)
                        {
                            std::cout << NNI_iter << std::endl;

                            for (auto NNI_iter1 = 0; NNI_iter1 < NNInterfaces_tmp[NNI_iter].size(); ++NNI_iter1)
                            {
                                std::cout << " " << NNInterfaces_tmp[NNI_iter][NNI_iter1] << std::endl;
                            }
                        }
                        //END OF DEBUG*/

                        if (dim == 2)
                        {
                            MetricField<double,2> metric_field(*partition);
            /*
                        double eta = 0.0001;
                        std::vector<double> psi(num_points_part);

                        for (size_t i = 0; i < num_points_part; ++i)
                        {
                            //double x = 2*partition->get_coords(i)[0]-1;
                            //double y = 2*partition->get_coords(i)[1]-1;
                            double x = partition->get_coords(i)[0];
                            double y = partition->get_coords(i)[1];
                    
                            psi[i] = 0.100000000000000*sin(50*x) + atan2(-0.100000000000000, (double)(2*x - sin(5*y)));
                            //psi[i] = 800000000000;
                        }
                        
                        metric_field.add_field(&(psi[0]), eta, 1);
            */

                            for (auto i = 0; i < partition->get_number_nodes(); ++i)
                            {
                                double m[] = {1.0, 1.0, 0.0};
                                metric_field.set_metric(m, i);
                            }
                            
                            //           auto metric_update_tic = std::chrono::system_clock::now();
                            metric_field.update_mesh();
            /*           std::chrono::duration<double> metric_update_time = std::chrono::system_clock::now() - metric_update_tic;*/
                        }

                        else
                        {
                            MetricField<double,3> metric_field(*partition);

                            //std::cout << "set metric field" << std::endl;
                            for (auto i = 0; i < partition->get_number_nodes(); ++i)
                            {
                                //std::cout << i << "/" << partition->get_number_nodes() << std::endl;
                                double m[] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.0};
                                metric_field.set_metric(m, i);
                            }

                            //auto metric_update_tic = std::chrono::system_clock::now();
                            //std::cout << " metric update in partition " << part_id << std::endl;
                            metric_field.update_mesh();
                    /*      std::chrono::duration<double> metric_update_time = std::chrono::system_clock::now() - metric_update_tic;*/
                        }
                //     std::chrono::duration<double> metric_time = std::chrono::system_clock::now() - metric_tic;
            /*
                        times[8] += metric_time.count();
                        times[9] += metric_update_time.count();   
                        */
                    }//end if algorithm == pragmatic for metric assignment
                } //end of if (act_iter == 0) for mesh metric update
                auto metric_toc = omp_get_wtime();
    
   
                //Heal mesh if the partition has data in its outbox
                auto heal_tic = omp_get_wtime();

                auto NNodes_before_healing = partition->get_number_nodes();
                auto NElements_before_healing = partition->get_number_elements();

                std::vector<int> new_vertices_per_element;
                /*
                //DEBUG
                std::vector<int> headV2E(partition->get_number_nodes()+1);
                headV2E[0] = 0;
                for (int i=0; i<partition->get_number_nodes(); ++i) {
                    headV2E[i+1] = headV2E[i];
                    for (int it=0; it<partition->NNList[i].size(); ++it) {
                        if (partition->NNList[i][it] > i) {
                            headV2E[i+1]++;
                        }
                    }
                } 

                std::cout << " NEdges before healing " << headV2E[partition->get_number_nodes()] << std::endl;
                //END OF DEBUG*/

                //Heal mesh if vertices have been inserted on interface and color is greater than 0
                //TODO: Change if-clause depending on the direction of the color-queue(increasing or decreasing color)
                if (color > 0)
                {
                    /*std::cout << "  healing of partition " << part_id << std::endl;
                    std::cout << "  partition has " << NNodes_before_healing << " initial nodes and " << NElements_before_healing << " initial elements" << std::endl;//*/
                    // Set the orientation of elements.
                    ElementProperty<double> * part_property = nullptr;

                    for(size_t i=0; i < partition->get_number_elements(); i++) 
                    {
                        const int *n=partition->get_element(i);
                        if(n[0]<0)
                            continue;

                        if(dim==2)
                        {
                            part_property = new ElementProperty<double>(partition->get_coords(n[0]), partition->get_coords(n[1]), partition->get_coords(n[2]));
                        }

                        else if(dim==3)
                        {
                            part_property = new ElementProperty<double>(partition->get_coords(n[0]), partition->get_coords(n[1]), partition->get_coords(n[2]), partition->get_coords(n[3]));
                        }

                        break;
                    }

                    for (auto it : partition_adjcy[part_id])
                    {
                        /*std::cout << " it: " << it << std::endl;*/
                        //first check if color of neighbor is smaller than own color, otherwise there is no data in the neighbor's outbox!!!*/
                        if (partition_colors[it] < color && outboxes[it].num_verts() > 0)
                        {   
                            size_t orig_NNodes_part = partition->get_number_nodes();
                            size_t orig_NElements_part = partition->get_number_elements();

                            auto verts_in_part = outboxes[it].verts_in_part(part_id);
                            std::vector<int> outbox_mapping(verts_in_part, -1);

                            //Resize vectors
                            new_vertices_per_element.resize(nedge*orig_NElements_part);
                            std::fill(new_vertices_per_element.begin(), new_vertices_per_element.end(), -1);

                            size_t reserve = verts_in_part + partition->NNodes;

                            if(partition->_coords.size()<reserve*dim) 
                            {
                                partition->_coords.resize(reserve*dim);
                                partition->metric.resize(reserve*msize);
                                partition->NNList.resize(reserve);
                                partition->NEList.resize(reserve);
                                partition->node_owner.resize(reserve);
                                partition->lnn2gnn.resize(reserve);
                            }
                            
                            auto edgeSplitCnt = partition->NNodes - orig_NNodes_part;

                            //Append new coords and new metrics to the partition
                            for (size_t i = 0, j = 0; i < outboxes[it].num_verts(); ++i)
                            {
                                if (outboxes[it][4*i] != part_id)
                                {
                                    continue;
                                }

                                try 
                                {
                                    g2l_vertices_tmp.at(outboxes[it][4*i+1]);
                                    g2l_vertices_tmp.at(outboxes[it][4*i+2]);
                                }

                                catch(...)
                                {
                                    continue;
                                }

                                //get 3rd element due to construction of outbox vector (3rd ele is local id of vertex in partition it)
                                /*std::cout << " local id in originating partition " << it << " is " << outboxes[it][4*i+3] << std::endl;
                                std::cout << " dim " << dim << std::endl;//*/
                                double p[dim];
                                pragmatic_partitions[it]->get_coords(outboxes[it][4*i+3], p);
                                /*
                                if (dim==3)
                                {
                                    std::cout << "healing coords for new vertex " << j << std::endl;
                                    std::cout << "  " << p[0] << " " << p[1] << " " << p[2] << " " << &(p[0]) << std::endl;
                                    std::cout << "  " << pragmatic_partitions[it]->get_coords(outboxes[it][4*i+3])[0] << " " << pragmatic_partitions[it]->get_coords(outboxes[it][4*i+3])[1];
                                    std::cout << " " << pragmatic_partitions[it]->get_coords(outboxes[it][4*i+3])[2] << std::endl;
                                }//*/

                                double m[msize];
                                pragmatic_partitions[it]->get_metric(outboxes[it][4*i+3], m);

                                partition->append_vertex(&p[0], &m[0]);
                               
                                l2g_vertices_tmp.push_back( l2g_vertex[it][outboxes[it][4*i+3]]);
                                g2l_vertices_tmp.insert(std::make_pair(l2g_vertex[it][outboxes[it][4*i+3]], orig_NNodes_part+j));
                               /* #pragma omp critical
                                {*/
                                
                                assert(l2g_vertex[it][outboxes[it][4*i+3]] < nodes_partition_ids.size());

                                nodes_partition_ids[l2g_vertex[it][outboxes[it][4*i+3]]].insert(part_id);
                               /* }*/

                                outbox_mapping[j] = partition->get_number_nodes()-1; 

                                //DEBUG
                                if (m[0] == 0 && m[1] == 0 && m[2] == 0 &&
                                    m[3] == 0 && m[4] == 0 && m[5] == 0)
                                {
                                    std::cout << " empty metric for " << outbox_mapping[j] << std::endl;
                                }
                                //END OF DEBUG

                                ++j;
                            }

                            std::set<int> elements_to_heal;

                            // Mark each element with its new vertices,
                            // update NNList for all split edges.
                            for (size_t i = 0, j = 0; i < outboxes[it].num_verts(); ++i)
                            {
                                if (outboxes[it][4*i] != part_id)
                                {
                                    continue;
                                }

                                try 
                                {
                                    g2l_vertices_tmp.at(outboxes[it][4*i+1]);
                                    g2l_vertices_tmp.at(outboxes[it][4*i+2]);
                                }

                                catch(...)
                                {
                                    continue;
                                }

                                auto vid = outboxes[it][4*i+3];
                                auto glob_firstid = outboxes[it][4*i+1];
                                auto glob_secondid = outboxes[it][4*i+2];

                                /*double p[dim];
                                original_mesh->get_coords(glob_firstid, p);
                                original_mesh->get_coords(glob_secondid, p);*/

                                auto firstid = g2l_vertices_tmp.at(glob_firstid);
                                auto secondid = g2l_vertices_tmp.at(glob_secondid);
                                auto local_vid = outbox_mapping[j];

                                double q[dim];
                                partition->get_coords(local_vid, q);
                                /*
                                if(dim==3)
                                {
                                    std::cout << "      " << q[0] << " " << q[1] << " " << q[2] << std::endl;
                                }//*/

                                // Find which elements share this edge and mark them with their new vertices.
                                std::set<int> NEList_firstid = partition->get_nelist(firstid);
                                std::set<int> NEList_secondid = partition->get_nelist(secondid);

                                std::set<index_t> intersection;
                                std::set_intersection(NEList_firstid.begin(), NEList_firstid.end(),
                                                    NEList_secondid.begin(), NEList_secondid.end(),
                                                    std::inserter(intersection, intersection.begin()));

                                for (auto ele_iter : intersection)
                                {
                                    size_t edgeOffset = edgeNumber(partition, ele_iter, firstid, secondid);

                                    if (nedge*ele_iter+edgeOffset > new_vertices_per_element.size())
                                    {
                                        viennamesh::error(1) << "Part_id: " << part_id << ", outboxes[it]: " << it << std::endl;
                                        viennamesh::error(1) << " outboxes[it][4*i]: " << outboxes[it][4*i] << std::endl;
                                        viennamesh::error(1) << " i: " << i << ", local_vid: " << local_vid << std::endl;
                                        viennamesh::error(1) << " firstid: " << firstid << ", secondid: " << secondid << std::endl; 
                                        viennamesh::error(1) << " got edgenumber: " << edgeOffset << std::endl;
                                        viennamesh::error(1) << " new_vertices_per_element.size = " << new_vertices_per_element.size() << std::endl;
                                        viennamesh::error(1) << " nedge: " << nedge << ", " << " eid: " << ele_iter << ", " << nedge*ele_iter + edgeOffset << std::endl;
                                    }

                                    new_vertices_per_element[nedge*ele_iter+edgeOffset] = local_vid;
                                    //std::cout << "   element " << ele_iter << " has to be healed " << std::endl;
                                    elements_to_heal.insert(ele_iter);
                                }

                                //std::cout << "local_vid " << local_vid << " NNodes_before_healing+j" << NNodes_before_healing+j << std::endl;

                                //Update NNList for newly created vertices.                        
                                /*partition->NNList[NNodes_before_healing+j].push_back(firstid);
                                partition->NNList[NNodes_before_healing+j].push_back(secondid);*/
                                partition->NNList[local_vid].push_back(firstid);
                                partition->NNList[local_vid].push_back(secondid);

                                partition->remove_nnlist(firstid, secondid);
                                partition->add_nnlist(firstid, local_vid);
                                partition->remove_nnlist(secondid, firstid);
                                partition->add_nnlist(secondid, local_vid);

                                ++j;
                                //*/
                            } //end of update NNList for all split edges

                            // If in 3D, we need to refine facets first. 
                            int origNElements = partition->get_number_elements();

                            if (dim == 3)
                            {
                                //std::cout << "do facet healing of partition " << part_id << std::endl;
                                for (auto eid : elements_to_heal)
                                {
                                    //std::cout << "   processing element " << eid << std::endl;//*/
                                    // Find the 4 facets comprising the element
                                    const index_t *n = partition->get_element(eid);
                                    if (n[0] <  0)
                                        continue;

                                    const index_t facets[4][3] = {{n[0], n[1], n[2]},
                                                                {n[0], n[1], n[3]},
                                                                {n[0], n[2], n[3]},
                                                                {n[1], n[2], n[3]}};

                                    for(int j=0; j<4; ++j) 
                                    {
                                        // Find which elements share this facet j
                                        const index_t *facet = facets[j];
                                        std::set<index_t> intersection01, EE;
                                        std::set_intersection(partition->NEList[facet[0]].begin(), partition->NEList[facet[0]].end(),
                                                            partition->NEList[facet[1]].begin(), partition->NEList[facet[1]].end(),
                                                            std::inserter(intersection01, intersection01.begin()));
                                        std::set_intersection(partition->NEList[facet[2]].begin(), partition->NEList[facet[2]].end(),
                                                            intersection01.begin(), intersection01.end(),
                                                            std::inserter(EE, EE.begin()));

                                        assert(EE.size() <= 2 );
                                        assert(EE.count(eid) == 1);

                                        // Prevent facet from being refined twice:
                                        // Only refine it if this is the element with the highest ID.
                                        if(eid == *EE.rbegin())
                                        {
                                            for(size_t k=0; k<3; ++k)
                                            {
                                                if(new_vertices_per_element[nedge*eid+edgeNumber(partition, eid, facet[k], facet[(k+1)%3])] != -1) 
                                                {
                                                    heal_facet(partition, nedge, new_vertices_per_element, eid, facet);
                                                    break;
                                                }
                                            }
                                            //elements_to_heal.insert(eid);
                                        } //end of if eid == *EE.rbegin()
                                    } //end of for loop int j < 4
                                } //end of for loop over origNElements
                            }   //end of if (dim == 3) for facet refining         
                            /*
                            //DEBUG
                            std::cout << "elements in elements_to_heal" << std::endl;

                            for (auto ele_iter : elements_to_heal)
                                std::cout << "   " << ele_iter << std::endl;
                            //END OF DEBUG*/

                            // Start element healing (i.e., refine the marked boundary elements)
                            auto splitCnt = 0;

                            for (auto ele_id : elements_to_heal)
                            {
                                for(size_t j=0; j<nedge; ++j)
                                {
                                    if(new_vertices_per_element[nedge*ele_id+j] != -1) 
                                    {

                                        if (dim == 2)
                                        {
                                            const int *n=partition->get_element(ele_id);

                                            // Note the order of the edges - the i'th edge is opposite the i'th node in the element.
                                            index_t newVertex[3] = {-1, -1, -1};
                                            newVertex[0] = new_vertices_per_element[nedge*ele_id];
                                            newVertex[1] = new_vertices_per_element[nedge*ele_id+1];
                                            newVertex[2] = new_vertices_per_element[nedge*ele_id+2];


                                            int heal_cnt=0;
                                            for(size_t i=0; i<3; ++i)
                                            {
                                                if(newVertex[i]!=-1)
                                                {
                                                    ++heal_cnt;
                                                }
                                            }

                                            if (heal_cnt == 1)
                                            {
                                                heal2D_1(partition, newVertex, ele_id, nloc, splitCnt, origNElements);
                                            }

                                            else if (heal_cnt == 2)
                                            {
                                                heal2D_2(partition, newVertex, ele_id, nloc, splitCnt, origNElements);
                                            }

                                            break;
                                        } //end of if (dim == 2)

                                        //else dim == 3
                                        else
                                        {
                                            //std::cout << "do 3D element healing of partition " << part_id << std::endl;
                                            const int *n=partition->get_element(ele_id);

                                            int heal_cnt;
                                            std::vector< DirectedEdge<index_t> > splitEdges;

                                            for(int j=0, pos=0; j<4; j++)
                                            {
                                                for(int k=j+1; k<4; k++) 
                                                {
                                                    index_t vertexID = new_vertices_per_element[nedge*ele_id+pos];
                                                    if(vertexID >= 0) 
                                                    {
                                                        splitEdges.push_back(DirectedEdge<index_t>(n[j], n[k], vertexID));
                                                    }
                                                    ++pos;
                                                }
                                            }
                                        
                                            heal_cnt=splitEdges.size();

                                            /*std::cout << " element " << ele_id << std::endl;
                                            std::cout << "    heal_cnt: " << heal_cnt << std::endl;//*/

                                            if (heal_cnt == 1)
                                            {
                                                heal3D_1(partition, ele_id, nloc, splitCnt, origNElements, splitEdges);
                                            }

                                            else if (heal_cnt == 2)
                                            {
                                                heal3D_2(partition, ele_id, nloc, splitCnt, origNElements, splitEdges);
                                            }

                                            else if (heal_cnt == 3)
                                            {
                                                heal3D_3(partition, ele_id, nloc, splitCnt, origNElements, splitEdges, msize, dim, part_property);
                                            }

                                            else if (heal_cnt == 4)
                                            {
                                                heal3D_4(partition, ele_id, nloc, splitCnt, origNElements, splitEdges, msize, dim, part_property);
                                            }

                                            else if (heal_cnt == 5)
                                            {
                                                heal3D_5(partition, ele_id, nloc, splitCnt, origNElements, splitEdges, msize, dim, part_property);
                                            }

                                            break;

                                        } //end of else (dim == 3)
                                    }
                                }

                            } //end of element healing*/
                            
                        } //end of if (partition_colors[it] < color)*/
                    } //end of for (auto it : partition_adjcy[part_id])
                    delete part_property;
                    //std::cout << "  partition has " << partition->get_number_nodes() << " nodes and " << partition->get_number_elements() << " elements after healing" << std::endl;
                } //end of //Heal mesh if the partition has data in its outbox (color > 0)
                /*
                //DEBUG
                std::vector<int> headV2E_after_healing(partition->get_number_nodes()+1);
                headV2E_after_healing[0] = 0;
                for (int i=0; i<partition->get_number_nodes(); ++i) {
                    headV2E_after_healing[i+1] = headV2E_after_healing[i];
                    for (int it=0; it<partition->NNList[i].size(); ++it) {
                        if (partition->NNList[i][it] > i) {
                            headV2E_after_healing[i+1]++;
                        }
                    }
                } 

                //END OF DEBUG*/
                /*
                //DEBUG 
                std::cout << "headV2E_after_healing" << std::endl;
                for (size_t i = 0 ; i < headV2E_after_healing.size(); ++i)
                {
                    std::cout << i << " " << headV2E_after_healing[i] << std::endl;
                }
                //END OF DEBUG*/

                /*
                //DEBUG
                std::string nodes_ids_name_after = "debug_output/nodes_partition_ids_after_healing_partition";
                nodes_ids_name_after += std::to_string(part_id);
                nodes_ids_name_after += "_iteration";
                nodes_ids_name_after += std::to_string(act_iter);
                nodes_ids_name += ".txt";

                std::ofstream nodes_ids_file_after;
                nodes_ids_file_after.open(nodes_ids_name_after.c_str());

                for (size_t nodes_partition_ids_iter = 0; nodes_partition_ids_iter < partition->get_number_nodes(); ++nodes_partition_ids_iter)
                {
                    nodes_ids_file_after << "Vertex " << nodes_partition_ids_iter << " (global ID: " << l2g_vertices_tmp[nodes_partition_ids_iter] << ") of " << partition->get_number_nodes() << std::endl;
                    double v[dim];
                    partition->get_coords(nodes_partition_ids_iter, v);
                    nodes_ids_file << v[0] << " " << v[1] << " " << v[2] << std::endl;
                    for (auto nodes_ids_iter : nodes_partition_ids[l2g_vertices_tmp[nodes_partition_ids_iter]])
                    {
                        nodes_ids_file_after << "  " << nodes_ids_iter << std::endl;
                    }
                }

                nodes_ids_file_after.close();
                //END OF DEBUG*/

                //Output timings
                auto heal_toc = omp_get_wtime();

                auto interfaces_tic = omp_get_wtime();
                partition->get_interfaces(NNInterfaces_tmp, nodes_partition_ids, l2g_vertices_tmp, g2l_vertices_tmp, part_id, /*FInterfaces_tmp,*/ act_iter+1);
                auto interfaces_toc = omp_get_wtime();
                //partition->defragment();

                /*
                //Output healed mesh
                std::cout << "  debug mesh output healing partition " << part_id << " iteration " << (act_iter+1)<< std::endl;
                std::string vtu_filename_heal_output = "examples/data/color_refinement/output/healed_part";
                vtu_filename_heal_output+=std::to_string(part_id);
                vtu_filename_heal_output+="_iteration";
                vtu_filename_heal_output+=std::to_string(act_iter+1);
                vtu_filename_heal_output+=".vtu";
                VTKTools<double>::export_vtu(vtu_filename_heal_output.c_str(), partition);//*/

                //std::cerr << " mesh healing done for partition " << part_id << std::endl;

                //double boundary_time = std::chrono::system_clock::now() - boundary_tic;
                /*NNInterfaces_tmp.resize(partition->get_number_nodes());
                FInterfaces_tmp.resize(partition->get_number_elements());*/ //Resizing done in get_interfaces()
                pragmatic_partitions[part_id] = partition;
                l2g_vertex[part_id] = l2g_vertices_tmp;
                g2l_vertex[part_id] = g2l_vertices_tmp;
                l2g_element[part_id] = l2g_elements_tmp;
                g2l_element[part_id] = g2l_elements_tmp;

        //       std::chrono::duration<double> mesh_time = std::chrono::system_clock::now() - mesh_tic;
        /*  
                times[6] += mesh_time.count();
                times[7] += boundary_time.count();
*/
                //METRIC USED TO BE HERE!!! 

                //double int_check_time {0.0};
                //double triangulate_time {0.0};
                double call_to_refine_time = 0.0;
                double defrag_time = 0.0;
                double refine_boundary_time = 0.0;
                //double tri_ds_time{0.0};

                viennamesh::info(2) << " Partition " << part_id << " has " << partition->get_number_nodes() << " Vertices and " << partition->get_number_elements() << " Elements" << std::endl;

            // auto refine_tic = std::chrono::system_clock::now();
                auto refine_tic = omp_get_wtime();
                if (algorithm == "pragmatic")
                {
                    if (dim == 2)
                    {
                        Refine<double,2> refiner(*partition);
                        double L_max = std::stod(options);
                        std::cout << "  L_max: " << L_max << std::endl;

                        double call_to_refine_tic = omp_get_wtime();
                        //if (color == 0)
                        {   
                            //#pragma omp critical
                            {
                                //std::cout << " 2d refining partition " << part_id << " with thread " << omp_get_thread_num() << std::endl;
                                //std::cerr << ".";
                                refiner.refine(L_max, nodes_partition_ids, l2g_vertices_tmp, part_id, outbox_data, 
                                        partition_colors, partition_adjcy[part_id], previous_nelements[part_id],
                                        NNInterfaces_tmp, global_NNodes, g2l_vertices_tmp, NNodes_before_healing, NElements_before_healing,
                                        /*FInterfaces_tmp,*/ act_iter+1);//*/  
                            }                          
                        } 

                        /*
                        //Output refined partition in each iteration
                        std::cout << "  debug mesh output refined partition " << part_id << " iteration " << (act_iter+1)<< std::endl;
                        std::string vtu_filename_refine_output = "examples/data/color_refinement/output/refine_part";
                        vtu_filename_refine_output+=std::to_string(part_id);
                        vtu_filename_refine_output+="_iteration";
                        vtu_filename_refine_output+=std::to_string(act_iter+1);
                        vtu_filename_refine_output+=".vtu";
                        VTKTools<double>::export_vtu(vtu_filename_refine_output.c_str(), partition);//*/

                        /*//DEBUG
                        ofstream dbg;
                        std::string dbg_filename = "examples/data/color_refinement/output/dbg_part";
                        dbg_filename+=std::to_string(part_id);
                        dbg_filename+="_iteration";
                        dbg_filename+=std::to_string(act_iter+1);
                        dbg_filename+=".txt";
                        dbg.open(dbg_filename.c_str());

                        dbg << "NNList of Partition " << part_id << " after refinement" << std::endl;
                        for (size_t vid = 0; vid < partition->get_number_nodes(); ++vid)
                        {
                            dbg << "Vert " << vid << std::endl;
                            for (auto nn_id : partition->NNList[vid])
                            {
                                dbg << "  " << nn_id << std::endl;
                            }
                        }

                        dbg << "NEList of Partition " << part_id << " after refinement" << std::endl;
                        for (size_t vid = 0; vid < partition->get_number_nodes(); ++vid)
                        {
                            dbg << "Vert " << vid << std::endl;
                            for (auto ne_id : partition->NEList[vid])
                            {
                                dbg << "  " << ne_id << std::endl;
                            }
                        }

                        dbg.close();
                        //END OF DEBUG


                        /*
                        // Set the orientation of elements.
                        ElementProperty<double> * part_property_after_refinement = nullptr;
                        for(size_t i=0; i < partition->get_number_elements(); i++) 
                        {
                            const int *n=partition->get_element(i);
                            if(n[0]<0)
                                continue;

                            if(dim==2)
                            {
                                part_property_after_refinement = new ElementProperty<double>(partition->get_coords(n[0]), partition->get_coords(n[1]), partition->get_coords(n[2]));
                            }

                            else if(dim==3)
                            {
                                part_property_after_refinement = new ElementProperty<double>(partition->get_coords(n[0]), partition->get_coords(n[1]), partition->get_coords(n[2]), partition->get_coords(n[3]));
                            }

                            break;
                        }*/

                        //triangulate_time = omp_get_wtime() - triangulate_tic;
                        call_to_refine_time = omp_get_wtime() - call_to_refine_tic;
                        /*
                        std::cout << "swapping" << std::endl;
                        Swapping<double,2> swapping(*partition);
                        swapping.swap(0.45);


                        //Output swapped partition in each iteration
                        std::cout << "  debug mesh output swapped partition " << part_id << " iteration " << (act_iter+1)<< std::endl;
                        std::string vtu_filename_swapped_output = "examples/data/color_refinement/output/swapped_part";
                        vtu_filename_swapped_output+=std::to_string(part_id);
                        vtu_filename_swapped_output+="_iteration";
                        vtu_filename_swapped_output+=std::to_string(act_iter+1);
                        vtu_filename_swapped_output+=".vtu";
                        VTKTools<double>::export_vtu(vtu_filename_swapped_output.c_str(), partition);//*/


                        l2g_vertex[part_id] = l2g_vertices_tmp;
                        g2l_vertex[part_id] = g2l_vertices_tmp;
                        l2g_element[part_id] = l2g_elements_tmp;
                        g2l_element[part_id] = g2l_elements_tmp;
                        outboxes[part_id]=outbox_data;
                    }

                    else
                    {
                        Refine<double,3> refiner(*partition);
                        double L_max = std::stod(options); //standard was 0.00005
                        std::cout << "  L_max: " << L_max << std::endl;

                        double call_to_refine_tic = omp_get_wtime();

                        //if (color == 0)
                        {
                            /*refiner.refine(0.0005, nodes_partition_ids, l2g_vertices_tmp, g2l_vertices_tmp, part_id, outbox_data, 
                                        partition_colors, partition_adjcy[part_id], previous_nelements[part_id]);//*/
                            //std::cout << " refining partition " << part_id << " with " << partition->get_number_elements() << " elements and ";
                            //std::cout << partition->get_number_nodes() << " nodes" << std::endl;
                            //std::cout << "NElements_before_healing " << NElements_before_healing << " NNodes_before_healing " << NNodes_before_healing << std::endl;
                            //std::cout << " 3d refining partition " << part_id << " with thread " << omp_get_thread_num() << std::endl;
                            refiner.refine(L_max, nodes_partition_ids, l2g_vertices_tmp, part_id, outbox_data, 
                                            partition_colors, partition_adjcy[part_id], previous_nelements[part_id],
                                            NNInterfaces_tmp, global_NNodes, g2l_vertices_tmp, NNodes_before_healing, NElements_before_healing,
                                            /*FInterfaces_tmp,*/act_iter+1);//*/
                            //std::cout << " partition has now " << partition->get_number_elements() << " elements and " << partition->get_number_nodes() << " nodes" << std::endl;
                        }
                        /*
                        //Output refined partition in each iteration
                        std::cout << "  debug mesh output refined partition " << part_id << " iteration " << (act_iter+1)<< std::endl;
                        std::string vtu_filename_refine_output = "examples/data/color_refinement/output/refine_part";
                        vtu_filename_refine_output+=std::to_string(part_id);
                        vtu_filename_refine_output+="_iteration";
                        vtu_filename_refine_output+=std::to_string(act_iter+1);
                        vtu_filename_refine_output+=".vtu";
                        VTKTools<double>::export_vtu(vtu_filename_refine_output.c_str(), partition);//*/

                        call_to_refine_time = omp_get_wtime() - call_to_refine_tic;

                        l2g_vertex[part_id] = l2g_vertices_tmp;
                        g2l_vertex[part_id] = g2l_vertices_tmp;
                        l2g_element[part_id] = l2g_elements_tmp;
                        g2l_element[part_id] = g2l_elements_tmp;
                        outboxes[part_id]=outbox_data; 
                    } 
/*
                    //DEBUG
                    std::ofstream nninterfaces_out;
                    std::string name;
                    name = "nninterfaces_after_refinement_";
                    name += std::to_string(part_id);
                    name += ".txt";
                    nninterfaces_out.open(name.c_str());

                    for (size_t i = 0; i < NNInterfaces_tmp.size(); ++i)
                    {
                        nninterfaces_out << "Node " << i << " has (NNInterfaces.size()) " << NNInterfaces_tmp[i].size() << " neighbors; (NNList.size(): " << partition->NNList[i].size() << ")" << std::endl;

                        for (size_t j = 0; j < NNInterfaces_tmp[i].size(); ++j)
                        {
                            nninterfaces_out << " " << partition->NNList[i][j] << ": ";
                            for (size_t k = 0; k < NNInterfaces_tmp[i][j].size(); ++k)
                            {
                                nninterfaces_out << " " << NNInterfaces_tmp[i][j][k] << std::endl;
                            }
                        }
                    }

                    nninterfaces_out.close();//*/
/*
                    std::ofstream nnlist;
                    std::string nnlist_name = "nnlist_after_refinement_partition";
                    nnlist_name += std::to_string(part_id);
                    nnlist_name += "_iteration";
                    nnlist_name += std::to_string(act_iter+1);
                    nnlist_name+=".txt";

                    nnlist.open(nnlist_name.c_str());

                    for (size_t i = 0; i < partition->NNList.size(); ++i)
                    {
                        nnlist << "Vertex " << i << " has " << partition->NNList[i].size() << " neighbors" << std::endl;

                        for (size_t j = 0; j < partition->NNList[i].size(); ++j)
                        {
                            nnlist << " " << partition->NNList[i][j] << std::endl;
                        }
                    }

                    nnlist.close();
                    //END OF DEBUG*/
                } //end of if (algorithm==pragmatic)

                else if (algorithm == "triangle")
                {       
                    //std::cout << "REWRITE triunsuitable, SEE TRIANGLE.H AND TRIANGLE.C FOR DETAILS!!!" << std::endl;

                    struct triangulateio tri_partition, tri_out;
                    //auto tri_ds_tic = std::chrono::system_clock::now();
                    //init tri_partition
                    tri_partition.numberofpoints = partition->get_number_nodes();
                    tri_partition.numberofpointattributes = 0;
                    tri_partition.pointmarkerlist = NULL;
                    tri_partition.pointlist = (REAL *) malloc(tri_partition.numberofpoints * 2 * sizeof(REAL) );
                    //tri_partition.pointlist = partition->get_coords_pointer();

                    for (size_t i = 0; i < tri_partition.numberofpoints; ++i)
                    {
                        tri_partition.pointlist[2*i] = partition->get_coords(i)[0];
                        tri_partition.pointlist[2*i+1] = partition->get_coords(i)[1];
                    }  //*/ 
            
                    tri_partition.numberoftriangles = partition->get_number_elements();
                    tri_partition.numberofcorners = 3;
                    tri_partition.numberoftriangleattributes = 0;
                    tri_partition.trianglelist = (int*) malloc ( tri_partition.numberoftriangles * 3 * sizeof(int) );
                    //tri_partition.trianglelist = partition->get_enlist_pointer();

                    for (size_t i = 0; i < tri_partition.numberoftriangles; ++i)
                    {
                        const int *element_ptr = nullptr;
                        element_ptr = partition->get_element(i);
                        
                        tri_partition.trianglelist[3*i+0] = *(element_ptr++);
                        tri_partition.trianglelist[3*i+1] = *(element_ptr++);
                        tri_partition.trianglelist[3*i+2] = *(element_ptr++);
                    }
                    //end of init tri_partition//*/

                    //init tri_out
                    tri_out.pointlist = (REAL *) NULL;
                    tri_out.pointmarkerlist = (int *) NULL;
                    tri_out.pointattributelist = (REAL *) NULL;
                    tri_out.trianglelist = (int *) NULL;
                    tri_out.numberofpoints = 0;
                    tri_out.numberofpointattributes = 0;

                    tri_out.triangleattributelist = (REAL *) NULL;
                    tri_out.neighborlist = (int *) NULL;
                    tri_out.segmentlist = (int *) NULL;
                    tri_out.segmentmarkerlist = (int *) NULL;
                    tri_out.edgelist = (int *) NULL;
                    tri_out.edgemarkerlist = (int *) NULL;

                    tri_out.trianglearealist = NULL;
                    tri_out.numberoftriangles = 0;
                    tri_out.numberofcorners = 0;
                    tri_out.numberoftriangleattributes = 0;
                    tri_out.numberofsegments = 0;

                    tri_out.holelist = NULL;
                    tri_out.numberofholes = 0;

                    tri_out.regionlist = NULL;
                    tri_out.numberofregions = 0;

                    tri_out.edgelist = NULL;
                    tri_out.edgemarkerlist = NULL;
                    tri_out.normlist = NULL;
                    tri_out.numberofedges = 0;
                    //end of init tri_out*/

                    //std::chrono::duration<double> tri_ds_dur = std::chrono::system_clock::now() - tri_ds_tic;
                    //tri_ds_time = tri_ds_dur.count();

                    //copy options string
                    char * options_buffer = new char[options.length()+1];
                    //std::string options = options;
                    //char * options_buffer = new char[1];
                    //viennamesh::error(1) << "uncomment the command three lines above and comment the two lines above to make it running again with triangle!" << std::endl;
                    std::strcpy(options_buffer, options.c_str());

                    //triangulate
                    auto triangulate_tic = omp_get_wtime();
                    //viennamesh::info(1) << "Refining partition " << part_id << " with options " << options_buffer << std::endl;
                    triangulate (options_buffer, &tri_partition, &tri_out, NULL);
                    call_to_refine_time = omp_get_wtime() - triangulate_tic;
                    //end of triangulate*/

                    //free all memory (ONLY FREE IF MEMORY HAS BEEN ALLOCATED, NOT IF POINTERS ARE USED)
                    free(tri_partition.pointlist);
                    free(tri_partition.trianglelist);
                    free(tri_partition.pointmarkerlist);//*/
                    //free(&tri_partition);

                    triangle_partitions[part_id] = tri_out;

                    /*free(tri_out.pointlist);
                    free(tri_out.trianglelist);
    */
                    delete[] options_buffer;
                    //end of free all memory                
                } //end of (algorithm==triangle)

                //Tetgen
                else if (algorithm == "tetgen")
                {
                    bool old = false;
                    //old tetgen version from 2018-10-04
                    if (old)
                    {
                        //tetgenio in, out;
                        tetgenio in;

                        //out.initialize();

                        in.firstnumber = 0;
                        in.numberofpoints = partition->get_number_nodes();
                        /*
                        in.pointlist = new REAL[in.numberofpoints * 3];
                        
                        for (size_t i = 0; i < in.numberofpoints; ++i)
                        {
                            in.pointlist[3*i]   = partition->get_coords(i)[0];
                            in.pointlist[3*i+1] = partition->get_coords(i)[1];
                            in.pointlist[3*i+2] = partition->get_coords(i)[2];
                        }//*/
                        /*
                        std::cout << "Address of in.pointlist[0] : " << &in.pointlist[0] << std::endl;
                        std::cout << "Address of _coords[0]      : " << &partition->_coords[0] << std::endl;
                        std::cout << "Address of tetrahedronlist : " << &in.tetrahedronlist[0] << std::endl;
                        std::cout << "Address of _ENList[0]      : " << &partition->_ENList[0] << std::endl; //*/

                        in.pointlist = &(partition->_coords[0]);

                        in.numberoftetrahedra = partition->get_number_elements();
                        /*
                        in.tetrahedronlist = new int[in.numberoftetrahedra*4];

                        for (size_t i = 0; i < in.numberoftetrahedra; ++i)
                        {
                            const int *n = partition->get_element(i);
                            in.tetrahedronlist[4*i]   = n[0]; 
                            in.tetrahedronlist[4*i+1] = n[1];
                            in.tetrahedronlist[4*i+2] = n[2];
                            in.tetrahedronlist[4*i+3] = n[3];  
                        }//*/

                        in.tetrahedronlist = &partition->_ENList[0];
/*
                        std::cout << "Address of in.pointlist[0] : " << &in.pointlist[0] << std::endl;
                        std::cout << "Address of _coords[0]      : " << &partition->_coords[0] << std::endl;
                        std::cout << "Address of tetrahedronlist : " << &in.tetrahedronlist[0] << std::endl;
                        std::cout << "Address of _ENList[0]      : " << &partition->_ENList[0] << std::endl; //*/
                        /*
                        in.numberoffacets = partition->get_number_elements();

                        in.facetlist = new tetgenio::facet[in.numberoffacets];
                        
                        for (size_t i = 0; i < in.numberoffacets; ++i)
                        {
                            tetgenio::facet & facet = in.facetlist[i];
                            facet.holelist = 0;      
                            
                            facet.numberofpolygons = 1;
                            facet.polygonlist = new tetgenio::polygon[facet.numberofpolygons];
                            facet.numberofholes = 0;
                            facet.holelist = NULL;

                            tetgenio::polygon & polygon = facet.polygonlist[0];
                            polygon.numberofvertices = 4;
                            polygon.vertexlist = new int[polygon.numberofvertices];

                            const int *element_ptr = nullptr;
                            element_ptr = partition->get_element(i);

                            polygon.vertexlist[0]   = *(element_ptr++);
                            polygon.vertexlist[1] = *(element_ptr++);
                            polygon.vertexlist[2] = *(element_ptr++);
                            polygon.vertexlist[3] = *(element_ptr++);
                        }//*/
                        
                        tetgenbehavior tet_behavior;

                        tet_behavior.parse_commandline(const_cast<char*>(options.c_str()));
                        
                        //std::cout << " Tetgen Options: " << options << std::endl;

                        auto triangulate_tic = omp_get_wtime();
                        //tetrahedralize(&tet_behavior, &in, &out);
                        tetrahedralize(&tet_behavior, &in, &tetgen_partitions[part_id]);

                        call_to_refine_time = omp_get_wtime() - triangulate_tic;

                        in.pointlist = NULL;
                        in.tetrahedronlist = NULL;

                        //tetgen_partitions[part_id] = new tetgenio (*out);

                        //in.deinitialize(false);
                    } //end of if(old) - old tetgen version of 2018-10-04

                    //Do boundary/interface refinement with Pragmatic.
                    //Afterwards refine the interior using TetGen with switch -Y!
                    else
                    {
                        /*std::cout << " Pragmatic Refinement of Boundary/Interface Facets" << std::endl;
                        std::cout << " Interior Refinement with TetGen" << std::endl;*/
                        /*
                        //create boundary nodes vector
                        std::vector<int> boundary_nodes(partition->get_number_nodes(), 0);

                        for(index_t i=0; i<partition->get_number_elements(); i++) 
                        {
                            const int *n=partition->get_element(i);
                            if(n[0]==-1)
                                continue;

                            //loop over all vertices of a tetrahedron
                            for(int j=0; j<4; j++) 
                            {
                                boundary_nodes[n[(j+1)%4]] = std::max(boundary_nodes[n[(j+1)%4]], partition->boundary[i*4+j]);
                                boundary_nodes[n[(j+2)%4]] = std::max(boundary_nodes[n[(j+2)%4]], partition->boundary[i*4+j]);
                                boundary_nodes[n[(j+3)%4]] = std::max(boundary_nodes[n[(j+3)%4]], partition->boundary[i*4+j]);
                            }
                        }//*/

                        //Here do the pragmatic refinement
                        Refine<double,3> refiner(*partition);
                        double L_max = 0.00005;
                        std::cout << "  Include command line option to provide argument for L_max!" << std::endl;
                        std::cout << "  L_max: " << L_max << std::endl; //standard was 0.00005

                        auto refine_boundary_tic = omp_get_wtime();
                        refiner.refine_boundary(L_max, nodes_partition_ids, l2g_vertices_tmp, part_id, outbox_data, 
                                                partition_colors, partition_adjcy[part_id], previous_nelements[part_id],
                                                NNInterfaces_tmp, global_NNodes, g2l_vertices_tmp, NNodes_before_healing, 
                                                NElements_before_healing, act_iter+1); //*/
                        refine_boundary_time = omp_get_wtime() - refine_boundary_tic;

                        auto defrag_tic = omp_get_wtime();
                        partition->defragment();
                        defrag_time = omp_get_wtime() - defrag_tic;

                        l2g_vertex[part_id] = l2g_vertices_tmp;
                        g2l_vertex[part_id] = g2l_vertices_tmp;
                        l2g_element[part_id] = l2g_elements_tmp;
                        g2l_element[part_id] = g2l_elements_tmp;
                        outboxes[part_id] = outbox_data; 
                        /*
                        //Output refined partition in each iteration
                        std::cout << "  debug mesh output refined partition " << part_id << " iteration " << (act_iter+1)<< std::endl;
                        std::string vtu_filename_refine_output = "examples/data/color_refinement/output/refine_boundary_part";
                        vtu_filename_refine_output+=std::to_string(part_id);
                        vtu_filename_refine_output+="_iteration";
                        vtu_filename_refine_output+=std::to_string(act_iter+1);
                        vtu_filename_refine_output+=".vtu";
                        VTKTools<double>::export_vtu(vtu_filename_refine_output.c_str(), partition);//*/

                        viennamesh::info(3) << "   Partition " << part_id << " has " << partition->get_number_nodes() << " Vertices and " << partition->get_number_elements() << " elements after boundary refinement" << std::endl;
                        
                        if ( act_iter == (max_num_iterations-1) )
                        {
                            /*
                            //Output refined partition in each iteration
                            std::cout << "  debug mesh output refined partition " << part_id << " iteration " << (act_iter+1)<< std::endl;
                            std::string vtu_filename_refine_output = "examples/data/color_refinement/output/refine_boundary_part";
                            vtu_filename_refine_output+=std::to_string(part_id);
                            vtu_filename_refine_output+="_iterations";
                            vtu_filename_refine_output+=std::to_string(act_iter+1);
                            vtu_filename_refine_output+=".vtu";
                            VTKTools<double>::export_vtu(vtu_filename_refine_output.c_str(), partition);//*/

                            //Create TetGen objects
                            tetgenio in;

                            in.firstnumber = 0;

                            in.numberofpoints = partition->get_number_nodes();
                            in.pointlist = &(partition->_coords[0]);

                            in.numberoftetrahedra = partition->get_number_elements();
                            in.tetrahedronlist = &partition->_ENList[0];

                            tetgenbehavior tet_behavior;

                            tet_behavior.parse_commandline(const_cast<char*>(options.c_str()));
                            
                            //std::cout << " Tetgen Options: " << options << std::endl;

                            auto tetrahedralize_tic = omp_get_wtime();
                            //tetrahedralize(&tet_behavior, &in, &out);
                            //viennamesh::info(3) << "  Tetrahedralize Partition " << part_id << std::endl;
                            tetrahedralize(&tet_behavior, &in, &tetgen_partitions[part_id]);
                            
                            call_to_refine_time = omp_get_wtime() - tetrahedralize_tic;

                            //tetgen_partitions[part_id] = in;

                            in.pointlist = NULL;
                            in.tetrahedronlist = NULL;
                        }
                    } //end of else

                } //end of tetgen

                //Pragmatic cavity
                else if (algorithm == "pragmaticcavity")
                {
                    Refine_Cavity<double,3> refiner_cavity(*partition);
                    double L_max = std::stod(options);
                    std::cout << "  L_max: " << L_max << std::endl;

                    double call_to_refine_tic = omp_get_wtime();

                    //std::cout << "NElements before refinement " <<  partition->get_number_elements() << " NVertices before refinement " << partition->get_number_nodes() << std::endl;
                    /*
                    //DEBUG
                    std::cout << "  debug mesh output input partition before refinement " << part_id << " iteration " << (act_iter+1)<< std::endl;
                    std::string vtu_filename = "examples/data/color_refinement/output/input_part_before_refinement_";
                    vtu_filename+=std::to_string(part_id);
                    vtu_filename+="_iteration";
                    vtu_filename+=std::to_string(act_iter);
                    vtu_filename+=".vtu";
                    VTKTools<double>::export_vtu(vtu_filename.c_str(), partition);
                    //END OF DEBUG*/

                    //double L_max = 0.00005;
                    //double L_max = 0.00005; //standard in MY IMPLEMENTATION

                    refiner_cavity.refine(L_max, nodes_partition_ids, l2g_vertices_tmp, part_id, outbox_data, 
                                          partition_colors, partition_adjcy[part_id], previous_nelements[part_id],
                                          NNInterfaces_tmp, global_NNodes, g2l_vertices_tmp, NNodes_before_healing, NElements_before_healing,
                                          act_iter+1);

                    call_to_refine_time = omp_get_wtime() - call_to_refine_tic;

                    l2g_vertex[part_id] = l2g_vertices_tmp;
                    g2l_vertex[part_id] = g2l_vertices_tmp;
                    l2g_element[part_id] = l2g_elements_tmp;
                    g2l_element[part_id] = g2l_elements_tmp;
                    outboxes[part_id]=outbox_data; 

                    /*
                    //DEBUG
                    //Output refined partition in each iteration
                    std::cout << "  debug mesh output refined partition " << part_id << " iteration " << (act_iter+1)<< std::endl;
                    std::string vtu_filename_refine_output = "examples/data/color_refinement/output/refine_boundary_part";
                    vtu_filename_refine_output+=std::to_string(part_id);
                    vtu_filename_refine_output+="_iteration";
                    vtu_filename_refine_output+=std::to_string(act_iter+1);
                    vtu_filename_refine_output+=".vtu";
                    VTKTools<double>::export_vtu(vtu_filename_refine_output.c_str(), partition);
                    //END OF DEBUG*/

                    //std::cout << "NElements after refinement " <<  partition->get_number_elements() << " NVertices after refinement " << partition->get_number_nodes() << std::endl;
                } //end of pragmaticcavity

                auto refine_toc = omp_get_wtime();
                //std::cerr << refine_toc - refine_tic << std::endl;
            
                auto threads_toc = omp_get_wtime();

                threads_log[omp_get_thread_num()] += threads_toc - threads_tic;
                mesh_log[omp_get_thread_num()] += mesh_toc - mesh_tic;
                heal_log[omp_get_thread_num()] += heal_toc - heal_tic;
                metric_log[omp_get_thread_num()] += metric_toc - metric_tic;
                call_refine_log[omp_get_thread_num()] += call_to_refine_time;
                refine_log[omp_get_thread_num()] += refine_toc - refine_tic;
                nodes_log[omp_get_thread_num()] += nodes_toc - nodes_tic;
                enlist_log[omp_get_thread_num()] += enlist_toc - enlist_tic;
                ++workload[omp_get_thread_num()];
                workload_elements[omp_get_thread_num()] += partition->get_number_elements();
                get_interfaces_log[omp_get_thread_num()] += interfaces_toc - interfaces_tic;
                defrag_log[omp_get_thread_num()] += defrag_time;
                refine_boundary_log[omp_get_thread_num()] += refine_boundary_time;

                //build_tri_ds[omp_get_thread_num()] += tri_ds_time;
                //int_check_log[omp_get_thread_num()] += int_check_time;
                /*
                //DEBUG
                //output .node and .ele files for tetgen
                ofstream node_file;
                std::string node_file_name = "examples/data/color_refinement/output/tetgen_input_part_";
                node_file_name+=std::to_string(part_id);
                node_file_name+="_iteration";
                node_file_name+=std::to_string(act_iter+1);
                node_file_name+=".node";

                node_file.open(node_file_name.c_str());

                node_file << "# Node count, " << partition->get_number_dimensions() << " dim, no attribute, no boundary marker" << std::endl;
                node_file << tetgen_partitions[part_id].numberofpoints << " " << partition->get_number_dimensions() << " 0 0" << std::endl;
                
                for (size_t vid = 0; vid < tetgen_partitions[part_id].numberofpoints; ++vid)
                {
                    node_file << vid+1 << " " << tetgen_partitions[part_id].pointlist[3*vid] << " " << tetgen_partitions[part_id].pointlist[3*vid+1];
                    node_file << " " << tetgen_partitions[part_id].pointlist[3*vid+2] << std::endl;
                }
                node_file.close();

                ofstream ele_file;
                std::string ele_file_name = "examples/data/color_refinement/output/tetgen_input_part_";
                ele_file_name+=std::to_string(part_id);
                ele_file_name+="_iteration";
                ele_file_name+=std::to_string(act_iter+1);
                ele_file_name+=".ele";

                ele_file.open(ele_file_name.c_str());

                ele_file << "# Tetrahedra count, 4 nodes per tetrahedron, no region attribute" << std::endl;
                ele_file << tetgen_partitions[part_id].numberoftetrahedra << " 4 0" << std::endl;
                
                for (size_t vid = 0; vid < tetgen_partitions[part_id].numberoftetrahedra; ++vid)
                {
                    if (tetgen_partitions[part_id].tetrahedronlist[4*vid] == -1)
                    {
                        std::cerr << " ERROR: no defragmentation of tetrahedronlist detected!" << std::endl;
                        continue;
                    }

                    ele_file << vid+1 << " " << tetgen_partitions[part_id].tetrahedronlist[4*vid]+1 << " " << tetgen_partitions[part_id].tetrahedronlist[4*vid+1]+1;
                    ele_file << " " << tetgen_partitions[part_id].tetrahedronlist[4*vid+2]+1 << " " << tetgen_partitions[part_id].tetrahedronlist[4*vid+3]+1 << std::endl;
                }
                ele_file.close();

                //end of output .node and .ele files for tetgen
                //END OF DEBUG*/

            }//end parallel for loop
        } //end for loop colors - iterate colors
        //std::cout << std::endl;
    } //end of number_iterations

    auto for_toc = omp_get_wtime();

    for_time = for_toc - for_tic;
/*
    //DEBUG
    for (size_t col_it=0; col_it < partition_colors.size(); ++col_it)
    {
        std::cout << "partition " << col_it << " has color " << partition_colors[col_it] << std::endl;
    }
    //END OF DEBUG*/

    viennamesh::info(1) << "Successfully adapted the mesh" << std::endl;
   /*
    for(size_t i =0; i < pragmatic_partitions.size(); ++i)
    {
        std::cerr << "partition " << i << ": " << std::endl;
        std::cerr << "  " << pragmatic_partitions[i]->get_number_nodes() << std::endl;
        std::cerr << "  " << pragmatic_partitions[i]->get_number_elements() << std::endl;
    }
*/
    return true;
}
//end of CreatePragmaticDataStructures_par

//WritePartitions
//
//Tasks: Writes Partitions
bool MeshPartitions::WritePartitions()
{
    viennamesh::info(1) << "Write " << pragmatic_partitions.size() << " partitions" << std::endl;

    //write partitions
    #pragma omp parallel for num_threads(8)
    for (size_t i = 0; i < pragmatic_partitions.size(); ++i)
    {
        std::cout << "Writing partition " << i << std::endl;
        std::cout << "  Vertex count = " << pragmatic_partitions[i]->get_number_nodes() << std::endl;
        std::cout << "  Cell count = " << pragmatic_partitions[i]->get_number_elements() << std::endl;
        
        std::string filename;
        filename += "examples/data/color_refinement/output/";
        filename += "output_";
        filename += "partition_";
        filename += std::to_string( i );
        
        //VTKTools<double>::export_vtu(filename.c_str(), pragmatic_partitions[i]);
    }

  return true;
} 
//end of WritePartitions

//WriteMergedMesh
//
//Tasks: Merges all mesh partitions and writes a single mesh file onto disk
bool MeshPartitions::WriteMergedMesh(std::string filename)
{
    std::cerr << "WriteMergedMesh()" << std::endl;
    return true;
}
//end of WriteMergedMesh

//GetRefinementStats
//
//Tasks: Returns the number of vertices and elements for a partitioned mesh
void MeshPartitions::GetRefinementStats(int* nodes, int* elements, std::string algorithm)
{
    int tmp_nodes = 0;
    int tmp_elements = 0;

    if (algorithm == "pragmatic" || algorithm == "pragmaticcavity")
    {
        for (size_t i =0; i < pragmatic_partitions.size(); ++i)
        {
            tmp_nodes += pragmatic_partitions[i]->get_number_nodes();
            tmp_elements += pragmatic_partitions[i]->get_number_elements();
        }
    }

    else if (algorithm == "triangle")
    {
        for (size_t i =0; i < triangle_partitions.size(); ++i)
        {
            tmp_nodes += triangle_partitions[i].numberofpoints;
            tmp_elements += triangle_partitions[i].numberoftriangles;
        }
    }

    else if (algorithm == "tetgen")
    {
        for (size_t i = 0; i < tetgen_partitions.size(); ++i)
        {
            tmp_nodes += tetgen_partitions[i].numberofpoints;
            tmp_elements += tetgen_partitions[i].numberoftetrahedra;
        }
    }

    *nodes = tmp_nodes;
    *elements = tmp_elements;
}
//end of GetRefinementStats

//MeshPartitions::edgeNumber
//
//Tasks: Returns edge number
int MeshPartitions::edgeNumber(Mesh<double>*& partition, index_t eid, index_t v1, index_t v2)
{
    const int *n=partition->get_element(eid);

    auto dim = partition->get_number_dimensions();

    if(dim==2) {
        /* In 2D:
            * Edge 0 is the edge (n[1],n[2]).
            * Edge 1 is the edge (n[0],n[2]).
            * Edge 2 is the edge (n[0],n[1]).
            */
        if(n[1]==v1 || n[1]==v2) {
            if(n[2]==v1 || n[2]==v2)
                return 0;
            else
                return 2;
        } else
            return 1;
    } else { //if(dim=3)
        /*
            * In 3D:
            * Edge 0 is the edge (n[0],n[1]).
            * Edge 1 is the edge (n[0],n[2]).
            * Edge 2 is the edge (n[0],n[3]).
            * Edge 3 is the edge (n[1],n[2]).
            * Edge 4 is the edge (n[1],n[3]).
            * Edge 5 is the edge (n[2],n[3]).
            */
        if(n[0]==v1 || n[0]==v2) {
            if(n[1]==v1 || n[1]==v2)
                return 0;
            else if(n[2]==v1 || n[2]==v2)
                return 1;
            else
                return 2;
        } else if(n[1]==v1 || n[1]==v2) {
            if(n[2]==v1 || n[2]==v2)
                return 3;
            else
                return 4;
        } else
            return 5;
    }
} //end of MeshPartitions::edgeNumber

//MeshPartitions::heal2D_1(Mesh<double>*& partition, const index_t *newVertex, int eid, int nloc, int& splitCnt, int& threadIdx)
//
//Task: Heals mesh after neighboring partition has altered its interface
void MeshPartitions::heal2D_1(Mesh<double>*& partition, const index_t *newVertex, int eid, int nloc, int& splitCnt, int& threadIdx)
{
    // Single edge split.
    //std::cout << " heal1 element " << eid << std::endl;
    const int *n=partition->get_element(eid);
    const int *boundary=&(partition->boundary[eid*nloc]);

    int rotated_ele[3];
    int rotated_boundary[3];
    index_t vertexID = -1;
    for(int j=0; j<3; j++)
        if(newVertex[j] >= 0) {
            vertexID = newVertex[j];

            rotated_ele[0] = n[j];
            rotated_ele[1] = n[(j+1)%3];
            rotated_ele[2] = n[(j+2)%3];
            
            rotated_boundary[0] = boundary[j];
            rotated_boundary[1] = boundary[(j+1)%3];
            rotated_boundary[2] = boundary[(j+2)%3];
            //*/
            /*
            rotated_boundary[0] = 0;
            rotated_boundary[1] = 0;
            rotated_boundary[2] = 0;
            //*/
            break;
        }
    assert(vertexID!=-1);

    const index_t ele0[] = {rotated_ele[0], rotated_ele[1], vertexID};
    const index_t ele1[] = {rotated_ele[0], vertexID, rotated_ele[2]};

    const index_t ele0_boundary[] = {rotated_boundary[0], 0, rotated_boundary[2]};
    const index_t ele1_boundary[] = {rotated_boundary[0], rotated_boundary[1], 0};

    index_t ele1ID;
    ele1ID = splitCnt;

    //std::cout << "ele1ID " << ele1ID << std::endl;
    
    partition->add_nnlist(vertexID, rotated_ele[0]);
    //std::cout << "  add nnlist " << vertexID << " " << rotated_ele[0] << std::endl;
    partition->add_nnlist(rotated_ele[0], vertexID);
    //std::cout << "  add nnlist " << rotated_ele[0] << " " << vertexID << std::endl;

    partition->add_nelist_fix(rotated_ele[0], ele1ID, threadIdx);
    //std::cout << "  add nelist fix " << rotated_ele[0] << " " << ele1ID << " " << threadIdx << std::endl;

    partition->add_nelist(vertexID, eid);
    //std::cout << "  add nelist " << vertexID << " " << eid << std::endl;
    partition->add_nelist_fix(vertexID, ele1ID, threadIdx);
    //std::cout << "  add nelist fix " << vertexID << " " << ele1ID << " " << threadIdx << std::endl;

    partition->remove_nelist(rotated_ele[2], eid);
    //std::cout << "  remove nelist " << rotated_ele[2] << " " << eid << std::endl;
    partition->add_nelist_fix(rotated_ele[2], ele1ID, threadIdx);
    //std::cout << "  add nelist fix " << rotated_ele[2] << " " << ele1ID << " " << threadIdx << std::endl;

    partition->replace_element(eid, ele0);
    //std::cout << "  replace element " << eid << " " << ele0[1] << " " << ele0[1] << " " << ele0[2] << std::endl;
    partition->append_element(ele1);
    //std::cout << "  append element " << ele1[0] << " " << ele1[1] << " " << ele1[2] << std::endl;

    splitCnt+=1;
}
//end of MeshPartitions::heal2D_1(Mesh<double>*& partition, const index_t *newVertex, int eid, int nloc, int& splitCnt, int& threadIdx)

//MeshPartitions::heal2D_2(Mesh<double>*& partition, const index_t *newVertex, int eid, int nloc, int& splitCnt, int& threadIdx)
//
//Task: Heals mesh after neighboring partition has altered its interface
void MeshPartitions::heal2D_2(Mesh<double>*& partition, const index_t *newVertex, int eid, int nloc, int& splitCnt, int& threadIdx)
{
    //std::cout << " heal2 " << eid << std::endl;
    const int *n=partition->get_element(eid);
    const int *boundary=&(partition->boundary[eid*nloc]);

    int rotated_ele[3]; 
    int rotated_boundary[3];
    index_t vertexID[2];

    for(int j=0; j<3; j++) 
    {
        if(newVertex[j] < 0) 
        {
            vertexID[0] = newVertex[(j+1)%3];
            vertexID[1] = newVertex[(j+2)%3];

            rotated_ele[0] = n[j];
            rotated_ele[1] = n[(j+1)%3];
            rotated_ele[2] = n[(j+2)%3];

            rotated_boundary[0] = boundary[j];
            rotated_boundary[1] = boundary[(j+1)%3];
            rotated_boundary[2] = boundary[(j+2)%3];

            //*/
            /*
            rotated_boundary[0] = 0;
            rotated_boundary[1] = 0;
            rotated_boundary[2] = 0;
            //*/

            break;
        }
    }  

    double ldiag0 = partition->calc_edge_length(rotated_ele[1], vertexID[0]);
    double ldiag1 = partition->calc_edge_length(rotated_ele[2], vertexID[1]);

    const int offset = ldiag0 < ldiag1 ? 0 : 1;

    const index_t ele0[] = {rotated_ele[0], vertexID[1], vertexID[0]};
    const index_t ele1[] = {vertexID[offset], rotated_ele[1], rotated_ele[2]};
    const index_t ele2[] = {vertexID[0], vertexID[1], rotated_ele[offset+1]};

    const index_t ele0_boundary[] = {0, rotated_boundary[1], rotated_boundary[2]};
    const index_t ele1_boundary[] = {rotated_boundary[0], (offset==0)?rotated_boundary[1]:0, (offset==0)?0:rotated_boundary[2]};
    const index_t ele2_boundary[] = {(offset==0)?rotated_boundary[2]:0, (offset==0)?0:rotated_boundary[1], 0};

    index_t ele0ID, ele2ID;
    ele0ID = splitCnt;
    ele2ID = ele0ID+1;

    // NNList: Connect vertexID[0] and vertexID[1] with each other
    partition->add_nnlist(vertexID[0], vertexID[1]);
    //std::cout << "  add nnlist " << vertexID[0] << " " << vertexID[1] << std::endl;
    partition->add_nnlist(vertexID[1], vertexID[0]);
    //std::cout << "  add nnlist " << vertexID[1] << " " << vertexID[0] << std::endl;

    // vertexID[offset] and rotated_ele[offset+1] are the vertices on the diagonal
    partition->add_nnlist(vertexID[offset], rotated_ele[offset+1]);
    //std::cout << "  add nnlist " << vertexID[offset] << " " << rotated_ele[offset+1] << std::endl;
    partition->add_nnlist(rotated_ele[offset+1], vertexID[offset]);
    //std::cout << "  add nnlist " << rotated_ele[offset+1] << " " << vertexID[offset] << std::endl;

    // rotated_ele[offset+1] is the old vertex which is on the diagonal
    // Add ele2 in rotated_ele[offset+1]'s NEList
    partition->add_nelist_fix(rotated_ele[offset+1], ele2ID, threadIdx);
    //std::cout << "  add nelist fix " << rotated_ele[offset+1] << " " << ele2ID << " " << threadIdx << std::endl;

    // Replace eid with ele0 in NEList[rotated_ele[0]]
    partition->remove_nelist(rotated_ele[0], eid);
    //std::cout << "  remove nelist " << rotated_ele [0] << " " << eid << std::endl;
    partition->add_nelist_fix(rotated_ele[0], ele0ID, threadIdx);
    //std::cout << "  add nelist fix " << rotated_ele[0] << " " << ele0ID << " " << threadIdx << std::endl;

    // Put ele0, ele1 and ele2 in vertexID[offset]'s NEList
    partition->add_nelist(vertexID[offset], eid);
    //std::cout << "  add nelist " << vertexID[offset] << " " << eid << std::endl;
    partition->add_nelist_fix(vertexID[offset], ele0ID, threadIdx);
    //std::cout << "  add nelist fix " << vertexID[offset] << " " << ele0ID << " " << threadIdx << std::endl;
    partition->add_nelist_fix(vertexID[offset], ele2ID, threadIdx);
    //std::cout << "  add nelist fix " << vertexID[offset] << " " << ele2ID << " " << threadIdx << std::endl;

    // vertexID[(offset+1)%2] is the new vertex which is not on the diagonal
    // Put ele0 and ele2 in vertexID[(offset+1)%2]'s NEList
    partition->add_nelist_fix(vertexID[(offset+1)%2], ele0ID, threadIdx);
    //std::cout << "  add nelist fix " << vertexID[(offset+1)%2] << " " << ele0ID << " " << threadIdx << std::endl;
    partition->add_nelist_fix(vertexID[(offset+1)%2], ele2ID, threadIdx);
    //std::cout << "  add nelist fix " << vertexID[(offset+1)%2] << " " << ele2ID << " " << threadIdx << std::endl;

    partition->replace_element(eid, ele1);
    //std::cout << "  replace element " << eid << " " << ele1[1] << " " << ele1[1] << " " << ele1[2] << std::endl;
    partition->append_element(ele0);
    //std::cout << "  append element " << ele0[0] << " " << ele0[1] << " " << ele0[2] << std::endl;
    partition->append_element(ele2);
    //std::cout << "  append element " << ele2[0] << " " << ele2[1] << " " << ele2[2] << std::endl;

    splitCnt += 2;
}
//end of MeshPartitions::heal2D_2(Mesh<double>*& partition, const index_t *newVertex, int eid, int nloc, int& splitCnt, int& threadIdx)

//MeshPartitions::heal_facet(Mesh<double>*& partition, int nedge, std::vector<int>& new_vertices_per_element, int eid, const index_t *facet)
//
//Task: Heals mesh after neighboring partition has altered its interface facets
void MeshPartitions::heal_facet(Mesh<double>*& partition, int nedge, std::vector<int>& new_vertices_per_element, int eid, const index_t *facet)
{
    //std::cout << "    heal facet for eid " << eid << std::endl;

    const index_t *n= partition->get_element(eid);

    index_t newVertex[3] = {-1, -1, -1};
    newVertex[0] = new_vertices_per_element[nedge*eid+edgeNumber(partition, eid, facet[1], facet[2])];
    newVertex[1] = new_vertices_per_element[nedge*eid+edgeNumber(partition, eid, facet[0], facet[2])];
    newVertex[2] = new_vertices_per_element[nedge*eid+edgeNumber(partition, eid, facet[0], facet[1])];

    int refine_cnt=0;

    for(size_t i=0; i<3; ++i)
    {
        if(newVertex[i]!=-1)
        {
            ++refine_cnt;
        }
    }

  //  std::cout << "     refine_cnt " << refine_cnt << std::endl;

    //switch the different cases of facet refinement (healing)
    switch(refine_cnt)
    {
        case 0:
        {
            //do nothing
            break;
        } //end of case 0

        case 1 :
        {
            // 1:2 facet bisection
            for(int j=0; j<3; j++)
            {
                if(newVertex[j] >= 0) 
                {
                    partition->add_nnlist(newVertex[j], facet[j]);
                    partition->add_nnlist(facet[j] , newVertex[j]);
                  //  std::cout << "        " << newVertex[j] << " " << facet[j] << std::endl;
                    break;
                }
            }

            break;
        } //end of case 1

        case 2 :
        {
            // 1:3 refinement with trapezoid split
            for(int j=0; j<3; j++) 
            {
                if(newVertex[j] < 0) 
                {
                    partition->add_nnlist(newVertex[(j+1)%3], newVertex[(j+2)%3]);
                    partition->add_nnlist(newVertex[(j+2)%3], newVertex[(j+1)%3]);
                   // std::cout << "        " << newVertex[(j+1)%3] << " " << newVertex[(j+1)%3] << std::endl;

                    double ldiag1 = partition->calc_edge_length(newVertex[(j+1)%3], facet[(j+1)%3]);
                    double ldiag2 = partition->calc_edge_length(newVertex[(j+2)%3], facet[(j+2)%3]);
                    const int offset = ldiag1 < ldiag2 ? (j+1)%3 : (j+2)%3;

                   // std::cout << "         offset: " << offset << std::endl;

                    partition->add_nnlist(newVertex[offset], facet[offset]);
                    partition->add_nnlist(facet[offset], newVertex[offset]);

                  //  std::cout << "         " << newVertex[offset] << " " << facet[offset] << std::endl;

                    break;
                }
            }

            break;
        } //end of case 2

        case 3 :
        {
            // 1:4 regular refinement
            for(int j=0; j<3; j++) 
            {
                partition->add_nnlist(newVertex[j], newVertex[(j+1)%3]);
                partition->add_nnlist(newVertex[(j+1)%3], newVertex[j]);
              //  std::cout << "         " << newVertex[j] << " " << newVertex[(j+1)%3] << std::endl;
            }

            break;
        } //end of case 3

        default:
            break;
    } //end of switch
}
//end of MeshPartitions::heal_facet(Mesh<double>*& partition, int nedge, std::vector<int>& new_vertices_per_element, int eid, const index_t *facet)

//MeshPartitions::heal3D_1(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges)
//
//Task: Heals mesh after neighboring partition has altered its interface
void MeshPartitions::heal3D_1(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges)
{
    //std::cout << "    heal3D_1 " << eid << std::endl;

    const int *n=partition->get_element(eid);
    //std::cout << "       " << n[0] << " " << n[1] << " " << n[2] << " " << n[3] << std::endl;
    const int *boundary=&(partition->boundary[eid*nloc]);

    std::map<index_t, int> b;
    for(int j=0; j<nloc; ++j)
    {
        b[n[j]] = boundary[j];
    }

    // Find the opposite edge
    index_t oe[2];
    for(int j=0, pos=0; j<4; j++)
    {
        if(!splitEdges[0].contains(n[j]))
        {
            oe[pos++] = n[j];
        }
    }

    // Form and add two new edges.
    const int ele0[] = {splitEdges[0].edge.first, splitEdges[0].id, oe[0], oe[1]};
    const int ele1[] = {splitEdges[0].edge.second, splitEdges[0].id, oe[0], oe[1]};

    const int ele0_boundary[] = {0, b[splitEdges[0].edge.second], b[oe[0]], b[oe[1]]};
    const int ele1_boundary[] = {0, b[splitEdges[0].edge.first], b[oe[0]], b[oe[1]]};

    index_t ele1ID;
    ele1ID = splitCnt;

    // ele1ID is a new ID which isn't correct yet, it has to be
    // updated once each thread has calculated how many new elements
    // it created, so put ele1ID into add_nelist_fix instead of add_nelist.
    // Put ele1 in oe[0] and oe[1]'s NEList
    partition->add_nelist_fix(oe[0], ele1ID, threadIdx);
    partition->add_nelist_fix(oe[1], ele1ID, threadIdx);

    // Put eid and ele1 in newVertex[0]'s NEList
    partition->add_nelist(splitEdges[0].id, eid);
    partition->add_nelist_fix(splitEdges[0].id, ele1ID, threadIdx);

    // Replace eid with ele1 in splitEdges[0].edge.second's NEList
    partition->remove_nelist(splitEdges[0].edge.second, eid);
    partition->add_nelist_fix(splitEdges[0].edge.second, ele1ID, threadIdx);

    partition->replace_element(eid, ele0);
    partition->append_element(ele1);
    splitCnt += 1;
}
//end of MeshPartitions::heal3D_1(Mesh<double>*& partition, int eid, int nloc, int splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges)

//MeshPartitions::heal3D_2(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges)
//
//Task: Heals mesh after neighboring partition has altered its interface
void MeshPartitions::heal3D_2(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges)
{
    //std::cout << "    heal3D_2 " << eid << std::endl;

    const int *n=partition->get_element(eid);
    const int *boundary=&(partition->boundary[eid*nloc]);

    //std::cout << "       " << n[0] << " " << n[1] << " " << n[2] << " " << n[3] << std::endl;

    std::map<index_t, int> b;

    for(int j=0; j<nloc; ++j)
    {
        b[n[j]] = boundary[j];
    }

    /* Here there are two possibilities. Either the two split
    * edges share a vertex (case 2(a)) or they are opposite edges
    * (case 2(b)). Case 2(a) results in a 1:3 subdivision, case 2(b)
    * results in a 1:4.
    */

    int n0=splitEdges[0].connected(splitEdges[1]);

    if(n0>=0) 
    {
        //std::cout << "     case 2(a)" << std::endl;

        /*
        *************
        * Case 2(a) *
        *************
        */
        int n1 = (n0 == splitEdges[0].edge.first) ? splitEdges[0].edge.second : splitEdges[0].edge.first;
        int n2 = (n0 == splitEdges[1].edge.first) ? splitEdges[1].edge.second : splitEdges[1].edge.first;

        // Opposite vertex
        int n3;
        for(int j=0; j<nloc; ++j)
            if(n[j] != n0 && n[j] != n1 && n[j] != n2) {
                n3 = n[j];
                break;
            }

        // Find the diagonal which has bisected the trapezoid.
        DirectedEdge<index_t> diagonal, offdiagonal;
        std::vector<index_t>::const_iterator p = std::find(partition->NNList[splitEdges[0].id].begin(),
                partition->NNList[splitEdges[0].id].end(), n2);
        if(p != partition->NNList[splitEdges[0].id].end()) {
            diagonal.edge.first = splitEdges[0].id;
            diagonal.edge.second = n2;
            offdiagonal.edge.first = splitEdges[1].id;
            offdiagonal.edge.second = n1;
        } else {
            assert(std::find(partition->NNList[splitEdges[1].id].begin(),
                                partition->NNList[splitEdges[1].id].end(), n1) != partition->NNList[splitEdges[1].id].end());
            diagonal.edge.first = splitEdges[1].id;
            diagonal.edge.second = n1;
            offdiagonal.edge.first = splitEdges[0].id;
            offdiagonal.edge.second = n2;
        }

        const int ele0[] = {n0, splitEdges[0].id, splitEdges[1].id, n3};
        const int ele1[] = {diagonal.edge.first, offdiagonal.edge.first, diagonal.edge.second, n3};
        const int ele2[] = {diagonal.edge.first, diagonal.edge.second, offdiagonal.edge.second, n3};

        const int ele0_boundary[] = {0, b[n1], b[n2], b[n3]};
        const int ele1_boundary[] = {b[offdiagonal.edge.second], 0, 0, b[n3]};
        const int ele2_boundary[] = {b[n0], b[diagonal.edge.second], 0, b[n3]};

        index_t ele1ID, ele2ID;
        ele1ID = splitCnt;
        ele2ID = ele1ID+1;

        partition->add_nelist(diagonal.edge.first, eid);
        partition->add_nelist_fix(diagonal.edge.first, ele1ID, threadIdx);
        partition->add_nelist_fix(diagonal.edge.first, ele2ID, threadIdx);

        partition->remove_nelist(diagonal.edge.second, eid);
        partition->add_nelist_fix(diagonal.edge.second, ele1ID, threadIdx);
        partition->add_nelist_fix(diagonal.edge.second, ele2ID, threadIdx);

        partition->add_nelist(offdiagonal.edge.first, eid);
        partition->add_nelist_fix(offdiagonal.edge.first, ele1ID, threadIdx);

        partition->remove_nelist(offdiagonal.edge.second, eid);
        partition->add_nelist_fix(offdiagonal.edge.second, ele2ID, threadIdx);

        partition->add_nelist_fix(n3, ele1ID, threadIdx);
        partition->add_nelist_fix(n3, ele2ID, threadIdx);

        partition->replace_element(eid, ele0);
        partition->append_element(ele1);
        partition->append_element(ele2);

        splitCnt += 2;
    } //end of if(n0>=0) (2(a))

    else 
    {
        //std::cout << "     case 2(b)" << std::endl;
        /*
        *************
        * Case 2(b) *
        *************
        */
        const int ele0[] = {splitEdges[0].edge.first, splitEdges[0].id, splitEdges[1].edge.first, splitEdges[1].id};
        const int ele1[] = {splitEdges[0].edge.first, splitEdges[0].id, splitEdges[1].edge.second, splitEdges[1].id};
        const int ele2[] = {splitEdges[0].edge.second, splitEdges[0].id, splitEdges[1].edge.first, splitEdges[1].id};
        const int ele3[] = {splitEdges[0].edge.second, splitEdges[0].id, splitEdges[1].edge.second, splitEdges[1].id};

        const int ele0_boundary[] = {0, b[splitEdges[0].edge.second], 0, b[splitEdges[1].edge.second]};
        const int ele1_boundary[] = {0, b[splitEdges[0].edge.second], 0, b[splitEdges[1].edge.first]};
        const int ele2_boundary[] = {0, b[splitEdges[0].edge.first], 0, b[splitEdges[1].edge.second]};
        const int ele3_boundary[] = {0, b[splitEdges[0].edge.first], 0, b[splitEdges[1].edge.first]};

        index_t ele1ID, ele2ID, ele3ID;
        ele1ID = splitCnt;
        ele2ID = ele1ID+1;
        ele3ID = ele1ID+2;

        partition->add_nnlist(splitEdges[0].id, splitEdges[1].id);
        partition->add_nnlist(splitEdges[1].id, splitEdges[0].id);

        partition->add_nelist(splitEdges[0].id, eid);
        partition->add_nelist_fix(splitEdges[0].id, ele1ID, threadIdx);
        partition->add_nelist_fix(splitEdges[0].id, ele2ID, threadIdx);
        partition->add_nelist_fix(splitEdges[0].id, ele3ID, threadIdx);

        partition->add_nelist(splitEdges[1].id, eid);
        partition->add_nelist_fix(splitEdges[1].id, ele1ID, threadIdx);
        partition->add_nelist_fix(splitEdges[1].id, ele2ID, threadIdx);
        partition->add_nelist_fix(splitEdges[1].id, ele3ID, threadIdx);

        partition->add_nelist_fix(splitEdges[0].edge.first, ele1ID, threadIdx);

        partition->remove_nelist(splitEdges[0].edge.second, eid);
        partition->add_nelist_fix(splitEdges[0].edge.second, ele2ID, threadIdx);
        partition->add_nelist_fix(splitEdges[0].edge.second, ele3ID, threadIdx);

        partition->add_nelist_fix(splitEdges[1].edge.first, ele2ID, threadIdx);

        partition->remove_nelist(splitEdges[1].edge.second, eid);
        partition->add_nelist_fix(splitEdges[1].edge.second, ele1ID, threadIdx);
        partition->add_nelist_fix(splitEdges[1].edge.second, ele3ID, threadIdx);

        partition->replace_element(eid, ele0);
        partition->append_element(ele1);
        partition->append_element(ele2);
        partition->append_element(ele3);

        splitCnt += 3;
    } //end of else (2(b))
}
//end of MeshPartitions::heal3D_2(Mesh<double>*& partition, int eid, int nloc, int splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges)

//MeshPartitions::heal3D_3(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges)
//
//Task: Heals mesh after neighboring partition has altered its interface
void MeshPartitions::heal3D_3(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges,
                              int msize, int dim, ElementProperty<double>*& property)
{
    //std::cout << "     heal3D_3 " << eid << std::endl;
    const int *n=partition->get_element(eid);
    const int *boundary=&(partition->boundary[eid*nloc]);

    //std::cout << "       " << n[0] << " " << n[1] << " " << n[2] << " " << n[3] << std::endl;

    std::map<index_t, int> b;
    for(int j=0; j<nloc; ++j)
    {
        b[n[j]] = boundary[j];
    }

    /* There are 3 cases that need to be considered. They can
    * be distinguished by the total number of nodes that are
    * common between any pair of edges.
    * Case 3(a): there are 3 different nodes common between pairs
    * of split edges, i.e the three new vertices are on the
    * same triangle.
    * Case 3(b): The three new vertices are around the same
    * original vertex.
    * Case 3(c): There are 2 different nodes common between pairs
    * of split edges.
    */
    std::set<index_t> shared;
    for(int j=0; j<3; j++) 
    {
        for(int k=j+1; k<3; k++) 
        {
            index_t nid = splitEdges[j].connected(splitEdges[k]);
            if(nid>=0)
            {
                shared.insert(nid);
            }
        }
    }
    size_t nshared = shared.size();

    if(nshared==3) 
    {
        //std::cout << "       3(a)" << std::endl; 
        /*
        *************
        * Case 3(a) *
        *************
        */
        index_t m[] = {-1, -1, -1, -1, -1, -1, -1};

        m[0] = splitEdges[0].edge.first;
        m[1] = splitEdges[0].id;
        m[2] = splitEdges[0].edge.second;
        if(splitEdges[1].contains(m[2])) {
            m[3] = splitEdges[1].id;
            if(splitEdges[1].edge.first!=m[2])
                m[4] = splitEdges[1].edge.first;
            else
                m[4] = splitEdges[1].edge.second;
            m[5] = splitEdges[2].id;
        } else {
            m[3] = splitEdges[2].id;
            if(splitEdges[2].edge.first!=m[2])
                m[4] = splitEdges[2].edge.first;
            else
                m[4] = splitEdges[2].edge.second;
            m[5] = splitEdges[1].id;
        }
        for(int j=0; j<4; j++) {
            if((n[j]!=m[0])&&(n[j]!=m[2])&&(n[j]!=m[4])) {
                m[6] = n[j];
                break;
            }
        }

        const int ele0[] = {m[0], m[1], m[5], m[6]};
        const int ele1[] = {m[1], m[2], m[3], m[6]};
        const int ele2[] = {m[5], m[3], m[4], m[6]};
        const int ele3[] = {m[1], m[3], m[5], m[6]};

        const int ele0_boundary[] = {0, b[m[2]], b[m[4]], b[m[6]]};
        const int ele1_boundary[] = {b[m[0]], 0, b[m[4]], b[m[6]]};
        const int ele2_boundary[] = {b[m[0]], b[m[2]], 0, b[m[6]]};
        const int ele3_boundary[] = {0, 0, 0, b[m[6]]};

        index_t ele1ID, ele2ID, ele3ID;
        ele1ID = splitCnt;
        ele2ID = ele1ID+1;
        ele3ID = ele1ID+2;

        partition->add_nelist(m[1], eid);
        partition->add_nelist_fix(m[1], ele1ID, threadIdx);
        partition->add_nelist_fix(m[1], ele3ID, threadIdx);

        partition->add_nelist(m[5], eid);
        partition->add_nelist_fix(m[5], ele2ID, threadIdx);
        partition->add_nelist_fix(m[5], ele3ID, threadIdx);

        partition->add_nelist_fix(m[3], ele1ID, threadIdx);
        partition->add_nelist_fix(m[3], ele2ID, threadIdx);
        partition->add_nelist_fix(m[3], ele3ID, threadIdx);

        partition->add_nelist_fix(m[6], ele1ID, threadIdx);
        partition->add_nelist_fix(m[6], ele2ID, threadIdx);
        partition->add_nelist_fix(m[6], ele3ID, threadIdx);

        partition->remove_nelist(m[2], eid);
        partition->add_nelist_fix(m[2], ele1ID, threadIdx);

        partition->remove_nelist(m[4], eid);
        partition->add_nelist_fix(m[4], ele2ID, threadIdx);

        partition->replace_element(eid, ele0);
        partition->append_element(ele1);
        partition->append_element(ele2);
        partition->append_element(ele3);

        splitCnt += 3;
    } //end of if (nshared == 3) (case 3(a))

    else if(nshared==1) 
    {
        //std::cout << "       3(b)" << std::endl; 
        /*
        *************
        * Case 3(b) *
        *************
        */

        // Find the three bottom vertices, i.e. vertices of
        // the original elements which are part of the wedge.
        index_t top_vertex = *shared.begin();
        index_t bottom_triangle[3], top_triangle[3];
        for(int j=0; j<3; ++j) 
        {
            if(splitEdges[j].edge.first != top_vertex) 
            {
                bottom_triangle[j] = splitEdges[j].edge.first;
            } 
            
            else 
            {
                bottom_triangle[j] = splitEdges[j].edge.second;
            }
            
            top_triangle[j] = splitEdges[j].id;
        }

        // Boundary values of each wedge side
        int bwedge[] = {b[bottom_triangle[2]], b[bottom_triangle[0]], b[bottom_triangle[1]], 0, b[top_vertex]};
        refine_wedge(partition, top_triangle, bottom_triangle, bwedge, NULL, eid, threadIdx, splitCnt, nloc, msize, dim, property);

        const int ele0[] = {top_vertex, splitEdges[0].id, splitEdges[1].id, splitEdges[2].id};
        const int ele0_boundary[] = {0, b[bottom_triangle[0]], b[bottom_triangle[1]], b[bottom_triangle[2]]};

        partition->remove_nelist(bottom_triangle[0], eid);
        partition->remove_nelist(bottom_triangle[1], eid);
        partition->remove_nelist(bottom_triangle[2], eid);
        partition->add_nelist(splitEdges[0].id, eid);
        partition->add_nelist(splitEdges[1].id, eid);
        partition->add_nelist(splitEdges[2].id, eid);

        partition->replace_element(eid, ele0);
    }
}
//end of MeshPartitions::heal3D_3(Mesh<double>*& partition, int eid, int nloc, int splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges)

//MeshPartitions::heal3D_4(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges, int msize, int dim)
//
//Task: Heals mesh after neighboring partition has altered its interface
void MeshPartitions::heal3D_4(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges, int msize, int dim,
                              ElementProperty<double>*& property)
{
    //std::cout << "     heal3D_4 " << eid << std::endl;
    const int *n=partition->get_element(eid);
    const int *boundary=&(partition->boundary[eid*nloc]);

    //std::cout << "       " << n[0] << " " << n[1] << " " << n[2] << " " << n[3] << std::endl;

    std::map<index_t, int> b;
    for(int j=0; j<nloc; ++j)
    {
        b[n[j]] = boundary[j];
    }

    /*
    * There are 2 cases here:
    *
    * Case 4(a): Three split edges are on the same triangle.
    * Case 4(b): Each of the four triangles has exactly two split edges.
    */

    std::set<index_t> shared;
    for(int j=0; j<4; ++j) 
    {
        for(int k=j+1; k<4; ++k) 
        {
            index_t nid = splitEdges[j].connected(splitEdges[k]);
            if(nid>=0)
            {
                shared.insert(nid);
            }
        }
    }

    size_t nshared = shared.size();
    assert(nshared==3 || nshared==4);

    if(nshared==3) 
    {
        //std::cout << "      4(a)" << std::endl;
        /*
        *************
        * Case 4(a) *
        *************
        */

        DirectedEdge<index_t>* p[4];
        int pos = 0;
        for(int j=0; j<4; ++j)
        {
            if(shared.count(splitEdges[j].edge.first)>0 && shared.count(splitEdges[j].edge.second)>0)
            {
                p[pos++] = &splitEdges[j];
            }

            else
            {
                p[3] = &splitEdges[j];
            }
        }

        assert(pos==3);

        // p[0], p[1] and p[2] point to the three split edges which
        // are on the same facet, p[3] points to the other split edge.

        // Re-arrange p[] so that p[0] points to the
        // split edge which is not connected to p[3].
        if(p[3]->connected(*p[0]) >= 0) 
        {
            for(int j=1; j<3; ++j) 
            {
                if(p[3]->connected(*p[j]) < 0) 
                {
                    DirectedEdge<index_t> *swap = p[j];
                    p[j] = p[0];
                    p[0] = swap;
                    break;
                }
            }
        }

        // Re-arrange p[3] if necessary so that edge.first
        // is the vertex on the triangle with the 3 split edges.
        if(shared.count(p[3]->edge.first)==0) 
        {
            index_t v = p[3]->edge.first;
            p[3]->edge.first = p[3]->edge.second;
            p[3]->edge.second = v;
        }

        // Same for p[1] and p[2]; make edge.first = p[3]->edge.first.
        for(int j=1; j<=2; ++j)
        {
            if(p[j]->edge.first != p[3]->edge.first) 
            {
                assert(p[j]->edge.second == p[3]->edge.first);
                p[j]->edge.second = p[j]->edge.first;
                p[j]->edge.first = p[3]->edge.first;
            }
        }

        /*
        * There are 3 sub-cases, depending on the way the trapezoids
        * on the facets between p[1]-p[3] and p[2]-p[3] were bisected.
        *
        * Case 4(a)(1): No diagonal involves p[3].
        * Case 4(a)(2): Only one diagonal involves p[3].
        * Case 4(a)(3): Both diagonals involve p[3].
        */

        std::vector< DirectedEdge<index_t> > diagonals;
        for(std::vector<index_t>::const_iterator it=partition->NNList[p[3]->id].begin(); it!=partition->NNList[p[3]->id].end(); ++it) 
        {
            if(*it == p[1]->edge.second || *it == p[2]->edge.second) 
            {
                diagonals.push_back(DirectedEdge<index_t>(p[3]->id, *it));
            }
        }

        switch(diagonals.size()) 
        {
            case 0: 
            {
                //std::cout << "        4(a) 1" << std::endl;
                // Case 4(a)(1)
                const int ele0[] = {p[0]->id, p[1]->edge.second, p[1]->id, p[3]->edge.second};
                const int ele1[] = {p[0]->id, p[1]->id, p[2]->id, p[3]->edge.second};
                const int ele2[] = {p[0]->id, p[2]->id, p[2]->edge.second, p[3]->edge.second};
                const int ele3[] = {p[1]->id, p[3]->id, p[2]->id, p[3]->edge.second};
                const int ele4[] = {p[1]->id, p[2]->id, p[3]->id, p[3]->edge.first};

                const int ele0_boundary[] = {b[p[2]->edge.second], 0, b[p[3]->edge.first], b[p[3]->edge.second]};
                const int ele1_boundary[] = {0, 0, 0, b[p[3]->edge.second]};
                const int ele2_boundary[] = {b[p[1]->edge.second], b[p[2]->edge.first], 0, b[p[3]->edge.second]};
                const int ele3_boundary[] = {b[p[1]->edge.second], 0, b[p[2]->edge.second], 0};
                const int ele4_boundary[] = {b[p[1]->edge.second], b[p[2]->edge.second], b[p[3]->edge.second], 0};

                index_t ele1ID, ele2ID, ele3ID, ele4ID;
                ele1ID = splitCnt;
                ele2ID = ele1ID+1;
                ele3ID = ele1ID+2;
                ele4ID = ele1ID+3;

                partition->remove_nelist(p[2]->edge.first, eid);
                partition->add_nelist_fix(p[2]->edge.first, ele4ID, threadIdx);

                partition->remove_nelist(p[2]->edge.second, eid);
                partition->add_nelist_fix(p[2]->edge.second, ele2ID, threadIdx);

                partition->add_nelist_fix(p[3]->edge.second, ele1ID, threadIdx);
                partition->add_nelist_fix(p[3]->edge.second, ele2ID, threadIdx);
                partition->add_nelist_fix(p[3]->edge.second, ele3ID, threadIdx);

                partition->add_nelist(p[0]->id, eid);
                partition->add_nelist_fix(p[0]->id, ele1ID, threadIdx);
                partition->add_nelist_fix(p[0]->id, ele2ID, threadIdx);

                partition->add_nelist(p[1]->id, eid);
                partition->add_nelist_fix(p[1]->id, ele1ID, threadIdx);
                partition->add_nelist_fix(p[1]->id, ele3ID, threadIdx);
                partition->add_nelist_fix(p[1]->id, ele4ID, threadIdx);

                partition->add_nelist_fix(p[2]->id, ele1ID, threadIdx);
                partition->add_nelist_fix(p[2]->id, ele2ID, threadIdx);
                partition->add_nelist_fix(p[2]->id, ele3ID, threadIdx);
                partition->add_nelist_fix(p[2]->id, ele4ID, threadIdx);

                partition->add_nelist_fix(p[3]->id, ele3ID, threadIdx);
                partition->add_nelist_fix(p[3]->id, ele4ID, threadIdx);

                partition->replace_element(eid, ele0);
                partition->append_element(ele1);
                partition->append_element(ele2);
                partition->append_element(ele3);
                partition->append_element(ele4);

                splitCnt += 4;

                break;
            } //end of case 0

            case 1: 
            {
                //std::cout << "        4(a) 2" << std::endl;
                // Case 4(a)(2)

                // Swap p[1] and p[2] if necessary so that p[2]->edge.second
                // is the ending point of the diagonal bisecting the trapezoid.
                if(p[2]->edge.second != diagonals[0].edge.second) 
                {
                    DirectedEdge<index_t> *swap = p[1];
                    p[1] = p[2];
                    p[2] = swap;
                }
                assert(p[2]->edge.second == diagonals[0].edge.second);

                const int ele0[] = {p[0]->id, p[1]->edge.second, p[1]->id, p[3]->edge.second};
                const int ele1[] = {p[0]->id, p[3]->id, p[2]->edge.second, p[3]->edge.second};
                const int ele2[] = {p[0]->id, p[1]->id, p[3]->id, p[3]->edge.second};
                const int ele3[] = {p[0]->id, p[3]->id, p[2]->id, p[2]->edge.second};
                const int ele4[] = {p[0]->id, p[1]->id, p[2]->id, p[3]->id};
                const int ele5[] = {p[2]->id, p[3]->id, p[1]->id, p[3]->edge.first};

                const int ele0_boundary[] = {b[p[2]->edge.second], 0, b[p[1]->edge.first], b[p[3]->edge.second]};
                const int ele1_boundary[] = {b[p[1]->edge.second], b[p[3]->edge.first], 0, 0};
                const int ele2_boundary[] = {b[p[2]->edge.second], 0, 0, 0};
                const int ele3_boundary[] = {b[p[1]->edge.second], b[p[3]->edge.second], 0, 0};
                const int ele4_boundary[] = {0, 0, 0, b[p[3]->edge.second]};
                const int ele5_boundary[] = {b[p[2]->edge.second], b[p[3]->edge.second], b[p[1]->edge.second], 0};

                index_t ele1ID, ele2ID, ele3ID, ele4ID, ele5ID;
                ele1ID = splitCnt;
                ele2ID = ele1ID+1;
                ele3ID = ele1ID+2;
                ele4ID = ele1ID+3;
                ele5ID = ele1ID+4;

                partition->add_nnlist(p[0]->id, p[3]->id);
                partition->add_nnlist(p[3]->id, p[0]->id);

                partition->remove_nelist(p[2]->edge.first, eid);
                partition->add_nelist_fix(p[2]->edge.first, ele5ID, threadIdx);

                partition->remove_nelist(p[2]->edge.second, eid);
                partition->add_nelist_fix(p[2]->edge.second, ele1ID, threadIdx);
                partition->add_nelist_fix(p[2]->edge.second, ele3ID, threadIdx);

                partition->add_nelist_fix(p[3]->edge.second, ele1ID, threadIdx);
                partition->add_nelist_fix(p[3]->edge.second, ele2ID, threadIdx);

                partition->add_nelist(p[0]->id, eid);
                partition->add_nelist_fix(p[0]->id, ele1ID, threadIdx);
                partition->add_nelist_fix(p[0]->id, ele2ID, threadIdx);
                partition->add_nelist_fix(p[0]->id, ele3ID, threadIdx);
                partition->add_nelist_fix(p[0]->id, ele4ID, threadIdx);

                partition->add_nelist(p[1]->id, eid);
                partition->add_nelist_fix(p[1]->id, ele2ID, threadIdx);
                partition->add_nelist_fix(p[1]->id, ele4ID, threadIdx);
                partition->add_nelist_fix(p[1]->id, ele5ID, threadIdx);

                partition->add_nelist_fix(p[2]->id, ele3ID, threadIdx);
                partition->add_nelist_fix(p[2]->id, ele4ID, threadIdx);
                partition->add_nelist_fix(p[2]->id, ele5ID, threadIdx);

                partition->add_nelist_fix(p[3]->id, ele1ID, threadIdx);
                partition->add_nelist_fix(p[3]->id, ele2ID, threadIdx);
                partition->add_nelist_fix(p[3]->id, ele3ID, threadIdx);
                partition->add_nelist_fix(p[3]->id, ele4ID, threadIdx);
                partition->add_nelist_fix(p[3]->id, ele5ID, threadIdx);

                partition->replace_element(eid, ele0);
                partition->append_element(ele1);
                partition->append_element(ele2);
                partition->append_element(ele3);
                partition->append_element(ele4);
                partition->append_element(ele5);

                splitCnt += 5;

                break;
            } //end of case 1

            case 2: 
            {
                //std::cout << "        4(a) 3" << std::endl;
                // Case 4(a)(3)
                const int ele0[] = {p[1]->edge.first, p[1]->id, p[2]->id, p[3]->id};
                const int ele1[] = {p[3]->id, p[1]->edge.second, p[0]->id, p[3]->edge.second};
                const int ele2[] = {p[3]->id, p[0]->id, p[2]->edge.second, p[3]->edge.second};
                const int ele3[] = {p[1]->id, p[1]->edge.second, p[0]->id, p[3]->id};
                const int ele4[] = {p[1]->id, p[0]->id, p[2]->id, p[3]->id};
                const int ele5[] = {p[2]->id, p[3]->id, p[0]->id, p[2]->edge.second};

                const int ele0_boundary[] = {0, b[p[1]->edge.second], b[p[2]->edge.second], b[p[3]->edge.second]};
                const int ele1_boundary[] = {b[p[3]->edge.first], 0, b[p[2]->edge.second], 0};
                const int ele2_boundary[] = {b[p[3]->edge.first], b[p[1]->edge.second], 0, 0};
                const int ele3_boundary[] = {0, 0, b[p[2]->edge.second], b[p[3]->edge.second]};
                const int ele4_boundary[] = {0, 0, 0, b[p[3]->edge.second]};
                const int ele5_boundary[] = {0, b[p[3]->edge.second], b[p[1]->edge.second], 0};

                index_t ele1ID, ele2ID, ele3ID, ele4ID, ele5ID;
                ele1ID = splitCnt;
                ele2ID = ele1ID+1;
                ele3ID = ele1ID+2;
                ele4ID = ele1ID+3;
                ele5ID = ele1ID+4;

                partition->add_nnlist(p[0]->id, p[3]->id);
                partition->add_nnlist(p[3]->id, p[0]->id);


                partition->remove_nelist(p[1]->edge.second, eid);
                partition->add_nelist_fix(p[1]->edge.second, ele1ID, threadIdx);
                partition->add_nelist_fix(p[1]->edge.second, ele3ID, threadIdx);

                partition->remove_nelist(p[2]->edge.second, eid);
                partition->add_nelist_fix(p[2]->edge.second, ele2ID, threadIdx);
                partition->add_nelist_fix(p[2]->edge.second, ele5ID, threadIdx);

                partition->remove_nelist(p[3]->edge.second, eid);
                partition->add_nelist_fix(p[3]->edge.second, ele1ID, threadIdx);
                partition->add_nelist_fix(p[3]->edge.second, ele2ID, threadIdx);

                partition->add_nelist_fix(p[0]->id, ele1ID, threadIdx);
                partition->add_nelist_fix(p[0]->id, ele2ID, threadIdx);
                partition->add_nelist_fix(p[0]->id, ele3ID, threadIdx);
                partition->add_nelist_fix(p[0]->id, ele4ID, threadIdx);
                partition->add_nelist_fix(p[0]->id, ele5ID, threadIdx);

                partition->add_nelist(p[1]->id, eid);
                partition->add_nelist_fix(p[1]->id, ele3ID, threadIdx);
                partition->add_nelist_fix(p[1]->id, ele4ID, threadIdx);

                partition->add_nelist(p[2]->id, eid);
                partition->add_nelist_fix(p[2]->id, ele4ID, threadIdx);
                partition->add_nelist_fix(p[2]->id, ele5ID, threadIdx);

                partition->add_nelist(p[3]->id, eid);
                partition->add_nelist_fix(p[3]->id, ele1ID, threadIdx);
                partition->add_nelist_fix(p[3]->id, ele2ID, threadIdx);
                partition->add_nelist_fix(p[3]->id, ele3ID, threadIdx);
                partition->add_nelist_fix(p[3]->id, ele4ID, threadIdx);
                partition->add_nelist_fix(p[3]->id, ele5ID, threadIdx);

                partition->replace_element(eid, ele0);
                partition->append_element(ele1);
                partition->append_element(ele2);
                partition->append_element(ele3);
                partition->append_element(ele4);
                partition->append_element(ele5);

                splitCnt += 5;

                break;
            } //end of case 2

            default:
                break;
        } //end of switch
    } //end of case 4(a)

    else 
    {
        //std::cout << "      4(b)" << std::endl;
        /*
        *************
        * Case 4(b) *
        *************
        */

        // In this case, the element is split into two wedges.

        // Find the top left, top right, bottom left and
        // bottom right split edges, as depicted in the paper.
        DirectedEdge<index_t> *tr, *tl, *br, *bl;
        tl = &splitEdges[0];

        // Find top right
        for(int j=1; j<4; ++j) 
        {
            if(splitEdges[j].contains(tl->edge.first)) 
            {
                tr = &splitEdges[j];
                break;
            }
        }

        // Re-arrange tr so that tl->edge.first == tr->edge.first
        if(tr->edge.first != tl->edge.first) 
        {
            assert(tr->edge.second == tl->edge.first);
            tr->edge.second = tr->edge.first;
            tr->edge.first = tl->edge.first;
        }

        // Find bottom left
        for(int j=1; j<4; ++j) 
        {
            if(splitEdges[j].contains(tl->edge.second)) 
            {
                bl = &splitEdges[j];
                break;
            }
        }
        // Re-arrange bl so that tl->edge.second == bl->edge.second
        if(bl->edge.second != tl->edge.second) 
        {
            assert(bl->edge.first == tl->edge.second);
            bl->edge.first = bl->edge.second;
            bl->edge.second = tl->edge.second;
        }

        // Find bottom right
        for(int j=1; j<4; ++j) 
        {
            if(splitEdges[j].contains(bl->edge.first) && splitEdges[j].contains(tr->edge.second)) {
                br = &splitEdges[j];
                break;
            }
        }

        // Re-arrange br so that bl->edge.first == br->edge.first
        if(br->edge.first != bl->edge.first) 
        {
            assert(br->edge.second == bl->edge.first);
            br->edge.second = br->edge.first;
            br->edge.first = bl->edge.first;
        }

        assert(tr->edge.second == br->edge.second);

        // Find how the trapezoids have been split
        DirectedEdge<index_t> bw1, bw2, tw1, tw2;
        std::vector<index_t>::const_iterator p;

        // For the bottom wedge:
        // 1) From tl->id to tr->edge.second or from tr->id to tl->edge.second?
        // 2) From bl->id to br->edge.second or from br->id to bl->edge.second?
        // For the top wedge:
        // 1) From tl->id to bl->edge.first or from bl->id to tl->edge.first?
        // 2) From tr->id to br->edge.first or from br->id to tr->edge.first?

        p = std::find(partition->NNList[tl->id].begin(), partition->NNList[tl->id].end(), tr->edge.second);
        if(p != partition->NNList[tl->id].end()) 
        {
            bw1.edge.first = tl->id;
            bw1.edge.second = tr->edge.second;
        } 
        
        else 
        {
            bw1.edge.first = tr->id;
            bw1.edge.second = tl->edge.second;
        }

        p = std::find(partition->NNList[bl->id].begin(), partition->NNList[bl->id].end(), br->edge.second);
        if(p != partition->NNList[bl->id].end()) 
        {
            bw2.edge.first = bl->id;
            bw2.edge.second = br->edge.second;
        } 
        
        else 
        {
            bw2.edge.first = br->id;
            bw2.edge.second = bl->edge.second;
        }

        p = std::find(partition->NNList[tl->id].begin(), partition->NNList[tl->id].end(), bl->edge.first);
        if(p != partition->NNList[tl->id].end()) 
        {
            tw1.edge.first = tl->id;
            tw1.edge.second = bl->edge.first;
        } 

        else 
        {
            tw1.edge.first = bl->id;
            tw1.edge.second = tl->edge.first;
        }

        p = std::find(partition->NNList[tr->id].begin(), partition->NNList[tr->id].end(), br->edge.first);
        if(p != partition->NNList[tr->id].end()) 
        {
            tw2.edge.first = tr->id;
            tw2.edge.second = br->edge.first;
        } 
        
        else 
        {
            tw2.edge.first = br->id;
            tw2.edge.second = tr->edge.first;
        }

        // If bw1 and bw2 are connected, then the third quadrilateral
        // can be split whichever way we like. Otherwise, we want to
        // choose the diagonal which will lead to a 1:3 wedge split.
        // Same for tw1 and tw2.

        DirectedEdge<index_t> bw, tw;
        bool flex_bottom, flex_top;

        if(bw1.connected(bw2) >= 0) 
        {
            flex_bottom = true;
        } 
        
        else 
        {
            flex_bottom = false;
            bw.edge.first = bw1.edge.first;
            bw.edge.second = bw2.edge.first;
            assert(bw.connected(bw1) >= 0 && bw.connected(bw2) >= 0);
        }

        if(tw1.connected(tw2) >= 0) 
        {
            flex_top = true;
        } 
        
        else 
        {
            flex_top = false;
            tw.edge.first = tw1.edge.first;
            tw.edge.second = tw2.edge.first;
            assert(tw.connected(tw1) >= 0 && tw.connected(tw2) >= 0);
        }

        DirectedEdge<index_t> diag;

        if(flex_top && !flex_bottom) 
        {
            // Choose the diagonal which is the preferred one for the bottom wedge
            diag.edge.first = bw.edge.first;
            diag.edge.second = bw.edge.second;
        } 
        
        else if(!flex_top && flex_bottom) 
        {
            // Choose the diagonal which is the preferred one for the top wedge
            diag.edge.first = tw.edge.first;
            diag.edge.second = tw.edge.second;
        } 
        
        else 
        {
            if(flex_top && flex_bottom) 
            {
                // Choose the shortest diagonal
                real_t ldiag1 = partition->calc_edge_length(tl->id, br->id);
                real_t ldiag2 = partition->calc_edge_length(bl->id, tr->id);

                if(ldiag1 < ldiag2) 
                {
                    diag.edge.first = tl->id;
                    diag.edge.second = br->id;
                } 
                
                else 
                {
                    diag.edge.first = bl->id;
                    diag.edge.second = tr->id;
                }
            }
            
            else 
            {
                // If we reach this point, it means we are not
                // flexible in any of the diagonals. If we are
                // lucky enough and bw==tw, then diag=bw.
                if(bw.contains(tw.edge.first) && bw.contains(tw.edge.second)) 
                {
                    diag.edge.first = bw.edge.first;
                    diag.edge.second = bw.edge.second;
                } 
                
                else 
                {
                    // Choose the shortest diagonal
                    real_t ldiag1 = partition->calc_edge_length(tl->id, br->id);
                    real_t ldiag2 = partition->calc_edge_length(bl->id, tr->id);

                    if(ldiag1 < ldiag2) 
                    {
                        diag.edge.first = tl->id;
                        diag.edge.second = br->id;
                    } 
                    
                    else 
                    {
                        diag.edge.first = bl->id;
                        diag.edge.second = tr->id;
                    }
                }
            }
        }

        partition->add_nnlist(diag.edge.first, diag.edge.second);
        partition->add_nnlist(diag.edge.second, diag.edge.first);

        // At this point, we have identified the wedges and how their sides
        // have been split, so we can proceed to the actual refinement.
        index_t top_triangle[3];
        index_t bottom_triangle[3];
        int bwedge[5];

        // Bottom wedge
        top_triangle[0] = tr->id;
        top_triangle[1] = tr->edge.second;
        top_triangle[2] = br->id;
        bottom_triangle[0] = tl->id;
        bottom_triangle[1] = tl->edge.second;
        bottom_triangle[2] = bl->id;
        bwedge[0] = b[bl->edge.first];
        bwedge[1] = b[tl->edge.first];
        bwedge[2] = 0;
        bwedge[3] = b[tl->edge.second];
        bwedge[4] = b[tr->edge.second];
        refine_wedge(partition, top_triangle, bottom_triangle, bwedge, &diag, eid, threadIdx, splitCnt, nloc, msize, dim, property);

        // Top wedge
        top_triangle[0] = tl->id;
        top_triangle[1] = tl->edge.first;
        top_triangle[2] = tr->id;
        bottom_triangle[0] = bl->id;
        bottom_triangle[1] = bl->edge.first;
        bottom_triangle[2] = br->id;
        bwedge[0] = b[tr->edge.second];
        bwedge[1] = b[tl->edge.second];
        bwedge[2] = 0;
        bwedge[3] = b[bl->edge.first];
        bwedge[4] = b[tl->edge.first];

        // Flip diag
        index_t swap = diag.edge.first;
        diag.edge.first = diag.edge.second;
        diag.edge.second = swap;
        refine_wedge(partition, top_triangle, bottom_triangle, bwedge, &diag, eid, threadIdx, splitCnt, nloc, msize, dim, property);

        // Remove parent element
        for(size_t j=0; j<nloc; ++j)
        {
            //std::cout << "     remove " << n[j] << " in " << eid << std::endl;
            partition->remove_nelist(n[j], eid);
        }

        //std::cout << " remove element " << eid << " by setting ENList[" << eid*nloc << "] to -1 " << std::endl; 
        partition->_ENList[eid*nloc] = -1;
    } //end of else (end of case 4(b))
}
//end of MeshPartitions::heal3D_4(Mesh<double>*& partition, int eid, int nloc, int splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges, int msize, int dim)

//MeshPartitions::heal3D_5(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges, int msize, int dim)
//
//Task: Heals mesh after neighboring partition has altered its interface
void MeshPartitions::heal3D_5(Mesh<double>*& partition, int eid, int nloc, int& splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges, int msize, int dim, ElementProperty<double>*& property)
{
    //std::cout << "     heal3D_5" << std::endl;
    const int *n=partition->get_element(eid);
    const int *boundary=&(partition->boundary[eid*nloc]);

    //std::cout << "       " << n[0] << " " << n[1] << " " << n[2] << " " << n[3] << std::endl;

    std::map<index_t, int> b;
    for(int j=0; j<nloc; ++j)
    {
        b[n[j]] = boundary[j];
    }

    // Find the unsplit edge
    int adj_cnt[] = {0, 0, 0, 0};
    for(int j=0; j<nloc; ++j) 
    {
        for(int k=0; k<5; ++k)
        {
            if(splitEdges[k].contains(n[j]))
            {
                ++adj_cnt[j];
            }
        }
    }

    // Vertices of the unsplit edge are adjacent to 2 split edges;
    // the other vertices are adjacent to 3 split edges.
    index_t ue[2];
    int pos=0;

    for(int j=0; j<nloc; ++j)
    {
        if(adj_cnt[j] == 2)
        {
            ue[pos++] = n[j];
        }
    }

    // Find the opposite edge
    DirectedEdge<index_t> *oe;

    for(int k=0; k<5; ++k)
    {
        if(!splitEdges[k].contains(ue[0]) && !splitEdges[k].contains(ue[1])) 
        {
            // Swap splitEdges[k] with splitEdges[4]
            if(k!=4) 
            {
                DirectedEdge<index_t> swap = splitEdges[4];
                splitEdges[4] = splitEdges[k];
                splitEdges[k] = swap;
            }

            oe = &splitEdges[4];
            break;
        }
    }

    // Like in 4(b), find tl, tr, bl and br
    DirectedEdge<index_t> *tr, *tl, *br, *bl;
    tl = &splitEdges[0];

    // Flip tl if necessary so that tl->edge.first is part of the unsplit edge
    if(oe->contains(tl->edge.first)) 
    {
        index_t swap = tl->edge.second;
        tl->edge.second = tl->edge.first;
        tl->edge.first = swap;
    }

    // Find top right
    for(int j=1; j<4; ++j) 
    {
        if(splitEdges[j].contains(tl->edge.first)) 
        {
            tr = &splitEdges[j];
            break;
        }
    }
    // Re-arrange tr so that tl->edge.first == tr->edge.first
    if(tr->edge.first != tl->edge.first) 
    {
        assert(tr->edge.second == tl->edge.first);
        tr->edge.second = tr->edge.first;
        tr->edge.first = tl->edge.first;
    }

    // Find bottom left
    for(int j=1; j<4; ++j) 
    {
        if(splitEdges[j].contains(tl->edge.second)) 
        {
            bl = &splitEdges[j];
            break;
        }
    }
    // Re-arrange bl so that tl->edge.second == bl->edge.second
    if(bl->edge.second != tl->edge.second) 
    {
        assert(bl->edge.first == tl->edge.second);
        bl->edge.first = bl->edge.second;
        bl->edge.second = tl->edge.second;
    }

    // Find bottom right
    for(int j=1; j<4; ++j) 
    {
        if(splitEdges[j].contains(bl->edge.first) && splitEdges[j].contains(tr->edge.second)) 
        {
            br = &splitEdges[j];
            break;
        }
    }
    // Re-arrange br so that bl->edge.first == br->edge.first
    if(br->edge.first != bl->edge.first) {
        assert(br->edge.second == bl->edge.first);
        br->edge.second = br->edge.first;
        br->edge.first = bl->edge.first;
    }

    assert(tr->edge.second == br->edge.second);
    assert(oe->contains(tl->edge.second) && oe->contains(tr->edge.second));

    // Find how the trapezoids have been split:
    // 1) From tl->id to bl->edge.first or from bl->id to tl->edge.first?
    // 2) From tr->id to br->edge.first or from br->id to tr->edge.first?
    DirectedEdge<index_t> q1, q2;
    std::vector<index_t>::const_iterator p;

    p = std::find(partition->NNList[tl->id].begin(), partition->NNList[tl->id].end(), bl->edge.first);

    if(p != partition->NNList[tl->id].end()) 
    {
        q1.edge.first = tl->id;
        q1.edge.second = bl->edge.first;
    } 
    
    else 
    {
        q1.edge.first = bl->id;
        q1.edge.second = tl->edge.first;
    }

    p = std::find(partition->NNList[tr->id].begin(), partition->NNList[tr->id].end(), br->edge.first);

    if(p != partition->NNList[tr->id].end()) 
    {
        q2.edge.first = tr->id;
        q2.edge.second = br->edge.first;
    } 
    
    else 
    {
        q2.edge.first = br->id;
        q2.edge.second = tr->edge.first;
    }

    DirectedEdge<index_t> diag, cross_diag;

    if(q1.connected(q2) >= 0) 
    {
        // We are flexible in choosing how the third quadrilateral
        // will be split and we will choose the shortest diagonal.
        real_t ldiag1 = partition->calc_edge_length(tl->id, br->id);
        real_t ldiag2 = partition->calc_edge_length(bl->id, tr->id);

        if(ldiag1 < ldiag2) 
        {
            diag.edge.first = br->id;
            diag.edge.second = tl->id;
            cross_diag.edge.first = tr->id;
            cross_diag.edge.second = bl->id;
        } 
        
        else 
        {
            diag.edge.first = bl->id;
            diag.edge.second = tr->id;
            cross_diag.edge.first = tl->id;
            cross_diag.edge.second = br->id;
        }
    } 
    
    else 
    {
        // We will choose the diagonal which leads to a 1:3 wedge refinement.
        if(q1.edge.first == bl->id) 
        {
            assert(q2.edge.first == tr->id);
            diag.edge.first = q1.edge.first;
            diag.edge.second = q2.edge.first;
        } 
        
        else 
        {
            assert(q2.edge.first == br->id);
            diag.edge.first = q2.edge.first;
            diag.edge.second = q1.edge.first;
        }

        assert((diag.edge.first==bl->id && diag.edge.second==tr->id) || (diag.edge.first==br->id && diag.edge.second==tl->id));

        cross_diag.edge.first = (diag.edge.second == tr->id ? tl->id : tr->id);
        cross_diag.edge.second = (diag.edge.first == br->id ? bl->id : br->id);

        assert((cross_diag.edge.first==tl->id && cross_diag.edge.second==br->id) || (cross_diag.edge.first==tr->id && cross_diag.edge.second==bl->id));
    }

    index_t bottom_triangle[] = {br->id, br->edge.first, bl->id};
    index_t top_triangle[] = {tr->id, tr->edge.first, tl->id};
    int bwedge[] = {b[bl->edge.second], b[br->edge.second], 0, b[bl->edge.first], b[tl->edge.first]};
    refine_wedge(partition, top_triangle, bottom_triangle, bwedge, &diag, eid, threadIdx, splitCnt, nloc, msize, dim, property);

    const int ele0[] = {tl->edge.second, bl->id, tl->id, oe->id};
    const int ele1[] = {tr->edge.second, tr->id, br->id, oe->id};
    const int ele2[] = {diag.edge.first, cross_diag.edge.first, diag.edge.second, oe->id};
    const int ele3[] = {diag.edge.first, diag.edge.second, cross_diag.edge.second, oe->id};

    const int ele0_boundary[] = {0, b[bl->edge.first], b[tl->edge.first], b[tr->edge.second]};
    const int ele1_boundary[] = {0, b[tl->edge.first], b[bl->edge.first], b[tl->edge.second]};
    const int ele2_boundary[] = {b[bl->edge.first], 0, 0, 0};
    const int ele3_boundary[] = {0, b[tl->edge.first], 0, 0};

    index_t ele1ID, ele2ID, ele3ID;
    ele1ID = splitCnt;
    ele2ID = ele1ID+1;
    ele3ID = ele1ID+2;

    partition->add_nnlist(diag.edge.first, diag.edge.second);
    partition->add_nnlist(diag.edge.second, diag.edge.first);

    partition->remove_nelist(tr->edge.first, eid);
    partition->remove_nelist(br->edge.first, eid);
    partition->remove_nelist(tr->edge.second, eid);

    partition->add_nelist(tl->id, eid);
    partition->add_nelist(bl->id, eid);
    partition->add_nelist(oe->id, eid);

    partition->add_nelist_fix(tr->edge.second, ele1ID, threadIdx);
    partition->add_nelist_fix(tr->id, ele1ID, threadIdx);
    partition->add_nelist_fix(br->id, ele1ID, threadIdx);
    partition->add_nelist_fix(oe->id, ele1ID, threadIdx);

    partition->add_nelist_fix(diag.edge.first, ele2ID, threadIdx);
    partition->add_nelist_fix(diag.edge.second, ele2ID, threadIdx);
    partition->add_nelist_fix(cross_diag.edge.first, ele2ID, threadIdx);
    partition->add_nelist_fix(oe->id, ele2ID, threadIdx);

    partition->add_nelist_fix(diag.edge.first, ele3ID, threadIdx);
    partition->add_nelist_fix(diag.edge.second, ele3ID, threadIdx);
    partition->add_nelist_fix(cross_diag.edge.second, ele3ID, threadIdx);
    partition->add_nelist_fix(oe->id, ele3ID, threadIdx);

    partition->replace_element(eid, ele0);
    partition->append_element(ele1);
    partition->append_element(ele2);
    partition->append_element(ele3);

    splitCnt += 3;
}
//end of MeshPartitions::heal3D_5(Mesh<double>*& partition, int eid, int nloc, int splitCnt, int& threadIdx, std::vector< DirectedEdge<index_t>>& splitEdges, int msize, int dim)

//MeshPartitions::refine_wedge(Mesh<double>*& partition, const index_t top_triangle[], const index_t bottom_triangle[], const int bndr[], DirectedEdge<index_t>* third_diag, int eid)
//
//Task: Refines wedge, is needed in heal3D functions!
void MeshPartitions::refine_wedge(Mesh<double>*& partition, const index_t top_triangle[], const index_t bottom_triangle[], const int bndr[], 
                                  DirectedEdge<index_t>* third_diag, int eid, int& threadIdx, int& splitCnt, int nloc, int msize, int dim,
                                  ElementProperty<double>*& property)
{
    //std::cout << "     refine_wedge" << std::endl;
    /*
    * bndr[] must contain the boundary values for each side of the wedge:
    * bndr[0], bndr[1] and bndr[2]: Boundary values of Side0, Side1 and Side2
    * bndr[3]: Boundary value of top triangle
    * bndr[4]: Boundary value of bottom triangle
    */

    /*
        * third_diag is used optionally if we need to define manually what the
        * third diagonal is (used in cases 4(b) and 5). It needs to be a directed
        * edge from the bottom triangle to the top triangle and be on Side2.
        */
    if(third_diag != NULL) 
    {
        for(int j=0; j<3; ++j) 
        {
            if(third_diag->edge.first == bottom_triangle[j] || third_diag->edge.second == top_triangle[j]) 
            {
                break;
            } 

            else if(third_diag->edge.first == top_triangle[j] || third_diag->edge.second == bottom_triangle[j]) 
            {
                index_t swap = third_diag->edge.first;
                third_diag->edge.first = third_diag->edge.second;
                third_diag->edge.second = swap;
                break;
            }
        }

        assert((third_diag->edge.first == bottom_triangle[2] && third_diag->edge.second == top_triangle[0]) ||
                (third_diag->edge.first == bottom_triangle[0] && third_diag->edge.second == top_triangle[2]));
    }

    /*
    * For each quadrilateral side of the wedge find
    * the diagonal which has bisected the wedge side.
    * Side0: bottom[0] - bottom[1] - top[1] - top[0]
    * Side1: bottom[1] - bottom[2] - top[2] - top[1]
    * Side2: bottom[2] - bottom[0] - top[0] - top[2]
    */

    std::vector< DirectedEdge<index_t> > diagonals, ghostDiagonals;
    for(int j=0; j<3; ++j) 
    {
        bool fwd_connected;
        if(j==2 && third_diag != NULL) 
        {
            fwd_connected = (bottom_triangle[j] == third_diag->edge.first ? true : false);
        } 
        
        else 
        {
            std::vector<index_t>::const_iterator p = std::find(partition->NNList[bottom_triangle[j]].begin(),
                    partition->NNList[bottom_triangle[j]].end(), top_triangle[(j+1)%3]);
            fwd_connected = (p != partition->NNList[bottom_triangle[j]].end() ? true : false);
        }

        if(fwd_connected) 
        {
            diagonals.push_back(DirectedEdge<index_t>(bottom_triangle[j], top_triangle[(j+1)%3]));
            ghostDiagonals.push_back(DirectedEdge<index_t>(bottom_triangle[(j+1)%3], top_triangle[j]));
        } 
        
        else 
        {
            diagonals.push_back(DirectedEdge<index_t>(bottom_triangle[(j+1)%3], top_triangle[j]));
            ghostDiagonals.push_back(DirectedEdge<index_t>(bottom_triangle[j], top_triangle[(j+1)%3]));
        }
    }

    // Determine how the wedge will be split
    std::vector<index_t> diag_shared;
    for(int j=0; j<3; j++) 
    {
        index_t nid = diagonals[j].connected(diagonals[(j+1)%3]);

        if(nid>=0)
        {
            diag_shared.push_back(nid);
        }
    }

    if(!diag_shared.empty()) 
    {
        /*
        ***************
        * Case 1-to-3 *
        ***************
        */

        assert(diag_shared.size() == 2);

        // Here we can subdivide the wedge into 3 tetrahedra.

        // Find the "middle" diagonal, i.e. the one which
        // consists of the two vertices in diag_shared.
        int middle;
        index_t non_shared_top=-1, non_shared_bottom=-1;
        for(int j=0; j<3; ++j) 
        {
            if(diagonals[j].contains(diag_shared[0]) && diagonals[j].contains(diag_shared[1])) 
            {
                middle = j;
                for(int k=0; k<2; ++k) 
                {
                    if(diagonals[(j+k+1)%3].edge.first != diag_shared[0] && diagonals[(j+k+1)%3].edge.first != diag_shared[1])
                    {
                        non_shared_bottom = diagonals[(j+k+1)%3].edge.first;
                    }

                    else
                    {
                        non_shared_top = diagonals[(j+k+1)%3].edge.second;
                    }
                }

                break;
            }
        }
        assert(non_shared_top >= 0 && non_shared_bottom >= 0);

        /*
        * 2 elements are formed by the three vertices of two connected
        * diagonals plus a fourth vertex which is the one vertex of top/
        * bottom triangle which does not belong to any diagonal.
        *
        * 1 element is formed by the four vertices of two disjoint diagonals.
        */
        index_t v_top, v_bottom;

        // diagonals[middle].edge.first is always one of the bottom vertices
        // diagonals[middle].edge.second is always one of the top vertices

        for(int j=0; j<3; ++j) 
        {
            if(top_triangle[j]!=diagonals[middle].edge.second && top_triangle[j]!=non_shared_top) 
            {
                v_top = top_triangle[j];
                assert(bottom_triangle[j] == diagonals[middle].edge.first);

                break;
            }
        }

        for(int j=0; j<3; ++j) 
        {
            if(bottom_triangle[j]!=diagonals[middle].edge.first && bottom_triangle[j]!=non_shared_bottom) 
            {
                v_bottom = bottom_triangle[j];
                assert(top_triangle[j] == diagonals[middle].edge.second);

                break;
            }
        }

        const int ele1[] = {diagonals[middle].edge.first, diagonals[middle].edge.second, non_shared_top, v_top};
        const int ele2[] = {diagonals[middle].edge.first, diagonals[middle].edge.second, v_bottom, non_shared_bottom};
        const int ele3[] = {diagonals[middle].edge.first, diagonals[middle].edge.second, non_shared_top, non_shared_bottom};

        int bv_bottom, bnsb, bfirst;
        for(int j=0; j<3; ++j) 
        {
            if(v_bottom == bottom_triangle[j]) 
            {
                bv_bottom = bndr[(j+1)%3];
            } 
            
            else if(non_shared_bottom == bottom_triangle[j]) 
            {
                bnsb = bndr[(j+1)%3];
            } 
            
            else 
            {
                bfirst = bndr[(j+1)%3];
            }
        }

        const int ele1_boundary[] = {bndr[3], bv_bottom, bnsb, 0};
        const int ele2_boundary[] = {bfirst, bndr[4], 0, bnsb};
        const int ele3_boundary[] = {bfirst, bv_bottom, 0, 0};

        index_t ele1ID, ele2ID, ele3ID;
        ele1ID = splitCnt;
        ele2ID = ele1ID+1;
        ele3ID = ele1ID+2;

        partition->add_nelist_fix(diagonals[middle].edge.first, ele1ID, threadIdx);
        partition->add_nelist_fix(diagonals[middle].edge.first, ele2ID, threadIdx);
        partition->add_nelist_fix(diagonals[middle].edge.first, ele3ID, threadIdx);

        partition->add_nelist_fix(diagonals[middle].edge.second, ele1ID, threadIdx);
        partition->add_nelist_fix(diagonals[middle].edge.second, ele2ID, threadIdx);
        partition->add_nelist_fix(diagonals[middle].edge.second, ele3ID, threadIdx);

        partition->add_nelist_fix(non_shared_bottom, ele2ID, threadIdx);
        partition->add_nelist_fix(non_shared_bottom, ele3ID, threadIdx);

        partition->add_nelist_fix(non_shared_top, ele1ID, threadIdx);
        partition->add_nelist_fix(non_shared_top, ele3ID, threadIdx);

        partition->add_nelist_fix(v_bottom, ele2ID, threadIdx);

        partition->add_nelist_fix(v_top, ele1ID, threadIdx);

        partition->append_element(ele1);
        partition->append_element(ele2);
        partition->append_element(ele3);

        splitCnt += 3;
    } //end of if for case 1-3

    else
    {
        /*
        ***************
        * Case 1-to-8 *
        ***************
        */

        /*
        * The wedge must by split into 8 tetrahedra with the introduction of
        * a new centroidal vertex. Each tetrahedron is formed by the three
        * vertices of a triangular facet (there are 8 triangular facets: 6 are
        * formed via the bisection of the 3 quadrilaterals of the wedge, 2 are
        * the top and bottom triangles and the centroidal vertex.
        */

        // Allocate space for the centroidal vertex
        index_t cid = pragmatic_omp_atomic_capture(&partition->NNodes, 1);

        const int ele1[] = {diagonals[0].edge.first, ghostDiagonals[0].edge.first, diagonals[0].edge.second, cid};
        const int ele2[] = {diagonals[0].edge.first, diagonals[0].edge.second, ghostDiagonals[0].edge.second, cid};
        const int ele3[] = {diagonals[1].edge.first, ghostDiagonals[1].edge.first, diagonals[1].edge.second, cid};
        const int ele4[] = {diagonals[1].edge.first, diagonals[1].edge.second, ghostDiagonals[1].edge.second, cid};
        const int ele5[] = {diagonals[2].edge.first, ghostDiagonals[2].edge.first, diagonals[2].edge.second, cid};
        const int ele6[] = {diagonals[2].edge.first, diagonals[2].edge.second, ghostDiagonals[2].edge.second, cid};
        const int ele7[] = {top_triangle[0], top_triangle[1], top_triangle[2], cid};
        const int ele8[] = {bottom_triangle[0], bottom_triangle[2], bottom_triangle[1], cid};

        const int ele1_boundary[] = {0, 0, 0, bndr[0]};
        const int ele2_boundary[] = {0, 0, 0, bndr[0]};
        const int ele3_boundary[] = {0, 0, 0, bndr[1]};
        const int ele4_boundary[] = {0, 0, 0, bndr[1]};
        const int ele5_boundary[] = {0, 0, 0, bndr[2]};
        const int ele6_boundary[] = {0, 0, 0, bndr[2]};
        const int ele7_boundary[] = {0, 0, 0, bndr[3]};
        const int ele8_boundary[] = {0, 0, 0, bndr[4]};

        index_t ele1ID, ele2ID, ele3ID, ele4ID, ele5ID, ele6ID, ele7ID, ele8ID;
        ele1ID = splitCnt;
        ele2ID = ele1ID+1;
        ele3ID = ele1ID+2;
        ele4ID = ele1ID+3;
        ele5ID = ele1ID+4;
        ele6ID = ele1ID+5;
        ele7ID = ele1ID+6;
        ele8ID = ele1ID+7;

        for(int j=0; j<3; ++j) 
        {
            partition->NNList[cid].push_back(top_triangle[j]);
            partition->NNList[cid].push_back(bottom_triangle[j]);
            partition->add_nnlist(top_triangle[j], cid);
            partition->add_nnlist(bottom_triangle[j], cid);
        }

        partition->add_nelist_fix(cid, ele1ID, threadIdx);
        partition->add_nelist_fix(cid, ele2ID, threadIdx);
        partition->add_nelist_fix(cid, ele3ID, threadIdx);
        partition->add_nelist_fix(cid, ele4ID, threadIdx);
        partition->add_nelist_fix(cid, ele5ID, threadIdx);
        partition->add_nelist_fix(cid, ele6ID, threadIdx);
        partition->add_nelist_fix(cid, ele7ID, threadIdx);
        partition->add_nelist_fix(cid, ele8ID, threadIdx);

        partition->add_nelist_fix(bottom_triangle[0], ele8ID, threadIdx);
        partition->add_nelist_fix(bottom_triangle[1], ele8ID, threadIdx);
        partition->add_nelist_fix(bottom_triangle[2], ele8ID, threadIdx);

        partition->add_nelist_fix(top_triangle[0], ele7ID, threadIdx);
        partition->add_nelist_fix(top_triangle[1], ele7ID, threadIdx);
        partition->add_nelist_fix(top_triangle[2], ele7ID, threadIdx);

        partition->add_nelist_fix(diagonals[0].edge.first, ele1ID, threadIdx);
        partition->add_nelist_fix(diagonals[0].edge.first, ele2ID, threadIdx);
        partition->add_nelist_fix(diagonals[0].edge.second, ele1ID, threadIdx);
        partition->add_nelist_fix(diagonals[0].edge.second, ele2ID, threadIdx);
        partition->add_nelist_fix(ghostDiagonals[0].edge.first, ele1ID, threadIdx);
        partition->add_nelist_fix(ghostDiagonals[0].edge.second, ele2ID, threadIdx);

        partition->add_nelist_fix(diagonals[1].edge.first, ele3ID, threadIdx);
        partition->add_nelist_fix(diagonals[1].edge.first, ele4ID, threadIdx);
        partition->add_nelist_fix(diagonals[1].edge.second, ele3ID, threadIdx);
        partition->add_nelist_fix(diagonals[1].edge.second, ele4ID, threadIdx);
        partition->add_nelist_fix(ghostDiagonals[1].edge.first, ele3ID, threadIdx);
        partition->add_nelist_fix(ghostDiagonals[1].edge.second, ele4ID, threadIdx);

        partition->add_nelist_fix(diagonals[2].edge.first, ele5ID, threadIdx);
        partition->add_nelist_fix(diagonals[2].edge.first, ele6ID, threadIdx);
        partition->add_nelist_fix(diagonals[2].edge.second, ele5ID, threadIdx);
        partition->add_nelist_fix(diagonals[2].edge.second, ele6ID, threadIdx);
        partition->add_nelist_fix(ghostDiagonals[2].edge.first, ele5ID, threadIdx);
        partition->add_nelist_fix(ghostDiagonals[2].edge.second, ele6ID, threadIdx);

        // Sort all 6 vertices of the wedge by their coordinates.
        // Need to do so to enforce consistency across MPI processes.
        std::map<Coords_t, index_t> coords_map;
        for(int j=0; j<3; ++j) 
        {
            Coords_t cb(partition->get_coords(bottom_triangle[j]));
            coords_map[cb] = bottom_triangle[j];

            Coords_t ct(partition->get_coords(top_triangle[j]));
            coords_map[ct] = top_triangle[j];
        }

        double nc[] = {0.0, 0.0, 0.0}; // new coordinates
        double nm[msize]; // new metric
        const index_t* n = partition->get_element(eid);

        {
            // Calculate the coordinates of the centroidal vertex.
            // We start with a temporary location at the euclidean barycentre of the wedge.
            for(typename std::map<Coords_t, index_t>::const_iterator it=coords_map.begin(); it!=coords_map.end(); ++it) 
            {
                const double *x = partition->get_coords(it->second);

                for(int j=0; j<dim; ++j)
                {
                    nc[j] += x[j];
                }
            }

            for(int j=0; j<dim; ++j) 
            {
                nc[j] /= coords_map.size();
                partition->_coords[cid*dim+j] = nc[j];
            }

            // Interpolate metric at temporary location using the parent element's basis functions
            std::map<Coords_t, index_t> parent_coords;
            for(int j=0; j<nloc; ++j) 
            {
                Coords_t cn(partition->get_coords(n[j]));
                parent_coords[cn] = n[j];
            }

            std::vector<const double *> x;
            std::vector<index_t> sorted_n;

            for(typename std::map<Coords_t, index_t>::const_iterator it=parent_coords.begin(); it!=parent_coords.end(); ++it) 
            {
                x.push_back(partition->get_coords(it->second));
                sorted_n.push_back(it->second);
            }

            // Order of parent element's vertices has changed, so volume might be negative.
            double L = fabs(property->volume(x[0], x[1], x[2], x[3]));

            double ll[4];
            ll[0] = fabs(property->volume(nc  , x[1], x[2], x[3])/L);
            ll[1] = fabs(property->volume(x[0], nc  , x[2], x[3])/L);
            ll[2] = fabs(property->volume(x[0], x[1], nc  , x[3])/L);
            ll[3] = fabs(property->volume(x[0], x[1], x[2], nc  )/L);

            for(int i=0; i<msize; i++) 
            {
                nm[i] = ll[0] * partition->metric[sorted_n[0]*msize+i]+
                        ll[1] * partition->metric[sorted_n[1]*msize+i]+
                        ll[2] * partition->metric[sorted_n[2]*msize+i]+
                        ll[3] * partition->metric[sorted_n[3]*msize+i];
                partition->metric[cid*msize+i] = nm[i];
            }
        }

        // Use the 3D laplacian smoothing kernel to find the barycentre of the wedge in metric space.
        Eigen::Matrix<double, 3, 3> A = Eigen::Matrix<double, 3, 3>::Zero(3, 3);
        Eigen::Matrix<double, 3, 1> q = Eigen::Matrix<double, 3, 1>::Zero(3);

        for(typename std::map<Coords_t, index_t>::const_iterator it=coords_map.begin(); it!=coords_map.end(); ++it) 
        {
            const double *il = partition->get_coords(it->second);
            double x = il[0]-nc[0];
            double y = il[1]-nc[1];
            double z = il[2]-nc[2];

            q[0] += nm[0]*x + nm[1]*y + nm[2]*z;
            q[1] += nm[1]*x + nm[3]*y + nm[4]*z;
            q[2] += nm[2]*x + nm[4]*y + nm[5]*z;

            A(0,0) += nm[0];
            A(0,1) += nm[1];
            A(0,2) += nm[2];
            A(1,1) += nm[3];
            A(1,2) += nm[4];
            A(2,2) += nm[5];
        }

        A(1,0) = A(0,1);
        A(2,0) = A(0,2);
        A(2,1) = A(1,2);

        // Want to solve the system Ap=q to find the new position, p.
        Eigen::Matrix<double, 3, 1> b = Eigen::Matrix<double, 3, 1>::Zero(3);
        Eigen::JacobiSVD<Eigen::Matrix3d, Eigen::HouseholderQRPreconditioner> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);

        b = svd.solve(q);

        for(int i=0; i<3; ++i) 
        {
            nc[i] += b[i];
        }

        // Interpolate metric at new location
        double l[]= {-1, -1, -1, -1};
        double tol=-1;
        std::vector<index_t> sorted_best_e(nloc);
        const index_t *welements[] = {ele1, ele2, ele3, ele4, ele5, ele6, ele7, ele8};

        for(int ele=0; ele<8; ++ele) 
        {
            std::map<Coords_t, index_t> local_coords;
            for(int j=0; j<nloc; ++j) 
            {
                Coords_t cl(partition->get_coords(welements[ele][j]));
                local_coords[cl] = welements[ele][j];
            }

            std::vector<const double *> x;
            std::vector<index_t> sorted_n;
            
            for(typename std::map<Coords_t, index_t>::const_iterator it=local_coords.begin(); it!=local_coords.end(); ++it) 
            {
                x.push_back(partition->get_coords(it->second));
                sorted_n.push_back(it->second);
            }

            double L = fabs(property->volume(x[0], x[1], x[2], x[3]));

            assert(L>0);

            double ll[4];
            ll[0] = fabs(property->volume(nc  , x[1], x[2], x[3])/L);
            ll[1] = fabs(property->volume(x[0], nc  , x[2], x[3])/L);
            ll[2] = fabs(property->volume(x[0], x[1], nc  , x[3])/L);
            ll[3] = fabs(property->volume(x[0], x[1], x[2], nc  )/L);

            double min_l = std::min(std::min(ll[0], ll[1]), std::min(ll[2], ll[3]));
            if(min_l>tol) 
            {
                tol = min_l;

                for(int j=0; j<nloc; ++j) 
                {
                    l[j] = ll[j];
                    sorted_best_e[j] = sorted_n[j];
                }
            }
        }

        for(int i=0; i<dim; ++i) 
        {
            partition->_coords[cid*dim+i] = nc[i];
        }

        for(int i=0; i<msize; ++i)
        {
            partition->metric[cid*msize+i] = l[0]*partition->metric[sorted_best_e[0]*msize+i]+
                                            l[1]*partition->metric[sorted_best_e[1]*msize+i]+
                                            l[2]*partition->metric[sorted_best_e[2]*msize+i]+
                                            l[3]*partition->metric[sorted_best_e[3]*msize+i];
        }

        partition->append_element(ele1);
        partition->append_element(ele2);
        partition->append_element(ele3);
        partition->append_element(ele4);
        partition->append_element(ele5);
        partition->append_element(ele6);
        partition->append_element(ele7);
        partition->append_element(ele8);

        splitCnt += 8;

        // Finally, assign a gnn and owner
        partition->node_owner[cid] = 0;
        partition->lnn2gnn[cid] = cid;                
    }
} //end of MeshPartitions::refine_wedge(Mesh<double>*& partition, const index_t top_triangle[], const index_t bottom_triangle[], const int bndr[], DirectedEdge<index_t>* third_diag, int eid)

//MeshPartitions::ConsistencyCheck()
//
//Task: Check mesh consistency
bool MeshPartitions::ConsistencyCheck()
{/*
    viennamesh::info(1) << "Checking mesh consistency" << std::endl;

    if (original_mesh->get_number_dimensions() == 2)
    {
        std::cout << " 2D Case" << std::endl;
    }

    else 
    {
        std::cout << " 3D Case" << std::endl;

        for (size_t pid = 0; pid < pragmatic_partitions.size(); ++pid)
        {
            std::cout << "   Checking partition " << pid << std::endl;
        }
    }
*/
    return true;
}
//ed of MeshPartitions::ConsistencyCheck()

//----------------------------------------------------------------------------------------------------------------------------------------------//
//                                                                     End                                                                      //
//----------------------------------------------------------------------------------------------------------------------------------------------//

#endif