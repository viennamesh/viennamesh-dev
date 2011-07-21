/* GTS Device File written by gtsio3 version DEVELOPMENT, Internal version, don't share!, build date: Feb  9 2011, 17:02:44 */

GTSDevice("Device")
{
  Info
  {
    Version("Draft 0.9")	// Fileformat version
    Dimension(2)		// Device dimension
    Comment
    {
    }
  }

  Pointlists(1)
  {
    // Pointlist(<name>, <unit>, <dimension>, <nr of points>) { <data> }
    Pointlist("CADpointlist", "m", 2, 8) 
    { 
        Intern {
          -0.000417000, 0.000244000,     // #0
          -0.000417000, 0.000149000,     // #1
          -0.000406000, 0.000149000,     // #2
          -0.000406000, 0.000244000,     // #3
          -9.20000e-05, 0.000244000,     // #4
          -9.20000e-05, 0.000149000,     // #5
          -8.10000e-05, 0.000149000,     // #6
          -8.10000e-05, 0.000244000      // #7
        } // End of Block 'Intern'
    } // End of Pointlist 'CADpointlist'

  } // End of Pointlists

  Linelists(1)
  {
    Linelist("CADlinelist", 12)
    {
      Ref(Pointlist, "CADpointlist")
        Intern {
               0,      1,     // #0
               1,      2,     // #1
               2,      3,     // #2
               3,      0,     // #3
               4,      5,     // #4
               5,      6,     // #5
               6,      7,     // #6
               7,      4,     // #7
               3,      2,     // #8
               2,      5,     // #9
               5,      4,     // #10
               4,      3      // #11
        } // End of Block Intern
    }
  }   // End of Linelists


// ***** BEGIN OF SEGMENT-List *****
  Segments(3)
  {
    Segment("anode", Boundary)  // Segment #0
    {
      Ref(Linelist, "CADlinelist")
      Material("Conductor")
      IsClosedHull(true)
      BoundaryElements(4)
      {
        Intern {
               0,      1,      2,      3      // #0
        } // End of Block Intern
      }
    }   // End of segment #0: anode


    Segment("cathode", Boundary)  // Segment #1
    {
      Ref(Linelist, "CADlinelist")
      Material("Conductor")
      IsClosedHull(true)
      BoundaryElements(4)
      {
        Intern {
               4,      5,      6,      7      // #0
        } // End of Block Intern
      }
    }   // End of segment #1: cathode


    Segment("bulk", Boundary)  // Segment #2
    {
      Ref(Linelist, "CADlinelist")
      Material("Donor")
      IsClosedHull(true)
      BoundaryElements(4)
      {
        Intern {
               8,      9,     10,     11      // #0
        } // End of Block Intern
      }
    }   // End of segment #2: bulk


  }  // End of Segments
// ***** END OF SEGMENT-List *****

}
