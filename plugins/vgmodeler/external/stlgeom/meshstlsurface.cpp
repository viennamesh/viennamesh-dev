#include <mystdlib.h>
#include <myadt.hpp>

#include <linalg.hpp>
#include <gprim.hpp>

#include <meshing.hpp>

// [JW] viennagrid has to be included prior to
// stlgeom, as the viennagrid domain has to be used within
// stltopology.hpp
#include "viennagridpp/mesh/mesh.hpp"
#include "viennagridpp/mesh/element_creation.hpp"

#include "stlgeom.hpp"


#include <boost/lexical_cast.hpp>
#include <map>

#include "gsse/datastructure/domain.hpp"
#include "gsse/algorithm.hpp"
#include "gsse/traversal.hpp"
#include "gsse/util.hpp"

#include "gsse/datastructure/domain_algorithms.hpp"
#include "gsse/topology/coboundary.hpp"
#include "gsse/topology/boundary.hpp"
#include "gsse/util/common.hpp"

#include "gsse_tools.hpp"
#include "gsse/geometry/metric_object.hpp"
#include "gsse_predicates.hpp"

extern "C" {
#define REAL double
   REAL orient2d(REAL *pa, REAL *pb, REAL *pc);
}

// #define DEBUGALL 1

namespace vgmnetgen
{

   template<typename ContainerT>
   void print_container(ContainerT& container)
   {
      typedef typename ContainerT::mapped_type value_type;

      typename ContainerT::iterator ccit;
      for(ccit = container.begin(); ccit != container.end(); ++ccit)
      {
         std::cout << "..id: " << (*ccit).first << std::endl;

         typename value_type::iterator cit;
         for(cit = (*ccit).second.begin(); cit != (*ccit).second.end(); ++cit)
         {
            std::cout << "\t..sub id: " << (*cit).first << " .. value: " << (*cit).second << std::endl;
         }
      }
   }


   template<typename ChartContT, typename EdgeContT, typename MappingT>
   void orient_chart(ChartContT& chart_container, EdgeContT& meshlines, int base_edge, int start_chart, MappingT& additional_edges)
   {

//       std::map<int,int>::iterator cit;
//       for(cit = (*mccit).second.begin(); cit != (*mccit).second.end(); ++cit)
      {
#ifdef DEBUGALL
         std::cout << "..in orient_chart checking chart: " << start_chart << std::endl;
#endif

         // the orientation of the base_edge has to be inverted
         //

         // the first material, the basis for all orientation, is correctly oriented
         // 0 = correct oriented, do not invert
         //
//          (*cit).second = 0;

#ifdef DEBUGALL
         std::cout << "..starting at base_edge: " << base_edge
                   << " p1: " << meshlines.Get(base_edge)->StartP() << " p2: " << meshlines.Get(base_edge)->EndP()
                   << std::endl;
#endif

         std::map<int,int> local_edge_used;

         int tries = 5;

         while(local_edge_used.size() != chart_container[start_chart].size() && tries > 0)
         {
            if(tries < 5)
            {
               std::map<int,int>::iterator eit;
               for(eit = chart_container[start_chart].begin() ; eit != chart_container[start_chart].end(); ++eit)
               {
//                std::cout << "..edge " << (*eit) << " mat: " << meshlines.Get((*eit).first)->material << ".. "
//                          << " .. p1: " << meshlines.Get((*eit).first)->StartP()
//                          << " .. p2: " << meshlines.Get((*eit).first)->EndP() << std::endl;

                  int mat = meshlines.Get((*eit).first)->material;

#ifdef DEBUGALL
                  std::cout << "..edge " << (*eit)
                            << " .. p1: " << meshlines.Get((*eit).first)->StartP()
                            << " .. p2: " << meshlines.Get((*eit).first)->EndP()
                            << " .. left: " << meshlines.Get((*eit).first)->segment_info[mat].lefttrig
                            << " .. right: " << meshlines.Get((*eit).first)->segment_info[mat].righttrig
//                             << " .. lface: " << geom.GetTriangle(meshlines.Get((*eit).first)->segment_info[0].lefttrig).GetFaceNum()
//                             << " .. rface: " << geom.GetTriangle(meshlines.Get((*eit).first)->segment_info[0].righttrig).GetFaceNum()
                            << " .. mat: " << meshlines.Get((*eit).first)->material
//                             << " .. ltrig mat: " << geom.GetTriangle(meshlines.Get((*eit).first)->segment_info[0].lefttrig).material[0]
//                             << " .. rtrig mat: " << geom.GetTriangle(meshlines.Get((*eit).first)->segment_info[0].righttrig).material[0]
                            << std::endl;
#endif


                  if(local_edge_used[(*eit).first] != 1)
                  {
                     base_edge = (*eit).first;
                     chart_container[start_chart][base_edge] = 0;
                     local_edge_used[(*eit).first] = 1;
                     break;
                  }
                  else
                  {
#ifdef DEBUGALL
                     std::cout << ".. in else .. NO MATCH .. " << std::endl;
#endif
                  }
               }
            }


            int start_edge = base_edge;
            int closed = 0;
            int count_edges = 1;

            int tries2 = 150;

            local_edge_used[start_edge] = 1;

            while(!closed && tries2 > 0)
            {
               // go through the edges of the chart and find a connected circle => output is a connected oriented circle
               //
               std::map<int,int>::iterator eit;
               for(eit = chart_container[start_chart].begin() ; eit != chart_container[start_chart].end(); ++eit)
               {
#ifdef DEBUGALL
                  std::cout << ".. start edge: " << start_edge << std::endl;
#endif

                  if(start_edge != (*eit).first)
                  {
                     STLLine* start_line = meshlines.Get(start_edge);
                     STLLine* test_line = meshlines.Get((*eit).first);

#ifdef DEBUGALL
                     std::cout << ".. test line: " << (*eit).first << " p1: " << test_line->StartP() << " p2: " << test_line->EndP() << std::endl;
#endif

                     if(chart_container[start_chart][start_edge])
                     {
#ifdef DEBUGALL
                        std::cout << "..using the inverted edge .. " << std::endl;
#endif

                        if( (start_line->StartP() == test_line->StartP()) )
                        {
#ifdef DEBUGALL
                           std::cout << ".. start_line->StartP == test_line->StartP .. " << std::endl;
#endif

                           if((*eit).first == base_edge)
                           {
#ifdef DEBUGALL
                              std::cout << ".. eit.first == base_edge .. " << std::endl;
#endif

                              closed = 1;
                              local_edge_used[(*eit).first] = 1;
                              break;
                           }
                           else
                           {
#ifdef DEBUGALL
                              std::cout << ".. eit.first != base_edge .. " << std::endl;
#endif

                              start_edge = (*eit).first;
//                        oriented_chart_container[(*cit).first][(*eit).first] = 0;
                              (*eit).second = 0;
                              count_edges++;
                              local_edge_used[(*eit).first] = 1;
                           }
                        }
                        else if( (start_line->StartP() == test_line->EndP()) )
                        {
#ifdef DEBUGALL
                           std::cout << "..coming to base_edge but other orientation ... problem ! " << std::endl;
#endif

                           if((*eit).first == base_edge)
                           {
                              closed = 1;
                              local_edge_used[(*eit).first] = 1;
                              break;
                           }
                           else
                           {
                              start_edge = (*eit).first;
//                        oriented_chart_container[(*cit).first][(*eit).first] = 1;
                              (*eit).second = 1;
                              count_edges++;
                              local_edge_used[(*eit).first] = 1;
                           }
                        }
                        else
                        {
                           std::cout << ".. in else .. NOT YET IMPLEMENTED .." << std::endl;

//                            start_edge = (*eit).first;
//                            (*eit).second = 1;
//                            count_edges++;
//                            local_edge_used[(*eit).first] = 1;
                        }
                     }
                     else
                     {
#ifdef DEBUGALL
                        std::cout << "..in else .. " << std::endl;
#endif

                        if( (start_line->EndP() == test_line->StartP()) )
                        {
#ifdef DEBUGALL
                           std::cout << ".. here " << std::endl;
#endif

                           if((*eit).first == base_edge)
                           {
                              closed = 1;
                              local_edge_used[(*eit).first] = 1;
                              break;
                           }
                           else
                           {
                              start_edge = (*eit).first;
//                        oriented_chart_container[(*cit).first][(*eit).first] = 0;
                              (*eit).second = 0;
                              count_edges++;
                              local_edge_used[(*eit).first] = 1;
                           }
                        }
                        else if( (start_line->EndP() == test_line->EndP()) )
                        {
#ifdef DEBUGALL
                           std::cout << "..coming to base_edge but other orientation ... problem ! " << std::endl;
#endif

                           if((*eit).first == base_edge)
                           {
                              closed = 1;
                              local_edge_used[(*eit).first] = 1;
                              break;
                           }
                           else
                           {
                              start_edge = (*eit).first;
//                        oriented_chart_container[(*cit).first][(*eit).first] = 1;
                              (*eit).second = 1;
                              count_edges++;
                              local_edge_used[(*eit).first] = 1;
                           }
                        }
#ifdef DEBUGALL
                        std::cout << "..end of else .. " << std::endl;
#endif
                     }
                  }
               }

#ifdef DEBUGALL
               std::cout << ".. tries2: " << tries2 << std::endl;
#endif

               tries2--;
            }

#ifdef DEBUGALL
            std::cout << "..processed edges " << local_edge_used.size() << " of " << chart_container[start_chart].size() << std::endl;
#endif

            if(local_edge_used.size() != chart_container[start_chart].size() && !closed)
            {
#ifdef DEBUGALL
               std::cout << "..ATTENTION: processed edges: " << local_edge_used.size() << " of " << chart_container[start_chart].size() << std::endl;
#endif

               std::map<int,int>::iterator leuit;
               for(leuit = local_edge_used.begin(); leuit != local_edge_used.end(); ++leuit)
               {
                  additional_edges[(*leuit).first] = (*leuit).second;
               }
            }

#ifdef DEBUGALL
            std::cout << ".. tries: " << tries << std::endl;
#endif
            tries--;
         }

         if(local_edge_used.size() != chart_container[start_chart].size())
         {
#ifdef DEBUGALL
            std::cout << "..ATTENTION: processed edges: " << local_edge_used.size() << " of " << chart_container[start_chart].size() << std::endl;
#endif

            std::map<int,int>::iterator eit;
            for(eit = chart_container[start_chart].begin() ; eit != chart_container[start_chart].end(); ++eit)
            {
               if(!local_edge_used[(*eit).first])
                  additional_edges[(*eit).first] = 1;
            }
         }
      }
   }


   template<typename MappingT, typename ContainerT, typename MeshlinesT>
   void orient_material(MappingT& material_chart_mapping, MappingT& chart_container,
                        MappingT& chart_material_mapping, MappingT& edge_chart_mapping, MappingT& chart_edge_mapping,
                        ContainerT& oriented_charts, MeshlinesT& meshlines,
                        int material_id, int base_chart, int base_edge, ContainerT& additional_edges)
   {
#ifdef DEBUGALL
      std::cout << ".. in orient material.." << std::endl;
      std::cout << ".. chart: " << base_chart << " .. in mat: " << material_id << std::endl;
#endif

      // orient the current material according to the given chart
      //
      orient_material_2(material_chart_mapping, chart_container, chart_material_mapping,
                        edge_chart_mapping, chart_edge_mapping, oriented_charts, meshlines,
                        material_id, base_chart, base_edge, additional_edges);


      // [INFO] now all neighbour charts are oriented => thus resulting in the orientation of the whole material


      // THIRD go through the neighbour charts and orient adjacent materials
      //
      std::map<int,int>::iterator cit;
      for(cit = material_chart_mapping[material_id].begin(); cit != material_chart_mapping[material_id].end(); ++cit)
      {
#ifdef DEBUGALL
         std::cout << ".. checking chart: " << (*cit).first << std::endl;
#endif

         std::map<int,int>::iterator mit;
         for(mit = chart_material_mapping[(*cit).first].begin();
             mit != chart_material_mapping[(*cit).first].end(); ++mit)
         {
#ifdef DEBUGALL
            std::cout << ".. in material: " << material_id << " checking material: " << (*mit).first << std::endl;
#endif

            long chart_count = 0;
            std::map<int,int>::iterator lcit;
            for(lcit = material_chart_mapping[(*mit).first].begin(); lcit != material_chart_mapping[(*mit).first].end(); ++lcit)
            {
               if(oriented_charts[(*lcit).first])
                  chart_count++;
            }

//          if(material_id != (*mit).first && used_materials[(*mit).first] != 1)

            // only go here if the material is adjacent AND has some not oriented charts
            //
            if(material_id != (*mit).first && chart_count < material_chart_mapping[(*mit).first].size())
            {
#ifdef DEBUGALL
               std::cout << ".. orienting the next material: " << (*mit).first << " ################" << std::endl;
#endif

               // set the orientation of the base_edge for the next material
               int chart_orientation = material_chart_mapping[material_id][(*cit).first];
               int new_orientation;

#ifdef DEBUGALL
               std::cout << ".. invert orienation.." << std::endl;
#endif

               // invert orientation
               if(chart_orientation == 1)
                  new_orientation = 0;
               else if(chart_orientation == 0)
                  new_orientation = 1;
               else
                  new_orientation = -1;

#ifdef DEBUGALL
               std::cout << "..new orientation: " << new_orientation << std::endl;
#endif

               material_chart_mapping[(*mit).first][(*cit).first] = new_orientation;

               int oriented_edge = (*chart_container[(*cit).first].begin()).first;

               // [INFO] determine if all the charts of the current materials have already been oriented
               //
//             std::map<int,int>::iterator lcit;
//             long chart_count = 0;
//             for(lcit = material_chart_mapping[material_id].begin(); lcit != material_chart_mapping[material_id].end(); ++lcit)
//             {
//                if(oriented_charts[(*lcit).first])
//                   chart_count++;
//             }

//             std::cout << "..chart_count: " << chart_count << std::endl;

//             // [INFO] only set the material to used if ALL the charts are oriented
//             //
//             if(chart_count == material_chart_mapping[material_id].size())
//                used_materials[material_id] = 1;
//             else
//                used_materials[material_id] = 0;

//             std::cout << "..after setting used materials of material " << material_id << " to: " << used_materials[material_id] << std::endl;

//             used_materials[(*mit).first] = 1;

//             std::cout << "..after setting used materials of material " << (*mit).first << " to: " << used_materials[(*mit).first] << std::endl;

               orient_material(material_chart_mapping, chart_container, chart_material_mapping,
                               edge_chart_mapping, chart_edge_mapping, oriented_charts, meshlines,
                               (*mit).first, (*cit).first, oriented_edge, additional_edges);
            }
         }
      }
   }

   template<typename MappingT, typename ContainerT, typename MeshlinesT>
   void orient_material_2(MappingT& material_chart_mapping, MappingT& chart_container,
                          MappingT& chart_material_mapping, MappingT& edge_chart_mapping, MappingT& chart_edge_mapping,
                          ContainerT& oriented_charts, MeshlinesT& meshlines,
                          int material_id, int base_chart, int base_edge, ContainerT& additional_edges)
   {
      std::cout << ".. in orient material 2.." << std::endl;

#ifdef DEBUGALL
      std::cout << ".. chart: " << base_chart << " .. in mat: " << material_id << std::endl;
#endif

      // FIRST orient the chart
      //
      if(oriented_charts[base_chart] != 1)
      {
#ifdef DEBUGALL
         std::cout << "..orienting chart: " << base_chart << std::endl;
#endif

         orient_chart(chart_container, meshlines, base_edge, base_chart, additional_edges);

         // set this, so that the chart is only oriented once
         //
         material_chart_mapping[material_id][base_chart] = 0;

         // in the adjacent material set the chart to the inverse orientation
         //
         std::map<int,int>::iterator mit;
         for(mit = chart_material_mapping[base_chart].begin(); mit != chart_material_mapping[base_chart].end(); ++mit)
         {
            if(material_id != (*mit).first)
            {
               material_chart_mapping[(*mit).first][base_chart] = 1;
            }
         }

         // set the chart to already oriented
         //
         oriented_charts[base_chart] = 1;

#ifdef DEBUGALL
         std::cout << "..chart_container after orientation .. " << std::endl;
         print_container(chart_container);
#endif
      }
      else
      {
#ifdef DEBUGALL
         std::cout << "..chart: " << base_chart << " already oriented .. " << std::endl;
#endif
      }


      // SECOND orient the adjacent charts
      //
      // the orientation of the base_chart can be determined in the material_chart_mapping
      // if it is 0 the orientation in the chart_container can be taken - otherwise it has to be inverted
      //
      // now go through all edges of this chart and orient the neighbour charts
      //
      std::map<int,int>::iterator eit;
      for(eit = chart_container[base_chart].begin(); eit != chart_container[base_chart].end(); ++eit)
      {
         int base_edge = (*eit).first;

         // these are the original orientations
         //
         int edge_orientation  = chart_container[base_chart][base_edge];
         int chart_orientation = material_chart_mapping[material_id][base_chart];

#ifdef DEBUGALL
         std::cout << "..edge: " << base_edge
                   << "..edge orientation:  " << edge_orientation
                   << "..chart orientation: " << chart_orientation
                   << std::endl;
#endif

         std::map<int,int>::iterator cit;
         for(cit = edge_chart_mapping[base_edge].begin(); cit != edge_chart_mapping[base_edge].end(); ++cit)
         {
#ifdef DEBUGALL
            std::cout << "..check chart " << (*cit).first << " on edge " << base_edge << std::endl;
#endif
            std::map<int,int>::iterator mit;
            for(mit = chart_material_mapping[(*cit).first].begin(); mit != chart_material_mapping[(*cit).first].end(); ++mit)
            {
               if((*cit).first != base_chart && material_id == (*mit).first && oriented_charts[(*cit).first] != 1)
               {
#ifdef DEBUGALL
                  std::cout << ".. orienting neighbour: " << (*cit).first << std::endl;
#endif
                  // set the orientation of the first edge for the neighbour chart
                  // invert the orientation of the edge independant on the orientation - it has to be inverted
                  //
                  int new_edge_orientation = edge_orientation;

                  // in the case the base_chart is not the inverse of the chart from the other material
                  //
                  if(!chart_orientation)
                  {
#ifdef DEBUGALL
                     std::cout << ".. invert orienation.." << std::endl;
#endif
                     // invert orientation
                     if(new_edge_orientation == 1)
                        new_edge_orientation = 0;
                     else
                        new_edge_orientation = 1;
                  }

                  // set the orientation of the base_edge for the next chart - has to be inverse to the current orientation
                  //
                  chart_container[(*cit).first][base_edge] = new_edge_orientation;

                  orient_material_2(material_chart_mapping, chart_container, chart_material_mapping,
                                  edge_chart_mapping, chart_edge_mapping, oriented_charts, meshlines, material_id, (*cit).first, (*eit).first, additional_edges);
               }
            }
         }
      }

#ifdef DEBUGALL
      std::cout << ".. before calling other materials .. " << std::endl;
#endif

   }




