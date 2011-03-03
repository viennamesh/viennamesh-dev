// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
// ----------------------------------------------------------------------------

/*! 
  \file kernel.h
  \brief Includes the classes in the kernel sub-package.
*/

/*!
  \page geom_kernel Kernel Package

  The kernel sub-package has the following functionality:
  - Point.h has \ref point "functions" for using ads::FixedArray's as points 
  and vectors.
  - content.h defines \ref content "functions" to compute content
  (length, area, volume, etc.).
  - geom::Ball is a ball in N-D.
  - geom::BBox is a bounding box in N-D, which is a closed interval.
  - geom::Circle3 is a circle in 3-D.
  - geom::CircularArc3 is a circular arc in 3-D.
  - geom::Interval is an open interval in N-D.
  - geom::Line_2 is a line in 2-D.
  - orientation.h defines \ref geom_kernel_orientation "orientation" functions.
  - geom::ParametrizedLine is a parametrized line in N-D.
  - geom::ParametrizedPlane is a parametrized plane in N-D.
  - geom::Plane is a plane in 3-D.
  - geom::SemiOpenInterval is a semi-open interval in N-D.
  - geom::Segment is a line segment in N-D.
  - geom::SegmentMath is a line segment in N-D that supports some 
    mathematical operations.
  - geom::Triangle is a triangle in N-D.

  Use this sub-package by including the file kernel.h.
*/

#if !defined(__geom_kernel_h__)
#define __geom_kernel_h__

#include "kernel/Ball.h"
#include "kernel/BBox.h"
#include "kernel/Circle3.h"
#include "kernel/CircularArc3.h"
#include "kernel/content.h"
#include "kernel/distance.h"
#include "kernel/Interval.h"
#include "kernel/Line_2.h"
#include "kernel/orientation.h"
#include "kernel/ParametrizedLine.h"
#include "kernel/ParametrizedPlane.h"
#include "kernel/Plane.h"
#include "kernel/Point.h"
#include "kernel/Segment.h"
#include "kernel/SegmentMath.h"
#include "kernel/SemiOpenInterval.h"
#include "kernel/Triangle.h"

#endif

// End of file.
