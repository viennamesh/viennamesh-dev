#ifndef _VIENNAMESH_DATA_HPP_
#define _VIENNAMESH_DATA_HPP_

#include <cassert>
#include "viennamesh/forwards.hpp"
#include "viennamesh/basic_data.hpp"
#include "viennagrid/core.hpp"

namespace viennamesh
{
  class abstract_data_handle
  {
    friend class context_handle;
    friend class algorithm_handle;

  protected:

    abstract_data_handle() : data(0) {}
    abstract_data_handle(viennamesh_data_wrapper data_, bool retain_ = true) : data(data_)
    {
      if (retain_)
        retain();
    }

  public:

    abstract_data_handle(abstract_data_handle const & handle_) : data(handle_.data) { retain(); }

    ~abstract_data_handle()
    {
      release();
    }

    abstract_data_handle & operator=(abstract_data_handle const & handle_)
    {
      release();
      data = handle_.data;
      retain();
      return *this;
    }

    bool valid() const { return data != 0; }
    operator bool() const { return valid(); }

    viennamesh_data_wrapper internal() const { return const_cast<viennamesh_data_wrapper>(data); }

  protected:

    void retain()
    {
      if (data)
        viennamesh_data_wrapper_retain(data);
    }

    void release()
    {
      if (data)
        viennamesh_data_wrapper_release(data);
    }

    viennamesh_data_wrapper data;
  };

  inline bool operator==(abstract_data_handle const & lhs, abstract_data_handle const & rhs)
  { return lhs.internal() == rhs.internal(); }

  inline bool operator!=(abstract_data_handle const & lhs, abstract_data_handle const & rhs)
  { return !(lhs==rhs); }




  template<typename DataT>
  class data_handle : public abstract_data_handle
  {
    template<typename FromT, typename ToT>
    friend void convert(data_handle<FromT> const & from, data_handle<ToT> const & to);

    friend class context_handle;
    friend class algorithm_handle;

    data_handle() : internal_data(0) {}
    data_handle(viennamesh_data_wrapper data_, bool retain_ = true) : abstract_data_handle(data_, retain_), internal_data(0)
    {
      if (data)
        viennamesh_data_wrapper_internal_get(data, (viennamesh_data*)&internal_data);
    }

  public:

    data_handle(data_handle<DataT> const & handle_) : abstract_data_handle(handle_.data), internal_data(handle_.internal_data) {}

    data_handle & operator=(data_handle<DataT> const & handle_)
    {
      static_cast<abstract_data_handle &>(*this) = static_cast<abstract_data_handle const &>(handle_);
      internal_data = handle_.internal_data;
      return *this;
    }

    void use(DataT & data_reference)
    {
      if (data)
        viennamesh_data_wrapper_internal_set(data, (viennamesh_data)&data_reference);

      internal_data = &data_reference;
    }



    DataT const & operator()() const
    {
      assert(internal_data);
      return *internal_data;
    }

    DataT & operator()()
    {
      assert(internal_data);
      return *internal_data;
    }

    void set(DataT const & data)
    {
      *internal_data = data;
    }

  private:

    int make(viennamesh_context context,
             std::string const & binary_format)
    {
      release();
      int result = viennamesh_data_wrapper_make(context,
                                  result_of::data_information<DataT>::type_name().c_str(),
                                  binary_format.c_str(),
                                  &data );

      if (result != VIENNAMESH_SUCCESS)
        return result;

      return viennamesh_data_wrapper_internal_get(data, (viennamesh_data*)&internal_data);
    }

    int make(viennamesh_context context)
    {
      return make( context, result_of::data_information<DataT>::local_binary_format() );
    }

    DataT * internal_data;
  };





  template<>
  class data_handle<viennagrid_mesh> : public abstract_data_handle
  {
    template<typename FromT, typename ToT>
    friend void convert(data_handle<FromT> const & from, data_handle<ToT> const & to);

    friend class context_handle;
    friend class algorithm_handle;

