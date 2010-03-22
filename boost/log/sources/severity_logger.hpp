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
 * \file   severity_logger.hpp
 * \author Andrey Semashev
 * \date   08.03.2007
 *
 * The header contains implementation of a logger with severity level support.
 */

#if (defined(_MSC_VER) && _MSC_VER > 1000)
#pragma once
#endif // _MSC_VER > 1000

#ifndef BOOST_LOG_SOURCES_SEVERITY_LOGGER_HPP_INCLUDED_
#define BOOST_LOG_SOURCES_SEVERITY_LOGGER_HPP_INCLUDED_

#include <boost/log/detail/prologue.hpp>
#if !defined(BOOST_LOG_NO_THREADS)
#include <boost/log/detail/light_rw_mutex.hpp>
#endif // !defined(BOOST_LOG_NO_THREADS)
#include <boost/log/sources/features.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/threading_models.hpp>
#include <boost/log/sources/severity_feature.hpp>

namespace boost {

namespace BOOST_LOG_NAMESPACE {

namespace sources {

#ifndef BOOST_LOG_DOXYGEN_PASS

#ifdef BOOST_LOG_USE_CHAR

//! Narrow-char logger with severity level support
template< typename LevelT = int >
class severity_logger :
    public basic_composite_logger<
        char,
        severity_logger< LevelT >,
        single_thread_model,
        typename features< severity< LevelT > >::type
    >
{
    BOOST_LOG_FORWARD_LOGGER_CONSTRUCTORS_TEMPLATE(severity_logger)
};

#if !defined(BOOST_LOG_NO_THREADS)

//! Narrow-char thread-safe logger with severity level support
template< typename LevelT = int >
class severity_logger_mt :
    public basic_composite_logger<
        char,
        severity_logger_mt< LevelT >,
        multi_thread_model< boost::log::aux::light_rw_mutex >,
        typename features< severity< LevelT > >::type
    >
{
    BOOST_LOG_FORWARD_LOGGER_CONSTRUCTORS_TEMPLATE(severity_logger_mt)
};

#endif // !defined(BOOST_LOG_NO_THREADS)

#endif

#ifdef BOOST_LOG_USE_WCHAR_T

//! Wide-char logger with severity level support
template< typename LevelT = int >
class wseverity_logger :
    public basic_composite_logger<
        wchar_t,
        wseverity_logger< LevelT >,
        single_thread_model,
        typename features< severity< LevelT > >::type
    >
{
    BOOST_LOG_FORWARD_LOGGER_CONSTRUCTORS_TEMPLATE(wseverity_logger)
};

#if !defined(BOOST_LOG_NO_THREADS)

//! Wide-char thread-safe logger with severity level support
template< typename LevelT = int >
class wseverity_logger_mt :
    public basic_composite_logger<
        wchar_t,
        wseverity_logger_mt< LevelT >,
        multi_thread_model< boost::log::aux::light_rw_mutex >,
        typename features< severity< LevelT > >::type
    >
{
    BOOST_LOG_FORWARD_LOGGER_CONSTRUCTORS_TEMPLATE(wseverity_logger_mt)
};

#endif // !defined(BOOST_LOG_NO_THREADS)

#endif

#else // BOOST_LOG_DOXYGEN_PASS

/*!
 * \brief Narrow-char logger. Functionally equivalent to \c basic_severity_logger.
 *
 * See \c severity class template for a more detailed description
 */
template< typename LevelT = int >
class severity_logger :
    public basic_composite_logger<
        char,
        severity_logger< LevelT >,
        single_thread_model,
        features< severity< LevelT > >
    >
{
public:
    /*!
     * Default constructor
     */
    severity_logger();
    /*!
     * Copy constructor
     */
    severity_logger(severity_logger const& that);
    /*!
     * Constructor with named arguments
     */
    template< typename... ArgsT >
    explicit severity_logger(ArgsT... const& args);
    /*!
     * Assignment operator
     */
    severity_logger& operator= (severity_logger const& that)
    /*!
     * Swaps two loggers
     */
    void swap(severity_logger& that);
};

/*!
 * \brief Narrow-char thread-safe logger. Functionally equivalent to \c basic_severity_logger.
 *
 * See \c severity class template for a more detailed description
 */
template< typename LevelT = int >
class severity_logger_mt :
    public basic_composite_logger<
        char,
        severity_logger_mt< LevelT >,
        multi_thread_model< implementation_defined >,
        features< severity< LevelT > >
    >
{
public:
    /*!
     * Default constructor
     */
    severity_logger_mt();
    /*!
     * Copy constructor
     */
    severity_logger_mt(severity_logger_mt const& that);
    /*!
     * Constructor with named arguments
     */
    template< typename... ArgsT >
    explicit severity_logger_mt(ArgsT... const& args);
    /*!
     * Assignment operator
     */
    severity_logger_mt& operator= (severity_logger_mt const& that)
    /*!
     * Swaps two loggers
     */
    void swap(severity_logger_mt& that);
};

/*!
 * \brief Wide-char logger. Functionally equivalent to \c basic_severity_logger.
 *
 * See \c severity class template for a more detailed description
 */
template< typename LevelT = int >
class wseverity_logger :
    public basic_composite_logger<
        wchar_t,
        wseverity_logger< LevelT >,
        single_thread_model,
        features< severity< LevelT > >
    >
{
public:
    /*!
     * Default constructor
     */
    wseverity_logger();
    /*!
     * Copy constructor
     */
    wseverity_logger(wseverity_logger const& that);
    /*!
     * Constructor with named arguments
     */
    template< typename... ArgsT >
    explicit wseverity_logger(ArgsT... const& args);
    /*!
     * Assignment operator
     */
    wseverity_logger& operator= (wseverity_logger const& that)
    /*!
     * Swaps two loggers
     */
    void swap(wseverity_logger& that);
};

/*!
 * \brief Wide-char thread-safe logger. Functionally equivalent to \c basic_severity_logger.
 *
 * See \c severity class template for a more detailed description
 */
template< typename LevelT = int >
class wseverity_logger_mt :
    public basic_composite_logger<
        wchar_t,
        wseverity_logger_mt< LevelT >,
        multi_thread_model< implementation_defined >,
        features< severity< LevelT > >
    >
{
public:
    /*!
     * Default constructor
     */
    wseverity_logger_mt();
    /*!
     * Copy constructor
     */
    wseverity_logger_mt(wseverity_logger_mt const& that);
    /*!
     * Constructor with named arguments
     */
    template< typename... ArgsT >
    explicit wseverity_logger_mt(ArgsT... const& args);
    /*!
     * Assignment operator
     */
    wseverity_logger_mt& operator= (wseverity_logger_mt const& that)
    /*!
     * Swaps two loggers
     */
    void swap(wseverity_logger_mt& that);
};

#endif // BOOST_LOG_DOXYGEN_PASS

} // namespace sources

} // namespace log

} // namespace boost

#endif // BOOST_LOG_SOURCES_SEVERITY_LOGGER_HPP_INCLUDED_
