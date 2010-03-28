/*
 * (C) 2007 Andrey Semashev
 *
 * Use, modification and distribution is subject to the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 * This header is the Boost.Log library implementation, see the library documentation
 * at http://www.boost.org/libs/log/doc/log.html.
 */
/*!
 * \file   to_syslog.hpp
 * \author Andrey Semashev
 * \date   14.03.2009
 *
 * The header contains implementation of convenience functions for enabling logging to syslog.
 */

#if (defined(_MSC_VER) && _MSC_VER > 1000)
#pragma once
#endif // _MSC_VER > 1000

#ifndef BOOST_LOG_UTILITY_INIT_TO_SYSLOG_HPP_INCLUDED_
#define BOOST_LOG_UTILITY_INIT_TO_SYSLOG_HPP_INCLUDED_

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <boost/preprocessor/comparison/greater.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_shifted_params.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/log/detail/prologue.hpp>
#include <boost/log/detail/sink_init_helpers.hpp>
#include <boost/log/detail/parameter_tools.hpp>
#include <boost/log/core/core.hpp>
#ifndef BOOST_LOG_NO_THREADS
#include <boost/log/sinks/sync_frontend.hpp>
#else
#include <boost/log/sinks/unlocked_frontend.hpp>
#endif
#include <boost/log/sinks/syslog_backend.hpp>

#include <boost/log/keywords/custom_severity_mapping.hpp>

//! \cond
#ifndef BOOST_LOG_NO_THREADS
#define BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL sinks::synchronous_sink
#else
#define BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL sinks::unlocked_sink
#endif
//! \endcond

using namespace std;

