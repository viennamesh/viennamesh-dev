/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                ViennaUtils - The Vienna Utilities Library
                             -----------------

   authors:    Josef Weinbub                    weinbub@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAUTILS_IO_BND
#define VIENNAUTILS_IO_BND


#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>

#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/predicate.hpp"

#include "viennautils/convert.hpp"
#include "viennautils/contio.hpp"

//#define IODEBUG

namespace viennautils
{
  namespace io
  {
    struct bnd_reader
    {

      static const int DIMG = 3;


      typedef double                            numeric_type;
      typedef boost::array<numeric_type, DIMG>  point_type;
      typedef std::vector<point_type>           geometry_container_type;
    
      typedef long                              index_type;

      typedef boost::array<std::size_t, 2>      edge_type;
      typedef std::vector<edge_type>            edge_container_type;
    
      
      typedef std::vector< index_type >         polygon_type;
      typedef std::vector< polygon_type >       polygon_container_type;

      typedef std::vector<polygon_container_type>   domain_type;
    
    
      int operator()(std::string const & filename) 
      {
        std::ifstream reader(filename.c_str());

        if (!reader){
          std::cerr << "Cannot open file " << filename << std::endl;
          throw "Cannot open file";
          return EXIT_FAILURE;
        }

        try{
        
          std::string token;
            
          std::size_t vertices = 0;
        
        #ifdef IODEBUG
          std::cout << "reading geometry information .. " << std::endl;        
        #endif                  
        
          while(1)
          {
            std::getline(reader, token);
            
            if(token.find("Vertices") != std::string::npos)
            {
              std::string::size_type delimiter_left = token.find("(")+1;
              std::string::size_type delimiter_right = token.find(")");              
              std::string vertices_str = token.substr(delimiter_left, delimiter_right-delimiter_left);
              vertices = viennautils::convert<std::size_t>()(vertices_str);
              break;
            }
          }            
          
        #ifdef IODEBUG
          std::cout << "vertices: " << vertices << std::endl;
        #endif

          geometry.resize(vertices);

          for(std::size_t vi = 0; vi < vertices; vi++)
          {
            point_type coords;

            for (int j=0; j < DIMG; j++)
            {
              reader >> coords[j];
            }
            geometry[vi] = coords;
          }
          
        #ifdef IODEBUG
          std::cout << "  finished loading point vectors .. " <<  std::endl;
        #endif          
          
        #ifdef IODEBUG
          std::cout << "reading topology information .. " << std::endl;        
        #endif                            
          
          
        #ifdef IODEBUG
          std::cout << "reading edge information .. " << std::endl;        
        #endif                  
        
          std::size_t edges = 0;        
        
          while(1)
          {
            std::getline(reader, token);
            
            if(token.find("Edges") != std::string::npos)
            {
              std::string::size_type delimiter_left = token.find("(")+1;
              std::string::size_type delimiter_right = token.find(")");              
              std::string edges_str = token.substr(delimiter_left, delimiter_right-delimiter_left);
              edges = viennautils::convert<std::size_t>()(edges_str);
              break;
            }
          }            
          
        #ifdef IODEBUG
          std::cout << "edges: " << edges << std::endl;
        #endif          
          
          edge_cont.resize(edges);

          for(std::size_t i = 0; i < edges; i++)
          {
            edge_type edge;
            reader >> edge[0];
            reader >> edge[1];            
            edge_cont[i] = edge;
          }
          
        #ifdef IODEBUG 
          std::cout << "  finished loading edges .. " <<  std::endl;
        #endif                    
          
          std::size_t faces = 0;                    
          
          while(1)
          {
            std::getline(reader, token);
            
            if(token.find("Faces") != std::string::npos)
            {
              std::string::size_type delimiter_left = token.find("(")+1;
              std::string::size_type delimiter_right = token.find(")");              
              std::string faces_str = token.substr(delimiter_left, delimiter_right-delimiter_left);
              faces = viennautils::convert<std::size_t>()(faces_str);
              break;
            }
          }

        #ifdef IODEBUG
          std::cout << "faces: " << faces << std::endl;
        #endif

          polygon_cont.resize(faces);

          std::size_t face_dim = 0;
          
          for(std::size_t fi = 0; fi < faces; fi++)
          {
            reader >> face_dim; // number of vertices of this face

            //std::cout << "face dim: " << face_dim << std::endl;

            polygon_cont[fi].resize(face_dim);

            for (std::size_t j=0; j < face_dim; j++)
            {
              reader >> polygon_cont[fi][j];
            }
          }
          
        #ifdef IODEBUG
          std::cout << "  finished loading faces .. " <<  std::endl;
        #endif                    
        
        #ifdef IODEBUG
          std::cout << "reading segment information .. " << std::endl;        
        #endif                                    
        
          std::size_t elements = 0;        
        
          while(1)
          {
            std::getline(reader, token);
            
            if(token.find("Elements") != std::string::npos)
            {
              std::string::size_type delimiter_left = token.find("(")+1;
              std::string::size_type delimiter_right = token.find(")");              
              std::string elements_str = token.substr(delimiter_left, delimiter_right-delimiter_left);
              elements = viennautils::convert<std::size_t>()(elements_str);
              break;
            }
          }

        #ifdef IODEBUG
          std::cout << "elements: " << elements << std::endl;
        #endif        
      
          domain.resize(elements);
      
          for(std::size_t i = 0; i < elements; i++)
          {
            reader >> token; // element type, ie, 10 ... polyhedron
            if(token != "10")
            {
              std::cerr << "Error: Elements of type: " << token << " are not supported .. " << std::endl;
              return EXIT_FAILURE;
            }
            
            std::size_t element_faces = 0;
            reader >> element_faces;
            
          #ifdef IODEBUG
            std::cout << "  element: " << i << " - faces: " << element_faces << std::endl;
          #endif        
          
            for(std::size_t f = 0; f < element_faces; f++)
            {
              index_type face;
              reader >> face;
              index_type mapped_face = map(face);

              polygon_type polygon = polygon_cont[mapped_face];


               // NOTE: the following polygon alteraions based on the 
               // sign of the polygon index have been deactivated
               // --> we thought this is nec, but it seems that it is not.
//              if(face < 0)
//              {
//               //std::cout << "orig poly: " << polygon;
//                std::reverse(polygon.begin(),polygon.end());
//                //std::for_each(polygon.begin(), polygon.end(), negate());
//               //std::cout << "adapted poly: " << polygon;                
//                
//              }
              
              polygon_type polygon_new;
              std::map<index_type, bool>  uniquer;                            
              
              for(std::size_t vi = 0; vi < polygon.size(); vi++)
              {
                index_type vertex = polygon[vi];
                index_type mapped_vertex = map(vertex);

                edge_type edge = edge_cont[mapped_vertex];

                if(vertex < 0) std::reverse(edge.begin(),edge.end());

                if(!uniquer[edge[0]])
                {
                  polygon_new.push_back(edge[0]);
                  uniquer[edge[0]] = true;
                }
                if(!uniquer[edge[1]])
                {
                  polygon_new.push_back(edge[1]);
                  uniquer[edge[1]] = true;
                }
              }
              //std::cout << "adapt: " << polygon_new;              
              domain[i].push_back(polygon_new);


//              if(face < 0)
//              {
//               exit(0);
//               }
            }
          }      
      
        #ifdef IODEBUG
          std::cout << "  finished loading elements .. " <<  std::endl;
        #endif                    
      
        } catch (...) {
          std::cerr << "Problems while reading file " << filename << std::endl;
        }
        return EXIT_SUCCESS;
      } //operator()
      
