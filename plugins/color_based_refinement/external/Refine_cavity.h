/*  Copyright (C) 2010 Imperial College London and others.
 *
 *  Please see the AUTHORS file in the main source directory for a
 *  full list of copyright holders.
 *
 *  Gerard Gorman
 *  Applied Modelling and Computation Group
 *  Department of Earth Science and Engineering
 *  Imperial College London
 *
 *  g.gorman@imperial.ac.uk
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following
 *  disclaimer in the documentation and/or other materials provided
 *  with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 *  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 *  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 *  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 */

#ifndef REFINE_CAVITY_H
#define REFINE_CAVITY_H

#include <algorithm>
#include <set>
#include <vector>
#include <queue>

#include <string.h>
#include <inttypes.h>

#include "DeferredOperations.h"
#include "Edge.h"
#include "ElementProperty.h"
#include "Mesh.h"

#include "../outbox.hpp"

/*! \brief Performs 2D/3D mesh refinement.
 *
 */
template<typename real_t, int dim> class Refine_Cavity
{
public:
    /// Default constructor.
    Refine_Cavity(Mesh<real_t> &mesh): nloc(dim+1), msize(dim==2?3:6), nedge(dim==2?3:6)
    {
        _mesh = &mesh;

        size_t NElements = _mesh->get_number_elements();

        //MY IMPLEMENTATION
        int nthreads = 1;
        threadIdx.resize(nthreads);
        //END OF MY IMPLEMENTATION

        // Set the orientation of elements.
        property = NULL;
        for(size_t i=0; i<NElements; i++) {
            const int *n=_mesh->get_element(i);
            if(n[0]<0)
                continue;

            if(dim==2)
                property = new ElementProperty<real_t>(_mesh->get_coords(n[0]),
                                                       _mesh->get_coords(n[1]), _mesh->get_coords(n[2]));
            else if(dim==3)
                property = new ElementProperty<real_t>(_mesh->get_coords(n[0]),
                                                       _mesh->get_coords(n[1]), _mesh->get_coords(n[2]), _mesh->get_coords(n[3]));

            break;
        }

     /*   MPI_Comm comm = _mesh->get_mpi_comm();

        MPI_Comm_size(comm, &nprocs);
        MPI_Comm_rank(comm, &rank);//*/

        //MY IMPLEMENTATION
        nprocs = 1;
        rank = 1;
        //END OF MY IMPLEMENTATION*/

        def_ops = new DeferredOperations<real_t>(_mesh, 1, defOp_scaling_factor);
    }

    /// Default destructor.
    ~Refine_Cavity()
    {
        delete property;
        delete def_ops;
    }
    
    
    /*! New refinement function:
          - edges are split one at a time (dropping template refinement)
          - algo to select edges to refine based on Ibanez's thesis:
               + simulate all possible refinements and save resulting quality
               + local optimization procedure to select edges to refine
          - introduction of the notion of cavity: here a cavity is an edge + neighboring tris/tets
     */


    double refine(double L_max, 
                  std::vector<std::set<int>>& nodes_part_ids, std::vector<int>& l2g_vertices, const int part_id, Outbox& outbox_data,
                  std::vector<int>& partition_colors, std::set<int>& partition_adjcy, const int previous_nelements, std::vector<std::vector<std::vector<int>>>& NNInterfaces,
                  int& global_NNodes, std::unordered_map<int,int>& g2l_vertices, const int NNodes_before_healing, const int NElements_before_healing, 
                  const int iteration)
    {
        size_t nbrSplits = 0;
        std::vector<int> state;

        nbrSplits = select_edges(L_max, state, 
                                 nodes_part_ids, l2g_vertices, part_id, outbox_data,
                                 partition_colors, partition_adjcy, previous_nelements, NNInterfaces,
                                 global_NNodes, g2l_vertices, NNodes_before_healing, NElements_before_healing,
                                 iteration);

       // std::cout << "state.size() " << state.size() << std::endl;

        if (nbrSplits > 0)
        {
            //std::cout << "perform refinement" << std::endl;
            perform_refinement(nbrSplits, &state[0],
                               nodes_part_ids, l2g_vertices, part_id, outbox_data,
                               partition_colors, partition_adjcy, previous_nelements, NNInterfaces,
                               global_NNodes, g2l_vertices, NNodes_before_healing, NElements_before_healing,
                               iteration);
        }

        return nbrSplits;
    }


    double select_edges(double L_max, std::vector<int> &state,
                        std::vector<std::set<int>>& nodes_part_ids, std::vector<int>& l2g_vertices, const int part_id, Outbox& outbox_data,
                        std::vector<int>& partition_colors, std::set<int>& partition_adjcy, const int previous_nelements, std::vector<std::vector<std::vector<int>>>& NNInterfaces,
                        int& global_NNodes, std::unordered_map<int,int>& g2l_vertices, const int NNodes_before_healing, const int NElements_before_healing, 
                        const int iteration)
    {
        //-- I. Simulate the edge splits if edge length > sqrt(2)
        
        //-- hash the edges + array of tags
        //   edges are hashed by lnn1->lnn2 and we only store those where lnn1 < lnn2
        //   note that we could consider gnn1 < gnn2 for halo consistency, but not sure it's useful
        
        int NNodes = _mesh->get_number_nodes();
        std::vector<int> headV2E(NNodes+1);
        headV2E[0] = 0;
        for (int i=0; i<NNodes; ++i) {
            headV2E[i+1] = headV2E[i];
            for (int it=0; it<_mesh->NNList[i].size(); ++it) {
                if (_mesh->NNList[i][it] > i) {
                    headV2E[i+1]++;
                }
            }
        }
        /*
        //DEBUG 
        std::cout << "headV2E" << std::endl;
        for (size_t i = 0 ; i < headV2E.size(); ++i)
        {
            std::cout << i << " " << headV2E[i] << std::endl;
        }
        //END OF DEBUG*/

        int NEdges = headV2E[NNodes];  

        int qual_cnt1 = 0;
        int qual_cnt2 = 0;
        int qual_cnt = 0;
        int length_cnt = 0;

        //-- Loop over the edges
        std::vector<int> ver2edg(NEdges); // this is the hashmap, corresponding to headV2E
        std::vector<int> ver2edg_first(NEdges); // store 1st id of edges until I find better solution TODO
        std::vector<double> qualities(NEdges);
        std::vector<double> lengths(NEdges);
        int cnt=0;
        for (int iVer=0; iVer<NNodes; ++iVer) {
            for (int i=0; i<_mesh->NNList[iVer].size(); ++i) {
                if (_mesh->NNList[iVer][i] > iVer) {

                    int iVer2 = _mesh->NNList[iVer][i];
                    ver2edg[cnt] = iVer2;
                    assert(cnt>=headV2E[iVer] && cnt<headV2E[iVer+1]);
                    double quality_old_cavity = compute_quality_cavity(iVer, iVer2);

                   /* if (_mesh->is_halo_node(iVer) || _mesh->is_halo_node(iVer2)) {
                        qualities[cnt] = -quality_old_cavity;
                        cnt++;
                        continue;
                    }*/

                    double length = _mesh->calc_edge_length_log(iVer, iVer2);
                    lengths[cnt] = length;
                    //std::cout << " length[" << cnt << "] " << length << std::endl;
                    if (length>L_max-1e-10) {
                        //---- simulate edge split
                        //---- compute and save quality of the resulting cavity
                        double worst_new_quality, worst_new_volume;
                        simulate_edge_split(iVer, iVer2, &worst_new_quality, &worst_new_volume);

                        //---- if quality is too bad, reject refinement
                        if (worst_new_quality < quality_old_cavity && worst_new_quality < 0.001) {// TODO set this threshold + check for slivers&co + change criteria
                            qualities[cnt] = -quality_old_cavity;
                            //std::cout << " quality too bad 1" << std::endl;
                            ++qual_cnt1;
                        }
                        if (worst_new_quality < 0.1*quality_old_cavity) {
                            //std::cout << " quality too bad 2" << std::endl;
                            qualities[cnt] = -quality_old_cavity;
                            ++qual_cnt2;
                        }
                        else {
                            qualities[cnt] = worst_new_quality;
                            //std::cout << " iVer " << iVer << " i " << _mesh->NNList[iVer][i] << std::endl;
                            ++qual_cnt;
                        }
                    }
                    else {
                        //std::cout << " length < L_max" << std::endl;
                        //std::cout << " length[" << cnt << "] " << length << std::endl;
                        qualities[cnt] = -quality_old_cavity;
                        ++length_cnt;
                    }
                    assert(cnt<NEdges);
                    cnt++;
                }
            }
            
        }
        /*
        //DEBUG
        std::cout << "ver2edg" << std::endl;
        for (size_t i =0; i < ver2edg.size(); ++i)
        {
            std::cout << i << " " << ver2edg[i] << std::endl;
        }
        //END OF DEBUG*/
        
        //-- II. Select edges to split with local optim procedure
        
        //-- initialize state vector with UNKNOWN
        //    -1 is UNKNOWN, 0 is NOT_IN, 1 is IN
        state.resize(NEdges);
        std::fill(state.begin(), state.end(), -1);
        for (int iEdg=0; iEdg<NEdges; ++iEdg) {
            if (qualities[iEdg]<0) {
                state[iEdg] = 0;
            }
        }
        /*
        //DEBUG
        std::cout << "initial state" << std::endl;
        for (size_t i = 0; i < state.size(); ++i)
        {
            std::cout << i << " " << state[i] << std::endl;
        }
        //END OF DEBUG*/
        
        //-- repeat following procedure until the state of all edges is not UNKNOWN
        
        //---- Loop over the edges v
        int cntSplit = 0;
        int stop = 0;
        
        while ( !stop ) {
            stop = 1;
            int e1, e2; // end vertices of current edge
            e1 = 0;
            for (int iEdg=0; iEdg<NEdges; ++iEdg) {
                if (iEdg >= headV2E[e1+1]){
                    while (iEdg >= headV2E[e1+1])
                       e1++;
                }
                e2 = ver2edg[iEdg];

                if (state[iEdg]>-1) {
                    continue;
                }
                if (qualities[iEdg] < 0) {
                    state[iEdg] = 0;
                    stop = 0;
                    continue;
                }

                //MY IMPLEMENTATION
                //std::cout << " edge number " << iEdg << " e1 " << e1 << " e2 " << e2 << std::endl;
                
                //std::vector<int>::iterator pos_e2 = std::find(_mesh->NNList[e1].begin(), _mesh->NNList[e1].end(), e2);
                auto pos_e2 = std::distance(_mesh->NNList[e1].begin(), std::find(_mesh->NNList[e1].begin(), _mesh->NNList[e1].end(), e2) );
                
                if (e2 >= NNodes_before_healing || e1 >= NNodes_before_healing)  //if > orig_NNodes prevents vertices which have been inserted from
                // neighboring partition during healing to be used again for refinement, thus preventing overrefinement on the interfaces
                {
                    //std::cout << "          continue because inserted from neighbor" << std::endl;
                    state[iEdg] = 0;
                    continue;                    
                }    
                

                //2D case
                if (dim == 2)
                {
                    ;
                }
                //end of 2D case

                //3D case
                else
                {
                    if (NNInterfaces[e1][pos_e2].size() == 1)
                    {
                        if (NNInterfaces[e1][pos_e2][0] != -1)
                        {
                            int target_part_id = NNInterfaces[e1][pos_e2][0];

                            //skip refinement if color of target_partition is smaller than own color
                            if (partition_colors[target_part_id] < partition_colors[part_id])
                            {
                                state[iEdg] = 0;
                                continue;
                            }
                        }
                    }

                    //if edge belongs to more than 2 different partitions, check if own partition color is the smallest,
                    //otherwise do not select this edge for refinement!
                    else if (NNInterfaces[e1][pos_e2].size() > 1)
                    {
                        if (part_id > NNInterfaces[e1][pos_e2][0])
                        {
                            //std::cout << "          continue because of color" << std::endl;
                            state[iEdg] = 0;
                            continue;
                        } //end of if (part_id > NNInterfaces[e1][e2][0])
                    } //end of if (NNInterfaces[e1][e2].size() > 1)
                }
                //end of 3D case
                //END OF MY IMPLEMENTATION

                // find neighboring cavities: edges on neighboring elements
                std::set<int> edges_neighbor;
                std::set<index_t> intersection; 
                std::set_intersection(_mesh->NEList[e1].begin(), _mesh->NEList[e1].end(),
                                      _mesh->NEList[e2].begin(), _mesh->NEList[e2].end(),
                                      std::inserter(intersection, intersection.begin()));
                typename std::set<index_t>::const_iterator elm_it;
                double max_length_cavity = 0;
                for(elm_it=intersection.begin(); elm_it!=intersection.end(); ++elm_it) {
                    int iElm = *elm_it;
                    for (int i=0; i<(dim==2?3:4); ++i) {
                        for (int j=i+1; j<(dim==2?3:4); ++j) {
                            int iVer1 = _mesh->_ENList[nloc*iElm+i];
                            int iVer2 = _mesh->_ENList[nloc*iElm+j];
                            if (iVer1 >= iVer2 ) {
                                int tmp = iVer1;
                                iVer1 = iVer2;
                                iVer2 = tmp;
                            }
                            assert((iVer1+1)<headV2E.size());
                            for (int k=headV2E[iVer1]; k<headV2E[iVer1+1]; ++k){
                                if (ver2edg[k] == iVer2 && k!=iEdg) {
                                    edges_neighbor.insert(k);
                                    max_length_cavity = fmax(max_length_cavity, lengths[k]);
                                }
                            }
                        }
                    }
                }
                // This value of 0.9 is what seems to make it work, but it reduces the interest of the algo...
                // drawback is: if there is a long edge I can't split, I am stuck with its neighbours too
                if (lengths[iEdg] < 0.9*max_length_cavity) {
                    state[iEdg] = 0;
                    stop = 0;
                    continue;
                }                
                
                //------ Loop over the neighboring cavities u
                typename std::set<int>::const_iterator edge_it;
                int cont = 0;
                for(edge_it=edges_neighbor.begin(); edge_it!=edges_neighbor.end(); ++edge_it) {
                    int iEdgNgb = *edge_it;

                    if (state[iEdgNgb] == 1) {
                        cont = 1;
                        break;
                    }
                }
                if (cont==1) {
                    state[iEdg] = 0;
                    stop = 0;
                    continue;
                }
                
                //------ Loop over the neighboring cavities u
                cont = 0;
                for(edge_it=edges_neighbor.begin(); edge_it!=edges_neighbor.end(); ++edge_it) {
                    int iEdgNgb = *edge_it;
                
                    if (state[iEdgNgb] == 0) {
                        continue;
                    }

                    if (qualities[iEdg]<qualities[iEdgNgb]) {
                        cont = 1;
                        break;
                    }

                    // again we could consider gnn1 < gnn2 for halo consistency, but not sure it's useful
                    if (qualities[iEdg]==qualities[iEdgNgb] && iEdgNgb>iEdg) {
                        cont = 1;
                        break;
                    }
                }
                if (cont==1) {
                    continue;
                }

                assert(state[iEdg] != 0);
                //std::cout << "   state[iEdg]= " << iEdg+1 << std::endl;
                state[iEdg] = iEdg+1;
                cntSplit++;
                stop = 0;
            } //end of for (int iEdg=0; iEdg<NEdges; ++iEdg)
        } //end of while ( !stop )
        printf("DEBUG   Number of splits / total number of edges: %d / %d\n", cntSplit, NEdges);
        
        return(cntSplit);
    }
    
    
    /*! Simulate splitting edge e1, e2, and remeshing its cavity in consequence
        give the worst quality and volume of the new cavity
     */
    void simulate_edge_split(int e1, int e2, double * worst_quality, double * worst_volume) {
        
        double quality = 1, volume = 1e10; // L1, Linf, volume
        double newCoords[3], newMetric[6];

        // Calculate the position of the new point. From equation 16 in
        // Li et al, Comp Methods Appl Mech Engrg 194 (2005) 4915-4950.
        real_t x, m;
        const real_t *x0 = _mesh->get_coords(e1);
        const double *m0 = _mesh->get_metric(e1);

        const real_t *x1 = _mesh->get_coords(e2);
        const double *m1 = _mesh->get_metric(e2);

        real_t weight = 1.0/(1.0 + sqrt(property->template length<dim>(x0, x1, m0)/
                                        property->template length<dim>(x0, x1, m1)));

        // Calculate position of new vertex
        for(size_t i=0; i<dim; i++) {
            x = x0[i]+weight*(x1[i] - x0[i]);
            newCoords[i] = x;
        }

        // Interpolate new metric 
        for(size_t i=0; i<msize; i++) {
            m = m0[i]+weight*(m1[i] - m0[i]);
            newMetric[i] = m;
            if(std::isnan(m))
                std::cerr<<"ERROR: metric health is bad in "<<__FILE__<<std::endl
                         <<"m0[i] = "<<m0[i]<<std::endl
                         <<"m1[i] = "<<m1[i]<<std::endl
                         <<"property->length(x0, x1, m0) = "<<property->template length<dim>(x0, x1, m0)<<std::endl
                             <<"property->length(x0, x1, m1) = "<<property->template length<dim>(x0, x1, m1)<<std::endl
                                     <<"weight = "<<weight<<std::endl;
        }
        
        // find the neighboring triangles
        std::set<index_t> intersection;
        std::set_intersection(_mesh->NEList[e1].begin(), _mesh->NEList[e1].end(),
                              _mesh->NEList[e2].begin(), _mesh->NEList[e2].end(),
                              std::inserter(intersection, intersection.begin()));
        
        if (dim==2) {
            
            // loop over these triangles and split them to compute quality
            typename std::set<index_t>::const_iterator tri_it;
            for(tri_it=intersection.begin(); tri_it!=intersection.end(); ++tri_it) {
                const int * v = _mesh->get_element(*tri_it);
                const double * x2, * m2;
                for (int i=0; i<3; ++i) {
                    if (v[i] != e1 && v[i] != e2)  {
                        x2 = _mesh->get_coords(v[i]);
                        m2 = _mesh->get_metric(v[i]);
                    }
                }
                // Now I know new triangles are newVertex,x2,x0 and newVertex,x2,x1 - here we don't care about orientation
                double qual1 = fabs(property->lipnikov(newCoords, x2, x0, newMetric, m2, m0));
                double qual2 = fabs(property->lipnikov(newCoords, x2, x1, newMetric, m2, m1));
                quality = fmin(quality, fmin(qual1, qual2));
                double vol1 = fabs(property->area(newCoords, x2, x0));
                double vol2 = fabs(property->area(newCoords, x2, x1));
                volume = fmin(volume, fmin(vol1, vol2));
            }
            
            
        }
        else {
            // loop over these tets and split them to compute quality
            typename std::set<index_t>::const_iterator tet_it;
            for(tet_it=intersection.begin(); tet_it!=intersection.end(); ++tet_it) {
                const int * v = _mesh->get_element(*tet_it);
                const double * x2, * m2, * x3, * m3;
                int i;
                for (i=0; i<4; ++i) {
                    if (v[i] != e1 && v[i] != e2)  {
                        x2 = _mesh->get_coords(v[i]);
                        m2 = _mesh->get_metric(v[i]);
                        break;
                    }
                }
                for (int j=i+1; j<4; ++j) {
                    if (v[j] != e1 && v[j] != e2)  {
                        x3 = _mesh->get_coords(v[j]);
                        m3 = _mesh->get_metric(v[j]);
                        break;
                    }
                }
                // Now I know new tets are newVertex,x2,x3,x0 and newVertex,x2,x3,x1 - here we don't care about orientation
                double qual1 = fabs(property->lipnikov(newCoords, x2, x3, x0, newMetric, m2, m3, m0));
                double qual2 = fabs(property->lipnikov(newCoords, x2, x3, x1, newMetric, m2, m3, m1));
                quality = fmin(quality, fmin(qual1, qual2));
                double vol1 = fabs(property->volume(newCoords, x2, x3, x0));
                double vol2 = fabs(property->volume(newCoords, x2, x3, x1));
                volume = fmin(volume, fmin(vol1, vol2));
            }
        }
        
        *worst_quality = quality;
        *worst_volume = volume;
    }
    
    
    double compute_quality_cavity(int e1, int e2) {
        
        double quality = 1;
        // find the neighboring elements
        std::set<index_t> intersection;
        std::set_intersection(_mesh->NEList[e1].begin(), _mesh->NEList[e1].end(),
                              _mesh->NEList[e2].begin(), _mesh->NEList[e2].end(),
                              std::inserter(intersection, intersection.begin()));
        
        // loop over theese traingles and split them to compute quality
        typename std::set<index_t>::const_iterator elm_it;
        for(elm_it=intersection.begin(); elm_it!=intersection.end(); ++elm_it) {
            int iElm = *elm_it;
            const double *x0, *x1, *x2, *x3, *m0, *m1, *m2, *m3;
            const int * elm = _mesh->get_element(iElm);
            x0 = _mesh->get_coords(elm[0]);
            m0 = _mesh->get_metric(elm[0]);
            x1 = _mesh->get_coords(elm[1]);
            m1 = _mesh->get_metric(elm[1]);
            x2 = _mesh->get_coords(elm[2]);
            m2 = _mesh->get_metric(elm[2]);
            if (dim==3){
                x3 = _mesh->get_coords(elm[3]);
                m3 = _mesh->get_metric(elm[3]);
            }
            double qual;
            if (dim==2) {
                qual = property->lipnikov(x0, x1, x2, m0, m1, m2);
            }
            else {
                qual = property->lipnikov(x0, x1, x2, x3, m0, m1, m2, m3);
            }
            quality = fmin(quality, qual);
        }
        
        return quality;
    }
    

    /*! Perform one level of refinement.
     *  The whole process is driven by the fact that we only refine 1 edge per 
     *  cavity, so the new connectivity within a cavity is not dependant on the others.
     */
    void perform_refinement(size_t edgeSplitCnt, int *state,
                            std::vector<std::set<int>>& nodes_part_ids, std::vector<int>& l2g_vertices, const int part_id, Outbox& outbox_data,
                            std::vector<int>& partition_colors, std::set<int>& partition_adjcy, const int previous_nelements, std::vector<std::vector<std::vector<int>>>& NNInterfaces,
                            int& global_NNodes, std::unordered_map<int,int>& g2l_vertices, const int NNodes_before_healing, const int NElements_before_healing, 
                            const int iteration)
    {
        /*std::cout << "PERFORM REFINEMENT" << std::endl;//*/
        origNElements = _mesh->get_number_elements();
        size_t origNNodes = _mesh->get_number_nodes();

        //std::cout << "origNNodes " << origNNodes << " NNodes before healing " << NNodes_before_healing << std::endl;

        splitCnt = 0;
        int continueCnt = 0;

        //std::cout << "  initial splitCnt " << splitCnt << " initial edgeSplitCnt " << edgeSplitCnt << std::endl;//*/

        std::vector<int> headV2E(_mesh->get_number_nodes()+1);
        headV2E[0] = 0;
        for (int i=0; i<_mesh->get_number_nodes(); ++i) {
            headV2E[i+1] = headV2E[i];
            for (int it=0; it<_mesh->NNList[i].size(); ++it) {
                if (_mesh->NNList[i][it] > i) {
                    headV2E[i+1]++;
                }
            }
        } 

        //std::cout << " NEdges before refinement in refine_cavity " << headV2E[_mesh->get_number_nodes()] << std::endl;
        
        newVertices.resize(edgeSplitCnt);
        //std::cout << "  initial newVertices.size() " << newVertices.size() << std::endl;//*/
        size_t newNNodes = _mesh->NNodes+edgeSplitCnt;
        if(_mesh->_coords.size()<newNNodes*dim) {
            _mesh->_coords.resize(newNNodes*dim);
            _mesh->metric.resize(newNNodes*msize);
            _mesh->NNList.resize(newNNodes);
            _mesh->NEList.resize(newNNodes);
            _mesh->node_owner.resize(newNNodes);
            _mesh->lnn2gnn.resize(newNNodes);
        }

        //MY OWN IMPLEMENTATION
        auto target_part_id = 0;
        int tid = 0;
        //size_t reserve_size = nedge*origNNodes/nthreads;
        size_t reserve_size = newNNodes;
        edge_neighbor_partitions.clear();
        edge_neighbor_partitions.reserve(reserve_size);
        //END OF MY OWN IMPLEMENTATION

        int cnt = 0;
        for(size_t i=0; i<origNNodes; ++i) {
            for(size_t it=0; it<_mesh->NNList[i].size(); ++it) {
                std::set<int> edge_neighs_tmp;
                index_t otherVertex = _mesh->NNList[i][it];
                
                //MY OWN IMPLEMENTATION
                if (otherVertex >= NNodes_before_healing || i >= NNodes_before_healing)  //if > orig_NNodes prevents vertices which have been inserted from
                // neighboring partition during healing to be used again for refinement, thus preventing overrefinement on the interfaces
                {
                    /*std::cout << "          continue" << std::endl;//*/
                    //++continueCnt;
                    ++cnt;
                    continue;                    
                }  //*/        

                if (i < otherVertex)
                {
                    //std::cout << "state[cnt] " << state[cnt] << std::endl;   
                    if (state[cnt] > 0) 
                    {
                        if (dim == 2)
                        {
                            //edge belongs either to only one partition or two different partitions
                            if (NNInterfaces[i][it].size() == 1)
                            {                                    
                                if (NNInterfaces[i][it][0] != -1) //adapt edge on the interface of the partition
                                {
                                    target_part_id = NNInterfaces[i][it][0];
                                    /*std::cout << "          adapt interface " << i << " " << otherVertex << " " << target_part_id << std::endl;//*/
                                    refine_edge(i, otherVertex, tid, outbox_data, l2g_vertices, target_part_id);
                                    edge_neighs_tmp.insert(target_part_id);
                                    splitCnt++;
                                }
                                else 
                                {
                                    /*std::cout << "          refine edge " << i << " " << otherVertex << std::endl;//*/
                                    refine_edge(i, otherVertex);
                                    splitCnt++;
                                    //edge_neighs_tmp.push_back(-1);
                                }                                    
                            } //end of edge belongs either to only one partition or two different partitions

                            //if edge belongs to more than 2 different partitions, check if own partition color is the smallest,
                            //otherwise do not refine!
                            else
                            {
                                /*std::cout << "  more partitions to update" << std::endl;//*/
                                if (part_id <= NNInterfaces[i][it][0])
                                {
                                    for (size_t edge_iter = 0; edge_iter < NNInterfaces[i][it].size(); ++edge_iter)
                                    {
                                        if (NNInterfaces[i][it][edge_iter] != part_id)
                                        {
                                            edge_neighs_tmp.insert(NNInterfaces[i][it][edge_iter]);
                                        }
                                    }

                                    target_part_id = NNInterfaces[i][it][0];
                                    refine_edge(i, otherVertex, tid, outbox_data, l2g_vertices, target_part_id);
                                    splitCnt++;
                                    edge_neighs_tmp.insert(target_part_id);
                                }
                            } //end of else
                        } //end of if (dim==2)

                        //3D case
                        else 
                        {
                            //edge belongs either to only one partition or two different partitions
                            if (NNInterfaces[i][it].size() == 1)
                            {  
                                /*std::cout << "  3D of " << i << " and " << otherVertex << " with new ID " << splitCnt+origNNodes-1 << " in partition " << part_id << std::endl;

                                std::cout << "    only 1 partition to update" << std::endl;//*/

                                if (NNInterfaces[i][it][0] != -1) //adapt edge on the interface of the partition
                                {
                                    target_part_id = NNInterfaces[i][it][0];
                                    /*std::cout << "          adapt interface " << i << " " << otherVertex << " " << target_part_id << std::endl;//*/
                                    refine_edge(i, otherVertex, tid, outbox_data, l2g_vertices, target_part_id);
                                    edge_neighs_tmp.insert(target_part_id);   
                                    ++splitCnt;                                     
                                }
                                else 
                                {
                                    //std::cout << "  3D of " << i << " and " << otherVertex << " with new ID " << splitCnt+origNNodes-1 << " in partition " << part_id << std::endl;

                                    /*std::cout << "          refine edge " << i << " " << otherVertex << " " << -1 << std::endl;//*/
                                    refine_edge(i, otherVertex);
                                    ++splitCnt;
                                    edge_neighs_tmp.insert(-1);
                                }
                            } //end of edge belongs either to only one partition or two different partitions

                            //if edge belongs to more than 2 different partitions, check if own partition color is the smallest,
                            //otherwise do not refine!
                            else
                            {
                                if (part_id <= NNInterfaces[i][it][0])
                                {   
                                    /*std::cout << "  3D of " << i << " and " << otherVertex << " with new ID " << splitCnt+origNNodes-1 << " in partition " << part_id << std::endl;
                                    std::cout << "    more partitions to update " << i << " " << otherVertex << " " << std::endl;//*/
                                        
                                    for (size_t edge_iter = 0; edge_iter < NNInterfaces[i][it].size(); ++edge_iter)
                                    {
                                        if (NNInterfaces[i][it][edge_iter] != part_id)
                                        {
                                            edge_neighs_tmp.insert(NNInterfaces[i][it][edge_iter]);
                                            //std::cout << "    " << edge_iter << ": " << NNInterfaces[i][it][edge_iter] << std::endl;
                                        }
                                    }
                                    
                                    target_part_id = NNInterfaces[i][it][0];
                                    refine_edge(i, otherVertex, tid, outbox_data, l2g_vertices, edge_neighs_tmp);
                                    ++splitCnt;
                                }

                                else
                                { 
                                    /*std::cout << "  3D of " << i << " and " << otherVertex << " in partition " << part_id << std::endl;
                                    std::cout << "    MUST NOT REFINE " << std::endl;//*/
                                        
                                    --splitCnt;
                                    continue;
                                }
                            }// end of else
                        } //end of 3D case

                        edge_neighbor_partitions.push_back(edge_neighs_tmp);
                        //END OF MY IMPLEMENTATION*/
                    /*
                     if (i < otherVertex) {
                            if (state[cnt] > 0) {
                                std::cout << "ref " << i << " " << otherVertex << std::endl;
                                refine_edge(i, otherVertex);
                                splitCnt++;
                            }//
                            cnt++;//*/

                    //MY IMPLEMENTATION
                    }//end of if (state[cnt] > 0)*/
                    cnt++;
                    //END OF MY IMPLEMENTATION
                }//end of if (i < otherVertex)
            } //end of for(size_t it=0; it<_mesh->NNList[i].size(); ++it)
        } //end of for(size_t i=0; i<origNNodes; ++i) 

        //MY IMPLEMENTATION
        threadIdx[tid] =_mesh->NNodes;
        //END OF MY IMPLEMENTATION*/
        /*std::cout << " contCnt " << continueCnt << " cnt " << cnt << std::endl;
        std::cout << " splitCnt " << splitCnt << " edgeSplitCnt " << edgeSplitCnt << std::endl;//*/

        _mesh->NNodes += splitCnt;
        assert(splitCnt==edgeSplitCnt);

        //MY IMPLEMENTATION
        auto glob_old_NNodes = 0;
        #pragma omp atomic capture
        {                
            glob_old_NNodes = global_NNodes;
            global_NNodes += splitCnt;
        }
        //END OF MY IMPLEMENTATION*/

        // Fix IDs of new vertices
        assert(newVertices.size()==splitCnt);
        for(size_t i=0; i<splitCnt; i++) {
            newVertices[i].id = origNNodes+i;

            //MY IMPLEMENTATION
            l2g_vertices.push_back(glob_old_NNodes+i);

            std::set<int> node_part_id_tmp_set;
            node_part_id_tmp_set.insert(part_id);
            for (auto en_iter : edge_neighbor_partitions[i])
            {
                if (en_iter != -1)
                {
                    node_part_id_tmp_set.insert(en_iter);
                }
            }

            #pragma omp critical
            {
                nodes_part_ids[l2g_vertices[threadIdx[tid]+i]]=node_part_id_tmp_set;
            }

            g2l_vertices.insert(std::make_pair(glob_old_NNodes+i, threadIdx[tid]+i));    
            //END OF MY IMPLEMENTATION*/
        }

        /*
         *   Element refinement: add new connectivity + elements
         *   The number of elements affected by the splits cannot be guessed a priori
         *   so we choose to add them to a temporary buffer.
         *   Another option would be to count them first to avoid moving memory...
         */

        splitCnt = 0;
        newElements.clear();
        newBoundaries.clear();
        newQualities.clear();
        newElements.reserve(dim*dim*origNElements);
        newBoundaries.reserve(dim*dim*origNElements);
        newQualities.reserve(origNElements);

        for(size_t i=0; i<edgeSplitCnt; ++i) {
            index_t vid = newVertices[i].id;
            index_t firstid = newVertices[i].edge.first;
            index_t secondid = newVertices[i].edge.second;

            /*
             * Update NNList for newly created vertices. This has to be done here, it cannot be
             * done during element refinement, because a split edge is shared between two elements
             * and we run the risk that these updates will happen twice, once for each element.
             */
            
            addNN(vid,firstid);
            addNN(vid,secondid);

            //MY IMPLEMENTATION
            //Now update the corresponding data in NNInterfaces            
            NNInterfaces.push_back(std::vector<std::vector<int>>());

            NNInterfaces[vid].resize(_mesh->NNList[vid].size());

            for (size_t nn_iter = 0; nn_iter < NNInterfaces[vid].size(); ++nn_iter)
            {
                for (auto enp_iter : edge_neighbor_partitions[i])
                {
                    NNInterfaces[vid][nn_iter].push_back(enp_iter);
                }
            }
            
            //remNN(firstid, secondid)
            auto position = std::find(_mesh->NNList[firstid].begin(), _mesh->NNList[firstid].end(), secondid);
            int vec_pos_firstid;
            if (position != _mesh->NNList[firstid].end())
            {
                vec_pos_firstid = position - _mesh->NNList[firstid].begin();
                NNInterfaces[firstid].erase( NNInterfaces[firstid].begin() + vec_pos_firstid );
            }
            else
            {
                std::cout << "   did not find 1" << std::endl;
            }

            //addNN(firstid, vid)
            //update now the newly created nninterfaces vector  
            std::vector<int> NNInterfaces_firstid_tmp;

            for (auto iter : edge_neighbor_partitions[i])
            {
                NNInterfaces_firstid_tmp.push_back(iter);
            }

            NNInterfaces[firstid].insert(NNInterfaces[firstid].begin()+vec_pos_firstid, NNInterfaces_firstid_tmp);
            
            //removeNN(secondid, firstid)
            position = std::find(_mesh->NNList[secondid].begin(), _mesh->NNList[secondid].end(), firstid);
            int vec_pos_secondid;
            if (position != _mesh->NNList[secondid].end())
            {
                vec_pos_secondid = position - _mesh->NNList[secondid].begin();
                NNInterfaces[secondid].erase( NNInterfaces[secondid].begin() + vec_pos_secondid );
            }
            else
            {
                std::cout << "   did not find 2" << std::endl;
            }

            //addNN(secondid, vid)
            //update now the newly created nninterfaces vector  
            std::vector<int> NNInterfaces_secondid_tmp;

            for (auto iter : edge_neighbor_partitions[i])
            {
                NNInterfaces_secondid_tmp.push_back(iter);
            }

            NNInterfaces[secondid].insert(NNInterfaces[secondid].begin()+vec_pos_secondid, NNInterfaces_secondid_tmp);
            //END OF MY IMPLEMENTATION
                    
            remNN(firstid, secondid);
            addNN(firstid, vid);
            remNN(secondid, firstid);
            addNN(secondid, vid);//*/
            /*
            _mesh->NNList[vid].push_back(firstid);
            _mesh->NNList[vid].push_back(secondid);//*/
/*
            def_ops->remNN(firstid, secondid, tid);
            def_ops->addNN(firstid, vid, tid);
            def_ops->remNN(secondid, firstid, tid);
            def_ops->addNN(secondid, vid, tid);//*/

            
            /*
             * Actual element refinement
             */

            // Find which elements share this edge and split them
            std::set<index_t> elm_around_split_edge;
            std::set_intersection(_mesh->NEList[firstid].begin(), _mesh->NEList[firstid].end(),
                                  _mesh->NEList[secondid].begin(), _mesh->NEList[secondid].end(),
                                  std::inserter(elm_around_split_edge, elm_around_split_edge.begin()));

            typename std::set<index_t>::const_iterator element;
            for(element=elm_around_split_edge.begin(); element!=elm_around_split_edge.end(); ++element) {
                index_t eid = *element;

                refine_element(eid, i);
            }

            /*
             *  Fix new vertex ownership
             */

            if(nprocs==1) {
                _mesh->node_owner[vid] = 0;
                _mesh->lnn2gnn[vid] = vid;
            } else {
                int owner0 = _mesh->node_owner[firstid];
                int owner1 = _mesh->node_owner[secondid];
                assert(owner0 == owner1);
                int owner = std::min(owner0, owner1);
                _mesh->node_owner[vid] = owner;

                if(_mesh->node_owner[vid] == rank)
                    _mesh->lnn2gnn[vid] = _mesh->gnn_offset+vid;
            }
        }

        _mesh->NElements += splitCnt;
        
        if(_mesh->_ENList.size()<_mesh->NElements*nloc) {
            _mesh->_ENList.resize(_mesh->NElements*nloc);
            _mesh->boundary.resize(_mesh->NElements*nloc);
            _mesh->quality.resize(_mesh->NElements);
        }

        // Append new elements to the mesh and commit deferred operations
        memcpy(&_mesh->_ENList[nloc*origNElements], &newElements[0], nloc*splitCnt*sizeof(index_t));
        memcpy(&_mesh->boundary[nloc*origNElements], &newBoundaries[0], nloc*splitCnt*sizeof(int));
        memcpy(&_mesh->quality[origNElements], &newQualities[0], splitCnt*sizeof(double));

        // Update halo.
        if(nprocs>1) {
            
            std::vector< std::set< DirectedEdge<index_t> > > recv_additional(nprocs), send_additional(nprocs);

            for(size_t i=0; i<edgeSplitCnt; ++i)
            {
                DirectedEdge<index_t> *vert = &newVertices[i];

                if(_mesh->node_owner[vert->id] != rank) {
                    // Vertex is owned by another MPI process, so prepare to update recv and recv_halo.
                    // Only update them if the vertex is actually visible by *this* MPI process,
                    // i.e. if at least one of its neighbours is owned by *this* process.
                    bool visible = false;
                    for(typename std::vector<index_t>::const_iterator neigh=_mesh->NNList[vert->id].begin(); neigh!=_mesh->NNList[vert->id].end(); ++neigh) {
                        if(_mesh->is_owned_node(*neigh)) {
                            visible = true;
                            DirectedEdge<index_t> gnn_edge(_mesh->lnn2gnn[vert->edge.first], _mesh->lnn2gnn[vert->edge.second], vert->id);
                            recv_additional[_mesh->node_owner[vert->id]].insert(gnn_edge);
                            break;
                        }
                    }
                } else {
                    // Vertex is owned by *this* MPI process, so check whether it is visible by other MPI processes.
                    // The latter is true only if both vertices of the original edge were halo vertices.
                    if(_mesh->is_halo_node(vert->edge.first) && _mesh->is_halo_node(vert->edge.second)) {
                        // Find which processes see this vertex
                        std::set<int> processes;
                        for(typename std::vector<index_t>::const_iterator neigh=_mesh->NNList[vert->id].begin(); neigh!=_mesh->NNList[vert->id].end(); ++neigh)
                            processes.insert(_mesh->node_owner[*neigh]);

                        processes.erase(rank);

                        for(typename std::set<int>::const_iterator proc=processes.begin(); proc!=processes.end(); ++proc) {
                            DirectedEdge<index_t> gnn_edge(_mesh->lnn2gnn[vert->edge.first], _mesh->lnn2gnn[vert->edge.second], vert->id);
                            send_additional[*proc].insert(gnn_edge);
                        }
                    }
                }
            }

            // Append vertices in recv_additional and send_additional to recv and send.
            // Mark how many vertices are added to each of these vectors.
            std::vector<size_t> recv_cnt(nprocs, 0), send_cnt(nprocs, 0);

            for(int i=0; i<nprocs; ++i)
            {
                recv_cnt[i] = recv_additional[i].size();
                for(typename std::set< DirectedEdge<index_t> >::const_iterator it=recv_additional[i].begin(); it!=recv_additional[i].end(); ++it) {
                    _mesh->recv[i].push_back(it->id);
                    _mesh->recv_halo.insert(it->id);
                }

                send_cnt[i] = send_additional[i].size();
                for(typename std::set< DirectedEdge<index_t> >::const_iterator it=send_additional[i].begin(); it!=send_additional[i].end(); ++it) {
                    _mesh->send[i].push_back(it->id);
                    _mesh->send_halo.insert(it->id);
                }
            }

            // Update global numbering
            _mesh->update_gappy_global_numbering(recv_cnt, send_cnt);

            // Now that the global numbering has been updated, update send_map and recv_map.
            for(int i=0; i<nprocs; ++i)
            {
                for(typename std::set< DirectedEdge<index_t> >::const_iterator it=recv_additional[i].begin(); it!=recv_additional[i].end(); ++it)
                    _mesh->recv_map[i][_mesh->lnn2gnn[it->id]] = it->id;

                for(typename std::set< DirectedEdge<index_t> >::const_iterator it=send_additional[i].begin(); it!=send_additional[i].end(); ++it)
                    _mesh->send_map[i][_mesh->lnn2gnn[it->id]] = it->id;
            }

            _mesh->trim_halo();
            
        }

#if !defined NDEBUG
        if(dim==2) {
            // Fix orientations of new elements.
            size_t NElements = _mesh->get_number_elements();

            for(size_t i=0; i<NElements; i++) {
                index_t n0 = _mesh->_ENList[i*nloc];
                if(n0<0)
                    continue;

                index_t n1 = _mesh->_ENList[i*nloc + 1];
                index_t n2 = _mesh->_ENList[i*nloc + 2];

                const real_t *x0 = &_mesh->_coords[n0*dim];
                const real_t *x1 = &_mesh->_coords[n1*dim];
                const real_t *x2 = &_mesh->_coords[n2*dim];

                real_t av = property->area(x0, x1, x2);

                if(av<=0) {
                    std::cerr<<"ERROR: inverted element in refinement"<<std::endl
                             <<"element = "<<n0<<", "<<n1<<", "<<n2<<std::endl;
                    exit(-1);
                }
            }
        }
#endif
    }

private:

    inline void refine_edge(index_t n0, index_t n1)
    {
        /*std::cout << "refine_edge pragmatic" << std::endl;//*/
        if(_mesh->lnn2gnn[n0] > _mesh->lnn2gnn[n1]) {
            // Needs to be swapped because we want the lesser gnn first.
            index_t tmp_n0=n0;
            n0=n1;
            n1=tmp_n0;
        }
        /*std::cout << "  splitCnt " << splitCnt << std::endl;
        std::cout << "  newVertices.size " << newVertices.size() << std::endl;//*/
        newVertices[splitCnt] = DirectedEdge<index_t>(n0, n1);

        // Calculate the position of the new point. From equation 16 in
        // Li et al, Comp Methods Appl Mech Engrg 194 (2005) 4915-4950.
        real_t x, m;
        const real_t *x0 = _mesh->get_coords(n0);
        const double *m0 = _mesh->get_metric(n0);

        const real_t *x1 = _mesh->get_coords(n1);
        const double *m1 = _mesh->get_metric(n1);

        real_t weight = 1.0/(1.0 + sqrt(property->template length<dim>(x0, x1, m0)/
                                        property->template length<dim>(x0, x1, m1)));

        // Calculate position of new vertex and append it to OMP thread's temp storage
        /*std::cout << "    newCoords" << std::endl;//*/
        for(size_t i=0; i<dim; i++) {
            x = x0[i]+weight*(x1[i] - x0[i]);
            /*std::cout << "      " << x << std::endl;//*/
            _mesh->_coords[(_mesh->NNodes+splitCnt)*dim+i] = x;
        }

        /*std::cout << "    new local ID " <<  _mesh->get_number_nodes() + splitCnt << std::endl;//*/

#if 0
        // TODO HACK CAD
        double newCrd[3];
        for(size_t i=0; i<dim; i++)
            newCrd[i] = x0[i]+weight*(x1[i] - x0[i]);
        if (_mesh->get_isOnBoundary(n0) == 1 && _mesh->get_isOnBoundary(n1) == 1){
            double r = 0.5/sqrt(newCrd[0]*newCrd[0]+newCrd[1]*newCrd[1]);
            newCrd[0] *= r;
            newCrd[1] *= r;
        }
        for(size_t i=0; i<dim; i++)
            newCoords[tid].push_back(newCrd[i]);
#endif

        // Interpolate new metric and append it to OMP thread's temp storage
        for(size_t i=0; i<msize; i++) {
            m = m0[i]+weight*(m1[i] - m0[i]);
            _mesh->metric[(_mesh->NNodes+splitCnt)*msize+i] = m;
            if(std::isnan(m))
                std::cerr<<"ERROR: metric health is bad in "<<__FILE__<<std::endl
                         <<"m0[i] = "<<m0[i]<<std::endl
                         <<"m1[i] = "<<m1[i]<<std::endl
                         <<"property->length(x0, x1, m0) = "<<property->template length<dim>(x0, x1, m0)<<std::endl
                             <<"property->length(x0, x1, m1) = "<<property->template length<dim>(x0, x1, m1)<<std::endl
                                     <<"weight = "<<weight<<std::endl;
        }
    } //end of refine_edge standard implementation
    
    //MY IMPLEMENTATION
    //refine edge with a single target_part_id
    inline void refine_edge(index_t n0, index_t n1, int tid, Outbox& outbox_data, std::vector<int>& l2g_vertices, int target_part_id)
    {
        /*std::cout << "refine_edge my version" << std::endl;//*/
        //if(_mesh->lnn2gnn[n0] > _mesh->lnn2gnn[n1]) { //THIS IS PRAGMATIC DEFAULT IMPLEMENTATION
        if (n0 > n1){
            // Needs to be swapped because we want the lesser gnn first.
            index_t tmp_n0=n0;
            n0=n1;
            n1=tmp_n0;
        }

        newVertices[splitCnt] = DirectedEdge<index_t>(n0, n1);

        // Calculate the position of the new point. From equation 16 in
        // Li et al, Comp Methods Appl Mech Engrg 194 (2005) 4915-4950.
        real_t x, m;
        const real_t *x0 = _mesh->get_coords(n0);
        const double *m0 = _mesh->get_metric(n0);

        const real_t *x1 = _mesh->get_coords(n1);
        const double *m1 = _mesh->get_metric(n1);

        real_t weight = 1.0/(1.0 + sqrt(property->template length<dim>(x0, x1, m0)/
                                        property->template length<dim>(x0, x1, m1)));

        // Calculate position of new vertex and append it to OMP thread's temp storage
        /*std::cout << "    newCoords" << std::endl;//*/
        for(size_t i=0; i<dim; i++) {
            x = x0[i]+weight*(x1[i] - x0[i]);
            /*std::cout << "      " << x << std::endl;//*/
            _mesh->_coords[(_mesh->NNodes+splitCnt)*dim+i]= x;
            //newCoords.push_back(x);
        }

        /*std::cout << "    new local ID " <<  _mesh->get_number_nodes() + splitCnt << std::endl;//*/
        outbox_data(target_part_id, l2g_vertices[n0], l2g_vertices[n1], _mesh->get_number_nodes() + splitCnt);

        // Interpolate new metric and append it to OMP thread's temp storage
        for(size_t i=0; i<msize; i++) {
            m = m0[i]+weight*(m1[i] - m0[i]);
            _mesh->metric[(_mesh->NNodes+splitCnt)*msize+i] = m;
            //newMetric.push_back(m);
            if(std::isnan(m))
                std::cerr<<"ERROR: metric health is bad in "<<__FILE__<<std::endl
                         <<"m0[i] = "<<m0[i]<<std::endl
                         <<"m1[i] = "<<m1[i]<<std::endl
                         <<"property->length(x0, x1, m0) = "<<property->template length<dim>(x0, x1, m0)<<std::endl
                             <<"property->length(x0, x1, m1) = "<<property->template length<dim>(x0, x1, m1)<<std::endl
                                     <<"weight = "<<weight<<std::endl;
        }
    } //end of refine_edge with a single target_part_id*/
    
    //refine_edge with multiple target_part_ids
    inline void refine_edge(index_t n0, index_t n1, int tid, Outbox& outbox_data, std::vector<int>& l2g_vertices, std::set<int> target_part_ids)
    {
        /*std::cout << "refine_edge my version multiple target_part_ids" << std::endl;//*/

        //if(_mesh->lnn2gnn[n0] > _mesh->lnn2gnn[n1]) { //THIS IS PRAGMATIC DEFAULT IMPLEMENTATION
        if (n0 > n1){
            // Needs to be swapped because we want the lesser gnn first.
            index_t tmp_n0=n0;
            n0=n1;
            n1=tmp_n0;
        }

        newVertices[splitCnt] = DirectedEdge<index_t>(n0, n1);

        // Calculate the position of the new point. From equation 16 in
        // Li et al, Comp Methods Appl Mech Engrg 194 (2005) 4915-4950.
        real_t x, m;
        const real_t *x0 = _mesh->get_coords(n0);
        const double *m0 = _mesh->get_metric(n0);

        const real_t *x1 = _mesh->get_coords(n1);
        const double *m1 = _mesh->get_metric(n1);

        real_t weight = 1.0/(1.0 + sqrt(property->template length<dim>(x0, x1, m0)/
                                        property->template length<dim>(x0, x1, m1)));

        // Calculate position of new vertex and append it to OMP thread's temp storage
        /*std::cout << "    newCoords" << std::endl;//*/
        for(size_t i=0; i<dim; i++) {
            x = x0[i]+weight*(x1[i] - x0[i]);
            //newCoords.push_back(x);
            //std::cout << "      " << x << std::endl;*/
            _mesh->_coords[(_mesh->NNodes+splitCnt)*dim+i] = x;
        }

        //std::cout << "    new local ID " <<  _mesh->get_number_nodes() + splitCnt << std::endl;

        //push data into outbox for each of the target_partition_ids
        for (auto it : target_part_ids)
        {
            outbox_data(it, l2g_vertices[n0], l2g_vertices[n1], _mesh->get_number_nodes() + splitCnt);
        } 

        // Interpolate new metric and append it to OMP thread's temp storage
        for(size_t i=0; i<msize; i++) {
            m = m0[i]+weight*(m1[i] - m0[i]);
            //newMetric.push_back(m);
            _mesh->metric[(_mesh->NNodes+splitCnt)*msize+i] = m;
            if(std::isnan(m))
                std::cerr<<"ERROR: metric health is bad in "<<__FILE__<<std::endl
                         <<"m0[i] = "<<m0[i]<<std::endl
                         <<"m1[i] = "<<m1[i]<<std::endl
                         <<"property->length(x0, x1, m0) = "<<property->template length<dim>(x0, x1, m0)<<std::endl
                             <<"property->length(x0, x1, m1) = "<<property->template length<dim>(x0, x1, m1)<<std::endl
                                     <<"weight = "<<weight<<std::endl;
        }
    } //end of refine_edge with multiple target_part_ids*/
    
    //END OF MY IMPLEMENTATION


#ifdef HAVE_BOOST_UNORDERED_MAP_HPP
    typedef boost::unordered_map<index_t, int> boundary_t;
#else
    typedef std::map<index_t, int> boundary_t;
#endif

    inline void refine_element(size_t eid, int i)
    {
        if (dim==2)
            refine2D_1(eid, i);
        else
            refine3D_1(eid, i);
    }

    inline void refine2D_1(int eid, int iEdgeSplit)
    {
        // Single edge split.

        const int *n=_mesh->get_element(eid);
        const int *boundary=&(_mesh->boundary[eid*nloc]);

        // Edge that is being split
        index_t vid = newVertices[iEdgeSplit].id;
        index_t firstid = newVertices[iEdgeSplit].edge.first;
        index_t secondid = newVertices[iEdgeSplit].edge.second;

        int rotated_ele[3];
        int rotated_boundary[3];
        index_t vertexID = vid;
        for (int j=0; j<3; j++)
            if (n[j] != firstid && n[j] != secondid) {
                rotated_ele[0] = n[j];
                rotated_ele[1] = n[(j+1)%3];
                rotated_ele[2] = n[(j+2)%3];

                rotated_boundary[0] = boundary[j];
                rotated_boundary[1] = boundary[(j+1)%3];
                rotated_boundary[2] = boundary[(j+2)%3];

                break;
            }
        assert(vertexID!=-1);

        const index_t ele0[] = {rotated_ele[0], rotated_ele[1], vertexID};
        const index_t ele1[] = {rotated_ele[0], vertexID, rotated_ele[2]};

        const index_t ele0_boundary[] = {rotated_boundary[0], 0, rotated_boundary[2]};
        const index_t ele1_boundary[] = {rotated_boundary[0], rotated_boundary[1], 0};

        index_t ele1ID;
        ele1ID = splitCnt;

        // Add rotated_ele[0] to vertexID's NNList
        addNN(vertexID, rotated_ele[0]);
        // Add vertexID to rotated_ele[0]'s NNList
        addNN(rotated_ele[0], vertexID);

        // Put ele1 in rotated_ele[0]'s NEList
        addNE(rotated_ele[0], ele1ID+origNElements);

        // Put eid and ele1 in vertexID's NEList
        addNE(vertexID, eid);
        addNE(vertexID, ele1ID+origNElements);

        // Replace eid with ele1 in rotated_ele[2]'s NEList
        remNE(rotated_ele[2], eid);
        addNE(rotated_ele[2], ele1ID+origNElements);

        assert(ele0[0]>=0 && ele0[1]>=0 && ele0[2]>=0);
        assert(ele1[0]>=0 && ele1[1]>=0 && ele1[2]>=0);

        replace_element(eid, ele0, ele0_boundary);
        append_element(ele1, ele1_boundary);
        splitCnt += 1;
    }

    inline void refine3D_1(int eid, int iEdgeSplit)
    {
        const int *n=_mesh->get_element(eid);
        const int *boundary=&(_mesh->boundary[eid*nloc]);

        // Edge that is being split
        index_t vid = newVertices[iEdgeSplit].id;
        index_t firstid = newVertices[iEdgeSplit].edge.first;
        index_t secondid = newVertices[iEdgeSplit].edge.second;

        boundary_t b;
        for (int j=0; j<nloc; ++j)
            b[n[j]] = boundary[j];

        // Find the opposite edge
        index_t oe[2], position[2];
        for (int j=0, pos=0; j<4; j++)
            if (n[j] != firstid && n[j] != secondid){
                position[pos] = j;
                oe[pos++] = n[j];
            }

        // First split the two facets if they belong to me (normal vector pointing to 4th vertex)

        for (int i=0; i<2; ++i){
            int flag = 0;
            int facet[3] = {firstid, secondid, oe[i]};
            std::sort(facet, facet+sizeof(facet)/sizeof(facet[0]));

            if (_mesh->boundary[4*eid+position[(i+1)%2]] > 0 )
                flag = 1;
            else {
                // compute the triple product of 2 vectors of the facet, and a vector going to the 4th vertex
                // this is a determinant, of which the explicit formula can be found in wikipedia
                const double * f0 = &_mesh->_coords[facet[0]*3];
                const double * f1 = &_mesh->_coords[facet[1]*3];
                const double * f2 = &_mesh->_coords[facet[2]*3];
                const double * ov = &_mesh->_coords[oe[(i+1)%2]*3];
                const double f0f1[3] = {f1[0]-f0[0], f1[1]-f0[1], f1[2]-f0[2]};
                const double f0f2[3] = {f2[0]-f0[0], f2[1]-f0[1], f2[2]-f0[2]};
                const double f0ov[3] = {ov[0]-f0[0], ov[1]-f0[1], ov[2]-f0[2]};
                const double det = (f0f1[0]*f0f2[1]*f0ov[2] + f0f2[0]*f0ov[1]*f0f1[2] + f0ov[0]*f0f1[1]*f0f2[2])
                                 - (f0f1[2]*f0f2[1]*f0ov[0] + f0f2[0]*f0ov[2]*f0f1[1] + f0ov[1]*f0f1[0]*f0f2[2]);
                                 
                assert(fabs(det) > DBL_EPSILON);
                // If the normal points to this tet, split the facet,
                if (det > 0)
                    flag = 1;
            }

            if (flag) {
                addNN(vid, oe[i]);
                addNN(oe[i], vid);
            }
        }

        // Then split element
        // Form and add two new edges.
        const int ele0[] = {firstid, vid, oe[0], oe[1]};
        const int ele1[] = {secondid, vid, oe[0], oe[1]};

        const int ele0_boundary[] = {0, b[secondid], b[oe[0]], b[oe[1]]};
        const int ele1_boundary[] = {0, b[firstid], b[oe[0]], b[oe[1]]};

        index_t ele1ID = splitCnt;

        // Put ele1 in oe[0] and oe[1]'s NEList
        addNE(oe[0], ele1ID+origNElements);
        addNE(oe[1], ele1ID+origNElements);

        // Put eid and ele1 in newVertex[0]'s NEList
        addNE(vid, eid);
        addNE(vid, ele1ID+origNElements);

        // Replace eid with ele1 in splitEdges[0].edge.second's NEList
        remNE(secondid, eid);
        addNE(secondid, ele1ID+origNElements);

        replace_element(eid, ele0, ele0_boundary);
        append_element(ele1, ele1_boundary);
        splitCnt += 1;
    }

    inline void append_element(const index_t *elem, const int *boundary)
    {
        if(dim==3) {
            // Fix orientation of new element.
            const real_t *x0 = &(_mesh->_coords[elem[0]*dim]);
            const real_t *x1 = &(_mesh->_coords[elem[1]*dim]);
            const real_t *x2 = &(_mesh->_coords[elem[2]*dim]);
            const real_t *x3 = &(_mesh->_coords[elem[3]*dim]);

            real_t av = property->volume(x0, x1, x2, x3);

            if(av<0) {
                index_t *e = const_cast<index_t *>(elem);
                int *b = const_cast<int *>(boundary);

                // Flip element
                index_t e0 = e[0];
                e[0] = e[1];
                e[1] = e0;

                // and boundary
                int b0 = b[0];
                b[0] = b[1];
                b[1] = b0;
            }
        }

        for(size_t i=0; i<nloc; ++i) {
            newElements.push_back(elem[i]);
            newBoundaries.push_back(boundary[i]);
        }

        double q = _mesh->template calculate_quality<dim>(elem);
        newQualities.push_back(q);
    }

    inline void replace_element(const index_t eid, const index_t *n, const int *boundary)
    {
        if(dim==3) {
            // Fix orientation of new element.
            const real_t *x0 = &(_mesh->_coords[n[0]*dim]);
            const real_t *x1 = &(_mesh->_coords[n[1]*dim]);
            const real_t *x2 = &(_mesh->_coords[n[2]*dim]);
            const real_t *x3 = &(_mesh->_coords[n[3]*dim]);

            real_t av = property->volume(x0, x1, x2, x3);

            if(av<0) {
                index_t *e = const_cast<index_t *>(n);
                int *b = const_cast<int *>(boundary);

                // Flip element
                index_t e0 = e[0];
                e[0] = e[1];
                e[1] = e0;

                // and boundary
                int b0 = b[0];
                b[0] = b[1];
                b[1] = b0;
            }
        }

        for(size_t i=0; i<nloc; i++) {
            _mesh->_ENList[eid*nloc+i]=n[i];
            _mesh->boundary[eid*nloc+i]=boundary[i];
        }

        _mesh->template update_quality<dim>(eid);
    }

    inline void addNN(const index_t i, const index_t n)
    {
        _mesh->NNList[i].push_back(n);
    }

    inline void remNN(const index_t i, const index_t n)
    {
        
        typename std::vector<index_t>::iterator position;
        position = std::find(_mesh->NNList[i].begin(), _mesh->NNList[i].end(), n);
        assert(position != _mesh->NNList[i].end());
        _mesh->NNList[i].erase(position);
    }

    inline void addNE(const index_t i, const index_t n)
    {
        _mesh->NEList[i].insert(n);
    }

    inline void remNE(const index_t i, const index_t n)
    {
        assert(_mesh->NEList[i].count(n) != 0);
        _mesh->NEList[i].erase(n);
    }

    inline size_t edgeNumber(index_t eid, index_t v1, index_t v2) const
    {
        const int *n=_mesh->get_element(eid);

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
    }

    std::vector< DirectedEdge<index_t> > newVertices;
    std::vector<real_t>                  newCoords;
    std::vector<double>                  newMetric;
    std::vector<index_t>                 newElements;
    std::vector<int>                     newBoundaries;
    std::vector<double>                  newQualities;

    size_t origNElements;
    size_t splitCnt;

    DeferredOperations<real_t>* def_ops;
    static const int defOp_scaling_factor = 32;

    Mesh<real_t> *_mesh;
    ElementProperty<real_t> *property;

    const size_t nloc, msize, nedge;
    int nprocs, rank;

    //MY OWN IMPLEMENTATION
    //std::vector<int> edge_neighbor_partition;
    std::vector<std::set<int>> edge_neighbor_partitions;
    std::vector<size_t> threadIdx;
    //END OF MY OWN IMPLEMENTATION
};


#endif
