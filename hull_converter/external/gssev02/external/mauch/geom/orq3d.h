// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
// ----------------------------------------------------------------------------

/*! 
  \file orq3d.h
  \brief Includes the Orthogonal Range Query classes.
*/

/*!
  \page geom_orq3d 3-D ORQ Package

  The 3-D ORQ sub-package has many classes for performing orthogonal range
  queries in 3-D:
  - geom::CellArray
  - geom::CellXYBinarySearchZ
  - geom::CellXYForwardSearchKeyZ
  - geom::CellXYForwardSearchZ
  - geom::KDTree
  - geom::Octree
  - geom::PlaceboCheck
  - geom::Placebo
  - geom::SequentialScan
  - geom::SortProject
  - geom::SortRankProject
  - geom::SparseCellArray

  Use this sub-package by including the file orq3d.h.

  For an explanation and analysis of each of these data structures, consult 
  my thesis, available from my web site: http://www.its.caltech.edu/~sean/

  Each of the orthogonal range query classes are templated on the record type.
  These data structures store pointers to the records.  Thus the user is 
  responsible for storing the records.  

  Additionally, each of the orthogonal range query classes is templated 
  on the multi-key type and the key type.  The multi-key type is a triple
  of keys.  The multi-key type must be subscriptable, i.e. if 
  \c mk is a multi-key, then one can access the three keys with
  \c mk[0], \c mk[1], and \c mk[2].  The multi-key type and the 
  key type are optional template parameters.  If the record class defines
  the types: \c multi_key_type and \c key_type, then the ORQ classes will
  be able to use these types as the default.  The record class must define
  the constant member function \c multi_key() that returns the multi-key
  (or a const reference to the multi-key).  See the three classes: 
  geom::Record, geom::RecordCArray, and geom::RecordSimple for examples
  of suitable record classes.  
  
  If the record class defines the output operator, \c operator<<(), then 
  one can use the output operators for the ORQ class.  Because of 
  implementation details, the record class must define this operator in order
  to use geom::KDTree or geom::Octree.  The function need not actually do
  anything, but it must be defined.
*/

#if !defined(__geom_orq3d_h__)
#define __geom_orq3d_h__

#include "orq3d/CellArray.h"
#include "orq3d/CellXYBinarySearchZ.h"
#include "orq3d/CellXYForwardSearchKeyZ.h"
#include "orq3d/CellXYForwardSearchZ.h"
#include "orq3d/CellXYSearchZ.h"
#include "orq3d/KDTree.h"
#include "orq3d/Octree.h"
#include "orq3d/ORQ.h"
#include "orq3d/PlaceboCheck.h"
#include "orq3d/Placebo.h"
#include "orq3d/RecordCompare.h"
#include "orq3d/Record.h"
#include "orq3d/SequentialScan.h"
#include "orq3d/SortProject.h"
#include "orq3d/SortRankProject.h"
#include "orq3d/SparseCellArray.h"

#endif

// End of file.
