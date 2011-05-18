#ifndef FILE_PARALLEL
#define FILE_PARALLEL


#ifndef PARALLEL
namespace netgen
{
  using namespace netgen;
  enum { id = 0 };
  enum { ntasks = 0 };
}


#else   // if PARALLEL


#include <mystdlib.h>
#include <meshing.hpp>
#include "incvis.hpp"


//#include "parallelfunc.hpp"
extern MPI_Group MPI_HIGHORDER_WORLD;
extern MPI_Comm MPI_HIGHORDER_COMM;

namespace netgen
{
  extern int id, ntasks;

  // -> Astrid: hab ein paar Änderungen gemacht. Wenn ok, lösch das alte und die Erklärungen


  template <class T>
  MPI_Datatype MyGetMPIType ( ) { cerr << "ERROR in GetMPIType() -- no type found" << endl;return 0;}

  template <>
  inline MPI_Datatype MyGetMPIType<int> ( ) 
  { return MPI_INT; }
  // { return new MPI_Datatype (MPI_INT); }
  
  template <>
  inline MPI_Datatype MyGetMPIType<double> ( ) 
  { return MPI_DOUBLE; }


  // damit gehen auch echte Konstante ohne Adresse
  inline void MyMPI_Send (int i, int dest)
  {
    int hi = i;
    MPI_Send( &hi, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
  }

  inline void MyMPI_Recv (int & i, int src)
  {
    MPI_Status status;
    MPI_Recv( &i, 1, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }



  inline void MyMPI_Send (const string & s, int dest)
  {
    MPI_Send( const_cast<char*> (s.c_str()), s.length(), MPI_CHAR, dest, 1, MPI_COMM_WORLD);
  }

  inline void MyMPI_Recv (string & s, int src)
  {
    MPI_Status status;
    int len;
    MPI_Probe (src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    MPI_Get_count (&status, MPI_CHAR, &len);
    s.assign (len, ' ');
    MPI_Recv( &s[0], len, MPI_CHAR, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }

 












  /*
  inline void MyMPI_Send (const ARRAY<int, 0> & s, int dest)
  {
     int len = s.Size();
     (*testout) << "send int array, len = " << len << endl;
     MPI_Send( &len, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
     if ( len == 0 ) return;
     MPI_Send( const_cast<int*> (&s[0]), len, MPI_INT, dest, 1, MPI_COMM_WORLD);
  }

  inline void MyMPI_Send (const ARRAY<double, 0> & s, int dest)
  {
    int len = s.Size();
    (*testout) << "send double array, len = " << len << endl;
     MPI_Send( &len, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
     if ( len == 0 ) return;

     MPI_Send( const_cast<double*> (&s[0]), len, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);

  }


  inline void MyMPI_Recv ( ARRAY <int, 0> & s, int src)
  {
    MPI_Status status;
    int len;
    MPI_Recv( &len, 1, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    s.SetSize(len);
    (*testout) << "recv int array, size " << len << endl;
    if ( len == 0 ) return;

    MPI_Recv( &s[0], len, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }

  inline void MyMPI_Recv ( ARRAY <double, 0> & s, int src)
  {
    MPI_Status status;
    int len;
    MPI_Recv( &len, 1, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    s.SetSize(len);
    (*testout) << "recv double array, size " << len << endl;
    if ( len == 0 ) return;
    MPI_Recv( &s[0], len, MPI_DOUBLE, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }
  */


  template <class T, int BASE>
  inline void MyMPI_Send (FlatArray<T, BASE> s, int dest)
  {
    MPI_Send( &s.First(), s.Size(), MyGetMPIType<T>(), dest, 1, MPI_COMM_WORLD);
  }

  template <class T, int BASE>
  inline void MyMPI_Recv ( ARRAY <T, BASE> & s, int src)
  {
    MPI_Status status;
    int len;
    MPI_Probe (src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    MPI_Get_count (&status, MyGetMPIType<T>(), &len);

    s.SetSize (len);
    MPI_Recv( &s.First(), len, MyGetMPIType<T>(), src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }

  template <class T, int BASE>
  inline int MyMPI_Recv ( ARRAY <T, BASE> & s)
  {
    MPI_Status status;
    int len;
    MPI_Probe (MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    int src = status.MPI_SOURCE;

    MPI_Get_count (&status, MyGetMPIType<T>(), &len);

    s.SetSize (len);
    MPI_Recv( &s.First(), len, MyGetMPIType<T>(), src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    return src;
  }



  template <class T, int BASE>
  inline void MyMPI_ISend (FlatArray<T, BASE> s, int dest, MPI_Request & request)
  {
    MPI_Isend( &s.First(), s.Size(), MyGetMPIType<T>(), dest, 1, MPI_COMM_WORLD, & request);
  }


  inline void MyMPI_BCast (ARRAY<int, 0> & s)
  {
    /*
    int size = s.Size();
    MPI_Bcast (&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (id != 0) 
      s.SetSize (size);
    MPI_Bcast (&s[0], s.Size(), MPI_INT, 0, MPI_COMM_WORLD);
    */

    if (id == 0)
      for (int dest = 1; dest < ntasks; dest++)
	MyMPI_Send (s, dest);
    else
      MyMPI_Recv (s, 0);
  }


 


  inline void MyMPI_Send (  int *& s, int & len,  int dest)
  {
     MPI_Send( &len, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
     MPI_Send( s, len, MPI_INT, dest, 1, MPI_COMM_WORLD);
  }


  inline void MyMPI_Recv ( int *& s, int & len, int src)
  {
    MPI_Status status;
    MPI_Recv( &len, 1, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    if ( s ) 
      delete [] s;
    s = new int [len];
    MPI_Recv( s, len, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }



  inline void MyMPI_Send ( double * s, int len,  int dest)
  {
     MPI_Send( &len, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
     MPI_Send( s, len, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
  }


  inline void MyMPI_Recv ( double *& s, int & len, int src)
  {
    MPI_Status status;
    MPI_Recv( &len, 1, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    if ( s )
      delete [] s;
    s = new double [len];
    MPI_Recv( s, len, MPI_DOUBLE, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }


// #include "parallelmesh.hpp"
#include "paralleltop.hpp"

#include "parallelinterface.hpp"

}

#endif // PARALLEL

#endif