  struct TriangleIndexType
  {
    TriangleIndexType() {}
    TriangleIndexType(int i0_, int i1_, int i2_)
    {
      original_indices.resize(3);
      original_indices[0] = i0_;
      original_indices[1] = i1_;
      original_indices[2] = i2_;

      ordered_indices = original_indices;

      std::sort( ordered_indices.begin(), ordered_indices.end() );
    }

    bool operator<( TriangleIndexType const & other ) const
    {
      for (std::size_t i = 0; i < 3; ++i)
      if (ordered_indices[i] != other.ordered_indices[i])
        return ordered_indices[i] < other.ordered_indices[i];

      return false;
    }

    std::vector<int> ordered_indices;
    std::vector<int> original_indices;
  };


void STLFindEdges (STLGeometry & geom,
                   class Mesh & mesh,
                   std::map<int, std::map<int,int> >& material_chart_mapping)
{
   int i, j, k;
  double h;

#ifdef DEBUGALL
  std::cout << "[FS] .. in STLFindEdges .." << std::endl;
#endif

  h = mparam.maxh;

  // mark edge points:
  int ngp = geom.GetNP();

#ifdef DEBUGALL
  std::cout << ".. number of facedescriptors 0: " << mesh.GetNFD() << std::endl;
  std::cout << ".. facedescriptor test: mesh.GetNSeg 0: " << mesh.GetNSeg() << std::endl;
  std::cout << "..number of points: " << mesh.GetNP() << std::endl;
  std::cout << "..number of edges: " << mesh.GetNE() << std::endl;
  std::cout << ".. feature size: " << h << std::endl;
#endif

  // [TEST][FS][LFS] .. commented out to control the feature size
  //                    here the edges will be refined ..
  //
//   h = 0.5;
  geom.RestrictLocalH(mesh, h);

#ifdef DEBUGALL
  PushStatusF("Mesh Lines");
#endif

  ARRAY<STLLine*> meshlines;
  ARRAY<Point3d> meshpoints;

#ifdef DEBUGALL
  PrintMessage(3,"Mesh Lines");
  std::cout << ".. geom.GetNLines: " << geom.GetNLines() << std::endl;
#endif

  for (i = 1; i <= geom.GetNLines(); i++)
    {

#ifdef DEBUGALL
       std::cout << "..stlline: "
                 << geom.GetLine(i)->StartP() << " "
                 << geom.GetLine(i)->EndP() << " \t "
                 << geom.GetPoint(geom.GetLine(i)->StartP()) << " "
                 << geom.GetPoint(geom.GetLine(i)->EndP())
                 << std::endl;

       for(int m=0; m<geom.material_size; m++)
       {
          //
          if(geom.GetLine(i)->segment_info[m].lefttrig != -1)
          {
             std::cout << "..seg: " << m
                       << " .. facenum[0]: " << geom.GetLine(i)->segment_info[m].facenum[0]
                       << " .. facenum[1]: " << geom.GetLine(i)->segment_info[m].facenum[1]
                       << " .. lefttrig: " << geom.GetLine(i)->segment_info[m].lefttrig
                       << " .. righttrig: " << geom.GetLine(i)->segment_info[m].righttrig
                       << " .. lefttrig-face: " <<  geom.GetTriangle(geom.GetLine(i)->segment_info[m].lefttrig).GetFaceNum()
                       << " .. righttrig-face: " <<  geom.GetTriangle(geom.GetLine(i)->segment_info[m].righttrig).GetFaceNum()
		       << ".. lefttrig size " << geom.GetLine(i)->segment_info[m].lefttrig_size
		       << ".. righttrig size " << geom.GetLine(i)->segment_info[m].righttrig_size
                       << std::endl;
          }
       }
#endif

      meshlines.Append(geom.GetLine(i)->Mesh(geom.GetPoints(), meshpoints, h, mesh));

#ifdef DEBUGALL
      std::cout << ".. new meshlines size: " << meshlines.Size() << std::endl;
#endif

//       SetThreadPercent(100.0 * (double)i/(double)geom.GetNLines());
    }

  geom.meshpoints.SetSize(0); //testing
  geom.meshlines.SetSize(0);  //testing
  for (i = 1; i <= meshpoints.Size(); i++)
    {
      geom.meshpoints.Append(meshpoints.Get(i)); //testing

      int pim = mesh.AddPoint(meshpoints.Get(i));
    }
  //(++++++++++++++testing
  for (i = 1; i <= geom.GetNLines(); i++)
    {
      geom.meshlines.Append(meshlines.Get(i));
    }
  //++++++++++++++testing)

#ifdef DEBUGALL
  PrintMessage(7,"feed with edges");
  std::cout << ".. meshlines.Size(): " << meshlines.Size() << std::endl;
  std::cout << ".. number of facedescriptors 1: " << mesh.GetNFD() << std::endl;
  std::cout << "..number of points: " << mesh.GetNP() << std::endl;
  std::cout << "..number of meshpoints: " << meshpoints.Size() << std::endl;
#endif

  // [FS] Episode 4 .. A new hope!
  //
  typedef std::map<long, int> edge_container;
  typedef std::vector<edge_container> face_container;
  typedef std::map<long, face_container > outfaces_container;
  typedef std::map<long, edge_container > faces_container;
  typedef std::map<long, std::vector<long> > edges_container;

  edges_container edges;
  faces_container faces;
  faces_container faces_oriented;
  std::map<long, std::vector<double> > faces_normals;
  std::map<long, int > faces_orient;
  std::map<long, int > faces_material;

  long edge_count = 0;

  for(int f=0; f<50; f++)
     faces_material[f] = -1;

#ifdef DEBUGALL
  // [FS] .. line debug
  //
  std::ofstream file1("fs_line_debug.gau32");
  std::map<long,long> pointmap;

  file1 << "3" << std::endl;

  for(i = 1; i <= meshlines.Size(); i++)
  {
     STLLine* line = meshlines.Get(i);

     std::cout << "..meshline: " << i << " .. p1: " << line->StartP() << " ..p2: " << line->EndP() << std::endl;

     std::map<long,long>::iterator pmit;
     pmit = pointmap.find(line->StartP());
     if(pmit == pointmap.end())
     {
        file1 << mesh.Point(line->StartP())(0) << " "
              << mesh.Point(line->StartP())(1) << " "
              << mesh.Point(line->StartP())(2) << " "
              << std::endl;
        pointmap[line->StartP()] = pointmap.size();
     }

     pmit = pointmap.find(line->EndP());
     if(pmit == pointmap.end())
     {
        file1 << mesh.Point(line->EndP())(0) << " "
              << mesh.Point(line->EndP())(1) << " "
              << mesh.Point(line->EndP())(2) << " "
              << std::endl;
        pointmap[line->EndP()] = pointmap.size();
     }
  }

  file1 << "1" << std::endl;
  file1 << "segment_1" << std::endl;
  file1 << meshlines.Size() << std::endl;
  file1 << pointmap.size() << std::endl;

  for(i = 1; i <= meshlines.Size(); i++)
  {
     STLLine* line = meshlines.Get(i);

//      file1 << i-1 << " " << line->StartP()-1 << " " << line->EndP()-1 << std::endl;
     file1 << i-1 << " " << pointmap[line->StartP()]-1 << " "
           << pointmap[line->EndP()]-1 << " "
           << pointmap[line->StartP()]-1
           << std::endl;

  }
  file1.close();
#endif

  //
  // [DEBUGALL] end ...

  // [FS] .. debug faces
  //
  std::map<int, int> point_map;
//   for(int mpi=1; mpi <= mesh.GetNP(); ++mpi)
//   {
//      std::cout << ".. meshpoint: " << mpi << "  " << mesh.Point(mpi) << std::endl;
//      for(int gpi=1; gpi <= geom.GetNP(); ++gpi)
//      {
//         std::cout << ".. geompoint: " << gpi << "  " << geom.GetPoint(gpi) << std::endl;
//         if(mesh.Point(mpi)(0) == geom.GetPoint(gpi)(0) &&
//            mesh.Point(mpi)(1) == geom.GetPoint(gpi)(1) &&
//            mesh.Point(mpi)(2) == geom.GetPoint(gpi)(2) )
//         {
//            point_map[mpi] = gpi;
//            std::cout << ".. mapping found: " << mpi << " to " << point_map[mpi] << std::endl;
//            break;
//         }
//      }
//   }


  // #############################################################################################
  // #############################################################################################
  //
  // NEW APPROACH - this time mathematically correct ... Episode 6 - The Return of the Orientation
  //
  // #############################################################################################
  // #############################################################################################

  std::map<int, int> used_faces;
  std::map<int, int> used_edges;
  std::map<int, int> oriented_charts;

  std::map<int,int> additional_edges;

  // material id ; char id + orientation (0 / 1) (original / inverted)
  //
//   std::map<int, std::map<int,int> > material_chart_mapping;
  std::map<int, std::map<int,int> > chart_material_mapping;
  std::map<int, std::map<int,int> > chart_container;
  std::map<int, std::map<int,int> > chart_edge_mapping;
  std::map<int, std::map<int,int> > edge_chart_mapping;
//   std::map<int, std::map<int,int> > oriented_chart_container;

  std::map<int, std::map<int,int> >::iterator mccit;

#ifdef DEBUGALL
  std::cout << "######################## MESHLINE INPUT #############################" << std::endl;

  for(i = 1; i <= meshlines.Size(); i++)
  {
     STLLine* line = meshlines.Get(i);

     std::cout << "..meshline: " << i << " .. p1: " << line->StartP() << " ..p2: " << line->EndP()
               << std::endl;

     for(int m=0; m < geom.material_size; m++)
     {
        std::cout << "..mat: " << m
                  << "..lefttrig: " << line->segment_info[m].lefttrig
                  << "..righttrig: " << line->segment_info[m].righttrig
                  << std::endl;
     }
  }
#endif


  for(int f=1; f <= geom.GetNOFaces(); ++f)
  {
     used_faces[f] = 0;
  }

#ifdef DEBUGALL
  std::cout << "######################## create CHART CONTAINER #############################" << std::endl;
#endif

  // [INFO] build the chart container
  //
  for(int m=0; m < geom.material_size; m++)
  {
#ifdef DEBUGALL
     std::cout << "..mat: " << m << std::endl;
#endif

     for(int f=1; f <= geom.GetNOFaces(); ++f)
     {
#ifdef DEBUGALL
        std::cout << "..face: " << f << std::endl;
#endif

        if(!used_faces[f])
        {
           int edge_count = 0;

           for(int i=1; i <= meshlines.Size(); i++)
           {
              STLLine* line = meshlines.Get(i);

              // [FS][TEST] tried to fix the structural edge artefarcts
              //
//               if( (line->segment_info[m].facenum[0] == f || line->segment_info[m].facenum[1] == f) &&
//                   (line->segment_info[m].facenum[0] != line->segment_info[m].facenum[1]) )

              if( (line->segment_info[m].facenum[0] == f || line->segment_info[m].facenum[1] == f) )
              {
                 chart_container[f][i] = 0;

#ifdef DEBUGALL
                 std::cout << "..adding edge: " << i << " ..p1: " << line->StartP() << " p2: " << line->EndP() << std::endl;
#endif

                 edge_count++;


                 if(line->segment_info[m].facenum[0] == line->segment_info[m].facenum[1])
                 {
                    additional_edges[i] = 1;
                 }
              }
           }

           if(edge_count > 0)
           {
              used_faces[f];
           }
        }
     }
  }

#ifdef DEBUGALL
  std::cout << "..chart_container: " << std::endl;
  print_container(chart_container);
#endif

#ifdef DEBUGALL
  std::cout << "######################## MATERIAL - CHART mapping #############################" << std::endl;
#endif

  // [INFO] create material 2 chart mapping
  //
  for(int f=1; f <= geom.GetNOFaces(); ++f)
  {
#ifdef DEBUGALL
     std::cout << "..face: " << f << std::endl;
#endif

     for(int m=0; m < geom.material_size; m++)
     {
#ifdef DEBUGALL
        std::cout << "..mat: " << m << std::endl;
#endif

        for(int i=1; i <= meshlines.Size(); i++)
        {
           STLLine* line = meshlines.Get(i);
           if(line->segment_info[m].facenum[0] == f || line->segment_info[m].facenum[1] == f)
           {
              if(chart_material_mapping[f].size() > 1)
              {
                 material_chart_mapping[m][f] = 1;
              }
              else
              {
                 material_chart_mapping[m][f] = 0;
              }

              chart_material_mapping[f][m] = 1;

#ifdef DEBUGALL
              std::cout << "..adding chart: " << f << " to material " << m << std::endl;
#endif
           }
        }
     }
  }

  // [DEBUGALL]
  //
#ifdef DEBUGALL
  std::cout << "..print material_chart_mapping.." << std::endl;
  print_container(material_chart_mapping);

  std::cout << "..print chart_material_mapping.." << std::endl;
  print_container(chart_material_mapping);
#endif

#ifdef DEBUGALL
  std::cout << "######################## CHART - EDGE mapping #############################" << std::endl;
#endif

  // [INFO] create chart 2 edge mapping
  //
  for(int f=1; f <= geom.GetNOFaces(); ++f)
  {
#ifdef DEBUGALL
     std::cout << "..face: " << f << std::endl;
#endif

     for(int m=0; m < geom.material_size; m++)
     {
#ifdef DEBUGALL
        std::cout << "..mat: " << m << std::endl;
#endif

        for(int i=1; i <= meshlines.Size(); i++)
        {
           STLLine* line = meshlines.Get(i);
           if(line->segment_info[m].facenum[0] == f || line->segment_info[m].facenum[1] == f)
           {
              if(edge_chart_mapping[i].size() > 1)
              {
                 chart_edge_mapping[f][i] = 1;
              }
              else
              {
                 chart_edge_mapping[f][i] = 0;
              }

              edge_chart_mapping[i][f] = 0;

#ifdef DEBUGALL
              std::cout << "..adding edge: " << i << " to chart " << f << std::endl;
#endif
           }
        }
     }
  }

  // [DEBUGALL]
  //
#ifdef DEBUGALL
  std::cout << "..print chart_edge_mapping.." << std::endl;
  print_container(chart_edge_mapping);

  std::cout << "..print edge_chart_mapping.." << std::endl;
  print_container(edge_chart_mapping);
#endif

#ifdef DEBUGALL
  std::cout << "######################## check CHART orientation #############################" << std::endl;
#endif

  // [INFO] check the orientation and correct it if necessary
  //
  // the first material is the basis for orienting the remaining material
  //
  mccit = material_chart_mapping.begin();

#ifdef DEBUGALL
  std::cout << "..material: " << (*mccit).first << std::endl;
#endif

  std::map<int,int>::iterator cit;
  for(cit = (*mccit).second.begin(); cit != (*mccit).second.end(); ++cit)
  {
#ifdef DEBUGALL
     std::cout << "..checking chart: " << (*cit).first << std::endl;
#endif

     // the first material, the basis for all orientation, is correctly oriented
     // 0 = correct oriented, do not invert
     //
     (*cit).second = 0;
     oriented_charts[(*cit).first] = 1; // to indicate, that the chart is already used and does not need to be meshed again

     int base_edge = 0;
     std::map<int,int> local_edge_used;

     int tries = 5;

     while(local_edge_used.size() != chart_container[(*cit).first].size() && tries > 0)
     {

        // find first edge, which is in the material and use it as starting edge
        //
        std::map<int,int>::iterator eit;
        for(eit = chart_container[(*cit).first].begin() ; eit != chart_container[(*cit).first].end(); ++eit)
        {
#ifdef DEBUGALL
           std::cout << "..edge " << (*eit)
                     << " .. p1: " << meshlines.Get((*eit).first)->StartP()
                     << " .. p2: " << meshlines.Get((*eit).first)->EndP()
                     << " .. left: " << meshlines.Get((*eit).first)->segment_info[0].lefttrig
                     << " .. right: " << meshlines.Get((*eit).first)->segment_info[0].righttrig
                     << " .. lface: " << geom.GetTriangle(meshlines.Get((*eit).first)->segment_info[0].lefttrig).GetFaceNum()
                     << " .. rface: " << geom.GetTriangle(meshlines.Get((*eit).first)->segment_info[0].righttrig).GetFaceNum()
                     << " .. mat: " << meshlines.Get((*eit).first)->material
                     << " .. ltrig mat: " << geom.GetTriangle(meshlines.Get((*eit).first)->segment_info[0].lefttrig).material[0]
                     << " .. rtrig mat: " << geom.GetTriangle(meshlines.Get((*eit).first)->segment_info[0].righttrig).material[0]
                     << std::endl;
#endif

           if(meshlines.Get((*eit).first)->material == (*mccit).first &&
              meshlines.Get((*eit).first)->segment_info[0].facenum[0] == (*cit).first &&
              local_edge_used[(*eit).first] != 1)
           {
              base_edge = (*eit).first;
              chart_container[(*cit).first][base_edge] = 0;
              local_edge_used[(*eit).first] = 1;
              break;
           }
           else if(meshlines.Get((*eit).first)->material == (*mccit).first &&
                   meshlines.Get((*eit).first)->segment_info[0].facenum[1] == (*cit).first &&
                   local_edge_used[(*eit).first] != 1)
           {
              base_edge = (*eit).first;
              chart_container[(*cit).first][base_edge] = 1;
              local_edge_used[(*eit).first] = 1;
              break;
           }
           else
           {
#ifdef DEBUGALL
              std::cout << ".. in else .. NO MATCH .. " << std::endl;
#endif
           }
        }

#ifdef DEBUGALL
        std::cout << "..starting at base_edge: " << base_edge << std::endl;
#endif

        int start_edge = base_edge;
        int closed = 0;
        int count_edges = 0;

        int tries2 = 150;

        while(!closed && (tries2 > 0))
        {
           // go through the edges of the chart and find a connected circle => output is a connected oriented circle
           //
           for(eit = chart_container[(*cit).first].begin() ; eit != chart_container[(*cit).first].end(); ++eit)
           {
#ifdef DEBUGALL
              std::cout << ".. start edge: " << start_edge << std::endl;
#endif

              if(start_edge != (*eit).first)
              {
                 STLLine* start_line = meshlines.Get(start_edge);
                 STLLine* test_line = meshlines.Get((*eit).first);

#ifdef DEBUGALL
                 std::cout << ".. test line: " << (*eit).first << " p1: " << test_line->StartP() << " p2: " << test_line->EndP() << std::endl;
#endif

                 if(chart_container[(*cit).first][start_edge])
                 {
#ifdef DEBUGALL
                    std::cout << "..using the inverted edge .. " << std::endl;
#endif

                    if( (start_line->StartP() == test_line->StartP()) )
                    {
                       if((*eit).first == base_edge)
                       {
                          closed = 1;
                          local_edge_used[(*eit).first] = 1;
                          break;
                       }
                       else
                       {
                          start_edge = (*eit).first;
//                        oriented_chart_container[(*cit).first][(*eit).first] = 0;
                          (*eit).second = 0;
                          local_edge_used[(*eit).first] = 1;
                       }
                    }
                    else if( (start_line->StartP() == test_line->EndP()) )
                    {
#ifdef DEBUGALL
                       std::cout << "..coming to base_edge but other orientation ... problem ! " << std::endl;
#endif

                       if((*eit).first == base_edge)
                       {
                          closed = 1;
                          local_edge_used[(*eit).first] = 1;
                          break;
                       }
                       else
                       {
                          start_edge = (*eit).first;
//                        oriented_chart_container[(*cit).first][(*eit).first] = 1;
                          (*eit).second = 1;
                          local_edge_used[(*eit).first] = 1;
                       }
                    }
                    else
                    {
                       // [FS][TODO] handle this case
                       //
#ifdef DEBUGALL
                       std::cout << ".. in else .. NO MATCH - NOT DEFINED .." << std::endl;
#endif

                    }
                 }
                 else
                 {
#ifdef DEBUGALL
                    std::cout << "..in else .. " << std::endl;
#endif

                    if( (start_line->EndP() == test_line->StartP()) )
                    {
                       if((*eit).first == base_edge)
                       {
                          closed = 1;
                          local_edge_used[(*eit).first] = 1;
                          break;
                       }
                       else
                       {
                          start_edge = (*eit).first;
//                        oriented_chart_container[(*cit).first][(*eit).first] = 0;
                          (*eit).second = 0;
                          local_edge_used[(*eit).first] = 1;
                       }
                    }
                    else if( (start_line->EndP() == test_line->EndP()) )
                    {
#ifdef DEBUGALL
                       std::cout << "..coming to base_edge but other orientation ... problem ! " << std::endl;
#endif

                       if((*eit).first == base_edge)
                       {
                          closed = 1;
                          local_edge_used[(*eit).first] = 1;
                          break;
                       }
                       else
                       {
                          start_edge = (*eit).first;
//                        oriented_chart_container[(*cit).first][(*eit).first] = 1;
                          (*eit).second = 1;
                          local_edge_used[(*eit).first] = 1;
                       }
                    }
                    else
                    {
                       // [FS][TODO] handle this case
                       //
#ifdef DEBUGALL
                       std::cout << ".. in else .. NO MATCH - NOT DEFINED .." << std::endl;
#endif
                    }
                 }
              }
           }
#ifdef DEBUGALL
           std::cout << "..tries2: " << tries2 << std::endl;
#endif
           tries2--;

#ifdef DEBUGALL
           std::cout << ".. end of while .. closed: " << closed << std::endl;
#endif
        }

#ifdef DEBUGALL
        std::cout << "..processed edges " << local_edge_used.size() << " of " << chart_container[(*cit).first].size() << std::endl;
#endif

//         if(local_edge_used.size() != chart_container[(*cit).first].size() && !closed)
//         {
//            std::map<int,int>::iterator leuit;
//            for(leuit = local_edge_used.begin(); leuit != local_edge_used.end(); ++leuit)
//            {
//               additional_edges[(*leuit).first] = (*leuit).second;
//            }
//         }

        tries--;
     }

//      if(local_edge_used.size() != chart_container[(*cit).first].size())
//      {
//         std::cout << "..ATTENTION: processed edges: " << local_edge_used.size() << " of " << chart_container[(*cit).first].size() << std::endl;

//         std::map<int,int>::iterator eit;
//         for(eit = chart_container[(*cit).first].begin() ; eit != chart_container[(*cit).first].end(); ++eit)
//         {
//            if(local_edge_used[(*eit).first] == 1)
//               additional_edges[(*eit).first] = 0;
//            else
//               additional_edges[(*eit).first] = 1;
//         }
//      }
  }

  // [DEBUGALL]
#ifdef DEBUGALL
  std::cout << "..chart_container after orientation .. " << std::endl;
  print_container(chart_container);
  print_container(material_chart_mapping);
#endif

  // now the first material is oriented => derive the orientation for the remaining materials from this material
  // use the shared charts, but invert the orientation
  //
  std::map<int,int> used_materials;
  used_materials[0] = 1;


#ifdef DEBUGALL
  std::cout << ".. STARTING CHART ORIENTATION .. " << std::endl;
#endif



//   std::map<int, std::map<int,int> >::iterator cmit;
//   for(cmit = chart_material_mapping.begin(); cmit != chart_material_mapping.end(); ++cmit)
//   {
//      std::cout << "..chart id: " << (*cmit).first << std::endl;

//      std::map<int,int>::iterator cit1;
//      for(cit1 = (*cmit).second.begin(); cit1 != (*cmit).second.end(); ++cit1)
//      {
//         std::cout << "\t..sub id: " << (*cit1).first << " .. value: " << (*cit1).second << std::endl;

//         if(!used_materials[(*cit1).first])
//         {
//            mccit = material_chart_mapping.find((*cit1).first);

  std::map<int,int> oriented_charts2;

//   mccit = material_chart_mapping.begin();
// //   ++mccit;
// //  for( ; mccit != material_chart_mapping.end(); ++mccit)
//   {
//      std::cout << "..material: " << (*mccit).first << std::endl;

// //      if(!used_materials[(*mccit).first])
//      {
//         int base_chart = -1;
//         int base_edge  = -1;
//         int orientation = -1;
//         std::map<int, int> local_oriented_charts;

//         std::map<int,int>::iterator cit;
//         cit = (*mccit).second.begin();

//         // set a default edge - if no oriented edge can be found then the first chart of the material is used
//         base_chart = (*cit).first;
//         base_edge  = (*chart_edge_mapping[(*cit).first].begin()).first;
//         orientation = (*cit).second;

//         for( ; cit != (*mccit).second.end(); ++cit)
//         {
//            std::cout << "..checking chart: " << (*cit).first << "..orientation: " << (*cit).second << std::endl;

//            if(oriented_charts[(*cit).first])
//            {
//               base_chart = (*cit).first;
//               base_edge  = (*chart_edge_mapping[(*cit).first].begin()).first;
//               orientation = (*cit).second;

//               std::cout << ".. setting base_chart: " << base_chart << std::endl;

//               break;
//            }
//         }

//         oriented_charts2[base_chart] = 1;

//         used_materials[(*mccit).first] = 1;

//         orient_material(material_chart_mapping, chart_container, chart_material_mapping, edge_chart_mapping, chart_edge_mapping,
//                         oriented_charts2, meshlines, (*mccit).first, base_chart, base_edge, additional_edges);

//         std::cout << "..chart_container after orientation: material .. " << std::endl;
//         print_container(chart_container);
//      }
//   }

#ifdef DEBUGALL
  std::cout << ".. FINISHING CHART ORIENTATION .. " << std::endl;
#endif



  // [DEBUGALL]
#ifdef DEBUGALL
  std::cout << "..chart_container after orientation 2 .. " << std::endl;
  print_container(chart_container);

  std::cout << "..material_chart_mapping after orientation 2 .. " << std::endl;
  print_container(material_chart_mapping);

  std::cout << "..size additional edges: " << additional_edges.size() << std::endl;
#endif

  Mesh mesh2;
  mesh2 = mesh;

  for(int f=1; f <= geom.GetNOFaces(); ++f)
  {
     used_faces[f] = 0;
  }

  for(mccit = material_chart_mapping.begin(); mccit != material_chart_mapping.end(); ++mccit)
  {
#ifdef DEBUGALL
     std::cout << "..material_chart_cont: " << (*mccit).first << std::endl;
#endif

     std::map<int,int>::iterator cit;
     for(cit = (*mccit).second.begin(); cit != (*mccit).second.end(); ++cit)
     {
#ifdef DEBUGALL
        std::cout << "\t..chart: " << (*cit).first << std::endl;
#endif

        if(!used_faces[(*cit).first])
        {
           used_faces[(*cit).first] = 1;

           std::map<int,int>::iterator eit;
           for(eit = chart_container[(*cit).first].begin(); eit != chart_container[(*cit).first].end(); ++eit)
           {
#ifdef DEBUGALL
              std::cout << "\t\t..edge: " << (*eit).first << std::endl;
#endif

              {
              STLLine* line = meshlines.Get((*eit).first);

              for (int ii = 1; ii <= line->GetNS(); ii++)
              {
#ifdef DEBUGALL
		std::cout << "..ii: " << ii << std::endl;
#endif

                 int p1, p2;
                 line->GetSeg(ii,p1,p2);

#ifdef DEBUGALL
                 std::cout << ".. p1: " << p1 << " .. p2: " << p2
                           << ".. dist: " << line->GetDist(ii)
                           << std::endl;
#endif

                 vgmnetgen::Segment seg;

                 seg.si = (*cit).first;
                 seg.edgenr = (*eit).first;

                 int invert = 0;

		 if(line->segment_info[(*mccit).first].facenum[0] == (*cit).first)
		   {
#ifdef DEBUGALL
		     std::cout << "..using original left/right trigs.." << std::endl;
#endif
// 		     seg.geominfo[0].trignum = line->segment_info[(*mccit).first].lefttrig;
// 		     seg.geominfo[1].trignum = line->segment_info[(*mccit).first].righttrig;
// 		     seg.geominfo[0].trignum = line->GetLeftTrig(ii);
// 		     seg.geominfo[1].trignum = line->GetLeftTrig(ii+1);

#ifdef DEBUGALL
                     std::cout << ".. line left trigs: " << line->GetLeftTrig(ii) << " " << line->GetLeftTrig(ii+1) << std::endl;
#endif

		     seg.geominfo[0].trignum = line->segment_info[(*mccit).first].lefttriangles[ii];
		     seg.geominfo[1].trignum = line->segment_info[(*mccit).first].lefttriangles[ii+1];

                     seg.p1 = p1;
                     seg.p2 = p2;
		   }
                 else if(line->segment_info[(*mccit).first].facenum[1] == (*cit).first)
		   {
#ifdef DEBUGALL
		     std::cout << "..using inverted left/right trigs.." << std::endl;
#endif
// 		     seg.geominfo[0].trignum = line->segment_info[(*mccit).first].righttrig;
// 		     seg.geominfo[1].trignum = line->segment_info[(*mccit).first].lefttrig;
// 		     seg.geominfo[0].trignum = line->GetRightTrig(ii);
// 		     seg.geominfo[1].trignum = line->GetRightTrig(ii+1);

		     seg.geominfo[0].trignum = line->segment_info[(*mccit).first].righttriangles[ii+1];
		     seg.geominfo[1].trignum = line->segment_info[(*mccit).first].righttriangles[ii];

                     seg.p1 = p2;
                     seg.p2 = p1;

                     invert = 1;
		   }
                 else
		   {
                     std::cout << "..left/right trigs NOT MATCHED.." << std::endl;

// 		     seg.geominfo[0].trignum = line->segment_info[(*mccit).first].lefttrig;
// 		     seg.geominfo[1].trignum = line->segment_info[(*mccit).first].righttrig;
// 		     seg.geominfo[0].trignum = line->GetLeftTrig(ii);
// 		     seg.geominfo[1].trignum = line->GetLeftTrig(ii+1);

		     seg.geominfo[0].trignum = line->segment_info[(*mccit).first].lefttriangles[ii];
		     seg.geominfo[1].trignum = line->segment_info[(*mccit).first].lefttriangles[ii+1];

                     seg.p1 = p1;
                     seg.p2 = p2;
 		   }

#ifdef DEBUGALL
		 std::cout << "..lefttrigs size: " << line->segment_info[(*mccit).first].lefttrig_size
			   << "..righttrigs size: " << line->segment_info[(*mccit).first].righttrig_size
			   << std::endl;

                 std::cout << ".. current material: " << (*mccit).first
                           << ".. trig material: " << geom.GetTriangle(line->segment_info[(*mccit).first].lefttriangles[ii]).material[0]
                           << std::endl;
#endif


                 // [INFO] insert the points according to the base orientation of the edge
                 //
//                  if((*eit).second)
//                  {
//                     std::cout << ".. invert orientation .. " << std::endl;

//                     seg.p1 = p2;
//                     seg.p2 = p1;
//                  }
//                  else
//                  {
//                     std::cout << ".. keep orientation .. " << std::endl;

//                     seg.p1 = p1;
//                     seg.p2 = p2;
//                   }


                 // [INFO] invert the orientation of the chart according to the material
                 //
                 if(material_chart_mapping[(*mccit).first][(*cit).first])
                 {
                    std::cout << ".. inverting edge because of chart - material orientation" << std::endl;
                    std::swap(seg.p1, seg.p2);
                 }

#ifdef DEBUGALL
                 std::cout << "..new approach: p1: " << seg.p1 << " p2: " << seg.p2
                           << ".. lefttrig: " << seg.geominfo[0].trignum
                           << ".. righttrig: " << seg.geominfo[1].trignum
                           << std::endl;
#endif


                 mesh2.AddSegment(seg);


                 // [FS] [TEST] CHECK BEFORE KEEPING IT
                 if(additional_edges[(*eit).first])
                 {
#ifdef DEBUGALL
                    std::cout << ".. adding additional edges .. " << (*eit).first << std::endl;
#endif

                    std::swap(seg.p1, seg.p2);

                    if(invert)
                    {
                       seg.geominfo[0].trignum = line->segment_info[(*mccit).first].lefttriangles[ii];
                       seg.geominfo[1].trignum = line->segment_info[(*mccit).first].lefttriangles[ii+1];
                    }
                    else
                    {
                       seg.geominfo[0].trignum = line->segment_info[(*mccit).first].righttriangles[ii+1];
                       seg.geominfo[1].trignum = line->segment_info[(*mccit).first].righttriangles[ii];
                    }

#ifdef DEBUGALL
                    std::cout << ".. p1: " << seg.p1 << " p2: " << seg.p2
                              << ".. lefttrig: " << seg.geominfo[0].trignum
                              << ".. righttrig: " << seg.geominfo[1].trignum
                              << std::endl;
#endif

                    mesh2.AddSegment(seg);
                 }


//                  if( (seg.si == 67) )
//                  {
// #ifdef DEBUGALL
//                     std::cout << "..correcting NON-MANIFOLD edge .. " << std::endl;
// #endif

//                     seg.p1 = 473;
//                     seg.p2 = 59;

//                     seg.geominfo[0].trignum = 16472;
//                     seg.geominfo[1].trignum = 16472;

// #ifdef DEBUGALL
//                     std::cout << ".. p1: " << seg.p1 << " p2: " << seg.p2
//                               << ".. lefttrig: " << seg.geominfo[0].trignum
//                                << ".. righttrig: " << seg.geominfo[1].trignum
//                               << std::endl;
// #endif

//                     mesh2.AddSegment(seg);
//                  }

//                  if( (seg.si == 68) )
//                  {
// #ifdef DEBUGALL
//                     std::cout << "..correcting NON-MANIFOLD edge .. " << std::endl;
// #endif

//                     seg.p1 = 59;
//                     seg.p2 = 473;

//                     seg.geominfo[0].trignum = 16471;
//                     seg.geominfo[1].trignum = 16471;

// #ifdef DEBUGALL
//                     std::cout << ".. p1: " << seg.p1 << " p2: " << seg.p2
//                               << ".. lefttrig: " << seg.geominfo[0].trignum
//                                << ".. righttrig: " << seg.geominfo[1].trignum
//                               << std::endl;
// #endif

//                     mesh2.AddSegment(seg);
//                  }


                 // [TEST]
//                  if((*cit).first >= 7)
//                  {
//                     std::cout << ".. inverting edge because of test - chart 7" << std::endl;
//                     std::swap(seg.p1, seg.p2);
//                  }


                 // [INFO] use the triangle number of the line segment calcualted by vgmnetgen only of it is on the same chart
                 //
//                  if(geom.GetTriangle(seg.geominfo[0].trignum).GetFaceNum() == geom.GetTriangle(line->GetLeftTrig(ii)).GetFaceNum())
//                  {
//                     seg.geominfo[0].trignum = line->GetLeftTrig(ii);
// //                     seg.geominfo[1].trignum = line->GetLeftTrig(ii+1);
//                  }
//                  if(geom.GetTriangle(seg.geominfo[1].trignum).GetFaceNum() == geom.GetTriangle(line->GetRightTrig(ii)).GetFaceNum())
//                  {
// //                     seg.geominfo[0].trignum = line->GetRightTrig(ii);
//                     seg.geominfo[1].trignum = line->GetRightTrig(ii);
//                  }



// 		 if(seg.si == 67 && seg.p1 == 2)
// 		   {
// 		     seg.geominfo[0].trignum = 12181;
// 		     std::cout << "..setting p1 to " << seg.geominfo[0].trignum << std::endl;
// 		   }
// 		 if(seg.si == 67 && seg.p2 == 1)
// 		   {
// 		     seg.geominfo[1].trignum = 12181;
// 		     std::cout << "..setting p2 to " << seg.geominfo[1].trignum << std::endl;
// 		   }

// 		 if(seg.si == 67 && seg.p1 == 3)
// 		   {
// 		     seg.geominfo[0].trignum = 12181;
// 		     std::cout << "..setting p1 to " << seg.geominfo[0].trignum << std::endl;
// 		   }
// 		 if(seg.si == 67 && seg.p2 == 2)
// 		   {
// 		     seg.geominfo[1].trignum = 12181;
// 		     std::cout << "..setting p2 to " << seg.geominfo[1].trignum << std::endl;
// 		   }

// 		 if(seg.si == 67 && seg.p1 == 4)
// 		   {
// 		     seg.geominfo[0].trignum = 12181;
// 		     std::cout << "..setting p1 to " << seg.geominfo[0].trignum << std::endl;
// 		   }
// 		 if(seg.si == 67 && seg.p2 == 3)
// 		   {
// 		     seg.geominfo[1].trignum = 12181;
// 		     std::cout << "..setting p1 to " << seg.geominfo[1].trignum << std::endl;
// 		   }

// 		 if(seg.si == 2 && seg.p1 == 47)
// 		   {
// 		     seg.geominfo[0].trignum = 6468;
// 		     std::cout << "..setting p1 to " << seg.geominfo[0].trignum << std::endl;
// 		   }
// 		 if(seg.si == 2 && seg.p2 == 47)
// 		   {
// 		     seg.geominfo[1].trignum = 6468;
// 		     std::cout << "..setting p2 to " << seg.geominfo[1].trignum << std::endl;
// 		   }

// 		 if(seg.si == 2 && seg.p1 == 48)
// 		   {
// 		     seg.geominfo[0].trignum = 4460;
// 		     std::cout << "..setting p1 to " << seg.geominfo[0].trignum << std::endl;
// 		   }
// 		 if(seg.si == 2 && seg.p2 == 48)
// 		   {
// 		     seg.geominfo[1].trignum = 4460;
// 		     std::cout << "..setting p2 to " << seg.geominfo[1].trignum << std::endl;
// 		   }

// 		 if(seg.si == 2 && seg.p1 == 49)
// 		   {
// 		     seg.geominfo[0].trignum = 1833;
// 		     std::cout << "..setting p1 to 1833" << std::endl;
// 		   }
// 		 if(seg.si == 2 && seg.p2 == 49)
// 		   {
// 		     seg.geominfo[1].trignum = 1833;
// 		     std::cout << "..setting p2 to 1833" << std::endl;
// 		   }


//                  seg.epgeominfo[0].edgenr = (*eit).first;
//                  seg.epgeominfo[0].dist = line->GetDist(ii);
//                  seg.epgeominfo[1].edgenr = (*eit).first;
//                  seg.epgeominfo[1].dist = line->GetDist(ii+1);



                 // [INFO] use this to cover all structural edges, which are not part of the chart boundary
                 //
//                  if(additional_edges[(*eit).first])
//                  if(line->segment_info[(*mccit).first].facenum[0] == line->segment_info[(*mccit).first].facenum[1])
//                  {
//                     std::cout << "..adding additional edge" << std::endl;
//                     std::swap(seg.p1, seg.p2);

//                     if(line->segment_info[(*mccit).first].facenum[0] == (*cit).first)
//                     {
//                        std::cout << "..using original left/right trigs.." << std::endl;

//                        seg.geominfo[0].trignum = line->segment_info[(*mccit).first].righttriangles[ii+1];
//                        seg.geominfo[1].trignum = line->segment_info[(*mccit).first].righttriangles[ii];
//                     }
//                     else if(line->segment_info[(*mccit).first].facenum[1] == (*cit).first)
//                     {
//                        std::cout << "..using inverted left/right trigs.." << std::endl;

//                        seg.geominfo[0].trignum = line->segment_info[(*mccit).first].lefttriangles[ii];
//                        seg.geominfo[1].trignum = line->segment_info[(*mccit).first].lefttriangles[ii+1];
//                     }
//                     else
//                     {
//                        std::cout << "..left/right trigs NOT MATCHED.." << std::endl;
//                        seg.geominfo[0].trignum = line->segment_info[(*mccit).first].righttriangles[ii+1];
//                        seg.geominfo[1].trignum = line->segment_info[(*mccit).first].righttriangles[ii];
//                     }

// 		    // [FS] commented
//  		    mesh2.AddSegment(seg);
//                  }

              }
              }
           }
        }
     }
  }


//   std::map<int,int>::iterator aeit;
//   for(aeit = additional_edges.begin(); aeit != additional_edges.end(); ++aeit)
//   {
//      std::cout << "\t\t..edge: " << (*aeit).first << std::endl;

//      STLLine* line = meshlines.Get((*aeit).first);

//      for (int ii = 1; ii <= line->GetNS(); ii++)
//      {
//         int p1, p2;
//         line->GetSeg(ii,p1,p2);

//         std::cout << ".. p1: " << p1 << " .. p2: " << p2
//                   << ".. dist: " << line->GetDist(ii)
//                   << std::endl;

//         vgmnetgen::Segment seg;

//         if(line->segment_info[(*aeit).second].facenum[0] == (*cit).first)
//         {
//            std::cout << "..using original left/right trigs.." << std::endl;
//            seg.geominfo[0].trignum = line->segment_info[(*aeit).second].lefttrig;
//            seg.geominfo[1].trignum = line->segment_info[(*aeit).second].righttrig;
//         }
//         else if(line->segment_info[(*aeit).second].facenum[1] == (*cit).first)
//         {
//            std::cout << "..using inverted left/right trigs.." << std::endl;
//            seg.geominfo[0].trignum = line->segment_info[(*aeit).second].righttrig;
//            seg.geominfo[1].trignum = line->segment_info[(*aeit).second].lefttrig;
//         }
//         else
//         {
//            std::cout << "..left/right trigs NOT MATCHED.." << std::endl;
//            seg.geominfo[0].trignum = line->segment_info[(*aeit).second].lefttrig;
//            seg.geominfo[1].trignum = line->segment_info[(*aeit).second].righttrig;
//         }


//         // [INFO] insert the points according to the base orientation of the edge
//         //
//         seg.p1 = p2;
//         seg.p2 = p1;

//         // [INFO] invert the orientation of the chart according to the material
//         //
//         if(material_chart_mapping[(*mc][(*cit).first])
//         {
//            std::cout << ".. inverting edge because of chart - material orientation" << std::endl;
//            std::swap(seg.p1, seg.p2);
//         }


//         // [INFO] use the triangle number of the line segment calcualted by vgmnetgen only of it is on the same chart
//         //
//         if(geom.GetTriangle(seg.geominfo[0].trignum).GetFaceNum() == geom.GetTriangle(line->GetLeftTrig(ii)).GetFaceNum())
//         {
//            seg.geominfo[0].trignum = line->GetLeftTrig(ii);
// //                     seg.geominfo[1].trignum = line->GetLeftTrig(ii+1);
//         }
//         if(geom.GetTriangle(seg.geominfo[1].trignum).GetFaceNum() == geom.GetTriangle(line->GetRightTrig(ii)).GetFaceNum())
//         {
// //                     seg.geominfo[0].trignum = line->GetRightTrig(ii);
//            seg.geominfo[1].trignum = line->GetRightTrig(ii);
//         }

//         std::cout << "..new approach: p1: " << seg.p1 << " p2: " << seg.p2
//                   << ".. lefttrig: " << seg.geominfo[0].trignum
//                   << ".. righttrig: " << seg.geominfo[1].trignum
//                   << std::endl;

//         seg.si = (*cit).first;
//         seg.edgenr = (*eit).first;

//         mesh2.AddSegment(seg);
//      }

//   }

  // go through the charts
  //
//   std::map<int, std::map<int,int> >::iterator ccit;
//   for(ccit = chart_container.begin(); ccit != chart_container.end(); ++ccit)
//   {
//      std::cout << "\t..chart: " << (*ccit).first << std::endl;

//      int material = -1;
//      std::map<int,int>::iterator mit;
//      for(mit = chart_material_mapping[(*ccit).first].begin(); mit != chart_material_mapping[(*ccit).first].end(); ++mit)
//      {
//         if(material_chart_mapping[(*mit).first][(*ccit).first] == 0)
//            material = (*mit).first;
//      }

//      std::cout << "\t..material: " << material << std::endl;

//      std::map<int,int>::iterator eit;
//      for(eit = chart_container[(*ccit).first].begin(); eit != chart_container[(*ccit).first].end(); ++eit)
//      {
//         std::cout << "\t\t..edge: " << (*eit).first << std::endl;


//         STLLine* line = meshlines.Get((*eit).first);

//         for (int ii = 1; ii <= line->GetNS(); ii++)
//         {
//            int p1, p2;
//            line->GetSeg(ii,p1,p2);

//            std::cout << ".. p1: " << p1 << " .. p2: " << p2
//                      << ".. dist: " << line->GetDist(ii)
//                      << std::endl;

//            vgmnetgen::Segment seg;

//            if(line->segment_info[material].facenum[0] == (*ccit).first)
//            {
//               std::cout << "..using original left/right trigs.." << std::endl;
//               seg.geominfo[0].trignum = line->segment_info[material].lefttrig;
//               seg.geominfo[1].trignum = line->segment_info[material].righttrig;
//            }
//            else if(line->segment_info[material].facenum[1] == (*ccit).first)
//            {
//               std::cout << "..using inverted left/right trigs.." << std::endl;
//               seg.geominfo[0].trignum = line->segment_info[material].righttrig;
//               seg.geominfo[1].trignum = line->segment_info[material].lefttrig;
//            }
//            else
//            {
//               std::cout << "..left/right trigs NOT MATCHED.." << std::endl;
//               seg.geominfo[0].trignum = line->segment_info[material].lefttrig;
//               seg.geominfo[1].trignum = line->segment_info[material].righttrig;
//            }


//            // [FS][NEW]
//            //
//            if((*eit).second)
//            {
//               seg.p1 = p2;
//               seg.p2 = p1;
//            }
//            else
//            {
//               seg.p1 = p1;
//               seg.p2 = p2;
//            }

//            std::cout << "..new approach: p1: " << seg.p1 << " p2: " << seg.p2
//                      << ".. lefttrig: " << seg.geominfo[0].trignum
//                      << ".. righttrig: " << seg.geominfo[1].trignum
//                      << std::endl;

//            seg.si = (*ccit).first;
//            seg.edgenr = (*eit).first;

//            mesh2.AddSegment(seg);
//         }
//      }
//   }


//  mesh2.Save("fs_mesh_new_approach.vol");

  mesh = mesh2;

  // ##### OLD APPROACH #####################

//   for(int f=1; f <= geom.GetNOFaces(); ++f)
//   {
//      used_faces[f] = 0;
//   }

//   for(int m=0; m<geom.material_size; m++)
//   {

//      for(int f=1; f <= geom.GetNOFaces(); ++f)
//      {
//         if(!used_faces[f])
//         {
//         std::cout << ".. face: " << f << std::endl;

//         for (i = 1; i <= geom.GetNT(); i++)
//         {
//            STLTriangle& nt = geom.GetTriangle(i);
//            if(nt.GetFaceNum() == f)
//            {
//               std::cout << nt << " ";
//            }
//         }
//         std::cout << std::endl;

//         for(i = 1; i <= meshlines.Size(); i++)
//         {
//            STLLine* line = meshlines.Get(i);

//            std::cout << " :: line: "   << line->StartP() << "-" << line->EndP() << " "
//                      << mesh.Point(line->StartP()) << "/" << mesh.Point(line->EndP())
//                      << " .. linesegments: " << line->GetNS()
//                      << std::endl;


//            if(line->segment_info[m].facenum[0] == f)
//            {
//               used_faces[f] = 1;
//               std::cout << ".. seg: " << m
//                         << ".. face[0]: "  << line->segment_info[m].facenum[0]
//                         << ".. lefttrig: " << line->segment_info[m].lefttrig
//                         << ".. righttrig: " << line->segment_info[m].righttrig
//                         << ".. " << geom.GetTriangle(line->segment_info[m].lefttrig) << " / "
//                         << ".. " << geom.GetTriangle(line->segment_info[m].righttrig)
//                         << ".. normal: " << geom.GetTriangle(line->segment_info[m].lefttrig).Normal()
//                         << std::endl;

//               // use the +1 index, because the vgmnetgen indices start at 1 not at 0
//               //

//               for (int ii = 1; ii <= line->GetNS(); ii++)
//               {
//                  int p1, p2;
//                  line->GetSeg(ii,p1,p2);

//                  std::cout << ".. p1: " << p1 << " .. p2: " << p2
//                            << ".. dist: " << line->GetDist(ii)
//                            << std::endl;

//                  vgmnetgen::Segment seg;

//                  std::vector<int> tmp_edge;
//                  tmp_edge.push_back(point_map[line->StartP()]);
//                  tmp_edge.push_back(point_map[line->EndP()]);

//                  std::cout << "..using geom edge: " << tmp_edge[0] << " - " << tmp_edge[1] << std::endl;

// //                  if(!find_edge_in_triangle(tmp_edge, geom.GetTriangle(line->segment_info[m].lefttrig)))
// //                  {
// //                     std::swap(p1,p2);
// //                     std::cout << "..inverting p1 and p2 to: " << p1 << " - " << p2 << std::endl;
// //                  }

//                  seg.p1 = p1;
//                  seg.p2 = p2;


//                  if(line->material != m && (geom.GetTriangle(line->GetLeftTrig(1)).GetFaceNum() == line->segment_info[m].facenum[0]) )
//                  {
//                     seg.p1 = p2;
//                     seg.p2 = p1;
//                  }
//                  else if(line->material != m && (geom.GetTriangle(line->GetRightTrig(1)).GetFaceNum() == line->segment_info[m].facenum[0]) )
//                  {
//                     seg.p1 = p1;
//                     seg.p2 = p2;
//                  }
// //                  else if(line->material != m)
// //                  {
// //                     seg.p1 = p2;
// //                     seg.p2 = p1;
// //                  }


//                  seg.geominfo[0].trignum = line->segment_info[m].lefttrig;
//                  seg.geominfo[1].trignum = line->segment_info[m].righttrig;


//                  std::cout << "..line lefttrig: " << line->GetLeftTrig(1) << "..normal: " << geom.GetTriangle(line->GetLeftTrig(1)).Normal()
//                            << "..line righttrig: " << line->GetRightTrig(1) << "..normal: " << geom.GetTriangle(line->GetRightTrig(1)).Normal()
//                            << std::endl;
//                  std::cout << "..segment righttrig: " << line->segment_info[m].righttrig
//                            << "..normal: " << geom.GetTriangle(line->segment_info[m].righttrig).Normal() << std::endl;

//                  if(geom.GetTriangle(line->GetLeftTrig(1)).Normal() == geom.GetTriangle(line->segment_info[m].lefttrig).Normal())
//                  {
//                     std::cout << ".. using same orientation.." << std::endl;
//                     seg.p1 = p1;
//                     seg.p2 = p2;
//                  }
//                  else
//                  {
//                     std::cout << ".. in else using different orientation.. " << std::endl;
//                     seg.p1 = p2;
//                     seg.p2 = p1;
//                  }


//                  seg.si = f;
//                  seg.edgenr = i;

//                  mesh.AddSegment(seg);
//               }
//            }
//            if(line->segment_info[m].facenum[1] == f)
//            {
//               used_faces[f] = 1;
//               std::cout << ".. seg: " << m
//                         << ".. face[1]: " << line->segment_info[m].facenum[1]
//                         << ".. lefttrig: " << line->segment_info[m].lefttrig
//                         << ".. righttrig: " << line->segment_info[m].righttrig
//                         << std::endl;

//               for (int ii = 1; ii <= line->GetNS(); ii++)
//               {
//                  int p1, p2;
//                  line->GetSeg(ii,p1,p2);

//                  std::cout << ".. p1: " << p1 << " .. p2: " << p2
//                            << ".. dist: " << line->GetDist(ii)
//                            << std::endl;

//                  vgmnetgen::Segment seg;

//                  std::vector<int> tmp_edge;
//                  tmp_edge.push_back(point_map[line->EndP()]);
//                  tmp_edge.push_back(point_map[line->StartP()]);

//                  std::cout << "..using geom edge: " << tmp_edge[0] << " - " << tmp_edge[1] << std::endl;

// //                  if(!find_edge_in_triangle(tmp_edge, geom.GetTriangle(line->segment_info[m].righttrig)))
// //                  {
// //                     std::swap(p1,p2);
// //                     std::cout << "..inverting p1 and p2 to: " << p1 << " - " << p2 << std::endl;
// //                  }

// //                  seg.p1 = p2;
// //                  seg.p2 = p1;

// //                  // using facenum[0], because we want to compare the face, that is in common with the other material
// //                  // since it is in common, it has already been meshed => therefore it cannot be the current face => facenum[0]
// //                  // GetTriangle(line->GetLeftTrig(1)).GetFaceNum()
// //                  if(line->material != m && (geom.GetTriangle(line->GetLeftTrig(1)).GetFaceNum() == line->segment_info[m].facenum[0]) )
// //                  {
// //                     seg.p1 = p1;
// //                     seg.p2 = p2;
// //                  }
// //                  else if(line->material != m && (geom.GetTriangle(line->GetRightTrig(1)).GetFaceNum() == line->segment_info[m].facenum[0]) )
// //                  {
// //                     seg.p1 = p2;
// //                     seg.p2 = p1;
// //                  }
// //                  else if(line->material != m)
// //                  {
// //                     seg.p1 = p1;
// //                     seg.p2 = p2;
// //                  }

//                  seg.geominfo[1].trignum = line->segment_info[m].lefttrig;
//                  seg.geominfo[0].trignum = line->segment_info[m].righttrig;


//                  std::cout << "..line lefttrig: " << line->GetLeftTrig(1) << "..normal: " << geom.GetTriangle(line->GetLeftTrig(1)).Normal()
//                            << "..line righttrig: " << line->GetRightTrig(1) << "..normal: " << geom.GetTriangle(line->GetRightTrig(1)).Normal()
//                            << std::endl;
//                  std::cout << "..segment righttrig: " << line->segment_info[m].righttrig
//                            << "..normal: " << geom.GetTriangle(line->segment_info[m].righttrig).Normal() << std::endl;

//                  if( (geom.GetTriangle(line->GetLeftTrig(1)).Normal()[0] == geom.GetTriangle(line->segment_info[m].righttrig).Normal()[0]) &&
//                      (geom.GetTriangle(line->GetLeftTrig(1)).Normal()[1] == geom.GetTriangle(line->segment_info[m].righttrig).Normal()[1]) &&
//                      (geom.GetTriangle(line->GetLeftTrig(1)).Normal()[2] == geom.GetTriangle(line->segment_info[m].righttrig).Normal()[2])
//                     )
//                  {
//                     std::cout << ".. is the same ... " << std::endl;
//                  }


//                  if(geom.GetTriangle(line->GetLeftTrig(1)).Normal() == geom.GetTriangle(line->segment_info[m].righttrig).Normal())
//                  {
//                     std::cout << ".. using same orientation.." << std::endl;
//                     seg.p1 = p1;
//                     seg.p2 = p2;
//                  }
//                  else if(geom.GetTriangle(line->GetRightTrig(1)).Normal() == geom.GetTriangle(line->segment_info[m].righttrig).Normal())
//                  {
//                     std::cout << ".. using different orientation.." << std::endl;
//                     seg.p1 = p2;
//                     seg.p2 = p1;
//                  }
//                  else
//                  {
//                     std::cout << ".. in else using same orientation.. " << std::endl;
//                     seg.p1 = p1;
//                     seg.p2 = p2;
//                  }

//                  seg.si = f;
//                  seg.edgenr = i;

//                  mesh.AddSegment(seg);
//               }
//            }
//         }
//      }
//      }
//      std::cout << std::endl;
//   }



  //
  // [DEBUGALL] end debug faces


// ###############################################################################

/*

  for(i = 1; i <= meshlines.Size(); i++)
  {
     STLLine* line = meshlines.Get(i);

     std::cout << ".. line: " << i
               << " :: edge: " << edge_count
               << std::endl;

     edges[edge_count].push_back(line->StartP());
     edges[edge_count].push_back(line->EndP());

     for(int m=0; m<10; m++)
     {
        if((line->segment_info[m].lefttrig != 0) && (line->segment_info[m].righttrig != 0))
        {
           // an edge is added to the face
           //
           faces[line->segment_info[m].facenum[0]][edge_count] = 0;

           // also store the face normals for later comparison - taken from the adjacent triangle
           //
           std::vector<double> normal1;
           normal1.push_back(geom.GetTriangle(line->segment_info[m].lefttrig).Normal()(0));
           normal1.push_back(geom.GetTriangle(line->segment_info[m].lefttrig).Normal()(1));
           normal1.push_back(geom.GetTriangle(line->segment_info[m].lefttrig).Normal()(2));
           faces_normals[line->segment_info[m].facenum[0]] = normal1;

           // store the first material the face is used in
           // [TODO] get rid of this use the container below - overall_material_faces
           //
           if(faces_material[line->segment_info[m].facenum[0]] == -1)
              faces_material[line->segment_info[m].facenum[0]] = m;

           // store the materials, which this face is part of
           //
           overall_material_faces[line->segment_info[m].facenum[0]][m] = 1;


           // the inverse edge is added here
           //
           faces[line->segment_info[m].facenum[1]][edge_count] = 1;

           // also store the face normals for later comparison - taken from the adjacent triangle
           //
           std::vector<double> normal2;
           normal2.push_back(geom.GetTriangle(line->segment_info[m].righttrig).Normal()(0));
           normal2.push_back(geom.GetTriangle(line->segment_info[m].righttrig).Normal()(1));
           normal2.push_back(geom.GetTriangle(line->segment_info[m].righttrig).Normal()(2));
           faces_normals[line->segment_info[m].facenum[1]] = normal2;

           // store the first material the face is used in
           // [TODO] get rid of this use the container below - overall_material_faces
           //
           if(faces_material[line->segment_info[m].facenum[1]] == -1)
              faces_material[line->segment_info[m].facenum[1]] = m;

           // store the materials, which this face is part of
           //
           overall_material_faces[line->segment_info[m].facenum[1]][m] = 1;
        }
     }

     edge_count++;
  }

  // [INFO] create a continous line of edges
  //
  long start = 0;
  long curr = 1;
  std::vector<int> temp;
  outfaces_container found_holes;

  faces_container::iterator fit;
  long t =0 ;
  for(fit = faces.begin(); fit != faces.end(); ++fit, ++t)
  {
     std::vector<long> used;
     for(int i=0 ; i< edges.size(); ++i)
        used.push_back(-1);

     std::cout << "..next call: " << t << std::endl;

     start = (*(*fit).second.begin()).first;
     long next = start;
     long face = (*fit).first;

     used[start] = start;

     edge_container test;
     test[(*(*fit).second.begin()).first] = (*(*fit).second.begin()).second;
     found_holes[face].push_back(test);

     if( find_next_edge((*fit).second, start, next, used, found_holes, edges, face) )
     {
        temp.push_back(t);
     }
  }


  // [DEBUGALL] START this is just debug output ##############################
  //
  edges_container::iterator eit;
  for(eit = edges.begin(); eit != edges.end(); ++eit)
  {
     std::cout << "edge: " << (*eit).first << " :: " ;

     typedef std::vector<long>::iterator v_iter;
     for(v_iter vit = (*eit).second.begin(); vit != (*eit).second.end(); ++vit)
     {
        std::cout << (*vit) << " " << mesh.Point((*vit))  << " :: ";
     }
     std::cout << std::endl;
  }

  std::cout << ".. faces normal .. " << std::endl;
//   faces_container::iterator fit;
  for(fit = faces.begin(); fit != faces.end(); ++fit)
  {
     std::cout << "face: " << (*fit).first << std::endl;

     edge_container::iterator eit;
     for(eit = (*fit).second.begin(); eit != (*fit).second.end(); ++eit)
     {
        std::cout << "\t " << (*eit).first << " = " << (*eit).second << std::endl;
     }
  }

  std::cout << ".. faces oriented .. " << std::endl;
  for(fit = faces_oriented.begin(); fit != faces_oriented.end(); ++fit)
  {
     std::cout << "face: " << (*fit).first << std::endl;

     edge_container::iterator eit;
     for(eit = (*fit).second.begin(); eit != (*fit).second.end(); ++eit)
     {
        std::cout << "\t " << (*eit).first << " = " << (*eit).second << std::endl;
     }
  }

  // [TEST] debug output
  //
  std::cout << ".. faces found .. " << std::endl;
  outfaces_container::iterator ofit;
  for(ofit = found_holes.begin(); ofit != found_holes.end(); ++ofit)
  {
     std::cout << "face: " << (*ofit).first << std::endl;

     face_container::iterator eit;
     for(eit = (*ofit).second.begin(); eit != (*ofit).second.end(); ++eit)
     {
        edge_container::iterator eeit;
        for(eeit = (*eit).begin(); eeit != (*eit).end(); ++eeit)
        {
           std::cout << "\t " << (*eeit).first << " = " << (*eeit).second << std::endl;
        }
     }
  }

  // [DEBUGALL] output of overall material faces info
  //
  std::cout << "..material faces information .. " << std::endl;
  std::map<long, std::map<int,int> >::iterator omfit;
  for(omfit = overall_material_faces.begin(); omfit != overall_material_faces.end(); ++omfit)
  {
     std::cout << "..face id: "<< (*omfit).first << std::endl;
     std::cout << "\t mat: ";
     std::map<int,int>::iterator mit;
     for(mit = (*omfit).second.begin(); mit != (*omfit).second.end(); ++mit)
     {
        std::cout << (*mit).first << " ";
     }
     std::cout << std::endl;
  }

  // [DEBUGALL] END ############################################################


  // [FS] check the orientation according to the normal vector of the face
  //
//   outfaces_container::iterator ofit;
  for(ofit = found_holes.begin(); ofit != found_holes.end(); ++ofit)
  {
     std::cout << "face: " << (*ofit).first << std::endl;

     // use the first two edge => 3 points to form a triangle
     //
     std::cout << ".. edge points: " << (*(*ofit).second[0].begin()).second << std::endl;

     std::vector<long> edge1, edge2;
     std::cout << " .. here1 " << std::endl;
     if((*(*ofit).second[0].begin()).second == 0)
     {
        edge1 = edges[(*(*ofit).second[0].begin()).first];
     std::cout << " .. here2 " << std::endl;
     }
     else
     {
        edge1.push_back(edges[(*(*ofit).second[0].begin()).first][1]);
        edge1.push_back(edges[(*(*ofit).second[0].begin()).first][0]);
     }

     std::cout << " .. here3 " << std::endl;

     if((*(*ofit).second[1].begin()).second == 0)
     {
        edge2 = edges[(*(*ofit).second[1].begin()).first];
     }
     else
     {
        edge2.push_back(edges[(*(*ofit).second[1].begin()).first][1]);
        edge2.push_back(edges[(*(*ofit).second[1].begin()).first][0]);
     }

     std::cout << ".. edge1: " << edge1[0] << " :: " << edge1[1] << std::endl;
     std::cout << ".. edge2: " << edge2[0] << " :: " << edge2[1] << std::endl;

     gsse::exact predicate;
     typedef double numeric_t;
     typedef gsse::metric_object<numeric_t, 3> point_t;

     point_t p1(mesh.Point(edge1[0])(0), mesh.Point(edge1[0])(1), mesh.Point(edge1[0])(2));
     point_t p2(mesh.Point(edge1[1])(0), mesh.Point(edge1[1])(1), mesh.Point(edge1[1])(2));
     point_t p3(mesh.Point(edge2[1])(0), mesh.Point(edge2[1])(1), mesh.Point(edge2[1])(2));

     std::cout << ".. normal: " << faces_normals[(*ofit).first][0] << " / "
               << faces_normals[(*ofit).first][1] << " / "
               << faces_normals[(*ofit).first][2] << std::endl;

     point_t normal(faces_normals[(*ofit).first][0], faces_normals[(*ofit).first][1], faces_normals[(*ofit).first][2]);

     point_t p4( (p1[0] + normal[0]), (p1[1] + normal[1]), (p1[2] + normal[2])) ;

     std::cout << "..p1: "  << p1
               << " :: p2 " << p2
               << " :: p3 " << p3
               << " :: p4 " << p4
               << std::endl;

     short result = predicate.orient3d(p1, p2, p3, p4);

     if(result < 0)
        faces_orient[(*ofit).first] = 0;
     else if(result > 0)
        faces_orient[(*ofit).first] = 1;
     else
        faces_orient[(*ofit).first] = -1;

     std::cout << "\t\t..result: " << result << std::endl;
  }


  std::cout << ".. faces found .. " << std::endl;
  for(ofit = found_holes.begin(); ofit != found_holes.end(); ++ofit)
  {
     std::cout << "face: " << (*ofit).first << std::endl;

     if(faces_orient[(*ofit).first] == 0)
     {
        face_container::iterator eit;
        for(eit = (*ofit).second.begin(); eit != (*ofit).second.end(); ++eit)
        {
           edge_container::iterator eeit;
           for(eeit = (*eit).begin(); eeit != (*eit).end(); ++eeit)
           {
              std::cout << "\t " << (*eeit).first << " = " << (*eeit).second << std::endl;
           }
        }
     }
     else
     {
        face_container::reverse_iterator eit;
        for(eit = (*ofit).second.rbegin(); eit != (*ofit).second.rend(); ++eit)
        {
           edge_container::reverse_iterator eeit;
           for(eeit = (*eit).rbegin(); eeit != (*eit).rend(); ++eeit)
           {
              // [FS] important do not loose this line - invert the orientation
              //
              (*eeit).second = ((*eeit).second +1) % 2;

              std::cout << "\t " << (*eeit).first << " = " << (*eeit).second << std::endl;
           }
        }
     }
  }



  std::cout << ".. adding line segments to mesh .. " << std::endl;
  for(ofit = found_holes.begin(); ofit != found_holes.end(); ++ofit)
  {
     std::cout << "..face: " << (*ofit).first
               << " :: material: " << faces_material[(*ofit).first]
               << std::endl;

     // [INFO] distinguish between two cases:
     //        1) face is oriented correct  == 0
     //        2) face needs to be inverted != 0
     //
     if(faces_orient[(*ofit).first] == 0)
     {
        face_container::iterator eit;
        for(eit = (*ofit).second.begin(); eit != (*ofit).second.end(); ++eit)
        {
           edge_container::iterator eeit;
           for(eeit = (*eit).begin(); eeit != (*eit).end(); ++eeit)
           {
              std::cout << "\t " << (*eeit).first << " = " << (*eeit).second << std::endl;

              int invert = 0;
              invert = (*eeit).second;

              STLLine* line = meshlines.Get((*eeit).first+1);
//               int mat = line->material;
              int mat = faces_material[(*ofit).first];

              for (j = 1; j <= line->GetNS(); j++)
              {
                 Segment seg1;

                 int p1, p2;
                 line->GetSeg(j, p1, p2);

                 if(invert)
                 {
                    seg1.p1 = p2;
                    seg1.p2 = p1;

                    seg1.epgeominfo[0].dist = line->GetDist(j+1);
                    seg1.epgeominfo[1].dist = line->GetDist(j);

                    // check if the correct triangle is given - depends on the orientation
                    // should be correct, but in some ways it is not working
                    // [TODO] investigate why in some ways the wrong triangle order is stored in geominfo
                    //
                    if((*ofit).first == geom.GetTriangle(line->segment_info[faces_material[(*ofit).first]].righttrig).GetFaceNum())
                    {
                       seg1.geominfo[0].trignum = line->segment_info[faces_material[(*ofit).first]].righttrig;
                       seg1.geominfo[1].trignum = line->segment_info[faces_material[(*ofit).first]].lefttrig;
                    }
                    else
                    {
                       seg1.geominfo[1].trignum = line->segment_info[faces_material[(*ofit).first]].righttrig;
                       seg1.geominfo[0].trignum = line->segment_info[faces_material[(*ofit).first]].lefttrig;
                    }
                 }
                 else
                 {
                    seg1.p1 = p1;
                    seg1.p2 = p2;

                    seg1.epgeominfo[0].dist = line->GetDist(j);
                    seg1.epgeominfo[1].dist = line->GetDist(j+1);

                    if((*ofit).first == geom.GetTriangle(line->segment_info[faces_material[(*ofit).first]].lefttrig).GetFaceNum())
                    {
                       seg1.geominfo[0].trignum = line->segment_info[faces_material[(*ofit).first]].lefttrig;
                       seg1.geominfo[1].trignum = line->segment_info[faces_material[(*ofit).first]].righttrig;
                    }
                    else
                    {
                       seg1.geominfo[1].trignum = line->segment_info[faces_material[(*ofit).first]].lefttrig;
                       seg1.geominfo[0].trignum = line->segment_info[faces_material[(*ofit).first]].righttrig;
                    }
                 }

                 std::cout << ".. p1: " << seg1.p1 << " :: p2: " << seg1.p2 << std::endl;

                 // face number
                 //
                 seg1.si = (*ofit).first;

//               std::cout << "..facenum comp: trig1: " << geom.GetTriangle(trig1).GetFaceNum()
//                         << " :: line: " << seg1.si << std::endl;

                 // edge number
                 seg1.edgenr = (*eeit).first+1;

                 // [FS] material info
                 //
                 seg1.domin = mat;
                 std::cout << ".. setting seg mat: " << seg1.domin << std::endl;

                 seg1.epgeominfo[0].edgenr = (*eeit).first+1;
                 seg1.epgeominfo[1].edgenr = (*eeit).first+1;


//                  std::cout << ".. facenum[0]: " << line->segment_info[m].facenum[0]
//                            << ".. facenum[1]: " << line->segment_info[m].facenum[1]
//                            << std::endl;

                 std::cout << ".. adding segment: " << seg1.p1 << " / " << seg1.p2 << std::endl;
                 mesh.AddSegment(seg1);
              }
           }
        }
     }
     else
     {
        face_container::reverse_iterator eit;
        for(eit = (*ofit).second.rbegin(); eit != (*ofit).second.rend(); ++eit)
        {
           edge_container::reverse_iterator eeit;
           for(eeit = (*eit).rbegin(); eeit != (*eit).rend(); ++eeit)
           {
              std::cout << "\t " << (*eeit).first << " = " << (*eeit).second << std::endl;

              int invert = 0;
              invert = (*eeit).second;

              STLLine* line = meshlines.Get((*eeit).first+1);
              int mat = line->material;

              for (j = 1; j <= line->GetNS(); j++)
              {
                 Segment seg1;

                 int p1, p2;
                 line->GetSeg(j, p1, p2);

                 if(invert)
                 {
                    seg1.p1 = p2;
                    seg1.p2 = p1;

                    seg1.epgeominfo[0].dist = line->GetDist(j+1);
                    seg1.epgeominfo[1].dist = line->GetDist(j);

                    if((*ofit).first == geom.GetTriangle(line->segment_info[faces_material[(*ofit).first]].righttrig).GetFaceNum())
                    {
                       seg1.geominfo[0].trignum = line->segment_info[faces_material[(*ofit).first]].righttrig;
                       seg1.geominfo[1].trignum = line->segment_info[faces_material[(*ofit).first]].lefttrig;
                    }
                    else
                    {
                       seg1.geominfo[1].trignum = line->segment_info[faces_material[(*ofit).first]].righttrig;
                       seg1.geominfo[0].trignum = line->segment_info[faces_material[(*ofit).first]].lefttrig;
                    }

                 }
                 else
                 {
                    seg1.p1 = p1;
                    seg1.p2 = p2;

                    seg1.epgeominfo[0].dist = line->GetDist(j);
                    seg1.epgeominfo[1].dist = line->GetDist(j+1);

                    if((*ofit).first == geom.GetTriangle(line->segment_info[faces_material[(*ofit).first]].lefttrig).GetFaceNum())
                    {
                       seg1.geominfo[0].trignum = line->segment_info[faces_material[(*ofit).first]].lefttrig;
                       seg1.geominfo[1].trignum = line->segment_info[faces_material[(*ofit).first]].righttrig;
                    }
                    else
                    {
                       seg1.geominfo[0].trignum = line->segment_info[faces_material[(*ofit).first]].righttrig;
                       seg1.geominfo[1].trignum = line->segment_info[faces_material[(*ofit).first]].lefttrig;
                    }
                 }

                 std::cout << ".. p1: " << seg1.p1 << " :: p2: " << seg1.p2 << std::endl;
                 std::cout << "..trig1: " << seg1.geominfo[0].trignum
                           << " :: trig2: " << seg1.geominfo[1].trignum
                           << std::endl;

                 // face number
                 //
                 seg1.si = (*ofit).first;

//               std::cout << "..facenum comp: trig1: " << geom.GetTriangle(trig1).GetFaceNum()
//                         << " :: line: " << seg1.si << std::endl;

                 // edge number
                 seg1.edgenr = (*eeit).first+1;

                 // [FS] material info
                 //
                 seg1.domin = mat;
                 std::cout << ".. setting seg mat: " << seg1.domin << std::endl;

                 seg1.epgeominfo[0].edgenr = (*eeit).first+1;
                 seg1.epgeominfo[1].edgenr = (*eeit).first+1;


//                  std::cout << ".. facenum[0]: " << line->segment_info[m].facenum[0]
//                            << ".. facenum[1]: " << line->segment_info[m].facenum[1]
//                            << std::endl;

                 std::cout << ".. adding segment: " << seg1.p1 << " / " << seg1.p2 << std::endl;
                 mesh.AddSegment(seg1);
              }
           }

        }
     }
  }

*/

#ifdef DEBUGALL
  std::cout << ".. number of facedescriptors 2: " << mesh.GetNFD() << std::endl;
#endif


  // [FS] .. an old face descriptor test
  //
  /*
  mesh.ClearFaceDescriptors();

  std::cout << ".. facedescriptor test: mesh.GetNSeg 2: " << mesh.GetNSeg() << std::endl;

  for (int i = 1; i <= mesh.GetNSeg(); i++)
    {
      std::cout << "segment " << mesh.LineSegment(i) << " :: material: " << mesh.LineSegment(i).domin << std::endl;

      // [FS][TEST] .. set all lines from the interface face to 0, meaning, that they are not meshed/used
      //
//       if(mesh.LineSegment(i).si == 14 || mesh.LineSegment(i).si == 15 || mesh.LineSegment(i).si == 16 || mesh.LineSegment(i).si == 17)
//       if(mesh.LineSegment(i).si == 11)
// 	{
// 	  mesh.LineSegment(i).si = 0;
// 	}
//       else
	{
	  int ok = 0;
	  for (int k = 1; k <= mesh.GetNFD(); k++)
	    {

		{
		  if (mesh.GetFaceDescriptor(k).SegmentFits (mesh.LineSegment(i)))
		    {
		      ok = k;
		      std::cout << "fits to " << k << std::endl;
		    }
		}
	    }
	  if (!ok)
	    {
	      ok = mesh.AddFaceDescriptor (FaceDescriptor (mesh.LineSegment(i)));

	      std::cout << "add new FaceDescriptor, now " << ok << " :: " << mesh.GetFaceDescriptor(ok) << std::endl;
	    }

	  std::cout << "change from " << mesh.LineSegment(i).si;

	  mesh.LineSegment(i).si = ok;

	  std::cout << " to " << mesh.LineSegment(i).si << std::endl;
	}
    }
  */

#ifdef DEBUGALL
  std::cout << ".. number of facedescriptors: " << mesh.GetNFD() << std::endl;
#endif

  //
  // END facedescriptor test


  PopStatus();

#ifdef DEBUGALL
  std::cout << "[FS] .. leaving STLFindEdges .." << std::endl;
#endif
}




void STLSurfaceMeshing1 (STLGeometry & geom,
			 class Mesh & mesh,
			 int retrynr);

int STLSurfaceMeshing (STLGeometry & geom,
		       class Mesh & mesh)
{
  int i, j;

#ifdef DEBUGALL
  PrintFnStart("Do Surface Meshing");
  std::cout << "[FS] .. in STLSurfaceMeshing .." << std::endl;
#endif

