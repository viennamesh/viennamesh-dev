#ifndef _VIENNAMESH_DATA_HPP_
#define _VIENNAMESH_DATA_HPP_

#include <cassert>
#include "viennameshpp/forwards.hpp"
#include "viennameshpp/common.hpp"
#include "viennagrid/viennagrid.hpp"

namespace viennamesh
{
  class abstract_data_handle
  {
    friend class context_handle;
    friend class algorithm_handle;

  protected:

    abstract_data_handle();
    abstract_data_handle(viennamesh_data_wrapper data_, bool retain_ = true);

  public:

    abstract_data_handle(abstract_data_handle const & handle_);
    ~abstract_data_handle();

    abstract_data_handle & operator=(abstract_data_handle const & handle_);

    bool valid() const;
    bool empty() const;

    int size() const;
    void resize(int size_);

    viennamesh_data_wrapper internal() const;

    std::string type_name() const;
    template<typename DataT>
    bool is_type() const
    {
      return type_name() == result_of::data_information<DataT>::type_name();
    }

  protected:

    void retain();
    void release();

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



  // viennagrid::mesh
  viennagrid::mesh to_cpp(viennagrid_mesh & src);
  void to_c(viennagrid::mesh const & src, viennagrid_mesh & dst);

  // viennagrid::quantity_field
  viennagrid::quantity_field to_cpp(viennagrid_quantity_field & src);
  void to_c(viennagrid::quantity_field const & src, viennagrid_quantity_field & dst);

  // viennagrid::point_t
  viennagrid::point to_cpp(viennamesh_point & src);
  void to_c(viennagrid::point const & src, viennamesh_point & dst);

  // viennagrid::seed_point_t
  seed_point to_cpp(viennamesh_seed_point & src);
  void to_c(seed_point const & src, viennamesh_seed_point & dst);

  // std::string
  std::string to_cpp(viennamesh_string & src);
  void to_c(std::string const & src, viennamesh_string dst);



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
      handle_error(viennamesh_data_wrapper_internal_get(data, position, (viennamesh_data*)&internal_data), data);
      assert(internal_data);
      return internal_data;
    }

    void make(viennamesh_context context)
    {
      release();
      handle_error(
        viennamesh_data_wrapper_make(context,
                                  result_of::data_information<DataT>::type_name().c_str(),
                                  &data ),
        internal());
    }

  };




  template<typename FromT, typename ToT>
  void convert(data_handle<FromT> const & from, data_handle<ToT> & to)
  {
    handle_error(viennamesh_data_wrapper_convert( from.data, to.data ), from);
  }

}

#endif
