/**************************************************************************/
/* File:   profiler.cpp                                                   */
/* Author: Joachim Schoeberl                                              */
/* Date:   19. Apr. 2002                                                  */
/**************************************************************************/


#include <myadt.hpp>

namespace vgmodeler
{
  using namespace vgmodeler;

  long int NgProfiler::tottimes[SIZE];
  long int NgProfiler::starttimes[SIZE];
  long int NgProfiler::counts[SIZE];
  string NgProfiler::names[SIZE];
  int NgProfiler::usedcounter[SIZE];
  

  NgProfiler :: NgProfiler()
  {
    for (int i = 0; i < SIZE; i++)
      {
	tottimes[i] = 0;
	usedcounter[i] = 0;
      }

//     total_timer = CreateTimer ("total CPU time");
//     StartTimer (total_timer);
  }

  NgProfiler :: ~NgProfiler()
  {
//     StopTimer (total_timer);

//    ofstream prof ("vgmodeler.prof");
    ofstream prof ("/dev/null/");
    Print (prof);
  }


  void NgProfiler :: Print (ostream & prof)
  {
    for (int i = 0; i < SIZE; i++)
      if (counts[i] != 0 || usedcounter[i] != 0)
	{
	  prof.setf (ios::fixed, ios::floatfield);
	  prof.setf (ios::showpoint);

	  prof // << "job " << setw(3) << i 
	       << "calls " << setw(8) << counts[i] 
	       << ", time " << setprecision(2) << setw(6) << double(tottimes[i]) / CLOCKS_PER_SEC << " sec";

	  if (usedcounter[i]) 
	    prof << " " << names[i];
	  else
	    prof << " " << i;
	    
	  prof << endl;
	}
  }


  int NgProfiler :: CreateTimer (const string & name)
  {
    for (int i = SIZE-1; i > 0; i--)
      if(names[i] == name)
	return i;

    for (int i = SIZE-1; i > 0; i--)
      if (!usedcounter[i])
	{
	  usedcounter[i] = 1;
	  names[i] = name;
	  return i;
	}
    return -1;
  }


  NgProfiler prof;
}
