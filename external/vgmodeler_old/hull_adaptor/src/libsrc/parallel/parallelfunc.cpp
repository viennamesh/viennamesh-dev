#ifdef PARALLEL

#ifdef OCCGEOMETRY
#include <occgeom.hpp>
#endif



#include <mystdlib.h>
#include <meshing.hpp>
#include "incvis.hpp"



#include <geometry2d.hpp>
#include <stlgeom.hpp>

#include <meshing.hpp>
#include <incvis.hpp>
#include <visual.hpp>
#include <mystdlib.h>
#include <myadt.hpp>
#include <linalg.hpp>
#include <csg.hpp>

#include "parallel.hpp"
#include "parallelfunc.hpp"


namespace netgen
{
#include "../interface/writeuser.hpp"
  extern string ngdir;
}

void Parallel_Exit();


namespace netgen {
extern AutoPtr<Mesh>  mesh;

  // geometry: either CSG, or, if an other is non-null, 
  // then the other
  extern AutoPtr<CSGeometry> geometry ;

  extern STLGeometry * stlgeometry;
  extern AutoPtr<SplineGeometry2d> geometry2d ;

#ifdef OCCGEOMETRY
  extern OCCGeometry * occgeometry;
#endif

#ifdef ACIS
  extern ACISGeometry * acisgeometry;
#endif

}

using namespace netgen;
using netgen::RegisterUserFormats;

#ifdef PARALLEL
  void Ng_Exit ()
  {
#ifdef NGSOLVE
    Parallel_Exit();
#endif

    delete stlgeometry;
    stlgeometry = NULL;
   

#ifdef OCCGEOMETRY 
	delete occgeometry; 
	occgeometry = 0;
#endif


    geometry.Reset (0);
    geometry2d.Reset (0);
    
    //    delete testout;
    return;
  }
#endif



void ParallelRun()
{   
      string message;
      
      MPI_Status status;
      
//       int id, rc, ntasks;

      MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
      MPI_Comm_rank(MPI_COMM_WORLD, &id);



      bool test = true;



      testout = new ofstream (string("testout_proc") + id  );      

      while ( test )
	{
#ifdef SCALASCA
#pragma pomp inst begin (message)
#endif

	  (*testout) << "wait for mess " << endl;
	  MyMPI_Recv ( message, 0 );
	  (*testout) << "message " << message << endl;

#ifdef SCALASCA
#pragma pomp inst end (message)
#endif

#ifdef NGSOLVE

	  if ( message.compare(0, 3, "ngs") == 0 ) 
	    {
	      PrintMessage ( 2, "Starting NgSolve routine ", message ) ;
	      NGS_ParallelRun (message);
	    }
	  else
#endif
	
	  if ( message == "mesh" )
	    {
	      mesh.Reset( new netgen::Mesh);
	      mesh->ReceiveParallelMesh();
	    }

	  else if ( message == "overlap++" )
	    {
	      mesh -> UpdateOverlap();
	    }

	  else if ( message == "visualize" )
	    {
	      cout << "p" << id << ": ACHTUNG - alles wieder zumachen, sonst geht nix mehr :)" << endl;
	      // initialize application
	      Tcl_Interp * myinterp = Tcl_CreateInterp ();
	      if (Tcl_AppInit (myinterp) == TCL_ERROR)
		{
		  cerr << "Exit Netgen due to initialization problem" << endl;
		  exit (1);
		}

	      string startfile = ngdir + "/libsrc/parallel/ng_parallel.tcl";
	      
	      if (verbose)
		cout << "Load Tcl-script from " << startfile << endl;
	      
	      int errcode = Tcl_EvalFile (myinterp, (char*)startfile.c_str());

	      if (errcode)
		{
		  cout << "Error in Tcl-Script:" << endl;
		  cout << "result = " << myinterp->result << endl;
		  cout << "in line " << myinterp->errorLine << endl;
		  
		  if (myinterp->errorLine == 1)
		    cout << "\nMake sure to set environment variable NETGENDIR" << endl;
		  
		  exit (1);
		}
	      

 	      // lookup user file formats and insert into format list:
 	      ARRAY<const char*> userformats;
 	      RegisterUserFormats (userformats);
	      
 	      ostringstream fstr;
 	      for (int i = 1; i <= userformats.Size(); i++)
 		fstr << ".ngmenu.file.filetype add radio -label \"" 
 		     << userformats.Get(i) << "\" -variable exportfiletype\n";
	      
	      
	      Tcl_Eval (myinterp, (char*)fstr.str().c_str());
 	      Tcl_SetVar (myinterp, "exportfiletype", "Neutral Format", 0);



	      Tk_MainLoop();
	      
	      
	      Tcl_DeleteInterp (myinterp); 
	      

	    }
	  else if ( message ==  "end" )
	    {
	      PrintMessage (1, "EXIT");
	      test = false;
	      // end netgen
	      Ng_Exit();
	    }


	  else
	    {
	      PrintMessage ( 1, "received unidentified message " + message );
	     
	      test = false;
	    }
	  
	}
      
       return;
}








#endif
