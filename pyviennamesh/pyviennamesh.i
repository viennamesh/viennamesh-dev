%module pyviennamesh

#define VIENNAMESH_USES_BOOST

%include <std_string.i>
%include <boost_shared_ptr.i>
%shared_ptr(viennamesh::base_algorithm)

%{
  #include <boost/shared_ptr.hpp>
  #include "../viennamesh/forwards.hpp"
  #include "../viennamesh/core/algorithm.hpp"
  #include "../viennamesh/core/algorithm_factory.hpp"

  using namespace viennamesh;
%}

%include "../viennamesh/core/algorithm.hpp"
%include "../viennamesh/core/algorithm_factory.hpp"
