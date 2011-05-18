#ifdef SOCKETS

// based on the socket class by Rob Tougher
// http://www.linuxgazette.com/issue74/tougher.html


#ifdef WIN32
#include <winsock2.h>
#endif

#include "sockets.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <math.h>



namespace SOCKETNAMESPACE
{
  using namespace SOCKETNAMESPACE;
  using namespace std;

#ifdef WIN32
  static bool sockets_warmed_up = false;
#endif


  SocketCommand::SocketCommand()
  {
    for(int i=0; i<SOCKETCOMMANDLENGTH; i++)
      command[i] = '-';
  }
  
  string SocketCommand::GetString(void) const
  {
    string retval;
    for(int i=0; i<SOCKETCOMMANDLENGTH; i++)
      retval += command[i];
    //retval += '\0';
    return retval;
  }

  SocketCommand & SocketCommand::operator= (const SocketCommand & sc)
  {
    for(int i=0; i<SOCKETCOMMANDLENGTH; i++)
      Set(i,sc.Get(i));
    
    return *this;
  }
  
  SocketCommand & SocketCommand::operator= (const char * sc)
  {
    int i=0;
    while(i<SOCKETCOMMANDLENGTH && sc[i] != '\0')
      {
	Set(i,sc[i]);
	i++;
      }
    while(i<SOCKETCOMMANDLENGTH)
      {
	Set(i,'-');
	i++;
      }
    
    return *this;
  }
  
  
  std::ostream & operator<<(std::ostream & s, const SocketCommand & sc)
  {
    for(int i=0; i<SOCKETCOMMANDLENGTH; i++)
      s << sc.Get(i);
    
    return s;
  }
  std::istream & operator>>(std::istream & s, SocketCommand & sc)
  {
    char val;
    for(int i=0; i<SOCKETCOMMANDLENGTH; i++)
      {
	s >> val;
	sc.Set(i,val);
      }
    return s;
  }
  
  
  bool operator==(const SocketCommand & sc1, const SocketCommand & sc2)
  {
    for(int i=0; i<SOCKETCOMMANDLENGTH; i++)
      if(sc1.Get(i) != sc2.Get(i)) return false;
    
    return true;
  }





  
  string GetPlainText(const SocketCommand & command)
  {
    std::string plaintext;

    if(command == CLOSE)
      plaintext = "Close";
    else if (command == EXIT)
      plaintext = "Exit";
    else if (command == BLOCK)
      plaintext = "Block";
    else if (command == UNBLOCK)
      plaintext = "Unblock";
    else if (command == RECVMESH)
      plaintext = "ReceiveMesh";
    else if (command == RECVPDE)
      plaintext = "ReceivePDE";
    else if (command == WRITESOLUTION)
      plaintext = "WriteSolution";
    else if (command == SOLVEPDE)
      plaintext = "SolvePDE";
    else
      plaintext = command.GetString();

    return plaintext;
  }











  Socket::Socket() :
    m_sock ( -1 )
  {

    memset ( &m_addr,
	     0,
	     sizeof ( m_addr ) );

#ifdef WIN32
    if(!sockets_warmed_up)
      {
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(1,1),&wsa))
	  {
	    cerr << "WSAStartup() failed, " << GetLastError() << endl;
	  }
      }