  geom.PrepareSurfaceMeshing();

#ifdef DEBUGALL
  mesh.Save ("mesh1.edges");
#endif

  // [FS] datastructure used for the face to material mapping
  //
  std::map<int, std::map<int,int> > material_chart_mapping;

//   geom.Save("fs_geometry_before.stl");

  if (mesh.GetNSeg() == 0)
    {
#ifdef DEBUGALL
      std::cout << " .. mesh.GetNSeg() is empty" << std::endl;
#endif
      STLFindEdges (geom, mesh, material_chart_mapping);
    }

#ifdef DEBUGALL
  geom.Save("fs_geometry_after.stl");
#endif

  int nopen;
  int outercnt = 20;

#ifdef DEBUGALL
  mesh.Save ("mesh2.edges");
#endif

  // [FS] test if mesh with edges and faces can be loaded
  //
//   Mesh mesh3;
//   mesh.Load("fs_surface_test.vol");
//   std::cout << " .. before assign .. " << std::endl;
//   mesh = mesh3;
//   std::cout << " .. after load and assign .. " << std::endl;

#ifdef DEBUGALL
  mesh.Save("mesh3.edges");
#endif

#ifdef DEBUGALL
  std::cout << "..number of segment: " << mesh.GetNSeg() << std::endl;
  std::cout << "..number of points: " << mesh.GetNP() << std::endl;
  std::cout << "..number of edges: " << mesh.GetNE() << std::endl;
  std::cout << "..number of facedescr: " << mesh.GetNFD() << std::endl;
#endif

#ifdef DEBUGALL
  for (i = 1; i <= mesh.GetNSeg(); i++)
    {
      const Segment & seg = mesh.LineSegment (i);

      std::cout << ".. linesegment " << i << " :: " << seg << std::endl;

      if (seg.geominfo[0].trignum <= 0 || seg.geominfo[1].trignum <= 0)
	{
	  std::cout << "Problem with segment " << i << ": " << seg << endl;
	}
    }
#endif

