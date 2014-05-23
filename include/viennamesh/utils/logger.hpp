#ifndef VIENNAUTILS_LOGGER_HPP
#define VIENNAUTILS_LOGGER_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include <string>
#include <vector>
#include <list>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>

#ifndef _WIN32
#include <fcntl.h>
#include <unistd.h>
#endif

#include "viennamesh/utils/string_tools.hpp"
#include "viennamesh/utils/timer.hpp"

namespace viennamesh
{
  using std::string;

  struct red
  {
    static std::string prefix() { return "\033[1;31m"; }
    static std::string suffix() { return "\033[0m"; }
  };
  struct green
  {
    static std::string prefix() { return "\033[1;32m"; }
    static std::string suffix() { return "\033[0m"; }
  };

  struct yellow
  {
    static std::string prefix() { return "\033[1;33m"; }
    static std::string suffix() { return "\033[0m"; }
  };

  struct blue
  {
    static std::string prefix() { return "\033[1;34m"; }
    static std::string suffix() { return "\033[0m"; }
  };

  struct magenta
  {
    static std::string prefix() { return "\033[1;35m"; }
    static std::string suffix() { return "\033[0m"; }
  };

  struct cyan
  {
    static std::string prefix() { return "\033[1;36m"; }
    static std::string suffix() { return "\033[0m"; }
  };



  struct stack_tag
  {
    static std::string name() { return "stack"; }
    typedef cyan color;
  };

  struct info_tag
  {
    static std::string name() { return "info"; }
    typedef blue color;
  };

  struct warning_tag
  {
    static std::string name() { return "warning"; }
    typedef yellow color;
  };

  struct error_tag
  {
    static std::string name() { return "error"; }
    typedef red color;
  };

  struct debug_tag
  {
    static std::string name() { return "debug"; }
    typedef magenta color;
  };

  template<typename LoggingTagT>
  std::string colored_name()
  {
    return LoggingTagT::color::prefix() + LoggingTagT::name() + LoggingTagT::color::suffix();
  }


  enum LoggerBehavior
  {
    use_global_loglevels = 0,
    use_local_loglevels = 1,
    use_category_loglevels = 2
  };

  class Logger;

  template<typename LoggingTagT>
  class log_instance
  {
  public:
    typedef std::ostringstream collector_stream_type;

    log_instance(Logger & logger_obj_, std::string const & category_, int log_level_) :
      os_( new collector_stream_type() ),
      logger_obj(logger_obj_),
      category(category_),
      log_level(log_level_) {}

    ~log_instance();

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

    Logger & logger_obj;
    std::string const & category;
    int log_level;
  };



  template<typename LoggingLevelT>
  struct LoggingLevels
  {
    LoggingLevels() {}
    LoggingLevels( LoggingLevelT default_level ) { set_all(default_level); }

    LoggingLevelT const & get( stack_tag ) const { return stack_level; }
    LoggingLevelT const & get( info_tag ) const { return info_level; }
    LoggingLevelT const & get( warning_tag ) const { return warning_level; }
    LoggingLevelT const & get( error_tag ) const { return error_level; }
    LoggingLevelT const & get( debug_tag ) const { return debug_level; }

    template<typename LoggingTagT>
    LoggingLevelT const & get() const { return get( LoggingTagT() ); }


    void set( stack_tag, LoggingLevelT const & value ) { stack_level = value; }
    void set( info_tag, LoggingLevelT const & value ) { info_level = value; }
    void set( warning_tag, LoggingLevelT const & value ) { warning_level = value; }
    void set( error_tag, LoggingLevelT const & value ) { error_level = value; }
    void set( debug_tag, LoggingLevelT const & value ) { debug_level = value; }

    template<typename LoggingTagT>
    void set(LoggingLevelT const & value) { set( LoggingTagT(), value ); }

    void set_all( LoggingLevelT default_level )
    {
      stack_level = default_level;
      info_level = default_level;
      warning_level = default_level;
      error_level = default_level;
      debug_level = default_level;
    }


    LoggingLevelT stack_level;
    LoggingLevelT info_level;
    LoggingLevelT warning_level;
    LoggingLevelT error_level;
    LoggingLevelT debug_level;
  };







  struct BaseCallback
  {
    virtual ~BaseCallback() {}

    virtual std::string make(
              Logger const & logger,
              std::string const & tag_name,
              std::string const & colored_tag_name,
              int log_level,
              std::string const & category,
              std::string const & message) const = 0;