namespace boost {

namespace BOOST_LOG_NAMESPACE {

namespace aux {

template< typename CustomEnumT >
inline typename sinks::syslog::custom_severity_mapping< CustomEnumT > acquire_custom_severity_mapping( sinks::syslog::custom_severity_mapping< CustomEnumT > const& mapping )
 {
    return mapping;
}

// The function installs the custom severity mapper into the syslog backend, if provided in the arguments pack
template< typename BackendT, typename ArgsT >
inline void setup_custom_severity_mapping(BackendT&, ArgsT const&, mpl::true_)
{
}

template< typename BackendT, typename ArgsT >
inline void setup_custom_severity_mapping(BackendT& b, ArgsT const& args, mpl::false_)
{
    b.set_severity_mapper(aux::acquire_custom_severity_mapping(args[keywords::custom_severity_mapping]));
}

//! The function constructs the sink and adds it to the core
template< typename CharT, typename ArgsT >
    shared_ptr< BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL<
        sinks::basic_syslog_backend< CharT >
    >
> init_log_to_syslog( ArgsT const& args )
{
    typedef sinks::basic_syslog_backend< CharT > backend_t;
    shared_ptr< backend_t > pBackend = boost::make_shared< backend_t >(args);

    aux::setup_formatter(*pBackend, args,
        typename is_void< typename parameter::binding< ArgsT, keywords::tag::format, void >::type >::type());

    aux::setup_custom_severity_mapping(*pBackend, args,
        typename is_void< typename parameter::binding< ArgsT, keywords::tag::custom_severity_mapping, void >::type >::type());

    shared_ptr< BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL< backend_t > > pSink =
        boost::make_shared< BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL< backend_t > >(pBackend);

    aux::setup_filter(*pSink, args,
        typename is_void< typename parameter::binding< ArgsT, keywords::tag::filter, void >::type >::type());

    basic_core< CharT >::get()->add_sink( pSink );

    return pSink;
}

} // namespace aux

#ifndef BOOST_LOG_DOXYGEN_PASS

#define BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL(z, n, data)\
    template< typename CharT, BOOST_PP_ENUM_PARAMS(n, typename T) >\
    inline shared_ptr<\
    	BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL<\
    		sinks::basic_syslog_backend< CharT >\
    	>\
    > init_log_to_syslog(BOOST_PP_ENUM_BINARY_PARAMS(n, T, const& arg))\
    {\
        return aux::init_log_to_syslog< CharT >((\
            arg0\
            BOOST_PP_COMMA_IF(BOOST_PP_GREATER(n, 1))\
            BOOST_PP_ENUM_SHIFTED_PARAMS(n, arg)\
        ));\
    }

BOOST_PP_REPEAT_FROM_TO(1, BOOST_LOG_MAX_PARAMETER_ARGS, BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL, ~)

#undef BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL

#if defined(BOOST_LOG_USE_CHAR)

#define BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL(z, n, data)\
    template< BOOST_PP_ENUM_PARAMS(n, typename T) >\
    inline shared_ptr<\
        BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL<\
            sinks::syslog_backend\
        >\
    > init_log_to_syslog(BOOST_PP_ENUM_BINARY_PARAMS(n, T, const& arg))\
    {\
        return init_log_to_syslog< char >(BOOST_PP_ENUM_PARAMS(n, arg));\
    }

BOOST_PP_REPEAT_FROM_TO(1, BOOST_LOG_MAX_PARAMETER_ARGS, BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL, ~)

#undef BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL

#endif // defined(BOOST_LOG_USE_CHAR)

#if defined(BOOST_LOG_USE_WCHAR_T)

#define BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL(z, n, data)\
    template< BOOST_PP_ENUM_PARAMS(n, typename T) >\
    inline shared_ptr<\
        BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL<\
            sinks::wsyslog_backend\
        >\
    > winit_log_to_syslog(BOOST_PP_ENUM_BINARY_PARAMS(n, T, const& arg))\
    {\
        return init_log_to_syslog< wchar_t >(BOOST_PP_ENUM_PARAMS(n, arg));\
    }

BOOST_PP_REPEAT_FROM_TO(1, BOOST_LOG_MAX_PARAMETER_ARGS, BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL, ~)

#undef BOOST_LOG_INIT_LOG_TO_SYSLOG_INTERNAL

#endif // defined(BOOST_LOG_USE_WCHAR_T)

#else // BOOST_LOG_DOXYGEN_PASS

/*!
 * The function initializes the logging library to write logs to Unix syslog.
 *
 * \param args A number of named arguments. The following parameters are supported:
 *             \li \c use_impl Specify native for openlog/syslog API calls or udp_socket_based for the internal version.
 *             \li \c facility The syslog facility to log to
 *             \li \c custom mapping of an enum used for severity to the syslog severities (which are fix)
 *             \li \c filter Specifies a filter to install into the sink. May be a string that represents a filter,
 *                           or a filter lambda expression.
 *             \li \c format Specifies a formatter to install into the sink. May be a string that represents a formatter,
 *                           or a formatter lambda expression (either streaming or Boost.Format-like notation).
 * \return Pointer to the constructed sink.
 */
template< typename CharT, typename... ArgsT >
shared_ptr<
    BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL<
        sinks::basic_syslog_backend< CharT >
    >
> init_log_to_syslog(ArgsT... const& args);

/*!
 * Equivalent to <tt>init_log_to_syslog< char >(args...);</tt>
 *
 * \overload
 */
template< typename... ArgsT >
shared_ptr<
    BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL<
        sinks::syslog_backend
    >
> init_log_to_syslog(ArgsT... const& args);

/*!
 * Equivalent to <tt>init_log_to_syslog< wchar_t >(args...);</tt>
 */
template< typename... ArgsT >
shared_ptr<
    BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL<
        sinks::wsyslog_backend
    >
> winit_log_to_syslog(ArgsT... const& args);

#endif // BOOST_LOG_DOXYGEN_PASS

} // namespace log

} // namespace boost

#undef BOOST_LOG_SYSLOG_SINK_FRONTEND_INTERNAL

#endif // BOOST_LOG_UTILITY_INIT_TO_SYSLOG_HPP_INCLUDED_
