#ifndef VIENNAMESH_ALGORITHM_IO_MESH_WRITER_HPP
#define VIENNAMESH_ALGORITHM_IO_MESH_WRITER_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/io/common.hpp"

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


      template<typename WriterProxyT>
      bool write_all( const_parameter_handle const & mesh, string const & filename,
                      int geometric_dimension, string cell_type, bool is_segmented );

      bool write_mphtxt( const_parameter_handle const & mesh, string const & filename,
                         int geometric_dimension, string cell_type );


      template<typename WriterProxyT, typename TagT, int DimensionV>
      bool generic_write( const_parameter_handle const & mesh, string const & filename, bool is_segmented );

      template<typename WriterProxyT, typename TagT, int DimensionV>
      bool generic_nonsegmented_write( const_parameter_handle const & mesh, string const & filename );

      template<typename WriterProxyT, typename TagT, int DimensionV>
      bool generic_segmented_write( const_parameter_handle const & mesh, string const & filename );


    };

  }

}



#endif