  do
    {
      outercnt--;
      if (outercnt <= 0)
	  return MESHING3_OUTERSTEPSEXCEEDED;

      if (multithread.terminate)
	{
	  return MESHING3_TERMINATE;
	}

      mesh.FindOpenSegments();
      nopen = mesh.GetNOpenSegments();

      if (nopen)
	{
	  int trialcnt = 0;
	  while (nopen && trialcnt <= 5)
	    {

	      // [FS] .. test
	      //
#ifdef DEBUGALL
	      std::string blabla;
	      std::cout << "============================================================================================================" << std::endl;
	      std::cout << ".. wait for input in STLSurfaceMeshing .. ==================================================================" << std::endl;
#endif
//   	      cin >> blabla;

	      if (multithread.terminate)
		{
		  return MESHING3_TERMINATE;
		}
	      trialcnt++;
	      STLSurfaceMeshing1 (geom, mesh, trialcnt);

#ifdef DEBUGALL
	      mesh.Save("fs_surface_mesh.vol");
              geom.Save("fs_geometry.stl");
#endif

	      mesh.FindOpenSegments();
	      nopen = mesh.GetNOpenSegments();
//  	      nopen = 0;

#ifdef DEBUGALL
	      std::cout << ".. nopen after meshing run: " << nopen << std::endl;
#endif

	      if (nopen)
		{
		  geom.ClearMarkedSegs();
		  for (i = 1; i <= nopen; i++)
		    {
		      const Segment & seg = mesh.GetOpenSegment (i);

#ifdef DEBUGALL
		      std::cout << "..open segment: " << seg << std::endl;
#endif

		      geom.AddMarkedSeg(mesh.Point(seg.p1),mesh.Point(seg.p2));
		    }

		  geom.InitMarkedTrigs();
		  for (i = 1; i <= nopen; i++)
		    {
		      const Segment & seg = mesh.GetOpenSegment (i);
		      geom.SetMarkedTrig(seg.geominfo[0].trignum,1);
		      geom.SetMarkedTrig(seg.geominfo[1].trignum,1);
		    }

		  MeshOptimizeSTLSurface optmesh(geom);
		  optmesh.SetFaceIndex (0);
		  optmesh.SetImproveEdges (0);
		  optmesh.SetMetricWeight (0);

		  mesh.CalcSurfacesOfNode();
		  optmesh.EdgeSwapping (mesh, 0);
		  mesh.CalcSurfacesOfNode();
		  optmesh.ImproveMesh (mesh);
		}

// 	      mesh.Compress();
// 	      mesh.FindOpenSegments();
// 	      nopen = mesh.GetNOpenSegments();

	      if (trialcnt <= 5 && nopen)
		{
		  mesh.RemoveOneLayerSurfaceElements();

		  if (trialcnt >= 4)
		    {
		      mesh.FindOpenSegments();
		      mesh.RemoveOneLayerSurfaceElements();

		      mesh.FindOpenSegments ();
		      nopen = mesh.GetNOpenSegments();
		    }
		}

#ifdef DEBUGALL
              std::cout << ".. end of while .. " << std::endl;
#endif
	    }


	  if (multithread.terminate)
	    return MESHING3_TERMINATE;

	  if (nopen)
	    {

//	      PrintMessage(3,"Meshing failed, trying to refine");

	      mesh.FindOpenSegments ();
	      nopen = mesh.GetNOpenSegments();

	      mesh.FindOpenSegments ();
	      mesh.RemoveOneLayerSurfaceElements();
	      mesh.FindOpenSegments ();
	      mesh.RemoveOneLayerSurfaceElements();

	      // Open edge-segments will be refined !
	      INDEX_2_HASHTABLE<int> openseght (nopen+1);
	      for (i = 1; i <= mesh.GetNOpenSegments(); i++)
		{
		  const Segment & seg = mesh.GetOpenSegment (i);
		  INDEX_2 i2(seg.p1, seg.p2);
		  i2.Sort();
		  openseght.Set (i2, 1);
		}


	      mesh.FindOpenSegments ();
	      mesh.RemoveOneLayerSurfaceElements();
	      mesh.FindOpenSegments ();
	      mesh.RemoveOneLayerSurfaceElements();


	      INDEX_2_HASHTABLE<int> newpht(100);

	      int nsegold = mesh.GetNSeg();
	      for (i = 1; i <= nsegold; i++)
		{
		  Segment seg = mesh.LineSegment(i);
		  INDEX_2 i2(seg.p1, seg.p2);
		  i2.Sort();
		  if (openseght.Used (i2))
		    {
		      // segment will be split
//		      PrintMessage(7,"Split segment ", int(seg.p1), "-", int(seg.p2));

		      Segment nseg1, nseg2;
		      EdgePointGeomInfo newgi;

		      const EdgePointGeomInfo & gi1 = seg.epgeominfo[0];
		      const EdgePointGeomInfo & gi2 = seg.epgeominfo[1];

		      newgi.dist = 0.5 * (gi1.dist + gi2.dist);
		      newgi.edgenr = gi1.edgenr;

		      int hi;

		      Point3d newp;
		      int newpi;

		      if (!newpht.Used (i2))
			{
			  newp = geom.GetLine (gi1.edgenr)->
			    GetPointInDist (geom.GetPoints(), newgi.dist, hi);
			  newpi = mesh.AddPoint (newp);
			  newpht.Set (i2, newpi);
			}
		      else
			{
			  newpi = newpht.Get (i2);
			  newp = mesh.Point (newpi);
			}

		      nseg1 = seg;
		      nseg2 = seg;
		      nseg1.p2 = newpi;
		      nseg1.epgeominfo[1] = newgi;

		      nseg2.p1 = newpi;
		      nseg2.epgeominfo[0] = newgi;

		      mesh.LineSegment(i) = nseg1;
		      mesh.AddSegment (nseg2);

		      mesh.RestrictLocalH (Center (mesh.Point(nseg1.p1),
						   mesh.Point(nseg1.p2)),
					   Dist (mesh.Point(nseg1.p1),
						 mesh.Point(nseg1.p2)));
		      mesh.RestrictLocalH (Center (mesh.Point(nseg2.p1),
						   mesh.Point(nseg2.p2)),
					   Dist (mesh.Point(nseg2.p1),
						 mesh.Point(nseg2.p2)));
		    }
		}

	    }

	  nopen = -1;
	}

      else

	{
#ifdef DEBUGALL
	  std::cout << "mesh is closed, verifying ..." << std::endl;
#endif

	  // [INFO] no open elements, check wrong elements (intersecting..)
	  //
//	  PrintMessage(5,"check overlapping");
	  // 	  mesh.FindOpenElements(); // would leed to locked points
	  if(mesh.CheckOverlappingBoundary())
	    {
	      return MESHING3_BADSURFACEMESH;
	    }


	  geom.InitMarkedTrigs();

	  for (i = 1; i <= mesh.GetNSE(); i++)
	    if (mesh.SurfaceElement(i).BadElement())
	      {
		int trig = mesh.SurfaceElement(i).PNum(1);
		geom.SetMarkedTrig(trig,1);
//		PrintMessage(7, "overlapping element, will be removed");
	      }



	  ARRAY<Point3d> refpts;
	  ARRAY<double> refh;

	  // was commented:

	  for (i = 1; i <= mesh.GetNSE(); i++)
	    if (mesh.SurfaceElement(i).BadElement())
	      {
		for (j = 1; j <= 3; j++)
		  {
		    refpts.Append (mesh.Point (mesh.SurfaceElement(i).PNum(j)));
		    refh.Append (mesh.GetH (refpts.Last()) / 2);
		  }
		mesh.DeleteSurfaceElement(i);
	      }

	  // [FS] .. commenting out this part creates a surface mesh - this surface mesh is currently
	  //         not consistent because the connecting layer is not meshed right
	  // delete wrong oriented element
	  for (i = 1; i <= mesh.GetNSE(); i++)
	    {
	      const Element2d & el = mesh.SurfaceElement(i);
	      if (!el.PNum(1))
		continue;

	      Vec3d n = Cross (Vec3d (mesh.Point(el.PNum(1)),
				      mesh.Point(el.PNum(2))),
			       Vec3d (mesh.Point(el.PNum(1)),
				      mesh.Point(el.PNum(3))));
	      Vec3d ng = geom.GetTriangle(el.GeomInfoPi(1).trignum).Normal();
	      if (n * ng < 0)
		{
		  refpts.Append (mesh.Point (mesh.SurfaceElement(i).PNum(1)));
		  refh.Append (mesh.GetH (refpts.Last()) / 2);
		  mesh.DeleteSurfaceElement(i);
		}
	    }
	  // end comments

	  for (i = 1; i <= refpts.Size(); i++)
	    mesh.RestrictLocalH (refpts.Get(i), refh.Get(i));

	  mesh.RemoveOneLayerSurfaceElements();

	  mesh.Compress();

	  mesh.FindOpenSegments ();
	  nopen = mesh.GetNOpenSegments();

	  /*
	  if (!nopen)
	    {
	      // mesh is still ok

	      void STLSurfaceOptimization (STLGeometry & geom,
					   class Mesh & mesh,
					   MeshingParameters & mparam)

	    }
	  */
	}
#ifdef DEBUGALL
      std::cout << "..end while: nopen : " << nopen << std::endl;
#endif
    }
  while (nopen);

#ifdef DEBUGALL
  mesh.Save("fs_surface_mesh.vol");
#endif

