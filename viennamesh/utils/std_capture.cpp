#include "viennamesh/utils/std_capture.hpp"
#include "viennamesh/utils/logger.hpp"

namespace viennautils
{

//   void * reader(void * data)
//   {
//     // Form descriptor
//     StdCapture & std_capture = *(StdCapture*)(data);
//     int readFd = std_capture.m_pipe[StdCapture::READ];
//     fd_set readset;
//     int err = 0, size = 0;
//     // Initialize time out struct for select()
//     struct timeval tv;
//     tv.tv_sec = 0;
//     tv.tv_usec = 10000;
//     // Implement the receiver loop
//     while( std_capture.m_capturing)
//     {
//       // Initialize the set
//       FD_ZERO(&readset);
//       FD_SET(readFd, &readset);
//       // Now, check for readability
//       err = select(readFd+1, &readset, NULL, NULL, &tv);
//       if (err > 0 && FD_ISSET(readFd, &readset))
//       {
//         // Clear flags
//         FD_CLR(readFd, &readset);
//
//         std::string m_captured;
//         std::string buf;
//         const int bufSize = 1024;
//         buf.resize(bufSize);
//         int bytesRead = 0;
//
//         bytesRead = read(std_capture.m_pipe[StdCapture::READ], &(*buf.begin()), bufSize);
//         while ( bytesRead > 0 )
//         {
//           buf.resize(bytesRead);
//           m_captured += buf;
//           bytesRead = read(std_capture.m_pipe[StdCapture::READ], &(*buf.begin()), bufSize);
//         }
//
//         std::string result;
//         std::string::size_type idx = m_captured.find_last_not_of("\r\n");
//         if (idx == std::string::npos)
//           result += m_captured;
//         else
//           result += m_captured.substr(0, idx+1);
//
//         write(std_capture.m_oldStdOut, m_captured.c_str(), m_captured.length()+1);
// //         viennamesh::info(5) << m_captured;
//       }
//     }
//
//     return NULL;
//   }

}