    virtual void write(std::string const & message) = 0;

    template<typename LoggingTagT>
    void log(Logger const & logger,
            int log_level,
            std::string const & category,
            std::string const & message);

    LoggingLevels< std::map<std::string, int> > category_log_levels;
    LoggingLevels< int > local_log_levels;
  };



  template<typename OutputFormaterT>
  struct OStreamCallback : public BaseCallback
  {
    OStreamCallback(std::ostream & stream_) : stream(stream_) {}

    virtual std::string make(
              Logger const & logger,
              std::string const & tag_name,
              std::string const & colored_tag_name,
              int log_level,
              std::string const & category,
              std::string const & message) const
    {
      return formater.make(logger, tag_name, colored_tag_name, log_level, category, message);
    }

    virtual void write(std::string const & message)
    {
      stream << message;
    }

    std::ostream & stream;
    OutputFormaterT formater;
  };

  template<typename OutputFormaterT>
  struct FileStreamCallback : public BaseCallback
  {
    FileStreamCallback(std::string const & filename) : stream(filename.c_str())
    {
//       stream.open( filename.c_str() );
    }

    virtual std::string make(
              Logger const & logger,
              std::string const & tag_name,
              std::string const & colored_tag_name,
              int log_level,
              std::string const & category,
              std::string const & message) const
    {
      return formater.make(logger, tag_name, colored_tag_name, log_level, category, message);
    }

    virtual void write(std::string const & message)
    {
      stream << message;
    }

    std::ofstream stream;
    OutputFormaterT formater;
  };






  class Logger
  {
  public:

    Logger() : behavior_(use_global_loglevels), indentation_count_(0), global_log_levels_(5) {}
    ~Logger()
    {
      for (std::list< BaseCallback * >::iterator it = callbacks.begin(); it != callbacks.end(); ++it)
        delete *it;
    }

    template<typename LoggingTagT>
    log_instance<LoggingTagT> stream(int log_level, std::string const & category = "")
    { return log_instance<LoggingTagT>(*this, category, log_level); }

    log_instance<stack_tag> stack(int log_level, std::string const & category  = "")
    { return stream<stack_tag>(log_level, category); }

    log_instance<info_tag> info(int log_level, std::string const & category  = "")
    { return stream<info_tag>(log_level, category); }

    log_instance<warning_tag> warning(int log_level, std::string const & category  = "")
    { return stream<warning_tag>(log_level, category); }

    log_instance<error_tag> error(int log_level, std::string const & category  = "")
    { return stream<error_tag>(log_level, category); }

    log_instance<debug_tag> debug(int log_level, std::string const & category  = "")
    { return stream<debug_tag>(log_level, category); }



    template<typename LoggingTagT>
    void log( int log_level,
                  std::string const & category,
                  std::string const & message )
    {
      for (std::list< BaseCallback * >::iterator it = callbacks.begin(); it != callbacks.end(); ++it)
        (*it)->log<LoggingTagT>(*this, log_level, category, message);
    }

    std::list< BaseCallback * >::iterator register_callback( BaseCallback * callback )
    {
      callbacks.push_back( callback );
      return --(callbacks.end());
    }

    void unregister_callback( std::list< BaseCallback * >::iterator const & callback_handle )
    {
      callbacks.erase(callback_handle);
    }

    LoggerBehavior behavior() const { return behavior_; }
    LoggingLevels< int > const & global_log_levels() const { return global_log_levels_; }
    template<typename LoggingTagT>
    void set_log_level( int level ) { global_log_levels_.set<LoggingTagT>(level); }
    void set_all_log_level( int level ) { global_log_levels_.set_all(level); }

    void increase_indentation() { ++indentation_count_; }
    void decrease_indentation() { --indentation_count_; }
    int indentation_count() const { return indentation_count_; }

  private:

    LoggerBehavior behavior_;
    int indentation_count_;

    LoggingLevels< int > global_log_levels_;

    std::list< BaseCallback * > callbacks;
  };



    template<typename LoggingTagT>
    log_instance<LoggingTagT>::~log_instance()
    {
      logger_obj.template log<LoggingTagT>( log_level, category, os_->str() );
      delete os_;
    }


