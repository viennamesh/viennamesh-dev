#ifndef _SOCKETS_HPP
#define _SOCKETS_HPP

// based on the socket class by Rob Tougher
// http://www.linuxgazette.com/issue74/tougher.html


#include <iostream>
#include <sys/types.h>

#ifdef WIN32

#include <winsock.h>

#define socklen_t int
#define MSG_NOSIGNAL 0

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SOCKET int

#endif

#include <string>



#define SOCKETCOMMANDLENGTH 2

#define SOCKETNAMESPACE netgen

namespace SOCKETNAMESPACE
{
  using namespace SOCKETNAMESPACE;


  class SocketCommand
  {
  private:
    char command[SOCKETCOMMANDLENGTH];

  public:
    SocketCommand & operator= (const SocketCommand & sc);
    SocketCommand & operator= (const char * sc);

    SocketCommand();
    SocketCommand(const SocketCommand & sc){*this = sc;}
    SocketCommand(const char * sc){*this = sc;}
    

    const char* Get(void) const {return command;}
    std::string GetString(void) const;
    char Get(const int i) const {return command[i];}
    void Set(const int i, const char c) {command[i] = c;}

    static int CommandLength(void) { return SOCKETCOMMANDLENGTH; }
  };

  std::ostream & operator<<(std::ostream & s, const SocketCommand & sc);
  std::istream & operator>>(std::istream & s, SocketCommand & sc);
  bool operator==(const SocketCommand & sc1, const SocketCommand & sc2);
  

  const SocketCommand ID("ID");

  const SocketCommand EMPTY("--");
  
  const SocketCommand NOTOK("-1");

  const SocketCommand OK("00");
  const SocketCommand CLOSE("01");
  const SocketCommand SETFPPREC("02");
  //const SocketCommand WRITESOCKETTYPE("03");
  const SocketCommand SENDSERVERSTATUS("03");
  //const SocketCommand SENDWORKINGSTATUS("04");
  const SocketCommand EXIT("05");
  const SocketCommand EMERGENCYEXIT("06");
  const SocketCommand SENDNEWID("07");
  
  const SocketCommand BLOCK("08");
  const SocketCommand UNBLOCK("09");

  const SocketCommand SENDQUEUESTATUS("0a");
  const SocketCommand SENDCURRENTJOBSTATUS("0b");

  const SocketCommand SENDANSWER("0c");

  const SocketCommand RECVMESH("10");

  const SocketCommand RECVPDE("20");
  const SocketCommand WRITESOLUTION("21");
  const SocketCommand SOLVEPDE("22");

  const SocketCommand KILLCURRENTJOB("23");


  const SocketCommand SETPOINT2D("2");
  const SocketCommand SETPOINT3D("3");
  const SocketCommand WRITEVALATPOINT("4");
  const SocketCommand WRITEVARIABLE("5");
  const SocketCommand RUNBVP("6");
  const SocketCommand SELECTGRIDFUNCTION("7");
  const SocketCommand SELECTBILINEARFORM("8");
  const SocketCommand SELECTLINEARFORM("9");
  const SocketCommand SELECTPRECONDITIONER("a");
  const SocketCommand SETMAXSTEPS("b");
  const SocketCommand SETPREC("c");
  const SocketCommand SETAPPLYD("d");
  const SocketCommand SETNOTAPPLYD("e");
  const SocketCommand SELECTFILECOEFFICIENTFUNCTION("f");

  const SocketCommand FILECOEFFICIENTFUNCTION_STARTWRITEIPS("g");
  const SocketCommand FILECOEFFICIENTFUNCTION_STOPWRITEIPS("h");
  const SocketCommand FILECOEFFICIENTFUNCTION_LOADVALUES("i");

  const SocketCommand FILECOEFFICIENTFUNCTION_STARTWRITEIPSWITHFILENAME("j");
  const SocketCommand FILECOEFFICIENTFUNCTION_STOPWRITEIPSWITHFILENAME("k");
  const SocketCommand FILECOEFFICIENTFUNCTION_LOADVALUESWITHFILENAME("l");


  const SocketCommand FILECOEFFICIENTFUNCTION_RESET("m");
  const SocketCommand ASSEMBLELINEARFORM("n");

  const SocketCommand SELECTFLUXGRIDFUNCTION("o");
  const SocketCommand RUNCALCFLUX("p");

  const SocketCommand SETUSEALL("q");
  const SocketCommand SETNOTUSEALL("r");

  const SocketCommand SETORDERSFILENAME("s");
  const SocketCommand SETIPFILENAME("t");
  const SocketCommand SETINTERPOLATIONFILENAME("u");
  
  const SocketCommand WRITEINTEGRATIONPOINTS("v");

  const SocketCommand BUILDINTERPOLATINGGRIDFUNCTION("w");


  const SocketCommand SAVESOLUTION("y");




  std::string GetPlainText(const SocketCommand & command);



  const int MAXHOSTNAME = 200;
  const int MAXCONNECTIONS = 5;
  const int MAXRECV = 500;

  class Socket
  {
  private:

    int m_sock;
    sockaddr_in m_addr;
  
    mutable int latesterror;

//   private:
//     int connect_time ( int sock, struct sockaddr * addr, int size_addr, const int timeout );


  public:
    Socket();
    virtual ~Socket();

    // Server initialization
    bool create();
    bool bind ( const int port );
    bool listen() const;
    bool accept ( Socket& ) const;

    // Client initialization
    bool connect ( const std::string host, const int port );

    // Data Transimission
    bool send ( const std::string & s ) const;
    bool send ( const int s ) const;

    int recv ( std::string& ) const;


    void set_non_blocking ( const bool );

    bool is_valid() const { return m_sock != -1; }

    virtual std::string GetLatestError(void);


  };




  class SocketException
  {
  private:
    std::string m_s;
    int problemclass;
  
  public:
    SocketException ( const std::string s, const int pclass = 0) : m_s ( s ), problemclass(pclass) {};
    ~SocketException (){};

    std::string Description(void) { return m_s; }
    int Problemclass(void) { return problemclass; }


  };



  class ServerSocket : private Socket
  {
  public:

    ServerSocket ( int port );
    ServerSocket ();
    virtual ~ServerSocket();

    const ServerSocket& operator << ( const std::string& ) const;
    const ServerSocket& operator >> ( std::string& ) const;

    void accept ( ServerSocket& );

  };



  class ClientSocket : private Socket
  {
  public:
    ClientSocket ( int port, std::string host="localhost" );
    virtual ~ClientSocket(){};

    const ClientSocket& operator << ( const std::string& ) const;
    const ClientSocket& operator >> ( std::string& ) const;

  };



  class ServerInfo
  {
  public:
    std::string host;
    int port;
    int clientid;
  public:
    ServerInfo();
    ServerInfo & operator= ( const ServerInfo & s );
    ServerInfo ( const ServerInfo & s ) { *this = s; }
  };

  bool operator== ( const ServerInfo & s1, const ServerInfo & s2 );


}

#endif // _SOCKETS_HPP

