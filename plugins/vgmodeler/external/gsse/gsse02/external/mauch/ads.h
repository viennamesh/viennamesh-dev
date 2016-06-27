// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2006-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

#if !defined(__ads_h__)
#define __ads_h__

#include "ads/algorithm.h"
#include "ads/array.h"
#include "ads/functor.h"
#include "ads/halfedge.h"
#include "ads/iterator.h"
#include "ads/priority_queue.h"
#include "ads/set.h"
#include "ads/tensor.h"
#include "ads/timer.h"
#include "ads/utility.h"

BEGIN_NAMESPACE_ADS

/*!
\mainpage Algorithms and Data Structures Package

\author Sean Mauch, http://www.its.caltech.edu/~sean/ , at(sean, dot(caltech, edu))

\section ads_introduction Introduction

This is a templated C++ class library.  All the functionality is
implemented in header files.  Thus there is no library to compile or
link with.  Just include the appropriate header files in your
application code when you compile.

The ADS package is composed of a number of sub-packages.  All classes
and functions are in the \ref ads namespace.  There are some general purpose
sub-packages:
- The \ref ads_array "array package" has fixed-size and dynamically-sized 
arrays.  There are classes for N-D arrays that either allocate
their own memory, or wrap externally allocated memory.
- The \ref ads_priority_queue "priority queue package" has priority queues
implemented with binary heaps and an approximate priority queue implemented
with a cell array.  
- The \ref ads_timer "timer package" has a simple timer class.  
.
Other sub-packages are preliminary or ad hoc.
- The \ref ads_algorithm "algorithm" package has min and max functions for more
than two arguments and functions for sorting.
- The \ref ads_functor "functor" package defines various utility functors.
- The \ref ads_iterator "iterator" package has iterator adapters.
- The \ref ads_tensor "tensor" package has square matrices.
- The \ref ads_halfedge "halfedge" package has a halfedge data structure.
- The \ref ads_utility "utility" package has a class for parsing 
  command line options, and string functions.

\section ads_compiling Compiling

The following compilers are supported:
<TABLE>
<TR>
<TH> Compiler
<TH> Versions
<TH> Flags
<TH> Date Tested
<TH> Notes

<TR> 
<TD> GNU Compiler Colection, g++
<TD> 3.4, 4.0, 4.2
<TD> -ansi -pedantic -Wall
<TD> June 3, 2007
<TD>
</TABLE>

- GNU C++ compiler, g++, version 3.3.x, with flags: -ansi -pedantic.
- Intel C++ compiler, icc, version 8.0, with flags: -strict_ansi.
.
This library is ANSI compliant.  (Hence the note about ANSI flags above.
If your code is not ANSI compliant, then you would not want to use
these flags.)  In theory, the code should compile on any platform/compiler
combination that supports ANSI C++.  In practice, I am sure that this is 
not the case.  In the future I will test the code with additional 
compilers to improve its portability.

The top-level directory of this package is called \c ads.  Each sub-package
has its own sub-directory.  For example, the array package is in the 
\c array directory.  Each sub-package has a header file in the top-level
directory that includes all the classes and functions for that package.
For the array package, this file is \c array.h.  To use the array package,
one would add:
\code
#include "ads/array.h"
\endcode
*/

/*!
\page ads_bibliography Bibliography

\anchor ads_edelsbrunner2001
Herbert Edelsbrunner.
"Geometry and Topology for Mesh Generation."
Cambridge University Press, 2001.
*/

END_NAMESPACE_ADS

#endif

// End of file.
