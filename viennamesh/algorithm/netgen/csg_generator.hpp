#ifndef VIENNAMESH_ALGORITHM_NETGEN_CSG_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_NETGEN_CSG_GENERATOR_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/netgen/mesh.hpp"


namespace viennamesh
{
  namespace netgen
  {
    class csg_mesher : public base_algorithm
    {
    public:

      string name() const { return "Netgen 5.1 mesher"; }

      bool run_impl()
      {
        const_string_parameter_handle csg_string = get_required_input<string>("default");
        output_parameter_proxy<netgen::output_mesh> output_mesh = output_proxy<netgen::output_mesh>("default");

        std::cout << csg_string() << std::endl;

        std::istringstream csg_stream(csg_string());
        ::netgen::CSGeometry * geom = ::netgen::ParseCSG( csg_stream );

        geom->FindIdenticSurfaces(1e-8);// * geom->MaxSize());
        ::netgen::MeshingParameters mparams;

        geom->GenerateMesh(output_mesh().mesh, mparams, 1, 5);

        delete geom;

        return true;
      }

    private:
    };
  }

}



#endif