    template<typename LoggingTagT>
    void BaseCallback::log(Logger const & logger,
                          int log_level,
                          std::string const & category,
                          std::string const & message)
    {
      if (logger.behavior() == use_global_loglevels)
      {
        if (log_level <= logger.global_log_levels().get<LoggingTagT>())
          write( make(logger, LoggingTagT::name(), colored_name<LoggingTagT>(), log_level, category, message) );
      }
      else if (logger.behavior() == use_local_loglevels)
      {
        if (log_level <= local_log_levels.get<LoggingTagT>())
          write( make(logger, LoggingTagT::name(), colored_name<LoggingTagT>(), log_level, category, message) );
      }
      else
      {
        std::map<std::string, int> const & level_map = category_log_levels.get<LoggingTagT>();
        std::map<std::string, int>::const_iterator it = level_map.find(category);
        if (it != level_map.end())
        {
          if (log_level <= it->second)
            write( make(logger, LoggingTagT::name(), colored_name<LoggingTagT>(), log_level, category, message) );
        }
        else if (log_level <= local_log_levels.get<LoggingTagT>() )
        {
          write( make(logger, LoggingTagT::name(), colored_name<LoggingTagT>(), log_level, category, message) );
        }
      }
    }



  class CoutColorFormater
  {
  public:

    CoutColorFormater() : last_char_newline(true) {}


    void make_header(Logger const & logger,
            std::string const &,
            std::string const & colored_tag_name,
            int log_level,
            std::string const & category,
            std::ostream & stream) const
    {
      for (int i = 0; i < logger.indentation_count(); ++i)
        stream << "  ";
      stream <<  "(" << log_level << ") " << colored_tag_name;
      if (!category.empty())
        stream << " in '" << category << "'";
      stream << ": ";
    }

    std::string make(
              Logger const & logger,
              std::string const & tag_name,
              std::string const & colored_tag_name,
              int log_level,
              std::string const & category,
              std::string const & message) const
    {
      std::ostringstream tmp;

      std::string::size_type pos = 0;
      while (pos < message.size())
      {
        if (last_char_newline)
        {
          make_header(logger, tag_name, colored_tag_name, log_level, category, tmp);
          last_char_newline = false;
        }

        std::string::size_type new_pos = message.find("\n", pos);
        if (new_pos == std::string::npos)
        {
          tmp << message.substr(pos, message.size()-pos);
          return tmp.str();
        }

        tmp << message.substr(pos, new_pos-pos) << "\n";
        pos = new_pos+1;
        last_char_newline = true;
      }

      return tmp.str();
    }

  private:

    mutable bool last_char_newline;
  };



  class FileStreamFormater
  {
  public:

    FileStreamFormater() : last_char_newline(true) {}


    void make_header(Logger const & logger,
            std::string const & tag_name,
            std::string const &,
            int log_level,
            std::string const & category,
            std::ostream & stream) const
    {
      for (int i = 0; i < logger.indentation_count(); ++i)
        stream << "  ";
      stream <<  "(" << log_level << ") " << tag_name;
      if (!category.empty())
        stream << " in '" << category << "'";
      stream << ": ";
    }

    std::string make(
              Logger const & logger,
              std::string const & tag_name,
              std::string const & colored_tag_name,
              int log_level,
              std::string const & category,
              std::string const & message) const
    {
      std::ostringstream tmp;

      std::string::size_type pos = 0;
      while (pos < message.size())
      {
        if (last_char_newline)
        {
          make_header(logger, tag_name, colored_tag_name, log_level, category, tmp);
          last_char_newline = false;
        }

        std::string::size_type new_pos = message.find("\n", pos);
        if (new_pos == std::string::npos)
        {
          tmp << message.substr(pos, message.size()-pos);
          return tmp.str();
        }

        tmp << message.substr(pos, new_pos-pos) << "\n";
        pos = new_pos+1;
        last_char_newline = true;
      }

      return tmp.str();
    }

  private:

    mutable bool last_char_newline;
  };




  Logger & logger();

  inline log_instance<info_tag> info(int log_level, std::string const & category  = "")
  { return logger().info(log_level, category); }
  inline log_instance<error_tag> error(int log_level, std::string const & category  = "")
  { return logger().error(log_level, category); }
  inline log_instance<warning_tag> warning(int log_level, std::string const & category  = "")
  { return logger().warning(log_level, category); }
  inline log_instance<debug_tag> debug(int log_level, std::string const & category  = "")
  { return logger().debug(log_level, category); }





  class LoggingStack
  {
  public:

