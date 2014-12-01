#ifndef _VIENNAMESH_LOGGER_HPP_
#define _VIENNAMESH_LOGGER_HPP_

#include <sstream>
#include "viennamesh/backend/api.h"
#include "viennamesh/timer.hpp"

namespace viennamesh
{
  class log_instance
  {
  public:
    typedef std::ostringstream collector_stream_type;
    typedef int (*viennamesh_log_function_type)(const char *, int, const char *);

    log_instance(viennamesh_log_function_type function, std::string const & category_, int log_level_) :
      os_( new collector_stream_type() ),
      function_(function),
      category(category_),
      log_level(log_level_) {}

    ~log_instance()
    {
      function_( os_->str().c_str(), log_level, category.c_str() );
      delete os_;
    }

    template <typename T>
    collector_stream_type & operator<<(const T & x )
    {
      get() << x;
      return get();
    }

  private:

    collector_stream_type & get() { return *os_; }

    log_instance & operator =(const log_instance &) { return *this; }

    collector_stream_type * os_;

    viennamesh_log_function_type function_;
    std::string const & category;
    int log_level;
  };





  inline log_instance info(int log_level, std::string const & category  = "")
  { return log_instance(viennamesh_log_info_line, category, log_level); }
  inline log_instance error(int log_level, std::string const & category  = "")
  { return log_instance(viennamesh_log_error_line, category, log_level); }
  inline log_instance warning(int log_level, std::string const & category  = "")
  { return log_instance(viennamesh_log_warning_line, category, log_level); }
  inline log_instance debug(int log_level, std::string const & category  = "")
  { return log_instance(viennamesh_log_debug_line, category, log_level); }
  inline log_instance stack(int log_level, std::string const & category  = "")
  { return log_instance(viennamesh_log_stack_line, category, log_level); }






  class LoggingStack
  {
  public:

    typedef int (*viennamesh_log_function_type)(const char *, int, const char *);

    LoggingStack() : stack_name("") { init(); }
    LoggingStack( std::string const & stack_name_ ) : stack_name(stack_name_) { init(); }

    ~LoggingStack() { deinit(); }

  private:

    void init()
    {
      stack(5) << "Opening stack";
      if (!stack_name.empty())
        stack(5) << " '" << stack_name << "'";
      stack(5) << "" << std::endl;
      viennamesh_log_increase_indentation();
      timer.start();
    }

    void deinit()
    {
      double time = timer.get();
      viennamesh_log_decrease_indentation();
      stack(5) << "Closing stack";
      if (!stack_name.empty())
        stack(5) << " '" << stack_name << "'";
      stack(5) << " (took " << time << "sec)" << std::endl;
    }

    viennautils::Timer timer;
    std::string stack_name;
  };




}

#endif
