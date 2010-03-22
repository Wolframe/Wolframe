/*
 * (C) 2009 Andrey Semashev
 *
 * Use, modification and distribution is subject to the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 * This header is the Boost.Log library implementation, see the library documentation
 * at http://www.boost.org/libs/log/doc/log.html.
 */
/*!
 * \file   filters/exception_policies.hpp
 * \author Andrey Semashev
 * \date   01.11.2009
 *
 * The header contains declaration of policies for exception throwing from filters
 */

#if (defined(_MSC_VER) && _MSC_VER > 1000)
#pragma once
#endif // _MSC_VER > 1000

#ifndef BOOST_LOG_FILTERS_EXCEPTION_POLICIES_HPP_INCLUDED_
#define BOOST_LOG_FILTERS_EXCEPTION_POLICIES_HPP_INCLUDED_

#include <boost/log/detail/prologue.hpp>
#include <boost/log/exceptions.hpp>

#include <boost/log/noreturn.hpp>

namespace boost {

namespace BOOST_LOG_NAMESPACE {

namespace filters {

/*!
 * \brief No-throw exception policy
 *
 * The policy will not throw exceptions on filtering errors
 */
struct no_throw_policy
{
    static void on_attribute_value_not_found(const char*, unsigned int)
    {
    }
};

/*!
 * \brief Throwing exception policy
 *
 * The policy will throw exceptions on filtering errors
 */
struct throw_policy
{
    static void on_attribute_value_not_found(const char* file, unsigned int line) NORETURN
    {
        missing_value::throw_(file, line, "Requested attribute value not found");
    }
};

} // namespace filters

} // namespace log

} // namespace boost

#endif // BOOST_LOG_FILTERS_EXCEPTION_POLICIES_HPP_INCLUDED_