  mesh.Compress();
  mesh.CalcSurfacesOfNode();

  STLSurfaceOptimization (geom, mesh, mparam);

  // #######################################################################################
  // FS convert to GSSE file
  //
#ifdef DEBUGALL
  std::cout << ".. converting mesh to gsse .. " << std::endl;
#endif
  std::ofstream outfile("surface_mesh.gau32");

//  std::cout << "..material faces information .. " << std::endl;
  std::map<int,int> materials;
//   std::map<int, std::map<int,int> >::iterator mcit;
//   for(mcit = material_chart_mapping.begin(); mcit != material_chart_mapping.end(); ++mcit)
//   {
//      std::cout << "..face id: "<< (*mcit).first << std::endl;
//      std::cout << "\t mat: ";
//      std::map<int,int>::iterator mit;
//      for(mit = (*omfit).second.begin(); mit != (*omfit).second.end(); ++mit)
//      {
//         std::cout << (*mit).first << " ";
//         materials[(*mit).first] = 1;
//      }
//      std::cout << std::endl;
//   }

  typedef SpaceGenerator<3, 2, 1>::FullSpace    FullSpace32;

  FullSpace32 output_space;

  convert_mesh_to_gsse(mesh, output_space, material_chart_mapping);
  initialize_topology(output_space);
  write_space(output_space, outfile);

