#ifndef _SOCKETMANAGER_HPP
#define _SOCKETMANAGER_HPP


#include <meshing.hpp>
#include <csg.hpp>
#include <myadt.hpp>
#include "sockets.hpp"


namespace netgen
{
  using namespace netgen;

  
  class JobDescriptor 
  {
  public:
    int userid;

    SocketCommand command;

    bool tempfile_input;
    string filename_input;
    string buffer_input;

    bool tempfile_output;
    string filename_output;
    string buffer_output;

  public:
    JobDescriptor & operator= (const JobDescriptor & jd);
    JobDescriptor(const JobDescriptor & jd){*this = jd;}
    JobDescriptor();
    ~JobDescriptor();
  };




  
  class ServerSocketUser;

  class ServerSocketManager
  {
  private:
    ARRAY<ServerSocketUser *> users;

    ARRAY<SocketCommand> commands;
    ARRAY<bool> isfastcommand;
    ARRAY<int> command2user;

    int fpprec;
    bool closeme;
    int port;

    int latestclientid;

    int blocked_for_user;

    int largeinputsize;
    
    ARRAY < JobDescriptor* > jobqueue;
    ARRAY < JobDescriptor* > answers;

    NgMutex jqmutex;
    NgMutex answermutex;

    bool worker_running;

    NgMutex jobinfomutex;
    int currentjobuser;
    SocketCommand currentjobcommand;
    string currentjobstatustext;

    
  private:
    void GetTempFileName(string & filename);

  public:
    ServerSocketManager ();

    ServerSocketManager( const int aport );

    void Init( const int aport );

    void RegisterUser(ServerSocketUser & user);

    int ReceiveCommand(const SocketCommand & command, istream & input, ostream & output,
		       string & infotext, NgMutex & infotextmutex);

    void Run( void );
    void WorkOnJobs( void );

    bool Good ( void ) const;

    void ResetLargeInputBound(const int bound) { largeinputsize = bound; }

    bool WorkerRunning(void) const {return worker_running;}
    
  };


  
  class ServerSocketUser
  {
  protected:
    ARRAY<SocketCommand> commands;
    ARRAY<bool> isfastcommand;
  public:
    ServerSocketUser();
    ServerSocketUser(ServerSocketManager & manager);

    virtual void RegisterAtManager(void);

    const ARRAY<SocketCommand> & GetCommands(void) const { return commands; }
    bool IsFastCommand(const int i) const {return (isfastcommand.Size() > i) && isfastcommand[i];}

    virtual int ReceiveCommand(const SocketCommand & command, istream & input, ostream & output,
			       string & infotext, NgMutex & infotextmutex) = 0;
  };


#ifndef ONLYBASECLASS
  class ServerSocketUserNetgen : public ServerSocketUser
  {
    AutoPtr < Mesh > & mesh_ptr;
    AutoPtr<CSGeometry> & geometry_ptr;
  public:
    ServerSocketUserNetgen(ServerSocketManager & manager, 
			   AutoPtr<Mesh> & amesh_ptr, AutoPtr<CSGeometry> & ageometry_ptr);
    virtual int ReceiveCommand(const SocketCommand & command, istream & input, ostream & output,
			       string & infotext, NgMutex & infotextmutex);
  };
#endif

}






#endif // _SOCKETMANAGER_HPP
