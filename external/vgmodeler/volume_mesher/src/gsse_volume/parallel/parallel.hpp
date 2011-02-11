#ifndef FILE_PARALLEL
#define FILE_PARALLEL


#include <mystdlib.h>
#include <meshing.hpp>
#include "incvis.hpp"


//#include "parallelfunc.hpp"

namespace netgen
{


  inline void MyMPI_Send (const int & i, int dest)
  {
    MPI_Send( const_cast<int*> (&i), 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
  }

  inline void MyMPI_Recv (int & i, int src)
  {
    MPI_Status status;
    MPI_Recv( &i, 1, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }


  
  inline void MyMPI_Send (const string & s, int dest)
  {
    int len = s.length();
    MPI_Send( &len, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
    const char * cstr = s.c_str();
    cout << s << endl;
    MPI_Send( const_cast<char*> (cstr), len, MPI_CHAR, dest, 1, MPI_COMM_WORLD);
  }

  inline void MyMPI_Recv (string & s, int src)
  {
    MPI_Status status;
    int len;
    MPI_Recv( &len, 1, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    s.assign (len, ' ');
    char * cstr = const_cast<char*> (s.c_str());
    MPI_Recv( cstr, len, MPI_CHAR, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }


 
 
  inline void MyMPI_Send (const ARRAY<int, 0> & s, int dest)
  {
     int len = s.Size();
     MPI_Send( &len, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);

     /*
     int * Tarray = new int (len );
     for ( int i = 0; i < len; i++)
       Tarray[i] = s[i];
     MPI_Send( const_cast<int*> (Tarray), len, MPI_INT, dest, 1, MPI_COMM_WORLD);
     */
     MPI_Send( const_cast<int*> (&s[0]), len, MPI_INT, dest, 1, MPI_COMM_WORLD);

     // delete Tarray;
  }


  inline void MyMPI_Recv ( ARRAY <int, 0> & s, int src)
  {
    MPI_Status status;
    int len;
    MPI_Recv( &len, 1, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    s.SetSize(len);
    MPI_Recv( &s[0], len, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    /*
    int * Tarray;
    Tarray = new int (len);
    MPI_Recv( Tarray, len, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    for ( int i = 0; i < len; i++)
      {cout <<Tarray[i];
	s[i] = Tarray[i];}
    delete Tarray;
    */
  }
 


  inline void MyMPI_Send (  int *& s, int & len,  int dest)
  {
     MPI_Send( &len, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
     MPI_Send( s, len, MPI_INT, dest, 1, MPI_COMM_WORLD);
//      (*testout) << "sent int[" << len << "] to " << dest << endl;
  }


  inline void MyMPI_Recv ( int *& s, int & len, int src)
  {
    MPI_Status status;
    MPI_Recv( &len, 1, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    delete s;
    s = new int [len];
    MPI_Recv( s, len, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }



  inline void MyMPI_Send ( double *& s, int len,  int dest)
  {
     MPI_Send( &len, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
     MPI_Send( s, len, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
  }


  inline void MyMPI_Recv ( double *& s, int & len, int src)
  {
    MPI_Status status;
    MPI_Recv( &len, 1, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    delete s;
    s = new double [len];
    MPI_Recv( s, len, MPI_DOUBLE, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }




#include "parallelmesh.hpp"
#include "paralleltop.hpp"

#include "parallelinterface.hpp"

}

#endif