  // END convert to GSSE file
  // #######################################################################################

#ifdef DEBUGALL
  std::cout << ".. leaving STLSurfaceMeshing .. " << std::endl;
#endif

  return MESHING3_OK;
}


// [JW] this is the specialization for viennagrid::domain:
//      the mesh result is written into this domain
//
int STLSurfaceMeshing (STLGeometry & geom,
		       class Mesh & mesh,
               viennagrid::mesh_t const & vgrid_mesh)
{
  int i, j;

  //mparam.Print(std::cout);

#ifdef DEBUGALL
  PrintFnStart("Do Surface Meshing");
  std::cout << "[FS] .. in STLSurfaceMeshing .." << std::endl;
#endif

  geom.PrepareSurfaceMeshing();

#ifdef DEBUGALL
  mesh.Save ("mesh1.edges");
#endif

  // [FS] datastructure used for the face to material mapping
  //
  std::map<int, std::map<int,int> > material_chart_mapping;

//   geom.Save("fs_geometry_before.stl");

  if (mesh.GetNSeg() == 0)
    {
#ifdef DEBUGALL
      std::cout << " .. mesh.GetNSeg() is empty" << std::endl;
#endif
      STLFindEdges (geom, mesh, material_chart_mapping);
    }

#ifdef DEBUGALL
  geom.Save("fs_geometry_after.stl");
#endif

  int nopen;
  int outercnt = 20;

#ifdef DEBUGALL
  mesh.Save ("mesh2.edges");
#endif

  // [FS] test if mesh with edges and faces can be loaded
  //
//   Mesh mesh3;
//   mesh.Load("fs_surface_test.vol");
//   std::cout << " .. before assign .. " << std::endl;
//   mesh = mesh3;
//   std::cout << " .. after load and assign .. " << std::endl;

#ifdef DEBUGALL
  mesh.Save("mesh3.edges");
#endif

#ifdef DEBUGALL
  std::cout << "..number of segment: " << mesh.GetNSeg() << std::endl;
  std::cout << "..number of points: " << mesh.GetNP() << std::endl;
  std::cout << "..number of edges: " << mesh.GetNE() << std::endl;
  std::cout << "..number of facedescr: " << mesh.GetNFD() << std::endl;
#endif

#ifdef DEBUGALL
  for (i = 1; i <= mesh.GetNSeg(); i++)
  {
    const Segment & seg = mesh.LineSegment (i);

    std::cout << ".. linesegment " << i << " :: " << seg << std::endl;

    if (seg.geominfo[0].trignum <= 0 || seg.geominfo[1].trignum <= 0)
    {
      std::cout << "Problem with segment " << i << ": " << seg << endl;
    }
  }
#endif

  do
  {
    outercnt--;
    if (outercnt <= 0)          return MESHING3_OUTERSTEPSEXCEEDED;
    if (multithread.terminate)  return MESHING3_TERMINATE;

    mesh.FindOpenSegments();
    nopen = mesh.GetNOpenSegments();

    if (nopen)
    {
      int trialcnt = 0;
      while (nopen && trialcnt <= 5)
      {
        // [FS] .. test
        //
      #ifdef DEBUGALL
        std::string blabla;
        std::cout << "============================================================================================================" << std::endl;
        std::cout << ".. wait for input in STLSurfaceMeshing .. ==================================================================" << std::endl;
      #endif
        if (multithread.terminate)  return MESHING3_TERMINATE;
        trialcnt++;
        // -----------------------------------------------
        // -----------------------------------------------
        STLSurfaceMeshing1 (geom, mesh, trialcnt);
        // -----------------------------------------------
        // -----------------------------------------------
      #ifdef DEBUGALL
        mesh.Save("fs_surface_mesh.vol");
        geom.Save("fs_geometry.stl");
      #endif

        mesh.FindOpenSegments();
        nopen = mesh.GetNOpenSegments();

      #ifdef DEBUGALL
        std::cout << ".. nopen after meshing run: " << nopen << std::endl;
      #endif

        if (nopen)
        {
          geom.ClearMarkedSegs();
          for (i = 1; i <= nopen; i++)
          {
            const Segment & seg = mesh.GetOpenSegment (i);
          #ifdef DEBUGALL
            std::cout << "..open segment: " << seg << std::endl;
          #endif
            geom.AddMarkedSeg(mesh.Point(seg.p1),mesh.Point(seg.p2));
          }

          geom.InitMarkedTrigs();
          for (i = 1; i <= nopen; i++)
          {
            const Segment & seg = mesh.GetOpenSegment (i);
            geom.SetMarkedTrig(seg.geominfo[0].trignum,1);
            geom.SetMarkedTrig(seg.geominfo[1].trignum,1);
          }
          MeshOptimizeSTLSurface optmesh(geom);
          optmesh.SetFaceIndex (0);
          optmesh.SetImproveEdges (0);
          optmesh.SetMetricWeight (0);

          mesh.CalcSurfacesOfNode();
          optmesh.EdgeSwapping (mesh, 0);
          mesh.CalcSurfacesOfNode();
          optmesh.ImproveMesh (mesh);
        }

        // mesh.Compress();
        // mesh.FindOpenSegments();
        // nopen = mesh.GetNOpenSegments();

        if (trialcnt <= 5 && nopen)
        {
          mesh.RemoveOneLayerSurfaceElements();

          if (trialcnt >= 4)
          {
            mesh.FindOpenSegments();
            mesh.RemoveOneLayerSurfaceElements();
            mesh.FindOpenSegments ();
            nopen = mesh.GetNOpenSegments();
          }
        }
      #ifdef DEBUGALL
        std::cout << ".. end of while .. " << std::endl;
      #endif
      }

      if (multithread.terminate)  return MESHING3_TERMINATE;

      if (nopen)
      {
//	      PrintMessage(3,"Meshing failed, trying to refine");
        mesh.FindOpenSegments ();
        nopen = mesh.GetNOpenSegments();
        mesh.FindOpenSegments ();
        mesh.RemoveOneLayerSurfaceElements();
        mesh.FindOpenSegments ();
        mesh.RemoveOneLayerSurfaceElements();

        // Open edge-segments will be refined !
        INDEX_2_HASHTABLE<int> openseght (nopen+1);
        for (i = 1; i <= mesh.GetNOpenSegments(); i++)
        {
          const Segment & seg = mesh.GetOpenSegment (i);
          INDEX_2 i2(seg.p1, seg.p2);
          i2.Sort();
          openseght.Set (i2, 1);
        }

        mesh.FindOpenSegments ();
        mesh.RemoveOneLayerSurfaceElements();
        mesh.FindOpenSegments ();
        mesh.RemoveOneLayerSurfaceElements();

        INDEX_2_HASHTABLE<int> newpht(100);

        int nsegold = mesh.GetNSeg();
        for (i = 1; i <= nsegold; i++)
        {
          Segment seg = mesh.LineSegment(i);
          INDEX_2 i2(seg.p1, seg.p2);
          i2.Sort();
          if (openseght.Used (i2))
          {
            // segment will be split
            // PrintMessage(7,"Split segment ", int(seg.p1), "-", int(seg.p2));
            Segment nseg1, nseg2;
            EdgePointGeomInfo newgi;

            const EdgePointGeomInfo & gi1 = seg.epgeominfo[0];
            const EdgePointGeomInfo & gi2 = seg.epgeominfo[1];

            newgi.dist = 0.5 * (gi1.dist + gi2.dist);
            newgi.edgenr = gi1.edgenr;

            int hi;

            Point3d newp;
            int newpi;

            if (!newpht.Used (i2))
            {
              newp = geom.GetLine (gi1.edgenr)->
              GetPointInDist (geom.GetPoints(), newgi.dist, hi);
              newpi = mesh.AddPoint (newp);
              newpht.Set (i2, newpi);
            }
            else
            {
              newpi = newpht.Get (i2);
              newp = mesh.Point (newpi);
            }

            nseg1 = seg;
            nseg2 = seg;
            nseg1.p2 = newpi;
            nseg1.epgeominfo[1] = newgi;

            nseg2.p1 = newpi;
            nseg2.epgeominfo[0] = newgi;

            mesh.LineSegment(i) = nseg1;
            mesh.AddSegment (nseg2);

            mesh.RestrictLocalH (
              Center(mesh.Point(nseg1.p1),mesh.Point(nseg1.p2)),
              Dist (mesh.Point(nseg1.p1), mesh.Point(nseg1.p2))
            );
            mesh.RestrictLocalH (
              Center(mesh.Point(nseg2.p1),mesh.Point(nseg2.p2)),
              Dist (mesh.Point(nseg2.p1), mesh.Point(nseg2.p2)));
          }
        }
      }
      nopen = -1;
    }
    else
    {
  #ifdef DEBUGALL
      std::cout << "mesh is closed, verifying ..." << std::endl;
  #endif

      // [INFO] no open elements, check wrong elements (intersecting..)
      //
      // PrintMessage(5,"check overlapping");
      // 	  mesh.FindOpenElements(); // would leed to locked points
      if(mesh.CheckOverlappingBoundary())
      {
        return MESHING3_BADSURFACEMESH;
      }

      geom.InitMarkedTrigs();

      for (i = 1; i <= mesh.GetNSE(); i++)
        if (mesh.SurfaceElement(i).BadElement())
        {
          int trig = mesh.SurfaceElement(i).PNum(1);
          geom.SetMarkedTrig(trig,1);
          // PrintMessage(7, "overlapping element, will be removed");
        }

      ARRAY<Point3d> refpts;
      ARRAY<double> refh;

      // was commented:

      for (i = 1; i <= mesh.GetNSE(); i++)
        if (mesh.SurfaceElement(i).BadElement())
        {
          for (j = 1; j <= 3; j++)
          {
            refpts.Append (mesh.Point (mesh.SurfaceElement(i).PNum(j)));
            refh.Append (mesh.GetH (refpts.Last()) / 2);
          }
          mesh.DeleteSurfaceElement(i);
        }

      // [FS] .. commenting out this part creates a surface mesh - this surface mesh is currently
      //         not consistent because the connecting layer is not meshed right
      // delete wrong oriented element
      for (i = 1; i <= mesh.GetNSE(); i++)
      {
        const Element2d & el = mesh.SurfaceElement(i);
        if (!el.PNum(1))  continue;

        Vec3d n = Cross (Vec3d (mesh.Point(el.PNum(1)),
        mesh.Point(el.PNum(2))),
        Vec3d (mesh.Point(el.PNum(1)),
        mesh.Point(el.PNum(3))));
        Vec3d ng = geom.GetTriangle(el.GeomInfoPi(1).trignum).Normal();
        if (n * ng < 0)
        {
          refpts.Append (mesh.Point (mesh.SurfaceElement(i).PNum(1)));
          refh.Append (mesh.GetH (refpts.Last()) / 2);
          mesh.DeleteSurfaceElement(i);
        }
      }
      // end comments

      for (i = 1; i <= refpts.Size(); i++)
        mesh.RestrictLocalH (refpts.Get(i), refh.Get(i));

      mesh.RemoveOneLayerSurfaceElements();
      mesh.Compress();
      mesh.FindOpenSegments ();
      nopen = mesh.GetNOpenSegments();

//      if (!nopen)
//      {
//        // mesh is still ok
//        void STLSurfaceOptimization (STLGeometry & geom, class Mesh & mesh, MeshingParameters & mparam)
//      }
    }
  #ifdef DEBUGALL
    std::cout << "..end while: nopen : " << nopen << std::endl;
  #endif
  }
  while (nopen);

#ifdef DEBUGALL
  mesh.Save("fs_surface_mesh.vol");
#endif

  mesh.Compress();
  mesh.CalcSurfacesOfNode();

  STLSurfaceOptimization (geom, mesh, mparam);

  // #######################################################################################
  // FS convert to GSSE file
  //
#ifdef DEBUGALL
  std::cout << ".. converting mesh to gsse .. " << std::endl;
#endif
//  std::cout << "..material faces information .. " << std::endl;
  std::map<int,int> materials;
//   std::map<int, std::map<int,int> >::iterator mcit;
//   for(mcit = material_chart_mapping.begin(); mcit != material_chart_mapping.end(); ++mcit)
//   {
//      std::cout << "..face id: "<< (*mcit).first << std::endl;
//      std::cout << "\t mat: ";
//      std::map<int,int>::iterator mit;
//      for(mit = (*omfit).second.begin(); mit != (*omfit).second.end(); ++mit)
//      {
//         std::cout << (*mit).first << " ";
//         materials[(*mit).first] = 1;
//      }
//      std::cout << std::endl;
//   }

  typedef SpaceGenerator<3, 2, 1>::FullSpace    FullSpace;

  FullSpace fullspace;

  convert_mesh_to_gsse(mesh, fullspace, material_chart_mapping);
  initialize_topology(fullspace);
//  write_space(output_space, outfile);

// [JW] here we transfer the data from the gsse domain to the viennagrid domain
//
  typedef viennagrid::mesh_t   MeshType;
  typedef viennagrid::result_of::region<MeshType>::type  RegionType;
  typedef viennagrid::result_of::point<MeshType>::type PointType;
//    typedef DomainType::config_type                                                  DomainConfiguration;
  typedef viennagrid::result_of::element<MeshType>::type       ElementType;
//    typedef DomainConfiguration::cell_tag                                         CellTag;
//   typedef viennagrid::triangle_tag CellTag;
//   typedef viennagrid::result_of::element<MeshType, CellTag>::type     CellType;

  using namespace gsse::access_specifier;
  typedef gsse::result_of::at_dim<FullSpace, AC>::type                 SpaceTopologySegmentsT;
  typedef gsse::result_of::at_dim<FullSpace, AP>::type                 FBPosT;
  typedef gsse::result_of::at_dim_isd<FBPosT>::type                    CoordPosT;
  typedef gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type  CellComplex;
  typedef gsse::result_of::at_dim<CellComplex, AT_cl>::type            CellContainer;

  SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(fullspace);
  FBPosT&                 geometry          = gsse::at_dim<AP>(fullspace);

  //domain.reserve_vertices(gsse::size( geometry ));  //[KR] Not needed any longer

  for (long i =0 ; i < gsse::size( geometry ); ++i)
  {
    CoordPosT point = gsse::at(i)(geometry);


    viennagrid::make_vertex( vgrid_mesh, viennagrid::make_point( point[0], point[1], point[2]) );

/*      PointType & point = viennagrid::point( PointType,

    VertexType vertex;
    vertex.point()[0] = point[0];
    vertex.point()[1] = point[1];
    vertex.point()[2] = point[2];
    vertex.id(i);
    domain.push_back(vertex);   */
  }

//    domain.segments().resize( gsse::size(segments_topology) );

  std::size_t global_cell_count = 0;
  for (long si = 0; si < gsse::size(segments_topology); ++si)
  {
    CellContainer& cell_cont =
        gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)));

    global_cell_count += gsse::size(cell_cont);
  }

  //domain.reserve_cells(global_cell_count);    //[KR] Not needed any longer


