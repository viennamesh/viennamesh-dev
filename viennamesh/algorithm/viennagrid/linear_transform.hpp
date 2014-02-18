#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_LINEAR_TRANSFORM_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_LINEAR_TRANSFORM_HPP

#include "viennamesh/core/algorithm.hpp"



namespace viennamesh
{

  namespace linear_transform
  {

    template<typename InputMeshT, typename InputSegmentationT, typename OutputMeshT, typename OutputSegmentationT>
    void linear_transform( InputMeshT const & input_mesh, InputSegmentationT const & input_segmentation,
                           OutputMeshT & output_mesh, OutputSegmentationT & output_segmentation,
                           typename viennagrid::result_of::coord<InputMeshT>::type const * matrix )
    {
      typedef typename viennagrid::result_of::point<InputMeshT>::type InputPointType;
      typedef typename viennagrid::result_of::point<OutputMeshT>::type OutputPointType;

      typedef typename viennagrid::result_of::cell<InputMeshT>::type InputCellType;
      typedef typename viennagrid::result_of::cell_id<InputMeshT>::type InputCellIDType;

      typedef typename viennagrid::result_of::cell<OutputMeshT>::type OutputCellType;
      typedef typename viennagrid::result_of::cell_handle<OutputMeshT>::type OutputCellHandleType;

      typedef typename viennagrid::result_of::vertex_id<InputMeshT>::type InputVertexIDType;
      typedef typename viennagrid::result_of::vertex_handle<OutputMeshT>::type OutputVertexHandleType;

      std::map<InputVertexIDType, OutputVertexHandleType> vertex_map;

      typedef typename viennagrid::result_of::const_vertex_range<InputMeshT>::type ConstInputVertexRangeType;
      typedef typename viennagrid::result_of::iterator<ConstInputVertexRangeType>::type ConstInputVertexIteratorType;

      ConstInputVertexRangeType vertices(input_mesh);
      for (ConstInputVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      {
        InputPointType in_point = viennagrid::point(*vit);
        OutputPointType out_point;
        for (unsigned int row = 0; row != out_point.size(); ++row)
        {
          out_point[row] = 0;
          for (unsigned int column = 0; column != in_point.size(); ++column)
            out_point[row] += in_point[column] * matrix[ row*in_point.size() + column ];
        }

        vertex_map[ vit->id() ] = viennagrid::make_vertex( output_mesh, out_point );
      }

      std::map<InputCellIDType, OutputCellHandleType> cell_map;

      typedef typename viennagrid::result_of::segment_handle<InputSegmentationT>::type InputSegmentHandleType;
      typedef typename viennagrid::result_of::segment_handle<OutputSegmentationT>::type OutputSegmentHandleType;

      for (typename InputSegmentationT::const_iterator sit = input_segmentation.begin(); sit != input_segmentation.end(); ++sit)
      {
        OutputSegmentHandleType & output_segment = output_segmentation.get_make_segment( sit->id() );

        typedef typename viennagrid::result_of::const_cell_range<InputSegmentHandleType>::type ConstInputCellRangeType;
        typedef typename viennagrid::result_of::iterator<ConstInputCellRangeType>::type ConstInputCellIteratorType;

        ConstInputCellRangeType cells(*sit);
        for (ConstInputCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
        {
          typedef typename viennagrid::result_of::const_vertex_range<InputCellType>::type ConstVertexOnInputCellRangeType;
          typedef typename viennagrid::result_of::iterator<ConstVertexOnInputCellRangeType>::type ConstVertexOnInputCellIteratorType;

          typename std::map<InputCellIDType, OutputCellHandleType>::iterator cmit = cell_map.find( cit->id() );
          if (cmit != cell_map.end())
          {
            viennagrid::add( output_segment, viennagrid::dereference_handle(output_mesh, cmit->second) );
          }
          else
          {
            ConstVertexOnInputCellRangeType vertices_on_cell( *cit );
            std::vector<OutputVertexHandleType> vertex_handles;
            for (ConstVertexOnInputCellIteratorType vit = vertices_on_cell.begin(); vit != vertices_on_cell.end(); ++vit)
              vertex_handles.push_back( vertex_map[vit->id()] );

            cell_map[ cit->id() ] = viennagrid::make_cell( output_segment, vertex_handles.begin(), vertex_handles.end() );
          }
        }
      }
    }


    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "ViennaGrid Linear Transform"; }

      typedef std::vector<double> MatrixType;

      template<unsigned int GeometricDimensionV, typename InputMeshT, typename InputSegmentationT>
      void generic_run_impl( InputMeshT const & input_mesh, InputSegmentationT const & input_segmentation, MatrixType const & matrix )
      {
        typedef typename viennagrid::result_of::cell_tag<InputMeshT>::type CellTagType;
        typedef typename viennamesh::result_of::full_config<CellTagType, GeometricDimensionV>::type OutputConfigType;
        typedef viennagrid::mesh<OutputConfigType> OutputMeshType;
        typedef typename viennagrid::result_of::segmentation<OutputMeshType>::type OutputSegmentationType;

        typedef viennagrid::segmented_mesh<OutputMeshType, OutputSegmentationType> OutputSegmentedMeshType;

        output_parameter_proxy<OutputSegmentedMeshType> output_mesh = output_proxy<OutputSegmentedMeshType>( "default" );

        linear_transform( input_mesh, input_segmentation, output_mesh().mesh, output_mesh().segmentation, &matrix[0] );
      }

      template<typename MeshT, typename SegmentationT>
      bool generic_run( MatrixType const & matrix )
      {
        typedef typename viennagrid::result_of::point<MeshT>::type PointType;
        typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

        typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type input_mesh = get_input<SegmentedMeshType>("default");
        if (input_mesh)
        {
          unsigned int destination_dimension = matrix.size() / viennagrid::result_of::geometric_dimension<MeshT>::value;
          info(5) << "Matrix size is " << matrix.size() << std::endl;
          info(5) << "Geometric desination size is " << destination_dimension << std::endl;

          if (destination_dimension != viennagrid::result_of::geometric_dimension<MeshT>::value)
            return false;

          generic_run_impl< viennagrid::result_of::geometric_dimension<MeshT>::value >(input_mesh().mesh, input_mesh().segmentation, matrix);

//           if (destination_dimension == 1)
//             generic_run_impl<1>(input_mesh().mesh, input_mesh().segmentation, matrix);
//           else if (destination_dimension == 2)
//             generic_run_impl<2>(input_mesh().mesh, input_mesh().segmentation, matrix);
//           else if (destination_dimension == 3)
//             generic_run_impl<3>(input_mesh().mesh, input_mesh().segmentation, matrix);
//           else
//             return false;

          return true;
        }

        return false;
      }

      bool run_impl()
      {
        viennamesh::result_of::const_parameter_handle<MatrixType>::type matrix = get_required_input<MatrixType>( "matrix" );

        if (generic_run<viennagrid::line_2d_mesh, viennagrid::line_2d_segmentation>( matrix() ))
          return true;

        if (generic_run<viennagrid::line_3d_mesh, viennagrid::line_3d_segmentation>( matrix() ))
          return true;

        if (generic_run<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>( matrix() ))
          return true;

        if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>( matrix() ))
          return true;


        error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
        return false;
      }

    private:
    };

  }
}

#endif
