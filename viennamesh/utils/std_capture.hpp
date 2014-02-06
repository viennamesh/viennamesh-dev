#ifndef VIENNAUTILS_STDCAPTURE_HPP
#define VIENNAUTILS_STDCAPTURE_HPP

#include <fcntl.h>
#include <stdio.h>
#include <string>

#ifdef _WIN32

  #include <io.h>
  #define pipe(X) _pipe(X,4096,O_BINARY)
  #define fileno _fileno
  #define dup2 _dup2
  #define read _read

#else

  #include <unistd.h>
  #include <pthread.h>

#endif


namespace viennautils
{

  // http://stackoverflow.com/questions/5419356/redirect-stdout-stderr-to-a-string
  // http://stackoverflow.com/questions/5911147/how-to-redirect-printf-output-back-into-code

  // http://ashishgrover.com/linux-multi-threading-fifos-or-named-pipes/


//   void * reader(void * data);
//
//   class StdCapture
//   {
//     friend void * reader(void * data);
//   public:
//
//       pthread_t readerThread;
//
//       StdCapture(): m_capturing(false), m_init(false), m_oldStdOut(0), m_oldStdErr(0)
//       {
//           m_pipe[READ] = 0;
//           m_pipe[WRITE] = 0;
//           if (pipe(m_pipe) == -1)
//               return;
//   #ifndef _WIN32
//           // Reading pipe has to be set to non-blocking
//           fcntl(m_pipe[READ], F_SETFL, fcntl(m_pipe[READ], F_GETFL) | O_NONBLOCK);
//   #endif
//
//           m_oldStdOut = dup(fileno(stdout));
//           m_oldStdErr = dup(fileno(stderr));
//           if (m_oldStdOut == -1 || m_oldStdErr == -1)
//               return;
//
//           m_init = true;
//       }
//
//       ~StdCapture()
//       {
//           if (m_capturing)
//           {
//               finish();
//           }
//           if (m_oldStdOut > 0)
//               close(m_oldStdOut);
//           if (m_oldStdErr > 0)
//               close(m_oldStdErr);
//           if (m_pipe[READ] > 0)
//               close(m_pipe[READ]);
//           if (m_pipe[WRITE] > 0)
//               close(m_pipe[WRITE]);
//       }
//
//
//       void start()
//       {
//           if (!m_init)
//               return;
//           if (m_capturing)
//               finish();
//           fflush(stdout);
//           fflush(stderr);
//           dup2(m_pipe[WRITE], fileno(stdout));
//           dup2(m_pipe[WRITE], fileno(stderr));
//           m_capturing = true;
//
//           pthread_create( &readerThread, NULL, &reader, (void*) (this));
//       }
//
//       bool finish()
//       {
//           if (!m_init)
//               return false;
//           if (!m_capturing)
//               return false;
//           fflush(stdout);
//           fflush(stderr);
//           dup2(m_oldStdOut, fileno(stdout));
//           dup2(m_oldStdErr, fileno(stderr));
//           m_captured.clear();
//
// //           std::string buf;
// //           const int bufSize = 1024;
// //           buf.resize(bufSize);
// //           int bytesRead = 0;
// //
// //   #ifdef _WIN32
// //           if (!eof(m_pipe[READ]))
// //           {
// //               bytesRead = read(m_pipe[READ], &(*buf.begin()), bufSize);
// //           }
// //           while(bytesRead == bufSize)
// //           {
// //               m_captured += buf;
// //               bytesRead = 0;
// //               if (!eof(m_pipe[READ]))
// //               {
// //                   bytesRead = read(m_pipe[READ], &(*buf.begin()), bufSize);
// //               }
// //           }
// //           if (bytesRead > 0)
// //           {
// //               buf.resize(bytesRead);
// //               m_captured += buf;
// //           }
// //   #else
// //
// //           bytesRead = read(m_pipe[READ], &(*buf.begin()), bufSize);
// //           while ( bytesRead > 0 )
// //           {
// //             buf.resize(bytesRead);
// //             m_captured += buf;
// //             bytesRead = read(m_pipe[READ], &(*buf.begin()), bufSize);
// //           }
// //
// //   #endif
//
//           m_capturing = false;
//           pthread_join( readerThread, NULL );
//
//           return true;
//       }
//
//       std::string get() const
//       {
//           std::string::size_type idx = m_captured.find_last_not_of("\r\n");
//           if (idx == std::string::npos)
//           {
//               return m_captured;
//           }
//           else
//           {
//               return m_captured.substr(0, idx+1);
//           }
//       }
//
//   private:
//       enum PIPES { READ, WRITE };
//       int m_pipe[2];
//       bool m_capturing;
//       bool m_init;
//       int m_oldStdOut;
//       int m_oldStdErr;
//       std::string m_captured;
//   };


}

#endif