//   typedef viennagrid::result_of::handle<MeshType, viennagrid::triangle_tag>::type TriangleHandleType;
  std::map<TriangleIndexType, ElementType> triangle_map;

  for (long si = 0; si < gsse::size(segments_topology); ++si)
  {
    CellContainer& cell_cont =
        gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology)));


    RegionType region = vgrid_mesh.get_make_region(geom.segment_id_map[si]);


    for( size_t ci = 0; ci < gsse::size( cell_cont ); ci++ )
    {
      TriangleIndexType index( cell_cont[ci][0], cell_cont[ci][1], cell_cont[ci][2] );
      std::map<TriangleIndexType, ElementType>::iterator tit = triangle_map.find(index);

      ElementType triangle;
      if (tit != triangle_map.end())
      {
        viennagrid::add( region, tit->second );
        ElementType triangle = tit->second;

//         *viennagrid::segment_element_info( region, viennagrid::dereference_handle(vgrid_mesh, triangle_handle) ) = (index.original_indices == tit->first.original_indices);
      }
      else
      {
        ElementType triangle = viennagrid::make_triangle( region,
                                  viennagrid::vertices(vgrid_mesh)[cell_cont[ci][0]],
                                  viennagrid::vertices(vgrid_mesh)[cell_cont[ci][1]],
                                  viennagrid::vertices(vgrid_mesh)[cell_cont[ci][2]]);
        triangle_map[index] = triangle;

//         *viennagrid::segment_element_info( segment, viennagrid::dereference_handle(vgrid_mesh, triangle_handle) ) = true;
      }
    }
  }

  // END conversion
  // #######################################################################################

#ifdef DEBUGALL
  std::cout << ".. leaving STLSurfaceMeshing .. " << std::endl;
#endif

  return MESHING3_OK;
}






