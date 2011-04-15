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
    Pointlist("CADpointlist", "m", 2, 4) 
    { 
        Intern {
          0, 0,     // #0
          1, 0,     // #1
          1, 1,     // #2
          0, 1     // #3
        } // End of Block 'Intern'
    } // End of Pointlist 'CADpointlist'

  } // End of Pointlists

  Linelists(1)
  {
    Linelist("CADlinelist", 4)
    {
      Ref(Pointlist, "CADpointlist")
        Intern {
               0,      1,     // #0
               1,      2,     // #1
               2,      3,     // #2
               3,      0     // #3
        } // End of Block Intern
    }
  }   // End of Linelists


// ***** BEGIN OF SEGMENT-List *****
  Segments(1)
  {
    Segment("Bulk", Boundary)  // Segment #0
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

  }  // End of Segments
// ***** END OF SEGMENT-List *****

}
