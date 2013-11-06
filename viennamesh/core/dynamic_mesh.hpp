#ifndef VIENNAMESH_CORE_DYNAMIC_MESH_HPP
#define VIENNAMESH_CORE_DYNAMIC_MESH_HPP

#include <vector>
#include <fstream>
#include <boost/iterator/iterator_concepts.hpp>

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/tetgen_poly_reader.hpp"

#include "viennamesh/forwards.hpp"
#include "viennamesh/core/mesh.hpp"
#include "viennamesh/core/convert.hpp"






// namespace viennamesh
// {
//   class BaseMesh;
//
//   typedef shared_ptr<BaseMesh> MeshHandle;
//   typedef shared_ptr<const BaseMesh> ConstMeshHandle;
//
//
//   struct BaseConversionFunction
//   {
//     BaseConversionFunction(int function_depth_, bool output_viennagrid_) : function_depth(function_depth_), output_viennagrid(output_viennagrid_) {}
//     virtual ~BaseConversionFunction() {}
//
//     virtual void convert( shared_ptr<const BaseMesh> const &, shared_ptr<BaseMesh> const & ) = 0;
//     virtual shared_ptr<BaseMesh> convert_to( shared_ptr<const BaseMesh> const & ) = 0;
//
//     int function_depth;
//     bool output_viennagrid;
//   };
//
//   template<typename InputMeshT, typename OutputMeshT>
//   struct ConversionFunction : BaseConversionFunction
//   {
//     ConversionFunction() : BaseConversionFunction(1, OutputMeshT::staticIsViennaGrid()) {}
//     ConversionFunction( function<void (InputMeshT const &, OutputMeshT &)> const & f) :
//         BaseConversionFunction(1, OutputMeshT::static_is_viennagrid()), convert_function(f) {}
//
//     virtual void convert( shared_ptr<const BaseMesh> const & input_mesh, shared_ptr<BaseMesh> const & output_mesh )
//     {
// #ifdef DEBUG
//       convert_function( dynamic_cast<InputMeshT const &>(*input_mesh), dynamic_cast<OutputMeshT &>(*output_mesh) );
// #else
//       convert_function( static_cast<InputMeshT const &>(*input_mesh), static_cast<OutputMeshT &>(*output_mesh) );
// #endif
//     }
//
//     virtual shared_ptr<BaseMesh> convert_to( shared_ptr<const BaseMesh> const & input_mesh )
//     {
//       shared_ptr<OutputMeshT> result( new OutputMeshT );
//       convert(input_mesh, result);
//       return result;
//     }
//
//     function<void (InputMeshT const &, OutputMeshT &)> convert_function;
//   };
//
//   struct DualConversionFunction : BaseConversionFunction
//   {
//     DualConversionFunction( shared_ptr<BaseConversionFunction> const & first_, shared_ptr<BaseConversionFunction> const & second_ ) :
//         BaseConversionFunction(first_->function_depth + second_->function_depth, second_->output_viennagrid), first(first_), second(second_) {}
//
//     virtual void convert( shared_ptr<const BaseMesh> const & input_mesh, shared_ptr<BaseMesh> const & output_mesh )
//     {
//       second->convert( first->convert_to(input_mesh), output_mesh );
//     }
//
//     virtual shared_ptr<BaseMesh> convert_to( shared_ptr<const BaseMesh> const & input_mesh )
//     {
//       return second->convert_to( first->convert_to(input_mesh) );
//     }
//
//     shared_ptr<BaseConversionFunction> first;
//     shared_ptr<BaseConversionFunction> second;
//   };
//
//   template<typename BaseMeshT>
//   class Converter
//   {
//   public:
//
//     typedef std::map<type_info_wrapper, shared_ptr<BaseConversionFunction> > ConversionFunctionMapType;
//     typedef std::map<type_info_wrapper, ConversionFunctionMapType> ConversionFunctionMapMapType;
//
//
//     shared_ptr<BaseMeshT> convert_to_nearest_viennagrid(shared_ptr<const BaseMeshT> const & input_mesh)
//     {
//       ConversionFunctionMapMapType::iterator imtit = conversions.find(typeid(*input_mesh));
//       if (imtit != conversions.end())
//       {
//         ConversionFunctionMapType::iterator best = imtit->second.end();
//
//         for (ConversionFunctionMapType::iterator omtit = imtit->second.begin();
//             omtit != imtit->second.end();
//             ++omtit)
//         {
//           if (omtit->second->output_viennagrid)
//           {
//             if (best == imtit->second.end())
//               best = omtit;
//             else
//             {
//               if (best->second->function_depth > omtit->second->function_depth)
//                 best = omtit;
//             }
//           }
//         }
//
//         if (best != imtit->second.end())
//         {
//           return best->second->convert_to(input_mesh);
//         }
//         else
//         {
//           return shared_ptr<BaseMeshT>();
//         }
//       }
//
//       return shared_ptr<BaseMeshT>();
//     }
//
//     template<typename MeshT>
//     bool is_convertable_to( shared_ptr<const BaseMeshT> const & input_mesh )
//     {
//       MeshT::static_init();
//
//       ConversionFunctionMapMapType::iterator it = conversions.find(typeid(*input_mesh));
//       if (it != conversions.end())
//       {
//         ConversionFunctionMapType::iterator jt = it->second.find(typeid(MeshT));
//         if ( jt != it->second.end() )
//         {
//           return true;
//         }
//       }
//
//       return false;
//     }
//
//
//     template<typename MeshT>
//     shared_ptr<BaseMeshT> convert_to(shared_ptr<const BaseMeshT> const & input_mesh)
//     {
//       MeshT::static_init();
//
//       ConversionFunctionMapMapType::iterator it = conversions.find(typeid(*input_mesh));
//       if (it != conversions.end())
//       {
//         ConversionFunctionMapType::iterator jt = it->second.find(typeid(MeshT));
//         if ( jt != it->second.end() )
//         {
//           return jt->second->convert_to(input_mesh);
//         }
//       }
//
//       return shared_ptr<BaseMeshT>();
//     }
//
//
//     bool is_convertable( shared_ptr<const BaseMeshT> const & input_mesh, shared_ptr<const BaseMeshT> const & output_mesh )
//     {
//       ConversionFunctionMapMapType::iterator it = conversions.find(typeid(*input_mesh));
//       if (it != conversions.end())
//       {
//         ConversionFunctionMapType::iterator jt = it->second.find(typeid(*output_mesh));
//         if ( jt != it->second.end() )
//         {
//           return true;
//         }
//       }
//
//       return false;
//     }
//
//     bool operator() ( shared_ptr<const BaseMeshT> const & input_mesh, shared_ptr<BaseMeshT> const & output_mesh )
//     {
//       ConversionFunctionMapMapType::iterator it = conversions.find(typeid(*input_mesh));
//       if (it != conversions.end())
//       {
//         ConversionFunctionMapType::iterator jt = it->second.find(typeid(*output_mesh));
//         if ( jt != it->second.end() )
//         {
//           jt->second->convert(input_mesh, output_mesh);
//           return true;
//         }
//       }
//
//       return false;
//     }
//
//     template<typename InputMeshT, typename OutputMeshT>
//     void register_conversion( void (*fp)(InputMeshT const &, OutputMeshT &) )
//     {
//       type_info_wrapper input_type_id(typeid(InputMeshT));
//       type_info_wrapper output_type_id(typeid(OutputMeshT));
//
//       shared_ptr<BaseConversionFunction> current_conversion(new ConversionFunction<InputMeshT, OutputMeshT>(fp));
//       simple_register_conversion( input_type_id, output_type_id, current_conversion );
//
//       for (ConversionFunctionMapMapType::iterator imtit = conversions.begin();
//            imtit != conversions.end();
//            ++imtit)
//       {
//         ConversionFunctionMapType::iterator omtit = imtit->second.find(input_type_id);
//         if (omtit != imtit->second.end())
//         {
//           simple_register_conversion(imtit->first, output_type_id, shared_ptr<BaseConversionFunction>(new DualConversionFunction(omtit->second, current_conversion)));
//         }
//       }
//
//       ConversionFunctionMapMapType::iterator imtit = conversions.find(output_type_id);
//       if (imtit != conversions.end())
//       {
//         for (ConversionFunctionMapType::iterator omtit = imtit->second.begin();
//             omtit != imtit->second.end();
//             ++omtit)
//         {
//           simple_register_conversion(input_type_id, omtit->first, shared_ptr<BaseConversionFunction>(new DualConversionFunction(current_conversion, omtit->second)));
//         }
//       }
//     }
//
//   private:
//
//     void simple_register_conversion( type_info_wrapper const & input_type_id, type_info_wrapper const & output_type_id, shared_ptr<BaseConversionFunction> const & conversion )
//     {
//       shared_ptr<BaseConversionFunction> & entry = conversions[input_type_id][output_type_id];
//       if (!entry)
//         entry = conversion;
//       else
//       {
//         if (entry->function_depth >= conversion->function_depth)
//           entry = conversion;
//       }
//     }
//
//     ConversionFunctionMapMapType conversions;
//   };
//
//
//
//
//   class BaseMesh : public enable_shared_from_this<BaseMesh>
//   {
//   public:
//     virtual ~BaseMesh() {}
//
//     virtual bool read( string const & filename ) = 0;
//     virtual bool write( string const & filename ) = 0;
//
//     bool is_convertable(MeshHandle dest_mesh) const
//     { return converter().is_convertable( shared_from_this(), dest_mesh ); }
//
//     bool convert_to(MeshHandle dest_mesh) const
//     { return converter()( shared_from_this(), dest_mesh ); }
//
//     template<typename MeshT>
//     bool is_convertable(MeshHandle dest_mesh) const
//     { return converter().is_convertable_to<MeshT>( shared_from_this(), dest_mesh ); }
//
//
//     template<typename MeshT>
//     MeshHandle getConverted() const
//     { return converter().convert_to<MeshT>( shared_from_this() ); }
//
//     virtual bool is_viennagrid() const = 0;
//
//   protected:
//
//     static Converter<BaseMesh> & converter()
//     {
//       static Converter<BaseMesh> converter_;
//       return converter_;
//     }
//   };
//
//   template<typename InputMeshT, typename OutputMeshT>
//   void dynamic_convert( InputMeshT const & input, OutputMeshT & output )
//   {
//     viennamesh::convert( input.mesh, input.segmentation, output.mesh, output.segmentation );
//   }
//
//
//   template<typename MeshT, typename SegmentationT>
//   struct mesh_converter;
//
//
//
//   template<typename MeshT, typename SegmentationT>
//   class MeshWrapper : public BaseMesh
//   {
//   public:
//     typedef MeshT mesh_type;
//     typedef SegmentationT segmentation_type;
//
//     MeshWrapper() { static_init(); }
//     MeshWrapper(mesh_type const & mesh_, segmentation_type const & segmentation_) :
//       mesh (mesh_), segmentation(segmentation_) { static_init(); }
//
//     bool read( string const & filename )
//     {
//       MeshHandle vgrid_mesh = BaseMesh::converter().convert_to_nearest_viennagrid( shared_from_this() );
//       if (vgrid_mesh)
//       {
//         vgrid_mesh->read(filename);
//         return true;
//       }
//       else
//         return false;
//     }
//
//     bool write( string const & filename )
//     {
//       MeshHandle vgrid_mesh = BaseMesh::converter().convert_to_nearest_viennagrid( shared_from_this() );
//       if (vgrid_mesh)
//       {
//         vgrid_mesh->write(filename);
//         return true;
//       }
//       else
//         return false;
//     }
//
//     bool is_viennagrid() const { return static_is_viennagrid(); }
//     static bool static_is_viennagrid() { return false; }
//
//     static void static_init()
//     {
//       static bool to_init = true;
//       if (to_init)
//       {
//         mesh_converter<mesh_type, segmentation_type>::register_functions(BaseMesh::converter());
//
//         to_init = false;
//       }
//     }
//
//     mesh_type mesh;
//     segmentation_type segmentation;
//   };
//
//
//   template<typename MeshT>
//   struct TetgenPolyReader
//   {
//     static bool read( MeshT & mesh, string const & filename ) { return false; }
//   };
//
//   template<>
//   struct TetgenPolyReader<viennagrid::plc_2d_mesh>
//   {
//     static bool read( viennagrid::plc_2d_mesh & mesh, string const & filename )
//     {
//       viennagrid::io::tetgen_poly_reader reader;
//       reader(mesh, filename);
//       return true;
//     }
//   };
//
//   template<>
//   struct TetgenPolyReader<viennagrid::plc_3d_mesh>
//   {
//     static bool read( viennagrid::plc_3d_mesh & mesh, string const & filename )
//     {
//       viennagrid::io::tetgen_poly_reader reader;
//       reader(mesh, filename);
//       return true;
//     }
//   };
//
//
//   template<typename WrappedMeshConfig, typename WrappedSegmentationConfig>
//   class MeshWrapper<viennagrid::mesh<WrappedMeshConfig>, viennagrid::segmentation<WrappedSegmentationConfig> > : public BaseMesh
//   {
//   public:
//     typedef viennagrid::mesh<WrappedMeshConfig> mesh_type;
//     typedef viennagrid::segmentation<WrappedSegmentationConfig> segmentation_type;
//
//     MeshWrapper() : segmentation(mesh) { static_init(); }
//     MeshWrapper(mesh_type const & mesh_, segmentation_type const & segmentation_) :
//       mesh (mesh_), segmentation(segmentation_) { static_init(); }
//
//     bool read( string const & filename )
//     {
//       string extension = filename.substr( filename.rfind(".")+1 );
//
//       if (extension == "vtu" || extension == "pvd")
//       {
//         viennagrid::io::vtk_reader<mesh_type, segmentation_type> vtk_writer;
//         vtk_writer(mesh, segmentation, filename);
//         return true;
//       }
//       else if (extension == "mesh")
//       {
//         viennagrid::io::netgen_reader reader;
//         reader(mesh, segmentation, filename);
//         return true;
//       }
//       else if (extension == "poly")
//       {
//         return TetgenPolyReader<mesh_type>::read(mesh, filename);
//       }
//
//       return false;
//     }
//
//     bool write( string const & filename )
//     {
//       string extension = filename.substr( filename.rfind(".")+1 );
//
//       if (extension == "vtu" || extension == "pvd")
//       {
//         viennagrid::io::vtk_writer<mesh_type, segmentation_type> vtk_writer;
//         vtk_writer( mesh, segmentation, filename.substr(0, filename.rfind(".")) );
//         return true;
//       }
//       else
//       {
//         return false;
//       }
//     }
//
//     bool is_viennagrid() const { return static_is_viennagrid(); }
//     static bool static_is_viennagrid() { return true; }
//
//     static void static_init()
//     {
//       static bool to_init = true;
//       if (to_init)
//       {
//         to_init = false;
//       }
//     }
//
//     mesh_type mesh;
//     segmentation_type segmentation;
//   };
//
//
//
//
//   MeshHandle readFile( string const & filename )
//   {
//     string extension = filename.substr( filename.rfind(".")+1 );
//
//     MeshHandle result;
//
//     if (extension == "mesh")
//       result = MeshHandle( new MeshWrapper<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> );
//
//     if (extension == "vtu" || extension == "pvd")
//     {
//       std::ifstream file(filename.c_str());
//       if (file)
//       {
//         int geometric_dimension;
//         int topologic_dimension;
//         int num_cells;
//
//         string line;
//         while (std::getline(file, line))
//         {
//           if ( line.find("<Piece") != string::npos )
//           {
//             string::size_type pos1 = line.find("NumberOfCells") + 15;
//             string::size_type pos2 = line.find("\"", pos1+1);
//             num_cells = atoi(line.substr(pos1, pos2-pos1).c_str());
//             break;
//           }
//         }
//
//         while (std::getline(file, line))
//         {
//           if ( line.find("<Points>") != string::npos )
//           {
//             while (std::getline(file, line))
//             {
//               if ( line.find("<DataArray") != string::npos )
//               {
//                 string::size_type pos1 = line.find("NumberOfComponents") + 20;
//                 string::size_type pos2 = line.find("\"", pos1+1);
//                 geometric_dimension = atoi(line.substr(pos1, pos2-pos1).c_str());
//                 break;
//               }
//             }
//             break;
//           }
//         }
//
//         while (std::getline(file, line))
//         {
//           if ( line.find("<Cells>") != string::npos )
//           {
//             while (std::getline(file, line))
//             {
//               if ( line.find("<DataArray") != string::npos )
//               {
//                 int counter = -1;
//                 while (file)
//                 {
//                   double bla;
//                   file >> bla;
//                   ++counter;
//                 }
//
//                 topologic_dimension = counter / num_cells - 1;
//               }
//             }
//             break;
//           }
//         }
//
//         if ( (geometric_dimension == 3) && (topologic_dimension == 2) )
//           result = MeshHandle( new MeshWrapper<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> );
//         else if ( (geometric_dimension == 3) && (topologic_dimension == 2) )
//           result = MeshHandle( new MeshWrapper<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> );
//       }
//     }
//
//     if (result)
//       result->read(filename);
//
//     return result;
//   }
// }





#endif
