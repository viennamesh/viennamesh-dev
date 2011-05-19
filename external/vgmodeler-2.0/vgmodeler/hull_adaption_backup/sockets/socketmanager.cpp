#ifdef SOCKETS


#include "socketmanager.hpp"
#include <stdio.h>
#include <sstream>



#ifdef WIN32

// Afx - Threads need different return - value:

static void* (*sfun)(void *);
unsigned int fun2 (void * val)
{
  sfun (val);
  return 0;
}

static CWinThread * workerthread = NULL;
void RunParallel ( void* (*fun)(void *), void * in)
{
  sfun = fun;
  workerthread = AfxBeginThread (fun2, in);
}

#else

#include <pthread.h>

static pthread_t workerthread = 0;
void RunParallel ( void * (*fun)(void *), void * in)
{
  pthread_attr_t attr;
  pthread_attr_init (&attr);
  // the following call can be removed if not available:
  pthread_attr_setstacksize(&attr, 1000000);  
  pthread_create (&workerthread, &attr, fun, in);
}


#endif


namespace netgen
{
  using namespace netgen;
  using namespace std;

  JobDescriptor::JobDescriptor()
  {
    userid = -1;
    command = "";
    tempfile_input = tempfile_output = false;
    filename_input = filename_output = "";
    buffer_input = buffer_output = "";

  }

  JobDescriptor::~JobDescriptor()
  {
    //     if(tempfile_input)
    //       remove(filename_input.c_str());
    //     if(tempfile_output)
    //       remove(filename_output.c_str());
  }

  
  JobDescriptor & JobDescriptor::operator= (const JobDescriptor & jd)
  {
    command = jd.command;

    userid = jd.userid;

    tempfile_input = jd.tempfile_input;
    filename_input = jd.filename_input;
    buffer_input = jd.buffer_input;
    
    tempfile_output = jd.tempfile_output;
    filename_output = jd.filename_output;
    buffer_output = jd.buffer_output;
    
    return *this;
  }
  

  


  ServerSocketManager::ServerSocketManager ()
  {
    port = -1;
    largeinputsize = 131072;
    worker_running = false;
    currentjobuser = 0;
    currentjobstatustext = "";
  }

  ServerSocketManager::ServerSocketManager( const int aport )
  {
    Init(aport);
    largeinputsize = 131072;
    worker_running = false;
    currentjobuser = 0;
    currentjobstatustext = "";
  }

  void ServerSocketManager::Init( const int aport )
  {

    port = aport;
    latestclientid = 0;
    blocked_for_user = -1;

    users.Append(NULL);

    commands.Append(CLOSE);
    isfastcommand.Append(false);
    command2user.Append(0);
    commands.Append(EXIT);
    isfastcommand.Append(false);
    command2user.Append(0);
    commands.Append(EMERGENCYEXIT);
    isfastcommand.Append(true);
    command2user.Append(0);
    commands.Append(OK);
    isfastcommand.Append(true);
    command2user.Append(0);
    commands.Append(SETFPPREC);
    isfastcommand.Append(false);
    command2user.Append(0);
    commands.Append(SENDSERVERSTATUS);
    isfastcommand.Append(true);
    command2user.Append(0);
    commands.Append(SENDQUEUESTATUS);
    isfastcommand.Append(true);
    command2user.Append(0);
    commands.Append(SENDCURRENTJOBSTATUS);
    isfastcommand.Append(true);
    command2user.Append(0);
    commands.Append(KILLCURRENTJOB);
    isfastcommand.Append(true);
    command2user.Append(0);
    commands.Append(SENDNEWID);
    isfastcommand.Append(true);
    command2user.Append(0);
    commands.Append(BLOCK);
    isfastcommand.Append(false);
    command2user.Append(0);
    commands.Append(UNBLOCK);
    isfastcommand.Append(false);
    command2user.Append(0);
    commands.Append(SENDANSWER);
    isfastcommand.Append(true);
    command2user.Append(0);


    fpprec = -1;
    closeme = false;
  }
  
  
  
  void ServerSocketManager :: GetTempFileName(string & filename)
  {    
    bool found = false;
    int i = 0;
    while(!found)
      {
	i++;
	ostringstream fn;
	fn << "netgen." << i << ".tmp";

	ifstream testit(fn.str().c_str());
	if(!testit)
	  {
	    filename = fn.str();
	    found = true;
	  }
      }
  }
  


