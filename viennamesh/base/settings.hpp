#ifndef VIENNAMESH_BASE_SETTINGS_HPP
#define VIENNAMESH_BASE_SETTINGS_HPP

#include "viennamesh/base/convert.hpp"
#include "viennamesh/utils/utils.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/concept_check.hpp>

namespace viennamesh
{

    namespace result_of
    {       
        template<typename algorithm_tag>
        struct settings;
    }

    
    
    template<typename scalar_type>
    class BaseParameter
    {
    public:
        BaseParameter(scalar_type const & default_value_) : is_ignored_(true), default_value(default_value_) {}
        
        bool is_ignored() const { return is_ignored_; }
        void set_ignored( bool is_ignored_val = true ) { is_ignored_ = is_ignored_val; }
        
        scalar_type get_default() const { return default_value; }
        
    private:
        bool is_ignored_;
        scalar_type default_value;
    };
    
    
    
    
    
    template<typename scalar_type_>
    class ScalarParameter : public BaseParameter<scalar_type_>
    {
    public:
        typedef scalar_type_ scalar_type;
        typedef ScalarParameter<scalar_type> self_type;
        
        ScalarParameter(scalar_type const & default_value_) : BaseParameter<scalar_type>(default_value_) {}

        scalar_type operator()() const { return !this->is_ignored() ? value : this->get_default(); }
        
        self_type & operator= (scalar_type const & value_)
        {
            set_scalar(value_);
            return *this;
        }
        
        void set_scalar(scalar_type const & value_)
        {
            value = value_;
            this->set_ignored(false);
        }

    private:
        scalar_type value;
    };
    
    
    
    template<typename scalar_type_, typename point_type_>
    class BaseFieldFunctor
    {
    public:
        typedef scalar_type_ scalar_type;
        typedef point_type_ point_type;
        
        virtual ~BaseFieldFunctor() {}
        
        virtual scalar_type look_up(point_type const & pos) const = 0;
    };
    
    template<typename scalar_type, typename point_type, typename functor>
    class FieldFunctor : public BaseFieldFunctor<scalar_type, point_type>
    {
    public:
        FieldFunctor(functor const & f_) : f(&f_) {}
        
        virtual scalar_type look_up(point_type const & pos) const { return (*f)(pos); }
        
    private:
        functor const * f;
    };
    
    
    
    template<bool is_scalar>
    struct set_helper;
    
    template<>
    struct set_helper<true>
    {
        template<typename field_parameter_ptr_type, typename type>
        static void set( field_parameter_ptr_type * f, type const & t )
        {
            f->set_scalar(t);
        }
    };
    
    template<>
    struct set_helper<false>
    {
        template<typename field_parameter_ptr_type, typename type>
        static void set( field_parameter_ptr_type * f, type const & t )
        {
            f->set_field(t);
        }
    };
    
    
    template<typename scalar_type_, typename point_type_>
    class FieldParameter : public ScalarParameter<scalar_type_>
    {
    public:
        
        typedef scalar_type_ scalar_type;
        typedef point_type_ point_type;
        
        typedef FieldParameter<scalar_type, point_type> self_type;
        
        typedef BaseFieldFunctor<scalar_type, point_type> field_functor_type;
        typedef boost::shared_ptr<field_functor_type> field_functor_ptr_type;
        typedef boost::shared_ptr<const field_functor_type> field_functor_const_ptr_type;
        
        FieldParameter(scalar_type const & default_value_) : ScalarParameter<scalar_type>(default_value_), is_scalar_(true), field_functor_() {}

        
        template<typename type>
        self_type & operator= ( type const & t )
        {
            set_helper< utils::is_scalar<type>::value >::set(this, t);
            return *this;
        }      
        

        void set_scalar( scalar_type_ const & value_ )
        {
            ScalarParameter<scalar_type_>::set_scalar(value_);
            set_scalar_state(true);
        }
        
        template<typename functor_type>
        void set_field( functor_type const & f )
        {
            field_functor_ = boost::shared_ptr<field_functor_type>(new FieldFunctor<scalar_type, point_type, functor_type>(f));
            this->set_ignored(false);
            
            set_scalar_state(false);
        }
        
        bool is_scalar() const { return is_scalar_; }
        void set_scalar_state( bool is_scalar_val = true ) { is_scalar_ = is_scalar_val; }
        
        field_functor_ptr_type get_field() { return field_functor_; }
        field_functor_const_ptr_type get_field() const { return field_functor_; }
        
    private:
        bool is_scalar_;
        field_functor_ptr_type field_functor_;
    };

}

#endif