    data_handle() : internal_data(0) {}
    data_handle(viennamesh_data_wrapper data_, bool retain_ = true) : abstract_data_handle(data_, retain_), internal_data(0)
    {
      if (data)
        viennamesh_data_wrapper_internal_get(data, (viennamesh_data*)&internal_data);
    }

  public:

    data_handle(data_handle<viennagrid_mesh> const & handle_) : abstract_data_handle(handle_.data), internal_data(handle_.internal_data) {}

    data_handle & operator=(data_handle<viennagrid_mesh> const & handle_)
    {
      static_cast<abstract_data_handle &>(*this) = static_cast<abstract_data_handle const &>(handle_);
      internal_data = handle_.internal_data;
      return *this;
    }

    void use(viennagrid_mesh & data_reference)
    {
      if (data)
        viennamesh_data_wrapper_internal_set(data, (viennamesh_data)&data_reference);

      internal_data = &data_reference;
    }

    viennagrid::mesh_t operator()() const
    {
      assert(internal_data);
      return viennagrid::mesh_t(*internal_data);
    }

    void set(viennagrid::mesh_t const & mesh)
    {
      viennagrid_mesh_hierarchy_release( (*this)().internal_mesh_hierarchy() );
      *internal_data = mesh.internal();
      viennagrid_mesh_hierarchy_retain( mesh.internal_mesh_hierarchy() );
    }

  private:

    int make(viennamesh_context context,
             std::string const & binary_format)
    {
      release();
      int result = viennamesh_data_wrapper_make(context,
                                  result_of::data_information<viennagrid_mesh>::type_name().c_str(),
                                  binary_format.c_str(),
                                  &data );

      if (result != VIENNAMESH_SUCCESS)
        return result;

      return viennamesh_data_wrapper_internal_get(data, (viennamesh_data*)&internal_data);
    }

    int make(viennamesh_context context)
    {
      return make( context, result_of::data_information<viennamesh_string>::local_binary_format() );
    }

    viennagrid_mesh * internal_data;
  };






  template<>
  class data_handle<viennagrid_quantity_field> : public abstract_data_handle
  {
    template<typename FromT, typename ToT>
    friend void convert(data_handle<FromT> const & from, data_handle<ToT> const & to);

    friend class context_handle;
    friend class algorithm_handle;

    data_handle() : internal_data(0) {}
    data_handle(viennamesh_data_wrapper data_, bool retain_ = true) : abstract_data_handle(data_, retain_), internal_data(0)
    {
      if (data)
        viennamesh_data_wrapper_internal_get(data, (viennamesh_data*)&internal_data);
    }

  public:

    data_handle(data_handle<viennagrid_quantity_field> const & handle_) : abstract_data_handle(handle_.data), internal_data(handle_.internal_data) {}

    data_handle & operator=(data_handle<viennagrid_quantity_field> const & handle_)
    {
      release();
      data = handle_.data;
      internal_data = handle_.internal_data;
      retain();
      return *this;
    }

    void use(viennagrid_quantity_field & data_reference)
    {
      if (data)
        viennamesh_data_wrapper_internal_set(data, (viennamesh_data)&data_reference);

      internal_data = &data_reference;
    }


    viennagrid::quantity_field operator()() const
    {
      assert(internal_data);
      return viennagrid::quantity_field(*internal_data);
    }

    void set(viennagrid::quantity_field const & quantity_field)
    {
      viennagrid_quantity_field_release( (*this)().internal() );
      *internal_data = quantity_field.internal();
      viennagrid_quantity_field_retain( quantity_field.internal() );
    }

  private:

    int make(viennamesh_context context,
             std::string const & binary_format)
    {
      release();
      int result = viennamesh_data_wrapper_make(context,
                                  result_of::data_information<viennagrid_quantity_field>::type_name().c_str(),
                                  binary_format.c_str(),
                                  &data );

      if (result != VIENNAMESH_SUCCESS)
        return result;

      return viennamesh_data_wrapper_internal_get(data, (viennamesh_data*)&internal_data);
    }

