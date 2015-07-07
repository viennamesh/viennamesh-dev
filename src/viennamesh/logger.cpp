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

#include "logger.hpp"

namespace viennamesh
{
  namespace backend
  {

    int Logger::register_color_cout_callback()
    {
      return register_callback( new StdOutCallback<CoutColorFormater>() );
    }

    int Logger::register_file_callback( std::string const & filename )
    {
      return register_callback( new FileStreamCallback<FileStreamFormater>(filename) );
    }

    Logger & logger()
    {
      static bool is_init = false;
      static Logger logger_;

      if (!is_init)
      {
        logger_.register_color_cout_callback();
        is_init = true;
      }

      return logger_;
    }


    StdCapture & StdCapture::get()
    {
      static StdCapture std_capture_;
      return std_capture_;
    }

#ifndef _WIN32
    void * StdCapture::reader(void * data)
    {
      // Form descriptor
      StdCapture & std_capture = *(StdCapture*)(data);
      int readFd = std_capture.m_pipe[StdCapture::READ];
      fd_set readset;
      int err = 0;
      // Initialize time out struct for select()
      struct timeval tv;
      tv.tv_sec = 0;
      tv.tv_usec = 10000;
      // Implement the receiver loop
      while(std_capture.thread_running)
      {
        // Initialize the set
        FD_ZERO(&readset);
        FD_SET(readFd, &readset);
        // Now, check for readability
        err = select(readFd+1, &readset, NULL, NULL, &tv);
        if (err > 0 && FD_ISSET(readFd, &readset))
        {
          // Clear flags
          FD_CLR(readFd, &readset);

          std::string m_captured;
          std::string buf;
          const int bufSize = 1024;
          buf.resize(bufSize);
          int bytesRead = 0;

          bytesRead = read(std_capture.m_pipe[StdCapture::READ], &(*buf.begin()), bufSize);
          while ( bytesRead > 0 )
          {
            buf.resize(bytesRead);
            m_captured += buf;
            bytesRead = read(std_capture.m_pipe[StdCapture::READ], &(*buf.begin()), bufSize);
          }

          logger().log<info_tag>(7, m_captured);
        }
      }

      return NULL;
    }

#endif

    template<typename OutputFormaterT>
    StdOutCallback<OutputFormaterT>::StdOutCallback() {}

    template<typename OutputFormaterT>
    void StdOutCallback<OutputFormaterT>::write(std::string const & message)
    {
#ifndef _WIN32
      if (StdCapture::get().capturing())
      {
        ::write( StdCapture::get().old_stdout(), message.c_str(), message.length()+1 );
      }
      else
        std::cout << message;
#else
      std::cout << message;
#endif
    }


  }
}
