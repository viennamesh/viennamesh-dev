// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 1999-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
// ----------------------------------------------------------------------------

#if !defined(__ads_composite_compare_ipp__)
#error This file is an implementation detail of composite_compare.
#endif

BEGIN_NAMESPACE_ADS

//
// Comparisons for N-D numbers.
//

template <int N, typename PointType>
inline
bool 
less_composite_fcn( const int n, const PointType& a,  const PointType& b )
{
#ifdef DEBUG_composite_compare
  assert( 0 <= n && n < N );
#endif
  if ( a[n] < b[n] ) {
    return true;
  }
  if ( a[n] == b[n] ) {
    for ( int i = (n+1)%N; i != n; i = (i+1)%N ) {
      if ( a[i] < b[i] ) {
	return true;
      }
      if ( a[i] != b[i] ) {
	break;
      }
    }
  }
  return false;
}

/* CONTINUE
template <int N, typename T>
inline
bool 
less_composite_fcn( int n, const FixedArray<N,T>* a, const FixedArray<N,T>* b )
{
  return less_composite_fcn( n, *a, *b );
}
*/

//
// Comparisons for 3-D numbers.
//

template <typename Pt3D>
inline
bool 
xless_composite_compare( const Pt3D& a, const Pt3D& b ) {
  if ( a[0] < b[0] ) {
    return true;
  }
  else if ( a[0] == b[0] ) {
    if ( a[1] < b[1] ) {
      return true;
    }
    else if ( a[1] == b[1] ) {
      if ( a[2] < b[2] ) {
	return true;
      }
    }
  }
  return false;
}
  
template <typename Pt3D>
inline
bool 
yless_composite_compare( const Pt3D& a, const Pt3D& b ) {
  if ( a[1] < b[1] ) {
    return true;
  }
  else if ( a[1] == b[1] ) {
    if ( a[2] < b[2] ) {
      return true;
    }
    else if ( a[2] == b[2] ) {
      if ( a[0] < b[0] ) {
	return true;
      }
    }
  }
  return false;
}
  
template <typename Pt3D>
inline
bool 
zless_composite_compare( const Pt3D& a, const Pt3D& b ) {
  if ( a[2] < b[2] ) {
    return true;
  }
  else if ( a[2] == b[2] ) {
    if ( a[0] < b[0] ) {
      return true;
    }
    else if ( a[0] == b[0] ) {
      if ( a[1] < b[1] ) {
	return true;
      }
    }
  }
  return false;
}
  
template <typename Pt3D>
inline
bool 
less_composite_compare( int i, const Pt3D& a, const Pt3D& b )
{
#ifdef DEBUG_composite_compare
  assert( 0 <= i && i < 3 );
#endif
  if ( i == 0 ) {
    return xless_composite_compare( a, b );
  }
  else if ( i == 1 ) {
    return yless_composite_compare( a, b );
  }
  return zless_composite_compare( a, b );
}




// CONTINUE
//--------------------------------------------------------------------------
#if 0
template <typename T>
inline
bool 
xless_composite_compare( const FixedArray<3,T>& a, 
			 const FixedArray<3,T>& b ) {
  if ( a[0] < b[0] ) {
    return true;
  }
  else if ( a[0] == b[0] ) {
    if ( a[1] < b[1] ) {
      return true;
    }
    else if ( a[1] == b[1] ) {
      if ( a[2] < b[2] ) {
	return true;
      }
    }
  }
  return false;
}
  
template <typename T>
inline
bool 
yless_composite_compare( const FixedArray<3,T>& a, 
			 const FixedArray<3,T>& b ) {
  if ( a[1] < b[1] ) {
    return true;
  }
  else if ( a[1] == b[1] ) {
    if ( a[2] < b[2] ) {
      return true;
    }
    else if ( a[2] == b[2] ) {
      if ( a[0] < b[0] ) {
	return true;
      }
    }
  }
  return false;
}
  
template <typename T>
inline
bool 
zless_composite_compare( const FixedArray<3,T>& a, 
			 const FixedArray<3,T>& b ) {
  if ( a[2] < b[2] ) {
    return true;
  }
  else if ( a[2] == b[2] ) {
    if ( a[0] < b[0] ) {
      return true;
    }
    else if ( a[0] == b[0] ) {
      if ( a[1] < b[1] ) {
	return true;
      }
    }
  }
  return false;
}
  
template <typename T>
inline
bool 
xless_composite_compare( const FixedArray<3,T>* a, 
			 const FixedArray<3,T>* b ) 
{
  return xless_composite_compare( *a, *b );
}
  
template <typename T>
inline
bool 
yless_composite_compare( const FixedArray<3,T>* a, 
			 const FixedArray<3,T>* b ) 
{
  return yless_composite_compare( *a, *b );
}
  
template <typename T>
inline
bool 
zless_composite_compare( const FixedArray<3,T>* a, 
			 const FixedArray<3,T>* b ) 
{
  return zless_composite_compare( *a, *b );
}
  
template <typename T>
inline
bool 
less_composite_compare( int i, const FixedArray<3,T>& a, 
			const FixedArray<3,T>& b )
{
#ifdef DEBUG_composite_compare
  assert( 0 <= i && i < 3 );
#endif
  if ( i == 0 ) {
    return xless_composite_compare( a, b );
  }
  else if ( i == 1 ) {
    return yless_composite_compare( a, b );
  }
  return zless_composite_compare( a, b );
}

template <typename T>
inline
bool 
less_composite_compare( int i, const FixedArray<3,T>* a, 
			const FixedArray<3,T>* b )
{
#ifdef DEBUG_composite_compare
  assert( 0 <= i && i < 3 );
#endif
  if ( i == 0 ) {
    return xless_composite_compare( a, b );
  }
  else if ( i == 1 ) {
    return yless_composite_compare( a, b );
  }
  return zless_composite_compare( a, b );
}
//--------------------------------------------------------------------------
#endif

END_NAMESPACE_ADS

// End of file.