    int make(viennamesh_context context)
    {
      return make( context, result_of::data_information<viennagrid_quantity_field>::local_binary_format() );
    }

    viennagrid_quantity_field * internal_data;
  };






  template<>
  class data_handle<viennamesh_string> : public abstract_data_handle
  {
    template<typename FromT, typename ToT>
    friend void convert(data_handle<FromT> const & from, data_handle<ToT> const & to);

    friend class context_handle;
    friend class algorithm_handle;

    data_handle() : internal_data(0) {}
    data_handle(viennamesh_data_wrapper data_, bool retain_ = true) : abstract_data_handle(data_, retain_), internal_data(0)
    {
      if (data)
        viennamesh_data_wrapper_internal_get(data, (viennamesh_data*)&internal_data);
    }

  public:

    data_handle(data_handle<viennamesh_string> const & handle_) : abstract_data_handle(handle_.data), internal_data(handle_.internal_data) {}

    data_handle & operator=(data_handle<viennamesh_string> const & handle_)
    {
      release();
      data = handle_.data;
      internal_data = handle_.internal_data;
      retain();
      return *this;
    }

    void use(viennamesh_string & data_reference)
    {
      if (data)
        viennamesh_data_wrapper_internal_set(data, (viennamesh_data)&data_reference);

      internal_data = &data_reference;
    }



    const std::string operator()() const
    {
      assert(internal_data);
      const char * tmp;
      viennamesh_string_get( *internal_data, &tmp );
      return tmp;
    }

    void set(std::string const & str)
    {
      viennamesh_string_set( *internal_data, str.c_str() );
    }

  private:

    int make(viennamesh_context context,
             std::string const & binary_format)
    {
      release();
      int result = viennamesh_data_wrapper_make(context,
                                  result_of::data_information<viennamesh_string>::type_name().c_str(),
                                  binary_format.c_str(),
                                  &data );

      if (result != VIENNAMESH_SUCCESS)
        return result;

      return viennamesh_data_wrapper_internal_get(data, (viennamesh_data*)&internal_data);
    }

    int make(viennamesh_context context)
    {
      return make( context, result_of::data_information<viennamesh_string>::local_binary_format() );
    }

    viennamesh_string * internal_data;
  };




  template<typename FromT, typename ToT>
  void convert(data_handle<FromT> const & from, data_handle<ToT> const & to)
  {
    viennamesh_data_wrapper_convert( from.data, to.data );
  }


  namespace result_of
  {


    template<typename T>
    struct c_type
    {
      typedef T type;
    };

    template<>
    struct c_type<viennagrid::mesh_t>
    {
      typedef viennagrid_mesh type;
    };

    template<>
    struct c_type<viennagrid::quantity_field>
    {
      typedef viennagrid_quantity_field type;
    };




    template<typename T>
    struct cpp_type
    {
      typedef T type;
    };

    template<>
    struct cpp_type<viennagrid_mesh>
    {
      typedef viennagrid::mesh_t type;
    };

    template<>
    struct cpp_type<viennagrid_quantity_field>
    {
      typedef viennagrid::quantity_field type;
    };



    template<typename DataT>
    struct data_handle
    {
      typedef viennamesh::data_handle<
        typename result_of::c_type<DataT>::type
      > type;
    };

    template<typename DataT>
    struct data_handle< viennamesh::data_handle<DataT> >
    {
      typedef viennamesh::data_handle<
        typename result_of::c_type<DataT>::type
      > type;
    };

    template<typename DataT>
    struct is_data_handle
    {
      static const bool value = false;
    };

    template<>
    struct is_data_handle<abstract_data_handle>
    {
      static const bool value = true;
    };

    template<typename DataT>
    struct is_data_handle< data_handle<DataT> >
    {
      static const bool value = true;
    };
  }


}

#endif
