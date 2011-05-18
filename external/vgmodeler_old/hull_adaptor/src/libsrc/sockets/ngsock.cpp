  
#ifdef SOCKETS
#include <string>
#include <sstream>
#include <iostream>
#include <myadt.hpp>

#include "incvis.hpp"
#include "sockets.hpp"
#include "meshing.hpp"
#include "csg.hpp"

namespace netgen
{
  extern AutoPtr<ClientSocket> clientsocket;
  //extern ARRAY< AutoPtr < ServerInfo > > servers;
  extern ARRAY< ServerInfo* > servers;
  extern AutoPtr<Mesh> mesh;
  extern AutoPtr<CSGeometry> geometry;



  int Ng_Socket (ClientData clientData,
		 Tcl_Interp * interp,
		 int argc, tcl_const char *argv[])
  {
    if(argc < 2)
      {
	Tcl_SetResult (interp, "wrong number of arguments", TCL_VOLATILE);
	return TCL_ERROR;
      }

    ostringstream output;
    string reply;

    int servernum = -1;
    if(argc >= 3) servernum = atoi(argv[2]);

    if (argc >= 3 && servernum < servers.Size() && servers[servernum]->clientid > 0)
      output << ID << " " << servers[servernum]->clientid << " ";
	


    if(strcmp(argv[1],"addserver") == 0 && argc >= 3)
      {
	try
	  {
	    if(argc >= 4)
	      clientsocket.Reset(new ClientSocket(atoi(argv[2]),
						  argv[3]));
	    else
	      clientsocket.Reset(new ClientSocket(atoi(argv[2])));
	  }	  
	catch( SocketException e)
	  {
	    char * dummy = new char[e.Description().size()+1];
	    strcpy(dummy,e.Description().c_str());
	    Tcl_SetResult (interp, dummy, TCL_VOLATILE);
	    clientsocket.Reset(NULL);
	    return TCL_ERROR;
	  }
	
	
	output << SENDSERVERSTATUS;
	(*clientsocket) << output.str();
	output.str("");
	(*clientsocket) >> reply;
	istringstream input(reply);

	int good;

	input >> good;

	if(good == 1)
	  {
	    

	    ServerInfo * auxserver = new ServerInfo;
	    if(argc >= 4)
	      auxserver->host = argv[3];
	    if(auxserver->host == "") auxserver->host = "localhost";

	    auxserver->port = atoi(argv[2]);
	    
	    servers.SetSize(servers.Size()+1);
	    servers.Last() = auxserver;
	  }
	else
	  {
	    char dummy[33];
	    strcpy(dummy,"Something wrong with the server");
	    Tcl_SetResult (interp, dummy, TCL_VOLATILE);
	    clientsocket.Reset(NULL);
	    return TCL_ERROR;
	  }
	
	clientsocket.Reset(NULL);
      }
    else if (strcmp(argv[1],"deletesocket") == 0 && argc >= 3)
      {
	int todelete = atoi(argv[2]);

	delete servers[todelete];

	servers.Delete(todelete);
      }
    else if (strcmp(argv[1],"sendmesh") == 0 && argc >= 3)
      {
	if(!mesh)
	  {
	    Tcl_SetResult (interp, "no mesh", TCL_VOLATILE);
	    return TCL_ERROR;
	  }

	try
	  {
	    clientsocket.Reset(new ClientSocket(servers[servernum]->port,
						servers[servernum]->host));
	  }
	catch( SocketException e)
	  {
	    char dummy[256];
	    strcpy(dummy,e.Description().c_str());
	    Tcl_SetResult (interp, dummy, TCL_VOLATILE);
	    return TCL_ERROR;
	  }
	

	output << RECVMESH << " ";
	mesh->Save(output);
	output << endl << endl << "endmesh" << endl << endl;
	if (geometry && geometry->GetNSurf()) geometry->SaveSurfaces(output);

	(*clientsocket) << output.str();
	output.str("");
	(*clientsocket) >> reply;

	clientsocket.Reset(NULL);
      }


    else if (strcmp(argv[1],"killcurrentjob") == 0 && argc >= 3)
      {
	try
	  {
	    clientsocket.Reset(new ClientSocket(servers[servernum]->port,
						servers[servernum]->host));
	  }
	catch( SocketException e)
	  {
	    char dummy[256];
	    strcpy(dummy,e.Description().c_str());
	    Tcl_SetResult (interp, dummy, TCL_VOLATILE);
	    return TCL_ERROR;
	  }

	output << KILLCURRENTJOB;
	(*clientsocket) << output.str();
	output.str("");
	(*clientsocket) >> reply;

	clientsocket.Reset(NULL);
      }    

    else if (strcmp(argv[1],"getid") == 0 && argc >= 3)
      {
	if(servers[servernum]->clientid != -1)
	  {
	    char dummy[17];
	    strcpy(dummy,"Have already ID");
	    Tcl_SetResult (interp, dummy, TCL_VOLATILE);
	    return TCL_ERROR;
	  }

	try
	  {
	    clientsocket.Reset(new ClientSocket(servers[servernum]->port,
						servers[servernum]->host));
	  }
	catch( SocketException e)
	  {
	    char dummy[256];
	    strcpy(dummy,e.Description().c_str());
	    Tcl_SetResult (interp, dummy, TCL_VOLATILE);
	    return TCL_ERROR;
	  }
	
	output << SENDNEWID;
	(*clientsocket) << output.str();
	output.str("");
	(*clientsocket) >> reply;
	istringstream input(reply);
	input >> servers[servernum]->clientid;

	
	ostringstream tclreply;
	tclreply << servers[servernum]->clientid;
	char* dummy;
	dummy = new char[tclreply.str().size()+1];
	strcpy(dummy,tclreply.str().c_str());
	Tcl_SetResult (interp, dummy, TCL_VOLATILE);

	clientsocket.Reset(NULL);
      }
    
    else if (strcmp(argv[1],"setid") == 0 && argc >= 4)
      {
	servers[servernum]->clientid = atoi(argv[3]);
      }

    else if (strcmp(argv[1],"blockserver") == 0 && argc >= 3)
      {
	if(servers[servernum]->clientid == -1)
	  {
	    char dummy[12];
	    strcpy(dummy,"Have no ID");
	    Tcl_SetResult (interp, dummy, TCL_VOLATILE);
	    return TCL_ERROR;
	  }

	try
	  {
	    clientsocket.Reset(new ClientSocket(servers[servernum]->port,
						servers[servernum]->host));
	  }
	catch( SocketException e)
	  {
	    char dummy[256];
	    strcpy(dummy,e.Description().c_str());
	    Tcl_SetResult (interp, dummy, TCL_VOLATILE);
	    return TCL_ERROR;
	  }
	output << BLOCK << " " << servers[servernum]->clientid;
	(*clientsocket) << output.str();
	output.str("");
	(*clientsocket) >> reply;
		
	clientsocket.Reset(NULL);
      } 
    else if (strcmp(argv[1],"unblockserver") == 0 && argc >= 3)
      {
	try
	  {
	    clientsocket.Reset(new ClientSocket(servers[servernum]->port,
						servers[servernum]->host));
	  }
	catch( SocketException e)
	  {
	    char * dummy = new char[e.Description().size()+1];
	    strcpy(dummy,e.Description().c_str());
	    Tcl_SetResult (interp, dummy, TCL_VOLATILE);
	    delete [] dummy;
	    return TCL_ERROR;
	  }
	
	output << UNBLOCK;
	(*clientsocket) << output.str();
	output.str("");
	(*clientsocket) >> reply;
	
	clientsocket.Reset(NULL);
      }
    else if (strcmp(argv[1],"sendjobstatus") == 0 && argc >= 3)
      {
	try
	  {
	    clientsocket.Reset(new ClientSocket(servers[servernum]->port,
						servers[servernum]->host));
	  }
	catch( SocketException e)
	  {
	    char dummy[256];
	    strcpy(dummy,e.Description().c_str());
	    Tcl_SetResult (interp, dummy, TCL_VOLATILE);
	    return TCL_ERROR;
	  }
	
	output << SENDCURRENTJOBSTATUS;
	(*clientsocket) << output.str();
	output.str("");
	(*clientsocket) >> reply;
	istringstream input(reply);

	int currentjob_user;
	SocketCommand currentjob_command;
	ostringstream currentjob_info_str;
	string currentjob_info = "";

	input >> currentjob_user >> currentjob_command;
	while(currentjob_info != "EOT")
	  {
	    currentjob_info_str << currentjob_info << " ";
	    input >> currentjob_info;
	  }
	currentjob_info = currentjob_info_str.str();

	ostringstream tclreply;
	tclreply << currentjob_user << " " << GetPlainText(currentjob_command)
		 << " " << currentjob_info;
	char* dummy;
	dummy = new char[tclreply.str().size()+1];
	strcpy(dummy,tclreply.str().c_str());
	Tcl_SetResult (interp, dummy, TCL_VOLATILE);


	clientsocket.Reset(NULL);
      }
    
    else if (strcmp(argv[1],"sendqueuestatus") == 0 && argc >= 3)
      {
	try
	  {
	    clientsocket.Reset(new ClientSocket(servers[servernum]->port,
						servers[servernum]->host));
	  }
	catch( SocketException e)
	  {
	    char dummy[256];
	    strcpy(dummy,e.Description().c_str());
	    Tcl_SetResult (interp, dummy, TCL_VOLATILE);
	    return TCL_ERROR;
	  }


	output << SENDQUEUESTATUS;
	(*clientsocket) << output.str();
	output.str("");
	(*clientsocket) >> reply;
	istringstream input(reply);

	int server_blocked_for_user;
	ostringstream tclreply;

	input >> server_blocked_for_user;
	tclreply << server_blocked_for_user << " ";


	for(int loop=0; loop<2; loop++)
	  {
	    int size;
	    input >> size;
	    tclreply << size << " ";

	    for(int i=0; i<size; i++)
	      {
		int id;
		SocketCommand comm;

		input >> id >> comm;
		tclreply << id << " " << GetPlainText(comm) << " ";
	      }
	  }

	char* dummy;
	dummy = new char[tclreply.str().size()+1];
	strcpy(dummy,tclreply.str().c_str());
	Tcl_SetResult (interp, dummy, TCL_VOLATILE);
	
	
	clientsocket.Reset(NULL);
      }

    else if (strcmp(argv[1],"getserverlist") == 0)
      {
	ostringstream tclreply;
	for (int i=0; i<servers.Size(); i++)
	  tclreply << servers[i]->host << " " << servers[i]->port << " " << servers[i]->clientid << " ";
	char* dummy;
	dummy = new char[tclreply.str().size()+1];
	strcpy(dummy,tclreply.str().c_str());
	Tcl_SetResult (interp, dummy, TCL_VOLATILE);	
      }

    else if (strcmp(argv[1],"saveserverlist") == 0)
      {
	ofstream outfile("sockets.ini");
	outfile << servers.Size() << " ";
	for(int i=0; i<servers.Size(); i++)
	  outfile << servers[i]->host << " " << servers[i]->port << " ";
	outfile.close();
      }

    else if (strcmp(argv[1],"loadserverlist") == 0)
      {
	ifstream infile ("sockets.ini");
	int numservers;

	if(infile.good())
	  {
	    infile >> numservers;
	    if(infile.good())
	      {
		servers.SetSize(servers.Size()+numservers);
		for(int i=0; infile.good() && i<numservers; i++)
		  {
		    ServerInfo * auxserver = new ServerInfo;
		    infile >> auxserver->host;
		    infile >> auxserver->port;
		    
		    servers[servers.Size()-numservers+i] = auxserver;
		  }
	      }
	  }
      }

    else
      {
	string err = "Ng_Socket: Unknown option \""; err += argv[1]; err += "\"";
	char * dummy = new char[err.size()+1];
	strcpy(dummy,err.c_str());
	Tcl_SetResult (interp,dummy,TCL_VOLATILE);
	delete [] dummy;
	return TCL_ERROR;
      }

    return TCL_OK;
  }
}

int Ng_Socket_Init (Tcl_Interp * interp)
{
  using namespace netgen;
  Tcl_CreateCommand (interp, "Ng_Socket", Ng_Socket,
		     (ClientData)NULL,
		     (Tcl_CmdDeleteProc*) NULL);  
  
  return TCL_OK;
}

#endif
