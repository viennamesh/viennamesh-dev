// Software License for MTL
// 
// Copyright (c) 2007 The Trustees of Indiana University.
//               2008 Dresden University of Technology and the Trustees of Indiana University.
// All rights reserved.
// Authors: Peter Gottschling and Andrew Lumsdaine
// 
// This file is part of the Matrix Template Library
// 
// See also license.mtl.txt in the distribution.

#ifndef MTL_IRANGE_INCLUDE
#define MTL_IRANGE_INCLUDE

#include <limits>


namespace mtl { 

    /// Maximal index
    const std::size_t imax= std::numeric_limits<std::size_t>::max();

    /// Class to define a half open index ranges 
    class irange
    {
      public:

        typedef std::size_t size_type;

        /// Create an index range of [start, finish)
        explicit irange(size_type start, size_type finish) : my_start(start), my_finish(finish) {}

        /// Create an index range of [0, finish)
        explicit irange(size_type finish) : my_start(0), my_finish(finish) {}

        /// Create an index range of [0, imax), i.e. all indices
        irange() : my_start(0), my_finish(imax) {}

        /// Set the index range to [start, finish)
	irange& set(size_type start, size_type finish) 
	{
	    my_start= start; my_finish= finish; return *this;
	}

        /// Set the index range of [0, finish)
	irange& set(size_type finish) 
	{
	    my_start= 0; my_finish= finish; return *this;
	}

        /// First index in range
        size_type start() const { return my_start; } 
        /// Past-end index in range
        size_type finish() const { return my_finish; }
        /// Number of indices
        size_type size() const { return my_finish > my_start ? my_finish - my_start : 0; }

      private:
        size_type my_start, my_finish;
    };

    namespace {
	irange iall;
    }
} // namespace mtl



#endif // MTL_IRANGE_INCLUDE