      struct negate 
      {
        template<typename T>
        void operator()(T& x) { x*=-1; }
      };
      
      void dump(std::ostream& ostr = std::cout)
      {
        ostr << "## GEOMETRY" << std::endl;
        for(std::size_t gi = 0; gi < geometry.size(); gi++)
        {
          ostr << "  point - id: " << gi << " - data: "; 
          viennautils::print()(geometry[gi], ostr);
        }      
      
        ostr << "## TOPOLOGY" << std::endl;
        for(std::size_t si = 0; si < domain.size(); si++)
        {
          ostr << "  segment: " << si << std::endl;
          for(std::size_t fi = 0; fi < domain[si].size(); fi++)
          {
            ostr << "    face - id: " << fi << " - size: " << domain[si][fi].size() << " - eles: ";
            viennautils::print()(domain[si][fi], ostr);
          }
        }
      }
      
      point_type& point(std::size_t index)
      {
         return geometry[index];
      }
      
      std::size_t geometry_size()
      {
         return geometry.size();
      }
      
      std::size_t segment_size()
      {  
         return domain.size();
      }
      
      polygon_container_type& segment(std::size_t index)
      {
         return domain[index];
      }
      
    private:
      index_type map(index_type index)
      {
        if(index >= 0) return index;
        else           return -(index)-1;  // this is the HIN specific mapping for negativ face and element values
      }
      
      geometry_container_type   geometry;
      edge_container_type       edge_cont;
      polygon_container_type    polygon_cont;      
      domain_type               domain;
      
    }; //class bnd_reader

  
  } //namespace io
} //namespace viennagrid

#endif