#endif

  }

  Socket::~Socket()
  {
    if ( is_valid() )
#ifdef WIN32		
      ::closesocket ( m_sock );
#else
    ::close ( m_sock );
#endif
  }

  bool Socket::create()
  {
    m_sock = socket ( AF_INET,
		      SOCK_STREAM,
		      0 );

    if ( ! is_valid() )
      return false;


    // TIME_WAIT - argh
    int on = 1;
    if ( setsockopt ( m_sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
      {
#ifdef WIN32
	latesterror = WSAGetLastError();
#else
	latesterror = errno;
#endif
	return false;
      }


    return true;

  }



  bool Socket::bind ( const int port )
  {

    if ( ! is_valid() )
      {
	return false;
      }



    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = htons ( port );

    int bind_return = ::bind ( m_sock,
			       ( struct sockaddr * ) &m_addr,
			       sizeof ( m_addr ) );


    if ( bind_return == -1 )
      {
#ifdef WIN32
	latesterror = WSAGetLastError();
#else
	latesterror = errno;
#endif
	return false;
      }

    return true;
  }


  bool Socket::listen() const
  {
    if ( ! is_valid() )
      {
	return false;
      }

    int listen_return = ::listen ( m_sock, MAXCONNECTIONS );


    if ( listen_return == -1 )
      {
#ifdef WIN32
	latesterror = WSAGetLastError();
#else
	latesterror = errno;
#endif
	return false;
      }

    return true;
  }


  bool Socket::accept ( Socket& new_socket ) const
  {
    int addr_length = sizeof ( m_addr );
    new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );

#ifdef WIN32
    if ( new_socket.m_sock == INVALID_SOCKET )
      return false;
    else
      return true;
#else
    if ( new_socket.m_sock <= 0 )
      return false;
    else
      return true;
#endif
  }


  bool Socket::send ( const std::string & s ) const
  {
    // If necessary the string is divided to substrings which are sent one by one.
    // The first substring has the form "totallength substring"

    std::ostringstream * sendstring = NULL;

    sendstring = new std::ostringstream;

    (*sendstring) << s.size() << " ";

    int curlength = sendstring->str().size();

    int partbegin = 0;
    int partlength = s.size();
    if(partlength + curlength > MAXRECV)
      partlength = MAXRECV-curlength;

    bool done = false;

    bool longsend = false;
    while(!done)
      {
      
	(*sendstring) << s.substr(partbegin,partlength);

	//std::cout << "SENDING " << std::endl << sendstring->str() << std::endl;
	

	int status = ::send ( m_sock, sendstring->str().c_str(), sendstring->str().size(), MSG_NOSIGNAL );
	
	if (status <= 0)//( status == -1 )
	  {	
#ifdef WIN32
	    latesterror = WSAGetLastError();
#else
	    latesterror = errno;
#endif
	    return false;
	  }
	
	partlength -= sendstring->str().size()-status;

	if ( status != sendstring->str().size() )
	  cerr << "WARNING: sent only " << status << " of " << sendstring->str().size() << endl;
	    
	
	if(partbegin+partlength >= s.size())
	  done = true;
	else
	  {
	    if(!longsend)
	      {
		cout << endl;
		longsend = true;
	      }
	    cout << "\r sent " << partbegin+partlength << "/" << s.size() << flush;
	    delete sendstring;
	    sendstring = new std::ostringstream;
	    partbegin += partlength;
	    partlength = MAXRECV;
	    if(partbegin+partlength > s.size())
	      partlength = s.size() - partbegin;
	  }
      }
    if(longsend)
      cout << "\r sent "<< s.size() << "/"<< s.size() << endl;

    delete sendstring;
    return true;
  }


  int Socket::recv ( std::string& s ) const
  {
    char buf [ MAXRECV + 1 ];

    s = "";

    memset ( buf, 0, MAXRECV + 1 );

    int status = ::recv ( m_sock, buf, MAXRECV, 0 );

    if ( status == -1 )
      {	  
#ifdef WIN32
	latesterror = WSAGetLastError();
#else
	latesterror = errno;
#endif
	printf("status == -1  errno == %d in Socket::recv\n", latesterror);
	return 0;
      }
    else if ( status == 0 )
      return 0;


    std::string sizestring;
    int startpos;
    for(startpos=0; startpos<MAXRECV+1 && buf[startpos] != ' '; startpos++)
      sizestring += buf[startpos];
  
    //cout << "sizestring " << sizestring << endl;

    int size = atoi(sizestring.c_str());

    s.reserve(size);

    for(int i=startpos; i<MAXRECV+1 && buf[i]; i++)
      s += buf[i];
	  
    if(buf[MAXRECV])
      cerr << "WARNING: buffer totally filled" << endl;

    //cout << "RECEIVING " << s << endl;

    bool longreceive = false;
    while(s.size() < size)
      {
	//cout << "size " << size << " s.size() " << s.size() << " s " << s << endl;
	if(!longreceive)
	  {
	    cout << endl;
	    longreceive = true;
	  }
	
	memset ( buf, 0, MAXRECV + 1 );
	int status = ::recv ( m_sock, buf, MAXRECV, 0 );
	cout << "\r received " << s.size() << "/" << size << flush;

	if ( status == -1 )
	  {
#ifdef WIN32
	    latesterror = WSAGetLastError();
#else
	    latesterror = errno;
#endif
	    printf("status == -1  errno == %d in Socket::recv\n", latesterror);
	    return 0;
	  }
	else if ( status == 0 )
	  return 0;

	for(int i=0; i<MAXRECV+1 && buf[i]; i++)
	  s += buf[i];

	//cout << "RECEIVING " << s << endl;
	
	if(buf[MAXRECV])
	  cerr << "WARNING: buffer totally filled" << endl;

      }
    if(longreceive)
      cout << "\r received " << size << "/" << size << endl;

    //std::cout << "RECV COMPLETED" << std::endl << s << std::endl << "=========" << std::endl;


    return status;
  
  }




  bool Socket::connect ( const std::string host, const int port )
  {
    if ( ! is_valid() ) return false;

    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons ( port );

#ifdef WIN32
    struct sockaddr_storage addr;
    int len = sizeof(addr);

    char * dummych; dummych = new char[host.size()+1]; strcpy(dummych,host.c_str());
    int status = WSAStringToAddress(dummych, AF_INET, NULL,(LPSOCKADDR)&addr, &len);
    delete [] dummych;
    latesterror = WSAGetLastError();
    memcpy(&(m_addr.sin_addr),&((struct sockaddr_in *) &addr)->sin_addr,4);

    if(status != 0) status = -1;
//     if ( status != 0)
//       {
// 	cout << GetLatestError() << endl;
// 	return false;
//       }
#else

    int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );

    //if ( errno == EAFNOSUPPORT ) return false;
#endif
    
    if(status != 1)
      {
	// trying "real name"
	struct hostent *shost;
	shost = gethostbyname(host.c_str());
	if(!shost)
	  return false;
	m_addr.sin_addr = *(struct in_addr*) shost->h_addr;
      }
    


    status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );
    
    if ( status == 0 )
      return true;
    else
      {
#ifdef WIN32
	latesterror = WSAGetLastError();
#else
	latesterror = errno;
#endif
	return false;
      }
  }

  void Socket::set_non_blocking ( const bool b )
  {
#ifdef WIN32
    cerr << "Socket::set_non_blocking not yet implemented for Windows" << endl;
    exit(10);
#else
    int opts;

    opts = fcntl ( m_sock,
		   F_GETFL );

    if ( opts < 0 )
      {
	return;
      }

    if ( b )
      opts = ( opts | O_NONBLOCK );
    else
      opts = ( opts & ~O_NONBLOCK );

    fcntl ( m_sock,
	    F_SETFL,opts );
#endif
  }


  std::string Socket::GetLatestError(void)
  {
#ifdef WIN32
    if(latesterror == WSAEADDRINUSE)
      return "Address already in use";
    if(latesterror == WSAECONNABORTED)
      return "Software caused connection abort";
    if(latesterror == WSAECONNREFUSED)
      return "Connection refused";
    if(latesterror == WSAECONNRESET)
      return "Connection reset by peer";
    if(latesterror == WSAEDESTADDRREQ)
      return "Destination address required";
    if(latesterror == WSAEHOSTUNREACH)
      return "No route to host";
    if(latesterror == WSAEMFILE)
      return "Too many open files";
    if(latesterror == WSAENETDOWN)
      return "Network is down";
    if(latesterror == WSAENETRESET)
      return "Network dropped connection";
    if(latesterror == WSAENOBUFS)
      return "No buffer space available";
    if(latesterror == WSAENETUNREACH)
      return "Network is unreachable";
    if(latesterror == WSAETIMEDOUT)
      return "Connection timed out";
    if(latesterror == WSAHOST_NOT_FOUND)
      return "Host not found";
    if(latesterror == WSASYSNOTREADY)
      return "Network sub-system is unavailable";
    if(latesterror == WSANOTINITIALISED)
      return "WSAStartup() not performed";
    if(latesterror == WSANO_DATA)
      return "Valid name, no data of that type";
    if(latesterror == WSANO_RECOVERY)
      return "Non-recoverable query error";
    if(latesterror == WSATRY_AGAIN)
      return "Non-authoritative host found";
    if(latesterror == WSAVERNOTSUPPORTED)
      return "Wrong WinSock DLL version";
#else
    if(latesterror == EACCES)
      return "The requested address is protected, and the current user has inadequate permission to access it.";
    if(latesterror == EADDRINUSE)
      return "The specified address is already in use.";
    if(latesterror == EADDRNOTAVAIL)
      return "The specified address is invalid or not available from the local machine, or for AF_CCITT sockets which use wild card addressing, the specified address space overlays the address space of an existing bind.";
    if(latesterror == EAFNOSUPPORT)
      return "The specified address is not a valid address for the address family of this socket.";
    if(latesterror == EBADF)
      return "no valid file descriptor";
    if(latesterror == EDESTADDRREQ)
      return "No addr parameter was specified.";
    if(latesterror == EFAULT)
      return "addr is not a valid pointer.";
    if(latesterror == EINVAL)
      return "The socket is already bound to an address, the socket has been shut down, addrlen is a bad value, or an attempt was made to bind() an AF_UNIX socket to an NFS-mounted (remote) name.";
    if(latesterror == ENETDOWN)
      return "The x25ifname field name specifies an interface that was shut down, or never initialized, or whose Level 2 protocol indicates that the link is not working: Wires might be broken, the interface hoods on the modem are broken, the modem failed, the phone connection failed (this error can be returned by AF_CCITT only), noise interfered with the line for a long period of time.";
    if(latesterror == ENETUNREACH)
      return "The X.25 Level 2 protocol is down. The X.25 link is not working: Wires might be broken, or connections are loose on the interface hoods at the modem, the modem failed, or noise interfered with the line for an extremely long period of time.";
    if(latesterror == ENOBUFS)
      return "No buffer space is available. The bind() cannot complete.";
    if(latesterror == ENOMEM)
      return "No memory is available. The bind() cannot complete.";
    if(latesterror == ENODEV)
      return "The x25ifname field name specifies a nonexistent interface. (This error can be returned by AF_CCITT only.)";
    if(latesterror == ENOTSOCK)
      return "s is a valid file descriptor, but it is not a socket.";
    if(latesterror == EOPNOTSUPP)
      return "The socket referenced by s does not support address binding.";
    if(latesterror == EISCONN)
      return "The connection is already bound. (AF_VME_LINK.)";
    if(latesterror == EOPNOTSUPP)
      return "The socket is not of a type that supports the operation.";
    if(latesterror == ECONNREFUSED)
      return "No one listening on the remote address.";
    if(latesterror == ETIMEDOUT)
      return "Timeout  while  attempting  connection. The server may be too busy to accept new connections. Note that for IP sockets the timeout may be very long when syncookies are enabled on the server.";
    if(latesterror == ENETUNREACH)
      return "Network is unreachable.";
    if(latesterror == EINPROGRESS)
      return "The  socket  is  non-blocking and the connection cannot be completed immediately.  It is possible to select(2) or poll(2) for completion by selecting  the  socket  for  writing. After  select  indicates  writability,  use getsockopt(2) to read the SO_ERROR option at level SOL_SOCKET to determine whether connect completed successfully (SO_ERROR is  zero) or  unsuccessfully (SO_ERROR is one of the usual error codes listed here, explaining the reason for the failure).";
    if(latesterror == EALREADY)
      return "The socket is non-blocking and a previous connection attempt has not yet been completed.";
    if(latesterror == EAGAIN)
      return "No  more  free local ports or insufficient entries in the routing cache. For PF_INET see the net.ipv4.ip_local_port_range sysctl in ip(7) on how to increase the number of  local ports.";
    if(latesterror == EPERM)
      return "The  user  tried  to  connect to a broadcast address without having the socket broadcast flag enabled or the connection request failed because of a local firewall rule.";
#endif
    ostringstream ust;
    ust << "Unknown error (" << latesterror << ").";
    return ust.str();
  }


  ServerSocket::ServerSocket ( int port )
  {
    if ( ! Socket::create() )
      {
	std::cerr << "Error during create: " << GetLatestError() << std::endl;
	throw SocketException ( "Could not create server socket." );
      }

    if ( ! Socket::bind ( port ) )
      {
	std::cerr << "Error during bind: " << GetLatestError() << std::endl;
	throw SocketException ( "Could not bind to port." );
      }

    if ( ! Socket::listen() )
      {
	std::cerr << "Error during listen: " << GetLatestError() << std::endl;
	throw SocketException ( "Could not listen to socket." );
      }

  }

  ServerSocket::ServerSocket ()
    : Socket()
  {
    ;
  }

  ServerSocket::~ServerSocket()
  {
  }


  const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
  {
    if ( ! Socket::send ( s ) )
      {
	throw SocketException ( "Could not write to socket." );
      }

    return *this;

  }


  const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
  {
    if ( ! Socket::recv ( s ) )
      {
	throw SocketException ( "Could not read from socket." );
      }

    return *this;
  }

  void ServerSocket::accept ( ServerSocket& sock )
  {
    if ( ! Socket::accept ( sock ) )
      {
	throw SocketException ( "Could not accept socket." );
      }
  }





  ClientSocket::ClientSocket ( int port, std::string host )
  {
    if ( ! Socket::create() )
      {
	printf("Could not create client socket.\n" );
	throw SocketException ( "Could not create client socket." );
      }

    if ( ! Socket::connect ( host, port ) )
      {
	printf( "Could not bind to port.\n" );
	throw SocketException ( "Could not bind to port." );
      }

  }


  const ClientSocket& ClientSocket::operator << ( const std::string& s ) const
  {
    if ( ! Socket::send ( s ) )
      {
	throw SocketException ( "Could not write to socket." );
      }

    return *this;

  }


  const ClientSocket& ClientSocket::operator >> ( std::string& s ) const
  {
    if ( ! Socket::recv ( s ) )
      {
	throw SocketException ( "Could not read from socket." );
      }

    return *this;
  }


  
  ServerInfo::ServerInfo()
  {
    host = "";
    port = 0;
    clientid = -1;
  }

  ServerInfo & ServerInfo::operator= ( const ServerInfo & s )
  {
    host = s.host; port = s.port; clientid = s.clientid;
    return *this;
  }

  bool operator== ( const ServerInfo & s1, const ServerInfo & s2 )
  {
    return s1.host==s2.host && s1.port==s2.port;
  }

}



#endif