  void ServerSocketManager::RegisterUser(ServerSocketUser & user)
  {
    if(port < 0)
      {
	throw NgException("ServerSocket Users can only be registered if netgen is started with -serversocket=...");
      }

    const ARRAY<SocketCommand> & usercommands = user.GetCommands();

    int numusers = users.Size();
    users.Append(&user);
    

    for(int i=0; i<usercommands.Size(); i++)
      {
	if(commands.Contains(usercommands[i]))
	  {
	    string err = "Command ID ";
	    err += usercommands[i].Get();
	    err += " already used";
	    throw NgException(err.c_str());
	    return;
	  }
	
	commands.Append(usercommands[i]);
	isfastcommand.Append(user.IsFastCommand(i));
	command2user.Append(numusers);
      }
  }


  void * WorkerDummy( void * manager)
  {
    ServerSocketManager & socketmanager = *static_cast<ServerSocketManager *>(manager);
    
    socketmanager.WorkOnJobs();

    workerthread = 0;
    return NULL;
  }


  void KillRunningThread(ServerSocketManager & manager)
  {
    if(workerthread && manager.WorkerRunning())
      {
	multithread.terminate = 1;
	cout << endl << "KILLING THREAD (please be patient)" << endl << endl;
#ifdef WIN32
	WaitForSingleObject(workerthread->m_hThread,INFINITE);
#else
	pthread_join(workerthread,NULL);
#endif
	multithread.terminate = 0;
      }
  }

  
  void ServerSocketManager::WorkOnJobs(void)
  {
    
    NgLock * lock;

    bool done = false;

    worker_running = true;

    while(!done)
      {
	JobDescriptor currentjob;

	
	lock = new NgLock(jqmutex,true);


	if(jobqueue.Size() == 0)
	  done = true;
	else
	  {
	    int pos = 0;
	    if(blocked_for_user > 0)
	      {
		done = true;
		while(pos < jobqueue.Size() && jobqueue[pos]->userid != blocked_for_user) pos++;
	      }

	    if(pos < jobqueue.Size())
	      {
		done = false;
		currentjob = *(jobqueue[pos]);
		for(int i=pos; i<jobqueue.Size()-1; i++)
		  *(jobqueue[i]) = *(jobqueue[i+1]);
		delete jobqueue.Last();
		jobqueue.DeleteLast();
	      }
	  }
	delete lock;


	if(!done)
	  {
	    istream * inputdata;
	    ifstream inputfile;
	    istringstream inputstring(currentjob.buffer_input);
	    if(currentjob.tempfile_input)
	      {
		inputfile.open(currentjob.filename_input.c_str());
		inputdata = &inputfile;
	      }
	    else
	      inputdata = &inputstring;
	    
	    ostringstream outputdata;

	    bool found = false;
	    
	    for(int i=0; !found && i<commands.Size(); i++)
	      {
		if(currentjob.command == commands[i])
		  {
		    found = true;
		    lock = new NgLock(jobinfomutex,true);
		    currentjobuser = currentjob.userid;
		    currentjobcommand = currentjob.command;
		    ostringstream newjoboutput;
		    newjoboutput << "* STARTING JOB FOR CLIENT " << currentjobuser << " *";
		    for(int j=0; j<newjoboutput.str().size(); j++)
		      cout << "*";
		    cout << endl << newjoboutput.str() << endl;
		    for(int j=0; j<newjoboutput.str().size(); j++)
		      cout << "*";
		    cout << endl;
		    delete lock;
		    
		    if(command2user[i] == 0)
		      found = (ReceiveCommand(currentjob.command,*inputdata,outputdata,
					      currentjobstatustext,jobinfomutex) == 0);
		    else
		      found = (users[command2user[i]]->ReceiveCommand(currentjob.command,*inputdata,outputdata,
								      currentjobstatustext,jobinfomutex) == 0);
		  }
	      }

	    if(currentjob.tempfile_input)
	      {
		inputfile.close();
		remove(currentjob.filename_input.c_str());
	      }
	    else
	      currentjob.buffer_input = "";

	    if(outputdata.str().size() > 0)
	      {
		if(outputdata.str().size() > largeinputsize)
		  {
		    currentjob.tempfile_output = true;
		    GetTempFileName(currentjob.filename_output);
		    ofstream outfile(currentjob.filename_output.c_str());
		    		    
		    outfile << outputdata.str() << endl;
		    outfile.close();
		  }
		else
		  {
		    currentjob.tempfile_output = false;
		    currentjob.buffer_output = outputdata.str();
		  }
		lock = new NgLock(answermutex,true);
		answers.Append(new JobDescriptor(currentjob));
		delete lock;

	      }
	
	    if(!found)
	      {
		string err = "Received unknown command "; err += currentjob.command.Get();
		throw SocketException(err.c_str(),1);
	      } 
	  }
	
      }

    worker_running = false;
    lock = new NgLock(jobinfomutex,true);
    currentjobuser = 0;
    currentjobstatustext = "";
    delete lock;
  }

  
  void ServerSocketManager::Run(void)
  {
    if(port < 0)
      return;

    cout << endl << "Starting socket server on port " << port << endl;


    
    NgLock * jqlock;
    JobDescriptor * newjob;
    
    try
      {
	ServerSocket server(port);
	
	closeme = false;

	while ( !closeme )
	  {
	    ServerSocket new_sock;
	    server.accept ( new_sock );
	    
	    try
	      {
		while ( !closeme )
		  {
		    string data;
		    new_sock >> data;
		    
		    istringstream inputdata(data);
		    ostringstream outputdata;
		    
		    if(fpprec >= 0) outputdata.precision(fpprec);
		    		    
		    
		    newjob = new JobDescriptor;
		    inputdata >> newjob->command;

		    int userid = -1;
		    if(newjob->command == ID)
		      {
			inputdata >> userid;
			inputdata >> newjob->command;
		      }

		    
		    bool found = false;
		    bool fastcommand;
	    
		    for(int i=0; !found && i<commands.Size(); i++)
		      {
			if(newjob->command == commands[i])
			  {
			    found = true;
			    fastcommand = isfastcommand[i];
			    if(fastcommand)
			      {
				string dummytext;
				if(command2user[i] == 0)
				  found = (ReceiveCommand(newjob->command,inputdata,outputdata,
							  dummytext,jobinfomutex) == 0);
				else
				  found = (users[command2user[i]]->ReceiveCommand(newjob->command,inputdata,outputdata,
										  dummytext,jobinfomutex) == 0);
			      }
			  }
		      }
		    if(!fastcommand)
		      {
			jqlock = new NgLock(jqmutex,true);

						
			jobqueue.Append(newjob);
			JobDescriptor & nj = *(jobqueue.Last());
			nj.userid = userid;
			if(data.size() < largeinputsize)
			  {
			    nj.tempfile_input = false;
			    nj.buffer_input = "";
			    ostringstream bufferstr; 
			    char c;
			    while(inputdata.get(c)) 
			      bufferstr.put(c);

			    nj.buffer_input = bufferstr.str();
			  }
			else
			  {
			    nj.tempfile_input = true;
			    GetTempFileName(nj.filename_input);
			    ofstream * outfile; outfile = new ofstream(nj.filename_input.c_str());
			    char c;
			    while(inputdata.get(c))
			      outfile->put(c);
			    			
			    outfile->close();
			    delete outfile;
			  }
			
			
			delete jqlock;
		      }
		    
		    outputdata << " " << OK;
		    
		    new_sock << outputdata.str();

		    //cout << "outputdata.str() \"" << outputdata.str() << "\"" << endl;

		    if(!worker_running && jobqueue.Size() > 0)
		      RunParallel(WorkerDummy,this);
		  }
	      }
	    catch ( SocketException & e) 
	      { 
		if(e.Problemclass() > 0) 
		  cerr << "SocketError: " << e.Description() << endl; 
	      }
	  }
      }
    catch ( SocketException& e )
      {
	std::cerr << "Exception was caught:" << e.Description() << endl
		  << "closing socket" << endl;
      }
  }


  
  int ServerSocketManager::ReceiveCommand(const SocketCommand & command, istream & input, ostream & output,
					  string & infotext, NgMutex & infotextmutex)
  {
    NgLock * lock; lock = new NgLock(infotextmutex,true);
    infotext = "idle";
    delete lock;

    if ( command == CLOSE )
      {
	closeme = true;
      }
    else if ( command == EXIT )
      {
	exit(0);
      }
    else if ( command == EMERGENCYEXIT )
      {
	exit(10);
      }
    else if ( command == SETFPPREC )
      {
	input >> fpprec;
      }
    else if ( command == SENDSERVERSTATUS )
      {
	if(Good())
	  output << "1 ";
	else
	  output << "-1 ";
      }
    else if ( command == SENDQUEUESTATUS )
      {
	NgLock * lock;

	output << blocked_for_user << " ";
	
	lock = new NgLock(jqmutex,true);
	output << jobqueue.Size() << " ";
	for(int i=0; i<jobqueue.Size(); i++)
	  output << jobqueue[i]->userid << " " << jobqueue[i]->command.GetString() << " ";
	delete lock;

	lock = new NgLock(answermutex,true);
	output << answers.Size() << " ";
	for(int i=0; i<answers.Size(); i++)
	  output << answers[i]->userid << " " << answers[i]->command.GetString() << " ";
	  
	delete lock;
      }
    else if ( command == SENDCURRENTJOBSTATUS )
      {
	NgLock * lock;
	lock = new NgLock(jobinfomutex,true);
	MyStr auxstatus;
	double percentage;
	GetStatus(auxstatus,percentage);
	if(auxstatus != MyStr("idle"))
	  {
	    ostringstream sstream;
	    sstream << auxstatus << ", " << int(percentage) << "% done";
	    currentjobstatustext = sstream.str();
	  }
	    
	output << currentjobuser << " " << currentjobcommand.GetString() << " "
	       << currentjobstatustext << " EOT ";
	delete lock;
      }

    else if ( command == KILLCURRENTJOB )
      {
	KillRunningThread(*this);
      }

    else if ( command == SENDNEWID )
      {
	latestclientid++;
	output << latestclientid << " ";
      }
    else if ( command == BLOCK )
      {
	input >> blocked_for_user;
	cout << "Server blocked for user " << blocked_for_user <<endl;
      }
    else if ( command == UNBLOCK )
      {
	blocked_for_user = -1;
      }
    else if ( command == SENDANSWER )
      {
	int userid;
	SocketCommand command;
	
	input >> userid >> command;

	int pos;
	bool found = false;
	
	for(pos = 0; !found && pos < answers.Size(); pos++)
	  {
	    //cout << answers[pos]->userid << " ?=? " << userid << ", " << answers[pos]->command << " ?=? " << command << endl;
	    found = ((answers[pos]->userid == userid) && (answers[pos]->command == command));
	  }
	pos--;
	//cout << "found " << found << endl;

	if(found)
	  {
	    output << OK << " ";

	    if(answers[pos]->tempfile_output)
	      {
		ifstream tempfile(answers[pos]->filename_output.c_str());
		char c;
		while(tempfile.get(c)) output.put(c);
		remove(answers[pos]->filename_output.c_str());
	      }
	    else
	      {
		istringstream strstr(answers[pos]->buffer_output);
		char c;
		while(strstr.get(c)) output.put(c);
	      }
	    delete answers[pos];
	    for(int i=pos; i<answers.Size()-1; i++)
	      answers[i] = answers[i+1];
	    answers.SetSize(answers.Size()-1);

	  }
	else
	  output << NOTOK << " ";
      }
    else
      {
	return -1;
      }
    return 0;
  }

