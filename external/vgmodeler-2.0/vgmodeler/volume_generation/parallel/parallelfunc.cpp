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

#ifdef PARALLEL
  void Ng_Exit ()
  {
    Parallel_Exit();

    delete stlgeometry;
    stlgeometry = NULL;
    
    delete occgeometry;
    occgeometry = 0;

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
      
      int id, rc, ntasks;

      MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
      MPI_Comm_rank(MPI_COMM_WORLD, &id);


      int isinit;
      MPI_Initialized( &isinit);

      bool test = true;



//       testout = new ofstream (string("testout_proc") + id  );      


      
      while ( test )
	{
	  MyMPI_Recv ( message, 0 );
	  std::cout << "message " << message << endl;
#ifdef NGSOLVE
      /**
       *  @brief  Compare substring to a string.
       *  @param pos  Index of first character of substring.
       *  @param n  Number of characters in substring.
       *  @param str  String to compare against.
       *  @return  Integer < 0, 0, or > 0.
       *
       *  Form the substring of this string from the @a n characters starting
       *  at @a pos.  Returns an integer < 0 if the substring is ordered
       *  before @a str, 0 if their values are equivalent, or > 0 if the
       *  substring is ordered after @a str.  Determines the effective length
       *  rlen of the strings to compare as the smallest of the length of the
       *  substring and @a str.size().  The function then compares the two
       *  strings by calling traits::compare(substring.data(),str.data(),rlen).
       *  If the result of the comparison is nonzero returns it, otherwise the
       *  shorter one is ordered first.
      */
//       int
//       compare(size_type __pos, size_type __n, const basic_string& __str) const;


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
	      mesh->LoadParallelMesh();
	    }
	  /*
	  else if ( message ==  "findexchangepoints")
	    {
	      mesh->FindExchangePoints();
	      PrintMessage ( 3, "Computed exchange vertices");
	    
	    }

	  else if ( message ==  "findexchangeedges" )
	    {
	      mesh->FindExchangeEdges();
	      PrintMessage ( 3, "Computed exchange edges");

	    }

	  else if ( message == "findexchangefaces" )
	    {
	      mesh->FindExchangeFaces();
	      PrintMessage ( 3, "Computed exchange faces");
	    }
	  */
	  else if ( message ==  "end" )
	    {
	      PrintMessage (1, "EXIT");
	      test = false;
	      // end netgen
	      Ng_Exit();
	    }


          /*
	  else if ( strcmp (message, "pdefile" ) == 0 )
	    {
	      
	      MPI_Recv( pdefile, 100, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	      string pdef(pdefile);
	      pde.LoadPDE (pdef, 1, 0);

	    }
          */
	  else
	    {
	      PrintMessage ( 1, "received unidentified message " + message );
	     
	      test = false;
	    }
	  
	}
      
       return;
}


void ReceiveMesh()
{
  ;
}












#endif
