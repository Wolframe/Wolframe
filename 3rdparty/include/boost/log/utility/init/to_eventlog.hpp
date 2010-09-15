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
 * \file   to_eventlog.hpp
 * \author Andrey Semashev
 * \date   14.03.2009
 *
 * The header contains implementation of convenience functions for enabling logging to the Windows event logger.
 */

#if (defined(_MSC_VER) && _MSC_VER > 1000)
#pragma once
#endif // _MSC_VER > 1000

#ifndef BOOST_LOG_UTILITY_INIT_TO_EVENTLOG_HPP_INCLUDED_
#define BOOST_LOG_UTILITY_INIT_TO_EVENTLOG_HPP_INCLUDED_

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
#include <boost/log/sinks/event_log_backend.hpp>

#include <boost/log/keywords/custom_event_type_mapping.hpp>

//! \cond
#ifndef BOOST_LOG_NO_THREADS
#define BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL sinks::synchronous_sink
#else
#define BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL sinks::unlocked_sink
#endif
//! \endcond

using namespace std;

namespace boost {

namespace BOOST_LOG_NAMESPACE {

namespace aux {

template< typename CustomEnumT >
inline typename sinks::event_log::custom_event_type_mapping< CustomEnumT > acquire_custom_event_type_mapping( sinks::event_log::custom_event_type_mapping< CustomEnumT > const& mapping )
{
    return mapping;
}

// The function installs the custom severity mapper into the syslog backend, if provided in the arguments pack
template< typename SinkT, typename ArgsT >
inline void setup_custom_event_type_mapping(SinkT&, ArgsT const&, mpl::true_)
{
}

template< typename SinkT, typename ArgsT >
inline void setup_custom_event_type_mapping(SinkT& s UNUSED, ArgsT const& args UNUSED, mpl::false_)
{
	s.locked_backend( )->set_event_type_mapper(aux::acquire_custom_event_type_mapping(args[keywords::custom_event_type_mapping]));
}

//! The function constructs the sink and adds it to the core
template< typename CharT, typename ArgsT >
    shared_ptr< BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL<
        sinks::basic_simple_event_log_backend< CharT >
    >
> init_log_to_eventlog( ArgsT const& args )
{
    typedef sinks::basic_simple_event_log_backend< CharT > backend_t;
    shared_ptr< backend_t > pBackend = boost::make_shared< backend_t >(args);

    aux::setup_formatter(*pBackend, args,
        typename is_void< typename parameter::binding< ArgsT, keywords::tag::format, void >::type >::type());

    shared_ptr< BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL< backend_t > > pSink =
        boost::make_shared< BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL< backend_t > >(pBackend);

    aux::setup_custom_event_type_mapping(*pSink, args,
        typename is_void< typename parameter::binding< ArgsT, keywords::tag::custom_event_type_mapping, void >::type >::type());
		
    aux::setup_filter(*pSink, args,
        typename is_void< typename parameter::binding< ArgsT, keywords::tag::filter, void >::type >::type());

    basic_core< CharT >::get()->add_sink( pSink );

    return pSink;
}

} // namespace aux

#ifndef BOOST_LOG_DOXYGEN_PASS

#define BOOST_LOG_INIT_LOG_TO_EVENTLOG_INTERNAL(z, n, data)\
    template< typename CharT, BOOST_PP_ENUM_PARAMS(n, typename T) >\
    inline shared_ptr<\
    	BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL<\
    		sinks::basic_simple_event_log_backend< CharT >\
    	>\
    > init_log_to_eventlog(BOOST_PP_ENUM_BINARY_PARAMS(n, T, const& arg))\
    {\
        return aux::init_log_to_eventlog< CharT >((\
            arg0\
            BOOST_PP_COMMA_IF(BOOST_PP_GREATER(n, 1))\
            BOOST_PP_ENUM_SHIFTED_PARAMS(n, arg)\
        ));\
    }

BOOST_PP_REPEAT_FROM_TO(1, BOOST_LOG_MAX_PARAMETER_ARGS, BOOST_LOG_INIT_LOG_TO_EVENTLOG_INTERNAL, ~)

#undef BOOST_LOG_INIT_LOG_TO_EVENTLOG_INTERNAL

#if defined(BOOST_LOG_USE_CHAR)

#define BOOST_LOG_INIT_LOG_TO_EVENTLOG_INTERNAL(z, n, data)\
    template< BOOST_PP_ENUM_PARAMS(n, typename T) >\
    inline shared_ptr<\
        BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL<\
            sinks::simple_event_log_backend\
        >\
    > init_log_to_eventlog(BOOST_PP_ENUM_BINARY_PARAMS(n, T, const& arg))\
    {\
        return init_log_to_eventlog< char >(BOOST_PP_ENUM_PARAMS(n, arg));\
    }

BOOST_PP_REPEAT_FROM_TO(1, BOOST_LOG_MAX_PARAMETER_ARGS, BOOST_LOG_INIT_LOG_TO_EVENTLOG_INTERNAL, ~)

#undef BOOST_LOG_INIT_LOG_TO_EVENTLOG_INTERNAL

#endif // defined(BOOST_LOG_USE_CHAR)

#if defined(BOOST_LOG_USE_WCHAR_T)

#define BOOST_LOG_INIT_LOG_TO_EVENTLOG_INTERNAL(z, n, data)\
    template< BOOST_PP_ENUM_PARAMS(n, typename T) >\
    inline shared_ptr<\
        BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL<\
            sinks::wsimple_event_log_backend\
        >\
    > winit_log_to_eventlog(BOOST_PP_ENUM_BINARY_PARAMS(n, T, const& arg))\
    {\
        return init_log_to_eventlog< wchar_t >(BOOST_PP_ENUM_PARAMS(n, arg));\
    }

BOOST_PP_REPEAT_FROM_TO(1, BOOST_LOG_MAX_PARAMETER_ARGS, BOOST_LOG_INIT_LOG_TO_EVENTLOG_INTERNAL, ~)

#undef BOOST_LOG_INIT_LOG_TO_EVENTLOG_INTERNAL

#endif // defined(BOOST_LOG_USE_WCHAR_T)

#else // BOOST_LOG_DOXYGEN_PASS

/*!
 * The function initializes the logging library to write logs to the Windows Event Logger.
 *
 * \param args A number of named arguments. The following parameters are supported:
 *             \li \c log_source The Event Logger source to log to
 *             \li \c filter Specifies a filter to install into the sink. May be a string that represents a filter,
 *                           or a filter lambda expression.
 *             \li \c format Specifies a formatter to install into the sink. May be a string that represents a formatter,
 *                           or a formatter lambda expression (either streaming or Boost.Format-like notation).
 * \return Pointer to the constructed sink.
 */
template< typename CharT, typename... ArgsT >
shared_ptr<
    BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL<
        sinks::basic_eventlog_backend< CharT >
    >
> init_log_to_eventlog(ArgsT... const& args);

/*!
 * Equivalent to <tt>init_log_to_eventlog< char >(args...);</tt>
 *
 * \overload
 */
template< typename... ArgsT >
shared_ptr<
    BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL<
        sinks::simple_event_log_backend
    >
> init_log_to_eventlog(ArgsT... const& args);

/*!
 * Equivalent to <tt>init_log_to_eventlog< wchar_t >(args...);</tt>
 */
template< typename... ArgsT >
shared_ptr<
    BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL<
        sinks::wsimple_event_log_backend
    >
> winit_log_to_eventlog(ArgsT... const& args);

#endif // BOOST_LOG_DOXYGEN_PASS

} // namespace log

} // namespace boost

#undef BOOST_LOG_EVENTLOG_SINK_FRONTEND_INTERNAL

#endif // BOOST_LOG_UTILITY_INIT_TO_EVENTLOG_HPP_INCLUDED_
