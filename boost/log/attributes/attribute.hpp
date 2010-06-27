/*
 *          Copyright Andrey Semashev 2007 - 2010.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   attribute.hpp
 * \author Andrey Semashev
 * \date   15.04.2007
 *
 * The header contains attribute interface definition.
 */

#if (defined(_MSC_VER) && _MSC_VER > 1000)
#pragma once
#endif // _MSC_VER > 1000

#ifndef BOOST_LOG_ATTRIBUTE_HPP_INCLUDED_
#define BOOST_LOG_ATTRIBUTE_HPP_INCLUDED_

#include <boost/log/detail/prologue.hpp>

namespace boost {

namespace BOOST_LOG_NAMESPACE {

#ifndef BOOST_LOG_DOXYGEN_PASS
class attribute_value;
#endif // BOOST_LOG_DOXYGEN_PASS

/*!
 * \brief A base class for an attribute
 *
 * An attribute is basically a wrapper for some logic of values acquision. The sole purpose of
 * an attribute is to return an actual value when requested. A simpliest attribute
 * can always return the same value that it stores internally, but more complex species may
 * perform a considirable amount of work to return a value, and their values may differ.
 *
 * A word about thread safety. An attribute should be prepared to be requested a value from
 * multiple threads concurrently.
 */
struct BOOST_LOG_NO_VTABLE attribute
{
    /*!
     * Destructor. Destroys the attribute.
     */
    virtual ~attribute() {}

    /*!
     * \return The actual attribute value. It shall not return empty values (exceptions
     *         shall be used to indicate errors).
     */
    virtual attribute_value get_value() = 0;
};

} // namespace log

} // namespace boost

#endif // BOOST_LOG_ATTRIBUTE_HPP_INCLUDED_
