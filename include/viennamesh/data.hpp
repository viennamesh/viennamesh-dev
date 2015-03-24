#ifndef _VIENNAMESH_DATA_HPP_
#define _VIENNAMESH_DATA_HPP_

#include <cassert>
#include "viennamesh/forwards.hpp"
#include "viennamesh/common.hpp"
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

    bool valid() const
    {
      return data != NULL && !empty();
    }
//     operator bool() const { return valid(); }

    bool empty() const { return size() == 0; }

    int size() const
    {
      int size_;
      viennamesh_data_wrapper_get_size(data, &size_);
      return size_;
    }

    void resize(int size_)
    {
      viennamesh_data_wrapper_resize(data, size_);
    }

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





  template<typename CT>
  typename result_of::cpp_result_type<CT>::type const & to_cpp(CT & src)
  {
    return src;
  }

  template<typename CPPT, typename CT>
  void to_c(CPPT const & src, CT & dst)
  {
    dst = src;
  }



  // viennagrid::mesh_t
  inline viennagrid::mesh_t to_cpp(viennagrid_mesh & src)
  {
    return viennagrid::mesh_t(src);
  }

  inline void to_c(viennagrid::mesh_t const & src, viennagrid_mesh & dst)
  {
    viennagrid_mesh_hierarchy mh;
    viennagrid_mesh_get_mesh_hierarchy(dst, &mh);
    viennagrid_mesh_hierarchy_release(mh);

    dst = src.internal();

    viennagrid_mesh_get_mesh_hierarchy(dst, &mh);
    viennagrid_mesh_hierarchy_retain(mh);
  }


  // viennagrid::quantity_field
  inline viennagrid::quantity_field to_cpp(viennagrid_quantity_field & src)
  {
    return viennagrid::quantity_field(src);
  }

  inline void to_c(viennagrid::quantity_field const & src, viennagrid_quantity_field & dst)
  {
    viennagrid_quantity_field_release( dst );
    dst = src.internal();
    viennagrid_quantity_field_retain( dst );
  }


  // viennagrid::point_t
  inline viennagrid::point_t to_cpp(viennamesh_point & src)
  {
    double * values;
    int size;
    viennamesh_point_get(src, &values, &size);
    viennagrid::point_t result(size);
    std::copy(values, values+size, &result[0]);
    return result;
  }

  inline void to_c(viennagrid::point_t const & src, viennamesh_point & dst)
  {
    viennamesh_point_delete(dst);
    viennamesh_point_make(&dst);
    viennamesh_point_set(dst, const_cast<double*>(&src[0]), src.size());
  }


  // viennagrid::seed_point_t
  inline seed_point_t to_cpp(viennamesh_seed_point & src)
  {
    double * values;
    int size;
    int region;
    viennamesh_seed_point_get(src, &values, &size, &region);
    viennagrid::point_t result(size);
    std::copy(values, values+size, &result[0]);
    return std::make_pair(result, region);
  }

  inline void to_c(seed_point_t const & src, viennamesh_seed_point & dst)
  {
    viennamesh_seed_point_delete(dst);
    viennamesh_seed_point_make(&dst);
    viennamesh_seed_point_set(dst, const_cast<double*>(&src.first[0]), src.first.size(), src.second);
  }


  // std::string
  inline std::string to_cpp(viennamesh_string & src)
  {
    const char * tmp;
    viennamesh_string_get( src, &tmp );
    return tmp;
  }

  inline void to_c(std::string const & src, viennamesh_string dst)
  {
    viennamesh_string_set( dst, src.c_str() );
  }





  template<typename DataT>
  class data_handle : public abstract_data_handle
  {
    template<typename FromT, typename ToT>
    friend void convert(data_handle<FromT> const & from, data_handle<ToT> & to);

    friend class context_handle;
    friend class algorithm_handle;

    data_handle() {}
    data_handle(viennamesh_data_wrapper data_, bool retain_ = true) : abstract_data_handle(data_, retain_) {}

  public:

    data_handle(data_handle<DataT> const & handle_) : abstract_data_handle(handle_.data) {}

    data_handle & operator=(data_handle<DataT> const & handle_)
    {
      static_cast<abstract_data_handle &>(*this) = static_cast<abstract_data_handle const &>(handle_);
      return *this;
    }

    typedef typename result_of::cpp_type<DataT>::type CPPType;
    typedef typename result_of::cpp_result_type<DataT>::type CPPResultType;


    CPPResultType operator()(int position) const
    {
      return to_cpp(*get_ptr(position));
    }
    CPPResultType operator()() const { return (*this)(0); }

    std::vector<CPPType> get_vector() const
    {
      std::vector<CPPType> result;
      for (int i = 0; i != size(); ++i)
        result.push_back( (*this)(i) );
      return result;
    }

    void set(int position, CPPType const & data_in)
    {
      viennamesh_data_wrapper_internal_make(data, position);
      to_c( data_in, *get_ptr(position) );
    }

    void set(CPPType const & data_in)
    {
      set(0, data_in);
    }

    void set(std::vector<CPPType> const & data_vector_in)
    {
      resize( data_vector_in.size() );
      for (std::size_t i = 0; i != data_vector_in.size(); ++i)
        set(i, data_vector_in[i]);
    }

    void push_back(CPPType const & data_in)
    {
      int pos = size();
      resize( pos+1 );
      set(pos, data_in);
    }


  private:

    DataT * get_ptr(int position) const
    {
      DataT * internal_data;
      viennamesh_data_wrapper_internal_get(data, position, (viennamesh_data*)&internal_data);
      assert(internal_data);
      return internal_data;
    }

    int make(viennamesh_context context)
    {
      release();
      int result = viennamesh_data_wrapper_make(context,
                                  result_of::data_information<DataT>::type_name().c_str(),
                                  &data );

      return result;
    }

  };




  template<typename FromT, typename ToT>
  void convert(data_handle<FromT> const & from, data_handle<ToT> & to)
  {
    viennamesh_data_wrapper_convert( from.data, to.data );
  }

}

#endif
