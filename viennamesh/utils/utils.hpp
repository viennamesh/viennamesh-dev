#ifndef VIENNAMESH_UTILS_UTILS_HPP
#define VIENNAMESH_UTILS_UTILS_HPP


#include <boost/shared_ptr.hpp>

namespace viennamesh
{
  namespace utils
  {

    template<typename some_type>
    struct element_type;

    template<typename some_type>
    struct element_type< some_type * >
    { typedef some_type type; };

    template<typename some_type>
    struct element_type< some_type const * >
    { typedef const some_type type; };

    template<typename some_type>
    struct element_type< boost::shared_ptr<some_type> >
    { typedef some_type type; };

    template<typename some_type>
    struct element_type< boost::shared_ptr<const some_type> >
    { typedef const some_type type; };




    template<typename type>
    struct is_scalar
    { const static bool value = false; };


    template<>
    struct is_scalar<bool>
    { const static bool value = true; };

    template<>
    struct is_scalar<unsigned char>
    { const static bool value = true; };

    template<>
    struct is_scalar<char>
    { const static bool value = true; };

    template<>
    struct is_scalar<unsigned short>
    { const static bool value = true; };

    template<>
    struct is_scalar<short>
    { const static bool value = true; };

    template<>
    struct is_scalar<unsigned int>
    { const static bool value = true; };

    template<>
    struct is_scalar<int>
    { const static bool value = true; };

    template<>
    struct is_scalar<unsigned long>
    { const static bool value = true; };

    template<>
    struct is_scalar<long>
    { const static bool value = true; };

    template<>
    struct is_scalar<unsigned long long>
    { const static bool value = true; };

    template<>
    struct is_scalar<long long>
    { const static bool value = true; };

    template<>
    struct is_scalar<float>
    { const static bool value = true; };

    template<>
    struct is_scalar<double>
    { const static bool value = true; };

  }
}

#endif
