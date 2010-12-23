//  Boost math/big_radix_whole.hpp header file  ------------------------------//

//  Copyright 2006 Daryle Walker.  Distributed under the Boost Software License,
//  Version 1.0.  (See the accompanying file LICENSE_1_0.txt or a copy at
//  <http://www.boost.org/LICENSE_1_0.txt>.)

//  See <http://www.boost.org/libs/math/> for the library's home page.

/** \file
    \brief I/O operators for arbitrary-length specific-radix unsigned integers

    Contains the I/O operators for \c big_radix_whole\<\> objects.  These
    operators are not in big_radix_whole_core.hpp to allow the option of using
    \c big_radix_whole\<\> without dragging in an I/O context.  But that header
    is \#included here so this header can be the master header for the
    \c big_radix_whole\<\> class template.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_HPP
#define BOOST_MATH_BIG_RADIX_WHOLE_HPP

#include <boost/math/big_radix_whole_core.hpp>  // for big_radix_whole

#include <istream>  // for std::basic_istream
#include <ostream>  // for std::basic_ostream


namespace boost
{
namespace math
{


//  Forward declarations  ----------------------------------------------------//

// I/O operators for arbitrary-length specific-radix unsigned integers
template < typename Ch, class Tr, int Rx, class Al >
    ::std::basic_istream<Ch, Tr> &
    operator >>( ::std::basic_istream<Ch, Tr> &i, big_radix_whole<Rx, Al> &n );

template < typename Ch, class Tr, int Rx, class Al >
    ::std::basic_ostream<Ch, Tr> &
    operator <<( ::std::basic_ostream<Ch, Tr> &o,
     big_radix_whole<Rx, Al> const &n );


}  // namespace math
}  // namespace boost


#include <algorithm>  // for std::fill_n, std::max, std::copy
#include <ios>        // for std::basic_ios, std::ios_base, std::streamsize
#include <iterator>   // for std::istreambuf_iterator, std::ostreambuf_iterator
#include <locale>     // for std::num_get, std::use_facet
#include <new>        // for std::bad_alloc
#include <sstream>    // for std::basic_ostringstream
#include <string>     // for std::basic_string

#include <boost/io/ios_state.hpp>  // for boost::io::ios_flags_saver


//! \cond
//  Implementation details  --------------------------------------------------//

namespace boost
{
namespace math
{
namespace detail
{

// Properly handle error flags and exceptions
// (Must only be called during exception handling)
// (From Langer's and Kreft's "Standard C++ I/O Streams and Locales" book)
template < typename Ch, class Tr >
void
stream_exception_handler
(
    ::std::basic_ios<Ch, Tr> &  stream,
    ::std::ios_base::iostate &  errors
)
{
    using ::std::ios_base;

    ios_base::iostate const  mask = stream.exceptions();

    try
    {
        throw;
    }
    catch ( ::std::bad_alloc & )
    {
        errors |= ios_base::badbit;

        if (ios_base::failbit == (mask&(ios_base::failbit | ios_base::badbit)))
        {
            stream.setstate( errors );
        }
        else if ( mask & ios_base::badbit )
        {
            try
            {
                stream.setstate( errors );
            }
            catch ( ios_base::failure & )
            {
                // Let this sub-exception be dropped....
            }

            throw;
        }
    }
    catch ( ... )
    {
        errors |= ios_base::failbit;

        if ( (mask & ios_base::badbit) && (errors & ios_base::badbit) )
        {
            stream.setstate( errors );
        }
        else if ( mask & ios_base::failbit )
        {
            try
            {
                stream.setstate( errors );
            }
            catch ( ios_base::failure & )
            {
                // Let this sub-exception be dropped....
            }

            throw;
        }
    }
}

// Base class for the scanners/printers
template < class Derived >
class big_radix_whole_io_actor_base
{
public:
    template < typename Ch, class Tr, int Rx, class Al >
    void  operator ()
    (
        ::std::basic_istream<Ch, Tr> &  i,
        big_radix_whole<Rx, Al> &       n
    ) const
    {
        using ::std::ios_base;

        if ( !i )  return;

        ios_base::iostate  errors = ios_base::goodbit;

        try
        {
            typename ::std::basic_istream<Ch, Tr>::sentry  s( i );

            if ( s )
            {
                errors = this->do_scan( i, n );
            }
        }
        catch ( ... )
        {
            stream_exception_handler( i, errors );
        }

        i.setstate( errors );
    }

    template < typename Ch, class Tr, int Rx, class Al >
    void  operator ()
    (
        ::std::basic_ostream<Ch, Tr> &   o,
        big_radix_whole<Rx, Al> const &  n
    ) const
    {
        using ::std::ios_base;

        if ( !o )  return;

        ios_base::iostate  errors = ios_base::goodbit;

        try
        {
            typename ::std::basic_ostream<Ch, Tr>::sentry  s( o );

            if ( s )
            {
                errors = this->do_print( o, n );
                o.width( 0 );
            }
        }
        catch ( ... )
        {
            stream_exception_handler( o, errors );
        }

        o.setstate( errors );
    }

private:
    template < typename Ch, class Tr, int Rx, class Al >
    ::std::ios_base::iostate  do_scan
    (
        ::std::basic_istream<Ch, Tr> &  i,
        big_radix_whole<Rx, Al> &       n
    ) const
    {
        using ::std::ios_base;

        // Check for leading sign
        typedef ::std::istreambuf_iterator<Ch, Tr>  iterator;

        iterator const  e;
        iterator        ii( i );
        bool            is_negative = false, had_sign = false;

        if ( e != ii )
        {
            switch ( i.narrow(*ii, '\0') )
            {
            case '-':
                is_negative = true;
                ++ii;
                had_sign = true;
                break;

            case '+':
                is_negative = false;
                ++ii;
                had_sign = true;
                break;

            default:
                // No change
                break;
            }
        }

        // Read each digit (making sure to read at least one)
        big_radix_whole<Rx, Al>  scratch( 0u, n.get_allocator() );
        ios_base::iostate        s = ios_base::goodbit;

        if ( static_cast<Derived const *>(this)->do_digit_scan_loop(ii, e, i, s,
         scratch) )
        {
            if ( is_negative && scratch )
            {
                // Cannot create a negative value
                s |= ios_base::failbit;
            }

            // Make sure the read fully succeeded before loading
            if ( !(s & ( ios_base::failbit | ios_base::badbit )) )
            {
                n.swap( scratch );
            }
        }
        else
        {
            if ( had_sign )
            {
                // Undo the unneeded sign extraction
                i.unget();
            }

            s |= ios_base::failbit;
        }

        return s;
    }

    template < typename Ch, class Tr, int Rx, class Al >
    ::std::ios_base::iostate  do_print
    (
        ::std::basic_ostream<Ch, Tr> &   o,
        big_radix_whole<Rx, Al> const &  n
    ) const
    {
        using ::std::ios_base;

        ::std::ostreambuf_iterator<Ch, Tr>  oi( o );

        // Write each digit, return the status
        return static_cast<Derived const *>( this )->do_digit_print_loop( oi, o,
         n ) ? ios_base::goodbit : ios_base::badbit;
    }

};  // boost::math::detail::big_radix_whole_io_actor_base

// Class for the scanners/printers, based on using hyphen separation
template < bool Hyphens = true >
class big_radix_whole_io_actor
    : public big_radix_whole_io_actor_base< big_radix_whole_io_actor<Hyphens> >
{
    friend class big_radix_whole_io_actor_base< big_radix_whole_io_actor >;

private:
    template < typename Ch, class Tr, int Rx, class Al >
    bool  do_digit_scan_loop
    (
        ::std::istreambuf_iterator<Ch, Tr> &  begin,
        ::std::istreambuf_iterator<Ch, Tr>    end,
        ::std::basic_istream<Ch, Tr> &        stream,
        ::std::ios_base::iostate &            errors,
        big_radix_whole<Rx, Al> &             scratch
    ) const
    {
        using ::std::ios_base;

        typedef ::std::num_get<Ch, ::std::istreambuf_iterator<Ch, Tr> >
         facet_type;

        bool                had_digits = false;
        facet_type const &  f = ::std::use_facet<facet_type>( stream.getloc() );

        // Ensure independent format
        ::boost::io::ios_flags_saver  r( stream );

        stream.setf( ios_base::dec, ios_base::basefield );

        // Loop through each (potential) digit
        while ( end != begin )
        {
            // Confirm the separating hyphen between each digit
            if ( had_digits )
            {
                if ( '-' == stream.narrow(*begin, '\0') )
                {
                    // Skip that hyphen
                    ++begin;
                }
                else
                {
                    // Reached non-number character, stop reading the number
                    break;
                }
            }

            // Read the first/next digit
            long  digit = -1;

            begin = f.get( begin, end, stream, errors, digit );

            // Confirm and store that digit
            if ( !(( ios_base::failbit | ios_base::badbit ) & errors) )
            {
                if ( 0 <= digit && digit < Rx )
                {
                    had_digits = true;
                    scratch.shift_up_add_single( static_cast<int>(digit) );

                    continue;
                }
                else
                {
                    // Digit out of range
                    errors |= ios_base::failbit;
                }
            }

            // Stop reading if something bad happens
            break;
        }

        return had_digits;
    }

    template < typename Ch, class Tr, int Rx, class Al >
    bool  do_digit_print_loop
    (
        ::std::ostreambuf_iterator<Ch, Tr> &  out,
        ::std::basic_ostream<Ch, Tr> &        stream,
        big_radix_whole<Rx, Al> const &       value
    ) const
    {
        using ::std::use_facet;
        using ::std::ios_base;
        using ::std::fill_n;

        typedef ::std::ostreambuf_iterator<Ch, Tr>           iterator;
        typedef ::std::num_put<Ch, iterator>                 facet_type;
        typedef typename big_radix_whole<Rx, Al>::size_type  size_type;

        switch ( size_type const  s = value.digit_count() )
        {
        default:
        {
            // Prepare a temporary string to hold digit list text
            // (It'll always use the default independent format.)
            ::std::basic_ostringstream<Ch, Tr, typename Al::template
             rebind<Ch>::other>  oss;

            oss.imbue( stream.getloc() );

            facet_type const &  f = use_facet<facet_type>( oss.getloc() );
            iterator            ossi( oss );

            // Loop through each digit
            for ( size_type  i = s ; (0u < i) && !ossi.failed() ; --i )
            {
                ossi = f.put( ossi, oss, oss.fill(),
                 static_cast<long>(value.quick_digit_at( i - 1u )) );

                if ( !ossi.failed() && (1u < i) )
                {
                    *ossi++ = oss.widen( '-' );
                }
            }

            // Transfer the completed string to the real stream
            if ( !ossi.failed() )
            {
                ::std::basic_string<Ch, Tr, typename Al::template
                 rebind<Ch>::other> const  text = oss.str();

                size_type const  w( stream.width() ), tl( text.length() );
                bool const       put_left = (stream.flags() &
                 ios_base::adjustfield) == ios_base::left;

                if ( !out.failed() && !put_left && (w > tl) )
                {
                    out = fill_n( out, w - tl, stream.fill() );
                }

                if ( !out.failed() )
                {
                    out = ::std::copy( text.begin(), text.end(), out );
                }

                if ( !out.failed() && put_left && (w > tl) )
                {
                    out = fill_n( out, w - tl, stream.fill() );
                }

                return !out.failed();
            }
            else
            {
                return false;
            }
        }

        case 1u:
        case 0u:
        {
            // Print the only digit as the full number, in independent format
            ::boost::io::ios_flags_saver  r( stream );

            stream.setf( ios_base::dec, ios_base::basefield );
            stream.unsetf( ios_base::showpos | ios_base::showbase );

            out = use_facet<facet_type>( stream.getloc() ).put( out, stream,
             stream.fill(), static_cast<long>(value.digit_at( 0u )) );

            return !out.failed();
        }
        }
    }

};  // boost::math::detail::big_radix_whole_io_actor (1)

// Class for the scanners/printers, based on not using hyphen separation
template < >
class big_radix_whole_io_actor<false>
    : public big_radix_whole_io_actor_base< big_radix_whole_io_actor<false> >
{
    friend class big_radix_whole_io_actor_base< big_radix_whole_io_actor >;

private:
    template < typename Ch, class Tr, int Rx, class Al >
    bool  do_digit_scan_loop
    (
        ::std::istreambuf_iterator<Ch, Tr> &  begin,
        ::std::istreambuf_iterator<Ch, Tr>    end,
        ::std::basic_istream<Ch, Tr> &        stream,
        ::std::ios_base::iostate &                  ,  // unused "errors"
        big_radix_whole<Rx, Al> &             scratch
    ) const
    {
        bool  had_digits = false;

        for ( ; end != begin ; ++begin )
        {
            char const  c = stream.narrow( *begin, '\0' );

            if ( '0' <= c && c <= '9' )
            {
                int const  digit = c - '0';

                if ( digit < Rx )
                {
                    had_digits = true;
                    scratch.shift_up_add_single( digit );
                    continue;
                }
            }

            // Bad character (oversized or non-digit) stops reading
            break;
        }

        return had_digits;
    }

    template < typename Ch, class Tr, int Rx, class Al >
    bool  do_digit_print_loop
    (
        ::std::ostreambuf_iterator<Ch, Tr> &  out,
        ::std::basic_ostream<Ch, Tr> &        stream,
        big_radix_whole<Rx, Al> const &       value
    ) const
    {
        using ::std::ios_base;
        using ::std::fill_n;

        // Print the minimal amount of digits, but always at least one
        typedef typename big_radix_whole<Rx, Al>::size_type  size_type;

        size_type const  s = ::std::max<size_type>( value.digit_count(), 1u );

        // Loop through each digit
        size_type const  w = static_cast<size_type>( stream.width() );
        bool const       put_left = (stream.flags() & ios_base::adjustfield) ==
         ios_base::left;

        if ( !out.failed() && !put_left && (w > s) )
        {
            out = fill_n( out, w - s, stream.fill() );
        }

        for ( size_type  i = s ; (0u < i) && !out.failed() ; --i )
        {
            *out++ = stream.widen( '0' + value.digit_at(i - 1u) );
        }

        if ( !out.failed() && put_left && (w > s) )
        {
            out = fill_n( out, w - s, stream.fill() );
        }

        // Use the stream-buffer-iterator's status
        return !out.failed();
    }

};  // boost::math::detail::big_radix_whole_io_actor (2)

}  // namespace detail
}  // namespace math
}  // namespace boost
//! \endcond


//  I/O function definitions for big_radix_whole  ----------------------------//

/** \brief  Reads a \c big_radix_whole\<\> from an input stream

    Receives a \c big_radix_whole\<\> object from an input stream.  The format
    is a string of digits.  For radices above 10, each digit is separated by a
    hyphen and is written in decimal notation.

    \param i  The input stream to perform the reading.
    \param n  The \c big_radix_whole\<\> object to store the result.

    \return  \p i
 */
template < typename Ch, class Tr, int Rx, class Al >
std::basic_istream<Ch, Tr> &
boost::math::operator >>
(
    std::basic_istream<Ch, Tr> &            i,
    boost::math::big_radix_whole<Rx, Al> &  n
)
{
    detail::big_radix_whole_io_actor<(Rx > 10)>()( i, n );
    return i;
}

/** \brief  Writes a \c big_radix_whole\<\> to an output stream

    Sends a \c big_radix_whole\<\> object to an output stream.  The format will
    be the list of digits, each given by their decimal expansion, separated by
    hyphens if the radix is greater than 10, from the highest non-zero digit to
    the ones digit.  (A single zero is written for zero values.)

    \param o  The output stream to perform the writing.
    \param n  The \c big_radix_whole\<\> object to be written.

    \return  \p o
 */
template < typename Ch, class Tr, int Rx, class Al >
inline
std::basic_ostream<Ch, Tr> &
boost::math::operator <<
(
    std::basic_ostream<Ch, Tr> &                  o,
    boost::math::big_radix_whole<Rx, Al> const &  n
)
{
    detail::big_radix_whole_io_actor<(Rx > 10)>()( o, n );
    return o;
}


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_HPP
