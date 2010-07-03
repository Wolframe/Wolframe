/*
 *          Copyright Andrey Semashev 2007 - 2010.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   attribute_value.hpp
 * \author Andrey Semashev
 * \date   21.05.2010
 *
 * The header contains methods of the \c attribute_value class. Use this header
 * to introduce the complete \c attribute_value implementation into your code.
 */

#if (defined(_MSC_VER) && _MSC_VER > 1000)
#pragma once
#endif // _MSC_VER > 1000

#ifndef BOOST_LOG_ATTRIBUTE_VALUE_HPP_INCLUDED_
#define BOOST_LOG_ATTRIBUTE_VALUE_HPP_INCLUDED_

#include <boost/log/detail/prologue.hpp>
#include <boost/log/detail/functional.hpp>
#include <boost/log/attributes/attribute_value_def.hpp>
#include <boost/log/utility/type_dispatch/static_type_dispatcher.hpp>

namespace boost {

namespace BOOST_LOG_NAMESPACE {

template< typename T >
inline optional< T > attribute_value::extract() const
{
    optional< T > res;
    boost::log::aux::assign_fun< optional< T > > receiver(res);
    boost::log::aux::single_type_dispatcher< T > disp(receiver);
    this->dispatch(disp);
    return res;
}

template< typename T, typename ReceiverT >
inline bool attribute_value::extract(ReceiverT receiver) const
{
    static_type_dispatcher< T > disp(receiver);
    return this->dispatch(disp);
}

} // namespace log

} // namespace boost

#endif // BOOST_LOG_ATTRIBUTE_VALUE_HPP_INCLUDED_
