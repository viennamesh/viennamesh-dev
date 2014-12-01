#include "viennamesh/plugin.hpp"

namespace viennamesh
{
  class square : public plugin_algorithm
  {
  public:

    static std::string name() { return "square"; }

    bool run(viennamesh::algorithm_handle)
    {
      info(1) << "YAY!" << std::endl;

      data_handle<double> my_double = get_input<double>("value");
      info(1) << "From the algorithm: " << my_double() << std::endl;

      data_handle<double> squared = make_data<double>();
      squared() = my_double()*my_double();

      set_output( "value", squared );

      return true;
    }

  private:
  };
}



int viennamesh_plugin_init(viennamesh_context ctx_)
{
  viennamesh::context_handle context(ctx_);

  viennamesh::register_algorithm<viennamesh::square>(context);

  return VIENNAMESH_SUCCESS;
}

