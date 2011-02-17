/* GTS Device File written by gtsio3 version DEVELOPMENT, Internal version, don't share!, build date: Feb  9 2011, 17:02:44 */

GTSDevice("Device")
{
  Info
  {
    Version("Draft 0.9")	// Fileformat version
    Dimension(3)		// Device dimension
    Comment
    {
    }
  }

  Pointlists(1)
  {
    // Pointlist(<name>, <unit>, <dimension>, <nr of points>) { <data> }
    Pointlist("CADpointlist", "m", 3, 12) 
    { 
        Intern {
          0.0, 0.0, 0.0,     // #0
          1.0, 0.0, 0.0,     // #1
          1.0, 0.0, 1.0,     // #2
          0.0, 0.0, 1.0,     // #3
          1.0, 1.0, 0.0,     // #4
          0.0, 1.0, 0.0,     // #5
          1.0, 1.0, 1.0,     // #6
          0.0, 1.0, 1.0,     // #7
          2.0, 0.0, 0.0,     // #8 
          2.0, 0.0, 1.0,     // #9
          2.0, 1.0, 0.0,     // #10
          2.0, 1.0, 1.0      // #11
        } // End of Block 'Intern'
    } // End of Pointlist 'CADpointlist'

  } // End of Pointlists

  Facelists(1)
  {
    Facelist("CADfacelist", 12)
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
  Segments(2)
  {
    Segment("anode", Boundary)  // Segment #0
    {
      Ref(Linelist, "CADlinelist")
      Material("Conductor")
      IsClosedHull(true)
      BoundaryElements(6)
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
      BoundaryElements(6)
      {
        Intern {
               4,      5,      6,      7      // #0
        } // End of Block Intern
      }
    }   // End of segment #1: cathode

  }  // End of Segments
// ***** END OF SEGMENT-List *****

}
