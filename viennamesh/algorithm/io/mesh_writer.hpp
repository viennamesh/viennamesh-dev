#ifndef VIENNAMESH_ALGORITHM_IO_MESH_WRITER_HPP
#define VIENNAMESH_ALGORITHM_IO_MESH_WRITER_HPP

#include "viennamesh/core/algorithm.hpp"


namespace viennamesh
{
  namespace io
  {

    class mesh_writer : public base_algorithm
    {
    public:

      string name() const { return "ViennaGrid Mesh Writer"; }
      bool run_impl();

    private:
      template<typename MeshT, typename SegmentationT>
      void write_vmesh( MeshT const & mesh,  SegmentationT const & segmentation, string const & filename );

      template<typename MeshT, typename ParameterHandleT>
      bool write( ParameterHandleT const & mesh, string const & filename );

      template<typename MeshT, typename SegmentationT, typename ParameterHandleT>
      bool write( ParameterHandleT const & mesh, string const & filename );

      template<typename TagT, int DimensionV>
      bool generic_run_nonsegmented( const_parameter_handle const & mesh, string const & filename );

      template<typename TagT, int DimensionV>
      bool generic_run_segmented( const_parameter_handle const & mesh, string const & filename );

      template<typename TagT, int DimensionV>
      bool generic_run( const_parameter_handle const & mesh, string const & filename );
    };

  }

}



#endif