  bool ServerSocketManager :: Good ( void ) const
  {
    //cout << "port = " << port << ", closeme = " << closeme << endl;
    return (port > 0 && !closeme );
  }







  ServerSocketUser::ServerSocketUser()
  {
    //RegisterAtManager();
  }

  ServerSocketUser::ServerSocketUser(ServerSocketManager & manager)
  {
    //manager.RegisterUser(*this);
  }

  
  void ServerSocketUser::RegisterAtManager(void)
  {
    return;
  }



  ServerSocketUserNetgen :: ServerSocketUserNetgen(ServerSocketManager & manager, 
						   AutoPtr<Mesh> & amesh_ptr, AutoPtr<CSGeometry> & ageometry_ptr) :
    mesh_ptr(amesh_ptr), geometry_ptr(ageometry_ptr)
  {
    commands.Append(RECVMESH);
    isfastcommand.Append(false);
    manager.RegisterUser(*this);
  }



  int ServerSocketUserNetgen :: ReceiveCommand(const SocketCommand & command, istream & input, ostream & output,
					       string & infotext, NgMutex & infotextmutex)
  {
    if ( command == RECVMESH )
      {
	NgLock * lock; lock = new NgLock(infotextmutex,true);
	infotext = "receiving mesh";
	delete lock;
	
	mesh_ptr.Reset ( new Mesh() );

	mesh_ptr -> Load(input);
	if(input.good())
	  {
	    string auxstring;
	    input >> auxstring;
	    
	    if(auxstring == "csgsurfaces")
	      {
		if (geometry_ptr)
		  geometry_ptr.Reset (new CSGeometry (""));
		geometry_ptr -> LoadSurfaces(input);
	      }
	  }
      }
    else
      return -1;

    return 0;

  }
  

  

}
#endif