    LoggingStack() : stack_name(""), logger_obj(logger()) { init(); }
    LoggingStack( std::string const & stack_name_ ) : stack_name(stack_name_), logger_obj(logger()) { init(); }
    LoggingStack( Logger & logger_obj_ ) : stack_name(""), logger_obj(logger_obj_) { init(); }
    LoggingStack( Logger & logger_obj_, std::string const & stack_name_ ) : stack_name(stack_name_), logger_obj(logger_obj_) { init(); }

    ~LoggingStack() { deinit(); }


  private:

    void init()
    {
      logger_obj.stack(5) << "Opening stack";
      if (!stack_name.empty())
        logger_obj.stack(5) << " '" << stack_name << "'";
      logger_obj.stack(5) << "" << std::endl;
      logger_obj.increase_indentation();
      timer.start();
    }

    void deinit()
    {
      double time = timer.get();
      logger_obj.decrease_indentation();
      logger_obj.stack(5) << "Closing stack";
      if (!stack_name.empty())
        logger_obj.stack(5) << " '" << stack_name << "'";
      logger_obj.stack(5) << " (took " << time << "sec)" << std::endl;
    }

    viennautils::Timer timer;
    std::string stack_name;
    Logger & logger_obj;
  };





#ifndef _WIN32
  // http://stackoverflow.com/questions/5419356/redirect-stdout-stderr-to-a-string
  // http://stackoverflow.com/questions/5911147/how-to-redirect-printf-output-back-into-code
  // http://ashishgrover.com/linux-multi-threading-fifos-or-named-pipes/

  void * reader(void * data);

  class StdCapture
  {
    friend void * reader(void * data);
  public:

      pthread_t readerThread;

      StdCapture(): m_capturing(false), m_init(false), m_oldStdOut(0), m_oldStdErr(0)
      {
          m_pipe[READ] = 0;
          m_pipe[WRITE] = 0;
          if (pipe(m_pipe) == -1)
              return;
//  #ifndef _WIN32
          // Reading pipe has to be set to non-blocking
          fcntl(m_pipe[READ], F_SETFL, fcntl(m_pipe[READ], F_GETFL) | O_NONBLOCK);
//  #endif

          m_oldStdOut = dup(fileno(stdout));
          m_oldStdErr = dup(fileno(stderr));
          if (m_oldStdOut == -1 || m_oldStdErr == -1)
              return;

          m_init = true;
      }

      ~StdCapture()
      {
          if (m_capturing)
          {
              finish();
          }
          if (m_oldStdOut > 0)
              close(m_oldStdOut);
          if (m_oldStdErr > 0)
              close(m_oldStdErr);
          if (m_pipe[READ] > 0)
              close(m_pipe[READ]);
          if (m_pipe[WRITE] > 0)
              close(m_pipe[WRITE]);
      }


      void start()
      {
          if (!m_init)
              return;
          if (m_capturing)
              finish();
          fflush(stdout);
          fflush(stderr);
          dup2(m_pipe[WRITE], fileno(stdout));
          dup2(m_pipe[WRITE], fileno(stderr));

          m_capturing = true;
          thread_running = true;
          pthread_create( &readerThread, NULL, &reader, (void*) (this));
      }

      bool finish()
      {
          if (!m_init)
              return false;
          if (!m_capturing)
              return false;
          fflush(stdout);
          fflush(stderr);

          thread_running = false;
          pthread_join( readerThread, NULL );
          m_capturing = false;

          dup2(m_oldStdOut, fileno(stdout));
          dup2(m_oldStdErr, fileno(stderr));

          return true;
      }

      bool capturing() const { return m_capturing; }
      int old_stdout() const { return m_oldStdOut; }

  private:
      enum PIPES { READ, WRITE };
      int m_pipe[2];

      bool thread_running;
      bool m_capturing;
      bool m_init;

      int m_oldStdOut;
      int m_oldStdErr;
  };
#else
	class StdCapture
	{
  public:

    StdCapture() {}
    ~StdCapture() {}

    void start() {}
    bool finish() { return true; }

    bool capturing() const { return false; }
    int old_stdout() const { return -1; }

  private:
	};
#endif

  StdCapture & std_capture();

  template<typename OutputFormaterT>
  struct StdOutCallback : public BaseCallback
  {
    StdOutCallback();

    virtual std::string make(
      Logger const & logger,
      std::string const & tag_name,
      std::string const & colored_tag_name,
      int log_level,
      std::string const & category,
      std::string const & message) const
    {
      return formater.make(logger, tag_name, colored_tag_name, log_level, category, message);
    }

    void write(std::string const & message);

    OutputFormaterT formater;
  };

}


#endif
