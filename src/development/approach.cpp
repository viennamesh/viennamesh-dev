http://stackoverflow.com/questions/5123688/c-iterator-pipelining-designs
http://ideone.com/5GxnW

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>
 
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/shared_ptr.hpp>
 
 
template <typename T>
struct identity
{
        T operator()(const T& x) { return x; }
};
 
struct any_function
{
        template <typename Res, typename Arg>
        any_function(boost::function<Res(Arg)> f)
        {
                impl.reset(make_impl(f));
        }
        
        boost::any operator()(const boost::any& x)
        {
                return impl->invoke(x);
        }
        
        static any_function compose(const any_function& f,
                                    const any_function& g)
        {
                any_function ans;
                ans.impl.reset(compose_impl(f.impl, g.impl));
                return ans;
        }
        
        template <typename T>
        static any_function id()
        {
                using boost::function;
                return function<T(T)>(identity<T>());
        }
        
        template <typename Res, typename Arg>
        boost::function<Res(Arg)> to_function()
        {
                return to_function_helper<Res, Arg>(impl);
        }
    
private:
        any_function() {}
        
        struct impl_type
        {
                virtual ~impl_type() {}
                virtual boost::any invoke(const boost::any&) = 0;
        };
        
        boost::shared_ptr<impl_type> impl;
        
        template <typename Res, typename Arg>
        static impl_type* make_impl(boost::function<Res(Arg)> f)
        {
                using boost::function;
                using boost::any;
                using boost::any_cast;
                
                class impl : public impl_type
                {
                        function<Res(Arg)> f;
                        
                        any invoke(const any& x)
                        {
                                return f(any_cast<Arg>(x));
                        }
                        
                public:
                        impl(function<Res(Arg)> f) : f(f) {}
                };
        
                return new impl(f);
        }
        
        static impl_type* compose_impl(const boost::shared_ptr<impl_type>& f,
                                       const boost::shared_ptr<impl_type>& g)
        {
                using boost::any;
                using boost::shared_ptr;
                
                class impl : public impl_type
                {
                        shared_ptr<impl_type> f, g;
                        
                        any invoke(const any& x)
                        {
                                return g->invoke(f->invoke(x));
                        }
                        
                public:
                        impl(const shared_ptr<impl_type>& f,
                             const shared_ptr<impl_type>& g)
                                : f(f), g(g)
                        {}
                };
                
                return new impl(f, g);
        }
        
        template <typename Res, typename Arg>
        struct to_function_helper
        {
                Res operator()(const Arg& x)
                {
                        using boost::any;
                        using boost::any_cast;
                        
                        return any_cast<Res>(p->invoke(any(x)));
                }
                
                to_function_helper(const boost::shared_ptr<impl_type>& p) : p(p) {}
                
        private:
                boost::shared_ptr<impl_type> p;
        };
};
 
 
template <typename Arg, typename Res, typename I>
boost::function<Res(Arg)> pipeline(I begin, I end)
{
        any_function ans = std::accumulate(
                begin, end, 
                any_function::id<Arg>(),
                std::ptr_fun(any_function::compose)
                );
 
        return ans.to_function<Res, Arg>();
}
 
 
double f(double x)
{
        return 4.2 * x;
}
 
int g(double x)
{
        return static_cast<int>(x - 2);
}
 
 
int main()
{
        using boost::function;
 
        std::vector<double> v;
        v.push_back(2);
        v.push_back(-1);
 
        std::vector<any_function> funcs;
        funcs.push_back(function<double(double)>(f));
        funcs.push_back(function<int(double)>(g));
 
        std::transform(v.begin(), v.end(),
                       std::ostream_iterator<int>(std::cout, "\n"),
                       pipeline<double, int>(funcs.begin(), funcs.end())
                );
                
        typedef boost::transform_iterator<
                boost::function<int(double)>, 
                std::vector<double>::const_iterator
        > iterator;
 
    boost::function<int(double)> ff = pipeline<double, int>(funcs.begin(), funcs.end());
    std::copy(iterator(v.begin(), ff), iterator(v.end(), ff), std::ostream_iterator<int>(std::cout, "\n"));
}