void STLSurfaceMeshing1 (STLGeometry & geom,
			 class Mesh & mesh,
			 int retrynr)
{
  int i, j, k;
  double h;

#ifdef DEBUGALL
  std::cout << " .. in STLSurfaceMeshing1 .. " << std::endl;
#endif

  h = mparam.maxh;

  mesh.FindOpenSegments();

  ARRAY<int> spiralps(0);
  spiralps.SetSize(0);
  for (i = 1; i <= geom.GetNP(); i++)
    {
      if (geom.GetSpiralPoint(i)) {spiralps.Append(i);}
    }

#ifdef DEBUGALL
  PrintMessage(7,"NO spiralpoints = ", spiralps.Size());
#endif

  int spfound;
  int sppointnum;
  int spcnt = 0;

  ARRAY<int> meshsp(mesh.GetNP());
  for (i = 1; i <= mesh.GetNP(); i++)
    {
      meshsp.Elem(i) = 0;
      for (j = 1; j <= spiralps.Size(); j++)
	if (Dist2(geom.GetPoint(spiralps.Get(j)), mesh.Point(i)) < 1e-20)
	  meshsp.Elem(i) = spiralps.Get(j);
    }


  ARRAY<int> opensegsperface(mesh.GetNFD());
  for (i = 1; i <= mesh.GetNFD(); i++)
    opensegsperface.Elem(i) = 0;
  for (i = 1; i <= mesh.GetNOpenSegments(); i++)
    {

      int si = mesh.GetOpenSegment (i).si;
      if (si >= 1 && si <= mesh.GetNFD())
	{
	  opensegsperface.Elem(si)++;
	}
      else
	{
#ifdef DEBUGALL
	  cerr << "illegal face index: " << si << " mesh.GetNFD: " <<  mesh.GetNFD() << endl;
#endif
	}
    }

#ifdef DEBUGALL
  std::cout << ".. mesh.GetNFD: " << mesh.GetNFD() << std::endl;
#endif

  double starttime = GetTime ();
  int fnr = 1;

//   for(fnr = mesh.GetNFD(); fnr > 0; fnr--)
//   for (fnr = 1; fnr <= 12; fnr++)
  for (fnr = 1; fnr <= mesh.GetNFD(); fnr++)
    {
      // [FS] .. test
      //
#ifdef DEBUGALL
      std::string blabla;
      std::cout << "##############################################################################################################" << std::endl;
      std::cout << ".. wait for input in STLSurfaceMeshing1 .. ###################################################################" << std::endl;
      std::cerr << ".. meshing chart: " << fnr << std::endl;

#endif
//      std::string blabla;
//      if(fnr == 735 || fnr == 736 || fnr == 737)
//         cin >> blabla;

#ifdef DEBUGALL
      std::cerr << ".. meshing chart: " << fnr << std::endl;
#endif

      {

#ifdef DEBUGALL
         std::cout << " .. before opensegsperface.Get(fnr) " << std::endl;
	std::cout << ".. opensegsperface.Get(fnr): " <<  opensegsperface.Get(fnr) << std::endl;
#endif

	// [FS] .. what does opensegsperface do ? in our case always the same value
	//
	  if (opensegsperface.Get(fnr))
	    {
#ifdef DEBUGALL
	std::cout << "..meshing face: " << fnr << " .. opensegsperface.Get(fnr): " <<  opensegsperface.Get(fnr) << std::endl;
#endif

	if (multithread.terminate)
	  return;

#ifdef DEBUGALL
	PrintMessage(5,"Meshing surface ", fnr, "/", mesh.GetNFD());
#endif

	// [FS] .. MeshingSTLSurface : public Meshing2
	//
	MeshingSTLSurface meshing (geom);

	meshing.SetStartTime (starttime);

	// [FS][INFO] add points to meshing2 and check for spiral points
	//            if no spiral point is found the multipointgeomInfo variable is set to
	//
	for (i = 1; i <= mesh.GetNP(); i++)
	  {
	    /*
	      spfound = 0;
	      for (j = 1; j <= spiralps.Size(); j++)
	      {
	      if (Dist2(geom.GetPoint(spiralps.Get(j)),mesh.Point(i)) < 1e-20)
		{spfound =  1; sppointnum = spiralps.Get(j);}
		}
	    */
	    sppointnum = 0;
	    if (i <= meshsp.Size())
	      sppointnum = meshsp.Get(i);

	  //spfound = 0;
#ifdef DEBUGALL
//	    std::cout << "..sppointnum: " << sppointnum << std::endl;
#endif

	  if (sppointnum)
	    {
	      MultiPointGeomInfo mgi;

	      int ntrigs = geom.NOTrigsPerPoint(sppointnum);
	      spcnt++;

	      for (j = 0; j < ntrigs; j++)
		{
		  PointGeomInfo gi;
		  gi.trignum = geom.TrigPerPoint(sppointnum, j+1);
		  mgi.AddPointGeomInfo (gi);
		}

	      // Einfuegen von ConePoint: Point bekommt alle
	      // Dreiecke (werden dann intern kopiert)
	      // Ein Segment zum ConePoint muss vorhanden sein !!!

	      meshing.AddPoint (mesh.Point(i), i, &mgi);

	    }
	  else
	    {
	      meshing.AddPoint (mesh.Point(i), i);
	    }
	}


      for (i = 1; i <= mesh.GetNOpenSegments(); i++)
	{
	  const Segment & seg = mesh.GetOpenSegment (i);
	  // [FS] surface index must match the face descriptor nr
	  //
	  if (seg.si == fnr)
	    {
	      meshing.AddBoundaryElement (seg.p1, seg.p2, seg.geominfo[0], seg.geominfo[1]);

#ifdef DEBUGALL
	      std::cout << "..adding boundary el: p1: " << seg.p1 << " :: p2: " << seg.p2 << std::endl;
	      std::cout << ".. seg.geominfo[0]: " << seg.geominfo[0]
			<< ".. seg.geominfo[1]: " << seg.geominfo[1] << std::endl;
#endif
	    }
	}

#ifdef DEBUGALL
      PrintMessage(3,"start meshing, trialcnt = ", retrynr);
#endif

      // [FS][TEST] .. testing if the lfs of a face can be given
      //
//       if(fnr == 11)
//       h = 0.125;
//       else
//       h = 1000.0;

#ifdef DEBUGALL
      std::cout << ".. start meshing with h = " << h << std::endl;
#endif

      meshing.GenerateMesh (mesh, h, fnr);  // face index

#ifdef DEBUGALL
      std::cout << " .. after GenerateMesh in STLSurfaceMeshing1 .. " << std::endl;
#endif

      // [FS] .. test code
	  ARRAY<SurfaceElementIndex> bla;
	  mesh.GetSurfaceElementsOfFace(8, bla);

#ifdef DEBUGALL
	  std::cout << "number surface elements of face: " << bla.Size() << std::endl;
#endif
#ifdef DEBUGALL
	  for(int t=0; t< bla.Size(); t++)
	    {
	      Element2d sel = mesh[bla[t]];

	      std::cout << "original surface element: " << t << " .. " << sel << std::endl;
	      std::cout << "surface element size: " << sel.GetNP() << std::endl;

	      for(int u=0; u<sel.GetNP(); u++)
		{
		  std::cout << "surf el geominfo: " << sel.GeomInfoPi(u+1) << std::endl;
		}
	    }
#endif


      // [FS] .. test
      //
//       sleep(2);

#ifdef OPENGL
      extern void Render();
      Render();
#endif


      }

	} // if FS test
    }


  mesh.CalcSurfacesOfNode();

#ifdef DEBUGALL
  std::cout << " .. leaving STLSurfaceMeshing1 .. " << std::endl;
#endif
}



void STLSurfaceOptimization (STLGeometry & geom,
			     class Mesh & mesh,
			     MeshingParameters & mparam)
{
  PrintFnStart("optimize STL Surface");


  MeshOptimizeSTLSurface optmesh(geom);
  //

  int i, j;
  /*
  for (i = 1; i <= mparam.optsteps2d; i++)
    {
      EdgeSwapping (mesh, 1, 1);
      CombineImprove (mesh, 1);
      optmesh.ImproveMesh (mesh, 0, 10, 1, 1);
    }
  */

  optmesh.SetFaceIndex (0);
  optmesh.SetImproveEdges (0);
  optmesh.SetMetricWeight (mparam.elsizeweight);

#ifdef DEBUGALL
  PrintMessage(5,"optimize string = ", mparam.optimize2d, " elsizew = ", mparam.elsizeweight);
#endif

  for (i = 1; i <= mparam.optsteps2d; i++)
    for (j = 1; j <= strlen(mparam.optimize2d); j++)
      {
	if (multithread.terminate)
	  break;

	//(*testout) << "optimize, before, step = " << mparam.optimize2d[j-1] << mesh.Point (3679) << endl;

	mesh.CalcSurfacesOfNode();
	switch (mparam.optimize2d[j-1])
	  {
	  case 's':
	    {
	      optmesh.EdgeSwapping (mesh, 0);
	      break;
	    }
	  case 'S':
	    {
	      optmesh.EdgeSwapping (mesh, 1);
	      break;
	    }
	  case 'm':
	    {
	      optmesh.ImproveMesh(mesh);
	      break;
	    }
	  case 'c':
	    {
	      optmesh.CombineImprove (mesh);
	      break;
	    }
	  }
	//(*testout) << "optimize, after, step = " << mparam.optimize2d[j-1] << mesh.Point (3679) << endl;
      }

  geom.surfaceoptimized = 1;

  mesh.Compress();
  mesh.CalcSurfacesOfNode();


}



MeshingSTLSurface :: MeshingSTLSurface (STLGeometry & ageom)
  : Meshing2(ageom.GetBoundingBox()), geom(ageom)
{
  ;
}

void MeshingSTLSurface :: DefineTransformation (const Point3d & p1, const Point3d & p2,
						const PointGeomInfo * geominfo,
						const PointGeomInfo * geominfo2)
{
  transformationtrig = geominfo[0].trignum;

  geom.DefineTangentialPlane(p1, p2, transformationtrig);
}

void MeshingSTLSurface :: TransformToPlain (const Point3d & locpoint, const MultiPointGeomInfo & gi,
					    Point2d & plainpoint, double h, int & zone)
{
#ifdef DEBUGALL
//   std::cout << "..in MeshingSTLSurface :: TransformToPlain .. " << std::endl;
#endif

  int trigs[10000];
  int i;

  if (gi.GetNPGI() >= 9999)
    {
      PrintError("In Transform to plane: increase size of trigs!!!");
    }

  for (i = 1; i <= gi.GetNPGI(); i++)
    trigs[i-1] = gi.GetPGI(i).trignum;
  trigs[gi.GetNPGI()] = 0;

  //  int trig = gi.trignum;
  //   (*testout) << "locpoint = " << locpoint;

  Point<2> hp2d;
  geom.ToPlane (locpoint, trigs, hp2d, h, zone, 1);
  plainpoint = hp2d;

  //  geom.ToPlane (locpoint, NULL, plainpoint, h, zone, 1);
  /*
  (*testout) << " plainpoint = " << plainpoint
	     << " h = " << h
	     << endl;
  */
}

/*
int MeshingSTLSurface :: ComputeLineGeoInfo (const Point3d & p1, const Point3d & p2,
					      int & geoinfosize, void *& geoinfo)
{
  static int geomtrig[2] = { 0, 0 };

  Point3d hp;
  hp = p1;
  geomtrig[0] = geom.Project (hp);

  hp = p2;
  geomtrig[1] = geom.Project (hp);

  geoinfosize = sizeof (geomtrig);
  geoinfo = &geomtrig;

  if (geomtrig[0] == 0)
    {
      return 1;
    }
  return 0;
}
*/


int MeshingSTLSurface :: ComputePointGeomInfo (const Point3d & p, PointGeomInfo & gi)
{
  // compute triangle of point,
  // if non-unique: 0

  Point<3> hp = p;
  gi.trignum = geom.Project (hp);

  if (!gi.trignum)
    {
      return 1;
    }

  return 0;
}


int MeshingSTLSurface ::
ChooseChartPointGeomInfo (const MultiPointGeomInfo & mpgi,
			  PointGeomInfo & pgi)
{
  int i;

  for (i = 1; i <= mpgi.GetNPGI(); i++)
    if (geom.TrigIsInOC (mpgi.GetPGI(i).trignum, geom.meshchart))
      {
	pgi = mpgi.GetPGI(i);
	return 0;
      }
  /*
  for (i = 0; i < mpgi.cnt; i++)
    {
      //      (*testout) << "d" << endl;
      if (geom.TrigIsInOC (mpgi.mgi[i].trignum, geom.meshchart))
	{
	  pgi = mpgi.mgi[i];
	  return 0;
	}
    }
  */
  PrintMessage(7,"INFORM: no gi on chart");
  pgi.trignum = 1;
  return 1;
}



int MeshingSTLSurface ::
IsLineVertexOnChart (const Point3d & p1, const Point3d & p2,
		     int endpoint, const PointGeomInfo & gi)
{
  Vec3d baselinenormal = geom.meshtrignv;

  int lineendtrig = gi.trignum;


  return geom.TrigIsInOC (lineendtrig, geom.meshchart);

  //  Vec3d linenormal = geom.GetTriangleNormal (lineendtrig);
  //  return ( (baselinenormal * linenormal) > cos (30 * (M_PI/180)) );
}

void MeshingSTLSurface ::
GetChartBoundary (ARRAY<Point2d > & points,
		  ARRAY<Point3d > & points3d,
		  ARRAY<INDEX_2> & lines, double h) const
{
  points.SetSize (0);
  points3d.SetSize (0);
  lines.SetSize (0);
  geom.GetMeshChartBoundary (points, points3d, lines, h);
}




int MeshingSTLSurface :: TransformFromPlain (Point2d & plainpoint,
					     Point3d & locpoint,
					     PointGeomInfo & gi,
					     double h)
{
  //return 0, wenn alles OK
  Point<3> hp3d;
  int res = geom.FromPlane (plainpoint, hp3d, h);
  locpoint = hp3d;
  ComputePointGeomInfo (locpoint, gi);
  return res;
}


int MeshingSTLSurface ::
BelongsToActiveChart (const Point3d & p,
		      const PointGeomInfo & gi)
{
  return (geom.TrigIsInOC(gi.trignum, geom.meshchart) != 0);
}



double MeshingSTLSurface :: CalcLocalH (const Point3d & p, double gh) const
{
  return gh;
}

double MeshingSTLSurface :: Area () const
{
  return geom.Area();
}






MeshOptimizeSTLSurface :: MeshOptimizeSTLSurface (STLGeometry & ageom)
  : MeshOptimize2d(), geom(ageom)
{
  ;
}


void MeshOptimizeSTLSurface :: SelectSurfaceOfPoint (const Point<3> & p,
						     const PointGeomInfo & gi)
{
  //  (*testout) << "sel char: " << gi.trignum << endl;

  geom.SelectChartOfTriangle (gi.trignum);
  //  geom.SelectChartOfPoint (p);
}


void MeshOptimizeSTLSurface :: ProjectPoint (INDEX surfind, Point<3> & p) const
{
  if (!geom.Project (p))
    {
      PrintMessage(7,"project failed");

      if (!geom.ProjectOnWholeSurface(p))
	{
	  PrintMessage(7, "project on whole surface failed");
	}
    }

  //  geometry.GetSurface(surfind)->Project (p);
}

void MeshOptimizeSTLSurface :: ProjectPoint2 (INDEX surfind, INDEX surfind2, Point<3> & p) const
{
  /*
  ProjectToEdge ( geometry.GetSurface(surfind),
		  geometry.GetSurface(surfind2), p);
  */
}

int  MeshOptimizeSTLSurface :: CalcPointGeomInfo(PointGeomInfo& gi, const Point<3> & p3) const
{
  Point<3> hp = p3;
  gi.trignum = geom.Project (hp);

  if (gi.trignum)
    {
      return 1;
    }

  return 0;

}

void MeshOptimizeSTLSurface :: GetNormalVector(INDEX surfind, const Point<3> & p, Vec<3> & n) const
{
  n = geom.GetChartNormalVector();

  /*
  geometry.GetSurface(surfind)->CalcGradient (p, n);
  n /= n.Length();
  if (geometry.GetSurface(surfind)->Inverse())
    n *= -1;
  */
}










RefinementSTLGeometry :: RefinementSTLGeometry (const STLGeometry & ageom)
  : Refinement(), geom(ageom)
{
  ;
}

RefinementSTLGeometry :: ~RefinementSTLGeometry ()
{
  ;
}

void RefinementSTLGeometry ::
PointBetween  (const Point<3> & p1, const Point<3> & p2, double secpoint,
	       int surfi,
	       const PointGeomInfo & gi1,
	       const PointGeomInfo & gi2,
	       Point<3> & newp, PointGeomInfo & newgi)
{
  newp = p1+secpoint*(p2-p1);

  /*
  (*testout) << "surf-between: p1 = " << p1 << ", p2 = " << p2
	     << ", gi = " << gi1 << " - " << gi2 << endl;
  */

  if (gi1.trignum > 0)
    {
      //      ((STLGeometry&)geom).SelectChartOfTriangle (gi1.trignum);

      Point<3> np1 = newp;
      Point<3> np2 = newp;
      ((STLGeometry&)geom).SelectChartOfTriangle (gi1.trignum);
      int tn1 = geom.Project (np1);

      ((STLGeometry&)geom).SelectChartOfTriangle (gi2.trignum);
      int tn2 = geom.Project (np2);

      newgi.trignum = tn1; //urspruengliche version
      newp = np1;          //urspruengliche version

      if (!newgi.trignum)
	{ newgi.trignum = tn2; newp = np2; }
      if (!newgi.trignum) newgi.trignum = gi1.trignum;

      /*
      if (tn1 != 0 && tn2 != 0 && ((STLGeometry&)geom).GetAngle(tn1,tn2) < M_PI*0.05)	{
	  newgi.trignum = tn1;
	  newp = np1;
	}
      else
	{
	  newp = ((STLGeometry&)geom).PointBetween(p1, gi1.trignum, p2, gi2.trignum);
	  tn1 = ((STLGeometry&)geom).Project(newp);
	  newgi.trignum = tn1;

	  if (!tn1)
	    {
	      newp = Center (p1, p2);
	      newgi.trignum = 0;

	    }
	}
      */
    }
  else
    {
      //      (*testout) << "WARNING: PointBetween got geominfo = 0" << endl;
      newp =  p1+secpoint*(p2-p1);
      newgi.trignum = 0;
    }

  //  (*testout) << "newp = " << newp << ", ngi = " << newgi << endl;
}

void RefinementSTLGeometry ::
PointBetween (const Point<3> & p1, const Point<3> & p2, double secpoint,
	      int surfi1, int surfi2,
	      const EdgePointGeomInfo & gi1,
	      const EdgePointGeomInfo & gi2,
	      Point<3> & newp, EdgePointGeomInfo & newgi)
{
  /*
  (*testout) << "edge-between: p1 = " << p1 << ", p2 = " << p2
	     << ", gi1,2 = " << gi1 << ", " << gi2 << endl;
  */
  /*
  newp = Center (p1, p2);
  ((STLGeometry&)geom).SelectChartOfTriangle (gi1.trignum);
  newgi.trignum = geom.Project (newp);
  */
  int hi;
  newgi.dist = (1.0-secpoint) * gi1.dist + secpoint*gi2.dist;
  newgi.edgenr = gi1.edgenr;

  /*
  (*testout) << "p1 = " << p1 << ", p2 = " << p2 << endl;
  (*testout) << "refedge: " << gi1.edgenr
	     << " d1 = " << gi1.dist << ", d2 = " << gi2.dist << endl;
  */
  newp = geom.GetLine (gi1.edgenr)->GetPointInDist (geom.GetPoints(), newgi.dist, hi);

  //  (*testout) << "newp = " << newp << endl;
}


void RefinementSTLGeometry :: ProjectToSurface (Point<3> & p, int surfi)
{
  cout << "RefinementSTLGeometry :: ProjectToSurface not implemented!" << endl;
};


void RefinementSTLGeometry :: ProjectToSurface (Point<3> & p, int surfi,
						PointGeomInfo & gi)
{
  ((STLGeometry&)geom).SelectChartOfTriangle (gi.trignum);
  gi.trignum = geom.Project (p);
  //  if (!gi.trignum)
  //    cout << "projectSTL failed" << endl;
};


}
